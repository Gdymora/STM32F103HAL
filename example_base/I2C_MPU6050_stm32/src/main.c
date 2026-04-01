// main.c — I2C + MPU6050 bare-metal
// PB6=SCL, PB7=SDA, PA9=TX UART

#include <stdint.h>

// RCC
#define RCC_BASE     0x40021000
#define RCC_APB2ENR  (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR  (*(volatile uint32_t *)(RCC_BASE + 0x1C))

// GPIOA
#define GPIOA_BASE   0x40010800
#define GPIOA_CRH    (*(volatile uint32_t *)(GPIOA_BASE + 0x04))

// GPIOB
#define GPIOB_BASE   0x40010C00
#define GPIOB_CRL    (*(volatile uint32_t *)(GPIOB_BASE + 0x00))

// USART1
#define USART1_BASE  0x40013800
#define USART1_SR    (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_DR    (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART1_BRR   (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART1_CR1   (*(volatile uint32_t *)(USART1_BASE + 0x0C))
#define SR_TXE       (1 << 7)

// I2C1
#define I2C1_BASE    0x40005400
#define I2C1_CR1     (*(volatile uint32_t *)(I2C1_BASE + 0x00))
#define I2C1_CR2     (*(volatile uint32_t *)(I2C1_BASE + 0x04))
#define I2C1_DR      (*(volatile uint32_t *)(I2C1_BASE + 0x10))
#define I2C1_SR1     (*(volatile uint32_t *)(I2C1_BASE + 0x14))
#define I2C1_SR2     (*(volatile uint32_t *)(I2C1_BASE + 0x18))
#define I2C1_CCR     (*(volatile uint32_t *)(I2C1_BASE + 0x1C))
#define I2C1_TRISE   (*(volatile uint32_t *)(I2C1_BASE + 0x20))

// I2C1_SR1 біти
#define I2C_SR1_SB     (1 << 0)   // Start Bit
#define I2C_SR1_ADDR   (1 << 1)   // Address sent
#define I2C_SR1_TXE    (1 << 7)   // DR порожній
#define I2C_SR1_RXNE   (1 << 6)   // DR не порожній
#define I2C_SR1_BTF    (1 << 2)   // Byte Transfer Finished

// I2C1_CR1 біти
#define I2C_CR1_PE     (1 << 0)   // Peripheral Enable
#define I2C_CR1_START  (1 << 8)   // Start
#define I2C_CR1_STOP   (1 << 9)   // Stop
#define I2C_CR1_ACK   (1 << 10)   // ACK

// MPU6050
#define MPU6050_ADDR  0x68
#define WHO_AM_I      0x75
#define PWR_MGMT_1    0x6B
#define ACCEL_XOUT_H  0x3B

// UART TX
static void uart_putc(char c) {
    while (!(USART1_SR & SR_TXE));
    USART1_DR = c;
}

static void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}

static void uart_print_hex(uint8_t v) {
    uint8_t hi = (v >> 4) & 0xF;
    uint8_t lo = v & 0xF;
    uart_putc(hi < 10 ? '0' + hi : 'A' + hi - 10);
    uart_putc(lo < 10 ? '0' + lo : 'A' + lo - 10);
}

// I2C функції
static void i2c_start(void) {
    I2C1_CR1 |= I2C_CR1_START;
    while (!(I2C1_SR1 & I2C_SR1_SB));
}

static void i2c_stop(void) {
    I2C1_CR1 |= I2C_CR1_STOP;
}

static void i2c_send_addr(uint8_t addr, uint8_t rw) {
    I2C1_DR = (addr << 1) | rw;
    while (!(I2C1_SR1 & I2C_SR1_ADDR));
    (void)I2C1_SR2;  // читаємо SR2 щоб скинути ADDR
}

static void i2c_send_byte(uint8_t data) {
    while (!(I2C1_SR1 & I2C_SR1_TXE));
    I2C1_DR = data;
    while (!(I2C1_SR1 & I2C_SR1_BTF));
}

static uint8_t i2c_read_byte_nack(void) {
    I2C1_CR1 &= ~I2C_CR1_ACK;  // NACK — останній байт
    i2c_stop();
    while (!(I2C1_SR1 & I2C_SR1_RXNE));
    return (uint8_t)I2C1_DR;
}

// записати в регістр MPU6050
static void mpu_write(uint8_t reg, uint8_t data) {
    i2c_start();
    i2c_send_addr(MPU6050_ADDR, 0);  // write
    i2c_send_byte(reg);
    i2c_send_byte(data);
    i2c_stop();
}

// читати один регістр MPU6050
static uint8_t mpu_read(uint8_t reg) {
    i2c_start();
    i2c_send_addr(MPU6050_ADDR, 0);  // write — вказуємо регістр
    i2c_send_byte(reg);
    i2c_start();                      // repeated start
    i2c_send_addr(MPU6050_ADDR, 1);  // read
    return i2c_read_byte_nack();
}

int main(void) {
    // тактування
    RCC_APB2ENR |= (1 << 2) | (1 << 3) | (1 << 14); // GPIOA, GPIOB, USART1
    RCC_APB1ENR |= (1 << 21);                          // I2C1

    // PA9 TX — AF push-pull 50MHz
    GPIOA_CRH &= ~(0xF << 4);
    GPIOA_CRH |=  (0xB << 4);

    // PB6 SCL, PB7 SDA — AF open-drain 50MHz = 0xF
    GPIOB_CRL &= ~(0xFF << 24);
    GPIOB_CRL |=  (0xFF << 24);  // 0xF для PB6 і PB7

    // UART 9600
    USART1_BRR = 8000000UL / 9600;
    USART1_CR1 = (1 << 13) | (1 << 3);  // UE + TE

    // I2C1: 100кГц, PCLK1=8MHz
    I2C1_CR2   = 8;          // FREQ = 8MHz
    I2C1_CCR   = 40;         // CCR = 8MHz/(2*100kHz) = 40
    I2C1_TRISE = 9;          // TRISE = (1000нс / 125нс) + 1 = 9
    I2C1_CR1   = I2C_CR1_PE; // увімкнути I2C

    uart_puts("I2C MPU6050\r\n");

    // читаємо WHO_AM_I — має бути 0x68
    uint8_t who = mpu_read(WHO_AM_I);
    uart_puts("WHO_AM_I: 0x");
    uart_print_hex(who);
    uart_puts("\r\n");

    if (who == 0x68) {
        uart_puts("MPU6050 OK!\r\n");

        // виводимо з sleep mode
        mpu_write(PWR_MGMT_1, 0x00);

        while (1) {
            // читаємо акселерометр X
            uint8_t hi = mpu_read(ACCEL_XOUT_H);
            uint8_t lo = mpu_read(ACCEL_XOUT_H + 1);
            int16_t ax = (int16_t)((hi << 8) | lo);

            uart_puts("AX: 0x");
            uart_print_hex(hi);
            uart_print_hex(lo);
            uart_puts("\r\n");

            // груба затримка
            for (volatile int i = 0; i < 500000; i++);
        }
    } else {
        uart_puts("MPU6050 not found!\r\n");
        while (1) {}
    }
}