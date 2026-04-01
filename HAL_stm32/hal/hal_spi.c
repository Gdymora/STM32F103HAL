#include "hal_spi.h"
#include "hal_gpio.h"


void spi1_init(void) {
    // тактування GPIOA + SPI1
    RCC_APB2ENR |= (1 << 2) | (1 << 12);

    // PA5 SCK, PA7 MOSI — AF push-pull 50MHz
    gpio_init(PIN_PA5, OUTPUT_AF_FAST);
    gpio_init(PIN_PA7, OUTPUT_AF_FAST);

    // PA6 MISO — input floating
    gpio_init(PIN_PA6, INPUT);

    // SPI1: master, fPCLK/8, SSM+SSI, MSB first
    SPI1_CR1 = (1 << 2)  |  // MSTR
               (1 << 9)  |  // SSM
               (1 << 8)  |  // SSI
               (2 << 3)  |  // BR = fPCLK/8
               (1 << 6);    // SPE
}

void spi1_send(uint8_t data) {
    while (!(SPI1_SR & SPI_SR_TXE));
    SPI1_DR = data;
    while (SPI1_SR & SPI_SR_BSY);
}