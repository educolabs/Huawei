#ifndef HWFWTOOL_BLOCKS_H
#define HWFWTOOL_BLOCKS_H

#include "common.h"

hw_error_t hw_blocks_extract(const hw_firmware_t *fw, const uint8_t *data, size_t size,
                              const char *output_dir, const hw_options_t *opts);
hw_error_t hw_blocks_extract_single(const hw_firmware_t *fw, const uint8_t *data, size_t size,
                                     const char *block_name, const char *output_path);
hw_error_t hw_blocks_repack(hw_firmware_t *fw, const char *input_dir, const char *output_path,
                             const hw_options_t *opts);
hw_error_t hw_blocks_verify(const hw_firmware_t *fw, const uint8_t *data, size_t size);
hw_error_t hw_block_decompress(const uint8_t *src, size_t src_size,
                                uint8_t **dst, size_t *dst_size,
                                hw_compress_type_t type);
hw_error_t hw_block_compress(const uint8_t *src, size_t src_size,
                              uint8_t **dst, size_t *dst_size,
                              hw_compress_type_t type);

#endif
