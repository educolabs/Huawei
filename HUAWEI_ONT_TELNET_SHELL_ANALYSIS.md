# Huawei ONT - Telnet & Shell Analysis from Live Session

## Analysis of Your Telnet Session

### Session Flow Breakdown

```
1. Connection: telnet 192.168.100.1
   → BusyBox telnetd daemon (part of ONT firmware)
   → clid (CLI daemon) handles the session

2. Login:
   Login: root
   Password: [entered]
   → HW_CLI_CheckLoginUser validates user
   → HW_CLI_CheckPwd checks password
   → "Password is default value, please modify it!" - Warning shown

3. Initial CLI (WAP>):
   → User is in normal mode
   → Commands filtered by CmdGroup permissions
   → 849 commands visible based on user group

4. SU Mode Entry:
   WAP>su
   success!
   SU_WAP>
   → HW_CLI_SU_Mode activated
   → HW_CLI_VerifySuPassword called (if configured)
   → Additional commands become visible (0x80000000+ CmdGroup)

5. Shell Access:
   SU_WAP>shell
   → HW_CLI_ExeShell / HW_CLI_ExecvShell called
   → BusyBox ash shell launched
   → Limited command set (no cat, grep, find, sudo, etc.)
```

---

## Shell Restrictions Analysis

### Commands Available in Shell (from your log)

```
Available:
✓ ls              - List directory
✓ ifconfig        - Network interface config
✓ iwconfig        - WiFi interface config
✓ iwpriv          - WiFi private commands
✓ killall         - Kill processes by name
✓ exit            - Exit shell
✓ *.sh scripts    - Some shell scripts (with restrictions)

Blocked:
✗ cat             - "Command is not existed"
✗ grep            - "Command is not existed"
✗ find            - "Command is not existed"
✗ exec            - "Command is not existed"
✗ sh              - "Command is not existed"
✗ sudo            - "Command is not existed"
✗ redtor          - "Command is not existed"
✗ restorehwmode.sh - "Permission denied"
```

### Why Commands Are Blocked

The shell is a **restricted BusyBox ash** with:

1. **Command whitelist** - Only specific applets are compiled in or symlinked
2. **PATH restrictions** - `/bin` contains only allowed commands
3. **Permission-based execution** - Scripts like `restorehwmode.sh` have execute permissions restricted
4. **No dynamic loading** - Cannot load additional shell or interpreters

---

## Understanding the Permission System

### File Permission Analysis

```bash
# From your log:
WAP(Dopra Linux) # restorehwmode.sh
/bin/sh: restorehwmode.sh: Permission denied
```

This indicates:
- The script exists in PATH
- But execute permission is denied for current user/group
- The file likely has permissions like `750` or `700` with root ownership

### Why `equipMode.sh on` Works But `restorehwmode.sh` Doesn't

```bash
WAP(Dopra Linux) # equipMode.sh on
on                    # ← Works!

WAP(Dopra Linux) # restorehwmode.sh
Permission denied     # ← Blocked!
```

**Reason:** Different permission bits on the scripts:
- `equipMode.sh` - Likely has world-executable permission (`755`)
- `restorehwmode.sh` - Restricted to root/group only (`750` or `700`)

---

## Methods to Bypass Shell Restrictions

### Method 1: Use CLI Commands Instead of Shell

The CLI (WAP> prompt) has many commands that don't require shell access:

```bash
# Display system info
display deviceInfo
display sysinfo
display version
display current-configuration

# Configure network
display ip route
display dhcp server user
display waninfo all

# WiFi configuration
display wifi information
get wlan enable
set wlan enable laninst 1 enable 1

# Hardinfo (most powerful)
display equip hardinfo
set hardinfo value SSMP_SPEC_CLI_USERGRP=0x80004000
```

### Method 2: Modify Permissions via CLI

If you can access the shell, try these workarounds:

```bash
# Check current permissions
ls -la /bin/restorehwmode.sh

# Try using absolute path with sh
/bin/sh /bin/restorehwmode.sh

# Try sourcing the script
. /bin/restorehwmode.sh

# Check if ontchmod exists (permission changer)
ls -la /sbin/ontchmod
```

### Method 3: Use Available Scripts

From your log, these scripts are available:

```bash
# WiFi configuration
Wifi2GSsidSet.sh
Wifi5GSsidSet.sh
WifiChainSet.sh
WifiChipInit.sh
WifiSsidSet.sh

# System customization
customize.sh
getcustominfo.sh
getcustomize.sh

# Equipment mode
EquipMode.sh
boardtype.sh

# Try executing them
EquipMode.sh on
customize.sh
getcustomize.sh
```

### Method 4: Leverage CLI `set` Commands

The CLI has extensive `set` commands that can modify system configuration:

```bash
# From SU_WAP> prompt:
set hardinfo value ...        # Modify hardware specs/features
set wifi radio ...            # Configure WiFi radio
set wlan enable ...           # Enable/disable WLAN
set port config ...           # Configure ports
set led ...                   # Control LEDs
set cpu freq ...              # Set CPU frequency
set boardItem ...             # Set board items
set machineItem ...           # Set machine items
```

---

## Complete Unlock Procedure (Based on Your Session)

### Step 1: Enter SU Mode

```bash
WAP>su
success!
SU_WAP>
```

### Step 2: Check Current Configuration

```bash
SU_WAP>display equip hardinfo
SSMP_SPEC_WEB_FRAME=frame_XGPON;SSMP_SPEC_WEB_MENUXML=MenuXGPONAbroad.xml;SSMP_SPEC_CLI_REMOTETELNET=1;SSMP_SPEC_CLI_REDLINEVERSION=1;SSMP_SPEC_CLI_USERGRP=0x80004000;SSMP_SPEC_WEB_PWDENCRYPT=3;&LANGUAGE=COMMON,DT_HUNGARY
```

**Analysis of your current config:**
- `SSMP_SPEC_WEB_FRAME=frame_XGPON` - Already using XGPON frame (good!)
- `SSMP_SPEC_WEB_MENUXML=MenuXGPONAbroad.xml` - Full menu (good!)
- `SSMP_SPEC_CLI_REMOTETELNET=1` - Remote telnet enabled (good!)
- `SSMP_SPEC_CLI_REDLINEVERSION=1` - Debug version enabled (good!)
- `SSMP_SPEC_CLI_USERGRP=0x80004000` - Admin advanced group (good!)
- `SSMP_SPEC_WEB_PWDENCRYPT=3` - Password encrypted (change to 0 for debug)
- `LANGUAGE=COMMON,DT_HUNGARY` - Hungarian language

### Step 3: Apply Unlock Commands

```bash
# Disable password encryption for debugging
SU_WAP>set hardinfo value SSMP_SPEC_WEB_PWDENCRYPT=0

# Enable all features
SU_WAP>set hardinfo value HW_SSMP_FEATURE_WEB=1;HW_SSMP_FEATURE_USB=1;HW_SSMP_FEATURE_TR069=1;BBSP_FT_FIREWALL=1;BBSP_FT_NAT=1;BBSP_FT_IPV6=1;BBSP_FT_ROUTE=1;BBSP_FT_UPNP_MAIN=1;BBSP_FT_WAN=1;FT_SSMP_TELNET_LAN_WAN=1

# Set to COMMON mode
SU_WAP>set hardinfo value customize_flag=COMMON;cfg_word=COMMON;usescene_flag=NOCHOOSE;custom_info=COMMON

# Save configuration
SU_WAP>save data
```

### Step 4: Access Shell and Create Debug Flags

```bash
SU_WAP>shell

# Now in shell:
WAP(Dopra Linux) # touch /mnt/jffs2/swm_debug
WAP(Dopra Linux) # touch /mnt/jffs2/debugcheck
WAP(Dopra Linux) # touch /etc/wap/DebugVersionFlag

# Try to reload features
WAP(Dopra Linux) # echo "Reload" > /proc/wap_proc/feature

# Exit shell
WAP(Dopra Linux) # exit
```

### Step 5: Reboot and Verify

```bash
SU_WAP>reset

# After reboot, reconnect and verify:
telnet 192.168.100.1
Login: root
Password: ...

WAP>su
SU_WAP>display equip hardinfo
# Should show updated values
```

---

## Understanding UserGroup 0x80004000

Your current UserGroup `0x80004000` breaks down as:

```
Binary: 1000 0000 0000 0000 0100 0000 0000 0000
Hex:    0x80004000

Bit 31 (0x80000000) = Admin/SU required
Bit 14  (0x00004000) = Advanced read/write

This gives access to:
✓ All admin commands (WAN, NAT, DHCP, firewall)
✓ Advanced display commands
✓ WiFi filter and MAC filter commands
✓ Debug commands
✓ Backup/restore operations
```

### To Get Even More Privileges

You could try combining bits:

```bash
# Admin + Debug + Advanced + Service
set hardinfo value SSMP_SPEC_CLI_USERGRP=0x80006010

# Breakdown:
# 0x80000000 = Admin
# 0x00004000 = Advanced
# 0x00002000 = Debug
# 0x00000010 = Basic config
```

**Note:** The actual privilege level depends on how `clid` interprets the bitmask. `0x80004000` is already very high.

---

## Commands to Try in Shell (Workarounds)

Since `cat`, `grep`, `find` are blocked, try these alternatives:

```bash
# List files
ls -la /bin/
ls -la /etc/wap/
ls -la /mnt/jffs2/

# Check file type
file /bin/cat 2>/dev/null || echo "file command not available"

# Read files using available tools
# Try using dd (if available)
dd if=/etc/passwd bs=1 count=100 2>/dev/null

# Try using more/less (if available)
more /etc/passwd
less /etc/passwd

# Try using head/tail (if available)
head /etc/passwd
tail /etc/passwd

# Check what's in /bin
ls /bin/ | grep -v "\.sh$"

# Use vi/vim (if available)
vi /etc/config

# Use sed/awk (if available)
sed -n '1,10p' /etc/passwd
awk '{print}' /etc/passwd
```

---

## Debug Mode Activation

### From CLI (SU mode):

```bash
# Enable debug logging
SU_WAP>set cwmp debug 1

# Enable VoIP debug
SU_WAP>set voicedebug

# Display debug info
SU_WAP>display debuglog info
SU_WAP>display debugwifilog info
```

### From Shell:

```bash
# Create debug flags
touch /mnt/jffs2/swm_debug
touch /mnt/jffs2/debugcheck
touch /etc/wap/DebugVersionFlag

# Check if debug mode is active
cat /mnt/jffs2/swm_debug 2>/dev/null && echo "SWM debug active"
```

---

## Web Interface Debug Mode

Your current web config:
```
SSMP_SPEC_WEB_FRAME=frame_XGPON      ← Full featured frame
SSMP_SPEC_WEB_MENUXML=MenuXGPONAbroad.xml  ← Full menu
SSMP_SPEC_WEB_PWDENCRYPT=3           ← Encrypted passwords
```

To enable web debug mode:

```bash
# From CLI:
set hardinfo value SSMP_SPEC_WEB_PWDENCRYPT=0

# From shell:
touch /etc/wap/DebugVersionFlag

# Then access web interface - should show additional debug pages
```

---

## Summary: Your System Status

Based on your telnet log, your ONT is already well-configured:

✅ **Good:**
- CLI UserGroup is `0x80004000` (Admin Advanced)
- Remote telnet is enabled
- Redline/debug version is enabled
- Web frame is `frame_XGPON` (full featured)
- Menu is `MenuXGPONAbroad.xml` (full menu)

🔧 **Can Improve:**
- Change `SSMP_SPEC_WEB_PWDENCRYPT` from 3 to 0 for debug
- Enable additional features via `set hardinfo value`
- Create debug flag files in shell
- Set language to include more options

🚫 **Limitations:**
- Shell is restricted (no cat, grep, find, sudo)
- Some scripts have permission restrictions
- Cannot easily read/modify files in shell

**Recommendation:** Use the CLI commands (`set hardinfo value`, `display`, `get`, `set`) for configuration instead of trying to bypass shell restrictions. The CLI is very powerful and can configure almost everything.
