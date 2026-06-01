#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "app.h"
#include "config.h"
#include "debug.h"
#include "timebase.h"

#include <avr/interrupt.h>

// ===================== Editable Project Configuration =====================

#define CFG_START_SERVO_AWAY_MS         1500UL
#define CFG_START_SERVO_TOWARDS_MS      3000UL
#define CFG_START_MOTOR_V_WITH_A_MS     4000UL
#define CFG_START_MOTOR_A_FINAL_RIGHT_MS 2000UL
#define CFG_START_FINAL_SERVO_AWAY_MS   1500UL
#define CFG_RAMP_INTERVAL_MS            100UL
#define CFG_LOAD_DISPLAY_INTERVAL_MS    100UL
#define CFG_HX711_READ_INTERVAL_MS      50UL
#define CFG_DEBUG_INTERVAL_MS           500UL
#define CFG_START_PRESSURE_THRESHOLD_G  700L

#define CFG_SERVO_STOP_US               1500U
#define CFG_SERVO1_TOWARDS_US           1000U
#define CFG_SERVO2_TOWARDS_US           2000U
#define CFG_SERVO1_AWAY_US              2000U
#define CFG_SERVO2_AWAY_US              1000U

#define CFG_EXTERNAL_MIN_PWM            45U
#define CFG_EXTERNAL_MAX_PWM            255U
#define CFG_EXTERNAL_STEP_PWM           7U

#define CFG_SHIELD_A_MIN_PWM            35U
#define CFG_SHIELD_A_MAX_PWM            255U
#define CFG_SHIELD_A_STEP_PWM           6U
#define CFG_SHIELD_A_ASSIST_MIN_PWM     25U
#define CFG_SHIELD_A_ASSIST_MAX_PWM     90U
#define CFG_SHIELD_A_ASSIST_STEP_PWM    2U
#define CFG_SHIELD_A_CALIBRATION_PWM    110U

#define CFG_SHIELD_B_MIN_PWM            70U
#define CFG_SHIELD_B_MAX_PWM            255U
#define CFG_SHIELD_B_STEP_PWM           8U

#define CFG_HX711_TARE_SAMPLES          32U
#define CFG_HX711_COUNTS_PER_GRAM       250L
#define CFG_HX711_FILTER_SHIFT          1U
#define CFG_HX711_READY_TIMEOUT_MS      1000U

#define CFG_NEXTION_BAUD                115200UL
#define CFG_DEBUG_BAUD                  115200UL
#define CFG_NEXTION_MAIN_PAGE_ID        0U
#define CFG_NEXTION_MANUAL_PAGE_ID      1U
#define CFG_NEXTION_CALIBRATION_ID      1U
#define CFG_NEXTION_START_ID            2U
#define CFG_NEXTION_STOP_ID             3U
#define CFG_NEXTION_TARE_ID             4U
#define CFG_NEXTION_MANUAL_ID           5U
#define CFG_NEXTION_MANUAL_A_LEFT_ID    1U
#define CFG_NEXTION_MANUAL_A_RIGHT_ID   2U
#define CFG_NEXTION_MANUAL_B_LEFT_ID    3U
#define CFG_NEXTION_MANUAL_B_RIGHT_ID   4U
#define CFG_NEXTION_MANUAL_V_LEFT_ID    5U
#define CFG_NEXTION_MANUAL_V_RIGHT_ID   6U
#define CFG_NEXTION_MANUAL_SERVO_OPEN_ID 7U
#define CFG_NEXTION_MANUAL_SERVO_CLOSE_ID 8U
#define CFG_NEXTION_BACK_ID             9U
#define CFG_NEXTION_STOP_ALL_ID         10U
#define CFG_NEXTION_LOAD_TEXT_NAME      "tLoad"
#define CFG_NEXTION_LOAD_PROGRESS_NAME  "jLoad"
#define CFG_NEXTION_STATUS_TEXT_NAME    "tState"
#define CFG_NEXTION_STEP_TEXT_NAME      "tStep"

#define CFG_STOP_BUTTON_DEBOUNCE_MS     20U

#define CFG_MANUAL_MOTOR_A_PWM          110U
#define CFG_MANUAL_EXTERNAL_MOTOR_PWM   110U
#define CFG_MANUAL_MOTOR_V_PWM          110U

// Pin summary:
// Nextion: TX2 D16 -> RX, RX2 D17 -> TX
// Servo 1: D44, Servo 2: D45
// External board: D10 -> ENA, D22 -> INA, D23 -> INB
// HX711: D24 -> DAT/DOUT, D25 -> CLK/SCK
// Physical stop button: D26 -> button -> GND

static const app_config_t APP_CONFIG = {
    .timing = {
        .servo_close_ms = 0,
        .external_reverse_ms = 0,
        .shield_left_ms = 0,
        .start_servo_away_ms = CFG_START_SERVO_AWAY_MS,
        .start_servo_towards_ms = CFG_START_SERVO_TOWARDS_MS,
        .start_motor_v_with_a_ms = CFG_START_MOTOR_V_WITH_A_MS,
        .start_motor_a_final_right_ms = CFG_START_MOTOR_A_FINAL_RIGHT_MS,
        .start_final_servo_away_ms = CFG_START_FINAL_SERVO_AWAY_MS,
        .ramp_interval_ms = CFG_RAMP_INTERVAL_MS,
        .load_display_interval_ms = CFG_LOAD_DISPLAY_INTERVAL_MS,
        .hx711_read_interval_ms = CFG_HX711_READ_INTERVAL_MS,
        .debug_interval_ms = CFG_DEBUG_INTERVAL_MS,
        .load_threshold_g = 0,
        .start_pressure_threshold_g = CFG_START_PRESSURE_THRESHOLD_G,
    },
    .nextion = {
        .main_page_id = CFG_NEXTION_MAIN_PAGE_ID,
        .manual_page_id = CFG_NEXTION_MANUAL_PAGE_ID,
        .calibration_button_id = CFG_NEXTION_CALIBRATION_ID,
        .start_button_id = CFG_NEXTION_START_ID,
        .stop_button_id = CFG_NEXTION_STOP_ID,
        .tare_button_id = CFG_NEXTION_TARE_ID,
        .manual_button_id = CFG_NEXTION_MANUAL_ID,
        .manual_a_left_button_id = CFG_NEXTION_MANUAL_A_LEFT_ID,
        .manual_a_right_button_id = CFG_NEXTION_MANUAL_A_RIGHT_ID,
        .manual_b_left_button_id = CFG_NEXTION_MANUAL_B_LEFT_ID,
        .manual_b_right_button_id = CFG_NEXTION_MANUAL_B_RIGHT_ID,
        .manual_v_left_button_id = CFG_NEXTION_MANUAL_V_LEFT_ID,
        .manual_v_right_button_id = CFG_NEXTION_MANUAL_V_RIGHT_ID,
        .manual_servo_open_button_id = CFG_NEXTION_MANUAL_SERVO_OPEN_ID,
        .manual_servo_close_button_id = CFG_NEXTION_MANUAL_SERVO_CLOSE_ID,
        .back_button_id = CFG_NEXTION_BACK_ID,
        .stop_all_button_id = CFG_NEXTION_STOP_ALL_ID,
        .load_text_name = CFG_NEXTION_LOAD_TEXT_NAME,
        .load_progress_name = CFG_NEXTION_LOAD_PROGRESS_NAME,
        .status_text_name = CFG_NEXTION_STATUS_TEXT_NAME,
        .step_text_name = CFG_NEXTION_STEP_TEXT_NAME,
        .baud = CFG_NEXTION_BAUD,
    },
    .hx711 = {
        .dout = GPIO_PIN(DDRA, PORTA, PINA, PA2), // D24
        .sck = GPIO_PIN(DDRA, PORTA, PINA, PA3),  // D25
        .tare_samples = CFG_HX711_TARE_SAMPLES,
        .counts_per_gram = CFG_HX711_COUNTS_PER_GRAM,
        .filter_shift = CFG_HX711_FILTER_SHIFT,
        .ready_timeout_ms = CFG_HX711_READY_TIMEOUT_MS,
    },
    .servo = {
        .stop_us = CFG_SERVO_STOP_US,
        .servo1_towards_us = CFG_SERVO1_TOWARDS_US,
        .servo2_towards_us = CFG_SERVO2_TOWARDS_US,
        .servo1_away_us = CFG_SERVO1_AWAY_US,
        .servo2_away_us = CFG_SERVO2_AWAY_US,
    },
    .shield = {
        .channel_a = {
            .direction = GPIO_PIN(DDRB, PORTB, PINB, PB6), // D12
            .brake = GPIO_PIN(DDRH, PORTH, PINH, PH6),     // D9
            .right_direction_high = true,
            .brake_active_high = true,
            .ramp = {
                .min_pwm = CFG_SHIELD_A_MIN_PWM,
                .max_pwm = CFG_SHIELD_A_MAX_PWM,
                .step_pwm = CFG_SHIELD_A_STEP_PWM,
            },
        },
        .channel_b = {
            .direction = GPIO_PIN(DDRB, PORTB, PINB, PB7), // D13
            .brake = GPIO_PIN(DDRH, PORTH, PINH, PH5),     // D8
            .right_direction_high = true,
            .brake_active_high = true,
            .ramp = {
                .min_pwm = CFG_SHIELD_B_MIN_PWM,
                .max_pwm = CFG_SHIELD_B_MAX_PWM,
                .step_pwm = CFG_SHIELD_B_STEP_PWM,
            },
        },
    },
    .external_motor = {
        .ena = GPIO_PIN(DDRB, PORTB, PINB, PB4),  // D10 / OC2A
        .in_a = GPIO_PIN(DDRA, PORTA, PINA, PA0), // D22
        .in_b = GPIO_PIN(DDRA, PORTA, PINA, PA1), // D23
        .forward_in_a_high = true,
        .ramp = {
            .min_pwm = CFG_EXTERNAL_MIN_PWM,
            .max_pwm = CFG_EXTERNAL_MAX_PWM,
            .step_pwm = CFG_EXTERNAL_STEP_PWM,
        },
    },
    .motor_a_assist_ramp = {
        .min_pwm = CFG_SHIELD_A_ASSIST_MIN_PWM,
        .max_pwm = CFG_SHIELD_A_ASSIST_MAX_PWM,
        .step_pwm = CFG_SHIELD_A_ASSIST_STEP_PWM,
    },
    .motor_a_calibration_pwm = CFG_SHIELD_A_CALIBRATION_PWM,
    .manual_motor_a_pwm = CFG_MANUAL_MOTOR_A_PWM,
    .manual_external_motor_pwm = CFG_MANUAL_EXTERNAL_MOTOR_PWM,
    .manual_motor_v_pwm = CFG_MANUAL_MOTOR_V_PWM,
    .stop_button = {
        .pin = GPIO_PIN(DDRA, PORTA, PINA, PA4), // D26
        .debounce_ms = CFG_STOP_BUTTON_DEBOUNCE_MS,
    },
};

int main(void)
{
    cli();
    timebase_init();
    sei();

    debug_init(CFG_DEBUG_BAUD);
    debug_log_text("Boot");

    app_init(&APP_CONFIG);

    while (1)
    {
        app_update();
    }
}
