#include "main.h"
#include "stm32g0xx_hal.h"

/* USER CODE BEGIN PV */
GPIO_InitTypeDef GPIO_InitStruct = {0};   /* GPIO config struct */

/* Timer handle for periodic LED toggle (TIM17) */
TIM_HandleTypeDef htim17;
/* USER CODE END PV */

void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
static void MX_TIM17_Init(void);
/* USER CODE END PFP */

int main(void)
{
    /* HAL init + clocks */
    HAL_Init();
    SystemClock_Config();

    /* GPIO clock + LED pin setup */
    LED4_GPIO_CLK_ENABLE();
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin   = LED4_PIN;
    HAL_GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStruct);

    /* --- Init TIM17 for periodic interrupt --- */
    MX_TIM17_Init();
    HAL_TIM_Base_Start_IT(&htim17);  /* Start timer in interrupt mode */

    /* Main loop does nothing; LED toggles in timer interrupt */
    while (1)
    {
        __WFI(); /* Optional: sleep until next interrupt to save power */
    }
}

/* Configure TIM17 to generate update interrupt at 2 Hz
   (ISR toggles LED -> visible blink 1 Hz) */
static void MX_TIM17_Init(void)
{
    /* Enable TIM17 clock */
    __HAL_RCC_TIM17_CLK_ENABLE();

    /* Compute prescaler/period from current PCLK1 */
    uint32_t tim_clk = HAL_RCC_GetPCLK1Freq();   /* e.g., ~56 MHz with PLL */
    uint32_t target_irq_hz = 2U;                 /* 2 interrupts per second */
    /* Make 1 kHz timer tick for nicer granularity */
    uint32_t psc    = (tim_clk / 1000U) - 1U;    
    uint32_t period = (1000U / target_irq_hz) - 1U; /* 499 -> 2 Hz */

    htim17.Instance = TIM17;
    htim17.Init.Prescaler         = psc;
    htim17.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim17.Init.Period            = period;
    htim17.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim17) != HAL_OK) { 
        Error_Handler(); 
    }

    /* Enable NVIC for TIM17 and set priority */
    HAL_NVIC_SetPriority(TIM17_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM17_IRQn);
}

/* PLL / HSI clock config – převzato z tvého původního GPIO příkladu */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv              = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = RCC_PLLM_DIV4;
    RCC_OscInitStruct.PLL.PLLN            = 70;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV10;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV5;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                       RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/* Simple error handler – infinite loop */
void Error_Handler(void)
{
    __disable_irq();
    while (1) {
        HAL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
        for (volatile uint32_t i = 0; i < 50000; ++i) { __NOP(); }
    }
}
