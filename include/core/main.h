#ifndef __MAIN_H
#define __MAIN_H
#ifdef __cplusplus
extern "C" { 
#endif

#include "stm32g0xx_hal.h"

/* LED4 on PA5 (Nucleo-G070RB) */
#define LED4_PIN        GPIO_PIN_5
#define LED4_GPIO_PORT  GPIOA
#define LED4_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

/* TIM17 handle is used in IRQ file */
extern TIM_HandleTypeDef htim17;

/* Prototypes used across translation units */
void SystemClock_Config(void);
void Error_Handler(void);

#ifdef __cplusplus
}
#endif
#endif /* __MAIN_H */
