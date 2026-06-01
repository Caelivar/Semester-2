#include "button.h"

#include "gpio.h"
#include "timebase.h"

static const button_config_t *g_config;
static bool g_stable_pressed;
static bool g_last_raw_pressed;
static uint32_t g_changed_ms;

void button_init(const button_config_t *config)
{
    g_config = config;
    gpio_set_input(config->pin);
    gpio_enable_pullup(config->pin);

    g_last_raw_pressed = !gpio_read(config->pin);
    g_stable_pressed = g_last_raw_pressed;
    g_changed_ms = timebase_millis();
}

void button_update(void)
{
    bool raw_pressed = !gpio_read(g_config->pin);
    uint32_t now = timebase_millis();

    if (raw_pressed != g_last_raw_pressed)
    {
        g_last_raw_pressed = raw_pressed;
        g_changed_ms = now;
        return;
    }

    if ((uint32_t)(now - g_changed_ms) >= g_config->debounce_ms)
    {
        g_stable_pressed = raw_pressed;
    }
}

bool button_is_pressed(void)
{
    return g_stable_pressed;
}
