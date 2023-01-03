// STANDARD LIBRARY
#include <stdint.h>

// AVR
#include <avr/io.h>

#define LED_PIN 5U

int main(void)
{
    /* Initialize the system */
    // set LED_BIT as output
    // all other pins are inputs
    DDRB = _BV(LED_PIN);
    
    /* Main loop */
    while (1) {
        // toggle led
        PORTB ^= _BV(LED_PIN);

        // short delay
        for(volatile long i = 0; i < 100000; ++i);

        for(volatile uint16_t d = UINT16_C(0); d < UINT16_C(1023); ++d) {
            for(volatile uint8_t delay = UINT8_C(0); delay < UINT8_C(255); ++delay) { ; }
        }
    }

    /* Satisfy compiler. Should never get here */
    return 0;
}
