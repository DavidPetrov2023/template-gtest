#include "stm32g0xx_hal.h"
#include "tx_api.h"
#include "tx_port.h"              // pro _tx_timer_interrupt()
#include "main.h"
extern volatile UINT led_flags_ready;

extern TIM_HandleTypeDef htim17;
extern TX_EVENT_FLAGS_GROUP led_flags;
void _tx_timer_interrupt(void);

void SysTick_Handler(void)
{
    HAL_IncTick();
    _tx_timer_interrupt();        // ThreadX tick
}

void TIM17_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim17);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM17)
    {
        if (led_flags_ready) {
            tx_event_flags_set(&led_flags, 0x01, TX_OR);
        }
    }
}
