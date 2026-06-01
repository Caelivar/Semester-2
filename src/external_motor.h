#ifndef EXTERNAL_MOTOR_H
#define EXTERNAL_MOTOR_H

#include "config.h"

#include <stdbool.h>
#include <stdint.h>

void external_motor_init(const external_motor_config_t *config);
void external_motor_stop(void);
void external_motor_set(bool forward, uint8_t pwm);

#endif
