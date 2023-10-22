#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

/*
This file makes no assumptions about system status so the
low-level API must be used, not HAL.
*/
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_usart.h"

/*
Need to raise privilege in some cases (e.g. assert) to be
able to disable interrupts etc.
 */
BaseType_t xPortRaisePrivilege(void);

/**
 * @brief Sets up the console UART making no assumptions
 * 
 * This is called by fault handlers before printing anything
 * it makes no assumptions about the configuration state of
 * the default console UART and sets it up from scratch to
 * ensure the debug messages are sent out regardless of
 * where the system was in setup or operation.
 */
static void fault_std_out_init(void)
{
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);

    /* Configure Tx Pin as : Alternate function, High Speed, Push pull, Pull up */
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;

    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Configure Rx Pin as : Alternate function, High Speed, Push pull, Pull up */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;

    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // no interrupt configuration, this is all running in exception handler mode

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

    LL_USART_InitTypeDef USART_InitStruct = {0};

    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;

    LL_USART_Init(USART3, &USART_InitStruct);
    LL_USART_Enable(USART3);
}

/**
 * @brief Sends a sequence of bytes through the stdout UART
 * 
 * Crash-safe function which doesn't depend on heap allocation
 * or syscall status, just dumps the message to the local UART
 * console.
 * 
 * @param msg pointer to the message to be written
 * @param len length in bytes to send
 */
static void write_std_out(const char *msg, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        while (!LL_USART_IsActiveFlag_TXE(USART3))
        {
            ;
        }

        LL_USART_TransmitData8(USART3, *msg++);
    }
}

/**
 * @brief Sends a null terminated string through stdout UART
 * 
 * Crash-safe function to send a null terminated string
 * to the local UART console.
 * 
 * @param msg pointer to the null terminated string
 */
static void write_std_out_str(const char *msg)
{
    write_std_out(msg, strlen(msg));
}

const char *const fault_hex_lut = "0123456789ABCDEF";

/**
 * @brief prints a 32 bit pointer on stdout UART
 * 
 * Crash-safe function to print a 32 bit pointer as
 * a string on the stdout UART.  Value is represented
 * as "0x" followed by 8 HEX characters.
 * 
 * @param ptr pointer to print
 */
static void write_std_out_ptr(void *ptr)
{
    write_std_out("0x", 2);
    for (int shift = 28; shift >= 0; shift -= 4)
    {
        write_std_out(&fault_hex_lut[(((uint32_t)ptr) >> shift) & 0xf], 1);
    }
}

/**
 * @brief print an integer value as a decimal number
 * 
 * Crash-safe function to print an integer without heap
 * usage.
 * 
 * @param var number to be printed
 */
static void write_std_out_dec(int var)
{
    bool started = false;
    if(var < 0)
    {
        write_std_out("-", 1);
        var = var * -1;
    }
    for(int div=1000000000;div>0;div = div/10)
    {
        if(started || (var / div))
        {
            started = true;
            write_std_out(&fault_hex_lut[(var / div)], 1);
            var -= ((var / div) * div);
        }
    }
}

/**
 * @brief Exception vector handlers
 * 
 * These handlers deal with the messy low-level detail of gathering the needed status
 * information.  They pass this on to a C layer once they've done the low level work.
 * 
 * To use these with CubeMX generated code you need to go to System Core -> NVIC and
 * select the Code Generation tab and uncheck generate code for the 4 fault handlers.
 * 
 * To stop all these different cases just getting promoted to HardFaults you need
 * to add
 * 
 * SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
 * SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
 * SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;
 */

__attribute__((naked)) void HardFault_Handler(void)
{
    __asm(
        "MOV    R1, #0  \n"
        "MOV    R0, LR  \n"
        "TST    R0, #4  \n"
        "ITE    EQ    \n"
        "MRSEQ  R0, MSP \n"
        "MRSNE  R0, PSP \n"
        "B      HardFault_HandlerC  \n");
}

__attribute__((naked)) void BusFault_Handler(void)
{
    __asm(
        "MOV    R1, #1  \n"
        "MOV    R0, LR  \n"
        "TST    R0, #4  \n"
        "ITE    EQ      \n"
        "MRSEQ  R0, MSP \n"
        "MRSNE  R0, PSP \n"
        "B      HardFault_HandlerC \n"
    );
}

__attribute__((naked)) void UsageFault_Handler(void)
{
    __asm(
        "MOV    R1, #2  \n"
        "MOV    R0, LR  \n"
        "TST    R0, #4  \n"
        "ITE    EQ      \n"
        "MRSEQ  R0, MSP \n"
        "MRSNE  R0, PSP \n"
        "B      HardFault_HandlerC \n"
    );
}

__attribute__((naked)) void MemManage_Handler(void)
{
    __asm(
        "MOV    R1, #3  \n"
        "MOV    R0, LR  \n"
        "TST    R0, #4  \n"
        "ITE    EQ      \n"
        "MRSEQ  R0, MSP \n"
        "MRSNE  R0, PSP \n"
        "B      HardFault_HandlerC \n"
    );
}

void HardFault_HandlerC(void **hardfault_args, int exception_type)
{
    fault_std_out_init();

    if(exception_type == 0)
    {
        write_std_out_str("Hard fault handler\r\n");
    }
    else if(exception_type == 1)
    {
        write_std_out_str("Bus fault handler\r\n");
    }
    else if(exception_type == 2)
    {
        write_std_out_str("Usage fault handler\r\n");
    }
    else if(exception_type == 3)
    {
        write_std_out_str("Memory fault handler\r\n");
    }
    else
    {
        write_std_out_str("Unexpected fault type\r\n");
    }

    write_std_out_str("Exception sp = ");
    write_std_out_ptr(hardfault_args);
    write_std_out_str("\r\n");

    write_std_out_str("Stacked r0   = ");
    write_std_out_ptr(hardfault_args[0]);
    write_std_out_str("\r\n");

    write_std_out_str("Stacked r1   = ");
    write_std_out_ptr(hardfault_args[1]);
    write_std_out_str("\r\n");

    write_std_out_str("Stacked r2   = ");
    write_std_out_ptr(hardfault_args[2]);
    write_std_out_str("\r\n");

    write_std_out_str("Stacked r3   = ");
    write_std_out_ptr(hardfault_args[3]);
    write_std_out_str("\r\n");

    write_std_out_str("Stacked r12  = ");
    write_std_out_ptr(hardfault_args[4]);
    write_std_out_str("\r\n");

    write_std_out_str("Stacked lr   = ");
    write_std_out_ptr(hardfault_args[5]);
    write_std_out_str("\r\n");

    write_std_out_str("Stacked pc   = ");
    write_std_out_ptr(hardfault_args[6]);
    write_std_out_str("\r\n");

    write_std_out_str("Stacked psr  = ");
    write_std_out_ptr(hardfault_args[7]);
    write_std_out_str("\r\n");

    write_std_out_str("\r\n");

    write_std_out_str("MMAR         = ");
    write_std_out_ptr(*((void **)(0xE000ED34)));
    write_std_out_str("\r\n");

    write_std_out_str("BFAR         = ");
    write_std_out_ptr(*((void **)(0xE000ED38)));
    write_std_out_str("\r\n");

    write_std_out_str("CFSR         = ");
    write_std_out_ptr(*((void **)(0xE000ED28)));
    write_std_out_str("\r\n");

    write_std_out_str("HFSR         = ");
    write_std_out_ptr(*((void **)(0xE000ED2C)));
    write_std_out_str("\r\n");

    write_std_out_str("DFSR         = ");
    write_std_out_ptr(*((void **)(0xE000ED30)));
    write_std_out_str("\r\n");

    write_std_out_str("AFSR         = ");
    write_std_out_ptr(*((void **)(0xE000ED3C)));
    write_std_out_str("\r\n");

    write_std_out_str("\r\n");

    while (1)
    {
        ;
    }
}

void vApplicationStackOverflowHook(xTaskHandle xTask __attribute__((__unused__)), signed char *pcTaskName)
{
    /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */

    // no interrupts now...
    xPortRaisePrivilege();
    portDISABLE_INTERRUPTS();

    // make sure the UART is configured
    fault_std_out_init();

    write_std_out_str("Stack overflow detected in ");
    write_std_out_str((char *)pcTaskName);
    write_std_out_str("\r\n\r\n");

    while(1) {;}
}

/**
 * @brief Assert implmentation for stm32 HAL
 * 
 * @param file location of assertion
 * @param line 
 */
void fault_assert(const char *file, unsigned int line)
{
    xPortRaisePrivilege();
    portDISABLE_INTERRUPTS();

    fault_std_out_init();

    write_std_out_str("Assertion failure in ");
    write_std_out_str(file);
    write_std_out_str(":");
    write_std_out_dec(line);
    write_std_out_str("\r\n");

    write_std_out_str("Thread was ");
    write_std_out_str(pcTaskGetTaskName(NULL));
    write_std_out_str("\r\n");

    while(1) {;}
}

/**
 * @brief Libc assert callback
 * 
 * @param file file containing assert()
 * @param line line the assert() is on
 * @param func name of the function the assert() is in (may be NULL in some cases)
 * @param failed_expr the expression that was tested and failed
 */
void __assert_func(const char *file, int line, const char *func, const char *failed_expr)
{
    xPortRaisePrivilege();
    portDISABLE_INTERRUPTS();

    fault_std_out_init();

    write_std_out_str("Assertion ");
    write_std_out_str(failed_expr);
    write_std_out_str(" failed in ");
    write_std_out_str(file);
    write_std_out_str(":");
    write_std_out_dec(line);
    if(func)
    {
        write_std_out_str(":");
        write_std_out_str(func);
    }
    write_std_out_str("\r\n");

    while(1) {;}
}