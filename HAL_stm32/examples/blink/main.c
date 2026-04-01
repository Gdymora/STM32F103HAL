#include "../hal/hal.h"

int main(void) {
    hal_init();

    gpio_init(PIN_PC13, OUTPUT);
    gpio_init(PIN_PA1,  INPUT_PULLUP);

   while (1) {
    gpio_toggle(PIN_PC13);
    delay_ms(500);
}
 

    return 0;
}

/* Button example 
  світлодіод до піна С13 катод (-) анод (+) до 3.3V.
*/