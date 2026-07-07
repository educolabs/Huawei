#include "hwfwtool/compress.h"
#include "hwfwtool/utils.h"
#include <zlib.h>
#include <stdlib.h>
#include <string.h>

hw_compress_type_t hw_detect_compression(const uint8_t *data, size_t size) {
    if (size < 4) return HW_COMP_NONE;

    if ((data[0] == 0x78 && data[1] == 0x01) ||
        (data[0] == 0x78 && data[1] == 0x5E) ||
        (data[0] == 0x78 && data[1] == 0x9C) ||
        (data[0] == 0x78 && data[1] == 0xDA)) {
        return HW_COMP_ZLIB;
    }

    if (data[0] == 0x1F && data[1] == 0x8B) {
        return HW_COMP_GZIP;
    }

    if (data[0] == 0x5D && data[1] == 0x00 && data[2] == 0x00) {
        return HW_COMP_LZMA;
    }

    if (size >= 8 && data[0] == 0xFF && data[1] == 0xFF &&
        data[2] == 0xFF && data[3] == 0xFF) {
        return HW_COMP_JFFS2;
    }

    return HW_COMP_NONE;
}

size_t hw_zlib_decompress_bound(size_t src_size) {
    return src_size * 20 + 1024;
}

hw_error_t hw_zlib_decompress(const uint8_t *src, size_t src_size,
                               uint8_t **dst, size_t *dst_size) {
    z_stream strm;
    memset(&strm, 0, sizeof(strm));

    size_t out_size = hw_zlib_decompress_bound(src_size);
    uint8_t *out_buf = (uint8_t *)malloc(out_size);
    if (!out_buf) return HW_ERR_MEMORY;

    strm.next_in = (Bytef *)src;
    strm.avail_in = (uInt)src_size;
    strm.next_out = (Bytef *)out_buf;
    strm.avail_out = (uInt)out_size;

    int ret = inflateInit(&strm);
    if (ret != Z_OK) {
        free(out_buf);
        return HW_ERR_DECOMPRESS;
    }

    do {
        if (strm.avail_out == 0) {
            out_size *= 2;
            uint8_t *new_buf = (uint8_t *)realloc(out_buf, out_size);
            if (!new_buf) {
                inflateEnd(&strm);
                free(out_buf);
                return HW_ERR_MEMORY;
            }
            out_buf = new_buf;
            strm.next_out = (Bytef *)(out_buf + strm.total_out);
            strm.avail_out = (uInt)(out_size - strm.total_out);
        }
        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            inflateEnd(&strm);
            free(out_buf);
            return HW_ERR_DECOMPRESS;
        }
    } while (ret != Z_STREAM_END);

    inflateEnd(&strm);

    *dst = out_buf;
    *dst_size = strm.total_out;
    return HW_ERR_OK;
}

hw_error_t hw_zlib_compress(const uint8_t *src, size_t src_size,
                             uint8_t **dst, size_t *dst_size, int level) {
    uLong comp_size = compressBound((uLong)src_size);
    uint8_t *out_buf = (uint8_t *)malloc(comp_size);
    if (!out_buf) return HW_ERR_MEMORY;

    int ret = compress2(out_buf, &comp_size, src, (uLong)src_size, level);
    if (ret != Z_OK) {
        free(out_buf);
        return HW_ERR_COMPRESS;
    }

    *dst = out_buf;
    *dst_size = (size_t)comp_size;
    return HW_ERR_OK;
}

hw_error_t hw_gzip_decompress(const uint8_t *src, size_t src_size,
                               uint8_t **dst, size_t *dst_size) {
    z_stream strm;
    memset(&strm, 0, sizeof(strm));

    size_t out_size = hw_zlib_decompress_bound(src_size);
    uint8_t *out_buf = (uint8_t *)malloc(out_size);
    if (!out_buf) return HW_ERR_MEMORY;

    strm.next_in = (Bytef *)src;
    strm.avail_in = (uInt)src_size;
    strm.next_out = (Bytef *)out_buf;
    strm.avail_out = (uInt)out_size;

    int ret = inflateInit2(&strm, 15 + 32);
    if (ret != Z_OK) {
        free(out_buf);
        return HW_ERR_DECOMPRESS;
    }

    do {
        if (strm.avail_out == 0) {
            out_size *= 2;
            uint8_t *new_buf = (uint8_t *)realloc(out_buf, out_size);
            if (!new_buf) {
                inflateEnd(&strm);
                free(out_buf);
                return HW_ERR_MEMORY;
            }
            out_buf = new_buf;
            strm.next_out = (Bytef *)(out_buf + strm.total_out);
            strm.avail_out = (uInt)(out_size - strm.total_out);
        }
        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            inflateEnd(&strm);
            free(out_buf);
            return HW_ERR_DECOMPRESS;
        }
    } while (ret != Z_STREAM_END);

    inflateEnd(&strm);

    *dst = out_buf;
    *dst_size = strm.total_out;
    return HW_ERR_OK;
}

hw_error_t hw_gzip_compress(const uint8_t *src, size_t src_size,
                             uint8_t **dst, size_t *dst_size) {
    z_stream strm;
    memset(&strm, 0, sizeof(strm));

    size_t out_size = compressBound((uLong)src_size) + 32;
    uint8_t *out_buf = (uint8_t *)malloc(out_size);
    if (!out_buf) return HW_ERR_MEMORY;

    int ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                            15 + 16, 8, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK) {
        free(out_buf);
        return HW_ERR_COMPRESS;
    }

    strm.next_in = (Bytef *)src;
    strm.avail_in = (uInt)src_size;
    strm.next_out = (Bytef *)out_buf;
    strm.avail_out = (uInt)out_size;

    ret = deflate(&strm, Z_FINISH);
    if (ret != Z_STREAM_END) {
        deflateEnd(&strm);
        free(out_buf);
        return HW_ERR_COMPRESS;
    }

    deflateEnd(&strm);

    *dst = out_buf;
    *dst_size = strm.total_out;
    return HW_ERR_OK;
}
