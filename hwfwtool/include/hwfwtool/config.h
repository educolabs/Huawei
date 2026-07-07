#ifndef HWFWTOOL_CONFIG_H
#define HWFWTOOL_CONFIG_H

#include "common.h"

hw_error_t hw_config_generate(const hw_firmware_t *fw, const char *output_dir);
hw_error_t hw_config_load(hw_firmware_t *fw, const char *config_path);
hw_error_t hw_config_save_manifest(const hw_firmware_t *fw, const char *output_dir);
hw_error_t hw_config_save_block_list(const hw_firmware_t *fw, const char *output_dir);

#endif
