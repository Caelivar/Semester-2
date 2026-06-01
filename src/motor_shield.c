#include "motor_shield.h"

#include "gpio.h"

#include <avr/io.h>

static const motor_shield_config_t *g_config;

static void channel_set_direction(const shield_channel_config_t *channel, bool right)
{
    gpio_write(channel->direction, right ? channel->right_direction_high
                                         : !channel->right_direction_high);
}

static void channel_set_brake(const shield_channel_config_t *channel, bool active)
{
    gpio_write(channel->brake, active ? channel->brake_active_high
                                      : !channel->brake_active_high);
}

void motor_shield_init(const motor_shield_config_t *config)
{
    g_config = config;

    gpio_set_output(config->channel_a.direction);
    gpio_set_output(config->channel_a.brake);
    gpio_set_output(config->channel_b.direction);
    gpio_set_output(config->channel_b.brake);

    DDRE |= (1 << PE5); // D3 / OC3C
    DDRB |= (1 << PB5); // D11 / OC1A

    channel_set_direction(&config->channel_a, true);
    channel_set_direction(&config->channel_b, true);
    channel_set_brake(&config->channel_a, false);
    channel_set_brake(&config->channel_b, false);

    TCCR3A = (1 << COM3C1) | (1 << WGM30);
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    OCR3C = 0;

    TCCR1A = (1 << COM1A1) | (1 << WGM10);
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    OCR1A = 0;
}

void motor_shield_stop_all(void)
{
    motor_shield_stop_a();
    motor_shield_stop_b();
}

void motor_shield_stop_a(void)
{
    OCR3C = 0;

    if (g_config != 0)
    {
        channel_set_brake(&g_config->channel_a, false);
    }
}

void motor_shield_stop_b(void)
{
    OCR1A = 0;

    if (g_config != 0)
    {
        channel_set_brake(&g_config->channel_b, false);
    }
}

void motor_shield_set_a(bool right, uint8_t pwm)
{
    channel_set_brake(&g_config->channel_a, false);
    channel_set_direction(&g_config->channel_a, right);
    OCR3C = pwm;
}

void motor_shield_set_b(bool right, uint8_t pwm)
{
    channel_set_brake(&g_config->channel_b, false);
    channel_set_direction(&g_config->channel_b, right);
    OCR1A = pwm;
}

void motor_shield_set(bool right, uint8_t pwm_a, uint8_t pwm_b)
{
    motor_shield_set_a(right, pwm_a);
    motor_shield_set_b(right, pwm_b);
}
