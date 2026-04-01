#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>

// базові адреси портів
#define GPIOA_BASE  0x40010800
#define GPIOB_BASE  0x40010C00
#define GPIOC_BASE  0x40011000

// RCC
#define RCC_BASE    0x40021000
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18))

// регістри GPIO через порт
#define GPIO_CRL(port)  (*(volatile uint32_t *)((port) + 0x00))
#define GPIO_CRH(port)  (*(volatile uint32_t *)((port) + 0x04))
#define GPIO_IDR(port)  (*(volatile uint32_t *)((port) + 0x08))
#define GPIO_ODR(port)  (*(volatile uint32_t *)((port) + 0x0C))
#define GPIO_BSRR(port) (*(volatile uint32_t *)((port) + 0x10))

// пакуємо порт і пін в одне uint32_t
// старші 16 біт — адреса порту (зсунута), молодші 8 біт — номер піна
#define PACK_PIN(port, pin) (((uint32_t)(port)) | ((uint32_t)(pin)))
#define UNPACK_PORT(p)      ((uint32_t)((p) & 0xFFFFFF00))
#define UNPACK_PIN(p)       ((uint8_t)((p) & 0xFF))

// зручні макроси для всіх пінів
#define PIN_PA0   PACK_PIN(GPIOA_BASE, 0)
#define PIN_PA1   PACK_PIN(GPIOA_BASE, 1)
#define PIN_PA2   PACK_PIN(GPIOA_BASE, 2)
#define PIN_PA3   PACK_PIN(GPIOA_BASE, 3)
#define PIN_PA4   PACK_PIN(GPIOA_BASE, 4)
#define PIN_PA5   PACK_PIN(GPIOA_BASE, 5)
#define PIN_PA6   PACK_PIN(GPIOA_BASE, 6)
#define PIN_PA7   PACK_PIN(GPIOA_BASE, 7)
#define PIN_PA8   PACK_PIN(GPIOA_BASE, 8)
#define PIN_PA9   PACK_PIN(GPIOA_BASE, 9)
#define PIN_PA10  PACK_PIN(GPIOA_BASE, 10)

#define PIN_PB0   PACK_PIN(GPIOB_BASE, 0)
#define PIN_PB1   PACK_PIN(GPIOB_BASE, 1)
#define PIN_PB6   PACK_PIN(GPIOB_BASE, 6)
#define PIN_PB7   PACK_PIN(GPIOB_BASE, 7)

#define PIN_PC13  PACK_PIN(GPIOC_BASE, 13)
#define PIN_PC14  PACK_PIN(GPIOC_BASE, 14)
#define PIN_PC15  PACK_PIN(GPIOC_BASE, 15)

// режими піна
#define INPUT         0x4  // input floating
#define INPUT_PULLUP  0x8  // input pull-up (+ ODR=1)
#define OUTPUT        0x2  // output push-pull 2MHz
#define OUTPUT_FAST   0x3  // output push-pull 50MHz
#define OUTPUT_AF_FAST  0xB  // Alternate Function Push-Pull 50MHz
// стани
#define LOW   0
#define HIGH  1

// функції
void gpio_init(uint32_t pin, uint8_t mode);
void gpio_write(uint32_t pin, uint8_t state);
void gpio_toggle(uint32_t pin);
uint8_t gpio_read(uint32_t pin);

#endif