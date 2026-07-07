#!/usr/bin/env python3
"""
Huawei HG8145V5 - Complete Decryption Flow Analysis
Based on Capstone ARM disassembly of aescrypt2 binary.

This script documents the exact encrypted file format and decryption pipeline.
"""

import struct
import os

FIRMWARE_BASE = "/home/runner/work/Huawei/Huawei/firmware_extracted/sq0_root"

def analyze_file_format():
    """
    Encrypted file format (determined from aescrypt2 disassembly):
    
    The aescrypt2 binary reads files with this structure:
    
    OFFSET  SIZE    FIELD               DESCRIPTION
    0x00    4       uiType              File type magic (uint32 LE = 1)
    0x04    4       uiFileCrc           CRC32 of original plaintext
    0x08    4       uiKeyLen            Length of key identifier
    0x0C    N       aucKey[uiKeyLen]    Key identifier / chip string
    ...     16      aucIv[16]           AES-CBC IV (16 bytes)
    ...     M       encrypted_data      AES-CBC encrypted content
    ...     32      aucHmac[32]         HMAC-SHA256 (at end of file)
    
    Decryption pipeline (MODE_DECRYPT = 1):
    
    1. HW_CTOOL_GetKeyChipStr() - Determine hardware key chip identifier
    2. HW_KMC_CfgGetKey(chipStr, key, keyLen) - Retrieve AES key from KMC store
    3. Read file header: type, CRC, key length, key identifier
    4. Read 16-byte IV from file
    5. SHA-256 hash of key material (key derivation):
       - HW_SSL_Sha2Start(&sha_ctx, SHA256)
       - HW_SSL_Sha2Update(&sha_ctx, key_data, key_len)
       - HW_SSL_Sha2Update(&sha_ctx, additional_data, additional_len)
       - HW_SSL_Sha2Finish(&sha_ctx, derived_key, &out_len)
    6. Set AES decryption key:
       - HW_SSL_AesSetKeyDec(&aes_ctx, derived_key, 32)
    7. Initialize HMAC for integrity verification:
       - HW_SSL_Sha2HmacStart(&hmac_ctx, hmac_key, hmac_key_len)
    8. Read encrypted data in 16-byte blocks:
       - HW_SSL_AesCryptEcb(&aes_ctx, block, block) - decrypt each block
       - HW_SSL_Sha2HmacUpdate(&hmac_ctx, decrypted_block, 16) - update HMAC
       - Write decrypted block to output
    9. Finish HMAC and verify:
       - HW_SSL_Sha2HmacFinish(&hmac_ctx, computed_hmac, &hmac_len)
       - Read 32-byte stored HMAC from end of file
       - HW_OS_MemCmp(computed_hmac, stored_hmac, 32)
       - If mismatch: "HMAC check failed: wrong key, or file corrupted."
    
    Key source: KMC (Key Management Center)
    - Keys stored in: /etc/wap/kmc_store_A and kmc_store_B (redundant)
    - KMC store format: magic 0x5f64978d + encrypted key material
    - Master keys protected by hardware root key (SoC-specific)
    - Key referenced by string: "SSMP_SPEC_CONFIG_ENCRYPTION_KEY"
    """
    pass

def dump_encrypted_file_structure(filepath):
    """Parse and display the structure of an encrypted file."""
    with open(filepath, 'rb') as f:
        data = f.read()
    
    fname = os.path.basename(filepath)
    fsize = len(data)
    
    print(f"\n{'='*60}")
    print(f"File: {fname} ({fsize} bytes)")
    print(f"{'='*60}")
    
    if fsize < 4:
        print("  Too small to be a valid encrypted file")
        return
    
    uiType = struct.unpack_from('<I', data, 0)[0]
    print(f"  [0x00] Type:       0x{uiType:08x} ({uiType})")
    
    if fsize >= 8:
        uiCrc = struct.unpack_from('<I', data, 4)[0]
        print(f"  [0x04] CRC:        0x{uiCrc:08x}")
    
    if fsize >= 12:
        uiKeyLen = struct.unpack_from('<I', data, 8)[0]
        print(f"  [0x08] Key Length:  0x{uiKeyLen:08x} ({uiKeyLen})")
        
        if uiKeyLen > 0 and uiKeyLen < 256 and fsize >= 12 + uiKeyLen:
            key_data = data[12:12+uiKeyLen]
            print(f"  [0x0C] Key ID:      {key_data.hex()}")
            
            iv_offset = 12 + uiKeyLen
            if fsize >= iv_offset + 16:
                iv = data[iv_offset:iv_offset+16]
                print(f"  [0x{iv_offset:02X}] IV:         {iv.hex()}")
                
                enc_start = iv_offset + 16
                hmac_size = 32
                enc_size = fsize - enc_start - hmac_size
                if enc_size > 0:
                    print(f"  [0x{enc_start:02X}] Encrypted:  {enc_size} bytes ({enc_size // 16} blocks)")
                    print(f"  [0x{fsize - hmac_size:02X}] HMAC:       {data[fsize-hmac_size:].hex()}")
                else:
                    print(f"  NOTE: Key length {uiKeyLen} seems incorrect (no room for IV+data+HMAC)")
                    print(f"  File may use simpler format: magic(4) + encrypted_blob({fsize-4})")
        else:
            print(f"  NOTE: Key length {uiKeyLen} seems invalid")
            print(f"  Likely format: magic(4) + encrypted_blob({fsize-4} bytes)")
    
    print(f"  Raw header: {data[:32].hex()}")

def main():
    print("=" * 70)
    print("Huawei HG8145V5 - Encrypted File Structure Analysis")
    print("=" * 70)
    
    encrypted_files = [
        f"{FIRMWARE_BASE}/etc/wap/hw_default_ctree.xml",
        f"{FIRMWARE_BASE}/etc/wap/hw_ctree.xml",
        f"{FIRMWARE_BASE}/etc/wap/hw_shell_cli.xml",
        f"{FIRMWARE_BASE}/etc/wap/hw_diag_cli.xml",
        f"{FIRMWARE_BASE}/etc/wap/HighTmperatureConfig.xml",
        f"{FIRMWARE_BASE}/etc/wap/spec/encrypt_spec/encrypt_spec.tar.gz",
    ]
    
    for fpath in encrypted_files:
        if os.path.exists(fpath):
            dump_encrypted_file_structure(fpath)
    
    print("\n" + "=" * 70)
    print("DECRYPTION APPROACH")
    print("=" * 70)
    print("""
The encryption uses AES-CBC with keys managed by Huawei's KMC system.
The master encryption key is protected by a hardware root key embedded
in the SoC (HiSilicon), making it impossible to extract from firmware alone.

To decrypt these files, you would need:
1. The actual device (to access hardware root key)
2. OR a JTAG/debug interface to dump KMC memory at runtime
3. OR emulate the SoC with QEMU (if hardware key can be mocked)

The KMC store files (kmc_store_A/B) contain encrypted key material:
  - Magic: 0x5f64978d
  - Key slots at offsets 0x38, 0x98, 0xE0, 0x148, 0x1E0
  - Each slot is 32 bytes of encrypted key data
  - Root key configuration at offset 0x20

The string "SSMP_SPEC_CONFIG_ENCRYPTION_KEY" in the kmc binary
identifies the key used for config file encryption.

Alternative approach:
  - Use the plaintext 2Config-DEFAULT.xml (179KB) as a known-plaintext
    attack vector if the same key encrypts both default and custom configs
  - hw_default_ctree.xml and hw_ctree.xml have identical encrypted headers,
    suggesting they contain identical plaintext encrypted with the same key
""")

if __name__ == '__main__':
    main()
