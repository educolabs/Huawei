#ifndef HWFWTOOL_FIRMWARE_H
#define HWFWTOOL_FIRMWARE_H

#include "common.h"

hw_error_t hw_firmware_init(hw_firmware_t *fw);
void hw_firmware_cleanup(hw_firmware_t *fw);
hw_error_t hw_firmware_load(hw_firmware_t *fw, const char *path);
hw_error_t hw_firmware_save(hw_firmware_t *fw, const char *path);
hw_error_t hw_firmware_parse_header(hw_firmware_t *fw, const uint8_t *data, size_t size);
hw_error_t hw_firmware_parse_blocks(hw_firmware_t *fw, const uint8_t *data, size_t size);
hw_error_t hw_firmware_parse_trailer(hw_firmware_t *fw, const uint8_t *data, size_t size);
hw_error_t hw_firmware_detect_compression(hw_firmware_t *fw, const uint8_t *data, size_t size);
void hw_firmware_print_info(const hw_firmware_t *fw);
void hw_firmware_print_blocks(const hw_firmware_t *fw);
hw_block_info_t *hw_firmware_find_block(hw_firmware_t *fw, const char *name);

#endif
