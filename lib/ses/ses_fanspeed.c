/**
 * @file    ses_fanspeed.c
 * @brief   Fan speed measurement module using tacho pulses and Timer1.
 *
 * This module uses external interrupt INT6 to count rising edges on the tacho
 * pin (PE6) of a fan, and Timer1 to periodically compute fan speed (RPM).
 * It provides both the most recent RPM measurement and a filtered version
 * using a median filter for improved stability.
 */

/* INCLUDES ******************************************************************/

#include "ses_fanspeed.h"
#include "ses_led.h"
#include "ses_timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

/* DEFINES & MACROS **********************************************************/

#define TACHO_PIN      PE6
#define TACHO_DDR      DDRE
#define TACHO_PORT     PORTE
#define TACHO_INT      INT6
#define TACHO_ISC      ISC60
#define EDGES_PER_REV  2

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#define TIMER1_PRESCALER 1024
#define TICKS_PER_SECOND (F_CPU / TIMER1_PRESCALER)
#define MEDIAN_FILTER_SIZE 7           ///< Number of samples for median filter

/* GLOBAL VARIABLES **********************************************************/

static volatile uint16_t edgeCount = 0;
static volatile uint16_t rpm = 0;
static volatile uint8_t fanStopped = 0;

static uint16_t rpmSamples[MEDIAN_FILTER_SIZE];
static uint8_t sampleIndex = 0;
static volatile uint8_t numSamples = 0;  

/*FUNCTION DEFINITION ********************************************************/

/**
 * @brief Timer1 callback, triggered once every second.
 */
static void fanspeed_timer1Handler(void) {
    cli();
    uint16_t count = edgeCount;
    edgeCount = 0;
    sei();

    if (count == 0) {
        rpm = 0;
        fanStopped = 1;
        led_redOn();
    } else {
        fanStopped = 0;
        led_redOff();
        rpm = (count * 60) / EDGES_PER_REV;
    }

    // Store sample in ring buffer for median filter
    rpmSamples[sampleIndex] = rpm;
    sampleIndex = (sampleIndex + 1) % MEDIAN_FILTER_SIZE;

    if (numSamples < MEDIAN_FILTER_SIZE) {
        numSamples++;
    }
}

/**
 * @brief Initializes the fan speed measurement module.
 */
void fanspeed_init(void) {
    // Set PE6 as input
    TACHO_DDR &= ~(1 << TACHO_PIN);

    // Enable INT6 on rising edge
    //Setting the interrupt sense in EICRB. Enabling the interrupt in EIMSK
    EICRB |= (1 << ISC61) | (1 << ISC60);  
    EIMSK |= (1 << TACHO_INT);

    // Enable Timer1 in CTC mode with 1 second period. calculate rpm once per second and reset the pulse count
    PRR0 &= ~(1 << PRTIM1); 
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode, Prescaler = 1024
    OCR1A = TICKS_PER_SECOND - 1; 
    TIMSK1 |= (1 << OCIE1A); // Enable compare match interrupt

    timer1_setCallback(fanspeed_timer1Handler);
}

/**
 * @brief INT6 Interrupt Service Routine (ISR).
 *
 * - Triggered on every rising edge of tacho signal (PE6).
 */
ISR(INT6_vect) {
    edgeCount++;
    led_yellowToggle();
}

/**
 * @brief Gets the most recent RPM measurement.
 *
 * @return RPM value calculated in the last 1-second interval.
 */
uint16_t fanspeed_getRecent(void) {
    return rpm;
}

/**
 * @brief Gets the filtered RPM using a median filter.
 *
 * Uses a simple insertion sort to find the median of the last N samples.
 *
 * @return Median RPM value of the most recent samples.
 */
uint16_t fanspeed_getFiltered(void) {
    uint16_t temp[MEDIAN_FILTER_SIZE];
    uint8_t i, j;
    // Copy current samples atomically
    uint8_t copyCount;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        copyCount = numSamples;
        for (i = 0; i < copyCount; i++) {
            temp[i] = rpmSamples[i];
        }
    }
    // Simple insertion sort up to copyCount
    for (i = 1; i < copyCount; i++) {
        uint16_t key = temp[i];
        j = i;
        while (j > 0 && temp[j - 1] > key) {
            temp[j] = temp[j - 1];
            j--;
        }
        temp[j] = key;
    }
    // Return median
    if (copyCount == 0) return 0;

    if (copyCount % 2 == 1) {
        return temp[copyCount / 2];
    } else {
        return (temp[copyCount / 2 - 1] + temp[copyCount / 2]) / 2;
    }
}

