#include "main.h"
#include "stm32g0xx_hal.h"
#include "tx_api.h"

/* ---- GPIO & TIM handles ---- */
GPIO_InitTypeDef GPIO_InitStruct = {0};
TIM_HandleTypeDef htim17;

/* ---- ThreadX objects ---- */
#define STACK_SZ 1024
static TX_THREAD led_thread;
static ULONG led_stack[STACK_SZ/sizeof(ULONG)];

TX_EVENT_FLAGS_GROUP led_flags;   /* ISR will set bit 0 */
volatile UINT led_flags_ready = 0;

/* Prototypes */
void SystemClock_Config(void);
static void MX_TIM17_Init(void);
static void led_entry(ULONG arg);
void Error_Handler(void);

/* ---------- LED thread ---------- */
static void led_entry(ULONG arg)
{
    (void)arg;
    ULONG flags;
    for (;;) {
        if (tx_event_flags_get(&led_flags, 0x01, TX_OR_CLEAR,
                               &flags, TX_WAIT_FOREVER) == TX_SUCCESS) {
            HAL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
        }
    }
}

void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;

    tx_event_flags_create(&led_flags, "led_flags");
    led_flags_ready = 1;

    tx_thread_create(&led_thread, "led", led_entry, 0,
                    led_stack, sizeof(led_stack),
                    5, 5, TX_NO_TIME_SLICE, TX_AUTO_START);

    MX_TIM17_Init();
    HAL_TIM_Base_Start_IT(&htim17);
}


int main(void)
{
    HAL_Init();
    SystemClock_Config();

    LED4_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef io = {
        .Pin = LED4_PIN, .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_HIGH
    };
    HAL_GPIO_Init(LED4_GPIO_PORT, &io);

    tx_kernel_enter();               // přenechat řízení RTOSu
    while (1) { __WFI(); }
}

/* Configure TIM17 to generate 2 IRQ/s (LED blink ≈1 Hz from RTOS thread) */
static void MX_TIM17_Init(void)
{
    __HAL_RCC_TIM17_CLK_ENABLE();
    uint32_t tim_clk = HAL_RCC_GetPCLK1Freq();
    uint32_t psc    = (tim_clk / 1000u) - 1u;        // 1 kHz
    uint32_t period = (1000u / 2u) - 1u;             // 2 IRQ/s

    htim17.Instance = TIM17;
    htim17.Init.Prescaler = psc;
    htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim17.Init.Period = period;
    htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&htim17);

    HAL_NVIC_SetPriority(TIM17_IRQn, 2, 0);          // G0 má priority 0..3
    HAL_NVIC_EnableIRQ(TIM17_IRQn);
}

/* Your working PLL config (unchanged from your example) */
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
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) { Error_Handler(); }
}

/* Simple error handler – fast blink to indicate fault */
void Error_Handler(void)
{
    __disable_irq();
    while (1) {
        HAL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
        for (volatile uint32_t i = 0; i < 50000; ++i) { __NOP(); }
    }
}
