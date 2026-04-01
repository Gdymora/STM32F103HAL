#ifndef HAL_EXTI_H
#define HAL_EXTI_H

#include <stdint.h>
#include "hal_gpio.h"

// тригери
#define EXTI_RISING   0
#define EXTI_FALLING  1
#define EXTI_BOTH     2

// EXTI регістри — потрібні в ISR в main.c
#define EXTI_BASE    0x40010400
#define EXTI_IMR     (*(volatile uint32_t *)(EXTI_BASE + 0x00))
#define EXTI_RTSR    (*(volatile uint32_t *)(EXTI_BASE + 0x08))
#define EXTI_FTSR    (*(volatile uint32_t *)(EXTI_BASE + 0x0C))
#define EXTI_PR      (*(volatile uint32_t *)(EXTI_BASE + 0x14))

void exti_init(uint32_t pin, uint8_t trigger);

#endif