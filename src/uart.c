#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "uart.h"

#include <avr/io.h>

static uint16_t baud_to_ubrr(uint32_t baud)
{
    return (uint16_t)((F_CPU / (8UL * baud)) - 1UL);
}

void uart0_init(uint32_t baud)
{
    uint16_t ubrr = baud_to_ubrr(baud);

    UCSR0A = (1 << U2X0);
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr & 0xFF);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

bool uart0_available(void)
{
    return (UCSR0A & (1 << RXC0)) != 0;
}

uint8_t uart0_read(void)
{
    return UDR0;
}

void uart0_write(uint8_t value)
{
    while ((UCSR0A & (1 << UDRE0)) == 0)
    {
    }

    UDR0 = value;
}

void uart0_write_str(const char *text)
{
    while (*text != '\0')
    {
        uart0_write((uint8_t)*text);
        text++;
    }
}

void uart0_write_i32(int32_t value)
{
    char buf[12];
    uint8_t i = sizeof(buf);
    bool negative = value < 0;
    uint32_t abs_value = negative ? (uint32_t)(-value) : (uint32_t)value;

    buf[--i] = '\0';

    do
    {
        buf[--i] = (char)('0' + (abs_value % 10U));
        abs_value /= 10U;
    } while (abs_value != 0U);

    if (negative)
    {
        buf[--i] = '-';
    }

    uart0_write_str(&buf[i]);
}

void uart2_init(uint32_t baud)
{
    uint16_t ubrr = baud_to_ubrr(baud);

    UCSR2A = (1 << U2X2);
    UBRR2H = (uint8_t)(ubrr >> 8);
    UBRR2L = (uint8_t)(ubrr & 0xFF);
    UCSR2B = (1 << RXEN2) | (1 << TXEN2);
    UCSR2C = (1 << UCSZ21) | (1 << UCSZ20);
}

bool uart2_available(void)
{
    return (UCSR2A & (1 << RXC2)) != 0;
}

uint8_t uart2_read(void)
{
    return UDR2;
}

void uart2_write(uint8_t value)
{
    while ((UCSR2A & (1 << UDRE2)) == 0)
    {
    }

    UDR2 = value;
}

void uart2_write_str(const char *text)
{
    while (*text != '\0')
    {
        uart2_write((uint8_t)*text);
        text++;
    }
}

void uart2_write_i32(int32_t value)
{
    char buf[12];
    uint8_t i = sizeof(buf);
    bool negative = value < 0;
    uint32_t abs_value = negative ? (uint32_t)(-value) : (uint32_t)value;

    buf[--i] = '\0';

    do
    {
        buf[--i] = (char)('0' + (abs_value % 10U));
        abs_value /= 10U;
    } while (abs_value != 0U);

    if (negative)
    {
        buf[--i] = '-';
    }

    uart2_write_str(&buf[i]);
}
