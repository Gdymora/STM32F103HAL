// main.c — TIM2 переривання, мигаємо LED без delay_ms і без SysTick
// Blue Pill STM32F103C8T6
// PC13 LED

#include <stdint.h>

// RCC
#define RCC_BASE 0x40021000
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR (*(volatile uint32_t *)(RCC_BASE + 0x1C))

// GPIOC
#define GPIOC_BASE 0x40011000
#define GPIOC_CRH (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))

// TIM2
#define TIM2_BASE 0x40000000
#define TIM2_CR1 (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM2_DIER (*(volatile uint32_t *)(TIM2_BASE + 0x0C))
#define TIM2_SR (*(volatile uint32_t *)(TIM2_BASE + 0x10))
#define TIM2_PSC (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM2_ARR (*(volatile uint32_t *)(TIM2_BASE + 0x2C))
#define TIM2_EGR (*(volatile uint32_t *)(TIM2_BASE + 0x14))

// NVIC — TIM2 = IRQ28, тому ISER0 біт 28
#define NVIC_ISER0 (*(volatile uint32_t *)0xE000E100)

void TIM2_IRQHandler(void)
{
    TIM2_SR = 0;
    // GPIOC_ODR &= ~(1 << 13); // просто вмикаємо LED
     GPIOC_ODR ^= (1 << 13);  // toggle — вмикаємо/вимикаємо
}

int main(void)
{
    // тактування GPIOC і TIM2
    RCC_APB2ENR |= (1 << 4); // GPIOC
    RCC_APB1ENR |= (1 << 0); // TIM2 на APB1!

    // PC13 output
    GPIOC_CRH &= ~(0xF << 20);
    GPIOC_CRH |= (0x2 << 20);

    // TIM2: переривання кожні 500мс
    // PSC = 7999 → 8MHz / (7999+1) = 1000 Гц → 1 тік = 1мс
    TIM2_PSC = 7999;
    TIM2_ARR = 499;

    // спочатку DIER і NVIC
    TIM2_DIER = (1 << 0);
    NVIC_ISER0 |= (1 << 28);

    // потім EGR → скидаємо SR → вмикаємо
    TIM2_EGR = (1 << 0);
    TIM2_SR = 0;
    TIM2_CR1 = (1 << 0); 

    while (1)
    {
        // нічого — вся логіка в ISR
    }
}