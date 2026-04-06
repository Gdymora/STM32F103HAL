#include "../hal/hal.h"

/*
ST7735S    Blue Pill
─────────────────────
VCC   →    3.3V
GND   →    GND
SCK   →    PA5
SDA   →    PA7 (MOSI)
AO    →    PA3 (DC)
RESET →    PA2
CS    →    PA4
LED   →    3.3V (підсвітка)

*/

#define PIN_CS    PIN_PA4
#define PIN_DC    PIN_PA3
#define PIN_RST   PIN_PA2

// RGB565 кольори
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define ORANGE  0xFC00

static void delay_loop(volatile uint32_t n) { while(n--); }

static void tft_cmd(uint8_t cmd) {
    gpio_write(PIN_DC, LOW);
    gpio_write(PIN_CS, LOW);
    spi1_send(cmd);
    gpio_write(PIN_CS, HIGH);
}

static void tft_data(uint8_t data) {
    gpio_write(PIN_DC, HIGH);
    gpio_write(PIN_CS, LOW);
    spi1_send(data);
    gpio_write(PIN_CS, HIGH);
}

static void tft_set_window(uint8_t x0, uint8_t y0,
                            uint8_t x1, uint8_t y1) {
    tft_cmd(0x2A);
    tft_data(0x00); tft_data(x0 + 2);
    tft_data(0x00); tft_data(x1 + 2);

    tft_cmd(0x2B);
    tft_data(0x00); tft_data(y0 + 3);
    tft_data(0x00); tft_data(y1 + 3);

    tft_cmd(0x2C);
}

static void tft_init(void) {
    gpio_write(PIN_RST, LOW); delay_loop(50000);
    gpio_write(PIN_RST, HIGH); delay_loop(50000);
    tft_cmd(0x01); delay_loop(100000);
    tft_cmd(0x11); delay_loop(100000);
    tft_cmd(0x3A); tft_data(0x05);
    tft_cmd(0x36); tft_data(0xC8);
    tft_cmd(0x29); delay_loop(50000);
}

// заливка прямокутника
static void tft_rect(uint8_t x, uint8_t y,
                     uint8_t w, uint8_t h, uint16_t color) {
    tft_set_window(x, y, x + w - 1, y + h - 1);
    gpio_write(PIN_DC, HIGH);
    gpio_write(PIN_CS, LOW);
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    for (int i = 0; i < w * h; i++) {
        spi1_send(hi);
        spi1_send(lo);
    }
    gpio_write(PIN_CS, HIGH);
}

// весь екран одним кольором
static void tft_fill(uint16_t color) {
    tft_rect(0, 0, 128, 128, color);
}

// шрифт 5x8 — пробіл + A-Z + 0-9
static const uint8_t font5x8[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // ' '
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x3F,0x40,0x38,0x40,0x3F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x07,0x08,0x70,0x08,0x07}, // Y
    {0x61,0x51,0x49,0x45,0x43}, // Z
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
};

// отримати індекс символу в масиві шрифту
static int8_t font_index(char c) {
    if (c == ' ') return 0;
    if (c >= 'A' && c <= 'Z') return c - 'A' + 1;
    if (c >= '0' && c <= '9') return c - '0' + 27;
    return 0;  // невідомий → пробіл
}

// вивести символ 5x8 з масштабом
static void tft_char(uint8_t x, uint8_t y, char c,
                     uint16_t fg, uint16_t bg, uint8_t scale) {
    int8_t idx = font_index(c);
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = font5x8[idx][col];
        for (uint8_t row = 0; row < 8; row++) {
            uint16_t color = (line & (1 << row)) ? fg : bg;
            tft_rect(x + col * scale,
                     y + row * scale,
                     scale, scale, color);
        }
    }
    // пробіл між символами
    tft_rect(x + 5 * scale, y, scale, 8 * scale, bg);
}

// вивести рядок
static void tft_str(uint8_t x, uint8_t y, const char *s,
                    uint16_t fg, uint16_t bg, uint8_t scale) {
    while (*s) {
        tft_char(x, y, *s++, fg, bg, scale);
        x += 6 * scale;  // 5 пікселів + 1 пробіл
    }
}

// вивести число з двома цифрами (00-99)
static void tft_num2(uint8_t x, uint8_t y, uint8_t val,
                     uint16_t fg, uint16_t bg, uint8_t scale) {
    char buf[3];
    buf[0] = '0' + val / 10;
    buf[1] = '0' + val % 10;
    buf[2] = '\0';
    tft_str(x, y, buf, fg, bg, scale);
}

int main(void) {
    hal_init();

    gpio_init(PIN_PA2, OUTPUT_FAST);
    gpio_init(PIN_PA3, OUTPUT_FAST);
    gpio_init(PIN_PA4, OUTPUT_FAST);
    spi1_init();

    tft_init();

    // чорний фон
    tft_fill(BLACK);

    // ── Демо графіки ──────────────────────────────────────

    // кольорові смуги зверху
    tft_rect(0,   0, 128, 8, RED);
    tft_rect(0,   8, 128, 8, GREEN);
    tft_rect(0,  16, 128, 8, BLUE);
    tft_rect(0,  24, 128, 8, YELLOW);
    tft_rect(0,  32, 128, 8, CYAN);
    tft_rect(0,  40, 128, 8, MAGENTA);

    // ── Демо тексту ───────────────────────────────────────

    // великий текст scale=2 (10x16 пікселів на символ)
    tft_str(4, 52, "STM32", YELLOW, BLACK, 2);

    // середній текст scale=1
    tft_str(4, 72, "BEZ HAL", WHITE, BLACK, 1);
    tft_str(4, 82, "SPI TFT", CYAN,  BLACK, 1);

    // цифри — імітація годинника
    tft_str(4,  96, "TIME:", WHITE, BLACK, 1);
    tft_num2(40, 96, 16, YELLOW, BLACK, 1);
    tft_str(52,  96, ":", WHITE, BLACK, 1);
    tft_num2(58, 96, 30, YELLOW, BLACK, 1);
    tft_str(70,  96, ":", WHITE, BLACK, 1);
    tft_num2(76, 96, 45, YELLOW, BLACK, 1);

    // рамка внизу
    tft_rect(0,  110, 128, 2, WHITE);
    tft_str(4,  114, "DOU.UA", GREEN, BLACK, 1);

    while (1) {}
}