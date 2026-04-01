#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss, _ebss;
extern int main(void);

void Reset_Handler(void);
void Default_Handler(void);

#define WEAK_ALIAS __attribute__((weak, alias("Default_Handler")))

// системні
void NMI_Handler(void)        WEAK_ALIAS;
void HardFault_Handler(void)  WEAK_ALIAS;
void SVC_Handler(void)        WEAK_ALIAS;
void PendSV_Handler(void)     WEAK_ALIAS;
void SysTick_Handler(void)    WEAK_ALIAS;

// всі IRQ з Table 63 RM0008 — STM32F10xxx
void WWDG_IRQHandler(void)           WEAK_ALIAS; // IRQ0
void PVD_IRQHandler(void)            WEAK_ALIAS; // IRQ1
void TAMPER_IRQHandler(void)         WEAK_ALIAS; // IRQ2
void RTC_IRQHandler(void)            WEAK_ALIAS; // IRQ3
void FLASH_IRQHandler(void)          WEAK_ALIAS; // IRQ4
void RCC_IRQHandler(void)            WEAK_ALIAS; // IRQ5
void EXTI0_IRQHandler(void)          WEAK_ALIAS; // IRQ6
void EXTI1_IRQHandler(void)          WEAK_ALIAS; // IRQ7
void EXTI2_IRQHandler(void)          WEAK_ALIAS; // IRQ8
void EXTI3_IRQHandler(void)          WEAK_ALIAS; // IRQ9
void EXTI4_IRQHandler(void)          WEAK_ALIAS; // IRQ10
void DMA1_Channel1_IRQHandler(void)  WEAK_ALIAS; // IRQ11
void DMA1_Channel2_IRQHandler(void)  WEAK_ALIAS; // IRQ12
void DMA1_Channel3_IRQHandler(void)  WEAK_ALIAS; // IRQ13
void DMA1_Channel4_IRQHandler(void)  WEAK_ALIAS; // IRQ14
void DMA1_Channel5_IRQHandler(void)  WEAK_ALIAS; // IRQ15
void DMA1_Channel6_IRQHandler(void)  WEAK_ALIAS; // IRQ16
void DMA1_Channel7_IRQHandler(void)  WEAK_ALIAS; // IRQ17
void ADC1_2_IRQHandler(void)         WEAK_ALIAS; // IRQ18
void USB_HP_CAN_TX_IRQHandler(void)  WEAK_ALIAS; // IRQ19
void USB_LP_CAN_RX0_IRQHandler(void) WEAK_ALIAS; // IRQ20
void CAN_RX1_IRQHandler(void)        WEAK_ALIAS; // IRQ21
void CAN_SCE_IRQHandler(void)        WEAK_ALIAS; // IRQ22
void EXTI9_5_IRQHandler(void)        WEAK_ALIAS; // IRQ23
void TIM1_BRK_IRQHandler(void)       WEAK_ALIAS; // IRQ24
void TIM1_UP_IRQHandler(void)        WEAK_ALIAS; // IRQ25
void TIM1_TRG_COM_IRQHandler(void)   WEAK_ALIAS; // IRQ26
void TIM1_CC_IRQHandler(void)        WEAK_ALIAS; // IRQ27
void TIM2_IRQHandler(void)           WEAK_ALIAS; // IRQ28
void TIM3_IRQHandler(void)           WEAK_ALIAS; // IRQ29
void TIM4_IRQHandler(void)           WEAK_ALIAS; // IRQ30
void I2C1_EV_IRQHandler(void)        WEAK_ALIAS; // IRQ31
void I2C1_ER_IRQHandler(void)        WEAK_ALIAS; // IRQ32
void I2C2_EV_IRQHandler(void)        WEAK_ALIAS; // IRQ33
void I2C2_ER_IRQHandler(void)        WEAK_ALIAS; // IRQ34
void SPI1_IRQHandler(void)           WEAK_ALIAS; // IRQ35
void SPI2_IRQHandler(void)           WEAK_ALIAS; // IRQ36
void USART1_IRQHandler(void)         WEAK_ALIAS; // IRQ37
void USART2_IRQHandler(void)         WEAK_ALIAS; // IRQ38
void USART3_IRQHandler(void)         WEAK_ALIAS; // IRQ39
void EXTI15_10_IRQHandler(void)      WEAK_ALIAS; // IRQ40
void RTCAlarm_IRQHandler(void)       WEAK_ALIAS; // IRQ41
void USBWakeUp_IRQHandler(void)      WEAK_ALIAS; // IRQ42

void Reset_Handler(void) {
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;
    dst = &_sbss;
    while (dst < &_ebss) *dst++ = 0;
    main();
    while (1);
}

void Default_Handler(void) { while (1); }

__attribute__((section(".vectors")))
uint32_t vectors[] = {
    (uint32_t)&_estack,
    (uint32_t)Reset_Handler,
    (uint32_t)NMI_Handler,
    (uint32_t)HardFault_Handler,
    0, 0, 0, 0, 0, 0, 0,
    (uint32_t)SVC_Handler,
    0, 0,
    (uint32_t)PendSV_Handler,
    (uint32_t)SysTick_Handler,
    (uint32_t)WWDG_IRQHandler,           // IRQ0
    (uint32_t)PVD_IRQHandler,            // IRQ1
    (uint32_t)TAMPER_IRQHandler,         // IRQ2
    (uint32_t)RTC_IRQHandler,            // IRQ3
    (uint32_t)FLASH_IRQHandler,          // IRQ4
    (uint32_t)RCC_IRQHandler,            // IRQ5
    (uint32_t)EXTI0_IRQHandler,          // IRQ6
    (uint32_t)EXTI1_IRQHandler,          // IRQ7
    (uint32_t)EXTI2_IRQHandler,          // IRQ8
    (uint32_t)EXTI3_IRQHandler,          // IRQ9
    (uint32_t)EXTI4_IRQHandler,          // IRQ10
    (uint32_t)DMA1_Channel1_IRQHandler,  // IRQ11
    (uint32_t)DMA1_Channel2_IRQHandler,  // IRQ12
    (uint32_t)DMA1_Channel3_IRQHandler,  // IRQ13
    (uint32_t)DMA1_Channel4_IRQHandler,  // IRQ14
    (uint32_t)DMA1_Channel5_IRQHandler,  // IRQ15
    (uint32_t)DMA1_Channel6_IRQHandler,  // IRQ16
    (uint32_t)DMA1_Channel7_IRQHandler,  // IRQ17
    (uint32_t)ADC1_2_IRQHandler,         // IRQ18
    (uint32_t)USB_HP_CAN_TX_IRQHandler,  // IRQ19
    (uint32_t)USB_LP_CAN_RX0_IRQHandler, // IRQ20
    (uint32_t)CAN_RX1_IRQHandler,        // IRQ21
    (uint32_t)CAN_SCE_IRQHandler,        // IRQ22
    (uint32_t)EXTI9_5_IRQHandler,        // IRQ23
    (uint32_t)TIM1_BRK_IRQHandler,       // IRQ24
    (uint32_t)TIM1_UP_IRQHandler,        // IRQ25
    (uint32_t)TIM1_TRG_COM_IRQHandler,   // IRQ26
    (uint32_t)TIM1_CC_IRQHandler,        // IRQ27
    (uint32_t)TIM2_IRQHandler,           // IRQ28
    (uint32_t)TIM3_IRQHandler,           // IRQ29
    (uint32_t)TIM4_IRQHandler,           // IRQ30
    (uint32_t)I2C1_EV_IRQHandler,        // IRQ31
    (uint32_t)I2C1_ER_IRQHandler,        // IRQ32
    (uint32_t)I2C2_EV_IRQHandler,        // IRQ33
    (uint32_t)I2C2_ER_IRQHandler,        // IRQ34
    (uint32_t)SPI1_IRQHandler,           // IRQ35
    (uint32_t)SPI2_IRQHandler,           // IRQ36
    (uint32_t)USART1_IRQHandler,         // IRQ37
    (uint32_t)USART2_IRQHandler,         // IRQ38
    (uint32_t)USART3_IRQHandler,         // IRQ39
    (uint32_t)EXTI15_10_IRQHandler,      // IRQ40
    (uint32_t)RTCAlarm_IRQHandler,       // IRQ41
    (uint32_t)USBWakeUp_IRQHandler,      // IRQ42
};
