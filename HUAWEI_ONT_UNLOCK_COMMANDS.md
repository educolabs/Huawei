# Huawei ONT - Complete Unlock & Configuration Commands

## Quick Reference: `set hardinfo value` Command Syntax

```
set hardinfo value SPEC1=value1;SPEC2=value2;&LANGUAGE=lang1,lang2
```

**Important:** Values are separated by semicolons (`;`). Language section starts with `&LANGUAGE=`.

---

## 1. UNLOCK CLI WITH MAXIMUM PRIVILEGES

### 1.1 Set CLI UserGroup to Highest Privilege (0x80004000 = Admin Advanced)

```bash
set hardinfo value SSMP_SPEC_CLI_USERGRP=0x80004000
```

**UserGroup Bitmask Values:**
- `0x80000000` - Admin/SU (WAN, NAT, DHCP, firewall, backup/restore)
- `0x80002000` - Admin debug (debugging voip, cwmp, upnp)
- `0x80004000` - Admin advanced (wifi filter, mac filter, display wifi) **RECOMMENDED**
- `0x80004010` - Admin service (display user device)
- `0x00004000` - Advanced read (display/status commands)
- `0x00000000` - All users (no restrictions)

### 1.2 Enable Remote Telnet Access

```bash
set hardinfo value SSMP_SPEC_CLI_REMOTETELNET=1
```

### 1.3 Set CLI to Redline/Debug Version

```bash
set hardinfo value SSMP_SPEC_CLI_REDLINEVERSION=1
```

### 1.4 Complete CLI Unlock Command (All-in-One)

```bash
set hardinfo value SSMP_SPEC_CLI_USERGRP=0x80004000;SSMP_SPEC_CLI_REMOTETELNET=1;SSMP_SPEC_CLI_REDLINEVERSION=1
```

---

## 2. CONFIGURE WEB INTERFACE FOR DEVELOPER/DEBUG MODE

### 2.1 Set Web Frame to XGPON (Full Featured)

```bash
set hardinfo value SSMP_SPEC_WEB_FRAME=frame_XGPON
```

**Available Frames:**
- `frame_XGPON` - XGS-PON full frame (most features) **RECOMMENDED**
- `frame_huawei` - Standard Huawei frame
- `frame_common` - Common/basic frame

### 2.2 Set Web Menu XML to Full Version

```bash
set hardinfo value SSMP_SPEC_WEB_MENUXML=MenuXGPONAbroad.xml
```

**Available Menus:**
- `MenuXGPONAbroad.xml` - XGPON abroad menu (full features) **RECOMMENDED**
- `MenuAbroad.xml` - Standard abroad menu
- `MenuCommon.xml` - Common menu (basic)

### 2.3 Disable Password Encryption (for debugging)

```bash
set hardinfo value SSMP_SPEC_WEB_PWDENCRYPT=0
```

**Password Encryption Values:**
- `0` - No encryption (plaintext, debug mode)
- `1` - Basic encryption
- `3` - Full encryption (production) **DEFAULT**

### 2.4 Set Web External Port

```bash
set hardinfo value SSMP_SPEC_WEB_OUTCHANGEPORT=80
```

### 2.5 Complete Web Configuration Command

```bash
set hardinfo value SSMP_SPEC_WEB_FRAME=frame_XGPON;SSMP_SPEC_WEB_MENUXML=MenuXGPONAbroad.xml;SSMP_SPEC_WEB_PWDENCRYPT=0;SSMP_SPEC_WEB_OUTCHANGEPORT=80
```

---

## 3. CHANGE FIRMWARE TO COMMON MODE

### 3.1 Set Customize Flag to COMMON

```bash
set hardinfo value customize_flag=COMMON
```

### 3.2 Set Configuration Word to COMMON

```bash
set hardinfo value cfg_word=COMMON
```

### 3.3 Set Use Scene Flag to NOCHOOSE (No Operator Restriction)

```bash
set hardinfo value usescene_flag=NOCHOOSE
```

### 3.4 Set Custom Info to COMMON

```bash
set hardinfo value custom_info=COMMON
```

### 3.5 Complete Common Mode Command

```bash
set hardinfo value customize_flag=COMMON;cfg_word=COMMON;usescene_flag=NOCHOOSE;custom_info=COMMON;&LANGUAGE=COMMON
```

---

## 4. ACTIVATE ALL FEATURES

### 4.1 Enable SSMP Features (System Management)

```bash
set hardinfo value HW_SSMP_FEATURE_WEB=1;HW_SSMP_FEATURE_USB=1;HW_SSMP_FEATURE_USBSTORAGE=1;HW_SSMP_FEATURE_USBPRINTER=1;HW_SSMP_FEATURE_DLNA=1;HW_SSMP_FEATURE_TR069=1;HW_SSMP_FEATURE_CFG_BACKUP=1;HW_SSMP_FEATURE_APM=1;HW_SSMP_FEATURE_APMLAN=1;HW_SSMP_FEATURE_APMWLAN=1;HW_SSMP_FEATURE_APMVOICE=1
```

### 4.2 Enable BBSP Features (Broadband Services)

```bash
set hardinfo value BBSP_FT_FIREWALL=1;BBSP_FT_NAT=1;BBSP_FT_IPV6=1;BBSP_FT_IPV6_CORE=1;BBSP_FT_IPV6_ROUTE=1;BBSP_FT_ROUTE=1;BBSP_FT_ROUTE_STATIC=1;BBSP_FT_ROUTE_POLICY=1;BBSP_FT_UPNP_MAIN=1;BBSP_FT_UPNP_SLV=1;BBSP_FT_DDNS_IP=1;BBSP_FT_DDNS_PPP=1;BBSP_FT_DMZ_IP=1;BBSP_FT_DMZ_PPP=1;BBSP_FT_PORTMAP_IP=1;BBSP_FT_PORTMAP_PPP=1;BBSP_FT_PORTTRIGGER_IP=1;BBSP_FT_PORTTRIGGER_PPP=1;BBSP_FT_IGMP_ENABLE=1;BBSP_FT_QOS_CFG=1;BBSP_FT_QOS_QUEUE=1;BBSP_FT_QOS_CAR=1;BBSP_FT_QOS_CLA=1;BBSP_FT_QOS_MIRROR=1;BBSP_FT_ACL=1;BBSP_FT_ALG=1;BBSP_FT_ARP=1;BBSP_FT_ARPPING=1;BBSP_FT_DOSFILTER=1;BBSP_FT_IPFILTERIN=1;BBSP_FT_IPFILTEROUT=1;BBSP_FT_MACFILTER=1;BBSP_FT_URLFIL=1;BBSP_FT_PING=1;BBSP_FT_DNS=1;BBSP_FT_DNS_CLIENT=1;BBSP_FT_DHCP_MAIN=1;BBSP_FT_DHCP_SLAVE=1;BBSP_FT_DHCP_HOST=1;BBSP_FT_DHCPOPTION=1;BBSP_FT_DHCP_CLIENT_OPTION=1;BBSP_FT_DHCPS_OPTION=1;BBSP_FT_DHCPS_COND_POOL=1;BBSP_FT_L3=1;BBSP_FT_L3_ALL=1;BBSP_FT_L3IGMP=1;BBSP_FT_WAN=1;BBSP_FT_WAN_CFG=1;BBSP_FT_SEC=1;BBSP_FT_RINGCHK=1;BBSP_FT_PORTAL=1;BBSP_FT_ONTACCESSCTL=1;BBSP_FT_PPPOE_EM=1;BBSP_FT_SNTP=1;BBSP_FT_BT=1
```

### 4.3 Enable AMP Features (Advanced Management)

```bash
set hardinfo value HW_AMP_FEATURE_COMMON=1;HW_AMP_FEATURE_WLAN=1
```

### 4.4 Enable Telnet WAN Access Feature

```bash
set hardinfo value FT_SSMP_TELNET_LAN_WAN=1
```

### 4.5 Enable Debug Features

```bash
set hardinfo value HW_FT_SSMP_LAN_TRANSPAREN=1;FT_SMART_BOARD=1;FT_OLT_UTC_TIME=1;HW_SSMP_FEATURE_CTRG=1;HW_FT_OSGI_JVM_FROM_VAR=1;SSMP_FT_DFX_ACS_UPGRADE=1
```

### 4.6 Complete Features Activation (All-in-One)

```bash
set hardinfo value HW_SSMP_FEATURE_WEB=1;HW_SSMP_FEATURE_USB=1;HW_SSMP_FEATURE_USBSTORAGE=1;HW_SSMP_FEATURE_DLNA=1;HW_SSMP_FEATURE_TR069=1;HW_SSMP_FEATURE_CFG_BACKUP=1;HW_SSMP_FEATURE_APM=1;BBSP_FT_FIREWALL=1;BBSP_FT_NAT=1;BBSP_FT_IPV6=1;BBSP_FT_ROUTE=1;BBSP_FT_UPNP_MAIN=1;BBSP_FT_DDNS_IP=1;BBSP_FT_DMZ_IP=1;BBSP_FT_PORTMAP_IP=1;BBSP_FT_IGMP_ENABLE=1;BBSP_FT_QOS_CFG=1;BBSP_FT_ACL=1;BBSP_FT_ALG=1;BBSP_FT_DOSFILTER=1;BBSP_FT_WAN=1;BBSP_FT_SEC=1;BBSP_FT_PING=1;BBSP_FT_DNS=1;BBSP_FT_DHCP_MAIN=1;BBSP_FT_L3=1;FT_SSMP_TELNET_LAN_WAN=1;HW_AMP_FEATURE_COMMON=1;HW_AMP_FEATURE_WLAN=1
```

---

## 5. CONFIGURE HARDWARE SPECIFICATIONS

### 5.1 Set Ethernet Ports

```bash
set hardinfo value eth_num=4;eth_type=1
```

**Values:**
- `eth_num` - Number of Ethernet ports (1-8, typically 4)
- `eth_type` - 0=FE (100Mbps), 1=GE (1Gbps)

### 5.2 Set WiFi/SSID Configuration

```bash
set hardinfo value ssid_num=8;wlan_num=2
```

**Values:**
- `ssid_num` - Number of SSIDs (1-16, typically 8)
- `wlan_num` - Number of WLAN interfaces (1-4)

### 5.3 Set USB Ports

```bash
set hardinfo value usb_num=2
```

### 5.4 Set PON Port

```bash
set hardinfo value pon_num=1
```

### 5.5 Set Product Type to HGU (Home Gateway Unit)

```bash
set hardinfo value pdt_type=HGU
```

**Product Types:**
- `HGU` - Home Gateway Unit (full features) **RECOMMENDED**
- `SFU` - Single Family Unit (basic)
- `MDU` - Multi-Dwelling Unit
- `MTU` - Multi-Tenant Unit

### 5.6 Complete Hardware Spec Command

```bash
set hardinfo value eth_num=4;eth_type=1;ssid_num=8;wlan_num=2;usb_num=2;pon_num=1;pdt_type=HGU
```

---

## 6. ENABLE DEBUG/DEVELOPER MODE

### 6.1 Create Debug Flag Files (from shell)

```bash
# Access shell first
su
shell

# Create debug flags
touch /mnt/jffs2/swm_debug
touch /mnt/jffs2/debugcheck
touch /etc/wap/DebugVersionFlag

# Reboot to apply
reboot
```

### 6.2 Enable SWM Debug via Hardinfo

```bash
set hardinfo value SSMP_SPEC_SWM_MODULESIZE=0
```

### 6.3 Enable Debug Upgrade

```bash
set hardinfo value HW_FEATURE_CHECKUPG_SPACE=0
```

---

## 7. ALLOW LOCAL FIRMWARE UPDATE

### 7.1 Disable Upgrade Space Check

```bash
set hardinfo value HW_FEATURE_CHECKUPG_SPACE=0
```

### 7.2 Enable Local Upgrade Feature

```bash
set hardinfo value SSMP_FT_DFX_ACS_UPGRADE=1
```

### 7.3 Set SWM Module Size for Local Files

```bash
set hardinfo value SSMP_SPEC_SWM_MODULESIZE=65536
```

---

## 8. CONFIGURE SECURITY & PERMISSIONS

### 8.1 Disable Password Encryption (Debug Mode)

```bash
set hardinfo value SSMP_SPEC_WEB_PWDENCRYPT=0
```

### 8.2 Set CLI User Config for Full Access

```bash
set hardinfo value SSMP_SPEC_CLI_USER_CFG=admin
```

### 8.3 Enable SSH Public Key Authentication

```bash
# From shell:
load ssh-pubkey
```

---

## 9. LANGUAGE CONFIGURATION

### 9.1 Set Multiple Languages

```bash
set hardinfo value &LANGUAGE=COMMON,DT_HUNGARY,DT_ENGLISH,DT_SPANISH
```

**Available Language Codes:**
- `COMMON` - Common/Default
- `DT_ENGLISH` - English
- `DT_SPANISH` - Spanish
- `DT_HUNGARY` - Hungarian
- `DT_FRENCH` - French
- `DT_GERMAN` - German
- `DT_ITALIAN` - Italian
- `DT_PORTUGUESE` - Portuguese
- `DT_RUSSIAN` - Russian
- `DT_CHINESE` - Chinese
- `DT_ARABIC` - Arabic

---

## 10. COMPLETE UNLOCK COMMANDS (COPY-PASTE READY)

### 10.1 Full CLI Unlock + Common Mode + All Features

```bash
set hardinfo value SSMP_SPEC_CLI_USERGRP=0x80004000;SSMP_SPEC_CLI_REMOTETELNET=1;SSMP_SPEC_CLI_REDLINEVERSION=1;customize_flag=COMMON;cfg_word=COMMON;usescene_flag=NOCHOOSE;custom_info=COMMON;SSMP_SPEC_WEB_FRAME=frame_XGPON;SSMP_SPEC_WEB_MENUXML=MenuXGPONAbroad.xml;SSMP_SPEC_WEB_PWDENCRYPT=0;HW_SSMP_FEATURE_WEB=1;HW_SSMP_FEATURE_USB=1;HW_SSMP_FEATURE_TR069=1;BBSP_FT_FIREWALL=1;BBSP_FT_NAT=1;BBSP_FT_IPV6=1;BBSP_FT_ROUTE=1;BBSP_FT_UPNP_MAIN=1;BBSP_FT_WAN=1;BBSP_FT_SEC=1;FT_SSMP_TELNET_LAN_WAN=1;pdt_type=HGU;&LANGUAGE=COMMON,DT_ENGLISH
```

### 10.2 Developer Mode Activation

```bash
set hardinfo value SSMP_SPEC_CLI_REDLINEVERSION=1;SSMP_SPEC_WEB_PWDENCRYPT=0;HW_FEATURE_CHECKUPG_SPACE=0;SSMP_FT_DFX_ACS_UPGRADE=1;HW_FT_OSGI_JVM_FROM_VAR=1;FT_SMART_BOARD=1
```

### 10.3 Maximum Privileges (Admin + Debug + Advanced)

```bash
set hardinfo value SSMP_SPEC_CLI_USERGRP=0x80004000;SSMP_SPEC_CLI_REMOTETELNET=1;SSMP_SPEC_CLI_REDLINEVERSION=1;SSMP_SPEC_WEB_FRAME=frame_XGPON;SSMP_SPEC_WEB_MENUXML=MenuXGPONAbroad.xml
```

### 10.4 Network Features Full Enable

```bash
set hardinfo value BBSP_FT_FIREWALL=1;BBSP_FT_NAT=1;BBSP_FT_IPV6=1;BBSP_FT_IPV6_CORE=1;BBSP_FT_IPV6_ROUTE=1;BBSP_FT_ROUTE=1;BBSP_FT_ROUTE_STATIC=1;BBSP_FT_ROUTE_POLICY=1;BBSP_FT_UPNP_MAIN=1;BBSP_FT_DDNS_IP=1;BBSP_FT_DMZ_IP=1;BBSP_FT_PORTMAP_IP=1;BBSP_FT_PORTTRIGGER_IP=1;BBSP_FT_IGMP_ENABLE=1;BBSP_FT_QOS_CFG=1;BBSP_FT_ACL=1;BBSP_FT_ALG=1;BBSP_FT_WAN=1;BBSP_FT_WAN_CFG=1;BBSP_FT_L3=1;BBSP_FT_L3_ALL=1
```

### 10.5 WiFi/AMP Full Features

```bash
set hardinfo value HW_AMP_FEATURE_COMMON=1;HW_AMP_FEATURE_WLAN=1;HW_SSMP_FEATURE_APM=1;HW_SSMP_FEATURE_APMLAN=1;HW_SSMP_FEATURE_APMWLAN=1;HW_SSMP_FEATURE_APMVOICE=1;HW_SSMP_FEATURE_WIFIAP_USABLE=1
```

---

## 11. SHELL UNLOCK PROCEDURE

### 11.1 Step-by-Step Shell Access

```bash
# 1. Connect via telnet
telnet 192.168.100.1

# 2. Login
Login: root
Password: <your_password>

# 3. Enter SU mode
su
# Enter SU password (usually same as login password)

# 4. Access shell
shell

# 5. You now have BusyBox shell with limited commands
# Available: ls, ifconfig, iwconfig, iwpriv, killall, etc.

# 6. Create debug flags for full access
touch /mnt/jffs2/swm_debug
touch /mnt/jffs2/debugcheck
touch /etc/wap/DebugVersionFlag

# 7. Reload features
echo "Reload" > /proc/wap_proc/feature

# 8. Reboot for full effect
reboot
```

### 11.2 Post-Reboot Verification

After reboot, reconnect and verify:

```bash
# Check current hardinfo
display equip hardinfo

# Should show your configured values
# Example output:
# SSMP_SPEC_CLI_USERGRP=0x80004000
# customize_flag=COMMON
# SSMP_SPEC_WEB_FRAME=frame_XGPON
# etc.
```

---

## 12. ADVANCED CONFIGURATION COMMANDS

### 12.1 Configure BBSP Specifications

```bash
set hardinfo value BBSP_SPEC_NAT_PORTMAPNUM=64;BBSP_SPEC_NAT_PORTTRIGGERNUM=64;BBSP_SPEC_IPV4_ROUTE_MAXNUM=256;BBSP_SPEC_IPV6_ROUTE_MAXNUM=64;BBSP_SPEC_QOS_QUEUENUM=256;BBSP_SPEC_FWD_SESSIONNUM=32000;BBSP_SPEC_NAPT_ITEM_NUM=16384
```

### 12.2 Configure WiFi Capabilities

```bash
set hardinfo value wifi_low_cap=37748737;wifi_high_cap=33554432
```

**Values:**
- `wifi_low_cap` - 2.4GHz frequency capability bitmask
- `wifi_high_cap` - 5GHz frequency capability bitmask

### 12.3 Configure Product Information

```bash
set hardinfo value proc_name=EG8145V5;proc_desc=Home Gateway HGU;vendor_id=HWTC;manufacture=Huawei Technologies Co., Ltd
```

### 12.4 Configure Feature File Paths

```bash
set hardinfo value featurecfg_hard=/mnt/jffs2/hw_hardinfo_feature;spec_hard=/mnt/jffs2/hw_hardinfo_spec;feature_cust=/etc/wap/cust_feature.cfg
```

---

## 13. TROUBLESHOOTING & VERIFICATION

### 13.1 Query Current Values

```bash
# Query a specific spec
display equip hardinfo

# Or use GetSpec (if available)
GetSpec SSMP_SPEC_CLI_USERGRP

# Query a specific feature
GetFeature HW_SSMP_FEATURE_WEB
```

### 13.2 Check Feature Status

```bash
# List all enabled features
display current-configuration

# Check specific subsystem
display sysinfo
display version
display deviceInfo
```

### 13.3 Reload Configuration

```bash
# Reload features without reboot
echo "Reload" > /proc/wap_proc/feature

# Restart CLI daemon
kill -HUP $(pidof clid)

# Full reboot (recommended after major changes)
reboot
```

---

## 14. IMPORTANT NOTES

### 14.1 Command Execution Order

1. **Always enter SU mode first** - `su` command
2. **Apply hardinfo changes** - `set hardinfo value ...`
3. **Create debug flags** (if needed) - `touch /mnt/jffs2/swm_debug`
4. **Reload or reboot** - `reboot` for full effect

### 14.2 Persistence

- Changes to hardinfo are **persistent** across reboots
- Debug flag files (`/mnt/jffs2/swm_debug`) are **persistent**
- Configuration is stored in `/mnt/jffs2/hw_hardinfo_spec` and `hw_hardinfo_feature`

### 14.3 Backup Before Changes

```bash
# Backup current configuration
cp /mnt/jffs2/hw_hardinfo_spec /mnt/jffs2/hw_hardinfo_spec.backup
cp /mnt/jffs2/hw_hardinfo_feature /mnt/jffs2/hw_hardinfo_feature.backup
cp /mnt/jffs2/hw_boardinfo /mnt/jffs2/hw_boardinfo.backup
```

### 14.4 Factory Reset (If Needed)

```bash
# From CLI (SU mode):
restore default configuration

# Or from shell:
/restorehwmode.sh

# Or manual:
rm /mnt/jffs2/hw_hardinfo_spec
rm /mnt/jffs2/hw_hardinfo_feature
reboot
```

---

## 15. SPEC VALUES REFERENCE TABLE

| Spec Name | Type | Description | Example Values |
|-----------|------|-------------|----------------|
| `SSMP_SPEC_CLI_USERGRP` | hex | CLI user group | `0x80004000` |
| `SSMP_SPEC_CLI_REMOTETELNET` | uint | Remote telnet enable | `0`, `1` |
| `SSMP_SPEC_CLI_REDLINEVERSION` | uint | Redline/debug version | `0`, `1` |
| `SSMP_SPEC_WEB_FRAME` | string | Web frame | `frame_XGPON` |
| `SSMP_SPEC_WEB_MENUXML` | string | Web menu XML | `MenuXGPONAbroad.xml` |
| `SSMP_SPEC_WEB_PWDENCRYPT` | uint | Password encryption | `0`, `1`, `3` |
| `SSMP_SPEC_WEB_OUTCHANGEPORT` | uint | Web external port | `80`, `443`, `8080` |
| `customize_flag` | string | Customization flag | `COMMON`, `TDE2` |
| `cfg_word` | string | Configuration word | `COMMON`, `TDE2` |
| `usescene_flag` | string | Use scene flag | `NOCHOOSE`, `EeAa` |
| `custom_info` | string | Custom info | `COMMON` |
| `pdt_type` | string | Product type | `HGU`, `SFU`, `MDU` |
| `eth_num` | uint | Ethernet port count | `1`-`8` |
| `eth_type` | uint | Ethernet type | `0`=FE, `1`=GE |
| `ssid_num` | uint | SSID count | `1`-`16` |
| `wlan_num` | uint | WLAN interface count | `1`-`4` |
| `usb_num` | uint | USB port count | `0`-`4` |
| `pon_num` | uint | PON port count | `1`-`2` |

---

*Generated from firmware analysis: EG8145V5-V500R022C00SPC340B019, HN8145XR-V500R022C10SPC160*
*Architecture: ARM 32-bit, musl libc, BusyBox 1.32.1*
