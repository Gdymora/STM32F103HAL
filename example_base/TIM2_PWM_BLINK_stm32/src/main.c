// main.c — TIM2 PWM, плавне дихання LED
// PA0 = TIM2 CH1, Alternate Function
// PSC=7 → 1MHz, ARR=999 → 1кГц PWM

#include <stdint.h>

// RCC
#define RCC_BASE     0x40021000
#define RCC_APB2ENR  (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR  (*(volatile uint32_t *)(RCC_BASE + 0x1C))

// GPIOA
#define GPIOA_BASE   0x40010800
#define GPIOA_CRL    (*(volatile uint32_t *)(GPIOA_BASE + 0x00))

// TIM2
#define TIM2_BASE    0x40000000
#define TIM2_CR1     (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM2_CCMR1   (*(volatile uint32_t *)(TIM2_BASE + 0x18))
#define TIM2_CCER    (*(volatile uint32_t *)(TIM2_BASE + 0x20))
#define TIM2_PSC     (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM2_ARR     (*(volatile uint32_t *)(TIM2_BASE + 0x2C))
#define TIM2_CCR1    (*(volatile uint32_t *)(TIM2_BASE + 0x34))
#define TIM2_EGR     (*(volatile uint32_t *)(TIM2_BASE + 0x14))

// SysTick для delay_ms
#define SYSTICK_BASE       0xE000E010
#define STK_CTRL           (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define STK_LOAD           (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define STK_VAL            (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))
#define STK_CTRL_ENABLE    (1 << 0)
#define STK_CTRL_TICKINT   (1 << 1)
#define STK_CTRL_CLKSOURCE (1 << 2)

static volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void) {
    ms_ticks++;
}

static void delay_ms(uint32_t ms) {
    uint32_t start = ms_ticks;
    while (ms_ticks - start < ms);
}

int main(void) {
    // тактування GPIOA і TIM2
    RCC_APB2ENR |= (1 << 2);   // GPIOA
    RCC_APB1ENR |= (1 << 0);   // TIM2

    // PA0 — Alternate Function Push-Pull 50MHz = 0xB
    GPIOA_CRL &= ~(0xF << 0);
    GPIOA_CRL |=  (0xB << 0);

    // SysTick 1мс
    STK_LOAD = 8000000UL / 1000 - 1;
    STK_VAL  = 0;
    STK_CTRL = STK_CTRL_ENABLE | STK_CTRL_TICKINT | STK_CTRL_CLKSOURCE;

    // TIM2 PWM
    // PSC=7 → 8MHz/(7+1) = 1MHz
    // ARR=999 → період 1000мкс = 1кГц
    TIM2_PSC  = 7;
    TIM2_ARR  = 999;
    TIM2_CCR1 = 0;  // duty cycle 0%

    // CCMR1: PWM mode 1 на CH1
    // OC1M = 110 (біти 6:4), OC1PE = 1 (біт 3)
    TIM2_CCMR1 = (6 << 4) | (1 << 3);

    // CCER: CC1E = 1 — увімкнути CH1
    TIM2_CCER = (1 << 0);

    // EGR: завантажити PSC і ARR
    TIM2_EGR = (1 << 0);

    // CR1: ARPE=1, CEN=1
    TIM2_CR1 = (1 << 7) | (1 << 0);

    while (1) {
        // плавно вгору 0% → 100%
        for (int i = 0; i <= 999; i++) {
            TIM2_CCR1 = i;
            delay_ms(1);
        }
        // плавно вниз 100% → 0%
        for (int i = 999; i >= 0; i--) {
            TIM2_CCR1 = i;
            delay_ms(1);
        }
    }
}