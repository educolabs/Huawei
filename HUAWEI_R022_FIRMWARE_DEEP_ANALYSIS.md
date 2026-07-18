# Huawei ONT R022 Firmware Deep Analysis

## Firmware Versions Analyzed
| Firmware | Model | Version | SoC | Type | Files |
|----------|-------|---------|-----|------|-------|
| EG8145V5-V500R022C00SPC340B019 | EG8145V5 | R022C00 | SD5116 (HiSilicon) | HGU GPON | 6233 |
| HN8145XR-V500R022C10SPC160 | HN8145XR | R022C10 | SD5116+FTTR | XGS-PON FTTR | 7149 |

---

## 1. SYSTEM ARCHITECTURE

### 1.1 Architecture
- **CPU**: ARM 32-bit (HiSilicon SD5116/SD5118)
- **Kernel**: Dopra Linux 5.10.0
- **Libc**: musl libc (static+dynamic)
- **Shell**: BusyBox v1.32.1 (ash, restricted)
- **CLI Daemon**: `clid` (ARM ELF 32-bit PIE, stripped, 199KB)
- **Web Server**: `web` binary (ARM ELF)
- **HN8145XR**: Additional OpenWrt-based rootfs (rootfs_5) with procd, netifd, ubus, LXC

### 1.2 Key Binaries
| Binary | Path | Size | Description |
|--------|------|------|-------------|
| `clid` | `/bin/clid` | 199KB | CLI daemon, handles WAP shell |
| `shellconfig` | `/bin/shellconfig` | - | Shell configuration manager |
| `sudo` | `/sbin/sudo` | 589KB | Sudo (compiled with --disable-authentication) |
| `cblc` | `/bin/cblc` | - | Config Backup/Load Controller, validates hardinfo files |
| `aescrypt2` | `/bin/aescrypt2` | - | AES encryption/decryption tool |
| `decrypt_boardinfo` | `/bin/decrypt_boardinfo` | - | Boardinfo decryption |
| `get_feature_spec` | `/sbin/get_feature_spec` | - | Feature/Spec query (aliased as GetFeature, GetSpec) |
| `ontchmod` | `/sbin/ontchmod` | - | ONT permission changer |
| `restorehwmode.sh` | `/bin/restorehwmode.sh` | 704 lines | Factory restore script |
| `customize.sh` | `/bin/customize.sh` | 28 lines | Carrier customization wrapper |
| `EquipMode.sh` | `/bin/EquipMode.sh` | 93 lines | Equipment test mode toggle |
| `busybox.nosuid` | `/bin/busybox.nosuid` | - | BusyBox without suid (restricted shell) |
| `busybox.suid` | `/sbin/busybox.suid` | - | BusyBox with suid (full shell) |

### 1.3 Key Libraries
| Library | Description |
|---------|-------------|
| `libhw_swm.so` | Software Management - handles hardinfo spec/feature files |
| `libhw_swm_dll.so` | SWM DLL - hardinfo file management, upgrade validation |
| `libhw_ssp_basic.so` | SSP Basic - `HW_Spec_GetValueByName`, `HW_Feature_IsSupportByFeatureName` |
| `libcfg_api.so` | Configuration API |
| `libdm_pdt.so` | Data Model PDT - hardinfo data model |
| `libhw_cli_dcom_transparent_core.so` | CLI DCOM transparent core |
| `libbbsp.so` | BBSP core library |

### 1.4 Critical File Paths
| Path | Description |
|------|-------------|
| `/mnt/jffs2/hw_hardinfo_spec` | Hardware spec (binary, integrity-checked by cblc) |
| `/mnt/jffs2/hw_hardinfo_feature` | Hardware feature (binary, integrity-checked by cblc) |
| `/mnt/jffs2/hw_boardinfo` | Board information (AES encrypted) |
| `/mnt/jffs2/hw_ctree.xml` | Current config tree (AES encrypted) |
| `/mnt/jffs2/hw_default_ctree.xml` | Default config tree |
| `/mnt/jffs2/customizepara.txt` | Customize parameters (possibly AES encrypted) |
| `/mnt/jffs2/Equip.sh` | Equipment mode flag file |
| `/mnt/jffs2/equiptestmode` | Equipment test mode flag |
| `/etc/wap/hw_cli.xml` | CLI command definitions (XML, encrypted in some models) |
| `/etc/wap/hw_shell_cli.xml` | Shell CLI definitions (binary/encrypted) |
| `/etc/wap/hw_diag_cli.xml` | Diagnostic CLI definitions (binary/encrypted) |
| `/etc/wap/customize/` | Carrier customization configs |
| `/etc/wap/spec/` | Spec configuration files (per-module) |
| `/etc/wap/ft/` | Feature configuration files (per-module) |
| `/etc/wap/group` | System group definitions |
| `/etc/wap/passwd` | System user definitions |
| `/etc/wap/DebugVersionFlag` | Debug version flag (presence = debug mode) |
| `/etc/wap/ALLDBGVersionFlag` | All-debug version flag |

---

## 2. TELNET SYSTEM

### 2.1 Telnet Access Flow
1. Connection via busybox telnetd
2. `clid` handles the session
3. `HW_CLI_CheckLoginUser` validates user credentials
4. `HW_CLI_CheckPwd` checks password (supports SHA256 if feature enabled)
5. `HW_CLI_CheckAftLogin` performs post-login checks
6. `HW_CLI_GetCliUserGroupInEquipMode` determines user group
7. `HW_CLI_SPEC_GetUserGroup` gets group from spec (`SSMP_SPEC_CLI_USERGRP`)
8. Command tree filtered based on `CmdGroup` permissions

### 2.2 Login Prompt
```
Welcome Visiting Huawei Home Gateway
Copyright by Huawei Technologies Co., Ltd.
Login:root
Password:
WAP>
```

### 2.3 SU Mode
```
WAP>su
success!
SU_WAP>
```
- SU mode grants access to commands with higher CmdGroup values
- Password required if `HW_SSMP_FEATURE_PWDCOMPLEX` is enabled

### 2.4 Shell Access
```
SU_WAP>shell
BusyBox v1.32.1 () built-in shell (ash)
WAP(Dopra Linux) #
```
- Uses `busybox.nosuid` (restricted, no cat/grep/find/sudo)
- Available commands: ls, ifconfig, iwconfig, iwpriv, killall, wl, wlconf
- Shell scripts: restorehwmode.sh, customize.sh, EquipMode.sh, etc.

### 2.5 Telnet Control Features
| Feature | Effect |
|---------|--------|
| `FT_SSMP_TELNET_LAN_WAN` | Controls telnet LAN/WAN access |
| `SSMP_SPEC_CLI_REMOTETELNET` | Enable remote telnet (1=enable) |
| `FEATURE_TELNET_IGNORE_TERMTYPE` | Terminal type handling |
| `CLI_IsAISCustomizeTelnet` | AIS customization check |

---

## 3. CLI COMMAND SYSTEM

### 3.1 Command XML Structure
Commands defined in `hw_cli.xml` with attributes:
- `CmdStr` - Command string
- `ObjPath` - Object path in data model
- `OpType` - Operation type
- `CmdGroup` - Permission bitmask

### 3.2 CmdGroup Permission Levels
| CmdGroup | Bits | Level | Commands | Description |
|----------|------|-------|----------|-------------|
| `""` | 0 | Unrestricted | ~20 | Factory/debug (set sn, set productmac via cmd_iODN) |
| `0x00000010` | 4 | Low | ~5 | Basic system (certs, flashlock, ploam) |
| `0x00002000` | 13 | Debug | ~15 | Packet debug (firewall, DHCP, DNS, PPPoE) |
| `0x00004000` | 14 | Operator | ~50 | Port config, PON stats, optic, PoE, USB |
| `0x00004010` | 14+4 | Service | ~337 | VoIP, OMCI, OAM, system mgmt, diagnostics |
| `0x10000000` | 28 | Plugin | ~3 | Plugin integrity check |
| `0x80000000` | 31 | **Admin** | **134** | WAN, NAT, firewall, DHCP, DNS, VoIP SIP, SN/password |
| `0x80000010` | 31+4 | Super | ~5 | WLAN enable/basic config |
| `0x80002000` | 31+13 | Debug+ | ~10 | VoIP signaling debug, connection trace, CWMP debug |
| `0x80004000` | 31+14 | **Expert** | ~25 | WiFi radio/SSID/MAC filter, FEM, save data |
| `0x80004010` | 31+14+4 | Max | ~5 | VSPA mgmt, user device display |

### 3.3 Total Commands: ~849 (EG8145V5), ~818 (HN8145XR)

### 3.4 Admin Commands (CmdGroup=0x80000000)
```
set sn                              # Set serial number
set password                        # Set CLI password
set productmac                      # Set product MAC
set mac                             # Set MAC address
set webuserpasswd                   # Set web user password
set loidpwd                         # Set LOID password
restore default configuration       # Factory reset
set tr069 info                      # Configure TR-069
load cfg / backup cfg               # Config backup/restore
wan                                 # WAN configuration
set pppwan                          # PPP WAN password
nat port mapping add/delete         # NAT/port forwarding
firewall rule add/delete/flush      # Firewall rules
ip route add/delete                 # Static routes
dnsserver add/delete static         # DNS configuration
```

### 3.5 Expert Commands (CmdGroup=0x80004000)
```
set wifi radio band {a/b}           # Enable/disable 2G/5G radio
set wifi expert                     # Advanced WiFi (bandwidth, DTIM, NSS, MCS, PMF, TXBF)
set ssid                            # Set SSID name/security/password
set wlan isolate                    # SSID isolation
save data                           # Save configuration to flash
display wlan stainfo                # Connected station details
```

### 3.6 Debug Commands (CmdGroup=0x80002000)
```
debugging voip signaling            # VoIP SIP debug
debugging connection trace          # Connection tracking debug
debugging cwmp server packet        # TR-069 packet debug
undo debugging all                  # Disable all debugging
```

---

## 4. USER/GROUP/PERMISSION SYSTEM

### 4.1 System Users (EG8145V5)
| User | UID:GID | Shell | Purpose |
|------|---------|-------|---------|
| `root` | 0:0 | `/sbin/nologin` | Root (locked) |
| `srv_clid` | 3030:2002 | `/bin/false` | CLI daemon |
| `srv_ssmp` | 3008:2002 | `/bin/false` | Device management |
| `srv_web` | 3004:2002 | `/bin/false` | Web interface |
| `cfg_cli` | 3010:2001 | `/bin/false` | CLI config |
| `srv_bbsp` | 3012:2002 | `/bin/false` | Broadband service |
| `srv_mu` | 3006:2002 | `/bin/false` | Memory utility |
| `srv_cwmp` | 3002:2002 | `/bin/false` | TR-069 |

### 4.2 Sudo Rules
```
srv_ssmp -> NOPASSWD: USB_MODULE, SSMP_CFG_CMD, SSMP_DUIT (/bin/restorehwmode.sh)
srv_clid -> NOPASSWD: RESTORE_CMD (/bin/hw_restore_manufactory_exec.sh), COLLECT_INFO
srv_mu   -> NOPASSWD: MU_MEMCLEAN_CMD
srv_bbsp -> NOPASSWD: BBSP_INSMOD, BBSP_RMMOD, BBSP_PARAMOD, BBSP_NFCT, BBSP_DPST
```

### 4.3 UserGroup Bitmask (SSMP_SPEC_CLI_USERGRP)
| Value | Access |
|-------|--------|
| `0x80000000` | Admin/SU (WAN, NAT, DHCP, firewall, backup/restore) |
| `0x80002000` | Admin debug (debugging voip, cwmp, upnp) |
| `0x80004000` | Admin advanced (wifi filter, mac filter, display wifi) |
| `0x80004010` | Admin service (display user device) |
| `0x00004000` | Advanced read (display/status commands) |
| `0x00000000` | All users (no restrictions) |

---

## 5. HARINFO SYSTEM

### 5.1 Architecture
- **Spec files**: `/mnt/jffs2/hw_hardinfo_spec` (binary, integrity-checked)
- **Feature files**: `/mnt/jffs2/hw_hardinfo_feature` (binary, integrity-checked)
- **Managed by**: `libhw_swm.so`, `libhw_swm_dll.so`
- **Validated by**: `cblc` binary using `SWM_SIG_CheckFileIsTrust`
- **Config source**: `/etc/wap/spec/` and `/etc/wap/ft/` directory hierarchy

### 5.2 Spec/Feature Hierarchy
```
base_{module}_spec.cfg  ->  spec.cfg (product)  ->  ISP customize spec
base_{module}_ft.cfg    ->  ft.cfg (product)    ->  ISP customize feature
```

### 5.3 The `set hardinfo value` Command
```
USAGE: set hardinfo value {spec1=val1;spec2=val2;&LANGUAGE=lang1,lang2}
```
- Values separated by semicolons (`;`)
- Language section starts with `&LANGUAGE=`
- Handled by `libhw_swm.so` and `libhw_swm_dll.so`
- Writes to `/mnt/jffs2/hw_hardinfo_spec` and `hw_hardinfo_feature`

### 5.4 The `display equip hardinfo` Command
Shows current spec/feature/language values:
```
SU_WAP>display equip hardinfo
SSMP_SPEC_WEB_FRAME=frame_XGPON;SSMP_SPEC_WEB_MENUXML=MenuXGPONAbroad.xml;
SSMP_SPEC_CLI_REMOTETELNET=1;SSMP_SPEC_CLI_REDLINEVERSION=1;
SSMP_SPEC_CLI_USERGRP=0x80004000;SSMP_SPEC_WEB_PWDENCRYPT=3;
&LANGUAGE=COMMON,DT_HUNGARY
```

---

## 6. WEB INTERFACE (/HTML or /web)

### 6.1 Frame Types
| Frame | Description |
|-------|-------------|
| `frame_XGPON` | XGS-PON full frame (most features) |
| `frame_huawei` | Standard Huawei frame |
| `frame_redhuawei` | Red Huawei branding |

### 6.2 Menu XML Files (90+ variants)
| Menu | Description |
|------|-------------|
| `MenuSmartAbroad.xml` | International smart gateway (default for abroad) |
| `MenuXGPONAbroad.xml` | XGPON international (full featured) |
| `MenuXGPONChina.xml` | XGPON China (485 lines, full admin+normal) |
| `MenuChina.xml` | Standard China |
| `MenuCmcc.xml` | China Mobile |
| `MenuUnicom.xml` | China Unicom |
| `MenuAbroad.xml` | Standard abroad |
| `MenuRemote.xml` | Remote management |
| `MenuE8c.xml` | China Telecom E8C |

### 6.3 MenuXGPONChina.xml Admin Sections
- Main Page, Smart Diagnose
- System Info: Device, WAN, Optic, Service, VoIP, Ethernet, Wireless, Home Net, Cloud, User Devices
- Advanced Config: WAN, LAN (L2/L3, DHCP, DHCPv6, Option82), PORT (ETH, CATV)
- Security: Firewall, DoS, IP/MAC filter, Parental Control, ACL, PSK Anti-brute
- Routing: IPv4/IPv6 static/dynamic/policy
- Forwarding: DMZ, port mapping/trigger
- Applications: SNTP, DLNA, DDNS, UPnP, IGMP, QoS, DNS, DSCP, VR, Certificates
- Wireless: basic/advance 2.4G+5G, schedule, Smart Coverage, Guest WiFi, RF mgmt, WAPI, Multi-AP, Access Control
- Voice: SIP/H.248
- System Management: TR069, Accounts, ONT Auth, NFC
- Maintenance: Upgrade, Config, Log, Speedtest, Mirror
- Bundle Management, IOS Experience

---

## 7. /etc/wap/customize DIRECTORY

### 7.1 Structure
```
/etc/wap/customize/
├── common/
│   ├── spec_common.cfg         # Base spec for international
│   ├── huawei_ft.cfg           # Base features for Huawei mode
│   ├── common_ft.cfg           # Common features (included by huawei_ft.cfg)
│   └── customize_relation.cfg  # Operator mapping (170+ entries)
├── china/
│   ├── spec_*.cfg              # Per-operator specs (100+ files)
│   ├── *_ft.cfg                # Per-operator features
│   ├── recover_*.sh            # Recovery scripts
│   ├── e8c_spec_*.cfg          # China Telecom provincial specs
│   └── customize_relation.cfg  # China operator mapping (179+ entries)
├── cmd_CMDC.xml                # CMDC CLI overrides
├── cmd_iODN.xml                # iODN CLI overrides
├── cmd_telialt.xml             # Telia CLI overrides
├── cpa.txt                     # OSGi security policy
├── cpa_cmcc.txt                # CMCC OSGi security policy
├── plugin_api/                 # Plugin API definitions
├── plugin_pre/                 # Forbidden pre-bundles
├── plugin_sys/                 # System bundles
└── plugin_objperm/             # Object permissions
```

### 7.2 spec_common.cfg (Default International)
```
SSMP_SPEC_WEB_FRAME = "frame_XGPON"
SSMP_SPEC_WEB_MENUXML = "MenuSmartAbroad.xml"
SSMP_SPEC_CLI_USERGRP = 0x00004000
SSMP_SPEC_CLI_REMOTETELNET = 1
SSMP_SPEC_CLI_REDLINEVERSION = 1
SSMP_SPEC_WEB_PWDENCRYPT = 3
BBSP_SPEC_RIP_RATE = 40
BBSP_SPEC_RIP_ROUTENUM = 128
SSMP_SPEC_PLUGIN_APILIST = "plugin_api"
OSGI_SPEC_SYSBUNDLE_LIST = "/etc/wap/customize/plugin_sys"
OSGI_SPEC_SECURITY_CFG = "/etc/wap/customize/cpa.txt"
OSGI_SPEC_FORBID_PREBUNDLE_LIST = "/etc/wap/customize/plugin_pre"
```

### 7.3 huawei_ft.cfg
```
feature.include="common_ft.cfg"
FT_CLI_SECURITY_ACCESS = 0    # DISABLED - no CLI password security
FT_TRANS_CONFG = 1            # Fast config swap enabled
```

### 7.4 customize_relation.cfg
Maps binword/cfgword to feature/spec/webconfig files for 170+ operator variants:
- `COMMON` -> Generic Huawei
- `E8C` -> China Telecom (30+ provinces)
- `CMCC` -> China Mobile
- `UNICOM/BJCU/GDCU/SDCU` -> China Unicom
- `V8XXC` -> FTTR variants

---

## 8. SHELL RESTRICTIONS ANALYSIS

### 8.1 Why Commands Are Blocked
The shell uses `busybox.nosuid` which is a restricted BusyBox build:
- **Available**: ls, ifconfig, iwconfig, iwpriv, killall, wl, wlconf, exit
- **Blocked**: cat, grep, find, exec, sh, sudo, redtor, echo
- **Permission denied**: restorehwmode.sh (needs root via su in CLI, not shell)

### 8.2 Why restorehwmode.sh Fails
```
WAP(Dopra Linux) # restorehwmode.sh
/bin/sh: restorehwmode.sh: Permission denied
```
- Script has `-r-xr-xr-x` permissions (executable but needs root context)
- Must be executed via sudo from CLI: `srv_ssmp` can run it via sudo NOPASSWD
- Or from SU_WAP> prompt with proper root access

### 8.3 Why EquipMode.sh Works
```
WAP(Dopra Linux) # equipMode.sh on
on
```
- Different permission model - checks user context differently
- Creates/removes `/mnt/jffs2/Equip.sh` flag file

### 8.4 Debug Version Flag
`/etc/wap/DebugVersionFlag` - When present:
- `restorehwmode.sh`: Logs execution timestamps (debug mode)
- `getcustominfo.sh`: **Skips equipment signature verification**
- `getcustomize.sh`: **Skips equipment signature verification**

---

## 9. HN8145XR DIFFERENCES

### 9.1 Additional Rootfs (rootfs_5)
- Full OpenWrt-based system with procd, netifd, ubus, LXC containers
- Has `opkg` package manager
- Includes `tcpdump`, `telnetd`, `dbus-daemon`, `lxc-*` tools
- More complete BusyBox with grep, find, etc.

### 9.2 FTTR-Specific Specs
- `BBSP_SPEC_FWD_SESSIONNUM` = 16000 (vs 8192 on EG8145V5)
- `BBSP_SPEC_IPV4_ROUTE_MAXNUM` = 400 (vs 32)
- `BBSP_SPEC_NAPT_ITEM_NUM` = 8192
- `BBSP_SPEC_POLICY_ROUTE_MAX` = 136
- `BBSP_SPEC_LAN_MAC_FILTER_NUM` = 32 (vs 8)

### 9.3 Additional Features
- `FT_DISABLE_UNSAFE_PROTOCOL` = 1
- `FT_SECURITY_ACCESS_WEB` = 1
- `FT_EAI_APP_SUPPORT` = 1
- `FT_FTTR_USE_ABOARD_GUIDEPAGE` = 1
- `FT_UNITE_LAN_USER` = 1

---

## 10. ENCRYPTION AND SECURITY

### 10.1 Config Encryption
- `aescrypt2` binary handles AES encryption/decryption
- `hw_ctree.xml` encrypted with key from `SPEC_OS_AES_CBC_APP_STR` = `"Df7!ui%s9(lmV1L8"`
- `hw_boardinfo` encrypted, decrypted by `decrypt_boardinfo`
- `customizepara.txt` possibly AES encrypted
- `encrypt_spec/` and `encrypt_spec_key/` contain encrypted spec keys (not standard tar.gz)

### 10.2 Password Security
- SHA256 hashing (feature-flagged: `HW_SSMP_FEATURE_WEB_SHA256`, `HW_SSMP_FEATURE_CLI_SHA256`)
- Min 8 chars (configurable via `SPEC_SAFE_PASSWORD_LENGTH`)
- Complexity enforcement (letters + numbers) via `HW_SSMP_FEATURE_PWDCOMPLEX`
- PBKDF2 salt length: 25 bytes (`SSMP_SPEC_PBKDF2_SALT_LEN`)
- Web password encryption levels: 0=none, 1=basic, 3=full (default)

### 10.3 Sudo Binary
- Compiled with `--disable-authentication` and `--without-pam`
- Standard sudo 1.9.x for ARM
- Rules defined per service user (see Section 4.2)

---

## 11. STATISTICS

| Category | EG8145V5 | HN8145XR |
|----------|----------|----------|
| Total files | 6233 | 7149 |
| CLI commands | ~849 | ~818 |
| SSMP specs | ~90 | ~90 |
| BBSP specs | ~80 | ~80 |
| WiFi specs | 18 | 18 |
| SSMP features | ~200 | ~200 |
| BBSP features | ~500 | ~500 |
| WiFi features | ~380 | ~380 |
| Customize files | 170+ ISP | 179+ ISP |
| Menu XMLs | 90+ | 16 |
| Web frames | 3 | 3 |
| System users | 22 | 27 |
| System groups | 16 | 34 |
