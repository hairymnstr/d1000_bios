#include <stdlib.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "cmd_ps.h"

int cmd_ps(int argc __attribute__((__unused__)), char **argv __attribute__((__unused__)))
{
    char *buffer = (char *)malloc(uxTaskGetNumberOfTasks() * 64);

    vTaskList(buffer);

    printf(buffer);

    free(buffer);

    return 0;
}