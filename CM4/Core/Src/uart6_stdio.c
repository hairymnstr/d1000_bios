#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "main.h"

extern UART_HandleTypeDef huart6;

int _write(int fd, char *ptr, int len)
{
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
    {
        HAL_UART_Transmit(&huart6, (uint8_t *)ptr, len, 0xffff);
        return len;
    }
    errno = EBADF;
    return -1;
}