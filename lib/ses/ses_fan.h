#ifndef SES_FAN_H_
#define SES_FAN_H_

/*INCLUDES *******************************************************************/

#include <stdint.h>

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes PWM and power control for the fan
 */
void fan_init(void);

/**
 * Enables fan power 
 */
void fan_enable(void);

/**
 * Disables fan power and sets duty cycle to 0 
 */
void fan_disable(void);

/**
 * Sets the fan speed based on the duty cycle.
 * @param dc Duty cycle from 0 (0%) to 255 (100%)
 */
void fan_setDutyCycle(uint8_t dc);

#endif /* SES_FAN_H_ */

