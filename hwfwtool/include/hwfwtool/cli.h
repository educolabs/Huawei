#ifndef HWFWTOOL_CLI_H
#define HWFWTOOL_CLI_H

#include "common.h"

typedef enum {
    HW_CMD_NONE    = 0,
    HW_CMD_INFO    = 1,
    HW_CMD_EXTRACT = 2,
    HW_CMD_REPACK  = 3,
    HW_CMD_VERIFY  = 4,
    HW_CMD_SIGN    = 5,
    HW_CMD_HELP    = 6,
    HW_CMD_VERSION = 7,
} hw_command_t;

typedef struct {
    hw_command_t command;
    hw_options_t options;
} hw_cli_args_t;

hw_error_t hw_cli_parse(int argc, char **argv, hw_cli_args_t *args);
void hw_cli_print_usage(void);
void hw_cli_print_version(void);
void hw_cli_print_help(hw_command_t cmd);

#endif
