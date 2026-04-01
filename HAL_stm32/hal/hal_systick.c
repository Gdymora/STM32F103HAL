#include "hal_systick.h"

#define SYSTICK_BASE       0xE000E010
#define STK_CTRL           (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define STK_LOAD           (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define STK_VAL            (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))

#define STK_CTRL_ENABLE    (1 << 0)
#define STK_CTRL_TICKINT   (1 << 1)
#define STK_CTRL_CLKSOURCE (1 << 2)

static volatile uint32_t _ticks = 0;

void SysTick_Handler(void) {
    _ticks++;
}

void systick_init(uint32_t cpu_hz) {
    STK_LOAD = (cpu_hz / 1000) - 1;
    STK_VAL  = 0;
    STK_CTRL = STK_CTRL_ENABLE | STK_CTRL_TICKINT | STK_CTRL_CLKSOURCE;
}

void delay_ms(uint32_t ms) {
    uint32_t start = _ticks;
    while (_ticks - start < ms);
}

uint32_t get_ticks(void) {
    return _ticks;
}