#include "app.h"

#include "button.h"
#include "debug.h"
#include "external_motor.h"
#include "hx711.h"
#include "motor_shield.h"
#include "nextion.h"
#include "ramp.h"
#include "servo.h"
#include "timebase.h"

typedef enum
{
    APP_IDLE = 0,
    APP_CALIBRATION_A_RIGHT,
    APP_START_B_LEFT_TO_LOAD,
    APP_START_SERVO_AWAY,
    APP_START_A_LEFT_TO_BUTTON,
    APP_START_SERVO_TOWARDS,
    APP_START_V_RIGHT_A_RIGHT,
    APP_START_A_RIGHT_FINAL,
    APP_START_FINAL_SERVO_AWAY,
    APP_MANUAL_A_LEFT,
    APP_MANUAL_A_RIGHT,
    APP_MANUAL_B_LEFT,
    APP_MANUAL_B_RIGHT,
    APP_MANUAL_V_LEFT,
    APP_MANUAL_V_RIGHT,
    APP_MANUAL_SERVO_OPEN,
    APP_MANUAL_SERVO_CLOSE
} app_state_t;

static const app_config_t *g_config;
static app_state_t g_state;
static uint32_t g_state_started_ms;
static uint32_t g_last_ramp_ms;
static uint32_t g_last_load_display_ms;
static uint32_t g_last_hx711_read_ms;
static uint32_t g_last_debug_ms;
static bool g_hx711_updated_since_debug;
static bool g_hx711_tare_ok;
static ramp_state_t g_external_ramp;
static ramp_state_t g_shield_a_ramp;
static ramp_state_t g_shield_b_ramp;
static ramp_state_t g_motor_a_assist_ramp;
static bool g_manual_page_active;

static void reset_ramps(void)
{
    ramp_reset(&g_external_ramp, g_config->external_motor.ramp);
    ramp_reset(&g_shield_a_ramp, g_config->shield.channel_a.ramp);
    ramp_reset(&g_shield_b_ramp, g_config->shield.channel_b.ramp);
    ramp_reset(&g_motor_a_assist_ramp, g_config->motor_a_assist_ramp);
}

static void stop_outputs(void)
{
    external_motor_stop();
    motor_shield_stop_all();
    servo_stop();
}

static void set_status_step(const char *status, const char *step)
{
    nextion_set_status(status);
    nextion_set_step(step);
}

static void set_idle_message(const char *status, const char *step)
{
    stop_outputs();
    reset_ramps();
    g_state = APP_IDLE;
    set_status_step(status, step);
}

static void enter_state(app_state_t state)
{
    stop_outputs();
    reset_ramps();

    g_state = state;
    g_state_started_ms = timebase_millis();
    g_last_ramp_ms = g_state_started_ms;

    if (state == APP_CALIBRATION_A_RIGHT)
    {
        motor_shield_set_a(true, g_config->motor_a_calibration_pwm);
        set_status_step("CALIBRATION", "Hold button: motor A right");
    }
    else if (state == APP_START_B_LEFT_TO_LOAD)
    {
        external_motor_set(false, g_external_ramp.value);
        set_status_step("RUNNING", "Motor B left to 700g");
    }
    else if (state == APP_START_SERVO_AWAY)
    {
        servo_move_away();
        set_status_step("RUNNING", "Servos open 1.5s");
    }
    else if (state == APP_START_A_LEFT_TO_BUTTON)
    {
        motor_shield_set_a(false, g_shield_a_ramp.value);
        set_status_step("RUNNING", "Motor A left to stop button");
    }
    else if (state == APP_START_SERVO_TOWARDS)
    {
        servo_move_towards();
        set_status_step("RUNNING", "Servos close 3s");
    }
    else if (state == APP_START_V_RIGHT_A_RIGHT)
    {
        motor_shield_set_b(true, g_shield_b_ramp.value);
        motor_shield_set_a(true, g_motor_a_assist_ramp.value);
        set_status_step("RUNNING", "Motor V right + A assist");
    }
    else if (state == APP_START_A_RIGHT_FINAL)
    {
        motor_shield_set_a(true, g_shield_a_ramp.value);
        set_status_step("RUNNING", "Motor A right 2s");
    }
    else if (state == APP_START_FINAL_SERVO_AWAY)
    {
        servo_move_away();
        set_status_step("RUNNING", "Final servo open");
    }
    else if (state == APP_MANUAL_A_LEFT)
    {
        motor_shield_set_a(false, g_config->manual_motor_a_pwm);
        set_status_step("MANUAL", "Motor A left");
    }
    else if (state == APP_MANUAL_A_RIGHT)
    {
        motor_shield_set_a(true, g_config->manual_motor_a_pwm);
        set_status_step("MANUAL", "Motor A right");
    }
    else if (state == APP_MANUAL_B_LEFT)
    {
        external_motor_set(false, g_config->manual_external_motor_pwm);
        set_status_step("MANUAL", "Motor B left");
    }
    else if (state == APP_MANUAL_B_RIGHT)
    {
        external_motor_set(true, g_config->manual_external_motor_pwm);
        set_status_step("MANUAL", "Motor B right");
    }
    else if (state == APP_MANUAL_V_LEFT)
    {
        motor_shield_set_b(false, g_config->manual_motor_v_pwm);
        set_status_step("MANUAL", "Motor V left");
    }
    else if (state == APP_MANUAL_V_RIGHT)
    {
        motor_shield_set_b(true, g_config->manual_motor_v_pwm);
        set_status_step("MANUAL", "Motor V right");
    }
    else if (state == APP_MANUAL_SERVO_OPEN)
    {
        servo_move_away();
        set_status_step("MANUAL", "Servos open");
    }
    else if (state == APP_MANUAL_SERVO_CLOSE)
    {
        servo_move_towards();
        set_status_step("MANUAL", "Servos close");
    }
}

static void finish_to_idle(const char *status, const char *step)
{
    set_idle_message(status, step);
}

static void handle_stop_request(bool stay_on_manual_page)
{
    if (!stay_on_manual_page)
    {
        g_manual_page_active = false;
        nextion_show_page(g_config->nextion.main_page_id);
    }

    finish_to_idle(stay_on_manual_page ? "MANUAL" : "STOPPED",
                   "All motors stopped");
}

static void handle_tare_request(void)
{
    g_manual_page_active = false;
    nextion_show_page(g_config->nextion.main_page_id);
    set_status_step("TARING", "Keep load cell unloaded");
    stop_outputs();
    reset_ramps();
    g_state = APP_IDLE;

    g_hx711_tare_ok = hx711_tare();
    g_hx711_updated_since_debug = false;
    g_last_hx711_read_ms = timebase_millis();

    if (g_hx711_tare_ok)
    {
        nextion_set_load(0);
        nextion_set_load_progress(0);
        set_status_step("READY", "Tare OK");
        debug_log_text("HX711 tare OK");
    }
    else
    {
        set_status_step("ERROR", "HX711 tare failed");
        debug_log_text("HX711 tare FAILED");
    }
}

static bool manual_release_matches_state(nextion_control_t control)
{
    return (control == NEXTION_CONTROL_MANUAL_A_LEFT &&
            g_state == APP_MANUAL_A_LEFT) ||
           (control == NEXTION_CONTROL_MANUAL_A_RIGHT &&
            g_state == APP_MANUAL_A_RIGHT) ||
           (control == NEXTION_CONTROL_MANUAL_B_LEFT &&
            g_state == APP_MANUAL_B_LEFT) ||
           (control == NEXTION_CONTROL_MANUAL_B_RIGHT &&
            g_state == APP_MANUAL_B_RIGHT) ||
           (control == NEXTION_CONTROL_MANUAL_V_LEFT &&
            g_state == APP_MANUAL_V_LEFT) ||
           (control == NEXTION_CONTROL_MANUAL_V_RIGHT &&
            g_state == APP_MANUAL_V_RIGHT) ||
           (control == NEXTION_CONTROL_MANUAL_SERVO_OPEN &&
            g_state == APP_MANUAL_SERVO_OPEN) ||
           (control == NEXTION_CONTROL_MANUAL_SERVO_CLOSE &&
            g_state == APP_MANUAL_SERVO_CLOSE);
}

static void handle_manual_press(nextion_control_t control)
{
    if (control == NEXTION_CONTROL_MANUAL_A_LEFT)
    {
        enter_state(APP_MANUAL_A_LEFT);
    }
    else if (control == NEXTION_CONTROL_MANUAL_A_RIGHT)
    {
        enter_state(APP_MANUAL_A_RIGHT);
    }
    else if (control == NEXTION_CONTROL_MANUAL_B_LEFT)
    {
        enter_state(APP_MANUAL_B_LEFT);
    }
    else if (control == NEXTION_CONTROL_MANUAL_B_RIGHT)
    {
        enter_state(APP_MANUAL_B_RIGHT);
    }
    else if (control == NEXTION_CONTROL_MANUAL_V_LEFT)
    {
        enter_state(APP_MANUAL_V_LEFT);
    }
    else if (control == NEXTION_CONTROL_MANUAL_V_RIGHT)
    {
        enter_state(APP_MANUAL_V_RIGHT);
    }
    else if (control == NEXTION_CONTROL_MANUAL_SERVO_OPEN)
    {
        enter_state(APP_MANUAL_SERVO_OPEN);
    }
    else if (control == NEXTION_CONTROL_MANUAL_SERVO_CLOSE)
    {
        enter_state(APP_MANUAL_SERVO_CLOSE);
    }
}

static void handle_nextion_event(nextion_event_t event)
{
    if (event.control == NEXTION_CONTROL_CALIBRATION)
    {
        if (event.pressed)
        {
            enter_state(APP_CALIBRATION_A_RIGHT);
        }
        else if (g_state == APP_CALIBRATION_A_RIGHT)
        {
            finish_to_idle("READY", "Calibration stopped");
        }
    }
    else if (event.control == NEXTION_CONTROL_START && !event.pressed)
    {
        g_manual_page_active = false;
        nextion_show_page(g_config->nextion.main_page_id);
        enter_state(APP_START_B_LEFT_TO_LOAD);
    }
    else if (event.control == NEXTION_CONTROL_STOP)
    {
        handle_stop_request(false);
    }
    else if (event.control == NEXTION_CONTROL_TARE && !event.pressed)
    {
        handle_tare_request();
    }
    else if (event.control == NEXTION_CONTROL_MANUAL && !event.pressed)
    {
        stop_outputs();
        reset_ramps();
        g_state = APP_IDLE;
        g_manual_page_active = true;
        nextion_show_page(g_config->nextion.manual_page_id);
        set_status_step("MANUAL", "Hold a control to test");
    }
    else if (event.control == NEXTION_CONTROL_BACK && !event.pressed)
    {
        g_manual_page_active = false;
        nextion_show_page(g_config->nextion.main_page_id);
        finish_to_idle("READY", "Waiting for bottle");
    }
    else if (event.control == NEXTION_CONTROL_STOP_ALL)
    {
        handle_stop_request(g_manual_page_active);
    }
    else if (event.control >= NEXTION_CONTROL_MANUAL_A_LEFT &&
             event.control <= NEXTION_CONTROL_MANUAL_SERVO_CLOSE)
    {
        if (event.pressed)
        {
            handle_manual_press(event.control);
        }
        else if (manual_release_matches_state(event.control))
        {
            finish_to_idle("MANUAL", "Hold a control to test");
        }
    }
}

static void update_load_display(uint32_t now)
{
    bool updated = false;
    int32_t grams;
    int32_t threshold;
    uint8_t progress;

    if ((uint32_t)(now - g_last_hx711_read_ms) >=
        g_config->timing.hx711_read_interval_ms)
    {
        g_last_hx711_read_ms = now;
        updated = hx711_update();
    }

    if (updated)
    {
        g_hx711_updated_since_debug = true;
    }

    if ((uint32_t)(now - g_last_load_display_ms) >=
        g_config->timing.load_display_interval_ms)
    {
        g_last_load_display_ms = now;
        if (g_hx711_tare_ok)
        {
            grams = hx711_get_grams();
            threshold = g_config->timing.start_pressure_threshold_g;
            progress = 0;

            if (threshold > 0 && grams > 0)
            {
                progress = (uint8_t)((grams * 100L) / threshold);
                if (progress > 100U)
                {
                    progress = 100U;
                }
            }

            nextion_set_load(grams);
            nextion_set_load_progress(progress);
        }
    }

    if ((uint32_t)(now - g_last_debug_ms) >= g_config->timing.debug_interval_ms)
    {
        g_last_debug_ms = now;
        debug_log_hx711(now,
                        hx711_is_ready(),
                        g_hx711_updated_since_debug,
                        hx711_get_last_attempt_raw(),
                        hx711_get_last_raw(),
                        hx711_get_offset(),
                        hx711_get_instant_grams(),
                        hx711_get_grams(),
                        hx711_get_update_count(),
                        hx711_get_miss_count(),
                        hx711_get_invalid_count());
        g_hx711_updated_since_debug = false;
    }
}

static void update_ramped_outputs(uint32_t now)
{
    if ((uint32_t)(now - g_last_ramp_ms) < g_config->timing.ramp_interval_ms)
    {
        return;
    }

    g_last_ramp_ms = now;

    if (g_state == APP_START_B_LEFT_TO_LOAD)
    {
        external_motor_set(false, ramp_step_up_hold(&g_external_ramp));
    }
    else if (g_state == APP_START_A_LEFT_TO_BUTTON)
    {
        motor_shield_set_a(false, ramp_step_up_hold(&g_shield_a_ramp));
    }
    else if (g_state == APP_START_V_RIGHT_A_RIGHT)
    {
        motor_shield_set_b(true, ramp_step_up_hold(&g_shield_b_ramp));
        motor_shield_set_a(true, ramp_step_up_hold(&g_motor_a_assist_ramp));
    }
    else if (g_state == APP_START_A_RIGHT_FINAL)
    {
        motor_shield_set_a(true, ramp_step_up_hold(&g_shield_a_ramp));
    }
}

static void update_state_completion(uint32_t now)
{
    uint32_t elapsed = (uint32_t)(now - g_state_started_ms);

    if (g_state == APP_START_B_LEFT_TO_LOAD &&
        hx711_get_grams() >= g_config->timing.start_pressure_threshold_g)
    {
        enter_state(APP_START_SERVO_AWAY);
    }
    else if (g_state == APP_START_SERVO_AWAY &&
             elapsed >= g_config->timing.start_servo_away_ms)
    {
        enter_state(APP_START_A_LEFT_TO_BUTTON);
    }
    else if (g_state == APP_START_A_LEFT_TO_BUTTON && button_is_pressed())
    {
        enter_state(APP_START_SERVO_TOWARDS);
    }
    else if (g_state == APP_START_SERVO_TOWARDS &&
             elapsed >= g_config->timing.start_servo_towards_ms)
    {
        enter_state(APP_START_V_RIGHT_A_RIGHT);
    }
    else if (g_state == APP_START_V_RIGHT_A_RIGHT &&
             elapsed >= g_config->timing.start_motor_v_with_a_ms)
    {
        enter_state(APP_START_A_RIGHT_FINAL);
    }
    else if (g_state == APP_START_A_RIGHT_FINAL &&
             elapsed >= g_config->timing.start_motor_a_final_right_ms)
    {
        enter_state(APP_START_FINAL_SERVO_AWAY);
    }
    else if (g_state == APP_START_FINAL_SERVO_AWAY &&
             elapsed >= g_config->timing.start_final_servo_away_ms)
    {
        g_manual_page_active = false;
        finish_to_idle("DONE", "Opening cycle complete");
    }
}

void app_init(const app_config_t *config)
{
    g_config = config;
    g_state = APP_IDLE;

    nextion_init(&config->nextion);
    motor_shield_init(&config->shield);
    external_motor_init(&config->external_motor);
    servo_init(&config->servo);
    hx711_init(&config->hx711);
    button_init(&config->stop_button);

    stop_outputs();
    reset_ramps();
    g_manual_page_active = false;

    g_hx711_tare_ok = hx711_tare();

    if (g_hx711_tare_ok)
    {
        debug_log_text("HX711 tare OK");
    }
    else
    {
        debug_log_text("HX711 tare FAILED");
    }

    g_last_load_display_ms = timebase_millis();
    g_last_hx711_read_ms = g_last_load_display_ms;
    g_last_debug_ms = g_last_load_display_ms;
    g_hx711_updated_since_debug = false;

    nextion_show_page(config->nextion.main_page_id);
    nextion_set_load(0);
    nextion_set_load_progress(0);

    if (g_hx711_tare_ok)
    {
        set_status_step("READY", "Waiting for bottle");
    }
    else
    {
        set_status_step("ERROR", "HX711 tare failed");
    }
}

void app_update(void)
{
    uint32_t now = timebase_millis();
    nextion_event_t event;

    button_update();

    while (nextion_poll_event(&event))
    {
        handle_nextion_event(event);
    }

    update_load_display(now);
    update_ramped_outputs(now);
    update_state_completion(now);
}
