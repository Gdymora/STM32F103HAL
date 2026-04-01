#include <stdint.h>

// RCC регістри
#define RCC_BASE     0x40021000
#define RCC_APB2ENR  (*(volatile uint32_t *)(RCC_BASE + 0x18))

// GPIOC регістри
#define GPIOC_BASE   0x40011000
#define GPIOC_CRH    (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR    (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))

// біти
#define RCC_APB2ENR_IOPCEN  (1 << 4)
#define GPIO_PIN_13         (1 << 13)

void delay(volatile uint32_t n) {
    while (n--);
}

int main(void) {
    // 1. вмикаємо тактування порту C
    RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;

    // 2. PC13 — вихід push-pull 2MHz
    // CRH керує пінами 8-15, PC13 — біти [23:20]
    // 0b0010 = output push-pull 2MHz
    GPIOC_CRH &= ~(0xF << 20);  // очищаємо
    GPIOC_CRH |=  (0x2 << 20);  // output push-pull 2MHz

    while (1) {
        GPIOC_ODR ^= GPIO_PIN_13;  // toggle PC13
        delay(500000);
    }

    return 0;
}