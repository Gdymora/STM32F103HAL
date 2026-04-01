// main.c — UART RX через переривання
// Приймаємо команди, LED реагує без polling

#include <stdint.h>

// RCC
#define RCC_BASE     0x40021000
#define RCC_APB2ENR  (*(volatile uint32_t *)(RCC_BASE + 0x18))

// GPIOA
#define GPIOA_BASE   0x40010800
#define GPIOA_CRL    (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR    (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

// GPIOC
#define GPIOC_BASE   0x40011000
#define GPIOC_CRH    (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_BSRR   (*(volatile uint32_t *)(GPIOC_BASE + 0x10))

// USART1
#define USART1_BASE  0x40013800
#define USART1_SR    (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_DR    (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART1_BRR   (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART1_CR1   (*(volatile uint32_t *)(USART1_BASE + 0x0C))

#define SR_RXNE  (1 << 5)   // є байт в DR
#define SR_TXE   (1 << 7)   // DR порожній
#define CR1_UE   (1 << 13)  // UART enable
#define CR1_TE   (1 << 3)   // TX enable
#define CR1_RE   (1 << 2)   // RX enable
#define CR1_RXNEIE (1 << 5) // переривання по RXNE

// NVIC
#define NVIC_ISER1   (*(volatile uint32_t *)0xE000E104) // IRQ32-63

// простий ring buffer
#define BUF_SIZE 32
static volatile char rx_buf[BUF_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

// TX — polling (залишаємо як є)
static void uart_putc(char c) {
    while (!(USART1_SR & SR_TXE));
    USART1_DR = c;
}

static void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}

// ISR — викликається коли прийшов байт
void USART1_IRQHandler(void) {
    if (USART1_SR & SR_RXNE) {
        char c = (char)USART1_DR;  // читання DR скидає RXNE автоматично

        // кладемо в ring buffer
        uint8_t next = (rx_head + 1) % BUF_SIZE;
        if (next != rx_tail) {     // якщо не переповнений
            rx_buf[rx_head] = c;
            rx_head = next;
        }
    }
}

// читаємо з буфера (не блокує!)
static uint8_t uart_available(void) {
    return rx_head != rx_tail;
}

static char uart_getc(void) {
    while (!uart_available());     // чекаємо якщо порожньо
    char c = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) % BUF_SIZE;
    return c;
}

int main(void) {
    // тактування GPIOA, GPIOC, USART1
    RCC_APB2ENR |= (1 << 2) | (1 << 4) | (1 << 14);

    // PC13 — output
    GPIOC_CRH &= ~(0xF << 20);
    GPIOC_CRH |=  (0x2 << 20);
    GPIOC_BSRR = (1 << 13);  // LED off

    // PA9 TX — Alternate Function Push-Pull 50MHz = 0xB
    GPIOA_CRL &= ~(0xF << 4);   // PA9 в CRH!
    // виправляємо — PA9 в CRH
    *(volatile uint32_t *)(GPIOA_BASE + 0x04) &= ~(0xF << 4);
    *(volatile uint32_t *)(GPIOA_BASE + 0x04) |=  (0xB << 4);

    // PA10 RX — input floating
    *(volatile uint32_t *)(GPIOA_BASE + 0x04) &= ~(0xF << 8);
    *(volatile uint32_t *)(GPIOA_BASE + 0x04) |=  (0x4 << 8);

    // USART1: 9600 baud, TX+RX + RXNE interrupt
    USART1_BRR = 8000000UL / 9600;
    USART1_CR1 = CR1_UE | CR1_TE | CR1_RE | CR1_RXNEIE;

    // NVIC: USART1 = IRQ37, тому ISER1 біт 5 (37 - 32 = 5)
    NVIC_ISER1 |= (1 << 5);

    uart_puts("UART IRQ ready\r\n");

    char cmd[16];
    uint8_t i = 0;

    while (1) {
        if (uart_available()) {
            char c = uart_getc();

            if (c == '\r' || c == '\n') {
                cmd[i] = '\0';

                if (cmd[0] == 'o' && cmd[1] == 'n') {
                    GPIOC_BSRR = (1 << (13 + 16));  // LED on
                    uart_puts("LED on\r\n");
                } else if (cmd[0] == 'o' && cmd[1] == 'f') {
                    GPIOC_BSRR = (1 << 13);          // LED off
                    uart_puts("LED off\r\n");
                } else if (i > 0) {
                    uart_puts("unknown: ");
                    uart_puts(cmd);
                    uart_puts("\r\n");
                }
                i = 0;
            } else if (i < 15) {
                cmd[i++] = c;
            }
        }
    }
} 