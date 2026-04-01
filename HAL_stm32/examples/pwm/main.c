#include "../hal/hal.h" 
int main(void) {
    hal_init();
    gpio_init(PIN_PA0, OUTPUT_AF_FAST);  // ← явно видно який пін
    tim2_init_pwm(1000, 999);

    while (1) {
        for (int i = 0; i <= 999; i++) {
            tim2_pwm_set(i);
            delay_ms(1);
        }
        for (int i = 999; i >= 0; i--) {
            tim2_pwm_set(i);
            delay_ms(1);
        }
    }
}

/* PWM — широтно-імпульсна модуляція, керування яскравістю світлодіода або швидкістю двигуна
   TIM2_CH1 на PA0, альтернативна функція push-pull, швидкість 50MHz
   частота PWM = 8MHz / (999 + 1) = 8kHz, період 125мкс
   duty cycle від 0% (0) до 100% (999) з кроком 0.1%
*/