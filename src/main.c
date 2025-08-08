#include "stm32g0xx_hal.h"

/* Nucleo-G070RB: LED on PA5, user button on PC13 */
#define LED_GPIO_PORT GPIOA
#define LED_PIN       GPIO_PIN_5
#define BTN_GPIO_PORT GPIOC
#define BTN_PIN       GPIO_PIN_13

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void EXTI4_15_IRQHandler_Config(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    //EXTI4_15_IRQHandler_Config();

    /* Idle loop */
    while (1) {
        /* nothing; LED toggles in EXTI callback */
        HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
        HAL_Delay(500);
    }
}

/* 64 MHz from HSI via PLL (G0-style fields) */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    /* Voltage scaling must be configured before higher frequencies */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    osc.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState            = RCC_HSI_ON;
    osc.HSIDiv              = RCC_HSI_DIV1;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

    osc.PLL.PLLState  = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSI;   /* 16 MHz */
    osc.PLL.PLLM      = RCC_PLLM_DIV4;       /* 16 / 4  = 4  */
    osc.PLL.PLLN      = 70;                  /* 4  * 70 = 280 */
    osc.PLL.PLLR      = RCC_PLLR_DIV5;       /* 280 / 5 = 56 MHz SYSCLK */
    osc.PLL.PLLP      = RCC_PLLP_DIV10;      /* not used here, but must be valid */
    if (HAL_RCC_OscConfig(&osc) != HAL_OK) { while (1) {} }

    clk.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    clk.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2) != HAL_OK) { while (1) {} }
}

static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* LED output */
    GPIO_InitTypeDef g = {0};
    g.Pin   = LED_PIN;
    g.Mode  = GPIO_MODE_OUTPUT_PP;
    g.Pull  = GPIO_NOPULL;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &g);

    /* Ensure LED off initially */
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET);
}

/* Configure EXTI on PC13 (falling edge) */
static void EXTI4_15_IRQHandler_Config(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin  = BTN_PIN;
    g.Mode = GPIO_MODE_IT_FALLING;
    g.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BTN_GPIO_PORT, &g);

    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

/* HAL EXTI callback: toggle LED when PC13 falls (button press) */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t pin)
{
    if (pin == BTN_PIN) {
        HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
    }
}
