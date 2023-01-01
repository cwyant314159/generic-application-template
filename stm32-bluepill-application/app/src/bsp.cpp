#include "bsp.hpp"

// STANDARD LIBRARY
#include <stdint.h>

// CMSIS
#include "stm32f1xx.h"

volatile uint32_t Bsp::Util::g_tick_ms;

void Bsp::Util::delay(uint32_t delay_ms)
{
    /* Compute the end tick of the delay */
    const uint32_t end = delay_ms + g_tick_ms;

    /* Wait for the delay to lapse. */
    while(g_tick_ms != end) { }
}