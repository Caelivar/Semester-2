#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "hx711.h"

#include "gpio.h"
#include "timebase.h"

#include <util/delay.h>

static const hx711_config_t *g_config;
static int32_t g_offset;
static int32_t g_grams;
static int32_t g_instant_grams;
static int32_t g_last_raw;
static int32_t g_last_attempt_raw;
static int32_t g_filtered_counts;
static uint32_t g_update_count;
static uint32_t g_miss_count;
static uint32_t g_invalid_count;
static bool g_filter_ready;

static bool hx711_ready(void)
{
    return !gpio_read(g_config->dout);
}

static bool hx711_wait_ready(uint16_t timeout_ms)
{
    uint32_t start = timebase_millis();

    while (!hx711_ready())
    {
        if ((uint32_t)(timebase_millis() - start) >= timeout_ms)
        {
            return false;
        }
    }

    return true;
}

static bool hx711_read_raw(int32_t *raw)
{
    uint32_t value = 0;

    if (!hx711_ready())
    {
        return false;
    }

    for (uint8_t i = 0; i < 24; i++)
    {
        gpio_write_high(g_config->sck);
        _delay_us(1);
        value = (value << 1) | (gpio_read(g_config->dout) ? 1UL : 0UL);
        gpio_write_low(g_config->sck);
        _delay_us(1);
    }

    gpio_write_high(g_config->sck);
    _delay_us(1);
    gpio_write_low(g_config->sck);
    _delay_us(1);

    if ((value & 0x800000UL) != 0)
    {
        value |= 0xFF000000UL;
    }

    *raw = (int32_t)value;
    return true;
}

void hx711_init(const hx711_config_t *config)
{
    g_config = config;
    g_offset = 0;
    g_grams = 0;
    g_instant_grams = 0;
    g_last_raw = 0;
    g_last_attempt_raw = 0;
    g_filtered_counts = 0;
    g_update_count = 0;
    g_miss_count = 0;
    g_invalid_count = 0;
    g_filter_ready = false;

    gpio_set_input(config->dout);
    gpio_set_output(config->sck);
    gpio_write_low(config->sck);
}

bool hx711_tare(void)
{
    int64_t sum = 0;
    int32_t raw = 0;
    uint8_t accepted = 0;

    while (accepted < g_config->tare_samples)
    {
        if (!hx711_wait_ready(g_config->ready_timeout_ms) || !hx711_read_raw(&raw))
        {
            g_miss_count++;
            return false;
        }

        g_last_attempt_raw = raw;

        if (raw == -1)
        {
            g_invalid_count++;
            return false;
        }

        sum += raw;
        accepted++;
    }

    g_offset = (int32_t)(sum / g_config->tare_samples);
    g_last_raw = raw;
    g_grams = 0;
    g_instant_grams = 0;
    g_filtered_counts = 0;
    g_filter_ready = false;
    return true;
}

bool hx711_update(void)
{
    int32_t raw = 0;
    int32_t scale = g_config->counts_per_gram;
    int32_t counts;
    uint8_t filter_shift;
    int32_t filter_div;
    int32_t correction;

    if (!hx711_read_raw(&raw))
    {
        g_miss_count++;
        return false;
    }

    g_last_attempt_raw = raw;

    if (raw == -1)
    {
        g_invalid_count++;
        return false;
    }

    g_last_raw = raw;

    if (scale == 0)
    {
        scale = 1;
    }

    counts = raw - g_offset;
    g_instant_grams = counts / scale;

    if (!g_filter_ready)
    {
        g_filtered_counts = counts;
        g_filter_ready = true;
    }
    else
    {
        filter_shift = g_config->filter_shift;
        if (filter_shift > 6)
        {
            filter_shift = 6;
        }

        filter_div = (int32_t)(1UL << filter_shift);
        correction = (counts - g_filtered_counts) / filter_div;

        if (correction == 0 && counts != g_filtered_counts)
        {
            correction = (counts > g_filtered_counts) ? 1 : -1;
        }

        g_filtered_counts += correction;
    }

    g_grams = g_filtered_counts / scale;
    g_update_count++;
    return true;
}

int32_t hx711_get_grams(void)
{
    return g_grams;
}

int32_t hx711_get_instant_grams(void)
{
    return g_instant_grams;
}

bool hx711_is_ready(void)
{
    return hx711_ready();
}

int32_t hx711_get_last_raw(void)
{
    return g_last_raw;
}

int32_t hx711_get_last_attempt_raw(void)
{
    return g_last_attempt_raw;
}

int32_t hx711_get_offset(void)
{
    return g_offset;
}

uint32_t hx711_get_update_count(void)
{
    return g_update_count;
}

uint32_t hx711_get_miss_count(void)
{
    return g_miss_count;
}

uint32_t hx711_get_invalid_count(void)
{
    return g_invalid_count;
}
