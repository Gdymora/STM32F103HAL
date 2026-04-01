#include <stdint.h>

// GPIOC
#define GPIOC_BASE  0x40011000
#define GPIOC_CRH   (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_BSRR  (*(volatile uint32_t *)(GPIOC_BASE + 0x10))
#define GPIOC_ODR   (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))

// RCC
#define RCC_BASE    0x40021000
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18))

// SysTick
#define SYSTICK_BASE  0xE000E010
#define STK_CTRL      (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define STK_LOAD      (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define STK_VAL       (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))

#define STK_CTRL_ENABLE    (1 << 0)
#define STK_CTRL_TICKINT   (1 << 1)  // ← вмикаємо переривання!
#define STK_CTRL_CLKSOURCE (1 << 2)

// лічильник мілісекунд
static volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void) {
    ms_ticks++;
}

int main(void) {
    // тактування GPIOC
    RCC_APB2ENR |= (1 << 4);

    // PC13 output
    GPIOC_CRH &= ~(0xF << 20);
    GPIOC_CRH |=  (0x2 << 20);
    GPIOC_BSRR = (1 << 13);  // LED off

    // SysTick: переривання кожну 1мс
    STK_LOAD = 8000000UL / 1000 - 1;  // 8MHz → 8000 тіків = 1мс
    STK_VAL  = 0;
    STK_CTRL = STK_CTRL_ENABLE | STK_CTRL_TICKINT | STK_CTRL_CLKSOURCE;

    uint32_t last = 0;

    while (1) {
        // не blocking! main продовжує працювати
        if (ms_ticks - last >= 500) {
            last = ms_ticks;
            GPIOC_ODR ^= (1 << 13);  // toggle LED
        }
    }
}