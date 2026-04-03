#ifndef HAL_H
#define HAL_H

#include "hal_gpio.h"
#include "hal_systick.h"
#include "hal_uart.h"
#include "hal_exti.h"
#include "hal_tim.h"
#include "hal_spi.h"
#include "hal_i2c.h"
#include "hal_adc.h"
#include "hal_ds1307.h"

// Blue Pill = STM32F103 = 8MHz HSI за замовчуванням
#define CPU_HZ  8000000UL

void hal_init(void);

#endif