#include <avr/io.h>
#include <util/delay.h>
#include "ses_led.h"

/** toggles the red LED of the SES-board */
int main(void) {
	led_redInit();     // Initialize the red LED pin
    led_yellowInit();

    while (1) {
        led_yellowOn();         // Turn red LED ON
         _delay_ms(1000);      // Wait for 500 ms

        // // led_redOff();        // Turn red LED OFF
        // // _delay_ms(1000);      // Wait for 500 ms

        led_redToggle();     // Toggle LED state
        _delay_ms(1000);      // Wait for 500 ms
	}
	return 0;
}
