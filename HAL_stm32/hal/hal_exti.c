#include "hal_exti.h"

#define AFIO_BASE    0x40010000
#define AFIO_EXTICR1 (*(volatile uint32_t *)(AFIO_BASE + 0x08))
#define AFIO_EXTICR2 (*(volatile uint32_t *)(AFIO_BASE + 0x0C))
#define AFIO_EXTICR3 (*(volatile uint32_t *)(AFIO_BASE + 0x10))
#define AFIO_EXTICR4 (*(volatile uint32_t *)(AFIO_BASE + 0x14))

#define EXTI_BASE    0x40010400
#define EXTI_IMR     (*(volatile uint32_t *)(EXTI_BASE + 0x00))
#define EXTI_RTSR    (*(volatile uint32_t *)(EXTI_BASE + 0x08))
#define EXTI_FTSR    (*(volatile uint32_t *)(EXTI_BASE + 0x0C))

#define NVIC_ISER0   (*(volatile uint32_t *)0xE000E100)
#define NVIC_ISER1   (*(volatile uint32_t *)0xE000E104)

// отримати код порту для AFIO (PA=0, PB=1, PC=2)
static uint8_t port_code(uint32_t port) {
    switch (port) {
        case GPIOA_BASE: return 0;
        case GPIOB_BASE: return 1;
        case GPIOC_BASE: return 2;
        default:         return 0;
    }
}

void exti_init(uint32_t pin, uint8_t trigger) {
    uint32_t port = UNPACK_PORT(pin);
    uint8_t  num  = UNPACK_PIN(pin);

    // тактування AFIO
    RCC_APB2ENR |= (1 << 0);

    // налаштовуємо GPIO як вхід
    gpio_init(pin, INPUT_PULLUP);

    // AFIO — підключаємо порт до лінії EXTI
    uint8_t code = port_code(port);
    volatile uint32_t *exticr;
    uint8_t shift;

    if      (num < 4)  { exticr = &AFIO_EXTICR1; shift = num * 4; }
    else if (num < 8)  { exticr = &AFIO_EXTICR2; shift = (num - 4) * 4; }
    else if (num < 12) { exticr = &AFIO_EXTICR3; shift = (num - 8) * 4; }
    else               { exticr = &AFIO_EXTICR4; shift = (num - 12) * 4; }

    *exticr &= ~(0xF << shift);
    *exticr |=  (code << shift);

    // EXTI тригер
    if (trigger == EXTI_RISING || trigger == EXTI_BOTH)
        EXTI_RTSR |= (1 << num);
    if (trigger == EXTI_FALLING || trigger == EXTI_BOTH)
        EXTI_FTSR |= (1 << num);

    // unmask
    EXTI_IMR |= (1 << num);

    // NVIC — IRQ номери для EXTI
    // EXTI0-4: IRQ6-10 → ISER0
    // EXTI5-9: IRQ23   → ISER0
    // EXTI10-15: IRQ40 → ISER1
    if (num <= 4) {
        NVIC_ISER0 |= (1 << (num + 6));
    } else if (num <= 9) {
        NVIC_ISER0 |= (1 << 23);
    } else {
        NVIC_ISER1 |= (1 << (40 - 32));
    }
}