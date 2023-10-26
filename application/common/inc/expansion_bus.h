#ifndef _EXPANSION_BUS_H
#define _EXPANSION_BUS_H 1

typedef struct expansion_ctx expansion_ctx_t;

typedef struct {
    SPI_HandleTypeDef *hspi;
    I2C_HandleTypeDef *hi2c;
    GPIO_TypeDef *cs0_port;
    uint16_t cs0_pin;
    GPIO_TypeDef *cs1_port;
    uint16_t cs1_pin;
    GPIO_TypeDef *rst_port;
    uint16_t rst_pin;
} expansion_config_t;


expansion_ctx_t *expansion_bus_init(expansion_config_t *conf);
void expansion_bus_set_led(expansion_ctx_t *ctx, uint8_t leds);

#endif /* ifndef _EXPANSION_BUS_H */