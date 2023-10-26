#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "semphr.h"

#include "main.h"
#include "expansion_bus.h"

struct expansion_ctx {
    SemaphoreHandle_t lock;
    SPI_HandleTypeDef *hspi;
    I2C_HandleTypeDef *hi2c;
    GPIO_TypeDef *cs0_port;
    uint16_t cs0_pin;
    GPIO_TypeDef *cs1_port;
    uint16_t cs1_pin;
    GPIO_TypeDef *rst_port;
    uint16_t rst_pin;
    uint8_t areg;
};

uint8_t expansion_register_xfer(expansion_ctx_t *ctx, bool read, uint8_t addr, uint8_t val)
{
    uint8_t txframe[3], rxframe[3];
    HAL_GPIO_WritePin(ctx->cs0_port, ctx->cs0_pin, GPIO_PIN_RESET);

    txframe[0] = 0x40;
    if (read)
    {
        txframe[0] |= 1;
    }
    txframe[1] = addr;
    txframe[2] = val;
    HAL_SPI_TransmitReceive(ctx->hspi, txframe, rxframe, 3, 0xffff);

    HAL_GPIO_WritePin(ctx->cs0_port, ctx->cs0_pin, GPIO_PIN_SET);

    return rxframe[2];
}

expansion_ctx_t *expansion_bus_init(expansion_config_t *conf)
{
    expansion_ctx_t *ctx = (expansion_ctx_t *)malloc(sizeof(expansion_ctx_t));
    if (ctx)
    {
        ctx->hspi = conf->hspi;
        ctx->hi2c = conf->hi2c;
        ctx->cs0_port = conf->cs0_port;
        ctx->cs0_pin = conf->cs0_pin;
        ctx->cs1_port = conf->cs1_port;
        ctx->cs1_pin = conf->cs1_pin;
        ctx->rst_port = conf->rst_port;
        ctx->rst_pin = conf->rst_pin;

        ctx->areg = 0;

        ctx->lock = xSemaphoreCreateMutex();

        HAL_GPIO_WritePin(ctx->rst_port, ctx->rst_pin, GPIO_PIN_SET);
        vTaskDelay(1);

        xSemaphoreTake(ctx->lock, portMAX_DELAY);
        {
            expansion_register_xfer(ctx, false, 0, 0);  // IODIRA = 0x00 => all outputs
        }
        xSemaphoreGive(ctx->lock);
    }
    return ctx;
}

void expansion_bus_set_led(expansion_ctx_t *ctx, uint8_t leds)
{
    xSemaphoreTake(ctx->lock, portMAX_DELAY);
    ctx->areg = (ctx->areg & 0x7) | (leds << 3);
    expansion_register_xfer(ctx, false, 0x14, ctx->areg);
    xSemaphoreGive(ctx->lock);
}
