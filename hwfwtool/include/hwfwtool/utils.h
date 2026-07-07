#ifndef HWFWTOOL_UTILS_H
#define HWFWTOOL_UTILS_H

#include "common.h"
#include <stdio.h>

void hw_log_init(int level);
void hw_log_error(const char *fmt, ...);
void hw_log_warn(const char *fmt, ...);
void hw_log_info(const char *fmt, ...);
void hw_log_debug(const char *fmt, ...);

hw_error_t hw_mkdir_p(const char *path);
hw_error_t hw_file_read(const char *path, uint8_t **data, size_t *size);
hw_error_t hw_file_write(const char *path, const uint8_t *data, size_t size);
bool hw_file_exists(const char *path);
bool hw_dir_exists(const char *path);
uint64_t hw_file_size(const char *path);
void hw_print_hex(const uint8_t *data, size_t size, size_t offset);
void hw_progress_bar(uint64_t current, uint64_t total);
char *hw_path_join(const char *dir, const char *file);
char *hw_strdup(const char *s);
void hw_free(void *p);
void *hw_malloc(size_t size);
void *hw_calloc(size_t count, size_t size);
void *hw_realloc(void *ptr, size_t size);

#endif
