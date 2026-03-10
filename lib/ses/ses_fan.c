/**
 * @file    ses_fan.c
 * @brief   Fan control driver using Timer3 for PWM for SES  platform.
 *
 * This module provides functions to initialize, enable, disable, and control
 * the speed of a fan using an 8-bit Fast PWM signal. 
 */

/* INCLUDES ******************************************************************/

#include "ses_fan.h"
#include <avr/io.h>

/* DEFINES & MACROS **********************************************************/

#define FAN_POWER_DDR   DDRD
#define FAN_POWER_PORT  PORTD
#define FAN_POWER_PIN   PD4

#define FAN_PWM_DDR     DDRC
#define FAN_PWM_PIN     PC6

/* FUNCTION DEFINITION *******************************************************/

/**
 * @brief Initializes fan control module.
 */
void fan_init(void) {
    // Configure PD4 as output to control the fan power
    FAN_POWER_DDR |= (1 << FAN_POWER_PIN);
    
    // Initially disable fan power
    FAN_POWER_PORT &= ~(1 << FAN_POWER_PIN);

    // Enable Timer3 by clearing Power Reduction bit
    PRR1 &= ~(1 << PRTIM3);

    // Set PC6 (OC3A) as output for PWM signal
    FAN_PWM_DDR |= (1 << FAN_PWM_PIN);

    // Configure Fast PWM, 8-bit mode
    // com3a1=1 and com3a0 = 0 sets compare output mode
    // Fast PWM 8-bit, Prescaler = 8, WGM31 is 0
    TCCR3A = (1 << COM3A1) | (1 << WGM30);  
    TCCR3B = (1 << WGM32) | (1 << CS31);        
    TCCR3C = 0; 

    // Initialize OCR3A duty cycle to 0 (fan off)
    OCR3A = 0; 
}

/**
 * @brief Enables power to the fan. 
 */
void fan_enable(void) {
    FAN_POWER_PORT |= (1 << FAN_POWER_PIN); 
}

/**
 * @brief Disables power to the fan. 
 */
void fan_disable(void) {
    FAN_POWER_PORT &= ~(1 << FAN_POWER_PIN); 
    fan_setDutyCycle(0);                     
}

/**
 * @brief Sets the PWM duty cycle for fan speed control.
 *
 * @param dc  Duty cycle value (0–255), where:
 *            0   = 0% (off)
 *            255 = ~100% (full speed)
 */
void fan_setDutyCycle(uint8_t dc) {
    OCR3A = dc; 
}
