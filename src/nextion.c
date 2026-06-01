#include "nextion.h"

#include "uart.h"

#include <stddef.h>

#define NEXTION_TOUCH_EVENT 0x65U
#define NEXTION_RELEASE_EVENT 0x00U
#define NEXTION_PRESS_EVENT 0x01U
#define NEXTION_END_BYTE 0xFFU

static const nextion_config_t *g_config;
static uint8_t g_frame[7];
static uint8_t g_frame_len;

static void nextion_send_end(void)
{
    uart2_write(NEXTION_END_BYTE);
    uart2_write(NEXTION_END_BYTE);
    uart2_write(NEXTION_END_BYTE);
}

static void nextion_cmd_begin(const char *component)
{
    uart2_write_str(component);
}

static nextion_control_t map_component(uint8_t page, uint8_t component)
{
    if (g_config == NULL)
    {
        return NEXTION_CONTROL_NONE;
    }

    if (page == g_config->main_page_id)
    {
        if (component == g_config->calibration_button_id)
        {
            return NEXTION_CONTROL_CALIBRATION;
        }
        if (component == g_config->start_button_id)
        {
            return NEXTION_CONTROL_START;
        }
        if (component == g_config->stop_button_id)
        {
            return NEXTION_CONTROL_STOP;
        }
        if (component == g_config->tare_button_id)
        {
            return NEXTION_CONTROL_TARE;
        }
        if (component == g_config->manual_button_id)
        {
            return NEXTION_CONTROL_MANUAL;
        }
    }

    if (page == g_config->manual_page_id)
    {
        if (component == g_config->manual_a_left_button_id)
        {
            return NEXTION_CONTROL_MANUAL_A_LEFT;
        }
        if (component == g_config->manual_a_right_button_id)
        {
            return NEXTION_CONTROL_MANUAL_A_RIGHT;
        }
        if (component == g_config->manual_b_left_button_id)
        {
            return NEXTION_CONTROL_MANUAL_B_LEFT;
        }
        if (component == g_config->manual_b_right_button_id)
        {
            return NEXTION_CONTROL_MANUAL_B_RIGHT;
        }
        if (component == g_config->manual_v_left_button_id)
        {
            return NEXTION_CONTROL_MANUAL_V_LEFT;
        }
        if (component == g_config->manual_v_right_button_id)
        {
            return NEXTION_CONTROL_MANUAL_V_RIGHT;
        }
        if (component == g_config->manual_servo_open_button_id)
        {
            return NEXTION_CONTROL_MANUAL_SERVO_OPEN;
        }
        if (component == g_config->manual_servo_close_button_id)
        {
            return NEXTION_CONTROL_MANUAL_SERVO_CLOSE;
        }
        if (component == g_config->back_button_id)
        {
            return NEXTION_CONTROL_BACK;
        }
        if (component == g_config->stop_all_button_id)
        {
            return NEXTION_CONTROL_STOP_ALL;
        }
    }

    return NEXTION_CONTROL_NONE;
}

static bool frame_has_end(void)
{
    return g_frame[4] == NEXTION_END_BYTE &&
           g_frame[5] == NEXTION_END_BYTE &&
           g_frame[6] == NEXTION_END_BYTE;
}

void nextion_init(const nextion_config_t *config)
{
    g_config = config;
    g_frame_len = 0;
    uart2_init(config->baud);
}

bool nextion_poll_event(nextion_event_t *event)
{
    while (uart2_available())
    {
        uint8_t value = uart2_read();

        if (g_frame_len == 0 && value != NEXTION_TOUCH_EVENT)
        {
            continue;
        }

        g_frame[g_frame_len++] = value;

        if (g_frame_len < sizeof(g_frame))
        {
            continue;
        }

        g_frame_len = 0;

        if (!frame_has_end() ||
            (g_frame[3] != NEXTION_PRESS_EVENT && g_frame[3] != NEXTION_RELEASE_EVENT))
        {
            continue;
        }

        event->control = map_component(g_frame[1], g_frame[2]);
        event->pressed = (g_frame[3] == NEXTION_PRESS_EVENT);
        return event->control != NEXTION_CONTROL_NONE;
    }

    return false;
}

bool nextion_poll_button(nextion_button_t *button)
{
    nextion_event_t event;

    while (nextion_poll_event(&event))
    {
        if (event.pressed)
        {
            continue;
        }

        if (event.control == NEXTION_CONTROL_CALIBRATION)
        {
            *button = NEXTION_BUTTON_CALIBRATION;
            return true;
        }
        if (event.control == NEXTION_CONTROL_START)
        {
            *button = NEXTION_BUTTON_START;
            return true;
        }
        if (event.control == NEXTION_CONTROL_STOP)
        {
            *button = NEXTION_BUTTON_STOP;
            return true;
        }
        if (event.control == NEXTION_CONTROL_TARE)
        {
            *button = NEXTION_BUTTON_TARE;
            return true;
        }
        if (event.control == NEXTION_CONTROL_MANUAL)
        {
            *button = NEXTION_BUTTON_MANUAL;
            return true;
        }
    }

    return false;
}

void nextion_show_page(uint8_t page_id)
{
    uart2_write_str("page ");
    uart2_write_i32(page_id);
    nextion_send_end();
}

void nextion_set_load(int32_t grams)
{
    int32_t abs_grams = grams < 0 ? -grams : grams;
    int32_t kg_whole = abs_grams / 1000;
    int32_t kg_tenth = (abs_grams % 1000) / 100;

    nextion_cmd_begin(g_config->load_text_name);
    uart2_write_str(".txt=\"");
    if (grams < 0)
    {
        uart2_write('-');
    }
    uart2_write_i32(kg_whole);
    uart2_write('.');
    uart2_write_i32(kg_tenth);
    uart2_write_str(" kg\"");
    nextion_send_end();
}

void nextion_set_load_progress(uint8_t percent)
{
    if (percent > 100U)
    {
        percent = 100U;
    }

    nextion_cmd_begin(g_config->load_progress_name);
    uart2_write_str(".val=");
    uart2_write_i32(percent);
    nextion_send_end();
}

void nextion_set_status(const char *status)
{
    nextion_cmd_begin(g_config->status_text_name);
    uart2_write_str(".txt=\"");
    uart2_write_str(status);
    uart2_write('"');
    nextion_send_end();
}

void nextion_set_step(const char *step)
{
    nextion_cmd_begin(g_config->step_text_name);
    uart2_write_str(".txt=\"");
    uart2_write_str(step);
    uart2_write('"');
    nextion_send_end();
}
