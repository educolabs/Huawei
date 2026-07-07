#ifndef HWFWTOOL_CRYPTO_H
#define HWFWTOOL_CRYPTO_H

#include "common.h"

uint32_t hw_crc32(const uint8_t *data, size_t size);
uint32_t hw_crc32_continue(uint32_t crc, const uint8_t *data, size_t size);
hw_error_t hw_verify_block_crc(const uint8_t *data, size_t size, uint32_t expected_crc);
hw_error_t hw_verify_file_crc(const uint8_t *data, size_t size, uint32_t expected_crc);
bool hw_check_signature(const uint8_t *data, size_t size);
hw_error_t hw_compute_signature(uint8_t *data, size_t size, const char *key_path);

#endif
