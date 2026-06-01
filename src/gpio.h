#ifndef GPIO_H
#define GPIO_H

#include "config.h"

static inline void gpio_set_output(gpio_pin_t pin)
{
    *pin.ddr |= (uint8_t)(1U << pin.bit);
}

static inline void gpio_set_input(gpio_pin_t pin)
{
    *pin.ddr &= (uint8_t)~(1U << pin.bit);
}

static inline void gpio_write_high(gpio_pin_t pin)
{
    *pin.port |= (uint8_t)(1U << pin.bit);
}

static inline void gpio_write_low(gpio_pin_t pin)
{
    *pin.port &= (uint8_t)~(1U << pin.bit);
}

static inline void gpio_write(gpio_pin_t pin, bool high)
{
    if (high)
    {
        gpio_write_high(pin);
    }
    else
    {
        gpio_write_low(pin);
    }
}

static inline bool gpio_read(gpio_pin_t pin)
{
    return ((*pin.pin & (uint8_t)(1U << pin.bit)) != 0);
}

static inline void gpio_enable_pullup(gpio_pin_t pin)
{
    gpio_write_high(pin);
}

#endif
