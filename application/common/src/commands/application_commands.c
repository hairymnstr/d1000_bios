#include <stdlib.h>
#include "drash.h"

#include "cmd_ps.h"
#include "cmd_check_clocks.h"

drash_command_t application_commands[] = {
    {
        .name = "ps",
        .func = cmd_ps,
    },
    {
        .name = "check-clocks",
        .func = cmd_check_clocks,
    }
};

size_t application_command_count = sizeof(application_commands) / sizeof(drash_command_t);
