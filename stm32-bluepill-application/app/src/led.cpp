#include "led.hpp"

Led::Led(GPIO_TypeDef * const port, uint32_t pin) :
    mPort(port),
    mPin(pin)
{ }

void Led::set(bool val)
{
    /*
     * The BSRR is used to atomically set and reset the pins of the port. The
     * upper 16 bits reset the corresponding pin when written with a 1. The 
     * lower 16 bits set the corresponding pin.
     */
    const uint32_t bit_offset = val ? 0 : 16;
    this->mPort->BSRR = 1 << (mPin + bit_offset);
}

void Led::toggle(void)
{
    this->mPort->ODR ^= (1 << this->mPin);
}