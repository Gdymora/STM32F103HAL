#include "hal_i2c.h"
#include "hal_gpio.h"

#define RCC_APB1ENR (*(volatile uint32_t *)(0x40021000 + 0x1C))

void i2c1_init(void)
{
    // тактування GPIOB + I2C1
    RCC_APB2ENR |= (1 << 3);  // GPIOB
    RCC_APB1ENR |= (1 << 21); // I2C1

    // PB6 SCL, PB7 SDA — AF open-drain 50MHz = 0xF
    gpio_init(PIN_PB6, 0xF);
    gpio_init(PIN_PB7, 0xF);

    // I2C1: 100кГц, PCLK1=8MHz
    I2C1_CR2 = 8;          // FREQ = 8MHz
    I2C1_CCR = 40;         // 8MHz / (2*100kHz) = 40
    I2C1_TRISE = 9;        // (1000нс/125нс) + 1 = 9
    I2C1_CR1 = I2C_CR1_PE; // увімкнути
}

static void i2c_start(void)
{
    I2C1_CR1 |= I2C_CR1_START;
    while (!(I2C1_SR1 & I2C_SR1_SB))
        ;
}

static void i2c_stop(void)
{
    I2C1_CR1 |= I2C_CR1_STOP;
}

static void i2c_send_addr(uint8_t addr, uint8_t rw)
{
    I2C1_DR = (addr << 1) | rw;
    while (!(I2C1_SR1 & I2C_SR1_ADDR))
        ;
    (void)I2C1_SR2; // скидаємо ADDR
}

static void i2c_send_byte(uint8_t data)
{
    while (!(I2C1_SR1 & I2C_SR1_TXE))
        ;
    I2C1_DR = data;
    while (!(I2C1_SR1 & I2C_SR1_BTF))
        ;
}

void i2c1_write(uint8_t addr, uint8_t reg, uint8_t data)
{
    i2c_start();
    i2c_send_addr(addr, 0); // write
    i2c_send_byte(reg);
    i2c_send_byte(data);
    i2c_stop();
}

uint8_t i2c1_read(uint8_t addr, uint8_t reg)
{
    i2c_start();
    i2c_send_addr(addr, 0); // write — вказуємо регістр
    i2c_send_byte(reg);

    i2c_start();            // repeated start
    i2c_send_addr(addr, 1); // read

    I2C1_CR1 &= ~I2C_CR1_ACK; // NACK — останній байт
    i2c_stop();
    while (!(I2C1_SR1 & I2C_SR1_RXNE))
        ;
    return (uint8_t)I2C1_DR;
}

void i2c1_read_buf(uint8_t addr, uint8_t reg,
                   uint8_t *buf, uint8_t len)
{
    i2c_start();
    i2c_send_addr(addr, 0);
    i2c_send_byte(reg);

    i2c_start();
    i2c_send_addr(addr, 1);

    I2C1_CR1 |= I2C_CR1_ACK; // ACK для всіх крім останнього

    for (uint8_t i = 0; i < len; i++)
    {
        if (i == len - 1)
        {
            I2C1_CR1 &= ~I2C_CR1_ACK; // NACK на останній
            i2c_stop();
        }
        while (!(I2C1_SR1 & I2C_SR1_RXNE))
            ;
        buf[i] = (uint8_t)I2C1_DR;
    }
}

void i2c1_write_buf(uint8_t addr, uint8_t reg,
                    uint8_t *data, uint8_t len)
{
    i2c_start();
    i2c_send_addr(addr, 0); // write
    i2c_send_byte(reg);     // перший регістр
    for (uint8_t i = 0; i < len; i++)
    {
        i2c_send_byte(data[i]);
    }
    i2c_stop();
}