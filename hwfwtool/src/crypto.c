#include "hwfwtool/crypto.h"
#include "hwfwtool/utils.h"
#include <string.h>

static uint32_t crc32_table[256];
static bool crc32_table_init = false;

static void crc32_init_table(void) {
    if (crc32_table_init) return;

    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
        crc32_table[i] = crc;
    }
    crc32_table_init = true;
}

uint32_t hw_crc32(const uint8_t *data, size_t size) {
    crc32_init_table();

    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < size; i++) {
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

uint32_t hw_crc32_continue(uint32_t crc, const uint8_t *data, size_t size) {
    crc32_init_table();

    crc ^= 0xFFFFFFFF;
    for (size_t i = 0; i < size; i++) {
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

hw_error_t hw_verify_block_crc(const uint8_t *data, size_t size, uint32_t expected_crc) {
    uint32_t computed = hw_crc32(data, size);
    if (computed != expected_crc) {
        hw_log_debug("CRC mismatch: computed=0x%08x, expected=0x%08x", computed, expected_crc);
        return HW_ERR_CRC;
    }
    return HW_ERR_OK;
}

hw_error_t hw_verify_file_crc(const uint8_t *data, size_t size, uint32_t expected_crc) {
    uint32_t computed = hw_crc32(data, size);
    if (computed != expected_crc) {
        hw_log_warn("File CRC mismatch: computed=0x%08x, expected=0x%08x", computed, expected_crc);
        return HW_ERR_CRC;
    }
    return HW_ERR_OK;
}

bool hw_check_signature(const uint8_t *data, size_t size) {
    if (size < 0x200) return false;

    const uint8_t *tail = data + size - 0x200;
    for (size_t i = 0; i < 0x100; i++) {
        if (tail[i] == 0x30 && tail[i + 1] == 0x82) {
            return true;
        }
    }

    for (size_t i = size - 0x500; i < size - 0x100; i++) {
        if (data[i] == 'H' && data[i + 1] == 'W') {
            return true;
        }
    }

    return false;
}

hw_error_t hw_compute_signature(uint8_t *data, size_t size, const char *key_path) {
    (void)data;
    (void)size;
    (void)key_path;
    hw_log_warn("Signature computation requires external key material");
    return HW_ERR_SIGN;
}
