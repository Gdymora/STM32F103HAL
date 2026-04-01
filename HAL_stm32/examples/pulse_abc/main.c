#include "../hal/hal.h"

#define PULSE_CHANNEL 1
#define THRESHOLD 2100 // поріг піку — підбери під свій сенсор
/*
Підключи Pulse Sensor:

Pulse Sensor S  → PA1
Pulse Sensor +  → 3.3V
Pulse Sensor -  → GND 
*/

int main(void)
{
    hal_init();
    uart_init(9600);
    adc1_init(PULSE_CHANNEL);

    uart_puts("Pulse Sensor BPM\r\n");

    uint32_t last_beat = 0; // час останнього удару
    uint32_t bpm = 0;
    uint8_t above = 0;  // чи вище порогу зараз
    uint8_t warmup = 5; // пропускаємо перші 5 ударів

    while (1)
    {
        uint16_t val = adc1_read();
        uint32_t now = get_ticks(); // мс від старту

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
                    { // 30-200 BPM
                        bpm = 60000 / interval;
                        uart_printf("BPM: %d\r\n", bpm);
                    }
                }
            }
            last_beat = now;
        }

        if (val < THRESHOLD)
        {
            above = 0;
        }

        delay_ms(10);
    }
}