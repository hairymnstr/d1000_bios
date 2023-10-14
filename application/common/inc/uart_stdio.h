#include "stm32h7xx_ll_usart.h"

#ifndef _UART_STDIO_H
#define _UART_STDIO_H

#ifndef UART_STDIO_RX_BUF_SIZE
#define UART_STDIO_RX_BUF_SIZE 1024
#endif

#ifndef UART_STDIO_TX_BUF_SIZE
#define UART_STDIO_TX_BUF_SIZE 1024
#endif

void uart_stdio_init(USART_TypeDef *uart);
void uart_stdio_irq_handler(USART_TypeDef *uart);

#endif /* ifndef _UART_STDIO_H */
