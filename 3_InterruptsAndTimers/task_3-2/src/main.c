
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "ses_led.h"
#include "ses_timer.h"
#include <avr/interrupt.h>

volatile uint16_t msCounter = 0;

void softwareTimer(void) {
    msCounter++;
    if (msCounter >= 1000) {
        led_yellowToggle();
        msCounter = 0;
    }
}

int main(void) {
    // Initialize hardware
    led_yellowInit();
    timer0_setCallback(softwareTimer); // Attach software callback
    timer0_start();              // Start 1ms Timer0


    // Enable global interrupts
    sei();

    while (1) {
        // Main loop does nothing — work is interrupt-driven
    }

    return 0;
}
