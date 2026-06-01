#ifndef RAMP_H
#define RAMP_H

#include "config.h"

#include <stdint.h>

typedef struct
{
    ramp_config_t config;
    uint8_t value;
} ramp_state_t;

void ramp_reset(ramp_state_t *state, ramp_config_t config);
uint8_t ramp_step_up_hold(ramp_state_t *state);

#endif
