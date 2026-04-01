#include "../hal/hal.h"

#define MPU6050_ADDR  0x68
#define WHO_AM_I      0x75
#define PWR_MGMT_1    0x6B
#define ACCEL_XOUT_H  0x3B

int main(void) {
    hal_init();
    uart_init(9600);
    i2c1_init();

    uart_puts("I2C HAL test\r\n");

    uint8_t who = i2c1_read(MPU6050_ADDR, WHO_AM_I);
    uart_puts("WHO_AM_I: 0x");
    // виводимо hex
    uint8_t hi = (who >> 4) & 0xF;
    uint8_t lo = who & 0xF;
    uart_putc(hi < 10 ? '0' + hi : 'A' + hi - 10);
    uart_putc(lo < 10 ? '0' + lo : 'A' + lo - 10);
    uart_puts("\r\n");

    // виводимо з sleep
    i2c1_write(MPU6050_ADDR, PWR_MGMT_1, 0x00);

    uint8_t buf[6];
    while (1) {
        i2c1_read_buf(MPU6050_ADDR, ACCEL_XOUT_H, buf, 6);
        int16_t ax = (int16_t)((buf[0] << 8) | buf[1]);
        int16_t ay = (int16_t)((buf[2] << 8) | buf[3]);
        int16_t az = (int16_t)((buf[4] << 8) | buf[5]);

        uart_printf("AX=%d AY=%d AZ=%d\r\n", ax, ay, az);
        delay_ms(500);
    }
}