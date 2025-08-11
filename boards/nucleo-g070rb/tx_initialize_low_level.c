// boards/nucleo-g070rb/tx_initialize_low_level.c
#include "tx_api.h"
#include "stm32g0xx.h"        // CMSIS device
#include "stm32g0xx_hal.h"    // HAL (for __NVIC_PRIO_BITS)

/* Provide the required low-level init for ThreadX port (Cortex-M0+ / GCC) */
void _tx_initialize_low_level(void)
{
    /* Set PendSV & SysTick priorities to the lowest (cooperate with HAL/RTOS) */
    NVIC_SetPriority(PendSV_IRQn,  (1U << __NVIC_PRIO_BITS) - 1U);
    NVIC_SetPriority(SysTick_IRQn, (1U << __NVIC_PRIO_BITS) - 1U);

    /* Configure SysTick to generate TX tick (uses TX_TIMER_TICKS_PER_SECOND) */
    uint32_t reload = (SystemCoreClock / TX_TIMER_TICKS_PER_SECOND) - 1U;

    SysTick->CTRL = 0;                 // disable during setup
    SysTick->LOAD = reload;            // reload value
    SysTick->VAL  = 0;                 // clear current
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;   // enable + IRQ
}
