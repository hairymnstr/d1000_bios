#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "cmd_check_clocks.h"

int cmd_check_clocks(int argc __attribute__((__unused__)), char **argv __attribute__((__unused__)))
{
    printf("Measuring clocks, please wait 10 seconds.\n");

    TickType_t enter_ticks = xTaskGetTickCount();
    time_t enter_time = time(NULL);
    uint32_t enter_fast_ticks = portGET_RUN_TIME_COUNTER_VALUE();

    vTaskDelay(configTICK_RATE_HZ * 10);

    TickType_t exit_ticks = xTaskGetTickCount();
    time_t exit_time = time(NULL);
    uint32_t exit_fast_ticks = portGET_RUN_TIME_COUNTER_VALUE();

    printf("In 10 seconds there were:\n");
    printf("  %u RTOS ticks\n", (unsigned int)(exit_ticks - enter_ticks));
    printf("  %u wall seconds\n", (unsigned int)(exit_time - enter_time));
    printf("  %u fast timer ticks\n", (unsigned int)(exit_fast_ticks - enter_fast_ticks));

    return 0;
}