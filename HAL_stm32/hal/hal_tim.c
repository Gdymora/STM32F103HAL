#include "hal_tim.h"
#include "hal_gpio.h"

#define RCC_APB1ENR (*(volatile uint32_t *)(0x40021000 + 0x1C))
#define NVIC_ISER0  (*(volatile uint32_t *)0xE000E100)

void tim2_init_irq(uint32_t period_ms) {
    RCC_APB1ENR |= (1 << 0);  // TIM2 ← обов'язково!

    TIM2_PSC  = 7999;
    TIM2_ARR  = period_ms - 1;
    TIM2_DIER = (1 << 0);
    TIM2_EGR  = (1 << 0);
    TIM2_SR   = 0;
    NVIC_ISER0 |= (1 << 28);
    TIM2_CR1  = (1 << 0);
}

void tim2_init_pwm(uint32_t freq_hz, uint32_t arr) {
    RCC_APB1ENR |= (1 << 0);  // TIM2

    uint32_t psc = (8000000UL / (freq_hz * (arr + 1))) - 1;
    TIM2_PSC  = psc;
    TIM2_ARR  = arr;
    TIM2_CCR1 = 0;

    TIM2_CCMR1 = (6 << 4) | (1 << 3);
    TIM2_CCER  = (1 << 0);
    TIM2_EGR   = (1 << 0);
    TIM2_CR1   = (1 << 7) | (1 << 0);
}

void tim2_pwm_set(uint32_t duty) {
    TIM2_CCR1 = duty;
}