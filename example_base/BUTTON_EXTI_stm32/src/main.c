// main.c — EXTI bare-metal, кнопка PA1 → LED PC13
#include <stdint.h>

// RCC
#define RCC_BASE 0x40021000
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18))

// GPIOA
#define GPIOA_BASE 0x40010800
#define GPIOA_CRL (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

// GPIOC
#define GPIOC_BASE 0x40011000
#define GPIOC_CRH (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))
#define GPIOC_BSRR (*(volatile uint32_t *)(GPIOC_BASE + 0x10))

// AFIO — підключає пін до EXTI лінії
#define AFIO_BASE 0x40010000
#define AFIO_EXTICR1 (*(volatile uint32_t *)(AFIO_BASE + 0x08))

// EXTI
#define EXTI_BASE 0x40010400
#define EXTI_IMR (*(volatile uint32_t *)(EXTI_BASE + 0x00))
#define EXTI_FTSR (*(volatile uint32_t *)(EXTI_BASE + 0x0C))
#define EXTI_PR (*(volatile uint32_t *)(EXTI_BASE + 0x14))

// NVIC — тільки те що треба для EXTI1
#define NVIC_ISER0 (*(volatile uint32_t *)0xE000E100)

static uint8_t led_state = 0;

void EXTI1_IRQHandler(void)
{
    if (EXTI_PR & (1 << 1))
    { // перевіряємо що це EXTI1
        // Проста затримка (антибрязк)
        for (volatile int i = 0; i < 50000; i++)
            ;
        led_state ^= 1;
        if (led_state)
        {
            GPIOC_BSRR = (1 << (13 + 16)); // LOW — LED on
        }
        else
        {
            GPIOC_BSRR = (1 << 13); // HIGH — LED off
        }
        EXTI_PR = (1 << 1); // скидаємо pending flag — ОБОВ'ЯЗКОВО
    }
}

int main(void)
{
    // тактування GPIOA, GPIOC, AFIO
    RCC_APB2ENR |= (1 << 2) | (1 << 4) | (1 << 0);

    // PC13 — output push-pull 2MHz
    GPIOC_CRH &= ~(0xF << 20);
    GPIOC_CRH |= (0x2 << 20);
    GPIOC_ODR |= (1 << 13); // LED off спочатку

    // PA1 — input pull-up
    GPIOA_CRL &= ~(0xF << 4);
    GPIOA_CRL |= (0x8 << 4); // input pull-up mode
    GPIOA_ODR |= (1 << 1);   // підтяжка вгору

    // AFIO: EXTI1 → порт A (значення 0000)
    AFIO_EXTICR1 &= ~(0xF << 4); // біти [7:4] = 0000 → PA

    // EXTI1: falling edge (кнопка до GND)
    EXTI_IMR |= (1 << 1);  // Одиниця, зсунута на 1 позицію = EXTI1
    EXTI_FTSR |= (1 << 1); // Налаштовуємо саме другу лінію (індекс 1)

    // NVIC: IRQ7 = EXTI1
    NVIC_ISER0 |= (1 << 7); // Вмикаємо 7-й канал, який залізно прив'язаний до EXTI1

    // Увімкнути глобальні переривання (Enable interrupts)
    // __asm volatile ("cpsie i" : : : "memory");

    while (1)
    {
        // нічого! вся логіка в ISR
    }
}
/* Три моменти на які зверни увагу:
EXTI_PR = (1 << 1) — скидається записом 1, не 0. Якщо забути — переривання буде викликатись нескінченно.
NVIC_ISER0 |= (1 << 7) — EXTI1 це IRQ номер 7 в таблиці STM32F103. Береться з RM0008 таблиця переривань.
AFIO_EXTICR1 — якщо кнопка на PB1 замість PA1 — міняємо на 0x1 << 4. PC1 — 0x2 << 4.
*/