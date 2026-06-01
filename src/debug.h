#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>
#include <stdint.h>

void debug_init(uint32_t baud);
void debug_write_str(const char *text);
void debug_write_i32(int32_t value);
void debug_write_u32(uint32_t value);
void debug_write_newline(void);
void debug_log_hx711(uint32_t ms,
                     bool ready,
                     bool updated,
                     int32_t attempt_raw,
                     int32_t valid_raw,
                     int32_t offset,
                     int32_t instant_grams,
                     int32_t grams,
                     uint32_t ok_count,
                     uint32_t miss_count,
                     uint32_t invalid_count);
void debug_log_text(const char *text);

#endif
