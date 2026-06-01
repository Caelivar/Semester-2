#include "external_motor.h"

#include "gpio.h"

#include <avr/io.h>

static const external_motor_config_t *g_config;

void external_motor_init(const external_motor_config_t *config)
{
    g_config = config;

    gpio_set_output(config->ena);
    gpio_set_output(config->in_a);
    gpio_set_output(config->in_b);

    gpio_write_low(config->in_a);
    gpio_write_low(config->in_b);

    TCCR2A = (1 << COM2A1) | (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << CS22);
    OCR2A = 0;
}

void external_motor_stop(void)
{
    OCR2A = 0;
    gpio_write_low(g_config->in_a);
    gpio_write_low(g_config->in_b);
}

void external_motor_set(bool forward, uint8_t pwm)
{
    bool in_a_high = forward ? g_config->forward_in_a_high
                             : !g_config->forward_in_a_high;

    gpio_write(g_config->in_a, in_a_high);
    gpio_write(g_config->in_b, !in_a_high);
    OCR2A = pwm;
}
