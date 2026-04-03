#include "../hal/hal.h"
#include "../hal/hal_ds1307.h"

static void print_two(uint8_t val) {
    if (val < 10) uart_putc('0');
    uart_printf("%d", (int32_t)val);
}

static const char *dow_names[] = {
    "", "Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Нд"
};

int main(void) {
    hal_init();
    uart_init(9600);
    i2c1_init();
    ds1307_init();

    uart_puts("DS1307 RTC test\r\n");

    // Встановлюємо час ОДИН РАЗ — потім закоментуй!
    ds1307_time_t set_t = {
        .sec   = 0,
        .min   = 30,
        .hour  = 16,
        .dow   = 4,   // Четвер
        .date  = 3,
        .month = 4,
        .year  = 26
    };
    ds1307_set(&set_t);
    uart_puts("Time set!\r\n");

    ds1307_time_t t;

    while (1) {
        ds1307_get(&t);

        print_two(t.hour); uart_putc(':');
        print_two(t.min);  uart_putc(':');
        print_two(t.sec);
        uart_puts("  ");
        uart_puts(dow_names[t.dow]);
        uart_putc(' ');
        print_two(t.date);  uart_putc('.');
        print_two(t.month); uart_puts(".20");
        print_two(t.year);
        uart_puts("\r\n");

        delay_ms(1000);
    }
}