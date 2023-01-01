#ifndef BSP_HPP
#define BSP_HPP

// STANDARD LIBRARY
#include <stdint.h>

// CMSIS
#include "stm32f1xx.h"

namespace Bsp {

    namespace Components {

        namespace Osc {
            /* high speed external oscillator frequency */
            static constexpr uint32_t HSE_OSC_FREQ_HZ = 8000000;

            /* low speed external oscillator frequency */
            static constexpr uint32_t LSE_OSC_FREQ_HZ = 32768;
        }

        namespace Led {
            /* on board LED GPIO port pin */
            static constexpr uint32_t pin = 13;

            /* on board LED GPIO port */
            static GPIO_TypeDef * const port = GPIOC;
        }
    }

    namespace Util {
        /* number of milliseconds counted by the SysTick interrupt */
        extern volatile uint32_t g_tick_ms;

        /* primitive delay routine */
        void delay(uint32_t delay_ms);
    }

    /* board specific initialization */
    static inline void init(void)
    {
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    }
}

#endif /* BSP_HPP */