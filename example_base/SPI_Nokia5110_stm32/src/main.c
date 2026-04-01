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

#define SPI_SR_TXE   (1 << 1)
#define SPI_SR_BSY   (1 << 7)

#define PIN_CE   4
#define PIN_DC   3
#define PIN_RST  2

#define CE_LOW    GPIOA_BSRR = (1 << (PIN_CE  + 16))
#define CE_HIGH   GPIOA_BSRR = (1 << PIN_CE)
#define DC_LOW    GPIOA_BSRR = (1 << (PIN_DC  + 16))
#define DC_HIGH   GPIOA_BSRR = (1 << PIN_DC)
#define RST_LOW   GPIOA_BSRR = (1 << (PIN_RST + 16))
#define RST_HIGH  GPIOA_BSRR = (1 << PIN_RST)

// шрифт 5x8
static const uint8_t font5x8[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // пробіл
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
};

static void delay(volatile uint32_t n) { while (n--); }

static void spi_send(uint8_t data) {
    while (!(SPI1_SR & SPI_SR_TXE));
    SPI1_DR = data;
    while (SPI1_SR & SPI_SR_BSY);
}

static void nokia_cmd(uint8_t cmd) {
    DC_LOW; CE_LOW;
    spi_send(cmd);
    CE_HIGH;
}

static void nokia_data(uint8_t data) {
    DC_HIGH; CE_LOW;
    spi_send(data);
    CE_HIGH;
}

static void nokia_clear(void) {
    nokia_cmd(0x40);
    nokia_cmd(0x80);
    for (int i = 0; i < 504; i++) nokia_data(0x00);
}

static void nokia_char(char c) {
    uint8_t idx = 0;
    if (c >= 'A' && c <= 'Z') idx = c - 'A' + 1;
    for (int i = 0; i < 5; i++) nokia_data(font5x8[idx][i]);
    nokia_data(0x00);
}

static void nokia_print(const char *s) {
    while (*s) nokia_char(*s++);
}

static void nokia_init(void) {
    RST_LOW; delay(10000);
    RST_HIGH; delay(10000);
    nokia_cmd(0x21);  // extended commands
    nokia_cmd(0xB8);  // contrast
    nokia_cmd(0x04);  // temp coefficient
    nokia_cmd(0x14);  // bias
    nokia_cmd(0x20);  // normal commands
    nokia_cmd(0x0C);  // display normal
}

int main(void) {
    RCC_APB2ENR |= (1 << 2) | (1 << 12);

    // PA2, PA3, PA4 — output 50MHz
    GPIOA_CRL &= ~(0xFFF << 8);
    GPIOA_CRL |=  (0x333 << 8);

    // PA5 SCK, PA7 MOSI — AF push-pull 50MHz
    GPIOA_CRL &= ~(0xF << 20);
    GPIOA_CRL |=  (0xB << 20);
    GPIOA_CRL &= ~(0xF << 28);
    GPIOA_CRL |=  (0xB << 28);

    // PA6 MISO — input floating
    GPIOA_CRL &= ~(0xF << 24);
    GPIOA_CRL |=  (0x4 << 24);

    // SPI1: master, fPCLK/8, SSM+SSI
    SPI1_CR1 = (1 << 2) | (1 << 9) | (1 << 8) | (2 << 3) | (1 << 6);

    nokia_init();
    nokia_clear();

    nokia_cmd(0x40);   // Y=0
    nokia_cmd(0x80);   // X=0
    nokia_print("STM32");

    nokia_cmd(0x41);   // Y=1
    nokia_cmd(0x80);   // X=0
    nokia_print("BEZ MAGII");

    while (1) {}
}