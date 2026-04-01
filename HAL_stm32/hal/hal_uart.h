#ifndef HAL_UART_H
#define HAL_UART_H

#include <stdint.h>
#include <stdarg.h>

void uart_init(uint32_t baud);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_printf(const char *fmt, ...);

char uart_getc(void);
void uart_gets(char *buf, uint8_t len);

#endif