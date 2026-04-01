#include "../hal/hal.h"

/* 
**SSD1306 OLED** 128x64 — I2C або SPI дисплей.
Це SPI версія SSD1306. Підключення до Blue Pill:
```
D0  → PA5 (SCK)
D1  → PA7 (MOSI)
RES → PA2
DC  → PA3
CS  → PA4
VCC → 3.3V
GND → GND
```
Ті самі піни що Nokia 5110! Тільки протокол ініціалізації інший.
*/
#define PIN_CS   PIN_PA4
#define PIN_DC   PIN_PA3
#define PIN_RST  PIN_PA2

#define PULSE_CHANNEL 1
#define THRESHOLD 2200

static void delay_loop(volatile uint32_t n) { while(n--); }

static void oled_cmd(uint8_t cmd) {
    gpio_write(PIN_DC, LOW);
    gpio_write(PIN_CS, LOW);
    spi1_send(cmd);
    gpio_write(PIN_CS, HIGH);
}

static void oled_data(uint8_t data) {
    gpio_write(PIN_DC, HIGH);
    gpio_write(PIN_CS, LOW);
    spi1_send(data);
    gpio_write(PIN_CS, HIGH);
}

static void oled_init(void) {
    gpio_write(PIN_RST, LOW); delay_loop(10000);
    gpio_write(PIN_RST, HIGH); delay_loop(10000);
    oled_cmd(0xAE);
    oled_cmd(0xD5); oled_cmd(0x80);
    oled_cmd(0xA8); oled_cmd(0x3F);
    oled_cmd(0xD3); oled_cmd(0x00);
    oled_cmd(0x40);
    oled_cmd(0x8D); oled_cmd(0x14);
    oled_cmd(0x20); oled_cmd(0x00);
    oled_cmd(0xA1);
    oled_cmd(0xC8);
    oled_cmd(0xDA); oled_cmd(0x12);
    oled_cmd(0x81); oled_cmd(0xFF);
    oled_cmd(0xD9); oled_cmd(0xF1);
    oled_cmd(0xDB); oled_cmd(0x40);
    oled_cmd(0xA4);
    oled_cmd(0xA6);
    oled_cmd(0xAF);
}

// записати в конкретну page і колонку
static void oled_set_pos(uint8_t page, uint8_t col) {
    oled_cmd(0x22); oled_cmd(page); oled_cmd(page);
    oled_cmd(0x21); oled_cmd(col); oled_cmd(127);
}

// шрифт 5x8 цифри 0-9
static const uint8_t digits[][5] = {
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

// шрифт великий 10x16 (два рядки)
static const uint8_t big_digits_top[][10] = {
    {0xFF,0x01,0x01,0x01,0xFF,0xFF,0x01,0x01,0x01,0xFF}, // 0
    {0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00}, // 1
    {0x83,0x41,0x41,0x41,0xFF,0xFF,0x01,0x01,0x01,0x01}, // 2
    {0x41,0x41,0x41,0x41,0xFF,0xFF,0x41,0x41,0x41,0xFF}, // 3
    {0xFF,0x40,0x40,0x40,0xFF,0xFF,0x40,0x40,0x40,0x00}, // 4
    {0xFF,0x41,0x41,0x41,0xC1,0xC1,0x41,0x41,0x41,0xFF}, // 5
    {0xFF,0x41,0x41,0x41,0xC1,0xFF,0x41,0x41,0x41,0xFF}, // 6
    {0x01,0x01,0x01,0x01,0xFF,0xFF,0x01,0x01,0x01,0x00}, // 7
    {0xFF,0x41,0x41,0x41,0xFF,0xFF,0x41,0x41,0x41,0xFF}, // 8
    {0xFF,0x41,0x41,0x41,0xFF,0xFF,0x41,0x41,0x41,0xFF}, // 9
};

static const uint8_t big_digits_bot[][10] = {
    {0xFF,0x80,0x80,0x80,0xFF,0xFF,0x80,0x80,0x80,0xFF}, // 0
    {0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00}, // 1
    {0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xC3}, // 2
    {0x80,0x80,0x80,0x80,0xFF,0xFF,0x80,0x80,0x80,0xFF}, // 3
    {0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00}, // 4
    {0xC1,0x80,0x80,0x80,0xFF,0xFF,0x80,0x80,0x80,0xFF}, // 5
    {0xFF,0x80,0x80,0x80,0xFF,0xFF,0x80,0x80,0x80,0xFF}, // 6
    {0x80,0x80,0x80,0x80,0xFF,0xFF,0x80,0x80,0x80,0x00}, // 7
    {0xFF,0x80,0x80,0x80,0xFF,0xFF,0x80,0x80,0x80,0xFF}, // 8
    {0xFF,0x80,0x80,0x80,0xFF,0xFF,0x80,0x80,0x80,0xFF}, // 9
};

static void oled_big_digit(uint8_t page, uint8_t col, uint8_t d) {
    // верхня половина — нижні 4 біти кожного байта
    oled_set_pos(page, col);
    for (int i = 0; i < 5; i++) {
        uint8_t b = digits[d][i];
        // розтягуємо біти: кожен біт → два біти
        uint8_t top = 0;
        for (int bit = 0; bit < 4; bit++)
            if (b & (1 << bit)) top |= (3 << (bit * 2));
        oled_data(top); oled_data(top);
    }

    // нижня половина — верхні 4 біти
    oled_set_pos(page + 1, col);
    for (int i = 0; i < 5; i++) {
        uint8_t b = digits[d][i];
        uint8_t bot = 0;
        for (int bit = 0; bit < 4; bit++)
            if (b & (1 << (bit + 4))) bot |= (3 << (bit * 2));
        oled_data(bot); oled_data(bot);
    }
}

static void oled_print_bpm(uint32_t bpm) {
    if (bpm > 999) bpm = 999;
    oled_big_digit(3, 20,  bpm / 100);
    oled_big_digit(3, 54,  (bpm / 10) % 10);
    oled_big_digit(3, 88,  bpm % 10);
}

// пульсуючий індикатор у жовтій зоні (page 0)
static uint8_t pulse_state = 0;

static void oled_pulse_indicator(void) {
    oled_set_pos(0, 0);
    // серце — просто заповнюємо або очищаємо жовту смугу
    uint8_t val = pulse_state ? 0xFF : 0x00;
    for (int i = 0; i < 128; i++) oled_data(val);
    pulse_state ^= 1;
}

// підпис BPM
static void oled_label(void) {
    static const uint8_t font[][5] = {
        {0x7F,0x49,0x49,0x49,0x36}, // B
        {0x7F,0x09,0x09,0x09,0x06}, // P
        {0x7F,0x02,0x0C,0x02,0x7F}, // M
    };
    oled_set_pos(2, 46);
    for (int c = 0; c < 3; c++) {
        for (int i = 0; i < 5; i++) oled_data(font[c][i]);
        oled_data(0x00);
    }
}

// очистити синю зону (page 2-7)
static void oled_clear_blue(void) {
    oled_cmd(0x22); oled_cmd(2); oled_cmd(7);
    oled_cmd(0x21); oled_cmd(0); oled_cmd(127);
    for (int i = 0; i < 768; i++) oled_data(0x00);
}

int main(void) {
    hal_init();
    uart_init(9600);
    adc1_init(PULSE_CHANNEL);

    gpio_init(PIN_PA2, OUTPUT_FAST);
    gpio_init(PIN_PA3, OUTPUT_FAST);
    gpio_init(PIN_PA4, OUTPUT_FAST);
    spi1_init();

    oled_init();
    oled_clear_blue();
    oled_label();
    oled_print_bpm(0);

    uint32_t last_beat = 0;
    uint32_t bpm       = 0;
    uint8_t  above     = 0;
    uint8_t  warmup    = 5;

    while (1) {
        uint16_t val = adc1_read();
        uint32_t now = get_ticks();

        if (val > THRESHOLD && !above) {
            above = 1;
            oled_pulse_indicator();  // миготить жовта смуга

            if (last_beat > 0) {
                if (warmup > 0) {
                    warmup--;
                } else {
                    uint32_t interval = now - last_beat;
                    if (interval > 300 && interval < 2000) {
                        bpm = 60000 / interval;
                        uart_printf("BPM: %d\r\n", bpm);
                        oled_print_bpm(bpm);
                    }
                }
            }
            last_beat = now;
        }

        if (val < THRESHOLD) above = 0;
        delay_ms(10);
    }
}