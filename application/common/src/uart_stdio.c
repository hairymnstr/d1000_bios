#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "uart_stdio.h"

QueueHandle_t uart_stdio_rx_q = NULL;
QueueHandle_t uart_stdio_tx_q = NULL;
USART_TypeDef *uart_stdio_uart = NULL;

int _read(int fd, void *buf, size_t count)
{
    int nread = -1;
    if (fd == STDIN_FILENO)
    {
        char *char_buf = buf;
        for (size_t n = 0;n<count;n++)
        {
            xQueueReceive(uart_stdio_rx_q, &char_buf[n], portMAX_DELAY);
        }
        nread = count;
    }
    else
    {
        errno = EINVAL;
    }
    return nread;
}

int _write(int fd, const void *buf, size_t count)
{
    int nwritten = -1;
    if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO))
    {
        const char *char_buf = (const char *)buf;
        char last_char = '\0';
        for (size_t n = 0; n<count;n++)
        {
            LL_USART_EnableIT_TXE(uart_stdio_uart);
            if ((char_buf[n] == '\n') && (last_char != '\r'))
            {
                xQueueSendToBack(uart_stdio_tx_q, "\r", portMAX_DELAY);
            }
            xQueueSendToBack(uart_stdio_tx_q, &char_buf[n], portMAX_DELAY);
            last_char = char_buf[n];
        }
        nwritten = count;
    }
    else
    {
        errno = EINVAL;
    }
    return nwritten;
}

void uart_stdio_init(USART_TypeDef *uart)
{
    uart_stdio_rx_q = xQueueCreate(UART_STDIO_RX_BUF_SIZE, 1);
    uart_stdio_tx_q = xQueueCreate(UART_STDIO_TX_BUF_SIZE, 1);

    LL_USART_EnableIT_RXNE(uart);
    uart_stdio_uart = uart;
}

void uart_stdio_irq_handler(USART_TypeDef *uart)
{
    BaseType_t woken = pdFALSE;
    if (LL_USART_IsActiveFlag_RXNE(uart))
    {
        char val = uart->RDR;
        xQueueSendToBackFromISR(uart_stdio_rx_q, &val, &woken);
    }
    if (LL_USART_IsActiveFlag_TXE(uart))
    {
        char val;
        if (xQueueReceiveFromISR(uart_stdio_tx_q, &val, &woken) == pdTRUE)
        {
            uart->TDR = val;
        }
        else
        {
            LL_USART_DisableIT_TXE(uart);
        }
    }
    portEND_SWITCHING_ISR(woken);
}
