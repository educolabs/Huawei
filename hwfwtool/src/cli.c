#include "hwfwtool/cli.h"
#include "hwfwtool/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void hw_cli_print_version(void) {
    printf("%s v%s\n", HWFWTOOL_NAME, HWFWTOOL_VERSION);
    printf("Huawei Firmware Editor Tool\n");
    printf("Supports: HG8145V5 and compatible devices\n");
}

void hw_cli_print_usage(void) {
    printf("\n");
    printf("Usage: " HWFWTOOL_NAME " <command> [options] <input>\n");
    printf("\n");
    printf("Commands:\n");
    printf("  info      <firmware.bin>                   Show firmware information\n");
    printf("  extract   <firmware.bin> [-o output_dir]   Extract firmware blocks\n");
    printf("  repack    <input_dir> [-o output.bin]      Repack firmware from extracted blocks\n");
    printf("  verify    <firmware.bin>                   Verify firmware block CRCs\n");
    printf("  sign      <firmware.bin> [-k key]          Sign firmware (requires key)\n");
    printf("  help      [command]                        Show help\n");
    printf("  version                                    Show version\n");
    printf("\n");
    printf("Options:\n");
    printf("  -o, --output <path>     Output file or directory\n");
    printf("  -v, --verbose           Enable verbose output\n");
    printf("  -f, --force             Force overwrite existing files\n");
    printf("  --no-compress           Disable compression during repack\n");
    printf("  --skip-crc              Skip CRC verification\n");
    printf("  --skip-verify           Skip firmware verification\n");
    printf("  -b, --block <name>      Extract specific block only\n");
    printf("  -k, --key <path>        Signing key file\n");
    printf("  -c, --config <path>     Configuration file\n");
    printf("  -q, --quiet             Quiet mode (minimal output)\n");
    printf("  -h, --help              Show this help\n");
    printf("\n");
    printf("Examples:\n");
    printf("  " HWFWTOOL_NAME " info firmware.bin\n");
    printf("  " HWFWTOOL_NAME " extract firmware.bin -o extracted/\n");
    printf("  " HWFWTOOL_NAME " extract firmware.bin -b KERNEL -o kernel.bin\n");
    printf("  " HWFWTOOL_NAME " repack extracted/ -o repacked.bin\n");
    printf("  " HWFWTOOL_NAME " verify firmware.bin\n");
    printf("\n");
}

void hw_cli_print_help(hw_command_t cmd) {
    switch (cmd) {
        case HW_CMD_INFO:
            printf("\nCommand: info\n");
            printf("Usage: " HWFWTOOL_NAME " info <firmware.bin>\n");
            printf("\nDisplays detailed information about the firmware file including:\n");
            printf("  - Header magic and version\n");
            printf("  - Product info, carrier, region\n");
            printf("  - Block table with sizes and CRCs\n");
            printf("  - Compression types\n");
            printf("  - Signature status\n");
            printf("\n");
            break;
        case HW_CMD_EXTRACT:
            printf("\nCommand: extract\n");
            printf("Usage: " HWFWTOOL_NAME " extract <firmware.bin> [-o output_dir]\n");
            printf("\nExtracts all firmware blocks into subdirectories:\n");
            printf("  raw/           - Raw block binaries\n");
            printf("  decompressed/  - Decompressed block data\n");
            printf("  trailer/       - Trailer/signature data\n");
            printf("  config.ini     - Configuration file\n");
            printf("  manifest.xml   - XML manifest\n");
            printf("\nOptions:\n");
            printf("  -b, --block <name>  Extract only the specified block\n");
            printf("\n");
            break;
        case HW_CMD_REPACK:
            printf("\nCommand: repack\n");
            printf("Usage: " HWFWTOOL_NAME " repack <input_dir> [-o output.bin]\n");
            printf("\nRepacks firmware from previously extracted blocks.\n");
            printf("Reads raw block data from input_dir/raw/ and reconstructs\n");
            printf("the firmware binary with correct headers and CRCs.\n");
            printf("\nOptions:\n");
            printf("  --no-compress    Disable compression\n");
            printf("  --skip-crc       Skip CRC recalculation\n");
            printf("\n");
            break;
        case HW_CMD_VERIFY:
            printf("\nCommand: verify\n");
            printf("Usage: " HWFWTOOL_NAME " verify <firmware.bin>\n");
            printf("\nVerifies the CRC32 checksum of each block in the firmware.\n");
            printf("\n");
            break;
        case HW_CMD_SIGN:
            printf("\nCommand: sign\n");
            printf("Usage: " HWFWTOOL_NAME " sign <firmware.bin> -k <key_file>\n");
            printf("\nSigns the firmware with the provided key.\n");
            printf("WARNING: Invalid signatures will brick your device!\n");
            printf("\n");
            break;
        default:
            hw_cli_print_usage();
            break;
    }
}

static hw_command_t parse_command(const char *arg) {
    if (strcmp(arg, "info") == 0)      return HW_CMD_INFO;
    if (strcmp(arg, "extract") == 0)    return HW_CMD_EXTRACT;
    if (strcmp(arg, "repack") == 0)     return HW_CMD_REPACK;
    if (strcmp(arg, "verify") == 0)     return HW_CMD_VERIFY;
    if (strcmp(arg, "sign") == 0)       return HW_CMD_SIGN;
    if (strcmp(arg, "help") == 0)       return HW_CMD_HELP;
    if (strcmp(arg, "version") == 0)    return HW_CMD_VERSION;
    if (strcmp(arg, "--version") == 0)  return HW_CMD_VERSION;
    if (strcmp(arg, "-v") == 0 || strcmp(arg, "--version") == 0) return HW_CMD_VERSION;
    if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0)    return HW_CMD_HELP;
    return HW_CMD_NONE;
}

hw_error_t hw_cli_parse(int argc, char **argv, hw_cli_args_t *args) {
    memset(args, 0, sizeof(hw_cli_args_t));
    args->options.log_level = 1;

    if (argc < 2) {
        hw_cli_print_usage();
        return HW_ERR_INVALID_ARG;
    }

    args->command = parse_command(argv[1]);
    if (args->command == HW_CMD_NONE) {
        hw_log_error("Unknown command: %s", argv[1]);
        hw_cli_print_usage();
        return HW_ERR_INVALID_ARG;
    }

    if (args->command == HW_CMD_HELP) {
        if (argc > 2) {
            hw_command_t subcmd = parse_command(argv[2]);
            hw_cli_print_help(subcmd);
        } else {
            hw_cli_print_usage();
        }
        return HW_ERR_OK;
    }

    if (args->command == HW_CMD_VERSION) {
        return HW_ERR_OK;
    }

    int positional = 0;
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                args->options.output_file = argv[++i];
            } else {
                hw_log_error("Missing argument for -o");
                return HW_ERR_INVALID_ARG;
            }
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            args->options.verbose = true;
            args->options.log_level = 2;
        } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--force") == 0) {
            args->options.force = true;
        } else if (strcmp(argv[i], "--no-compress") == 0) {
            args->options.no_compress = true;
        } else if (strcmp(argv[i], "--skip-crc") == 0) {
            args->options.skip_crc = true;
        } else if (strcmp(argv[i], "--skip-verify") == 0) {
            args->options.skip_verify = true;
        } else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--block") == 0) {
            if (i + 1 < argc) {
                args->options.block_name = argv[++i];
            } else {
                hw_log_error("Missing argument for -b");
                return HW_ERR_INVALID_ARG;
            }
        } else if (strcmp(argv[i], "-k") == 0 || strcmp(argv[i], "--key") == 0) {
            if (i + 1 < argc) {
                args->options.sign_key = argv[++i];
            } else {
                hw_log_error("Missing argument for -k");
                return HW_ERR_INVALID_ARG;
            }
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) {
            if (i + 1 < argc) {
                args->options.config_file = argv[++i];
            } else {
                hw_log_error("Missing argument for -c");
                return HW_ERR_INVALID_ARG;
            }
        } else if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0) {
            args->options.log_level = 0;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            hw_cli_print_help(args->command);
            return HW_ERR_OK;
        } else if (argv[i][0] != '-') {
            if (positional == 0) {
                args->options.input_file = argv[i];
            } else {
                args->options.output_dir = argv[i];
            }
            positional++;
        } else {
            hw_log_error("Unknown option: %s", argv[i]);
            return HW_ERR_INVALID_ARG;
        }
    }

    if (!args->options.input_file && args->command != HW_CMD_REPACK) {
        hw_log_error("No input file specified");
        return HW_ERR_INVALID_ARG;
    }

    if (!args->options.input_file && args->command == HW_CMD_REPACK) {
        hw_log_error("No input directory specified");
        return HW_ERR_INVALID_ARG;
    }

    if (args->command == HW_CMD_EXTRACT && !args->options.output_file) {
        args->options.output_dir = "extracted";
    }

    if (args->command == HW_CMD_REPACK && !args->options.output_file) {
        args->options.output_file = "repacked.bin";
    }

    return HW_ERR_OK;
}
