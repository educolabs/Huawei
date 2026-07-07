# Huawei HG8145V5 Firmware Decryption Analysis Report

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
