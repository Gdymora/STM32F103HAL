#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdint.h>

// ADC1 регістри
#define ADC1_BASE    0x40012400
#define ADC1_SR      (*(volatile uint32_t *)(ADC1_BASE + 0x00))
#define ADC1_CR1     (*(volatile uint32_t *)(ADC1_BASE + 0x04))
#define ADC1_CR2     (*(volatile uint32_t *)(ADC1_BASE + 0x08))
#define ADC1_SMPR2   (*(volatile uint32_t *)(ADC1_BASE + 0x14))
#define ADC1_SQR3    (*(volatile uint32_t *)(ADC1_BASE + 0x34))
#define ADC1_DR      (*(volatile uint32_t *)(ADC1_BASE + 0x4C))

// SR біти
#define ADC_SR_EOC   (1 << 1)  // End Of Conversion

// CR2 біти
#define ADC_CR2_ADON    (1 << 0)   // увімкнути ADC
#define ADC_CR2_CAL     (1 << 2)   // калібрування
#define ADC_CR2_SWSTART (1 << 22)  // старт перетворення
#define ADC_CR2_EXTTRIG (1 << 20)  // зовнішній тригер

// ініціалізація ADC1
// channel — номер каналу (0=PA0, 1=PA1 ... 9=PB1)
void adc1_init(uint8_t channel);

// читати значення (0-4095)
uint16_t adc1_read(void);

#endif