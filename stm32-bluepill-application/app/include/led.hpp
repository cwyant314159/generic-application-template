#ifndef LED_HPP
#define LED_HPP

#include <stdint.h>

// APP
#include "bsp.hpp"

// CMSIS
#include "stm32f1xx.h"

class Led {

    public:
        Led(GPIO_TypeDef * const port, uint32_t pin);
        void set(bool val);
        void toggle(void);

    private:
        GPIO_TypeDef * const mPort = nullptr;
        uint32_t mPin = 0;
};

#endif /* LED_HPP */