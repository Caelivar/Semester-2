#include "ramp.h"

void ramp_reset(ramp_state_t *state, ramp_config_t config)
{
    state->config = config;
    state->value = config.min_pwm;
}

uint8_t ramp_step_up_hold(ramp_state_t *state)
{
    uint16_t next;

    if (state->value >= state->config.max_pwm)
    {
        state->value = state->config.max_pwm;
        return state->value;
    }

    next = (uint16_t)state->value + state->config.step_pwm;
    if (next >= state->config.max_pwm)
    {
        state->value = state->config.max_pwm;
    }
    else
    {
        state->value = (uint8_t)next;
    }

    return state->value;
}
