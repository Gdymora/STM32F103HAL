#ifndef HAL_I2C_H
#define HAL_I2C_H

#include <stdint.h>

// I2C1 регістри
#define I2C1_BASE    0x40005400
#define I2C1_CR1     (*(volatile uint32_t *)(I2C1_BASE + 0x00))
#define I2C1_CR2     (*(volatile uint32_t *)(I2C1_BASE + 0x04))
#define I2C1_DR      (*(volatile uint32_t *)(I2C1_BASE + 0x10))
#define I2C1_SR1     (*(volatile uint32_t *)(I2C1_BASE + 0x14))
#define I2C1_SR2     (*(volatile uint32_t *)(I2C1_BASE + 0x18))
#define I2C1_CCR     (*(volatile uint32_t *)(I2C1_BASE + 0x1C))
#define I2C1_TRISE   (*(volatile uint32_t *)(I2C1_BASE + 0x20))

// SR1 біти
#define I2C_SR1_SB     (1 << 0)
#define I2C_SR1_ADDR   (1 << 1)
#define I2C_SR1_BTF    (1 << 2)
#define I2C_SR1_RXNE   (1 << 6)
#define I2C_SR1_TXE    (1 << 7)

// CR1 біти
#define I2C_CR1_PE     (1 << 0)
#define I2C_CR1_START  (1 << 8)
#define I2C_CR1_STOP   (1 << 9)
#define I2C_CR1_ACK    (1 << 10)

// ініціалізація I2C1 — PB6=SCL, PB7=SDA, 100кГц
void i2c1_init(void);

// записати байт в регістр пристрою
void i2c1_write(uint8_t addr, uint8_t reg, uint8_t data);

// читати один байт з регістра пристрою
uint8_t i2c1_read(uint8_t addr, uint8_t reg);

// читати кілька байт
void i2c1_read_buf(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif