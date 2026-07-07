#include "hwfwtool/firmware.h"
#include "hwfwtool/compress.h"
#include "hwfwtool/crypto.h"
#include "hwfwtool/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

hw_error_t hw_firmware_init(hw_firmware_t *fw) {
    memset(fw, 0, sizeof(hw_firmware_t));
    return HW_ERR_OK;
}

void hw_firmware_cleanup(hw_firmware_t *fw) {
    if (fw->trailer_data) {
        free(fw->trailer_data);
        fw->trailer_data = NULL;
    }
}

hw_error_t hw_firmware_load(hw_firmware_t *fw, const char *path) {
    uint8_t *data = NULL;
    size_t size = 0;

    hw_error_t err = hw_file_read(path, &data, &size);
    if (err != HW_ERR_OK) return err;

    hw_log_info("Loaded firmware: %s (%zu bytes, %.2f MB)",
                path, size, (double)size / (1024.0 * 1024.0));

    err = hw_firmware_parse_header(fw, data, size);
    if (err != HW_ERR_OK) {
        free(data);
        return err;
    }

    err = hw_firmware_parse_blocks(fw, data, size);
    if (err != HW_ERR_OK) {
        free(data);
        return err;
    }

    err = hw_firmware_detect_compression(fw, data, size);
    if (err != HW_ERR_OK) {
        free(data);
        return err;
    }

    err = hw_firmware_parse_trailer(fw, data, size);
    if (err != HW_ERR_OK) {
        free(data);
        return err;
    }

    fw->is_signed = hw_check_signature(data, size);
    fw->file_size = size;

    free(data);
    return HW_ERR_OK;
}

hw_error_t hw_firmware_save(hw_firmware_t *fw, const char *path) {
    (void)fw;
    (void)path;
    hw_log_error("Firmware save not yet implemented");
    return HW_ERR_IO;
}

hw_error_t hw_firmware_parse_header(hw_firmware_t *fw, const uint8_t *data, size_t size) {
    if (size < HWNP_HEADER_SIZE + HWNP_PRODUCT_SIZE) {
        return HW_ERR_TRUNCATED;
    }

    if (memcmp(data, HWNP_MAGIC, HWNP_MAGIC_SIZE) != 0) {
        hw_log_error("Invalid firmware magic: expected '%s'", HWNP_MAGIC);
        return HW_ERR_MAGIC;
    }

    memcpy(fw->magic, data, HWNP_MAGIC_SIZE);
    fw->magic[HWNP_MAGIC_SIZE] = '\0';

    fw->header_hash = *(uint32_t *)(data + 4);
    fw->file_hash = *(uint32_t *)(data + 8);

    size_t product_start = 0x20;
    while (product_start < 0x40 && data[product_start] == '\0') {
        product_start++;
    }

    size_t product_len = 0;
    while (product_start + product_len < size && product_len < HWNP_PRODUCT_SIZE &&
           data[product_start + product_len] != '\0') {
        product_len++;
    }
    memcpy(fw->product_info, data + product_start, product_len);
    fw->product_info[product_len] = '\0';

    char *parts[8];
    char product_copy[256];
    strncpy(product_copy, fw->product_info, sizeof(product_copy) - 1);
    product_copy[sizeof(product_copy) - 1] = '\0';

    int part_count = 0;
    char *tok = strtok(product_copy, "|");
    while (tok && part_count < 8) {
        parts[part_count++] = tok;
        tok = strtok(NULL, "|");
    }

    if (part_count >= 6) {
        strncpy(fw->carrier, parts[0], sizeof(fw->carrier) - 1);
        strncpy(fw->region, parts[3], sizeof(fw->region) - 1);
        strncpy(fw->country, parts[4], sizeof(fw->country) - 1);
    }

    for (size_t i = 0x100; i < size - 20 && i < 0x1000; i++) {
        if (data[i] == 'V' && data[i + 1] == '5' && data[i + 2] == '0' &&
            data[i + 3] == '0' && data[i + 4] == 'R') {
            size_t ver_len = 0;
            while (i + ver_len < size && data[i + ver_len] != '\0' && ver_len < HWNP_VERSION_MAX - 1) {
                ver_len++;
            }
            memcpy(fw->firmware_version, data + i, ver_len);
            fw->firmware_version[ver_len] = '\0';
            break;
        }
    }

    hw_log_debug("Header: magic=%s, version=%s, product=%s",
                 fw->magic, fw->firmware_version, fw->product_info);

    return HW_ERR_OK;
}

hw_error_t hw_firmware_parse_blocks(hw_firmware_t *fw, const uint8_t *data, size_t size) {
    uint32_t block_count = 0;
    uint64_t data_offset = HWNP_DATA_START;

    const uint64_t BLOCK_TABLE_START = 0x120;

    for (uint32_t i = 0; i < HWNP_MAX_BLOCKS; i++) {
        uint64_t entry_base = BLOCK_TABLE_START + (i * HWNP_BLOCK_DESC_SIZE);
        uint64_t name_offset = entry_base + HWNP_NAME_OFFSET;
        uint64_t meta_offset = entry_base + HWNP_META_OFFSET;

        if (meta_offset + sizeof(hw_block_meta_t) > size) break;

        uint32_t index = *(uint32_t *)(data + meta_offset);
        uint32_t crc = *(uint32_t *)(data + meta_offset + 4);
        uint32_t timestamp = *(uint32_t *)(data + meta_offset + 8);
        uint32_t comp_size = *(uint32_t *)(data + meta_offset + 12);

        if (index == 0 || index > 100) continue;

        uint64_t flash_offset = meta_offset + sizeof(hw_block_meta_t);
        char flash_path[HWNP_PATH_MAX];
        memset(flash_path, 0, sizeof(flash_path));
        size_t fp_len = 0;
        while (flash_offset + fp_len < size && fp_len < HWNP_PATH_MAX - 1 &&
               data[flash_offset + fp_len] != '\0') {
            fp_len++;
        }
        memcpy(flash_path, data + flash_offset, fp_len);

        if (fp_len == 0) continue;

        hw_block_info_t *block = &fw->blocks[block_count];
        memset(block, 0, sizeof(hw_block_info_t));

        if (name_offset < size - HWNP_NAME_MAX) {
            const uint8_t *name_ptr = data + name_offset;
            size_t name_len = 0;

            while (name_len < HWNP_NAME_MAX && name_ptr[name_len] != '\0') {
                uint8_t c = name_ptr[name_len];
                if (!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')) {
                    break;
                }
                name_len++;
            }

            if (name_len >= 3) {
                memcpy(block->name, name_ptr, name_len);
            }
        }

        if (block->name[0] == '\0') {
            const char *fp = flash_path;
            if (strncmp(fp, "flash:", 6) == 0) fp += 6;
            else if (strncmp(fp, "file:", 5) == 0) fp += 5;

            const char *last_slash = strrchr(fp, '/');
            if (last_slash) last_slash++;
            else last_slash = fp;

            size_t auto_len = strlen(last_slash);
            if (auto_len >= HWNP_NAME_MAX) auto_len = HWNP_NAME_MAX - 1;
            memcpy(block->name, last_slash, auto_len);
            for (size_t c = 0; c < auto_len; c++) {
                if (block->name[c] >= 'a' && block->name[c] <= 'z')
                    block->name[c] -= 32;
                if (block->name[c] == '.') block->name[c] = '_';
            }
        }

        uint64_t ver_offset = name_offset + HWNP_NAME_MAX;
        if (ver_offset < size) {
            size_t ver_len = 0;
            while (ver_offset + ver_len < size && ver_len < HWNP_VERSION_MAX - 1 &&
                   data[ver_offset + ver_len] != '\0') {
                if (data[ver_offset + ver_len] == 'V' && ver_len == 0) {
                    ver_len++;
                    continue;
                }
                if (ver_len > 0 && data[ver_offset + ver_len] >= ' ' &&
                    data[ver_offset + ver_len] < 127) {
                    ver_len++;
                    continue;
                }
                break;
            }
            if (ver_len > 3) {
                memcpy(block->version, data + ver_offset, ver_len);
                block->version[ver_len] = '\0';
                block->has_version = true;
                if (strlen(fw->firmware_version) == 0) {
                    strncpy(fw->firmware_version, block->version, HWNP_VERSION_MAX - 1);
                }
            }
        }

        snprintf(block->flash_path, HWNP_PATH_MAX, "%s", flash_path);
        block->index = index;
        block->crc32 = crc;
        block->timestamp = timestamp;
        block->comp_size = comp_size;
        block->data_offset = data_offset;

        if (strncmp(flash_path, "flash:", 6) == 0) {
            block->target = HW_BLOCK_FLASH;
        } else if (strncmp(flash_path, "file:", 5) == 0) {
            block->target = HW_BLOCK_FILE;
        }

        if (data_offset + comp_size <= size) {
            block->compression = hw_detect_compression(data + data_offset,
                                                        comp_size < 256 ? comp_size : 256);
        }

        data_offset += comp_size;
        block_count++;

        hw_log_debug("Block %u: name=%s, flash=%s, size=%u, offset=0x%llx",
                     index, block->name, block->flash_path, comp_size,
                     (unsigned long long)block->data_offset);
    }

    fw->block_count = block_count;
    fw->data_start = HWNP_DATA_START;
    fw->data_end = data_offset;

    if (block_count == 0) {
        hw_log_error("No blocks found in firmware");
        return HW_ERR_BLOCK_COUNT;
    }

    return HW_ERR_OK;
}

hw_error_t hw_firmware_parse_trailer(hw_firmware_t *fw, const uint8_t *data, size_t size) {
    uint64_t trailer_start = fw->data_end;

    if (trailer_start >= size) {
        fw->trailer_offset = size;
        fw->trailer_size = 0;
        return HW_ERR_OK;
    }

    fw->trailer_offset = trailer_start;
    fw->trailer_size = size - trailer_start;

    for (uint64_t i = trailer_start; i < size - 10; i++) {
        if (data[i] == 'H' && data[i + 1] == 'W' &&
            (data[i + 2] == 0x00 || data[i + 2] == 'N')) {
            fw->has_capability = true;
            break;
        }
    }

    size_t trailer_copy_size = fw->trailer_size < 0x10000 ? fw->trailer_size : 0x10000;
    fw->trailer_data = (uint8_t *)malloc(trailer_copy_size);
    if (fw->trailer_data) {
        memcpy(fw->trailer_data, data + trailer_start, trailer_copy_size);
        fw->trailer_data_size = trailer_copy_size;
    }

    hw_log_debug("Trailer: offset=0x%llx, size=%llu",
                 (unsigned long long)fw->trailer_offset,
                 (unsigned long long)fw->trailer_size);

    return HW_ERR_OK;
}

hw_error_t hw_firmware_detect_compression(hw_firmware_t *fw, const uint8_t *data, size_t size) {
    for (uint32_t i = 0; i < fw->block_count; i++) {
        hw_block_info_t *block = &fw->blocks[i];
        if (block->data_offset + block->comp_size <= size && block->comp_size > 0) {
            block->compression = hw_detect_compression(data + block->data_offset,
                                                        block->comp_size < 256 ? block->comp_size : 256);
        }
    }
    return HW_ERR_OK;
}

void hw_firmware_print_info(const hw_firmware_t *fw) {
    printf("\n");
    printf("============================================================\n");
    printf("  HUAWEI FIRMWARE INFORMATION\n");
    printf("============================================================\n");
    printf("\n");
    printf("  Magic:           %s\n", fw->magic);
    printf("  Firmware Ver:    %s\n", fw->firmware_version);
    printf("  Product Info:    %s\n", fw->product_info);
    printf("  Carrier:         %s\n", fw->carrier);
    printf("  Region:          %s\n", fw->region);
    printf("  Country:         %s\n", fw->country);
    printf("  File Size:       %llu bytes (%.2f MB)\n",
           (unsigned long long)fw->file_size,
           (double)fw->file_size / (1024.0 * 1024.0));
    printf("  Block Count:     %u\n", fw->block_count);
    printf("  Data Start:      0x%08llx\n", (unsigned long long)fw->data_start);
    printf("  Data End:        0x%08llx\n", (unsigned long long)fw->data_end);
    printf("  Trailer Offset:  0x%08llx\n", (unsigned long long)fw->trailer_offset);
    printf("  Trailer Size:    %llu bytes\n", (unsigned long long)fw->trailer_size);
    printf("  Signed:          %s\n", fw->is_signed ? "Yes" : "No");
    printf("  Has Capability:  %s\n", fw->has_capability ? "Yes" : "No");
    printf("  Header Hash:     0x%08x\n", fw->header_hash);
    printf("  File Hash:       0x%08x\n", fw->file_hash);
    printf("\n");
}

void hw_firmware_print_blocks(const hw_firmware_t *fw) {
    printf("  BLOCK TABLE\n");
    printf("  -----------------------------------------------------------\n");
    printf("  %-4s %-14s %-28s %-10s %-12s %-8s\n",
           "Idx", "Name", "Flash Path", "Size", "Compression", "CRC32");
    printf("  -----------------------------------------------------------\n");

    for (uint32_t i = 0; i < fw->block_count; i++) {
        const hw_block_info_t *b = &fw->blocks[i];
        printf("  %-4u %-14s %-28s %-10u %-12s 0x%08x\n",
               b->index, b->name, b->flash_path, b->comp_size,
               hw_compress_type_string(b->compression), b->crc32);
        printf("       Data Offset: 0x%08llx  Timestamp: 0x%08x",
               (unsigned long long)b->data_offset, b->timestamp);
        if (b->has_version) {
            printf("  Ver: %s", b->version);
        }
        printf("\n");
    }
    printf("  -----------------------------------------------------------\n\n");
}

hw_block_info_t *hw_firmware_find_block(hw_firmware_t *fw, const char *name) {
    for (uint32_t i = 0; i < fw->block_count; i++) {
        if (strcmp(fw->blocks[i].name, name) == 0) {
            return &fw->blocks[i];
        }
    }
    return NULL;
}
