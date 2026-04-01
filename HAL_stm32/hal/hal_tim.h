#ifndef HAL_TIM_H
#define HAL_TIM_H

#include <stdint.h>

// TIM2 регістри — потрібні в ISR в main.c
#define TIM2_BASE    0x40000000
#define TIM2_CR1     (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM2_DIER    (*(volatile uint32_t *)(TIM2_BASE + 0x0C))
#define TIM2_SR      (*(volatile uint32_t *)(TIM2_BASE + 0x10))
#define TIM2_EGR     (*(volatile uint32_t *)(TIM2_BASE + 0x14))
#define TIM2_CCMR1   (*(volatile uint32_t *)(TIM2_BASE + 0x18))
#define TIM2_CCER    (*(volatile uint32_t *)(TIM2_BASE + 0x20))
#define TIM2_PSC     (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM2_ARR     (*(volatile uint32_t *)(TIM2_BASE + 0x2C))
#define TIM2_CCR1    (*(volatile uint32_t *)(TIM2_BASE + 0x34))

// TIM2_SR біти
#define TIM_SR_UIF   (1 << 0)  // update interrupt flag

// ініціалізація TIM2 з перериванням
// period_ms — період в мілісекундах
void tim2_init_irq(uint32_t period_ms);

// ініціалізація TIM2 PWM на PA0 (CH1)
// freq_hz  — частота PWM (наприклад 1000 = 1кГц)
// arr      — роздільна здатність (наприклад 999)
void tim2_init_pwm(uint32_t freq_hz, uint32_t arr);

// встановити duty cycle (0 до arr)
void tim2_pwm_set(uint32_t duty);

#endif