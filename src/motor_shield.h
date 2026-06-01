#ifndef MOTOR_SHIELD_H
#define MOTOR_SHIELD_H

#include "config.h"

#include <stdbool.h>
#include <stdint.h>

void motor_shield_init(const motor_shield_config_t *config);
void motor_shield_stop_all(void);
void motor_shield_stop_a(void);
void motor_shield_stop_b(void);
void motor_shield_set_a(bool right, uint8_t pwm);
void motor_shield_set_b(bool right, uint8_t pwm);
void motor_shield_set(bool right, uint8_t pwm_a, uint8_t pwm_b);

#endif
