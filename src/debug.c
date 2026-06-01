#include "debug.h"

#include "uart.h"

void debug_init(uint32_t baud)
{
    uart0_init(baud);
}

void debug_write_str(const char *text)
{
    uart0_write_str(text);
}

void debug_write_i32(int32_t value)
{
    uart0_write_i32(value);
}

void debug_write_u32(uint32_t value)
{
    uart0_write_i32((int32_t)value);
}

void debug_write_newline(void)
{
    uart0_write('\r');
    uart0_write('\n');
}

void debug_log_text(const char *text)
{
    debug_write_str(text);
    debug_write_newline();
}

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
                     uint32_t invalid_count)
{
    debug_write_str("HX711 ms=");
    debug_write_u32(ms);
    debug_write_str(" ready=");
    debug_write_i32(ready ? 1 : 0);
    debug_write_str(" updated=");
    debug_write_i32(updated ? 1 : 0);
    debug_write_str(" attempt_raw=");
    debug_write_i32(attempt_raw);
    debug_write_str(" valid_raw=");
    debug_write_i32(valid_raw);
    debug_write_str(" offset=");
    debug_write_i32(offset);
    debug_write_str(" instant_g=");
    debug_write_i32(instant_grams);
    debug_write_str(" grams=");
    debug_write_i32(grams);
    debug_write_str(" ok=");
    debug_write_u32(ok_count);
    debug_write_str(" miss=");
    debug_write_u32(miss_count);
    debug_write_str(" invalid=");
    debug_write_u32(invalid_count);
    debug_write_newline();
}
