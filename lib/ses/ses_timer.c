/**
 * @file ses_timer.c
 * @brief Timer driver for SES  platform.
 *
 * This module provides functionality to configure and control Timer0 and
 * Timer1 on the AVR microcontroller in CTC (Clear Timer on Compare Match) mode.
 * It supports periodic interrupts for use with a task scheduler or other
 * time-based functionalities.
 */

/* INCLUDES ******************************************************************/
#include "ses_timer.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include "ses_scheduler.h"

/* DEFINES & MACROS **********************************************************/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Timer0 Configuration (1ms interval)
#define PRESCALER_TIMER0 64   //other prescalar value - 256,1024
#define TIMER0_CYC_FOR_1MILLISEC ((F_CPU / PRESCALER_TIMER0 / 1000) - 1)  //This value is loaded into OCR0A to trigger interrupt every 1 ms.

// Timer1 Configuration (5ms interval)
#define PRESCALER_TIMER1 64
#define TIMER1_CYC_FOR_5MILLISEC ((F_CPU / PRESCALER_TIMER1 / 200) - 1) // 5ms = 200Hz//This value is loaded into OCR1A to trigger interrupt every 5 ms.

#define T_CONTROL_REGISTER_A               TCCR0A
#define WAVEFORM_GENERATION_MODE           WGM01

/* GLOBAL VARIABLES **********************************************************/
static volatile pTimerCallback timer0_callback = 0;
static volatile pTimerCallback timer1_callback = 0;

/*FUNCTION DEFINITION ********************************************************/
/**
 * @brief Assign a callback function to Timer0 interrupt.
 * 
 * @param cb Pointer to the callback function.
 */
void timer0_setCallback(pTimerCallback cb) {
	timer0_callback = cb;
}

/**
 * @brief Start Timer0 in CTC mode with 1ms period.
 * Enables Timer0 compare match interrupt.
 */
void timer0_start() {
	// Configure Timer0 for CTC mode (Clear Timer on Compare Match)
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);  // Set prescaler = 64 (CS01 and CS00 bits)
    
    // Set output compare value for 1ms interval
    OCR0A = TIMER0_CYC_FOR_1MILLISEC;

    TIMSK0 |= (1 << OCIE0A);     // Enable compare match interrupt. This allows the ISR TIMER0_COMPA_vect to be called when OCR0A matches TCNT0.
    TIFR0  |= (1 << OCF0A);      // Clear any pending interrupt
}

/**
 * @brief Stop Timer0 and disable its interrupt.
 */
void timer0_stop() {
    TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00)); // Clear prescaler bits to stop the timer
    TIMSK0 &= ~(1 << OCIE0A);  // Disable interrupt
}

/**
 * @brief Assign a callback function to Timer1 interrupt.
 * 
 * @param cb Pointer to the callback function.
 */
void timer1_setCallback(pTimerCallback cb) {
	timer1_callback = cb;
}

/**
 * @brief Start Timer1 in CTC mode with 5ms period.
 * Enables Timer1 compare match interrupt.
 */
void timer1_start() {
	// CTC mode using OCR1A
    TCCR1B = (1 << WGM12);
    TCCR1B |= (1 << CS11) | (1 << CS10);  // Prescaler = 64

    OCR1A = TIMER1_CYC_FOR_5MILLISEC;

    TIMSK1 |= (1 << OCIE1A);     // Enable compare A interrupt
    TIFR1  |= (1 << OCF1A);      // Clear interrupt flag
}

/**
 * @brief Stop Timer1 and disable its interrupt.
 */
void timer1_stop() {
	TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10)); // Stop timer
    TIMSK1 &= ~(1 << OCIE1A);  // Disable interrupt
}

/* INTERRUPT SERVICE ROUTINES ***********************************************/

/**
 * @brief Timer0 Compare Match ISR. 
 * Calls user-defined callback set via timer0_setCallback()
 * Called every 1ms if timer0 is running.
 */
ISR(TIMER0_COMPA_vect) {
    if (timer0_callback != 0) {
        timer0_callback();  
    }
}

/**
 * @brief Timer1 Compare Match ISR.
 * Calls user-defined callback set via timer1_setCallback()
 * Called every 5ms if timer1 is running.
 */
ISR(TIMER1_COMPA_vect) {
	if (timer1_callback != 0) {
		timer1_callback();
	}
}

