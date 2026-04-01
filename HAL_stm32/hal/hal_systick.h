#ifndef HAL_SYSTICK_H
#define HAL_SYSTICK_H

#include <stdint.h>

void systick_init(uint32_t cpu_hz);
void delay_ms(uint32_t ms);
uint32_t get_ticks(void);  // повертає мс від старту
#endif