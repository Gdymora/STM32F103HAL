#include "../hal/hal.h"

// Nokia піни
#define PIN_CE PIN_PA4
#define PIN_DC PIN_PA3
#define PIN_RST PIN_PA2

#define PULSE_CHANNEL 1
#define THRESHOLD 2200
/*
Підключи Pulse Sensor:

Pulse Sensor S  → PA1
Pulse Sensor +  → 3.3V
Pulse Sensor -  → GND 
*/

// мінімальний шрифт для цифр 0-9
static const uint8_t font5x8[][5] = {
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
};

static void delay_loop(volatile uint32_t n)
{
    while (n--)
        ;
}

static void nokia_cmd(uint8_t cmd)
{
    gpio_write(PIN_DC, LOW);
    gpio_write(PIN_CE, LOW);
    spi1_send(cmd);
    gpio_write(PIN_CE, HIGH);
}

static void nokia_data(uint8_t data)
{
    gpio_write(PIN_DC, HIGH);
    gpio_write(PIN_CE, LOW);
    spi1_send(data);
    gpio_write(PIN_CE, HIGH);
}

static void nokia_clear(void)
{
    nokia_cmd(0x40);
    nokia_cmd(0x80);
    for (int i = 0; i < 504; i++)
        nokia_data(0x00);
}

static void nokia_init(void)
{
    gpio_write(PIN_RST, LOW);
    delay_loop(10000);
    gpio_write(PIN_RST, HIGH);
    delay_loop(10000);
    nokia_cmd(0x21);
    nokia_cmd(0xB8);
    nokia_cmd(0x04);
    nokia_cmd(0x14);
    nokia_cmd(0x20);
    nokia_cmd(0x0C);
}

static void nokia_char(uint8_t c)
{
    for (int i = 0; i < 5; i++)
        nokia_data(font5x8[c][i]);
    nokia_data(0x00);
}

static void nokia_print_bpm(uint32_t bpm)
{
    nokia_cmd(0x41);      // рядок 1
    nokia_cmd(0x80 | 20); // колонка 20 — по центру
    nokia_char(bpm / 100);
    nokia_char((bpm / 10) % 10);
    nokia_char(bpm % 10);
}

static void nokia_print_label(void)
{
    // статичний рядок "BPM" зверху
    static const uint8_t font[][5] = {
        {0x7F, 0x49, 0x49, 0x49, 0x36}, // B  ← був P
        {0x7F, 0x09, 0x09, 0x09, 0x06}, // P  ← був B
        {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    };
    nokia_cmd(0x40);
    nokia_cmd(0x80 | 30);
    for (int c = 0; c < 3; c++)
    {
        for (int i = 0; i < 5; i++)
            nokia_data(font[c][i]);
        nokia_data(0x00);
    }
}

int main(void)
{
    hal_init();
    uart_init(9600);
    adc1_init(PULSE_CHANNEL);

    // Nokia ініціалізація
    gpio_init(PIN_PA2, OUTPUT_FAST);
    gpio_init(PIN_PA3, OUTPUT_FAST);
    gpio_init(PIN_PA4, OUTPUT_FAST);
    spi1_init();
    nokia_init();
    nokia_clear();
    nokia_print_label();

    uart_puts("Pulse + Display\r\n");

    uint32_t last_beat = 0;
    uint32_t bpm = 0;
    uint8_t above = 0;
    uint8_t warmup = 5;

    while (1)
    {
        uint16_t val = adc1_read();
        uint32_t now = get_ticks();

        if (val > THRESHOLD && !above)
        {
            above = 1;
            if (last_beat > 0)
            {
                if (warmup > 0)
                {
                    warmup--;
                }
                else
                {
                    uint32_t interval = now - last_beat;
                    if (interval > 300 && interval < 2000)
                    {
                        bpm = 60000 / interval;
                        uart_printf("BPM: %d\r\n", bpm);
                        nokia_print_bpm(bpm); // оновлюємо екран
                    }
                }
            }
            last_beat = now;
        }

        if (val < THRESHOLD)
            above = 0;

        delay_ms(10);
    }
}