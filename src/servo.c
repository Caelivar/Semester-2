#include "servo.h"

#include <avr/io.h>

#define SERVO_TICKS_PER_US 2U

static const servo_config_t *g_config;

static uint16_t servo_us_to_ticks(uint16_t us)
{
    return (uint16_t)(us * SERVO_TICKS_PER_US);
}

static void servo_write(uint16_t servo1_us, uint16_t servo2_us)
{
    OCR5C = servo_us_to_ticks(servo1_us); // D44 / OC5C
    OCR5B = servo_us_to_ticks(servo2_us); // D45 / OC5B
}

void servo_init(const servo_config_t *config)
{
    g_config = config;

    DDRL |= (1 << PL5) | (1 << PL4);

    TCCR5A = (1 << COM5B1) | (1 << COM5C1) | (1 << WGM51);
    TCCR5B = (1 << WGM53) | (1 << WGM52) | (1 << CS51);
    ICR5 = 39999;

    servo_stop();
}

void servo_stop(void)
{
    servo_write(g_config->stop_us, g_config->stop_us);
}

void servo_move_towards(void)
{
    servo_write(g_config->servo1_towards_us, g_config->servo2_towards_us);
}

void servo_move_away(void)
{
    servo_write(g_config->servo1_away_us, g_config->servo2_away_us);
}
