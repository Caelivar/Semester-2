#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <stdint.h>

void uart0_init(uint32_t baud);
bool uart0_available(void);
uint8_t uart0_read(void);
void uart0_write(uint8_t value);
void uart0_write_str(const char *text);
void uart0_write_i32(int32_t value);

void uart2_init(uint32_t baud);
bool uart2_available(void);
uint8_t uart2_read(void);
void uart2_write(uint8_t value);
void uart2_write_str(const char *text);
void uart2_write_i32(int32_t value);

#endif
