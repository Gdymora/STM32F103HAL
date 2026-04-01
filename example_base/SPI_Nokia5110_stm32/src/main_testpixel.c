// main.c — SPI + Nokia 5110 bare-metal
#include <stdint.h>

// RCC
#define RCC_BASE     0x40021000
#define RCC_APB2ENR  (*(volatile uint32_t *)(RCC_BASE + 0x18))

// GPIOA
#define GPIOA_BASE   0x40010800
#define GPIOA_CRL    (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_BSRR   (*(volatile uint32_t *)(GPIOA_BASE + 0x10))

// SPI1
#define SPI1_BASE    0x40013000
#define SPI1_CR1     (*(volatile uint32_t *)(SPI1_BASE + 0x00))
#define SPI1_SR      (*(volatile uint32_t *)(SPI1_BASE + 0x08))
#define SPI1_DR      (*(volatile uint32_t *)(SPI1_BASE + 0x0C))

#define SPI_SR_TXE   (1 << 1)  // TX buffer порожній
#define SPI_SR_BSY   (1 << 7)  // SPI зайнятий

// керуючі піни
#define PIN_CE   4   // PA4
#define PIN_DC   3   // PA3
#define PIN_RST  2   // PA2

#define CE_LOW    GPIOA_BSRR = (1 << (PIN_CE  + 16))
#define CE_HIGH   GPIOA_BSRR = (1 << PIN_CE)
#define DC_LOW    GPIOA_BSRR = (1 << (PIN_DC  + 16))
#define DC_HIGH   GPIOA_BSRR = (1 << PIN_DC)
#define RST_LOW   GPIOA_BSRR = (1 << (PIN_RST + 16))
#define RST_HIGH  GPIOA_BSRR = (1 << PIN_RST)

static void delay(volatile uint32_t n) {
    while (n--);
}

static void spi_send(uint8_t data) {
    while (!(SPI1_SR & SPI_SR_TXE));  // чекаємо поки TX вільний
    SPI1_DR = data;
    while (SPI1_SR & SPI_SR_BSY);     // чекаємо завершення
}

static void nokia_cmd(uint8_t cmd) {
    DC_LOW;
    CE_LOW;
    spi_send(cmd);
    CE_HIGH;
}

static void nokia_data(uint8_t data) {
    DC_HIGH;
    CE_LOW;
    spi_send(data);
    CE_HIGH;
}

static void nokia_init(void) {
    RST_LOW;
    delay(10000);
    RST_HIGH;
    delay(10000);

    nokia_cmd(0x21);  // extended commands
    nokia_cmd(0xB8);  // Vop (contrast)
    nokia_cmd(0x04);  // temp coefficient
    nokia_cmd(0x14);  // bias
    nokia_cmd(0x20);  // normal commands
    nokia_cmd(0x0C);  // display normal
}

int main(void) {
    // тактування GPIOA і SPI1
    RCC_APB2ENR |= (1 << 2) | (1 << 12);  // GPIOA + SPI1

    // PA2, PA3, PA4 — output push-pull 50MHz (керуючі піни)
    GPIOA_CRL &= ~(0xFFF << 8);
    GPIOA_CRL |=  (0x333 << 8);  // 0x3 = output 50MHz для PA2,PA3,PA4

    // PA5 SCK, PA7 MOSI — AF push-pull 50MHz = 0xB
    GPIOA_CRL &= ~(0xF << 20);
    GPIOA_CRL |=  (0xB << 20);   // PA5
    GPIOA_CRL &= ~(0xF << 28);
    GPIOA_CRL |=  (0xB << 28);   // PA7

    // PA6 MISO — input floating = 0x4
    GPIOA_CRL &= ~(0xF << 24);
    GPIOA_CRL |=  (0x4 << 24);

    // SPI1: master, fPCLK/8, CPOL=0, CPHA=0, MSB first
    SPI1_CR1 = (1 << 2)  |  // MSTR — master mode
               (1 << 9)  |  // SSM  — software NSS
               (1 << 8)  |  // SSI  — NSS high
               (2 << 3)  |  // BR   — fPCLK/8
               (1 << 6);    // SPE  — SPI enable

    nokia_init();

    // заповнюємо екран — всі пікселі увімкнені
    for (int i = 0; i < 504; i++) {  // 84x48 / 8 = 504 байти
        nokia_data(0xFF);
    }

    while (1) {}
}