#include "hwfwtool/blocks.h"
#include "hwfwtool/compress.h"
#include "hwfwtool/crypto.h"
#include "hwfwtool/config.h"
#include "hwfwtool/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

hw_error_t hw_blocks_extract(const hw_firmware_t *fw, const uint8_t *data, size_t size,
                              const char *output_dir, const hw_options_t *opts) {
    hw_error_t err;

    err = hw_mkdir_p(output_dir);
    if (err != HW_ERR_OK) {
        hw_log_error("Cannot create output directory: %s", output_dir);
        return err;
    }

    char *raw_dir = hw_path_join(output_dir, "raw");
    char *decomp_dir = hw_path_join(output_dir, "decompressed");
    if (!raw_dir || !decomp_dir) {
        hw_free(raw_dir);
        hw_free(decomp_dir);
        return HW_ERR_MEMORY;
    }

    hw_mkdir_p(raw_dir);
    hw_mkdir_p(decomp_dir);

    printf("\nExtracting %u blocks...\n\n", fw->block_count);

    for (uint32_t i = 0; i < fw->block_count; i++) {
        const hw_block_info_t *block = &fw->blocks[i];

        if (block->data_offset + block->comp_size > size) {
            hw_log_warn("Block '%s' extends beyond file, skipping", block->name);
            continue;
        }

        printf("  [%u/%u] %s (%s)\n", i + 1, fw->block_count, block->name, block->flash_path);
        printf("         Size: %u bytes, Compression: %s, CRC: 0x%08x\n",
               block->comp_size, hw_compress_type_string(block->compression), block->crc32);

        char raw_filename[256];
        if (strcmp(block->name, "UNKNOWN") == 0 || block->name[0] == '\0') {
            sprintf(raw_filename, "BLOCK_%u.bin", block->index);
        } else {
            sprintf(raw_filename, "%s.bin", block->name);
        }
        char *raw_path = hw_path_join(raw_dir, raw_filename);

        err = hw_file_write(raw_path, data + block->data_offset, block->comp_size);
        if (err != HW_ERR_OK) {
            hw_log_error("Failed to write raw block: %s", raw_path);
        } else {
            printf("         Raw: %s\n", raw_path);
        }

        if (!opts->skip_crc && block->comp_size > 0) {
            hw_error_t crc_err = hw_verify_block_crc(data + block->data_offset,
                                                      block->comp_size, block->crc32);
            if (crc_err == HW_ERR_CRC) {
                printf("         CRC: MISMATCH (expected 0x%08x)\n", block->crc32);
            } else {
                printf("         CRC: OK\n");
            }
        }

        if (block->compression == HW_COMP_ZLIB || block->compression == HW_COMP_GZIP) {
            uint8_t *decomp_data = NULL;
            size_t decomp_size = 0;

            hw_error_t decomp_err;
            if (block->compression == HW_COMP_ZLIB) {
                decomp_err = hw_zlib_decompress(data + block->data_offset,
                                                 block->comp_size,
                                                 &decomp_data, &decomp_size);
            } else {
                decomp_err = hw_gzip_decompress(data + block->data_offset,
                                                 block->comp_size,
                                                 &decomp_data, &decomp_size);
            }

            if (decomp_err == HW_ERR_OK && decomp_data) {
                char *decomp_filename = (char *)malloc(strlen(block->name) + 8);
                sprintf(decomp_filename, "%s.dec", block->name);
                char *decomp_path = hw_path_join(decomp_dir, decomp_filename);

                err = hw_file_write(decomp_path, decomp_data, decomp_size);
                if (err == HW_ERR_OK) {
                    printf("         Decompressed: %s (%zu bytes)\n", decomp_path, decomp_size);
                }

                free(decomp_data);
                free(decomp_filename);
                free(decomp_path);
            } else {
                printf("         Decompression: skipped (no valid compressed data)\n");
            }
        } else if (block->compression == HW_COMP_JFFS2) {
            printf("         Type: JFFS2 filesystem (raw flash image)\n");
            char *jffs2_filename = (char *)malloc(strlen(block->name) + 8);
            sprintf(jffs2_filename, "%s.jffs2", block->name);
            char *jffs2_path = hw_path_join(decomp_dir, jffs2_filename);
            hw_file_write(jffs2_path, data + block->data_offset, block->comp_size);
            printf("         JFFS2: %s\n", jffs2_path);
            free(jffs2_filename);
            free(jffs2_path);
        } else {
            printf("         Type: Raw/uncompressed\n");
        }

        printf("\n");
        free(raw_path);
    }

    char *trailer_dir = hw_path_join(output_dir, "trailer");
    if (trailer_dir) {
        hw_mkdir_p(trailer_dir);
        if (fw->trailer_data && fw->trailer_data_size > 0) {
            char *trailer_path = hw_path_join(trailer_dir, "trailer.bin");
            if (trailer_path) {
                hw_file_write(trailer_path, data + fw->trailer_offset, fw->trailer_size);
                printf("  Trailer: %s (%llu bytes)\n", trailer_path,
                       (unsigned long long)fw->trailer_size);
                free(trailer_path);
            }
        }
        free(trailer_dir);
    }

    err = hw_config_generate(fw, output_dir);
    if (err == HW_ERR_OK) {
        printf("  Config: %s/config.ini\n", output_dir);
    }

    err = hw_config_save_manifest(fw, output_dir);
    if (err == HW_ERR_OK) {
        printf("  Manifest: %s/manifest.xml\n", output_dir);
    }

    free(raw_dir);
    free(decomp_dir);

    printf("\nExtraction complete.\n\n");
    return HW_ERR_OK;
}

hw_error_t hw_blocks_extract_single(const hw_firmware_t *fw, const uint8_t *data, size_t size,
                                     const char *block_name, const char *output_path) {
    for (uint32_t i = 0; i < fw->block_count; i++) {
        const hw_block_info_t *block = &fw->blocks[i];
        if (strcmp(block->name, block_name) != 0) continue;

        if (block->data_offset + block->comp_size > size) {
            return HW_ERR_TRUNCATED;
        }

        return hw_file_write(output_path, data + block->data_offset, block->comp_size);
    }
    return HW_ERR_NOT_FOUND;
}

hw_error_t hw_block_decompress(const uint8_t *src, size_t src_size,
                                uint8_t **dst, size_t *dst_size,
                                hw_compress_type_t type) {
    switch (type) {
        case HW_COMP_ZLIB:
            return hw_zlib_decompress(src, src_size, dst, dst_size);
        case HW_COMP_GZIP:
            return hw_gzip_decompress(src, src_size, dst, dst_size);
        case HW_COMP_NONE:
            *dst = (uint8_t *)malloc(src_size);
            if (!*dst) return HW_ERR_MEMORY;
            memcpy(*dst, src, src_size);
            *dst_size = src_size;
            return HW_ERR_OK;
        default:
            return HW_ERR_DECOMPRESS;
    }
}

hw_error_t hw_block_compress(const uint8_t *src, size_t src_size,
                              uint8_t **dst, size_t *dst_size,
                              hw_compress_type_t type) {
    switch (type) {
        case HW_COMP_ZLIB:
            return hw_zlib_compress(src, src_size, dst, dst_size, 6);
        case HW_COMP_GZIP:
            return hw_gzip_compress(src, src_size, dst, dst_size);
        case HW_COMP_NONE:
            *dst = (uint8_t *)malloc(src_size);
            if (!*dst) return HW_ERR_MEMORY;
            memcpy(*dst, src, src_size);
            *dst_size = src_size;
            return HW_ERR_OK;
        default:
            return HW_ERR_COMPRESS;
    }
}

hw_error_t hw_blocks_verify(const hw_firmware_t *fw, const uint8_t *data, size_t size) {
    printf("\nVerifying firmware blocks...\n\n");

    int pass = 0, fail = 0;
    for (uint32_t i = 0; i < fw->block_count; i++) {
        const hw_block_info_t *block = &fw->blocks[i];

        if (block->data_offset + block->comp_size > size) {
            printf("  [FAIL] %s: truncated\n", block->name);
            fail++;
            continue;
        }

        hw_error_t err = hw_verify_block_crc(data + block->data_offset,
                                              block->comp_size, block->crc32);
        if (err == HW_ERR_OK) {
            printf("  [ OK ] %s: CRC 0x%08x\n", block->name, block->crc32);
            pass++;
        } else {
            uint32_t computed = hw_crc32(data + block->data_offset, block->comp_size);
            printf("  [FAIL] %s: CRC mismatch (computed=0x%08x, expected=0x%08x)\n",
                   block->name, computed, block->crc32);
            fail++;
        }
    }

    printf("\n  Results: %d passed, %d failed, %d total\n\n", pass, fail, fw->block_count);
    return (fail > 0) ? HW_ERR_VERIFY : HW_ERR_OK;
}

hw_error_t hw_blocks_repack(hw_firmware_t *fw, const char *input_dir, const char *output_path,
                             const hw_options_t *opts) {
    (void)opts;
    printf("\nRepacking firmware from: %s\n", input_dir);
    printf("Output: %s\n\n", output_path);

    char *raw_dir = hw_path_join(input_dir, "raw");
    if (!raw_dir) return HW_ERR_MEMORY;

    if (!hw_dir_exists(raw_dir)) {
        hw_log_error("Raw blocks directory not found: %s", raw_dir);
        free(raw_dir);
        return HW_ERR_NOT_FOUND;
    }

    size_t total_data_size = 0;
    for (uint32_t i = 0; i < fw->block_count; i++) {
        total_data_size += fw->blocks[i].comp_size;
    }

    size_t output_size = HWNP_DATA_START + total_data_size + fw->trailer_size;
    uint8_t *output_data = (uint8_t *)calloc(1, output_size);
    if (!output_data) {
        free(raw_dir);
        return HW_ERR_MEMORY;
    }

    memcpy(output_data, "HWNP", 4);
    *(uint32_t *)(output_data + 4) = fw->header_hash;
    *(uint32_t *)(output_data + 8) = fw->file_hash;

    memcpy(output_data + 0x20, fw->product_info, strlen(fw->product_info));

    size_t current_data_offset = HWNP_DATA_START;
    for (uint32_t i = 0; i < fw->block_count; i++) {
        hw_block_info_t *block = &fw->blocks[i];

        char raw_filename[256];
        if (strcmp(block->name, "UNKNOWN") == 0 || block->name[0] == '\0') {
            sprintf(raw_filename, "BLOCK_%u.bin", block->index);
        } else {
            sprintf(raw_filename, "%s.bin", block->name);
        }
        char *raw_path = hw_path_join(raw_dir, raw_filename);

        uint8_t *block_data = NULL;
        size_t block_size = 0;
        hw_error_t err = hw_file_read(raw_path, &block_data, &block_size);

        if (err == HW_ERR_OK) {
            if (current_data_offset + block_size <= output_size) {
                memcpy(output_data + current_data_offset, block_data, block_size);
                block->crc32 = hw_crc32(block_data, block_size);
                block->comp_size = (uint32_t)block_size;
                printf("  Repacked: %s (%zu bytes, CRC=0x%08x)\n",
                       block->name, block_size, block->crc32);
            }
            free(block_data);
        } else {
            hw_log_warn("Block file not found: %s, using original", raw_path);
        }

        current_data_offset += block->comp_size;
        free(raw_path);
    }

    if (fw->trailer_data && fw->trailer_data_size > 0) {
        memcpy(output_data + current_data_offset, fw->trailer_data,
               fw->trailer_data_size < fw->trailer_size ? fw->trailer_data_size : fw->trailer_size);
    }

    uint32_t file_crc = hw_crc32(output_data, output_size);
    *(uint32_t *)(output_data + 16) = file_crc;

    hw_error_t err = hw_file_write(output_path, output_data, output_size);
    free(output_data);
    free(raw_dir);

    if (err == HW_ERR_OK) {
        printf("\nRepack complete: %s (%zu bytes)\n\n", output_path, output_size);
    }

    return err;
}
