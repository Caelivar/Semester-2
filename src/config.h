#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

typedef struct
{
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *pin;
    uint8_t bit;
} gpio_pin_t;

#define GPIO_PIN(ddr_reg, port_reg, pin_reg, bit_name) \
    { &(ddr_reg), &(port_reg), &(pin_reg), (bit_name) }

typedef struct
{
    uint8_t min_pwm;
    uint8_t max_pwm;
    uint8_t step_pwm;
} ramp_config_t;

typedef struct
{
    gpio_pin_t direction;
    gpio_pin_t brake;
    bool right_direction_high;
    bool brake_active_high;
    ramp_config_t ramp;
} shield_channel_config_t;

typedef struct
{
    shield_channel_config_t channel_a;
    shield_channel_config_t channel_b;
} motor_shield_config_t;

typedef struct
{
    gpio_pin_t ena;
    gpio_pin_t in_a;
    gpio_pin_t in_b;
    bool forward_in_a_high;
    ramp_config_t ramp;
} external_motor_config_t;

typedef struct
{
    uint16_t stop_us;
    uint16_t servo1_towards_us;
    uint16_t servo2_towards_us;
    uint16_t servo1_away_us;
    uint16_t servo2_away_us;
} servo_config_t;

typedef struct
{
    gpio_pin_t dout;
    gpio_pin_t sck;
    uint8_t tare_samples;
    int32_t counts_per_gram;
    uint8_t filter_shift;
    uint16_t ready_timeout_ms;
} hx711_config_t;

typedef struct
{
    uint8_t main_page_id;
    uint8_t manual_page_id;
    uint8_t calibration_button_id;
    uint8_t start_button_id;
    uint8_t stop_button_id;
    uint8_t tare_button_id;
    uint8_t manual_button_id;
    uint8_t manual_a_left_button_id;
    uint8_t manual_a_right_button_id;
    uint8_t manual_b_left_button_id;
    uint8_t manual_b_right_button_id;
    uint8_t manual_v_left_button_id;
    uint8_t manual_v_right_button_id;
    uint8_t manual_servo_open_button_id;
    uint8_t manual_servo_close_button_id;
    uint8_t back_button_id;
    uint8_t stop_all_button_id;
    const char *load_text_name;
    const char *load_progress_name;
    const char *status_text_name;
    const char *step_text_name;
    uint32_t baud;
} nextion_config_t;

typedef struct
{
    gpio_pin_t pin;
    uint16_t debounce_ms;
} button_config_t;

typedef struct
{
    uint32_t servo_close_ms;
    uint32_t external_reverse_ms;
    uint32_t shield_left_ms;
    uint32_t start_servo_away_ms;
    uint32_t start_servo_towards_ms;
    uint32_t start_motor_v_with_a_ms;
    uint32_t start_motor_a_final_right_ms;
    uint32_t start_final_servo_away_ms;
    uint32_t ramp_interval_ms;
    uint32_t load_display_interval_ms;
    uint32_t hx711_read_interval_ms;
    uint32_t debug_interval_ms;
    int32_t load_threshold_g;
    int32_t start_pressure_threshold_g;
} app_timing_config_t;

typedef struct
{
    app_timing_config_t timing;
    nextion_config_t nextion;
    hx711_config_t hx711;
    servo_config_t servo;
    motor_shield_config_t shield;
    external_motor_config_t external_motor;
    ramp_config_t motor_a_assist_ramp;
    uint8_t motor_a_calibration_pwm;
    uint8_t manual_motor_a_pwm;
    uint8_t manual_external_motor_pwm;
    uint8_t manual_motor_v_pwm;
    button_config_t stop_button;
} app_config_t;

#endif
