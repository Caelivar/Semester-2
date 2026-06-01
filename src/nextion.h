#ifndef NEXTION_H
#define NEXTION_H

#include "config.h"

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    NEXTION_CONTROL_NONE = 0,
    NEXTION_CONTROL_CALIBRATION,
    NEXTION_CONTROL_START,
    NEXTION_CONTROL_STOP,
    NEXTION_CONTROL_TARE,
    NEXTION_CONTROL_MANUAL,
    NEXTION_CONTROL_MANUAL_A_LEFT,
    NEXTION_CONTROL_MANUAL_A_RIGHT,
    NEXTION_CONTROL_MANUAL_B_LEFT,
    NEXTION_CONTROL_MANUAL_B_RIGHT,
    NEXTION_CONTROL_MANUAL_V_LEFT,
    NEXTION_CONTROL_MANUAL_V_RIGHT,
    NEXTION_CONTROL_MANUAL_SERVO_OPEN,
    NEXTION_CONTROL_MANUAL_SERVO_CLOSE,
    NEXTION_CONTROL_BACK,
    NEXTION_CONTROL_STOP_ALL
} nextion_control_t;

typedef struct
{
    nextion_control_t control;
    bool pressed;
} nextion_event_t;

typedef enum
{
    NEXTION_BUTTON_NONE = 0,
    NEXTION_BUTTON_CALIBRATION,
    NEXTION_BUTTON_START,
    NEXTION_BUTTON_STOP,
    NEXTION_BUTTON_TARE,
    NEXTION_BUTTON_MANUAL
} nextion_button_t;

void nextion_init(const nextion_config_t *config);
bool nextion_poll_event(nextion_event_t *event);
bool nextion_poll_button(nextion_button_t *button);
void nextion_show_page(uint8_t page_id);
void nextion_set_load(int32_t grams);
void nextion_set_load_progress(uint8_t percent);
void nextion_set_status(const char *status);
void nextion_set_step(const char *step);

#endif
