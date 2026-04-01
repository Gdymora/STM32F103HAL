#include "../hal/hal.h"

int main(void) {
    hal_init();

    gpio_init(PIN_PC13, OUTPUT);
    gpio_init(PIN_PA1,  INPUT_PULLUP);

    while (1) {
    if (!gpio_read(PIN_PA1)) {
        gpio_write(PIN_PC13, LOW);   // натиснуто = LED світиться
    } else {
        gpio_write(PIN_PC13, HIGH);  // відпущено = LED не світиться
    }
}

    return 0;
}

/* Button example 
  Кнопка до піна А1' + анодом, катод - до GND, світлодіод до піна С13 катод (-) анод (+) до 3.3V.
  Коли кнопка натиснута, світлодіод світиться, 
  коли відпущена - не світиться.
*/