#ifndef HAL_SPI_H
#define HAL_SPI_H

#include <stdint.h>

// SPI1 регістри
#define SPI1_BASE    0x40013000
#define SPI1_CR1     (*(volatile uint32_t *)(SPI1_BASE + 0x00))
#define SPI1_SR      (*(volatile uint32_t *)(SPI1_BASE + 0x08))
#define SPI1_DR      (*(volatile uint32_t *)(SPI1_BASE + 0x0C))

#define SPI_SR_TXE   (1 << 1)
#define SPI_SR_BSY   (1 << 7)

// ініціалізація SPI1
// PA5=SCK, PA7=MOSI, PA6=MISO
void spi1_init(void);

// відправити один байт
void spi1_send(uint8_t data);

#endif