#ifndef SERVO_H
#define SERVO_H

#include "config.h"

void servo_init(const servo_config_t *config);
void servo_stop(void);
void servo_move_towards(void);
void servo_move_away(void);

#endif
