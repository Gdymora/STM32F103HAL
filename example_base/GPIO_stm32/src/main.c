#include <stdint.h>

// RCC регістри
#define RCC_BASE 0x40021000
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18))

/*
   Кожен порт має свою базову адресу.
   Згідно з документацією (Reference Manual):
      GPIOA_BASE: 0x40010800
      GPIOB_BASE: 0x40010C00
      GPIOC_BASE: 0x40011000
*/

// GPIOA регістри
#define GPIOA_BASE 0x40010800
#define GPIOA_CRL (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

// GPIOB регістри
#define GPIOB_BASE 0x40010C00
#define GPIOB_CRL (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_ODR (*(volatile uint32_t *)(GPIOB_BASE + 0x0C))

// GPIOC регістри
#define GPIOC_BASE 0x40011000
#define GPIOC_CRH (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))

#define GPIOC_BSRR (*(volatile uint32_t *)(GPIOC_BASE + 0x10))

// біти для тактування портів A
#define RCC_APB2ENR_IOPAEN (1 << 2)
#define GPIO_PIN_0 (1 << 0)

// біти для тактування портів  B
#define RCC_APB2ENR_IOPBEN (1 << 3)

// біти для тактування портів  C
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
    // 1. Вмикаємо тактування для Port A та Port C
    RCC_APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN);

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

    // 2. налаштування PC13, PC14, PC15 як output
    GPIOA_CRL &= ~(
        (0xF << 20) |
        (0xF << 24) |
        (0xF << 28));

    GPIOA_CRL |= ((0x2 << 0));

    while (1)
    {
        // GPIOC_ODR ^= GPIO_PIN_13;
        // delay(300000);
        /* в реальних проектах частіше використовують BSRR (Bit Set/Reset Register) */

        GPIOC_BSRR = (1 << 13); // Увімкнути LED (Set)
        delay(300000);
        // щоб вимкнути 13-й пін? (Підказка: це 13 + 16-й біт).

        GPIOC_BSRR = (1 << (13 + 16)); // Вимкнути LED (Reset)
        delay(300000);

        GPIOC_ODR ^= GPIO_PIN_14;
        delay(300000);

        GPIOC_ODR ^= GPIO_PIN_15;
        delay(300000);

        GPIOA_ODR ^= GPIO_PIN_0;
        delay(300000);
    }

    return 0;
}