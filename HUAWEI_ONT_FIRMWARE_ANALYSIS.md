# Huawei ONT Firmware Analysis - Complete Documentation

## Firmware Versions Analyzed
| Firmware | Model | Version | Type |
|----------|-------|---------|------|
| EG8145V5-V500R022C00SPC340B019 | EG8145V5 | R022 | HGU (Home Gateway Unit) |
| HG8145C_17120_ENG | HG8145C | Engineering | HGU |
| HG8145V5-V500R020C10SPC212 | HG8145V5 | R020C10 | HGU |
| HN8145XR-V500R022C10SPC160 | HN8145XR | R022C10 | XGS-PON |

---

## 1. SYSTEM ARCHITECTURE

### 1.1 Key Binaries
| Binary | Path | Description |
|--------|------|-------------|
| `clid` | `/bin/clid` | CLI Daemon - Main CLI shell handler (ARM 32-bit, stripped, musl libc) |
| `shellconfig` | `/bin/shellconfig` | Shell configuration manager |
| `sudo` | `/sbin/sudo` | Sudo binary - Privilege escalation (standard sudo 1.9.x, ARM) |
| `cblc` | `/bin/cblc` | Configuration Backup/Load Controller - manages hardinfo files |
| `bbsp` | `/bin/bbsp` | Broadband Service Processor |
| `cfgtoolc` | `/bin/cfgtoolc` | Configuration tool (client) |
| `get_feature_spec` | `/sbin/get_feature_spec` | Feature/Spec query tool (aliased as GetFeature, GetSpec) |
| `telnet` | `/bin/telnet` | Telnet client (busybox symlink) |
| `ontchmod` | `/sbin/ontchmod` | ONT permission changer |

### 1.2 Key Libraries
| Library | Description |
|---------|-------------|
| `libhw_swm.so` | Software Management - handles hardinfo spec/feature files |
| `libhw_swm_dll.so` | SWM DLL - hardinfo file management, upgrade validation |
| `libhw_ssp_basic.so` | SSP Basic - contains `HW_Spec_GetValueByName`, `HW_Feature_IsSupportByFeatureName` |
| `libcfg_api.so` | Configuration API |
| `libdm_pdt.so` | Data Model PDT - hardinfo data model |
| `libhw_smp_dm_pdt.so` | SMP Data Model PDT |
| `libhw_cli_dcom_transparent_core.so` | CLI DCOM transparent core |
| `libbbsp.so` | BBSP core library |

### 1.3 Critical File Paths
| Path | Description |
|------|-------------|
| `/mnt/jffs2/hw_hardinfo_spec` | Hardware spec configuration (binary/encrypted) |
| `/mnt/jffs2/hw_hardinfo_feature` | Hardware feature configuration (binary/encrypted) |
| `/mnt/jffs2/hw_hardinfo_spec.bak` | Spec backup |
| `/mnt/jffs2/hw_hardinfo_feature.bak` | Feature backup |
| `/mnt/jffs2/hw_boardinfo` | Board information (encrypted) |
| `/mnt/jffs2/hw_ctree.xml` | Current configuration tree (encrypted) |
| `/mnt/jffs2/hw_default_ctree.xml` | Default configuration tree |
| `/etc/wap/hw_cli.xml` | CLI command definitions (encrypted) |
| `/etc/wap/dm.cfg/` | Device model configurations (per-model hardinfo) |
| `/etc/wap/ft/` | Feature configuration files |
| `/etc/wap/spec/` | Spec configuration files |
| `/etc/wap/group` | System group definitions |
| `/etc/wap/passwd` | System user definitions |
| `/var/specifiedUpgradeInfo` | Upgrade info |
| `/mnt/jffs2/swm_debug` | SWM debug flag |
| `/mnt/jffs2/debugcheck` | Debug check flag |

---

## 2. TELNET SYSTEM

### 2.1 Telnet Access
- Telnet daemon is managed by the CLI system (`clid`)
- Telnet sessions create PTY devices at `/var/clitelnet_<pid>.pts`
- Telnet access is controlled by features:
  - `FT_SSMP_TELNET_LAN_WAN` - Controls telnet LAN/WAN access
  - `FEATURE_TELNET_IGNORE_TERMTYPE` - Terminal type handling
- The CLI function `HW_CLI_TelnetLocalAddr` handles telnet address resolution
- `HW_CLI_IsLocalClient` checks if the connection is local
- `CLI_IsAISCustomizeTelnet` checks for AIS customization

### 2.2 Telnet Login Flow
1. Connection established via busybox telnetd
2. `clid` handles the session
3. `HW_CLI_CheckLoginUser` validates user
4. `HW_CLI_CheckPwd` checks password
5. `HW_CLI_CheckAftLogin` performs post-login checks
6. `HW_CLI_GetCliUserGroupInEquipMode` determines user group
7. `HW_CLI_SPEC_GetUserGroup` gets group from spec
8. Command tree is filtered based on `CmdGroup` permissions

---

## 3. CLI SHELL SYSTEM

### 3.1 CLI Architecture
- The CLI is XML-driven (`hw_cli.xml`)
- Commands are defined with `CmdStr`, `ObjPath`, `OpType`, `CmdGroup`
- The `clid` binary loads the XML command tree at startup
- Commands are filtered based on user group permissions
- Internal commands are registered via `HW_CLI_RegInternelCmd`
- Module commands loaded via `HW_CLI_InitModuleCmdTree`

### 3.2 CLI Command Permission Groups (CmdGroup)
The CmdGroup field is a 32-bit bitmask controlling access:

| CmdGroup Value | Access Level | Description |
|---------------|-------------|-------------|
| `""` (empty) | **All users** | Basic commands, no restriction |
| `0x00000010` | **Basic config** | Basic configuration commands |
| `0x00002000` | **Debug read** | Debug display commands |
| `0x00004000` | **Advanced read** | Advanced display/status commands |
| `0x00004001` | **Advanced config** | Advanced configuration |
| `0x00004010` | **Service config** | Service-level configuration |
| `0x00000000` | **All users** | Same as empty |
| `0x10000000` | **System** | System-level operations (preplugin) |
| `0x80000000` | **Admin/SU** | Admin-only commands (WAN, NAT, DHCP, firewall, backup/restore) |
| `0x80000010` | **Admin config** | Admin configuration (set wlan enable) |
| `0x80002000` | **Admin debug** | Admin debug commands (debugging voip, cwmp, upnp) |
| `0x80004000` | **Admin advanced** | Admin advanced (wifi filter, mac filter, display wifi) |
| `0x80004010` | **Admin service** | Admin service (display user device) |

**Bit Pattern:**
- Bit 31 (0x80000000) = Admin/SU required
- Bit 28 (0x10000000) = System level
- Bits 14-15 (0x0000C000) = Service level
- Bits 12-13 (0x00003000) = Debug level
- Bits 8-11 (0x00000F00) = Advanced level
- Bits 0-7 (0x000000FF) = Basic level

### 3.3 Total CLI Commands: 849 unique commands

---

## 4. USERGROUPS AND PRIVILEGE LEVELS

### 4.1 Group Definitions (from `/etc/wap/group`)

| GID | Group Name | Members | Privilege Level |
|-----|-----------|---------|----------------|
| 0 | `root` | (none directly) | **HIGHEST** - System root |
| 5 | `tty` | srv_clid | Terminal access |
| 6 | `disk` | srv_ssmp, osgi_proxy, srv_wifi, srv_mu, srv_clid, srv_amp, srv_dbus | Disk access |
| 9 | `kmem` | (none) | Kernel memory |
| 10 | `hlp` | srv_kmc | Help/KMC |
| 11 | `fuse` | srv_ssmp | FUSE filesystem |
| 500 | `kmc` | (none) | Key Management |
| 2000 | `osgi` | srv_ssmp, srv_appm, srv_cagent, srv_clid, srv_cms, srv_dbus, srv_wifi | **OSGi platform** |
| 2001 | `config` | srv_ssmp, cfg_cwmp, srv_clid, srv_comm, srv_amp, srv_web, srv_cms | **Configuration** |
| 2002 | `service` | cfg_cwmp, osgi_proxy, srv_cagent, srv_apm, srv_kmc, srv_comm, srv_web | **Service** |
| 2003 | `tools` | (none) | **Tools/Diagnostic** |
| 2014 | `apm` | (none) | Application Management |
| 2024 | `voice` | srv_voice | Voice services |
| 2034 | `comm` | srv_comm | Communication |
| 5000 | `samba` | (none) | Samba/File sharing |
| 65534 | `nobody` | (none) | **LOWEST** - No privileges |

### 4.2 User Accounts (from `/etc/wap/passwd`)

| UID | User | GID | Home | Shell | Role |
|-----|------|-----|------|-------|------|
| 0 | root | 0 | /root | /sbin/nologin | System root (locked) |
| 3003 | srv_amp | 2002 | /var/srv_amp | /bin/false | AMP service |
| 3004 | srv_web | 2002 | /var/srv_web | /bin/false | Web service |
| 3005 | osgi_proxy | 2000 | /var/osgi_proxy | /bin/false | OSGi proxy |
| 3006 | srv_igmp | 2002 | /var/srv_igmp | /bin/false | IGMP service |
| 3007 | cfg_cwmp | 2001 | /var/cfg_cwmp | /bin/false | CWMP config |
| 3008 | srv_ssmp | 2002 | /var/srv_ssmp | /bin/false | SSMP service |
| 3010 | cfg_cli | 2001 | /var/cfg_cli | /bin/false | CLI config |
| 3012 | srv_bbsp | 2002 | /var/srv_bbsp | /bin/false | BBSP service |
| 3014 | srv_dbus | 2002 | /var/srv_dbus | /bin/false | D-Bus service |
| 3015 | srv_udm | 2002 | /var/srv_udm | /bin/false | UDM service |
| 3018 | srv_apm | 2014 | /var/srv_apm | /bin/false | APM service |
| 3020 | srv_kmc | 500 | /var/srv_kmc | /bin/false | KMC service |
| 3021 | srv_cms | 2002 | /var/srv_cms | /bin/false | CMS service |
| 3022 | srv_mu | 2002 | /var/srv_mu | /bin/false | MU service |
| 3023 | srv_em | 2002 | /var/srv_em | /bin/false | EM service |
| 3030 | srv_clid | 2002 | /var/srv_clid | /bin/false | CLI daemon |
| 3029 | srv_comm | 2034 | /var/srv_comm | /bin/false | Communication |
| 4002 | srv_voice | 2002 | /var/service | /bin/false | Voice service |
| 4003 | srv_appm | 2002 | /var/service | /bin/false | App management |
| 4005 | srv_cagent | 2000 | /var/srv_cagent | /bin/false | C-Agent |
| 65534 | nobody | 65534 | /tmp | /bin/false | No privileges |

### 4.3 Privilege Hierarchy (Highest to Lowest)

1. **root (GID 0)** - Full system access, but shell is `/sbin/nologin`
2. **config (GID 2001)** - Configuration access (cfg_cwmp, srv_clid, srv_web, srv_amp)
3. **osgi (GID 2000)** - Platform access (srv_ssmp, srv_clid, srv_wifi, srv_dbus)
4. **service (GID 2002)** - Service access (most service accounts)
5. **tools (GID 2003)** - Diagnostic tools (no members by default)
6. **apm (GID 2014)** - Application management
7. **comm (GID 2034)** - Communication services
8. **voice (GID 2024)** - Voice services only
9. **nobody (GID 65534)** - No access

### 4.4 CLI User Groups (from clid binary analysis)
The CLI uses its own user group system via `HW_CLI_GetCliUserGroupInEquipMode` and `HW_CLI_SPEC_GetUserGroup`:
- The user group determines which `CmdGroup` commands are visible
- SU mode (`HW_CLI_SU_Mode`) provides elevated access
- SU password verification via `HW_CLI_VerifySuPassword`
- Admin authority check via `HW_CLI_IsInitAdmAuthorityCmd`

---

## 5. HARDINFO SYSTEM - `set hardinfo value` ANALYSIS

### 5.1 Binary/Library Locations
The `set hardinfo value` command is handled by:
- **Primary**: `libhw_swm.so` (Software Management library)
- **Secondary**: `libhw_swm_dll.so` (SWM DLL)
- **Data Model**: `libdm_pdt.so`, `libhw_smp_dm_pdt.so`
- **Utility**: `cblc` binary (Configuration Backup/Load Controller)
- **Query**: `get_feature_spec` (aliased as `GetFeature`, `GetSpec`)

### 5.2 Hardinfo File Structure
Files stored at `/mnt/jffs2/`:
- `hw_hardinfo_spec` - Spec values (binary format)
- `hw_hardinfo_feature` - Feature flags (binary format)
- Both have `.bak` backups

### 5.3 Source Configuration (dm.cfg files)
The hardinfo values originate from `/etc/wap/dm.cfg/HWSOC1<MODEL>.cfg` files.
Format: `obj.id="0xNNNNNNNN";obj.name="<name>";obj.value="<value>";`

### 5.4 Complete Hardinfo Spec Values (obj.id -> obj.name -> valid values)

#### Hardware Specification (obj.id 0x00000001 - 0x000000FF)

| obj.id | obj.name | Description | Valid Values |
|--------|----------|-------------|--------------|
| 0x00000001 | `eth_num` | Ethernet port count | 1-8 (typically 4) |
| 0x00000002 | `eth_type` | Ethernet type | 0=FE, 1=GE |
| 0x00000003 | `pots_num` | POTS port count | 0-4 |
| 0x00000004 | `pots_type` | POTS type | 1=Common, 2=BT |
| 0x00000005 | `ssid_num` | SSID/WiFi count | 1-16 (typically 8) |
| 0x00000006 | `rf_num` | RF port count | 0-4 |
| 0x00000007 | `usb_num` | USB port count | 0-4 |
| 0x00000008 | `wlan_num` | WLAN interface count | 1-4 |
| 0x00000009 | `tdm_num` | TDM port count | 0-4 |
| 0x0000000A | `pon_num` | PON port count | 1-2 |
| 0x0000000B | `mac_num` | MAC address count | 1-32 |
| 0x0000000C | `proc_id` | Product ID | Numeric (e.g., 345) |
| 0x0000000D | `proc_name` | Product name | String (e.g., "EG8145V5") |
| 0x0000000E | `proc_desc` | Product description | String |
| 0x0000000F | `proc_class` | Product class | String |
| 0x00000010 | `equ_id` | Equipment ID | String |
| 0x00000011 | `pdt_type` | Product type | "HGU", "SFU", "MDU", "MTU" |
| 0x00000012 | `manufacture` | Manufacturer | "Huawei Technologies Co., Ltd" |
| 0x00000013 | `vendor_id` | Vendor ID | "HWTC" (4 chars) |
| 0x00000014 | `key_cap` | Key capability (WLAN/WPS/RESET) | Hex bitmask (e.g., 0x000f) |
| 0x00000015 | `battery_led_cap` | Battery LED capability | 0-1 |
| 0x00000016 | `battery_cap` | Battery capability | 0-1 |
| 0x00000017 | `cpu_capability` | CPU capability (pps) | Numeric (e.g., 13000) |
| 0x00000018 | `bak_batt` | Backup battery support | 0-1 |
| 0x00000019 | `wlan_light` | WLAN indicator count | 0-4 |
| 0x0000001a | `customize_flag` | Customization flag | "COMMON", "TDE2", "TDESME2", operator name |
| 0x0000001b | `cfg_word` | Configuration word | "COMMON", "TDE2", "TDESME2", operator name |
| 0x0000001d | `ethoammac_num` | EthOAM MAC count | 0-4 |
| 0x00000020 | `country_code` | Country code | 2-char string |
| 0x00000025 | `wifi_low_cap` | 2.4G WiFi frequency capability | Bitmask (e.g., 37748737) |
| 0x00000026 | `wifi_high_cap` | 5G WiFi frequency capability | Bitmask (e.g., 33554432) |
| 0x00000028 | `temperature_protect` | Temperature protection | 0-1 |
| 0x00000031 | `usescene_flag` | Use scene flag | "EeAa", "NOCHOOSE", operator codes |
| 0x00000035 | `loid` | Logical ID | String |
| 0x00000039 | `hw_version` | Hardware version | Hex (e.g., 0x00102001) |
| 0x0000003a | `sn_password` | SN password | String |
| 0x0000003c | `omci_olt_id` | OMCI OLT ID | Hex/String |
| 0x0000003d | `omci_onu_id` | OMCI ONU ID | String |
| 0x00000042 | `xpon_mode` | XPON mode | 0=Auto, specific mode values |
| 0x00000052 | `eponkey` | EPON key | String (encrypted) |
| 0x00000060 | `custom_info` | Custom info | String |
| 0x00000061 | `ap_mode_flag` | AP mode flag | 0-1 |
| 0x00000065 | `mac_offset_1` | MAC offset 1 | String |
| 0x00000066 | `mac_offset_2` | MAC offset 2 | String |
| 0x00000073 | `dual_core_flag` | Dual core flag | 0-1 |
| 0x00000083 | `extra_cap` | Extra capability | String |

#### Feature Configuration File Paths (obj.id 0xB0000001 - 0xB0000019)

| obj.id | obj.name | Description | Valid Values |
|--------|----------|-------------|--------------|
| 0xB0000001 | `featurecfg_ssmp` | SSMP feature config path | File path |
| 0xB0000002 | `featurecfg_bbsp` | BBSP feature config path | File path |
| 0xB0000003 | `featurecfg_amp` | AMP feature config path | File path |
| 0xB0000004 | `featurecfg_vspa` | VSPA feature config path | File path |
| 0xB0000005 | `featurecfg_hard` | Hardinfo feature file path | `/mnt/jffs2/hw_hardinfo_feature` |
| 0xB0000006 | `featurecfg_wifi` | WiFi feature config path | File path |
| 0xB0000007 | `featurecfg_smart` | Smart feature config path | File path |
| 0xB0000011 | `spec_ssmp` | SSMP spec path | File path |
| 0xB0000012 | `spec_bbsp` | BBSP spec path | File path |
| 0xB0000013 | `spec_amp` | AMP spec path | File path |
| 0xB0000014 | `spec_vspa` | VSPA spec path | File path |
| 0xB0000015 | `spec_boardtype` | Board type spec path | File path |
| 0xB0000016 | `spec_hard` | Hardinfo spec path | `/mnt/jffs2/hw_hardinfo_spec` |
| 0xB0000017 | `feature_cust` | Custom feature path | `/etc/wap/cust_feature.cfg` |
| 0xB0000018 | `spec_smart` | Smart spec path | File path |
| 0xB0000019 | `spec_wifi` | WiFi spec path | File path |

---

## 6. FEATURE FLAGS - Complete List

### 6.1 SSMP Features (from ft_default.cfg and per-model configs)

| Feature Name | Description | Enable Values |
|-------------|-------------|---------------|
| `HW_SSMP_FEATURE_ACCESSMNGT` | Access management | 0/1 |
| `HW_SSMP_FEATURE_APM` | Application management | 0/1 |
| `HW_SSMP_FEATURE_APMLAN` | APM LAN | 0/1 |
| `HW_SSMP_FEATURE_APMRF` | APM RF | 0/1 |
| `HW_SSMP_FEATURE_APMVOICE` | APM Voice | 0/1 |
| `HW_SSMP_FEATURE_APMWLAN` | APM WLAN | 0/1 |
| `HW_SSMP_FEATURE_BATTERY` | Battery support | 0/1 |
| `HW_SSMP_FEATURE_CFG_BACKUP` | Config backup | 0/1 |
| `HW_SSMP_FEATURE_CWMP_CTCOM` | CWMP China Telecom | 0/1 |
| `HW_SSMP_FEATURE_CWMP_CU` | CWMP China Unicom | 0/1 |
| `HW_SSMP_FEATURE_CWMP_DEF` | CWMP Default | 0/1 |
| `HW_SSMP_FEATURE_CWMP_JSCT` | CWMP Jiangsu CT | 0/1 |
| `HW_SSMP_FEATURE_CWMP_PWDKEY` | CWMP password key | 0/1 |
| `HW_SSMP_FEATURE_DM_LEDRESETKEY` | DM LED reset key | 0/1 |
| `HW_SSMP_FEATURE_DM_SMBA` | DM Samba | 0/1 |
| `HW_SSMP_FEATURE_DLNA` | DLNA support | 0/1 |
| `HW_SSMP_FEATURE_MNGT_AHCT` | Management Anhui CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_CQCT` | Management Chongqing CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_CT` | Management China Telecom | 0/1 |
| `HW_SSMP_FEATURE_MNGT_FJCT` | Management Fujian CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_GDCT` | Management Guangdong CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_GSCT` | Management Gansu CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_GZCT` | Management Guizhou CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_HLJCT` | Management Heilongjiang CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_HUNCT` | Management Hunan CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_JSCT` | Management Jiangsu CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_JXCT` | Management Jiangxi CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_NXCT` | Management Ningxia CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_PCCW` | Management PCCW | 0/1 |
| `HW_SSMP_FEATURE_MNGT_QHCT` | Management Qinghai CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_SCCT` | Management Sichuan CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_SHCT` | Management Shanghai CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_SZCT` | Management Shenzhen CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_TELMEX` | Management Telmex | 0/1 |
| `HW_SSMP_FEATURE_MNGT_UNICOM` | Management Unicom | 0/1 |
| `HW_SSMP_FEATURE_MNGT_XJCT` | Management Xinjiang CT | 0/1 |
| `HW_SSMP_FEATURE_MNGT_YNCT` | Management Yunnan CT | 0/1 |
| `HW_SSMP_FEATURE_OUI_STATIC` | OUI Static | 0/1 |
| `HW_SSMP_FEATURE_QUICKCFG` | Quick configuration | 0/1 |
| `HW_SSMP_FEATURE_RESET_CTCOM` | Reset China Telecom | 0/1 |
| `HW_SSMP_FEATURE_RESET_CU` | Reset China Unicom | 0/1 |
| `HW_SSMP_FEATURE_RESET_DEF` | Reset Default | 0/1 |
| `HW_SSMP_FEATURE_RESET_JSCT` | Reset Jiangsu CT | 0/1 |
| `HW_SSMP_FEATURE_TR069` | TR069 support | 0/1 |
| `HW_SSMP_FEATURE_USB` | USB support | 0/1 |
| `HW_SSMP_FEATURE_USBPRINTER` | USB printer | 0/1 |
| `HW_SSMP_FEATURE_USBSTORAGE` | USB storage | 0/1 |
| `HW_SSMP_FEATURE_USD` | USD support | 0/1 |
| `HW_SSMP_FEATURE_WEB` | Web interface | 0/1 |
| `HW_SSMP_FEATURE_WIFIAP_USABLE` | WiFi AP usable | 0/1 |
| `FT_SMART_BOARD` | Smart board | 0/1 |
| `FT_OLT_UTC_TIME` | OLT UTC time | 0/1 |
| `HW_FT_SSMP_LAN_TRANSPAREN` | LAN transparent | 0/1 |
| `HW_FEATURE_CHECKUPG_SPACE` | Check upgrade space | 0/1 |
| `FT_FTTR_CHINA_UNITE_BIN` | FTTR China Unicom | 0/1 |
| `FT_NEW_AP` | New AP support | 0/1 |
| `FT_ONT_SWITCH_AP_MODE` | ONT switch AP mode | 0/1 |
| `FT_ONT_SWITCH_AP_MODE_RECOVER` | ONT AP mode recover | 0/1 |
| `HW_SSMP_FEATURE_CTRG` | CTC RG support | 0/1 |
| `HW_FT_OSGI_JVM_FROM_VAR` | OSGi JVM from var | 0/1 |
| `SSMP_FT_DFX_ACS_UPGRADE` | DFX ACS upgrade | 0/1 |

### 6.2 BBSP Features

| Feature Name | Description | Enable Values |
|-------------|-------------|---------------|
| `BBSP_FT_SNTP` | SNTP | 0/1 |
| `BBSP_FT_8010_DT` | 802.10 DT | 0/1 |
| `BBSP_FT_ACCLIMIT` | Access limit | 0/1 |
| `BBSP_FT_ACL` | ACL | 0/1 |
| `BBSP_FT_ALG` | ALG | 0/1 |
| `BBSP_FT_ARP` | ARP | 0/1 |
| `BBSP_FT_ARPPING` | ARP ping | 0/1 |
| `BBSP_FT_BT` | Bluetooth | 0/1 |
| `BBSP_FT_CTC` | China Telecom | 0/1 |
| `BBSP_FT_DDNS_IP` | DDNS IP | 0/1 |
| `BBSP_FT_DDNS_PPP` | DDNS PPP | 0/1 |
| `BBSP_FT_DHCP_HOST` | DHCP host | 0/1 |
| `BBSP_FT_DHCP_HOSTNUM` | DHCP host number | Numeric |
| `BBSP_FT_DHCP_MAIN` | DHCP main | 0/1 |
| `BBSP_FT_DHCPOPTION` | DHCP option | 0/1 |
| `BBSP_FT_DHCP_CLIENT_OPTION` | DHCP client option | 0/1 |
| `BBSP_FT_DHCP_SLAVE` | DHCP slave | 0/1 |
| `BBSP_FT_DHCPS_OPTION` | DHCP server option | 0/1 |
| `BBSP_FT_DHCPS_COND_POOL` | DHCP conditional pool | 0/1 |
| `BBSP_FT_DMZ_IP` | DMZ IP | 0/1 |
| `BBSP_FT_DMZ_PPP` | DMZ PPP | 0/1 |
| `BBSP_FT_DNS` | DNS | 0/1 |
| `BBSP_FT_DNS_CLIENT` | DNS client | 0/1 |
| `BBSP_FT_DOSFILTER` | DoS filter | 0/1 |
| `BBSP_FT_DSCP_PRI` | DSCP priority | 0/1 |
| `BBSP_FT_FIREWALL` | Firewall | 0/1 |
| `BBSP_FT_IGMP_ENABLE` | IGMP enable | 0/1 |
| `BBSP_FT_IPFILTERIN` | IP filter in | 0/1 |
| `BBSP_FT_IPFILTEROUT` | IP filter out | 0/1 |
| `BBSP_FT_IPV6` | IPv6 | 0/1 |
| `BBSP_FT_IPV6_CORE` | IPv6 core | 0/1 |
| `BBSP_FT_IPV6_DHCP6S` | IPv6 DHCP6S | 0/1 |
| `BBSP_FT_IPV6_DIAG` | IPv6 diagnostic | 0/1 |
| `BBSP_FT_IPV6_DSLITE` | IPv6 DS-Lite | 0/1 |
| `BBSP_FT_IPV6_LANDEV` | IPv6 LAN device | 0/1 |
| `BBSP_FT_IPV6_ROUTE` | IPv6 route | 0/1 |
| `BBSP_FT_IPV6_WANCFG` | IPv6 WAN config | 0/1 |
| `BBSP_FT_L3` | Layer 3 | 0/1 |
| `BBSP_FT_L3_ALL` | Layer 3 all | 0/1 |
| `BBSP_FT_L3IGMP` | L3 IGMP | 0/1 |
| `BBSP_FT_LANBIND_IP` | LAN bind IP | 0/1 |
| `BBSP_FT_LANBIND_PPP` | LAN bind PPP | 0/1 |
| `BBSP_FT_LANBIND_SSID` | LAN bind SSID | 0/1 |
| `BBSP_FT_LANHOSTIP` | LAN host IP | 0/1 |
| `BBSP_FT_MACFILTER` | MAC filter | 0/1 |
| `BBSP_FT_NAT` | NAT | 0/1 |
| `BBSP_FT_ONTACCESSCTL` | ONT access control | 0/1 |
| `BBSP_FT_PING` | Ping | 0/1 |
| `BBSP_FT_PORTAL` | Portal | 0/1 |
| `BBSP_FT_PORTMAP_IP` | Port map IP | 0/1 |
| `BBSP_FT_PORTMAP_PPP` | Port map PPP | 0/1 |
| `BBSP_FT_PORTTRIGGER_IP` | Port trigger IP | 0/1 |
| `BBSP_FT_PORTTRIGGER_PPP` | Port trigger PPP | 0/1 |
| `BBSP_FT_PPPOE_EM` | PPPoE EM | 0/1 |
| `BBSP_FT_QOS_CAR` | QoS CAR | 0/1 |
| `BBSP_FT_QOS_CFG` | QoS config | 0/1 |
| `BBSP_FT_QOS_CLA` | QoS classifier | 0/1 |
| `BBSP_FT_QOS_MIRROR` | QoS mirror | 0/1 |
| `BBSP_FT_QOS_QUEUE` | QoS queue | 0/1 |
| `BBSP_FT_RINGCHK` | Ring check | 0/1 |
| `BBSP_FT_ROUTE` | Route | 0/1 |
| `BBSP_FT_ROUTE_STATIC` | Static route | 0/1 |
| `BBSP_FT_ROUTE_POLICY` | Policy route | 0/1 |
| `BBSP_FT_SEC` | Security | 0/1 |
| `BBSP_FT_UPNP_MAIN` | UPnP main | 0/1 |
| `BBSP_FT_UPNP_SLV` | UPnP slave | 0/1 |
| `BBSP_FT_URLFIL` | URL filter | 0/1 |
| `BBSP_FT_WAN` | WAN | 0/1 |
| `BBSP_FT_WAN_CFG` | WAN config | 0/1 |
| `BBSP_FT_WLANMACFILER` | WLAN MAC filter | 0/1 |

### 6.3 AMP Features

| Feature Name | Description | Enable Values |
|-------------|-------------|---------------|
| `HW_AMP_FEATURE_COMMON` | Common mode | 0/1 |
| `HW_AMP_FEATURE_CTC` | China Telecom | 0/1 |
| `HW_AMP_FEATURE_DT` | Datang | 0/1 |
| `HW_AMP_FEATURE_WLAN` | WLAN | 0/1 |
| `HW_AMP_FEATURE_BJCU` | Beijing Unicom | 0/1 |
| `HW_AMP_FEATURE_HUNCT` | Hunan CT | 0/1 |
| `HW_AMP_FEATURE_QHCT` | Qinghai CT | 0/1 |
| `HW_AMP_FEATURE_TELMEX` | Telmex | 0/1 |

---

## 7. SPEC VALUES - Complete List

### 7.1 SSMP Spec Values

| Spec Name | Type | Description | Valid Values |
|-----------|------|-------------|--------------|
| `SPEC_OS_AES_CBC_APP_STR` | string | AES encryption key | String |
| `SSMP_SPEC_WEB_OUTCHANGEPORT` | uint | Web external port | 80, 443, 8080, etc. |
| `SSMP_SPEC_WEB_NO_AUTH_PAGE` | string | Web pages without auth | Semicolon-separated list |
| `SSMP_SPEC_WEB_CFGFILE_SIZE` | uint | Web config file size | Numeric (KB) |
| `SSMP_SPEC_WEB_PREVMENUXML` | string | Web previous menu XML | String |
| `SSMP_SPEC_WEB_PREVINDEXASP` | string | Web previous index ASP | String |
| `SSMP_SPEC_SWM_MODULESIZE` | uint | SWM module size | Numeric |
| `SSMP_SPEC_SWM_FLASHCONFIGADDR` | uint | SWM flash config address | Hex |
| `SSMP_SPEC_BLANK_CLIFILENAME` | string | Blank CLI filename | String |
| `SSMP_SPEC_LLID_MAC_NUM` | uint | LLID MAC number | 0-8 |
| `SSMP_SPEC_ETHOAM_MAC_NUM` | uint | EthOAM MAC number | 0-8 |
| `SPEC_OMCI_PACKAGE_NUM_DELAY` | uint | OMCI package delay | 0-N |
| `SSMP_SPEC_CLI_USER_CFG` | string | CLI user config | String |
| `SSMP_SPEC_STB_MAC_NUM` | uint | STB MAC number | 0-8 |
| `SPEC_SSL_ENABLE_DEPVERSION` | uint | SSL deprecated version | 0/1 |
| `SPEC_SSL_ENABLE_DEPCIPHER` | uint | SSL deprecated cipher | 0/1 |
| `SSMP_SPEC_DEVSCOM_DSL_IP` | string | DevSCom DSL IP | IP address |
| `SSMP_SPEC_DEVSCOM_LTE_IP` | string | DevScOm LTE IP | IP address |
| `SSMP_SPEC_DEVSCOM_NETMASK` | string | DevScOm netmask | Netmask |
| `SSMP_SPEC_DEVSCOM_COMPORT` | uint | DevScOm COM port | Numeric |
| `SPEC_AIS_KEY_ID` | uint | AIS key ID | Numeric |
| `SPEC_AIS_DOMAIN_ID` | uint | AIS domain ID | Numeric |
| `USB_SPEC_PCDN_NUM` | uint | USB PCDN number | 0-N |
| `SPEC_DATA_MODEL_MAP` | string | Data model map | String |

### 7.2 BBSP Spec Values

| Spec Name | Type | Description | Valid Values |
|-----------|------|-------------|--------------|
| `BBSP_SPEC_NAT_PORTMAPNUM` | uint | NAT port map count | 8-64 |
| `BBSP_SPEC_NAT_PORTTRIGGERNUM` | uint | NAT port trigger count | 8-64 |
| `BBSP_SPEC_IPV4_ROUTE_MAXNUM` | uint | IPv4 route max | 64-256 |
| `BBSP_SPEC_IPV6_ROUTE_MAXNUM` | uint | IPv6 route max | 16-64 |
| `BBSP_SPEC_QOS_QUEUENUM` | uint | QoS queue number | 64-256 |
| `BBSP_SPEC_IGMP_MODE` | string | IGMP mode | "GPON", "EPON" |
| `BBSP_SPEC_ROUTEWAN_MAXNUM` | uint | Route WAN max | 4-16 |
| `BBSP_SPEC_WAN_IPORPPPCONNNUM` | uint | WAN IP/PPP conn max | 2-8 |
| `BBSP_SPEC_NAPT_ITEM_NUM` | uint | NAPT item number | 4096-16384 |
| `BBSP_SPEC_FWD_SESSIONNUM` | uint | Forward session number | 8000-32000 |
| `BBSP_SPEC_SERVICELIST_INT_KEY` | string | Internet service key | "HSI" |
| `BBSP_SPEC_SERVICELIST_VOIP_KEY` | string | VoIP service key | "VOICE" |
| `BBSP_SPEC_SERVICELIST_TR069_KEY` | string | TR069 service key | "ACS" |
| `BBSP_SPEC_SERVICELIST_IPTV_KEY` | string | IPTV service key | "IPTV" |

---

## 8. SUDO UNLOCK PROCEDURE FOR TELNET SHELL

### 8.1 Understanding the Sudo System
The `sudo` binary at `/sbin/sudo` is a standard sudo 1.9.x compiled for ARM with musl libc. It:
- Uses policy plugins for authorization
- Supports `get_user_groups` and `fill_group_list`
- Has `direct_exec_allowed` function
- Checks user via `get_user_info`

### 8.2 Method 1: Direct Shell Escape via clid
The CLI shell (`clid`) has internal command execution via `HW_CLI_ExeShell` and `HW_CLI_ExecvShell`. When in SU mode, the shell can execute system commands.

**Steps:**
```
1. Connect via telnet to the ONT
   telnet <ONT_IP>

2. Login with credentials (default: root/root or Epontech/Epontech)

3. Enter SU mode:
   su

4. Enter SU password (same as login password or configured SU password)

5. Once in SU mode, execute shell commands:
   shell
   (or the CLI may automatically provide shell access)

6. From shell, you have root-level access
```

### 8.3 Method 2: Exploiting the CLI Command Tree
The CLI has commands with empty CmdGroup (accessible to all users) that can be leveraged:

```
1. Connect via telnet
2. Login with any valid user
3. Use commands with empty CmdGroup to gather info:
   display chginfo
   display current-configuration
   display lanethinfo
4. Use 'capture' commands for network analysis
```

### 8.4 Method 3: Modifying hardinfo to Unlock Full Access
The hardinfo system controls which features are available. By modifying the hardinfo files:

```
1. Access via telnet with SU privileges
2. Use cblc or direct file manipulation:
   
   # Backup current hardinfo
   cp /mnt/jffs2/hw_hardinfo_feature /mnt/jffs2/hw_hardinfo_feature.orig
   cp /mnt/jffs2/hw_hardinfo_spec /mnt/jffs2/hw_hardinfo_spec.orig
   
   # Use GetFeature/GetSpec to query current values
   GetFeature <FEATURE_NAME>
   GetSpec <SPEC_NAME>
```

### 8.5 Method 4: Debug Mode Activation
```
1. Create debug flag file:
   touch /mnt/jffs2/swm_debug
   
2. Or create debugcheck flag:
   touch /mnt/jffs2/debugcheck
   
3. Create DebugVersionFlag for debug version:
   touch /etc/wap/DebugVersionFlag
   
4. Reboot the ONT
```

---

## 9. COMPLETE PROCEDURE: UNLOCK CLI, CHANGE FIRMWARE TO COMMON, ACTIVATE ALL FEATURES

### 9.1 Step-by-Step Procedure via Telnet

**Prerequisites:** Telnet access to the ONT with SU/root credentials.

#### Step 1: Connect and Login
```bash
telnet <ONT_IP_ADDRESS>
# Login with: root / <password>
# Or: telecomadmin /admintelecom (operator default)
# Or: Epontech / Epontech (common default)
```

#### Step 2: Enter SU Mode
```bash
su
# Enter SU password when prompted
```

#### Step 3: Change Firmware Mode to COMMON
The `restorehwmode.sh` script resets the ONT to factory common mode. The key hardinfo values that control the mode are:

```bash
# obj.id 0x0000001a (customize_flag) -> set to "COMMON"
# obj.id 0x0000001b (cfg_word) -> set to "COMMON"
# obj.id 0x00000031 (usescene_flag) -> set to "NOCHOOSE"

# Method A: Use restorehwmode.sh (full factory reset to common)
/restorehwmode.sh

# Method B: Manual hardinfo modification via boardinfo
# Copy boardinfo to temp location
cp /mnt/jffs2/hw_boardinfo /var/dup_boardinfo

# Edit the boardinfo file (if accessible)
# Change obj.id 0x0000001a value to "COMMON"
# Change obj.id 0x0000001b value to "COMMON"
# Change obj.id 0x00000031 value to "NOCHOOSE"

# Copy back
cp /var/dup_boardinfo /mnt/jffs2/hw_boardinfo
cp /var/dup_boardinfo /mnt/jffs2/hw_boardinfo.bak
```

#### Step 4: Activate All Features via hardinfo
```bash
# The feature file controls which features are enabled
# Feature file format: binary, managed by libhw_swm.so

# Query current feature status:
GetFeature HW_SSMP_FEATURE_WEB
GetFeature HW_SSMP_FEATURE_USB
GetFeature HW_SSMP_FEATURE_DLNA
GetFeature BBSP_FT_FIREWALL
GetFeature BBSP_FT_IPV6
GetFeature BBSP_FT_NAT

# To enable all features, the hw_hardinfo_feature file needs to be modified
# This is typically done through the SWM system

# Alternative: Modify the feature config files directly
# Edit /etc/wap/ft/ft_default.cfg and set all feature.enable="1"
# Then reload:
echo "Reload" > /proc/wap_proc/feature
```

#### Step 5: Configure Web for Debug/Developer Mode
```bash
# Enable debug web interface
touch /etc/wap/DebugVersionFlag

# Set web port to standard (if changed)
# Via spec:
# SSMP_SPEC_WEB_OUTCHANGEPORT = 80

# Enable additional web pages by modifying the no-auth page list
# Via CLI (if available):
# set web mode debug

# Enable web access from WAN:
# Via hardinfo or spec modification
```

#### Step 6: Unlock Full CLI Without Restrictions
```bash
# The CLI command visibility is controlled by CmdGroup in hw_cli.xml
# Commands with 0x80000000+ require SU mode

# Ensure you're in SU mode:
su

# The CLI user group determines visible commands
# To see ALL commands, the user must be in the highest privilege group

# Check current user group:
# (Internal CLI command, may not be directly accessible)

# Force reload of CLI configuration:
echo "Reload" > /proc/wap_proc/feature

# Or restart the CLI daemon:
kill -HUP $(pidof clid)
```

#### Step 7: Enable Telnet WAN Access
```bash
# Enable telnet from WAN side
# Via feature flag:
# FT_SSMP_TELNET_LAN_WAN = 1

# Or via CLI command (if available):
# set telnet wan enable
```

### 9.2 Key Commands Summary

| Action | Command | Notes |
|--------|---------|-------|
| Enter SU mode | `su` | Requires SU password |
| Query feature | `GetFeature <NAME>` | Returns 0 or 1 |
| Query spec | `GetSpec <NAME>` | Returns value |
| Reload features | `echo "Reload" > /proc/wap_proc/feature` | Hot reload |
| Factory reset to common | `/restorehwmode.sh` | Full reset |
| Enable debug mode | `touch /etc/wap/DebugVersionFlag` | Then reboot |
| Restart CLI | `kill -HUP $(pidof clid)` | Restart CLI daemon |
| Query hardinfo | `cblc` | Shows hardinfo status |

---

## 10. CLI COMMANDS BY CATEGORY

### 10.1 System Commands (All Users - Empty CmdGroup)
```
display chginfo                    - Display change info
display current-configuration      - Display current config
display lanethinfo                 - Display LAN ethernet info
display lldp local                 - Display LLDP local
display lldp remote                - Display LLDP remote
display dosfilterrule              - Display DoS filter rules
display ipconn                     - Display IP connections
display ipfilterinrule             - Display IP filter in rules
display ipfilteroutrule            - Display IP filter out rules
display macfilterrule              - Display MAC filter rules
display ip-fpm                     - Display IP FPM
display dhcpopt                    - Display DHCP options
display algpara                    - Display ALG parameters
display port mac num               - Display port MAC count
display ipconlanbind               - Display IP connection LAN bind
capture start/stop/option/autostop - Packet capture
acl show/statclear                 - ACL operations
arp add/del/get/set                - ARP table operations
```

### 10.2 Configuration Commands (Empty CmdGroup)
```
add wanconn                        - Add WAN connection
del wanconn                        - Delete WAN connection
add pppconn                        - Add PPP connection
del pppconn                        - Delete PPP connection
add ipif                           - Add IP interface
del lanif                          - Delete LAN interface
add ipconn                         - Add IP connection
del ipconn                         - Delete IP connection
add ipfilterinrule                 - Add IP filter input rule
add ipfilteroutrule                - Add IP filter output rule
add macfilterrule                  - Add MAC filter rule
add policy route                   - Add policy route
add arping                         - Add ARP ping
add urlflt                         - Add URL filter
add typeportal                     - Add type portal
set wlan enable                    - Enable/disable WLAN
set wlan basic                     - Set WLAN basic config
get wlan enable                    - Get WLAN enable status
get wlan basic                     - Get WLAN basic config
set wlan psk                       - Set WLAN pre-shared key
set wlan wep key                   - Set WLAN WEP key
```

### 10.3 Admin Commands (0x80000000 - SU Required)
```
backup cfg                         - Backup configuration
load cfg                           - Load configuration
restore default configuration      - Factory reset
display firmware version           - Show firmware version
display cli client                 - Show CLI clients
display connection stats           - Show connection statistics
display dhcp client statistics     - Show DHCP client stats
display dhcpsrv info               - Show DHCP server info
display nat config                 - Show NAT config
display upnp mapping               - Show UPnP mappings
display voip service state         - Show VoIP state
display ip-interface               - Show IP interfaces
display ip -6 route                - Show IPv6 routes
set wan fibre/adsl/vdsl/umts       - Configure WAN type
nat config                         - Configure NAT
nat port mapping add/delete/flush  - Port forwarding
ip route add/delete                - Static routes
ip neigh add/delete/flush          - ARP entries
ddns set                           - Configure DDNS
dns client add/delete server       - DNS servers
pppoe client config                - PPPoE configuration
igmp config                        - IGMP configuration
set pppwan                         - Set PPP WAN
set webpa                          - Set WebPA
wan                                - WAN configuration
clear nat table                    - Clear NAT table
flush igmp/upnp                    - Flush IGMP/UPnP
```

### 10.4 Debug Commands (0x80002000 - Admin Debug)
```
debugging alg                      - Debug ALG
debugging connection trace         - Debug connection trace
debugging cwmp server packet       - Debug CWMP packets
debugging dhcp server packet       - Debug DHCP server
debugging upnp                     - Debug UPnP
debugging user trace               - Debug user trace
debugging voip signaling           - Debug VoIP signaling
debugging voip remote-diagnose     - VoIP remote diagnosis
voip calltest start/stop           - VoIP call test
voip set account/service           - Configure VoIP
voip service flush                 - Flush VoIP service
voip remote-diagnose parameters    - VoIP remote diagnose params
```

### 10.5 WiFi/AMP Commands (0x80004000 - Admin Advanced)
```
display macaddress                 - Display MAC addresses
display wifi information           - Display WiFi info
display wifi neighbor              - Display WiFi neighbors
display wifi associate             - Display WiFi associations
display wifi radio                 - Display WiFi radio
display wifi filter                - Display WiFi filter
display wlan staevent              - Display WLAN station events
display femPar info                - Display FEM parameters
display radio stats                - Display radio statistics
add/del wifi filter                - Add/delete WiFi filter
set wifi filter                    - Set WiFi filter
set llid                           - Set LLID
set lanport qbuf                   - Set LAN port queue buffer
save data                          - Save data
wifi del fem par/calibration       - Delete FEM parameters
```

---

## 11. CAPSTONE DISASSEMBLY ANALYSIS

### 11.1 clid Binary - Key Functions
The `clid` binary (ARM 32-bit, stripped) contains these critical functions identified via string references:

- `HW_CLI_GetCliUserGroupInEquipMode` (offset 0x66fc) - Gets user group in equipment mode
- `HW_CLI_SPEC_GetUserGroup` (offset 0x724b) - Gets user group from spec
- `HW_CLI_SU_Mode` (offset 0x5f0d) - Checks/manages SU mode
- `HW_CLI_VerifySuPassword` (offset 0x6017) - Verifies SU password
- `HW_CLI_IfNeedVerifySuPassword` - Checks if SU password verification needed
- `HW_Spec_GetValueByName` (offset 0x4c16) - Gets spec value by name
- `HW_Feature_IsSupportByFeatureName` (offset 0x4a27) - Checks feature support
- `HW_IsCommonVersion` (offset 0x5034) - Checks if common version
- `HW_SSP_IsDebugMode` (offset 0x5151) - Checks debug mode
- `HW_CLI_TelnetLocalAddr` (offset 0x5f77) - Gets telnet local address
- `HW_CLI_IsLocalClient` (offset 0x5b56) - Checks if local client
- `HW_CLI_CheckAccessAuthority` (offset 0x637a) - Checks access authority
- `HW_CLI_CheckCliAuthority` (offset 0x686d) - Checks CLI authority
- `HW_CLI_IsInitAdmAuthorityCmd` (offset 0x5e4d) - Checks admin authority init
- `HW_CLI_HaveRightNotInCusList` - Checks rights not in custom list
- `HW_CLI_ShellDealWithShellCmd` - Processes shell commands
- `HW_CLI_ExeShell` - Executes shell
- `HW_CLI_ExecvShell` - Executes shell with execv
- `HW_CLI_InitEncryptCmdTree` - Initializes encrypted command tree
- `HW_CLI_InitModuleCmdTree` - Initializes module command tree

### 11.2 sudo Binary Analysis
The sudo binary is a standard sudo 1.9.x with:
- `run_command` - Main command execution
- `sudo_execute` - Execution handler
- `direct_exec_allowed` - Direct exec check
- `get_user_info` - User info retrieval
- `get_user_groups` - Group retrieval
- `fill_group_list` - Group list population
- `command_info_to_details` - Command info parsing
- `parse_preserved_fds` - FD preservation
- Policy plugin interface for authorization

### 11.3 libhw_swm.so - Hardinfo Management
Key strings and paths:
- `/mnt/jffs2/hw_hardinfo_spec` - Spec file path
- `/mnt/jffs2/hw_hardinfo_feature` - Feature file path
- `spec is not trust` - Spec validation message
- `ft is not trust` - Feature validation message
- `/mnt/jffs2/swm_debug` - Debug flag path
- `swm_common.c` - Common SWM operations

### 11.4 libhw_swm_dll.so - Hardinfo DLL
Key paths and messages:
- `/mnt/jffs2/hw_hardinfo_spec` and `.bak`
- `/mnt/jffs2/hw_hardinfo_feature` and `.bak`
- `spec,ft,cfgsign are` - Configuration signing
- `Check item value [%s] not in the` - Value validation
- `Check item value [%s] in the [%s` - Value range check
- `/mnt/jffs2/debugcheck` - Debug check flag
- `allowed to upgrade debug bin` - Debug upgrade control
- `swm_cfgfile_common.c` - Common config file handling

---

## 12. WEB INTERFACE CONFIGURATION

### 12.1 Web Spec Configuration
Key spec values for web interface:
- `SSMP_SPEC_WEB_OUTCHANGEPORT` = 80 (web port)
- `SSMP_SPEC_WEB_NO_AUTH_PAGE` - Pages accessible without auth
- `SSMP_SPEC_WEB_CFGFILE_SIZE` = 2048 (config file size limit)
- `SSMP_SPEC_WEB_PREVMENUXML` - Previous menu XML
- `SSMP_SPEC_WEB_PREVINDEXASP` - Previous index page

### 12.2 Enabling Debug/Developer Web
```bash
# Create debug flag
touch /etc/wap/DebugVersionFlag

# Modify web spec to show all pages
# Via CLI or direct spec modification

# The web interface reads from:
# /web/AllUsers/ - Common web files
# /web/frame_huawei/ - Huawei frame
# /web/frame_XGPON/ - XGPON frame
# /web/menu/ - Menu definitions
```

---

## 13. QUICK REFERENCE: COMMON OPERATIONS

### 13.1 Query Current Configuration
```bash
# Get all features
GetFeature <FEATURE_NAME>

# Get all specs
GetSpec <SPEC_NAME>

# Display current config via CLI
display current-configuration

# Display firmware version
display firmware version
```

### 13.2 Modify hardinfo Values
```bash
# The hardinfo files are binary/encrypted
# Use cblc for backup/load operations
# Or modify the source dm.cfg files and regenerate

# For spec changes:
# Edit /etc/wap/spec/ssmp/spec_v5.cfg
# Then reload

# For feature changes:
# Edit /etc/wap/ft/ssmp/EG8145_v5.cfg
# Or /etc/wap/ft/ft_default.cfg
# Then: echo "Reload" > /proc/wap_proc/feature
```

### 13.3 Network Configuration via CLI
```bash
# Show WAN connections
display wan connection

# Show LAN configuration
display lanethinfo

# Show WiFi configuration
get wlan enable laninst 1
get wlan basic laninst 1 wlaninst 1

# Set WiFi
set wlan enable laninst 1 enable 1
set wlan basic laninst 1 wlaninst 1 ssid "MyNetwork"

# Show NAT
display nat config

# Show routes
display ip route
display ip -6 route
```

---

*Analysis performed on firmware from https://github.com/educolabs/Huawei/releases/tag/v1*
*Tools used: Python 3, Capstone Disassembly Framework, strings, file analysis*
*Firmware architecture: ARM 32-bit, musl libc, stripped binaries*
