#ifndef HWFWTOOL_COMPRESS_H
#define HWFWTOOL_COMPRESS_H

#include "common.h"

hw_error_t hw_zlib_decompress(const uint8_t *src, size_t src_size,
                               uint8_t **dst, size_t *dst_size);
hw_error_t hw_zlib_compress(const uint8_t *src, size_t src_size,
                             uint8_t **dst, size_t *dst_size, int level);
hw_error_t hw_gzip_decompress(const uint8_t *src, size_t src_size,
                               uint8_t **dst, size_t *dst_size);
hw_error_t hw_gzip_compress(const uint8_t *src, size_t src_size,
                             uint8_t **dst, size_t *dst_size);
hw_compress_type_t hw_detect_compression(const uint8_t *data, size_t size);
size_t hw_zlib_decompress_bound(size_t src_size);

#endif
