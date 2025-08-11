// boards/nucleo-g070rb/stm32g0xx_it.c
#include "stm32g0xx_hal.h"

/* Minimal exception handlers */
void NMI_Handler(void) {}
void HardFault_Handler(void) { while (1) {} }
void SVC_Handler(void) {}
void PendSV_Handler(void) {}

/* This drives HAL_Delay() time base (1 ms) */
void SysTick_Handler(void)
{
    HAL_IncTick();
    // For some HAL versions you might also add:
    // HAL_SYSTICK_IRQHandler();
}

/* If you later use the user button on PC13 (EXTI13) */
void EXTI4_15_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}
