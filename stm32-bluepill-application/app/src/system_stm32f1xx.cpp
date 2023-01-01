#include "bsp.hpp"
#include "stm32f1xx.h"

#if defined(USE_HAL_DRIVER)
    #include "stm32f1xx_hal_conf.h"

    #if !defined(HSE_VALUE)
        #define HSE_VALUE 8000000U /* Hz */
    #endif

    #if !defined(HSI_VALUE)
        #define HSI_VALUE 8000000U /* Hz */
    #endif
#else
    #include "bsp.hpp"
    #define HSE_VALUE Bsp::Components::Osc::HSE_OSC_FREQ_HZ
    #define HSI_VALUE 8000000U /* Hz */
#endif

/*
 * This variable is used by the CMSIS to keep track of the current system
 * clock. 
 * This variable is updated in three ways:
 *   1) by calling CMSIS function SystemCoreClockUpdate()
 *   2) by calling HAL API function HAL_RCC_GetHCLKFreq()
 *   3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
 *       Note: If you use this function to configure the system clock; then there
 *           is no need to call the 2 first functions listed above, since SystemCoreClock
 *           variable is updated automatically.
 */
extern "C" {
    uint32_t SystemCoreClock         = 16000000;
    const uint8_t AHBPrescTable[16U] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
    const uint8_t APBPrescTable[8U]  = {0, 0, 0, 0, 1, 2, 3, 4};
}

#if !defined(USE_HAL_DRIVER)
/*
 * GPIO initialization helper function prototypes.
 */
static void enableGpioPortClk(const GPIO_TypeDef * const port);
static void configureGpioPinOut(GPIO_TypeDef * const port, uint32_t pin);
#endif

/*
 * This function is called by the Reset_Handler after initializing the data
 * and BSS sections. The static constructors have not been called yet, so it
 * is not safe to access any global C++ objects.
 *
 * When not using the HAL, this function is the perfect place for clock setup,
 * IO configuration/assignments, and peripheral enables. Peripheral
 * initialization and configuration is best left to the main application. This
 * function is very application specific and will likely be changed for every
 * application. What this funciton lacks in flexibility, it makes up for it in
 * readability since we can use the constants and helpers defined in the Bsp
 * namespace.
 * 
 * For the template application not using the HAL, SystemInit will do the
 * following:
 * 
 *      1. Set the system clock frequency to the 72MHz maximum.
 *      2. Disable the HSI oscillator after configuring the system clock.
 *      3. Configure the IRQ priority bit usage.
 *      4. Configure (but not enable) the SysTick to tick at 1ms.
 *      5. Configure the LED's GPIO port.
 * 
 * The SysTick enable is left to the main application since the SysTick ISR may
 * or may not reference global C++ objects.
 */
extern "C" void SystemInit (void)
{
#if !defined(USE_HAL_DRIVER)
    /* CLOCK CONFIGURATION */
    /*
     * Configure the RCC control register to turn on HSE oscillator. To prevent
     * system glitches while setting up the clocks, the HSI (default)
     * oscillator is also enabled. The default value for the HSI trim (0x10) is
     * also added to prevent potential glitches. See section 7.3.1 in the
     * processor reference manual for more info.
     */
    RCC->CR = RCC_CR_HSEON | RCC_CR_HSION | (0x10 << RCC_CR_HSITRIM_Pos);

    /*
     * Wait for the HSERDY bit to be set in the RCC control register. Normally
     * it is bad form to do sit in a while loop monitoring a register without
     * any kind of timeout, but for this demonstration project, it will be good
     * enough.
     */
    while((RCC->CR & RCC_CR_HSERDY) == 0) { }

    /*
     * Configure the bus clock dividers in the RCC configuration register. To
     * be extra safe, bus clock dividers are configured separate from PLL
     * configuration. This is done to ensure that bus frequencies are for sure
     * below f_MAX.
     *
     * The application will run at 72MHz once clock configuration is complete.
     * All busses in the design will be configured to run at at their full
     * speed. The table below summarizes the clock sources, clock source
     * dividers, resultant clock speed, and resultant clock names.
     *
     * bus  | source | div | MHz | Name
     * -----+--------+-----+-----+-------
     * AHB  | SYSCLK |  1  | 72  | HCLK
     * APB1 | HCLK   |  2  | 36  | PCLK1
     * APB2 | HCLK   |  1  | 72  | PCLK2
     * ADC  | PCLK2  |  6  | 12  | ADCCLK
     *
     * Based on the above table, the clock to timers 2-7 and 12-14 is 72Mhz
     * (PCLK1 x 2). The clock to timers 1 and 8-11 is 72MHz (PCLK2 x 1).
     *
     * Even though USB is not used in this application, the USB prescaler is
     * configured to give the appropriate USB clock (48MHz).
     *
     * For more information, see Figure 8 (Clock Tree) and section 7.3.2 in the
     * processor reference manual.
     */
    RCC->CFGR = RCC_CFGR_USBPRE | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_ADCPRE_DIV6;

    /*
     * Configure the PLL parameters in the RCC configuration register. This
     * sets the PLL source to the undivided HSE clock (8Mhz). The PLL
     * multiplier is set to 9 to give the maximum clock speed of 72MHz.
     */
    RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;

    /*
     * Turn on the PLL by setting the PLLON bit of the RCC control register.
     */
    RCC->CR |= RCC_CR_PLLON;

    /*
     * Wait for the PLLRDY bit to be set in the RCC control register.
     */
    while((RCC->CR & RCC_CR_PLLRDY) == 0) { }

    /*
     * Since the SYSCLK is being changed, the latency bits in the flash access
     * control register must be modified. With a SYSCLK of 72MHz, two wait
     * states are required for flash access. See section 3.3.3 in the processor
     * reference manual for more information.
     *
     * Since the default configuration has the prefetch buffer enabled, the
     * buffer enable is set as well.
     *
     * There is a bug in the HAL FLASH_ACR_LATENCY_2 is shifted 1 too many
     * bits.
     */
    FLASH->ACR = FLASH_ACR_PRFTBE | (FLASH_ACR_LATENCY_2 >> 1);

    /*
     * Now that the bus dividers and PLL are configured, the system clock can
     * be changed from the HSI to the PLL output in the RCC configuration
     * register.
     *
     * Care must be taken not to clear any previously configured bits.
     */
    RCC->CFGR |= (RCC->CFGR & RCC_CFGR_SW) | RCC_CFGR_SW_PLL;

    /*
     * Wait for the clock switch status to indicate the PLL is the system
     * clock.
     */
    while((RCC->CR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) { }

    /*
     * Disable the HSI since it is no longer in use.
     */
    RCC->CR &= ~(RCC_CR_HSION);

    /*
     * Update the system core clock variable for CMSIS.
     */
    SystemCoreClockUpdate();

    /* IRQ PRIORITY CONFIGURATION */
    /*
     * Configure the System Control Block to give the maximum amount of bits to
     * the interrupt group priority. See section 4.4.5 of the processor
     * programming manual for more information.
     *
     * To get the appropriate priority group bits, the value 0x3 must be
     * written to bits [10:8]
     *
     * A special value of 0x05FA must be written to the upper 16 bits for the
     * register to update.
     */
    SCB->AIRCR = (0x05FA << 16) | (0x3 << 8);

    /*
     * Set the SysTick interrupt to the lowest priority (highest numerical 
     * value).
     */
    NVIC_SetPriority(SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);

    /*
     * Configure the SysTick to trigger an interrupt every 1 ms. The system
     * clock is 72MHz. To get a get a 1ms pulse, we need to compute the number
     * of clock ticks in 1ms.
     *
     *  72MHz = 72000000 ticks * 1 s       = 72000
     *          --------------   ---------   -----
     *          1 s              1000 ms     1 ms
     *
     * This means we need to set the SysTick to trigger an interrupt every
     * 72000 clock ticks. To compensate for the zero crossing tick, an extra
     * minus 1 is need.
     */
    constexpr uint32_t ms_ticks = 72000000 / 1000;
    constexpr uint32_t load     = (ms_ticks - 1) & 0xFFFFFF;
    SysTick->LOAD = load;

    /*
     * Write to the VAL register to clear it and the COUNTFLAG in the control
     * register.
     */
    SysTick->VAL = 0;

    /*
     * Configure the control register to enable both the counter and the tick
     * interrupt. The clock source for the SysTick is configured as the
     * processor (AHB) clock.
     */
    SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk;

    /*
     * Enable the peripheral clocks and IO used in the application.
     */
    enableGpioPortClk(Bsp::Components::Led::port);
    configureGpioPinOut(Bsp::Components::Led::port, Bsp::Components::Led::pin);

#endif /* !defined(USE_HAL_DRIVER) */
}

/*
 * Update SystemCoreClock variable according to Clock Register Values. The
 * SystemCoreClock variable contains the core clock (HCLK), it can be used by
 * the user application to setup the SysTick timer or configure other
 * parameters.
 *
 * NOTE:
 *  Each time the core clock (HCLK) changes, this function must be called to 
 *  update SystemCoreClock variable value. Otherwise, any configuration based
 *  on this variable will be incorrect.
 *
 * NOTE:
 *  The system frequency computed by this function is not the real frequency in
 *  the chip. It is calculated based on the predefined constant and the
 *  selected clock source:
 *
 *      If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
 *
 *      If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
 *
 *      If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**)
 *          or HSI_VALUE(*) multiplied by the PLL factors.
 *
 *  (*)  HSI_VALUE is a constant defined in stm32f1xx.h file (default value 8
 *       MHz) but the real value may vary depending on the variations in
 *       voltage and temperature.
 *
 *  (**) HSE_VALUE is a constant defined in stm32f1xx.h file (default value 8
 *       MHz or 25 MHz, depending on the product used), user has to ensure that
 *       HSE_VALUE is same as the real frequency of the crystal used. Otherwise,
 *       this function may have wrong result.
 *
 *  The result of this function could be not correct when using fractional
 *  value for HSE crystal.
 */
extern "C" void SystemCoreClockUpdate(void)
{
    /*
     * Grab a local copy of the configuration register once just incase an
     * interrupt that modifies this register happens.
     */
    const uint32_t cfgr = RCC->CFGR;
  
    /*
     * Compute the SYSCLK that will eventually be divided down to the system
     * core clock.
     */
    uint32_t sysclk = 0;
    switch (cfgr & RCC_CFGR_SWS) {
    case RCC_CFGR_SWS_HSI: {
        sysclk = HSI_VALUE;
        break;
    }
    
    case RCC_CFGR_SWS_HSE: {
        sysclk = HSE_VALUE;
        break;
    }

    case RCC_CFGR_SWS_PLL: { 
        /*
         * The PLL multiplier can be computed by adding 2 the register bits 
         * of the PLL multiplier. See section 7.3.2 of the processor reference
         * manual for more information.
         * 
         * NOTE: The maximum value of the multiplier is 16.
         */
        const uint32_t pll_mult_bits = (cfgr & RCC_CFGR_PLLMULL_Msk) >> RCC_CFGR_PLLMULL_Pos;
        const uint32_t pll_mult_calc = pll_mult_bits + 2;
        
        /* Clip the multiplier to its maximum value. */
        const uint32_t mult = (pll_mult_calc > 16) ? 16 : pll_mult_calc;

        /* 
         * Determine the source clock of the PLL. A value of zero indicates
         * that the PLL will use the HSI clock divided by 2. Non-zero source
         * values mean the PLL is source by the HSE.
         */
        const uint32_t src     = cfgr & RCC_CFGR_PLLSRC;
        const uint32_t src_clk = (0 == src) ? HSI_VALUE >> 1 : // HSI/2
                                              HSE_VALUE      ;
        
        /*
         * Determine if the PLL source clock is pre-divided. A pre-div value
         * of one means the HSE clock is divided by 2 before entry to the PLL.
         * Zero means the HSE clock is used undivided. This value has no
         * meaning when the HSI clock is used as the PLL source.
         */
        const uint32_t pre_div = cfgr & RCC_CFGR_PLLXTPRE;
        const uint32_t div     = (src != 0 && pre_div != 0) ? 2 : 1;

        /* Compute the sysclk using the values computed above. */
        sysclk = (src_clk / div) * mult;
        break;
    }

    default: {
        sysclk = HSI_VALUE;
        break;
    }
    }

    /*
     * Determine the AHB prescaler that divides the system clock into the final
     * system core clock frequency.
     */
    const uint32_t hpre = cfgr & RCC_CFGR_HPRE;
    const uint32_t ahb_div = (RCC_CFGR_HPRE_DIV2   == hpre) ? 2   :
                             (RCC_CFGR_HPRE_DIV4   == hpre) ? 4   :
                             (RCC_CFGR_HPRE_DIV8   == hpre) ? 8   :
                             (RCC_CFGR_HPRE_DIV16  == hpre) ? 16  :
                             (RCC_CFGR_HPRE_DIV64  == hpre) ? 64  :
                             (RCC_CFGR_HPRE_DIV128 == hpre) ? 128 :
                             (RCC_CFGR_HPRE_DIV256 == hpre) ? 256 :
                             (RCC_CFGR_HPRE_DIV512 == hpre) ? 512 :
                                                            1     ;

    /*
     * Compute the final system core clock. The system core has a constant
     * divide by 8 (right shift 3) after the AHB prescaler.
     */
    SystemCoreClock = (sysclk / ahb_div) >> 3;
}

#if !defined(USE_HAL_DRIVER)
/*
 * Enable the given GPIO port peripheral clock in the RCC module.
 */
static void enableGpioPortClk(const GPIO_TypeDef * const port)
{
    const uint32_t en = port == GPIOA ? RCC_APB2ENR_IOPAEN :
                        port == GPIOB ? RCC_APB2ENR_IOPBEN :
                        port == GPIOC ? RCC_APB2ENR_IOPCEN :
                        port == GPIOD ? RCC_APB2ENR_IOPDEN :
                        port == GPIOE ? RCC_APB2ENR_IOPEEN :
                                        0                  ;

    RCC->APB2ENR |= en;
}

/*
 * Configure the given GPIO pin as a push-pull fast output.
 */
static void configureGpioPinOut(GPIO_TypeDef * const port, uint32_t pin)
{
    /*
     * Exit the function early if the pin number is too high. 
     */
    if (pin > 15) return;

    /*
     * The GPIO pin configuration is distributed as 4 bits per pin in two
     * registers. For GPIO pins 0-7 the CRL register is used. Pins 8-15 are
     * configured in CRH. The offset for a given GPIO pins configuration
     * register can be computed with the following formula:
     *
     *  offset = (pin % 8) * 4
     * 
     * or without using divides and multiplies
     * 
     *  offset = (pin & 7) << 2;
     */
    const uint32_t offset = (pin & 7) << 2;

    /* 
     * For push-pull outputs, the upper 2 configuration bits are 0x00. To
     * enable fast mode on a pin the lower 2 configuration bits are 0x11;
     */
    constexpr uint32_t pin_cfg = 0x0011;
    
    /*
     * Configure either the high or low configuration register based on the pin
     * number.
     */
    const uint32_t mask = 0xFFFF  << offset;
    const uint32_t cfg  = pin_cfg << offset;
    if (pin <= 7) {
        port->CRL = (port->CRL & ~mask) | cfg;
    } else {
        port->CRH = (port->CRH & ~mask) | cfg;
    }

    /*
     * Default the newly configured pins to low. The atomic set and clear
     * registers has the IO clear bits in the upper 16 bits.
     */
    port->BSRR = 1 << (pin + 16);
}
#endif