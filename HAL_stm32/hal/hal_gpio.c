#include "hal_gpio.h"

static void rcc_enable(uint32_t port) {
    switch (port) {
        case GPIOA_BASE: RCC_APB2ENR |= (1 << 2); break;
        case GPIOB_BASE: RCC_APB2ENR |= (1 << 3); break;
        case GPIOC_BASE: RCC_APB2ENR |= (1 << 4); break;
    }
}

void gpio_init(uint32_t pin, uint8_t mode) {
    uint32_t port   = UNPACK_PORT(pin);
    uint8_t  num    = UNPACK_PIN(pin);

    // вмикаємо тактування потрібного порту
    rcc_enable(port);

    uint8_t shift;
    volatile uint32_t *cr;

    if (num < 8) {
        cr    = &GPIO_CRL(port);
        shift = num * 4;
    } else {
        cr    = &GPIO_CRH(port);
        shift = (num - 8) * 4;
    }

    *cr &= ~(0xF << shift);
    *cr |=  (mode << shift);

    // для INPUT_PULLUP — підтягуємо через ODR
    if (mode == INPUT_PULLUP) {
        GPIO_ODR(port) |= (1 << num);
    }
}

void gpio_write(uint32_t pin, uint8_t state) {
    uint32_t port = UNPACK_PORT(pin);
    uint8_t  num  = UNPACK_PIN(pin);

    if (state) {
        GPIO_BSRR(port) = (1 << num);           // SET
    } else {
        GPIO_BSRR(port) = (1 << (num + 16));    // RESET
    }
}

void gpio_toggle(uint32_t pin) {
    uint32_t port = UNPACK_PORT(pin);
    uint8_t  num  = UNPACK_PIN(pin);
    GPIO_ODR(port) ^= (1 << num);
}

uint8_t gpio_read(uint32_t pin) {
    uint32_t port = UNPACK_PORT(pin);
    uint8_t  num  = UNPACK_PIN(pin);
    return (GPIO_IDR(port) >> num) & 1;
}