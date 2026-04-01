#include <stdint.h>

// RCC регістри
#define RCC_BASE 0x40021000
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18))

// GPIOC регістри
#define GPIOC_BASE 0x40011000
#define GPIOC_CRH (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))

// біти
#define RCC_APB2ENR_IOPCEN (1 << 4)
#define GPIO_PIN_13 (1 << 13)
#define GPIO_PIN_14 (1 << 14)
#define GPIO_PIN_15 (1 << 15)

void delay(volatile uint32_t n)
{
    while (n--)
        ;
}

int main(void)
{
    // 1. вмикаємо тактування порту C
    RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;

    // 2. PC13 — вихід push-pull 2MHz
    // CRH керує пінами 8-15, PC13 — біти [23:20]
    // 0b0010 = output push-pull 2MHz

    // 2. налаштування PC13, PC14, PC15 як output
    GPIOC_CRH &= ~(
        (0xF << 20) |
        (0xF << 24) |
        (0xF << 28));

    GPIOC_CRH |= ((0x2 << 20) |
                  (0x2 << 24) |
                  (0x2 << 28));

    while (1)
    {
        GPIOC_ODR ^= GPIO_PIN_13;
        delay(300000);

        GPIOC_ODR ^= GPIO_PIN_14;
        delay(300000);

        GPIOC_ODR ^= GPIO_PIN_15;
        delay(300000);
    }

    return 0;
}