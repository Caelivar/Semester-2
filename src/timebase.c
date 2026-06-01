#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "timebase.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

static volatile uint32_t g_ms = 0;

ISR(TIMER0_COMPA_vect)
{
    g_ms++;
}

void timebase_init(void)
{
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);
    OCR0A = 249;
    TIMSK0 = (1 << OCIE0A);
}

uint32_t timebase_millis(void)
{
    uint32_t value;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        value = g_ms;
    }

    return value;
}
