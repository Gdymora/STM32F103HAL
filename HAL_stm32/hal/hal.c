#include "hal.h"

void hal_init(void) {
    systick_init(CPU_HZ);
}