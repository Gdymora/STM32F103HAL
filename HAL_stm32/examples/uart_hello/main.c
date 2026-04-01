#include "../hal/hal.h"

/* 
   Blue Pill PA9  → RX на адаптері
   Blue Pill PA10 → TX на адаптері
   Blue Pill GND  → GND на адаптері
   minicom -b 9600 -D /dev/ttyUSB0 

 */

#include "../hal/hal.h"

int main(void)
{
    hal_init();
    uart_init(9600);
    gpio_init(PIN_PC13, OUTPUT);

    uart_puts("STM32 HAL ready\r\n");
    uart_printf("hello %s, num=%d, hex=%x\r\n", "stm32", 42, 0xDEAD);
    char buf[16];

    while (1)
    {
        uart_gets(buf, 16);

        if (buf[0] == 'o' && buf[1] == 'n')
        {
            gpio_write(PIN_PC13, LOW); // інверсна логіка
            uart_puts("LED on\r\n");
        }
        else if (buf[0] == 'o' && buf[1] == 'f' && buf[2] == 'f')
        {
            gpio_write(PIN_PC13, HIGH);
            uart_puts("LED off\r\n");
        }
        else
        {
            uart_puts("unknown: ");
            uart_puts(buf);
            uart_puts("\r\n");
        }
    }
}