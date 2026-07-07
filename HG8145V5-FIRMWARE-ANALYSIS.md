# Huawei HG8145V5-R020-212 Firmware Analysis Report

**Firmware**: HG8145V5-R020-212.bin (50MB)  
**Source**: https://github.com/educolabs/Huawei/releases/download/v1/HG8145V5-R020-212.bin  
**Magic Header**: HWNP  
**Filesystem**: 3x SquashFS partitions  
**Analysis Date**: 2026-07-07

---

## Table of Contents

1. [Firmware Structure](#1-firmware-structure)
2. [Frame Directories Analysis](#2-frame-directories-analysis)
3. [Menu XML Analysis](#3-menu-xml-analysis)
4. [Debug & Developer Options](#4-debug--developer-options)
5. [Security Findings](#5-security-findings)
6. [ISP/Operator Coverage](#6-ispoperator-coverage)
7. [Architecture Tiers](#7-architecture-tiers)

---

## 1. Firmware Structure

The firmware contains three SquashFS partitions:

| Partition | Offset | Content |
|-----------|--------|---------|
| sq0 | 0x28adee | (secondary) |
| **sq1** | **0x2861dae** | **HTML web interface (main target)** |
| sq2 | 0x2cf90d9 | (tertiary) |

The primary web interface resides in `sq1/html/` with two key directories:
- `menu/` — 111 XML menu definition files
- 13 `frame_*` directories — ISP-specific UI skins and logic

---

## 2. Frame Directories Analysis

### 2.1 Overview

| Frame Directory | ISP/Region | Files | Architecture Tier |
|---|---|---|---|
| `frame_Arabic` | Mobily/Bayanat (Saudi Arabia) | 51 | Tier 1 (Classic GPON) |
| `frame_IraqO3` | O3 Telecom (Iraq) | 50 | Tier 2 (Enhanced GPON) |
| `frame_Stc` | STC (Saudi Arabia) | 57 | Tier 1 (Classic GPON) |
| `frame_XGPON` | Multi-ISP XG-PON | ~310 | Tier 3 (Modern XG-PON) |
| `frame_argentina` | Personal (Argentina) | 30 | Tier 1 (Classic GPON) |
| `frame_du` | du (UAE) | 30 | Tier 2 (Enhanced GPON) |
| `frame_huawei` | Generic Huawei | ~175 | Tier 2 (Enhanced GPON) |
| `frame_pccw` | PCCW (Hong Kong) | 28 | Tier 1 (Classic GPON) |
| `frame_qtel` | Q-Tel/Ooredoo (Qatar) | 47 | Tier 1 (Classic GPON) |
| `frame_telmex` | Telmex (Mexico) | 38 | Tier 2 (Enhanced GPON) |
| `frame_xgponglobe` | Globe XG-PON (Philippines) | ~130 | Tier 3 (Modern XG-PON) |
| `frame_zain` | Zain (Saudi/Kuwait) | 60 | Tier 2 (Enhanced GPON) |
| `FrameAISAP` | AIS Fibre (Thailand) | ~210 | Tier 3 (Modern XG-PON) |

### 2.2 Common Files (Present in ALL Frames)

| File | Purpose |
|------|---------|
| `index.asp` | Entry point redirect |
| `login.asp` | Login page |
| `refresh.asp` | Session refresh |
| `relocation.asp` | Redirect handler |
| `asp/GetRandCount.asp` | CSRF token generation |
| `asp/getMenuArray.asp` | Menu structure provider |
| `asp/getProductName.asp` | Product name retrieval |
| `Cusjs/InitFormCus.js` | Custom form initialization |
| `images/scale1.gif` | Loading indicator |
| `images/scale2.gif` | Loading indicator |

### 2.3 Unique Files & Features Per Frame

#### frame_Arabic (Mobily - Saudi Arabia)
- Arabic RTL support, English/Arabic language switcher
- Copyright: "Bayanat Al Oula (subsidiary of Etihad Etisalat)"
- Custom backgrounds: `Mobily_AR_bg.jpg`, `mobily_EN_bg.jpg`

#### frame_IraqO3 (O3 Telecom - Iraq)
- **`genaral.asp`** — 1,648-line monolithic device info page (misspelled filename)
- **`preframe.asp`**, **`preindex.asp`** — Pre-login pages
- Social media links (Facebook, YouTube), "Call us 066186"
- External "Recharge" menu linking to `http://ftthportal.o3-telecom.com`
- Hides "Home" and "Advanced" menus from navigation
- `.gitignore` file left in production firmware

#### frame_Stc (STC - Saudi Arabia)
- Arabic+English bilingual
- APP download QR code
- STC/Atheeb branding variants with custom logos

#### frame_XGPON (Multi-ISP XG-PON — Largest Frame)
- **`portal/`** directory with 8 portal configuration ASPs
- **`frameaspdes/`** with 10 language resource files
- Sub-ISP CSS variants for 20+ ISPs: Singtel, Telmex, Vodacom, OrangeMT, Claro Brazil, PLDT, Antel, MarocTelecom, Telecentro, and more
- **`login_singtel.asp`** — Duplicate Singtel-specific login
- **`successpwd.html`**, **`errpwd.html`** — Static password change pages
- CAPTCHA support for TOT/TRUERG ISPs
- Password modify overlay and WiFi SSID change for PLDT
- Pre-filled credentials for ANTEL

#### frame_argentina (Personal - Argentina)
- **`pindex.asp`** — Password-less auto-login bypass page
- Simplest frame with fewest files
- English+Chinese support only
- Uses `LogoutAdminFunc()` instead of `logoutfunc()`

#### frame_du (du - UAE)
- **Custom font**: `fonts/DuCoHeadline400.woff`
- **`redirect_to_wizard.asp`** — Setup wizard redirect
- CfgMode support: ORO ("Internet Box 1000"), Basic/Expert mode switching
- Social media footer links (Facebook, Twitter, LinkedIn, YouTube)
- Multi-language: English, Portuguese, Japanese, Spanish, Russian

#### frame_huawei (Generic Huawei — Most Versatile)
- **Activation/Registration system**: `activation.asp`, `register.asp`, `loidreg.asp`, `ponpwd.asp`
- **Setup wizards**: `ros_setup_wizard.asp`, `ros_simple_wizard.asp`, `ros_trouble_wizard.asp`
- **Firmware update**: `updateConfig.asp`, `updateNote.asp`, `updatePopWindow.asp`
- **Triple-play auth**: `trip_3bb.asp`, `trip_RMNT.asp`, `tripacs.asp`, `tripleTAuthAcs.asp`
- **ISP-specific pages**: `viettel_internet_down.asp`, `viettel_pon_loss.asp`, `diagnosis_clarodr.asp`
- CfgMode sub-CSS: CNT, CNT2, GLOBE, ORO, ROSTELECOM, SINGTEL
- 7 languages: English, Portuguese, Japanese, Spanish, Russian, Chinese, Turkish
- `GetShortStr()` for truncating long menu names
- SONET flag support, Beltelecom custom password, TeliaLT ONT Authentication

#### frame_pccw (PCCW - Hong Kong)
- **`PccwShowProc.asp`**, **`pccw.asp`** — PCCW-specific processing pages
- **`asp/ontOnlineStatus.asp`** — Online status checker
- 4 languages: English, Portuguese, Japanese, Spanish

#### frame_qtel (Q-Tel/Ooredoo - Qatar)
- Most heavily branded login: custom Q-Tel background, buttons, footer
- Custom images: `banner_qtel.gif/jpg`, `login_background.gif/jpg`, `login_button.gif/jpg`

#### frame_telmex (Telmex - Mexico)
- **`telmexssh.asp`** — SSH/firewall configuration page
- **`remoteandvoip.asp`** — Remote access + VoIP configuration
- **`remoteaccess/access.asp`** — Remote access control
- **`asp/secondLogin.asp`** — Second login system
- Real-time PON/PPPoE status display in sidebar (5-second polling)
- CfgMode variants: TELMEXACCESS, TELMEXACCESSNV, TELMEXVULA, TELMEXRESALE
- Shows WPA KEY as default password hint after lockout
- `TELMEX = true` hardcoded flag

#### frame_xgponglobe (Globe XG-PON - Philippines)
- **Multiple login variants**: `login1.asp`, `login2.asp`, `login_1.asp`
- **`CustomApp/test.asp`** — Developer placeholder with Chinese instructions for AMP, BBSP, voice, SSMP teams
- **`public/public_info_page.asp`** — Public info page
- Login page auto-refreshes every 6 minutes
- `IsNeedToInfoPage()` redirect check

#### frame_zain (Zain - Saudi Arabia/Kuwait)
- **`login_cut.asp`** — Session cutoff page
- **Activation system**: `activation.asp`, `activation_fail.asp`, `activation_success.asp`
- **Firmware update**: `updateConfig.asp`, `updateNote.asp`, `updatePopWindow.asp`
- Title prefixed with "Zain " + productName
- Pink hover color (`#e0218a`) for logout

#### FrameAISAP (AIS Fibre - Thailand)
- **`landingpage.asp`** — Pre-login landing page with AIS branding
- **CAPTCHA support** (`Captcha_enable`)
- **ISP locking** (`FT_SSMP_ISP_LOCKING`), device lock status check
- **`UpdateFlag`** for password-same-as-gateway indicator
- Bootstrap-based modern login UI
- Portal directory with full portal configuration
- Custom images: `aisfibre.jpg`, `img_logo_ais_fibre.jpg`, `background.jpg`

### 2.4 Frame Feature Comparison Matrix

| Feature | Arabic | IraqO3 | Stc | XGPON | argentina | du | huawei | pccw | qtel | telmex | xgponglobe | zain | AISAP |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| CfgMode support | - | Y | - | - | - | Y | Y | - | - | Y | - | Y | - |
| Basic/Expert mode | - | - | - | - | - | Y | Y | - | - | - | - | Y | - |
| Multi-language logout | EN/AR | EN | EN/AR | N/A | EN/CN | 5 lang | 7 lang | 4 lang | EN/CN | EN/ES | N/A | 5 lang | N/A |
| APP QR download | - | - | Y | - | - | Y | Y | Y | - | - | - | Y | - |
| Password force change | Y | Y | Y | N/A | - | Y | Y | Y | Y | - | N/A | Y | N/A |
| Menu filtering | - | Y | - | - | - | - | - | - | - | Y | - | - | - |
| PON/PPPoE status | - | - | - | - | - | - | - | - | - | Y | - | - | - |
| Second login system | - | - | - | - | - | - | - | - | - | Y | - | - | - |
| Social media links | - | Y | - | - | - | Y | - | - | - | - | - | - | - |
| CAPTCHA | - | - | - | Y | - | Y | Y | - | - | - | - | - | Y |
| Auto-login bypass | - | - | - | - | Y | - | - | - | - | - | - | - | - |
| ISP locking | - | - | - | - | - | - | - | - | - | - | - | - | Y |
| Activation/Registration | - | - | - | - | - | - | Y | - | - | - | - | Y | - |
| Setup wizards | - | - | - | - | - | - | Y | - | - | - | - | - | - |

### 2.5 Login Page Comparison

| Feature | Arabic | IraqO3 | Stc | XGPON | argentina | du | huawei | pccw | qtel | telmex | xgponglobe | zain | AISAP |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Languages | EN/AR | EN | EN/AR | 10+ | EN/CN | 5 | 7 | 4 | EN/CN | EN/ES | 6+ | 5 | 7 |
| i18n system | Inline | Inline | Inline | ssmpdes.js | Inline | Inline | Inline | Inline | Inline | Inline | ssmpdes.js | Inline | ssmpdes.js |
| Register button | - | - | - | - | - | - | Y | - | - | - | - | - | - |
| Auto-login | - | - | - | - | Y | - | - | - | - | - | - | - | - |
| Hint password | - | - | - | - | - | - | - | - | - | Y | - | - | - |
| Pre-filled creds | - | - | - | Y | - | - | - | - | - | Y | - | - | - |
| Default pwd check | - | - | - | - | - | - | Y | - | - | Y | - | - | - |
| Custom branding | Mobily | O3 | STC | Multi | Huawei | du | Multi | PCCW | QTel | Telmex | Globe | Zain | AIS Fibre |

---

## 3. Menu XML Analysis

### 3.1 Overview

- **Total menu files**: 111 XML files
- **Total unique menu configurations**: 111 ISP/region variants
- **Identical pair**: `menu.xml` and `MenuAbroad.xml` (same content, 566 items)

### 3.2 Menu Size Ranking — Top 20 (Most Feature-Rich)

| Rank | File | Items | Lines |
|------|------|-------|-------|
| 1 | **MenuAbroad.xml** | 566 | 632 |
| 2 | **menu.xml** (base) | 566 | 632 |
| 3 | **MenuRussian.xml** | 531 | 594 |
| 4 | **MenuRussianKaz.xml** | 526 | 589 |
| 5 | **MenuCtm.xml** | 525 | 591 |
| 6 | **MenuBrazClaro.xml** | 517 | 591 |
| 7 | **MenuSmartAntel.xml** | 510 | 619 |
| 8 | **MenuSTC.xml** | 466 | 529 |
| 9 | **MenuPtvdf.xml** | 457 | 517 |
| 10 | **MenuAntel.xml** | 438 | 525 |
| 11 | **MenuE8cAwifi.xml** | 428 | 534 |
| 12 | **MenuTelecentro.xml** | 429 | 497 |
| 13 | **MenuMobily.xml** | 427 | 485 |
| 14 | **MenuSonet.xml** | 426 | 483 |
| 15 | **MenuE8c.xml** | 416 | 514 |
| 16 | **MenuClarodr.xml** | 410 | 462 |
| 17 | **MenuCmcc.xml** | 408 | 499 |
| 18 | **MenuClaro.xml** | 407 | 456 |
| 19 | **MenuCmccRmsReg.xml** | 404 | 495 |
| 20 | **MenuBjct.xml** | 402 | 496 |

### 3.3 Menu Size Ranking — Bottom 10 (Most Restrictive)

| Rank | File | Items | Lines |
|------|------|-------|-------|
| 1 | **menulankaGuide.xml** | 25 | 35 |
| 2 | **MenuViettelGuide.xml** | 27 | 37 |
| 3 | **MenuTdeinstall.xml** | 28 | 41 |
| 4 | **MenuBellTelus.xml** | 29 | 42 |
| 5 | **MenuPtvdfb.xml** | 132 | 152 |
| 6 | **MenuPt.xml** | 151 | 171 |
| 7 | **MenuBjcu.xml** | 165 | 205 |
| 8 | **MenuDt.xml** | 174 | 194 |
| 9 | **MenuSmartAbroadAP.xml** | 209 | 228 |
| 10 | **MenuSmartChinaAP.xml** | 209 | 228 |

### 3.4 Feature Comparison Matrix (Key Menus)

| Feature | menu.xml | menuglobe | MenuAbroad | MenuChina | MenuTelmex | MenuTelmexAccess | MenuTmczstForAdmin | MenuRemote | MenuRemoteVoip | MenuXGPONAbroad | MenuXGPONChinaRT | MenuSmartAbroad | MenuTriplet | MenuTriplet2wifi6 | MenuViettel | MenuSTC | MenuBharti |
|---------|----------|-----------|------------|-----------|------------|-----------------|-------------------|------------|---------------|----------------|-----------------|----------------|-------------|------------------|-------------|---------|------------|
| VoIP/Voice | Y | Y | Y | Y | Y(info) | Y | Y | **N** | Y | Y | Y | Y | Y | Y | Y | Y | Y |
| WiFi Advanced | Y | Y | Y | Y | Y(basic) | Y(basic) | Y | Y(basic) | Y(basic) | Y | Y | Y | Y | Y | Y | Y | Y |
| Firewall/Security | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y |
| TR-069/CWMP | Y | Y | Y | Y | Y(status) | Y | Y | Y | Y | Y | Y | Y | **N** | **N** | **N** | Y | Y |
| QoS | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y |
| USB/Storage | Y | Y | Y | Y | Y | Y | Y | Y | Y | **N** | **N** | Y | Y | Y | Y | Y | Y |
| Route/NAT | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y | Y |
| Debug Log | Y | Y | Y | Y | **N** | **N** | Y | **N** | **N** | Y | Y | Y | Y | Y | Y | **N** | Y |
| Guest WiFi | Y | **N** | Y | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | Y |
| 802.1X | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **Y** | **N** | **N** | **N** | **N** | **N** |
| PoE Config | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **Y** | **N** | **N** | **N** | **N** | **N** |
| LTE/Mobile | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **Y** | **N** | **N** | **N** | **N** | **N** |
| PowerCube | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **Y** | **N** | **N** | **N** | **N** | **N** |
| LLDP | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **Y** | **N** | **N** | **N** | **N** | **N** |
| EAI Config | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **Y** | **N** | **N** | **N** | **N** | **N** |
| Bundle/OSGi | Y | **N** | Y | Y | **N** | **N** | Y | **N** | **N** | Y | **N** | Y | **N** | **N** | Y | Y | Y |
| PCP | Y | Y | Y | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | Y | **N** | **N** | Y | **N** | Y |
| IPv6 Firewall | **N** | Y | **N** | **N** | **N** | **N** | Y | **N** | **N** | **N** | Y | Y | Y | Y | Y | **N** | Y |
| Internet Access Ctrl | Y | **N** | Y | Y | **N** | **N** | **N** | **N** | **N** | **N** | **N** | Y | **N** | **N** | **N** | **N** | **N** |
| DECT | **N** | **N** | **N** | **N** | **N** | **N** | Y | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** | **N** |
| Speedtest | Y | **N** | Y | Y | **N** | **N** | Y | **N** | **N** | Y | **N** | Y | **N** | **N** | **N** | Y | **N** |

### 3.5 Most Permissive Menus (Ranked)

| Rank | Menu | Notable Unique Features |
|------|------|------------------------|
| 1 | **MenuSmartAbroad.xml** | Most features: 802.1X, PoE, LTE/Mobile, PowerCube, LLDP, EAI, PCP, IPv6 FW, Internet Access Ctrl, Debug Log, Speedtest |
| 2 | **menu.xml / MenuAbroad.xml** | Full base: Debug Log, Bundle/OSGi, Route/NAT, QoS, USB, VoIP, Guest WiFi, PCP, Internet Access Ctrl |
| 3 | **MenuTmczstForAdmin/All/User.xml** | Smart UI with icons, DECT, Debug Log, Speedtest, TR-069, VoIP, Bundle/OSGi |
| 4 | **MenuXGPONAbroad.xml** | XGPON variant: Debug Log, Speedtest, Bundle/OSGi, TR-069, VoIP |
| 5 | **MenuViettel.xml** | Debug Log, DHCP/PPPoE Emulation, 802.1ag, Ringcheck, Bundle/OSGi |
| 6 | **MenuBharti.xml** | Debug Log, DHCP/PPPoE Emulation, 802.1ag, Guest WiFi, Bundle/OSGi |
| 7 | **MenuChina.xml** | Debug Log, VR settings, L2 QoS, Bundle/OSGi, GXB Monitor |
| 8 | **MenuSTC.xml** | TR-069, Speedtest, Bundle/OSGi, VoIP, QoS (no Debug Log) |
| 9 | **menuglobe.xml** | IPv6 Firewall, VoIP, Debug Log, QoS (no USB, no Guest WiFi) |
| 10 | **MenuTriplet.xml** | VoIP Call Log, Debug Log, Bundle/OSGi (no TR-069 page) |

### 3.6 Most Restrictive Menus (Ranked)

| Rank | Menu | Restrictions |
|------|------|-------------|
| 1 | **menulankaGuide.xml** | 25 items — Guide-only, no configuration |
| 2 | **MenuViettelGuide.xml** | 27 items — Guide-only, no configuration |
| 3 | **MenuTdeinstall.xml** | 28 items — Installation-only minimal menu |
| 4 | **MenuBellTelus.xml** | 29 items — No VoIP, debug, USB, QoS |
| 5 | **MenuPtvdfb.xml** | 132 items — Reduced feature set |
| 6 | **MenuPt.xml** | 151 items — No VoIP, debug, USB, QoS, TR-069 |
| 7 | **MenuBjcu.xml** | 165 items — No WiFi advanced, debug, VoIP, USB |
| 8 | **MenuDt.xml** | 174 items — No VoIP, debug, TR-069, USB |
| 9 | **MenuSmartAbroadAP.xml** | 209 items — AP mode: no VoIP, debug, TR-069, Bundle |
| 10 | **MenuSmartChinaAP.xml** | 209 items — AP mode: no VoIP, debug, TR-069, Bundle |

---

## 4. Debug & Developer Options

### 4.1 Debug Features in Menus

~85 of 111 menu files contain Debug Log pages.

| Debug Feature | Present In |
|---------------|-----------|
| **Debug Log** | menu.xml, MenuAbroad, menuglobe, MenuChina, MenuTmczst*, MenuSmartAbroad, MenuXGPON*, MenuViettel, MenuBharti, MenuTriplet*, and ~75 others |
| **Fault Info Collection** | Same as Debug Log (always paired) |
| **DHCP/PPPoE Emulation** | menu.xml, MenuAbroad, MenuChina, MenuViettel, MenuBharti |
| **802.1ag/Y.1731** | menu.xml, MenuAbroad, MenuChina, MenuViettel, MenuBharti |
| **Remote Mirror** | menu.xml, MenuAbroad, MenuTmczst*, MenuSmartAbroad, MenuXGPON* |
| **Ringcheck** | menu.xml, MenuAbroad, MenuViettel |
| **LTE Log Collection** | MenuSmartAbroad only |
| **Segment Speedtest** | MenuTmczst*, MenuSTC, MenuSmartAbroad, MenuXGPONAbroad |

**Menus WITHOUT Debug Log**: MenuTelmex, MenuTelmexAccess, MenuRemote, MenuRemoteVoip, and ~20 minimal/guide menus.

### 4.2 Developer Artifacts Found in Frames

| File | Frame | Description |
|------|-------|-------------|
| `CustomApp/test.asp` | xgponglobe | Developer placeholder with Chinese instructions for AMP, BBSP, voice, SSMP team assignments |
| `CustomApp/.gitignore` | IraqO3 | Git ignore file accidentally shipped in production firmware |
| `genaral.asp` | IraqO3 | Misspelled filename ("genaral" vs "general"), 1,648-line monolithic page |
| `huaweidefault.html` | huawei | Static default Huawei page |
| `wifi_parameters.html` | huawei | Static HTML WiFi parameters page |
| `successpwd.html` | XGPON | Static password change success page |
| `errpwd.html` | XGPON, FrameAISAP | Static password error page |
| `login_singtel.asp` | XGPON | Duplicate Singtel-specific login (alongside main login.asp) |
| `login1.asp`, `login2.asp`, `login_1.asp` | xgponglobe | Multiple redundant login page variants |
| `pindex.asp` | argentina | Password-less auto-login bypass page |
| `Cusjs/mainpagesrc_new.asp` | FrameAISAP | New version of mainpage source alongside original |
| `CustomApp/mainpage_bin6.asp` | FrameAISAP | Binary/hex variant of mainpage |

---

## 5. Security Findings

### 5.1 Authentication & Access

| Finding | Severity | Location |
|---------|----------|----------|
| **Password-less auto-login** | HIGH | `frame_argentina/pindex.asp` |
| **WPA KEY shown as password hint** | MEDIUM | `frame_telmex/login.asp` (after lockout) |
| **Second login system** | MEDIUM | `frame_telmex/asp/secondLogin.asp` |
| **SSH configuration page** | MEDIUM | `frame_telmex/telmexssh.asp` |
| **Pre-filled credentials** | LOW | `frame_XGPON/login.asp` (ANTEL), `frame_telmex/login.asp` |
| **Default password check** | INFO | `frame_huawei/login.asp`, `frame_telmex/login.asp` |

### 5.2 Developer/Debug Exposure

| Finding | Severity | Location |
|---------|----------|----------|
| **Debug Log viewer in ~85 menus** | MEDIUM | Most menu XML files |
| **DHCP/PPPoE Emulation** | MEDIUM | menu.xml, MenuAbroad, MenuChina, MenuViettel, MenuBharti |
| **Remote Mirror capability** | MEDIUM | menu.xml, MenuAbroad, MenuTmczst*, MenuSmartAbroad, MenuXGPON* |
| **Developer test page** | LOW | `frame_xgponglobe/CustomApp/test.asp` |
| **`.gitignore` in production** | LOW | `frame_IraqO3/CustomApp/.gitignore` |

### 5.3 ISP Locking & Control

| Finding | Location |
|---------|----------|
| **ISP Locking feature** (`FT_SSMP_ISP_LOCKING`) | FrameAISAP |
| **Device lock status check** | FrameAISAP |
| **Menu filtering** (hides Home/Advanced) | frame_IraqO3, frame_telmex |
| **External recharge link** to `http://ftthportal.o3-telecom.com` | frame_IraqO3 |

---

## 6. ISP/Operator Coverage

### 6.1 Complete ISP List (from CfgMode values and frame analysis)

| ISP/Operator | Country | Frame | Menu(s) |
|---|---|---|---|
| Mobily/Bayanat Al Oula | Saudi Arabia | frame_Arabic | MenuMobily |
| O3 Telecom | Iraq | frame_IraqO3 | — |
| STC (Saudi Telecom) | Saudi Arabia | frame_Stc | MenuSTC |
| Singtel | Singapore | frame_XGPON | — |
| Telmex | Mexico | frame_telmex | MenuTelmex, MenuTelmexAccess |
| Vodacom | South Africa | frame_XGPON | — |
| OrangeMT | — | frame_XGPON | — |
| Claro | Brazil | frame_XGPON | MenuBrazClaro, MenuClaro, MenuClarodr |
| PLDT | Philippines | frame_XGPON | — |
| Antel | Uruguay | frame_XGPON | MenuAntel, MenuSmartAntel |
| Maroc Telecom | Morocco | frame_XGPON | — |
| Telecentro | Argentina | frame_XGPON | MenuTelecentro |
| Paraguay PSN | Paraguay | frame_XGPON | — |
| MyTime | — | frame_XGPON | — |
| True | Thailand | frame_XGPON | — |
| Umniah | Jordan | frame_XGPON | — |
| Oman ONT | Oman | frame_XGPON | — |
| Awasr | — | frame_XGPON | — |
| DNZ Telecom | — | frame_XGPON | — |
| TM | Malaysia | frame_XGPON | — |
| TTNET | Turkey | frame_XGPON | — |
| TalkTalk | UK | frame_XGPON | — |
| CMHK | Hong Kong | frame_XGPON | — |
| Personal | Argentina | frame_argentina | — |
| du | UAE | frame_du | — |
| PCCW/HKT | Hong Kong | frame_pccw | — |
| Q-Tel/Ooredoo | Qatar | frame_qtel | — |
| Globe Telecom | Philippines | frame_xgponglobe | menuglobe |
| Zain | Saudi Arabia/Kuwait | frame_zain | — |
| AIS Fibre | Thailand | FrameAISAP | — |
| Viettel | Vietnam | — | MenuViettel, MenuViettelGuide |
| Bharti Airtel | India | — | MenuBharti |
| China Mobile (CMCC) | China | — | MenuCmcc, MenuCmccRmsReg |
| China Telecom (CTM) | China | — | MenuCtm |
| SONET | — | — | MenuSonet |
| Rostelecom | Russia | — | MenuRussian, MenuRussianKaz |
| Bell/Telus | Canada | — | MenuBellTelus |
| Deutsche Telekom | Germany | — | MenuDt |
| Portugal Telecom | Portugal | — | MenuPt |
| Lanka Bell | Sri Lanka | — | menulankaGuide |
| T-Deinstall | — | — | MenuTdeinstall |

### 6.2 Language Support Summary

| Languages | Frames |
|-----------|--------|
| English + Arabic | Arabic, Stc |
| English only | IraqO3 |
| English + Chinese | argentina, qtel |
| English + Spanish | telmex |
| 4 languages (EN/PT/JP/ES) | pccw |
| 5 languages (EN/PT/JP/ES/RU) | du, xgponglobe, zain |
| 7 languages (EN/PT/JP/ES/RU/CN/TR) | huawei |
| 7+ languages (with Thai) | FrameAISAP |
| 10+ languages (dynamic loading) | XGPON |

---

## 7. Architecture Tiers

### Tier 1: Classic GPON (Simple)
**Frames**: Arabic, Argentina, PCCW, QTel, STC  
**Characteristics**:
- Traditional `frame.asp` with inline menu rendering
- 2-3 languages maximum
- Minimal ISP customization
- No CfgMode support
- No Basic/Expert mode switching

### Tier 2: Enhanced GPON (Complex)
**Frames**: du, huawei, IraqO3, Telmex, Zain  
**Characteristics**:
- Extended `frame.asp` with CfgMode support
- Basic/Expert mode switching (du, huawei, zain)
- 5-7 languages
- Activation/registration flows (huawei, zain)
- Setup wizard pages (huawei)
- Custom branding and social media links

### Tier 3: XG-PON/NG-PON (Modern)
**Frames**: XGPON, xgponglobe, FrameAISAP  
**Characteristics**:
- No traditional `frame.asp` (uses minimal `Cusjs/frame.asp`)
- i18n via `ssmpdes.js` resource files (dynamic language loading)
- Modern login with Bootstrap-based UI
- Portal subsystem (`portal/` directory)
- Mainpage dashboard
- 10+ language support
- Extensive CfgMode-driven branding (20+ ISP variants)
- CAPTCHA support
- ISP locking features

---

## Appendix: Complete Menu File List

```
menu.xml                    MenuGlobe.xml               MenuAbroad.xml
MenuChina.xml               MenuTelmex.xml              MenuTelmexAccess.xml
MenuTmczstForAdmin.xml      MenuTmczstForAll.xml        MenuTmczstForUser.xml
MenuRemote.xml              MenuRemoteVoip.xml          MenuXGPONAbroad.xml
MenuXGPONChinaRT.xml        MenuSmartAbroad.xml         MenuSmartAbroadAP.xml
MenuSmartChinaAP.xml        MenuTriplet.xml             MenuTriplet2wifi6.xml
MenuViettel.xml             MenuViettelGuide.xml        MenuSTC.xml
MenuBharti.xml              MenuMobily.xml              MenuSonet.xml
MenuCmcc.xml                MenuCmccRmsReg.xml          MenuCtm.xml
MenuRussian.xml             MenuRussianKaz.xml          MenuBrazClaro.xml
MenuSmartAntel.xml          MenuPtvdf.xml               MenuPtvdfb.xml
MenuAntel.xml               MenuE8cAwifi.xml            MenuE8c.xml
MenuTelecentro.xml          MenuClarodr.xml             MenuClaro.xml
MenuBjct.xml                MenuBjcu.xml                MenuBellTelus.xml
MenuDt.xml                  MenuPt.xml                  MenuTdeinstall.xml
menulankaGuide.xml          menuglobe.xml               ... (+ ~70 more)
```
