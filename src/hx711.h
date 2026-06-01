#ifndef HX711_H
#define HX711_H

#include "config.h"

#include <stdbool.h>
#include <stdint.h>

void hx711_init(const hx711_config_t *config);
bool hx711_tare(void);
bool hx711_update(void);
int32_t hx711_get_grams(void);
int32_t hx711_get_instant_grams(void);
bool hx711_is_ready(void);
int32_t hx711_get_last_raw(void);
int32_t hx711_get_last_attempt_raw(void);
int32_t hx711_get_offset(void);
uint32_t hx711_get_update_count(void);
uint32_t hx711_get_miss_count(void);
uint32_t hx711_get_invalid_count(void);

#endif
