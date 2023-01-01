#include "stm32f1xx_it.h"

#ifdef USE_HAL_DRIVER
#include "stm32f1xx_hal.h"
#else
#include "bsp.hpp"
#endif

/* non-maskable interrupt handler */
extern "C" void NMI_Handler(void)
{
    while (1) { }
}

/* hard fault interrupt handler */
extern "C" void HardFault_Handler(void)
{
    while (1) { }
}

/* memory management fault interrupt handler */
extern "C" void MemManage_Handler(void)
{
    while (1) { }
}

/* bus fault interrupt handler */
extern "C" void BusFault_Handler(void)
{
    while (1) { }
}

/* usage fault interrupt handler */
extern "C" void UsageFault_Handler(void)
{
    while (1) { }
}

/* system service call interrupt handler */
extern "C" void SVC_Handler(void)
{
}

/* debug monitor interrupt handler */
extern "C" void DebugMon_Handler(void)
{
}

/* pendable request for system service interrupt handler */
extern "C" void PendSV_Handler(void)
{
}

/* system tick interrupt handler */
extern "C" void SysTick_Handler(void)
{
#if defined(USE_HAL_DRIVER)
    HAL_IncTick();
#else
    ++Bsp::Util::g_tick_ms;
#endif
}
