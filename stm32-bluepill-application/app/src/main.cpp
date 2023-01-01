#include <stdint.h>

#if defined(USE_HAL_DRIVER)

// STM32 HAL
#include "stm32f1xx_hal.h"

#else

// APP
#include "bsp.hpp"
#include "led.hpp"

// CMSIS
#include "stm32f1xx.h"

#endif

#if defined(USE_HAL_DRIVER)
static constexpr uint32_t LED_Pin = GPIO_PIN_13;
static GPIO_TypeDef *LED_GPIO_Port = GPIOC;
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
#endif

int main(void)
{
    /* Initialize the system */
#if defined(USE_HAL_DRIVER)
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
#else
    Bsp::init();
    Led led(Bsp::Components::Led::port, Bsp::Components::Led::pin);
#endif
    
    /* Main loop */
    while (1) {

        /* Toggle the LED */
#if defined(USE_HAL_DRIVER)
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        HAL_Delay(500);
#else
        led.toggle();
        Bsp::Util::delay(500);
#endif
    }

    /* Satisfy compiler. Should never get here */
    return 0;
}

#if defined(USE_HAL_DRIVER)
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {
        .OscillatorType      = RCC_OSCILLATORTYPE_HSE,
        .HSEState            = RCC_HSE_ON,
        .HSEPredivValue      = RCC_HSE_PREDIV_DIV1,
        .LSEState            = RCC_LSE_OFF,
        .HSIState            = RCC_HSI_ON,
        .HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
        .LSIState            = RCC_LSI_OFF,
        .PLL = {
            .PLLState  = RCC_PLL_ON,
            .PLLSource = RCC_PLLSOURCE_HSE,
            .PLLMUL    = RCC_PLL_MUL9
        },
    };

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        __BKPT(0);
    }

    RCC_ClkInitTypeDef RCC_ClkInitStruct = {
        .ClockType = RCC_CLOCKTYPE_HCLK   |
                     RCC_CLOCKTYPE_SYSCLK |
                     RCC_CLOCKTYPE_PCLK1  |
                     RCC_CLOCKTYPE_PCLK2  ,

        .SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK,
        .AHBCLKDivider  = RCC_SYSCLK_DIV1,
        .APB1CLKDivider = RCC_HCLK_DIV2,
        .APB2CLKDivider = RCC_HCLK_DIV1,
    };

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        __BKPT(0);
    }
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin  = LED_Pin,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_HIGH
    };

    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
}
#endif
