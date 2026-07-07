#include "hwfwtool/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <unistd.h>
#endif

static int g_log_level = 1;

void hw_log_init(int level) {
    g_log_level = level;
}

void hw_log_error(const char *fmt, ...) {
    va_list args;
    fprintf(stderr, "[ERROR] ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void hw_log_warn(const char *fmt, ...) {
    if (g_log_level < 1) return;
    va_list args;
    fprintf(stderr, "[WARN]  ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void hw_log_info(const char *fmt, ...) {
    if (g_log_level < 1) return;
    va_list args;
    printf("[INFO]  ");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

void hw_log_debug(const char *fmt, ...) {
    if (g_log_level < 2) return;
    va_list args;
    printf("[DEBUG] ");
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

hw_error_t hw_mkdir_p(const char *path) {
    char tmp[1024];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (tmp[len - 1] == PATH_SEP || tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }

    for (p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == PATH_SEP) {
            *p = '\0';
#ifdef _WIN32
            if (_mkdir(tmp) != 0 && errno != EEXIST) {
#else
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
#endif
                return HW_ERR_IO;
            }
            *p = PATH_SEP;
        }
    }
#ifdef _WIN32
    if (_mkdir(tmp) != 0 && errno != EEXIST) {
#else
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
#endif
        return HW_ERR_IO;
    }
    return HW_ERR_OK;
}

hw_error_t hw_file_read(const char *path, uint8_t **data, size_t *size) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        hw_log_error("Cannot open file: %s", path);
        return HW_ERR_IO;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fsize <= 0) {
        fclose(f);
        return HW_ERR_IO;
    }

    uint8_t *buf = (uint8_t *)malloc((size_t)fsize);
    if (!buf) {
        fclose(f);
        return HW_ERR_MEMORY;
    }

    size_t read_bytes = fread(buf, 1, (size_t)fsize, f);
    fclose(f);

    if (read_bytes != (size_t)fsize) {
        free(buf);
        return HW_ERR_IO;
    }

    *data = buf;
    *size = (size_t)fsize;
    return HW_ERR_OK;
}

hw_error_t hw_file_write(const char *path, const uint8_t *data, size_t size) {
    FILE *f = fopen(path, "wb");
    if (!f) {
        hw_log_error("Cannot create file: %s", path);
        return HW_ERR_IO;
    }

    size_t written = fwrite(data, 1, size, f);
    fclose(f);

    if (written != size) {
        return HW_ERR_IO;
    }

    return HW_ERR_OK;
}

bool hw_file_exists(const char *path) {
    FILE *f = fopen(path, "rb");
    if (f) {
        fclose(f);
        return true;
    }
    return false;
}

bool hw_dir_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

uint64_t hw_file_size(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return (uint64_t)st.st_size;
}

void hw_print_hex(const uint8_t *data, size_t size, size_t offset) {
    for (size_t i = 0; i < size && i < 256; i += 16) {
        printf("  %08zx: ", offset + i);
        for (size_t j = 0; j < 16; j++) {
            if (i + j < size) {
                printf("%02x ", data[i + j]);
            } else {
                printf("   ");
            }
        }
        printf(" ");
        for (size_t j = 0; j < 16 && i + j < size; j++) {
            uint8_t c = data[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }
        printf("\n");
    }
}

void hw_progress_bar(uint64_t current, uint64_t total) {
    if (total == 0) return;
    int pct = (int)((current * 100) / total);
    int bar_width = 40;
    int filled = (bar_width * pct) / 100;

    printf("\r  [");
    for (int i = 0; i < bar_width; i++) {
        printf("%c", i < filled ? '#' : '-');
    }
    printf("] %3d%% (%llu / %llu)", pct,
           (unsigned long long)current, (unsigned long long)total);
    fflush(stdout);

    if (current >= total) {
        printf("\n");
    }
}

char *hw_path_join(const char *dir, const char *file) {
    size_t dlen = strlen(dir);
    size_t flen = strlen(file);
    char *result = (char *)malloc(dlen + flen + 2);
    if (!result) return NULL;

    memcpy(result, dir, dlen);
    if (dlen > 0 && dir[dlen - 1] != '/' && dir[dlen - 1] != PATH_SEP) {
        result[dlen] = PATH_SEP;
        dlen++;
    }
    memcpy(result + dlen, file, flen + 1);
    return result;
}

char *hw_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char *dup = (char *)malloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}

void hw_free(void *p) {
    free(p);
}

void *hw_malloc(size_t size) {
    void *p = malloc(size);
    if (!p) hw_log_error("Memory allocation failed (%zu bytes)", size);
    return p;
}

void *hw_calloc(size_t count, size_t size) {
    void *p = calloc(count, size);
    if (!p) hw_log_error("Memory allocation failed (%zu * %zu bytes)", count, size);
    return p;
}

void *hw_realloc(void *ptr, size_t size) {
    void *p = realloc(ptr, size);
    if (!p && size > 0) hw_log_error("Memory reallocation failed (%zu bytes)", size);
    return p;
}

const char *hw_error_string(hw_error_t err) {
    switch (err) {
        case HW_ERR_OK:           return "OK";
        case HW_ERR_MEMORY:       return "Memory allocation failed";
        case HW_ERR_IO:           return "I/O error";
        case HW_ERR_FORMAT:       return "Invalid firmware format";
        case HW_ERR_MAGIC:        return "Invalid magic number";
        case HW_ERR_CRC:          return "CRC mismatch";
        case HW_ERR_COMPRESS:     return "Compression failed";
        case HW_ERR_DECOMPRESS:   return "Decompression failed";
        case HW_ERR_SIGN:         return "Signing failed";
        case HW_ERR_VERIFY:       return "Verification failed";
        case HW_ERR_INVALID_ARG:  return "Invalid argument";
        case HW_ERR_NOT_FOUND:    return "Not found";
        case HW_ERR_BLOCK_COUNT:  return "Invalid block count";
        case HW_ERR_TRUNCATED:    return "File truncated";
        default:                  return "Unknown error";
    }
}

const char *hw_compress_type_string(hw_compress_type_t type) {
    switch (type) {
        case HW_COMP_NONE:    return "none";
        case HW_COMP_ZLIB:    return "zlib";
        case HW_COMP_LZMA:    return "lzma";
        case HW_COMP_GZIP:    return "gzip";
        case HW_COMP_JFFS2:   return "jffs2";
        case HW_COMP_UNKNOWN: return "unknown";
        default:              return "invalid";
    }
}

const char *hw_block_target_string(hw_block_target_t target) {
    switch (target) {
        case HW_BLOCK_FLASH: return "flash";
        case HW_BLOCK_FILE:  return "file";
        default:             return "unknown";
    }
}
