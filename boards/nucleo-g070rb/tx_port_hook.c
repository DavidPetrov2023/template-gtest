#include "tx_api.h"
#include "stm32g0xx_hal.h"

/* HAL nebude konfigurovat SysTick */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
    (void)TickPriority;
    return HAL_OK;
}

/* HAL tick napoj na ThreadX čas */
uint32_t HAL_GetTick(void) {
    return (uint32_t)((1000u / TX_TIMER_TICKS_PER_SECOND) * tx_time_get());
}

/* Volitelné: ať HAL_Delay používá ThreadX sleep */
void HAL_Delay(uint32_t ms) {
    ULONG ticks = (ULONG)(( (uint64_t)ms * TX_TIMER_TICKS_PER_SECOND + 999 ) / 1000);
    tx_thread_sleep(ticks ? ticks : 1);
}
