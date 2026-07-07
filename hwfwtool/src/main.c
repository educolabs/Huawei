#include "hwfwtool/common.h"
#include "hwfwtool/firmware.h"
#include "hwfwtool/blocks.h"
#include "hwfwtool/crypto.h"
#include "hwfwtool/config.h"
#include "hwfwtool/cli.h"
#include "hwfwtool/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cmd_info(const hw_options_t *opts) {
    hw_firmware_t fw;
    hw_firmware_init(&fw);

    hw_error_t err = hw_firmware_load(&fw, opts->input_file);
    if (err != HW_ERR_OK) {
        hw_log_error("Failed to load firmware: %s", hw_error_string(err));
        hw_firmware_cleanup(&fw);
        return 1;
    }

    hw_firmware_print_info(&fw);
    hw_firmware_print_blocks(&fw);

    hw_firmware_cleanup(&fw);
    return 0;
}

static int cmd_extract(const hw_options_t *opts) {
    hw_firmware_t fw;
    hw_firmware_init(&fw);

    uint8_t *data = NULL;
    size_t size = 0;

    hw_error_t err = hw_file_read(opts->input_file, &data, &size);
    if (err != HW_ERR_OK) {
        hw_log_error("Failed to read firmware: %s", opts->input_file);
        return 1;
    }

    err = hw_firmware_parse_header(&fw, data, size);
    if (err != HW_ERR_OK) {
        hw_log_error("Failed to parse header: %s", hw_error_string(err));
        free(data);
        hw_firmware_cleanup(&fw);
        return 1;
    }

    err = hw_firmware_parse_blocks(&fw, data, size);
    if (err != HW_ERR_OK) {
        hw_log_error("Failed to parse blocks: %s", hw_error_string(err));
        free(data);
        hw_firmware_cleanup(&fw);
        return 1;
    }

    hw_firmware_detect_compression(&fw, data, size);
    hw_firmware_parse_trailer(&fw, data, size);
    fw.file_size = size;
    fw.is_signed = hw_check_signature(data, size);

    const char *output_dir = opts->output_file ? opts->output_file : "extracted";

    if (opts->block_name) {
        char *out_path = hw_path_join(output_dir, opts->block_name);
        if (!out_path) {
            free(data);
            hw_firmware_cleanup(&fw);
            return 1;
        }
        char *out_file = (char *)malloc(strlen(out_path) + 8);
        sprintf(out_file, "%s.bin", out_path);
        err = hw_blocks_extract_single(&fw, data, size, opts->block_name, out_file);
        if (err == HW_ERR_OK) {
            printf("Extracted block '%s' to %s\n", opts->block_name, out_file);
        } else {
            hw_log_error("Failed to extract block '%s': %s", opts->block_name, hw_error_string(err));
        }
        free(out_path);
        free(out_file);
    } else {
        err = hw_blocks_extract(&fw, data, size, output_dir, opts);
    }

    free(data);
    hw_firmware_cleanup(&fw);
    return (err == HW_ERR_OK) ? 0 : 1;
}

static int cmd_repack(const hw_options_t *opts) {
    hw_firmware_t fw;
    hw_firmware_init(&fw);

    char *config_path = hw_path_join(opts->input_file, "config.ini");
    if (!config_path) return 1;

    if (hw_file_exists(config_path)) {
        hw_config_load(&fw, config_path);
    }
    free(config_path);

    const char *input_dir = opts->input_file;
    char *raw_dir = hw_path_join(input_dir, "raw");
    if (!raw_dir) return 1;

    if (!hw_dir_exists(raw_dir)) {
        hw_log_error("Input directory does not contain raw/ subdirectory: %s", input_dir);
        free(raw_dir);
        hw_firmware_cleanup(&fw);
        return 1;
    }

    char *manifest_path = hw_path_join(input_dir, "manifest.xml");
    if (manifest_path && hw_file_exists(manifest_path)) {
        hw_log_info("Found manifest, loading block info...");
    }
    free(manifest_path);

    if (fw.block_count == 0) {
        hw_log_info("No config found, attempting to load original firmware for metadata...");
        char *orig_firmware = hw_path_join(input_dir, "firmware.bin");
        if (orig_firmware && hw_file_exists(orig_firmware)) {
            hw_firmware_load(&fw, orig_firmware);
        }
        free(orig_firmware);
    }

    if (fw.block_count == 0) {
        hw_log_error("Cannot determine block layout. Extract a firmware first or provide config.");
        free(raw_dir);
        hw_firmware_cleanup(&fw);
        return 1;
    }

    const char *output_path = opts->output_file ? opts->output_file : "repacked.bin";
    hw_error_t err = hw_blocks_repack(&fw, input_dir, output_path, opts);

    free(raw_dir);
    hw_firmware_cleanup(&fw);
    return (err == HW_ERR_OK) ? 0 : 1;
}

static int cmd_verify(const hw_options_t *opts) {
    hw_firmware_t fw;
    hw_firmware_init(&fw);

    uint8_t *data = NULL;
    size_t size = 0;

    hw_error_t err = hw_file_read(opts->input_file, &data, &size);
    if (err != HW_ERR_OK) {
        hw_log_error("Failed to read firmware: %s", opts->input_file);
        return 1;
    }

    err = hw_firmware_parse_header(&fw, data, size);
    if (err != HW_ERR_OK) {
        free(data);
        hw_firmware_cleanup(&fw);
        return 1;
    }

    err = hw_firmware_parse_blocks(&fw, data, size);
    if (err != HW_ERR_OK) {
        free(data);
        hw_firmware_cleanup(&fw);
        return 1;
    }

    err = hw_blocks_verify(&fw, data, size);

    free(data);
    hw_firmware_cleanup(&fw);
    return (err == HW_ERR_OK) ? 0 : 1;
}

static int cmd_sign(const hw_options_t *opts) {
    if (!opts->sign_key) {
        hw_log_error("Signing requires a key file (-k option)");
        return 1;
    }

    hw_log_warn("Firmware signing is a dangerous operation.");
    hw_log_warn("An invalid signature will prevent the device from booting.");

    printf("\nSigning firmware: %s\n", opts->input_file);
    printf("Key file: %s\n", opts->sign_key);
    printf("Output: %s\n\n", opts->output_file ? opts->output_file : "signed.bin");

    hw_error_t err = hw_compute_signature(NULL, 0, opts->sign_key);
    return (err == HW_ERR_OK) ? 0 : 1;
}

int main(int argc, char **argv) {
    hw_cli_args_t args;
    hw_error_t err = hw_cli_parse(argc, argv, &args);

    if (err != HW_ERR_OK) {
        return 1;
    }

    hw_log_init(args.options.log_level);

    if (args.command == HW_CMD_VERSION) {
        hw_cli_print_version();
        return 0;
    }

    if (args.command == HW_CMD_HELP) {
        return 0;
    }

    int ret = 0;
    switch (args.command) {
        case HW_CMD_INFO:
            ret = cmd_info(&args.options);
            break;
        case HW_CMD_EXTRACT:
            ret = cmd_extract(&args.options);
            break;
        case HW_CMD_REPACK:
            ret = cmd_repack(&args.options);
            break;
        case HW_CMD_VERIFY:
            ret = cmd_verify(&args.options);
            break;
        case HW_CMD_SIGN:
            ret = cmd_sign(&args.options);
            break;
        default:
            hw_cli_print_usage();
            ret = 1;
            break;
    }

    return ret;
}
