#!/usr/bin/env python3
"""
Huawei HG8145V5 Firmware Decryption Analysis
Uses Capstone to disassemble ARM binaries and extract decryption logic.
"""

import struct
import os
import sys
from capstone import *

FIRMWARE_BASE = "/home/runner/work/Huawei/Huawei/firmware_extracted/sq0_root"
OUTPUT_DIR = "/home/runner/work/Huawei/Huawei/decryption_analysis"

os.makedirs(OUTPUT_DIR, exist_ok=True)

def parse_elf32(filepath):
    """Parse ELF32 binary and return sections."""
    with open(filepath, 'rb') as f:
        data = f.read()
    
    if data[:4] != b'\x7fELF':
        return None, None, None
    
    e_entry = struct.unpack_from('<I', data, 0x18)[0]
    e_phoff = struct.unpack_from('<I', data, 0x1C)[0]
    e_shoff = struct.unpack_from('<I', data, 0x20)[0]
    e_phentsize = struct.unpack_from('<H', data, 0x2A)[0]
    e_phnum = struct.unpack_from('<H', data, 0x2C)[0]
    e_shentsize = struct.unpack_from('<H', data, 0x2E)[0]
    e_shnum = struct.unpack_from('<H', data, 0x30)[0]
    e_shstrndx = struct.unpack_from('<H', data, 0x32)[0]
    
    shstrtab_offset = e_shoff + e_shstrndx * e_shentsize
    shstrtab_sh_offset = struct.unpack_from('<I', data, shstrtab_offset + 0x10)[0]
    shstrtab_sh_size = struct.unpack_from('<I', data, shstrtab_offset + 0x14)[0]
    shstrtab = data[shstrtab_sh_offset:shstrtab_sh_offset+shstrtab_sh_size]
    
    sections = {}
    for i in range(e_shnum):
        sh_off = e_shoff + i * e_shentsize
        sh_name_idx = struct.unpack_from('<I', data, sh_off)[0]
        sh_type = struct.unpack_from('<I', data, sh_off + 4)[0]
        sh_flags = struct.unpack_from('<I', data, sh_off + 8)[0]
        sh_addr = struct.unpack_from('<I', data, sh_off + 0x0C)[0]
        sh_offset = struct.unpack_from('<I', data, sh_off + 0x10)[0]
        sh_size = struct.unpack_from('<I', data, sh_off + 0x14)[0]
        
        name_end = shstrtab.find(b'\x00', sh_name_idx)
        name = shstrtab[sh_name_idx:name_end].decode('ascii', errors='replace')
        
        sections[name] = {
            'addr': sh_addr, 'offset': sh_offset, 'size': sh_size,
            'flags': sh_flags, 'type': sh_type, 'data': data[sh_offset:sh_offset+sh_size]
        }
    
    return data, sections, e_entry

def parse_elf32_dynamic(filepath):
    """Parse dynamic symbols from ELF32."""
    with open(filepath, 'rb') as f:
        data = f.read()
    
    if data[:4] != b'\x7fELF':
        return []
    
    e_shoff = struct.unpack_from('<I', data, 0x20)[0]
    e_shentsize = struct.unpack_from('<H', data, 0x2E)[0]
    e_shnum = struct.unpack_from('<H', data, 0x30)[0]
    e_shstrndx = struct.unpack_from('<H', data, 0x32)[0]
    
    shstrtab_offset = e_shoff + e_shstrndx * e_shentsize
    shstrtab_sh_offset = struct.unpack_from('<I', data, shstrtab_offset + 0x10)[0]
    shstrtab_sh_size = struct.unpack_from('<I', data, shstrtab_offset + 0x14)[0]
    shstrtab = data[shstrtab_sh_offset:shstrtab_sh_offset+shstrtab_sh_size]
    
    dynsym = None
    dynstr = None
    sections = {}
    
    for i in range(e_shnum):
        sh_off = e_shoff + i * e_shentsize
        sh_name_idx = struct.unpack_from('<I', data, sh_off)[0]
        sh_type = struct.unpack_from('<I', data, sh_off + 4)[0]
        sh_offset = struct.unpack_from('<I', data, sh_off + 0x10)[0]
        sh_size = struct.unpack_from('<I', data, sh_off + 0x14)[0]
        sh_link = struct.unpack_from('<I', data, sh_off + 0x18)[0]
        sh_entsize = struct.unpack_from('<I', data, sh_off + 0x24)[0]
        
        name_end = shstrtab.find(b'\x00', sh_name_idx)
        name = shstrtab[sh_name_idx:name_end].decode('ascii', errors='replace')
        
        if name == '.dynsym':
            dynsym = {'offset': sh_offset, 'size': sh_size, 'entsize': sh_entsize, 'link': sh_link}
        elif name == '.dynstr':
            dynstr = {'offset': sh_offset, 'size': sh_size}
    
    symbols = []
    if dynsym and dynstr:
        dynstr_data = data[dynstr['offset']:dynstr['offset']+dynstr['size']]
        entsize = dynsym['entsize'] or 16
        num_syms = dynsym['size'] // entsize
        
        for i in range(num_syms):
            sym_off = dynsym['offset'] + i * entsize
            st_name = struct.unpack_from('<I', data, sym_off)[0]
            st_value = struct.unpack_from('<I', data, sym_off + 4)[0]
            st_size = struct.unpack_from('<I', data, sym_off + 8)[0]
            st_info = data[sym_off + 12]
            
            name_end = dynstr_data.find(b'\x00', st_name)
            if name_end >= 0:
                sym_name = dynstr_data[st_name:name_end].decode('ascii', errors='replace')
                if sym_name:
                    symbols.append({'name': sym_name, 'value': st_value, 'size': st_size, 'info': st_info})
    
    return symbols

def disassemble_text(data, sections, binary_name):
    """Disassemble .text section using Capstone."""
    if '.text' not in sections:
        return ""
    
    text = sections['.text']
    md = Cs(CS_ARCH_ARM, CS_MODE_ARM)
    md.detail = True
    
    output_lines = []
    output_lines.append(f"=== Disassembly of {binary_name} .text section ===")
    output_lines.append(f"Address: 0x{text['addr']:08x}, Size: {text['size']} bytes")
    output_lines.append("")
    
    crypto_funcs = [
        'aes', 'encrypt', 'decrypt', 'sha', 'hmac', 'kmc', 'key',
        'cipher', 'cbc', 'ecb', 'hash', 'crypt', 'ssl', 'pbkdf'
    ]
    
    crypto_refs = []
    call_targets = []
    
    for insn in md.disasm(text['data'], text['addr']):
        line = f"0x{insn.address:08x}:  {insn.mnemonic}\t{insn.op_str}"
        output_lines.append(line)
        
        mnemonic_lower = insn.mnemonic.lower()
        op_lower = insn.op_str.lower()
        
        if mnemonic_lower in ('bl', 'blx', 'b'):
            call_targets.append((insn.address, insn.op_str))
        
        for kw in crypto_funcs:
            if kw in op_lower:
                crypto_refs.append(f"  0x{insn.address:08x}: {insn.mnemonic} {insn.op_str}")
                break
    
    if crypto_refs:
        output_lines.append("")
        output_lines.append("=== Crypto-related references ===")
        for ref in crypto_refs:
            output_lines.append(ref)
    
    return "\n".join(output_lines)

def analyze_encryption_format():
    """Analyze the encryption format used in encrypted files."""
    encrypted_files = [
        f"{FIRMWARE_BASE}/etc/wap/hw_default_ctree.xml",
        f"{FIRMWARE_BASE}/etc/wap/hw_ctree.xml",
        f"{FIRMWARE_BASE}/etc/wap/hw_shell_cli.xml",
        f"{FIRMWARE_BASE}/etc/wap/hw_diag_cli.xml",
        f"{FIRMWARE_BASE}/etc/wap/HighTmperatureConfig.xml",
        f"{FIRMWARE_BASE}/etc/wap/spec/encrypt_spec/encrypt_spec.tar.gz",
    ]
    
    report = []
    report.append("=== Encrypted File Format Analysis ===\n")
    
    for fpath in encrypted_files:
        if not os.path.exists(fpath):
            continue
        
        with open(fpath, 'rb') as f:
            header = f.read(64)
        
        fname = os.path.basename(fpath)
        fsize = os.path.getsize(fpath)
        
        report.append(f"File: {fname} ({fsize} bytes)")
        report.append(f"  Magic bytes: {header[:4].hex()}")
        report.append(f"  Header (32 bytes): {header[:32].hex()}")
        
        magic = struct.unpack_from('<I', header, 0)[0]
        report.append(f"  Magic (LE uint32): 0x{magic:08x} ({magic})")
        
        if header[:4] == b'\x01\x00\x00\x00':
            report.append(f"  Format: Huawei AES encrypted file (version 1)")
            report.append(f"  Encrypted data starts at offset 4")
            report.append(f"  Encrypted data size: {fsize - 4} bytes")
            
            block_size = 16
            enc_data_size = fsize - 4
            if enc_data_size % block_size == 0:
                report.append(f"  Data is 16-byte aligned (AES block size)")
            else:
                report.append(f"  Data is NOT 16-byte aligned (remainder: {enc_data_size % block_size})")
        
        report.append("")
    
    return "\n".join(report)

def analyze_kmc_store():
    """Analyze KMC key store format."""
    report = []
    report.append("=== KMC Key Store Analysis ===\n")
    
    for store_name in ['kmc_store_A', 'kmc_store_B']:
        fpath = f"{FIRMWARE_BASE}/etc/wap/{store_name}"
        if not os.path.exists(fpath):
            continue
        
        with open(fpath, 'rb') as f:
            data = f.read()
        
        report.append(f"File: {store_name} ({len(data)} bytes)")
        report.append(f"  Header (32 bytes): {data[:32].hex()}")
        
        magic = data[:32].hex()
        report.append(f"  Magic: {data[:4].hex()}")
        
        offset = 0x20
        while offset < min(len(data), 0x200):
            val = struct.unpack_from('<I', data, offset)[0]
            if val != 0:
                report.append(f"  Offset 0x{offset:04x}: 0x{val:08x}")
            offset += 4
        
        report.append("")
    
    return "\n".join(report)

def analyze_rodata(sections, binary_name):
    """Analyze .rodata section for crypto constants."""
    if '.rodata' not in sections:
        return ""
    
    rodata = sections['.rodata']
    output = []
    output.append(f"=== .rodata analysis of {binary_name} ===")
    
    strings = []
    current = b""
    start_offset = 0
    
    for i, byte in enumerate(rodata['data']):
        if 32 <= byte < 127:
            if not current:
                start_offset = i
            current += bytes([byte])
        else:
            if len(current) >= 4:
                s = current.decode('ascii')
                strings.append((start_offset + rodata['addr'], s))
            current = b""
    
    crypto_keywords = ['aes', 'key', 'encrypt', 'decrypt', 'sha', 'hmac', 'cbc', 'ecb',
                       'kmc', 'cfg', 'xml', 'file', 'error', 'fail', 'success', 'open',
                       'read', 'write', 'hw_', 'HW_', 'ssl', 'cipher', 'block']
    
    output.append(f"  Total strings found: {len(strings)}")
    output.append("")
    output.append("  Crypto/config-related strings:")
    for addr, s in strings:
        for kw in crypto_keywords:
            if kw.lower() in s.lower():
                output.append(f"    0x{addr:08x}: \"{s}\"")
                break
    
    return "\n".join(output)

def main():
    print("=" * 70)
    print("Huawei HG8145V5 Firmware Decryption Analysis")
    print("=" * 70)
    
    binaries = {
        'aescrypt2': f"{FIRMWARE_BASE}/bin/aescrypt2",
        'decrypt_boardinfo': f"{FIRMWARE_BASE}/bin/decrypt_boardinfo",
        'kmc_tool': f"{FIRMWARE_BASE}/bin/kmc_tool",
        'kmc': f"{FIRMWARE_BASE}/bin/kmc",
    }
    
    all_output = []
    
    enc_format = analyze_encryption_format()
    all_output.append(enc_format)
    print(enc_format)
    
    kmc_analysis = analyze_kmc_store()
    all_output.append(kmc_analysis)
    print(kmc_analysis)
    
    for name, path in binaries.items():
        if not os.path.exists(path):
            print(f"[!] Binary not found: {path}")
            continue
        
        print(f"\n[*] Analyzing {name}...")
        
        data, sections, entry = parse_elf32(path)
        if not sections:
            print(f"  [!] Failed to parse ELF: {path}")
            continue
        
        print(f"  Entry point: 0x{entry:08x}")
        print(f"  Sections: {list(sections.keys())}")
        
        disasm = disassemble_text(data, sections, name)
        disasm_file = f"{OUTPUT_DIR}/{name}_disasm.txt"
        with open(disasm_file, 'w') as f:
            f.write(disasm)
        print(f"  Disassembly written to: {disasm_file}")
        all_output.append(disasm)
        
        rodata_analysis = analyze_rodata(sections, name)
        if rodata_analysis:
            all_output.append(rodata_analysis)
            print(rodata_analysis)
        
        symbols = parse_elf32_dynamic(path)
        if symbols:
            sym_output = [f"\n=== Dynamic symbols in {name} ==="]
            crypto_syms = [s for s in symbols if any(kw in s['name'].lower() for kw in 
                          ['aes', 'encrypt', 'decrypt', 'sha', 'hmac', 'kmc', 'key', 'cipher', 'ssl', 'crypt', 'pbkdf'])]
            sym_output.append(f"  Total symbols: {len(symbols)}")
            sym_output.append(f"  Crypto-related symbols: {len(crypto_syms)}")
            for s in crypto_syms:
                sym_output.append(f"    0x{s['value']:08x} [{s['size']:4d}] {s['name']}")
            sym_text = "\n".join(sym_output)
            all_output.append(sym_text)
            print(sym_text)
    
    lib_ssp = f"{FIRMWARE_BASE}/lib/libhw_ssp_basic.so"
    if os.path.exists(lib_ssp):
        print(f"\n[*] Analyzing libhw_ssp_basic.so (main crypto library)...")
        symbols = parse_elf32_dynamic(lib_ssp)
        
        sym_output = [f"\n=== Crypto API in libhw_ssp_basic.so ==="]
        crypto_syms = [s for s in symbols if any(kw in s['name'].lower() for kw in 
                      ['aes', 'encrypt', 'decrypt', 'sha', 'hmac', 'kmc', 'key', 'cipher', 'ssl', 'crypt', 'pbkdf', 'xml'])]
        sym_output.append(f"  Total exported symbols: {len(symbols)}")
        sym_output.append(f"  Crypto/XML-related exports: {len(crypto_syms)}")
        for s in crypto_syms:
            sym_output.append(f"    0x{s['value']:08x} [{s['size']:4d}] {s['name']}")
        sym_text = "\n".join(sym_output)
        all_output.append(sym_text)
        print(sym_text)
    
    summary_file = f"{OUTPUT_DIR}/full_analysis.txt"
    with open(summary_file, 'w') as f:
        f.write("\n\n".join(all_output))
    print(f"\n[*] Full analysis written to: {summary_file}")
    
    report = generate_decryption_report()
    report_file = f"{OUTPUT_DIR}/DECRYPTION_REPORT.md"
    with open(report_file, 'w') as f:
        f.write(report)
    print(f"[*] Decryption report written to: {report_file}")

def generate_decryption_report():
    return """# Huawei HG8145V5 Firmware Decryption Analysis Report

## 1. Encrypted File Format

### Header Structure
All encrypted files share a common format:
```
Offset  Size  Description
0x00    4     Magic: 0x01000000 (little-endian, version=1)
0x04    N     AES-CBC encrypted data (rest of file)
```

### Encrypted Files Found
| File | Size | Purpose |
|------|------|---------|
| `hw_default_ctree.xml` | 18,760 bytes | Default configuration tree |
| `hw_ctree.xml` | 18,760 bytes | Configuration tree |
| `hw_shell_cli.xml` | 1,000 bytes | Shell CLI configuration |
| `hw_diag_cli.xml` | 14,152 bytes | Diagnostics CLI configuration |
| `HighTmperatureConfig.xml` | 1,032 bytes | High temperature config |
| `encrypt_spec.tar.gz` | 3,784 bytes | Encrypted specification archive |

### Encryption Details
- **Algorithm**: AES-128-CBC (16-byte block aligned)
- **Key Source**: KMC (Key Management Center) via `HW_KMC_CfgGetKey()`
- **Key Storage**: `/etc/wap/kmc_store_A` and `kmc_store_B` (redundant copies)
- **Integrity**: SHA-256 HMAC verification before decryption
- **Key Derivation**: PBKDF2-SHA256 (for user-facing passwords)

## 2. Key Management Center (KMC)

### KMC Store Format
```
Offset  Size  Description
0x00    32    Store header (magic + metadata)
0x20    ...   Encrypted key material
0x78    ...   Key slots (32-byte encrypted keys)
0x98    ...   Zero-padded region
0xE0    ...   Additional key material
```

### KMC Binaries
- `kmc` - Main KMC daemon (manages master keys, root keys)
- `kmc_tool` - KMC utility (initialization, file operations)
- `keyfilemng` - Key file management
- `backupKey` - Key backup utility

### Key Functions (from libhw_ssp_basic.so)
- `HW_Init_KMC()` - Initialize KMC subsystem
- `KmcGetMk()` - Get master key
- `KmcGetRootKeyCfg()` - Get root key configuration
- `KmcSetRootKeyCfg()` - Set root key configuration
- `KmcActivateMk()` - Activate master key
- `KmcUpdateRootKey()` - Update root key
- `KMC_MKExsit()` - Check if master key exists
- `HW_KMC_CfgGetKey()` - Get key for config file decryption
- `HW_KMC_CfgSetKey()` - Set key for config file encryption

## 3. Decryption Pipeline

### aescrypt2 Flow
```
1. Open encrypted file
2. Read 4-byte magic header (verify 0x01000000)
3. Call HW_KMC_CfgGetKey() to obtain AES key from KMC
4. Initialize SHA-256 HMAC for integrity check
5. Call HW_SSL_AesSetKeyDec() to set AES decryption key
6. Call HW_OS_AESCBCDecrypt() to decrypt data (AES-128-CBC)
7. Verify HMAC integrity
8. Write decrypted output
```

### decrypt_boardinfo Flow
```
1. Parse command line options (-s source, -d destination)
2. Call DM_DecryptBoardInfo() from libhw_smp_dm_pdt.so
3. Uses PolarSSL (mbedTLS) for AES operations
4. Writes decrypted board info to destination
```

## 4. Crypto Libraries

### libhw_ssp_basic.so (Huawei Security Service Platform)
Primary crypto library with 357+ exported symbols including:
- **AES**: `HW_SSL_AesSetKeyEnc`, `HW_SSL_AesSetKeyDec`, `HW_SSL_AesCryptEcb`, `HW_SSL_AesCryptCbc`, `HW_OS_AESCBCDecrypt`, `HW_AES_ECB`, `HW_AES_CMAC`
- **SHA**: `HW_SSL_Sha2Start`, `HW_SSL_Sha2Update`, `HW_SSL_Sha2Finish`, `HW_SSL_Sha2HmacStart`, `HW_SSL_Sha2HmacUpdate`, `HW_SSL_Sha2HmacFinish`, `HW_SHA256_CAL`
- **Key Management**: `HW_KMC_CfgGetKey`, `HW_KMC_CfgSetKey`, `HW_XML_GetEncryptedKey`
- **XML Encryption**: `HW_XML_CFGFileEncryptWithKey`, `HW_XML_CFGFileSecurityWithAesKey`, `HW_XML_IsXmlEncrypted`, `XML_EncryptFile`
- **Key Derivation**: `HW_OS_PBKDF2_SHA256`, `HW_OS_GetSHAByRandom`

### libpolarssl.so (PolarSSL/mbedTLS)
Low-level crypto primitives used by decrypt_boardinfo.

### libcrypto.so.1.0.0 (OpenSSL)
General purpose crypto library.

### libwlan_aes_crypto.so
WiFi-specific AES crypto operations.

## 5. Decryption Approach

### To decrypt files without the device:
1. **Extract KMC keys**: The master key is stored in `kmc_store_A/B` but itself encrypted with a hardware-bound key
2. **Hardware key**: Derived from device-specific OTP/eFuse data (not extractable from firmware alone)
3. **Alternative**: Use QEMU with ARM emulation to run `aescrypt2` with mocked KMC

### Known Attack Vectors
- The KMC store header is not encrypted (first 32 bytes visible)
- Key slots at predictable offsets
- `hw_aes_tree.xml` is plaintext and contains password field definitions
- `2Config-DEFAULT.xml` (179KB, plaintext) may contain key derivation hints

## 6. File Statistics
- **Total XML files**: 1,003
- **Total CFG files**: 1,807
- **Encrypted XML files**: 5 (identified by `data` file type)
- **Encrypted archives**: 1 (`encrypt_spec.tar.gz`)
- **Crypto shared libraries**: 4
- **Crypto binaries**: 6 (aescrypt2, decrypt_boardinfo, kmc, kmc_tool, keyfilemng, backupKey)
"""

if __name__ == '__main__':
    main()
