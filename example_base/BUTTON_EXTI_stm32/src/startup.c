#include <stdint.h>

// оголошені в лінкер скрипті
extern uint32_t _estack;
extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss, _ebss;

// наш main
extern int main(void);

void Reset_Handler(void) {
    // копіюємо .data з Flash в SRAM
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // обнуляємо .bss
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    main();

    // якщо main повернувся — висимо тут
    while (1);
}

// дефолтний handler для всіх переривань
void Default_Handler(void) {
    while (1);
}

// аліаси — всі невикористані переривання йдуть в Default_Handler
#define WEAK_ALIAS __attribute__((weak, alias("Default_Handler")))

void NMI_Handler(void)        WEAK_ALIAS;
void HardFault_Handler(void)  WEAK_ALIAS;
void SVC_Handler(void)        WEAK_ALIAS;
void PendSV_Handler(void)     WEAK_ALIAS;
void SysTick_Handler(void)    WEAK_ALIAS;
void WWDG_IRQHandler(void)      WEAK_ALIAS;
void PVD_IRQHandler(void)       WEAK_ALIAS;
void TAMPER_IRQHandler(void)    WEAK_ALIAS;
void RTC_IRQHandler(void)       WEAK_ALIAS;
void FLASH_IRQHandler(void)     WEAK_ALIAS;
void RCC_IRQHandler(void)       WEAK_ALIAS;
void EXTI0_IRQHandler(void)     WEAK_ALIAS;
void EXTI1_IRQHandler(void)     WEAK_ALIAS; // Твоя функція з main.c підставиться сюди

// таблиця векторів — лягає в самий початок Flash
__attribute__((section(".vectors")))
uint32_t vectors[] = {
    (uint32_t)&_estack,          // 0: початковий стек (top of SRAM)
    (uint32_t)Reset_Handler,     // 1: reset
    (uint32_t)NMI_Handler,       // 2: NMI
    (uint32_t)HardFault_Handler, // 3: hard fault
    0, 0, 0, 0, 0, 0, 0,        // 4-10: reserved
    (uint32_t)SVC_Handler,       // 11: SVCall
    0, 0,                        // 12-13: reserved
    (uint32_t)PendSV_Handler,    // 14: PendSV
    (uint32_t)SysTick_Handler,   // 15: SysTick
    // --- ПЕРИФЕРІЙНІ ПЕРЕРИВАННЯ ---
    (uint32_t)WWDG_IRQHandler,      // 16 (IRQ 0)
    (uint32_t)PVD_IRQHandler,       // 17 (IRQ 1)
    (uint32_t)TAMPER_IRQHandler,    // 18 (IRQ 2)
    (uint32_t)RTC_IRQHandler,       // 19 (IRQ 3)
    (uint32_t)FLASH_IRQHandler,     // 20 (IRQ 4)
    (uint32_t)RCC_IRQHandler,       // 21 (IRQ 5)
    (uint32_t)EXTI0_IRQHandler,     // 22 (IRQ 6)
    (uint32_t)EXTI1_IRQHandler,     // 23 (IRQ 7) <--- ОСЬ ТУТ МАГІЯ!
};