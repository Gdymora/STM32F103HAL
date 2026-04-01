#include "hal_uart.h"
#include "hal_gpio.h"

#define USART1_BASE 0x40013800
#define USART1_SR (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_DR (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART1_BRR (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART1_CR1 (*(volatile uint32_t *)(USART1_BASE + 0x0C))

#define SR_TXE (1 << 7)  // TX buffer порожній
#define CR1_UE (1 << 13) // UART enable
#define CR1_TE (1 << 3)  // TX enable
#define SR_RXNE (1 << 5) // RX buffer не порожній
#define CR1_RE (1 << 2)  // RX enable

// USART1 на APB2, тактування через RCC_APB2ENR біт 14
#define RCC_USART1EN (1 << 14)

void uart_init(uint32_t baud)
{
    // тактування USART1 і GPIOA
    RCC_APB2ENR |= RCC_USART1EN;

    // PA9 = TX = Alternate Function Push-Pull 50MHz = 0xB
    gpio_init(PIN_PA9, 0xB);

    // BRR = CPU_HZ / baud
    // для 8MHz і 9600 baud = 8000000/9600 = 833 = 0x341
    USART1_BRR = 8000000UL / baud;

    // вмикаємо UART і TX
    USART1_CR1 = CR1_UE | CR1_TE | CR1_RE;
}

void uart_putc(char c)
{
    // чекаємо поки TX buffer звільниться
    while (!(USART1_SR & SR_TXE))
        ;
    USART1_DR = c;
}

void uart_puts(const char *s)
{
    while (*s)
    {
        uart_putc(*s++);
    }
}

char uart_getc(void)
{
    while (!(USART1_SR & SR_RXNE))
        ;
    return (char)USART1_DR;
}

void uart_gets(char *buf, uint8_t len)
{
    uint8_t i = 0;
    char c;
    while (i < len - 1)
    {
        c = uart_getc();
        if (c == '\r' || c == '\n')
            break;
        buf[i++] = c;
    }
    buf[i] = '\0';
}

static void uart_print_int(int32_t n)
{
    if (n < 0)
    {
        uart_putc('-');
        n = -n;
    }
    if (n == 0)
    {
        uart_putc('0');
        return;
    }
    char buf[12];
    uint8_t i = 0;
    while (n > 0)
    {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    for (int8_t j = i - 1; j >= 0; j--)
    {
        uart_putc(buf[j]);
    }
}

void uart_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 'd':
            {
                int32_t n = va_arg(args, int32_t);
                uart_print_int(n);
                break;
            }
            case 's':
            {
                char *s = va_arg(args, char *);
                uart_puts(s);
                break;
            }
            case 'x':
            {
                uint32_t n = va_arg(args, uint32_t);
                uart_puts("0x");
                for (int8_t i = 28; i >= 0; i -= 4)
                {
                    uint8_t nibble = (n >> i) & 0xF;
                    uart_putc(nibble < 10 ? '0' + nibble : 'a' + nibble - 10);
                }
                break;
            }
            case 'c':
            {
                char c = (char)va_arg(args, int);
                uart_putc(c);
                break;
            }
            case '%':
            {
                uart_putc('%');
                break;
            }
            }
        }
        else
        {
            uart_putc(*fmt);
        }
        fmt++;
    }

    va_end(args);
}