#include "stm32g0xx_hal.h"
#include "main.h"  // LED4_* a extern htim17

void NMI_Handler(void) {}
void HardFault_Handler(void) { while (1) {} }
void SVC_Handler(void) {}
void PendSV_Handler(void) {}
void SysTick_Handler(void) { HAL_IncTick(); }

void TIM17_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim17);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM17) {
        HAL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
    }
}
