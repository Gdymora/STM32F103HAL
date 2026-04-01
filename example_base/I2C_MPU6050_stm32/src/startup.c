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
void USART1_IRQHandler(void)    WEAK_ALIAS;

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
     // IRQ0-IRQ21 (позиції 16-37)
    (uint32_t)Default_Handler,   // IRQ0  WWDG
    (uint32_t)Default_Handler,   // IRQ1  PVD
    (uint32_t)Default_Handler,   // IRQ2  TAMPER
    (uint32_t)Default_Handler,   // IRQ3  RTC
    (uint32_t)Default_Handler,   // IRQ4  FLASH
    (uint32_t)Default_Handler,   // IRQ5  RCC
    (uint32_t)Default_Handler,   // IRQ6  EXTI0
    (uint32_t)Default_Handler,   // IRQ7  EXTI1
    (uint32_t)Default_Handler,   // IRQ8  EXTI2
    (uint32_t)Default_Handler,   // IRQ9  EXTI3
    (uint32_t)Default_Handler,   // IRQ10 EXTI4
    (uint32_t)Default_Handler,   // IRQ11 DMA1_Ch1
    (uint32_t)Default_Handler,   // IRQ12 DMA1_Ch2
    (uint32_t)Default_Handler,   // IRQ13 DMA1_Ch3
    (uint32_t)Default_Handler,   // IRQ14 DMA1_Ch4
    (uint32_t)Default_Handler,   // IRQ15 DMA1_Ch5
    (uint32_t)Default_Handler,   // IRQ16 DMA1_Ch6
    (uint32_t)Default_Handler,   // IRQ17 DMA1_Ch7
    (uint32_t)Default_Handler,   // IRQ18 ADC1_2
    (uint32_t)Default_Handler,   // IRQ19 USB_HP_CAN_TX
    (uint32_t)Default_Handler,   // IRQ20 USB_LP_CAN_RX0
    (uint32_t)Default_Handler,   // IRQ21 CAN_RX1
    (uint32_t)Default_Handler,   // IRQ22 CAN_SCE
    (uint32_t)Default_Handler,   // IRQ23 EXTI9_5
    (uint32_t)Default_Handler,   // IRQ24 TIM1_BRK
    (uint32_t)Default_Handler,   // IRQ25 TIM1_UP
    (uint32_t)Default_Handler,   // IRQ26 TIM1_TRG_COM
    (uint32_t)Default_Handler,   // IRQ27 TIM1_CC
    (uint32_t)Default_Handler,   // IRQ28 TIM2
    (uint32_t)Default_Handler,   // IRQ29 TIM3
    (uint32_t)Default_Handler,   // IRQ30 TIM4
    (uint32_t)Default_Handler,   // IRQ31 I2C1_EV
    (uint32_t)Default_Handler,   // IRQ32 I2C1_ER
    (uint32_t)Default_Handler,   // IRQ33 I2C2_EV
    (uint32_t)Default_Handler,   // IRQ34 I2C2_ER
    (uint32_t)Default_Handler,   // IRQ35 SPI1
    (uint32_t)Default_Handler,   // IRQ36 SPI2
    (uint32_t)USART1_IRQHandler, // IRQ37 USART1 ← ось тут!

};