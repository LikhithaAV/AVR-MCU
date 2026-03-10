
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "ses_led.h"
#include "ses_button.h"
#include <avr/interrupt.h>


void onPushButtonEvent(void) {
    led_redToggle();
}

void onRotaryButtonEvent(void) {
    led_greenToggle();
}


int main(void) {
    // Initialize hardware
    led_redInit();
    led_greenInit();
    button_init();

    // Set callbacks
    button_setRotaryButtonCallback(onRotaryButtonEvent);
    button_setPushButtonCallback(onPushButtonEvent);

    // Enable global interrupts
    sei();

    while (1) {
        // Main loop does nothing — work is interrupt-driven
    }

    return 0;
}
