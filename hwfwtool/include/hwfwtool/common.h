#ifndef HWFWTOOL_COMMON_H
#define HWFWTOOL_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#define PATH_SEP '\\'
#define PATH_SEP_STR "\\"
#else
#define PATH_SEP '/'
#define PATH_SEP_STR "/"
#endif

#define HWNP_MAGIC          "HWNP"
#define HWNP_MAGIC_SIZE     4
#define HWNP_HEADER_SIZE    0x20
#define HWNP_PRODUCT_SIZE   0x30
#define HWNP_BLOCK_DESC_SIZE 0x168
#define HWNP_DATA_START     0x1000
#define HWNP_NAME_OFFSET    0x114
#define HWNP_META_OFFSET    0x16C
#define HWNP_FLASH_OFFSET   0x17C
#define HWNP_NAME_MAX       32
#define HWNP_PATH_MAX       128
#define HWNP_VERSION_MAX    64
#define HWNP_MAX_BLOCKS     16

#define HWFWTOOL_VERSION    "1.0.0"
#define HWFWTOOL_NAME       "hwfwtool"

typedef enum {
    HW_ERR_OK           = 0,
    HW_ERR_MEMORY       = -1,
    HW_ERR_IO           = -2,
    HW_ERR_FORMAT       = -3,
    HW_ERR_MAGIC        = -4,
    HW_ERR_CRC          = -5,
    HW_ERR_COMPRESS     = -6,
    HW_ERR_DECOMPRESS   = -7,
    HW_ERR_SIGN         = -8,
    HW_ERR_VERIFY       = -9,
    HW_ERR_INVALID_ARG  = -10,
    HW_ERR_NOT_FOUND    = -11,
    HW_ERR_BLOCK_COUNT  = -12,
    HW_ERR_TRUNCATED    = -13,
} hw_error_t;

typedef enum {
    HW_COMP_NONE    = 0,
    HW_COMP_ZLIB    = 1,
    HW_COMP_LZMA    = 2,
    HW_COMP_GZIP    = 3,
    HW_COMP_JFFS2   = 4,
    HW_COMP_UNKNOWN = 5,
} hw_compress_type_t;

typedef enum {
    HW_BLOCK_FLASH    = 0,
    HW_BLOCK_FILE     = 1,
} hw_block_target_t;

#pragma pack(push, 1)

typedef struct {
    char     magic[HWNP_MAGIC_SIZE];
    uint32_t header_hash;
    uint32_t file_hash;
    uint32_t header_size;
    uint32_t file_crc;
    uint32_t block_count;
    uint8_t  reserved[HWNP_HEADER_SIZE - 24];
} hw_header_t;

typedef struct {
    uint32_t index;
    uint32_t crc32;
    uint32_t timestamp;
    uint32_t comp_size;
} hw_block_meta_t;

typedef struct {
    uint8_t  padding[HWNP_NAME_OFFSET];
    char     name[HWNP_NAME_MAX];
    char     version[HWNP_VERSION_MAX];
    hw_block_meta_t meta;
    char     flash_path[HWNP_PATH_MAX];
} hw_block_descriptor_t;

#pragma pack(pop)

typedef struct {
    char     name[HWNP_NAME_MAX];
    char     version[HWNP_VERSION_MAX];
    char     flash_path[HWNP_PATH_MAX];
    char     product_info[256];
    uint32_t index;
    uint32_t crc32;
    uint32_t timestamp;
    uint32_t comp_size;
    uint32_t orig_size;
    uint64_t data_offset;
    hw_compress_type_t compression;
    hw_block_target_t  target;
    uint32_t flags;
    bool     has_version;
    bool     is_signed;
} hw_block_info_t;

typedef struct {
    char     magic[HWNP_MAGIC_SIZE + 1];
    char     product_info[256];
    char     firmware_version[HWNP_VERSION_MAX];
    char     board_id[64];
    char     carrier[64];
    char     region[64];
    char     country[64];
    uint32_t header_hash;
    uint32_t file_hash;
    uint32_t file_crc;
    uint32_t block_count;
    uint64_t file_size;
    uint64_t data_start;
    uint64_t data_end;
    uint64_t trailer_offset;
    uint64_t trailer_size;
    bool     is_signed;
    bool     has_capability;
    hw_block_info_t blocks[HWNP_MAX_BLOCKS];
    uint8_t  *trailer_data;
    size_t   trailer_data_size;
} hw_firmware_t;

typedef struct {
    bool        verbose;
    bool        force;
    bool        no_compress;
    bool        skip_crc;
    bool        skip_verify;
    const char *input_file;
    const char *output_file;
    const char *output_dir;
    const char *config_file;
    const char *sign_key;
    const char *block_name;
    int         log_level;
} hw_options_t;

const char *hw_error_string(hw_error_t err);
const char *hw_compress_type_string(hw_compress_type_t type);
const char *hw_block_target_string(hw_block_target_t target);

#endif
