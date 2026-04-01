#include "hal_adc.h"
#include "hal_gpio.h"
#include "hal_systick.h"

// піни ADC каналів
static const uint32_t adc_pins[] = {
    PACK_PIN(GPIOA_BASE, 0),  // CH0 = PA0
    PACK_PIN(GPIOA_BASE, 1),  // CH1 = PA1
    PACK_PIN(GPIOA_BASE, 2),  // CH2 = PA2
    PACK_PIN(GPIOA_BASE, 3),  // CH3 = PA3
    PACK_PIN(GPIOA_BASE, 4),  // CH4 = PA4
    PACK_PIN(GPIOA_BASE, 5),  // CH5 = PA5
    PACK_PIN(GPIOA_BASE, 6),  // CH6 = PA6
    PACK_PIN(GPIOA_BASE, 7),  // CH7 = PA7
    PACK_PIN(GPIOB_BASE, 0),  // CH8 = PB0
    PACK_PIN(GPIOB_BASE, 1),  // CH9 = PB1
};

void adc1_init(uint8_t channel) {
    // тактування ADC1 і GPIOA/B
    RCC_APB2ENR |= (1 << 9);   // ADC1
    RCC_APB2ENR |= (1 << 2);   // GPIOA
    RCC_APB2ENR |= (1 << 3);   // GPIOB

    // пін як analog input = 0x0
    if (channel < 10) {
        uint32_t pin = adc_pins[channel];
        uint32_t port = UNPACK_PORT(pin);
        uint8_t  num  = UNPACK_PIN(pin);
        uint8_t  shift = num * 4;
        volatile uint32_t *cr = (num < 8) ?
            (volatile uint32_t *)(port + 0x00) :
            (volatile uint32_t *)(port + 0x04);
        if (num >= 8) shift = (num - 8) * 4;
        *cr &= ~(0xF << shift);  // 0x0 = analog input
    }

    // увімкнути ADC
    ADC1_CR2 = ADC_CR2_ADON;
    delay_ms(1);

    // калібрування
    ADC1_CR2 |= ADC_CR2_CAL;
    while (ADC1_CR2 & ADC_CR2_CAL);

    // sample time = 239.5 циклів для каналу
    ADC1_SMPR2 |= (7 << (channel * 3));

    // вибираємо канал
    ADC1_SQR3 = channel;

    // EXTTRIG + SWSTART режим
    ADC1_CR2 |= ADC_CR2_EXTTRIG | (7 << 17);
}

uint16_t adc1_read(void) {
    ADC1_CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1_SR & ADC_SR_EOC));
    return (uint16_t)ADC1_DR;
}