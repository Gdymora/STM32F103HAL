#include "../hal/hal.h"
/* Кнопка на піні A1 світлодіод на 13 */
// ISR — поза main!
void EXTI1_IRQHandler(void) {
    if (EXTI_PR & (1 << 1)) {
        EXTI_PR = (1 << 1);
        gpio_toggle(PIN_PC13);
    }
}

int main(void) {
    hal_init();
    gpio_init(PIN_PC13, OUTPUT);
    exti_init(PIN_PA1, EXTI_FALLING);

    while (1) {
        // нічого — вся логіка в ISR
    }
}

/* Button example кнопка з використанням переривання EXTI.
  Кнопка до піна А1' + анодом, катод - до GND, світлодіод до піна С13 катод (-) анод (+) до 3.3V.
  Коли кнопка натиснута, світлодіод світиться, 
  коли відпущена - не світиться.
*/