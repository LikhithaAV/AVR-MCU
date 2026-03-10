/**
 * @file ses_led.c
 * @brief LED control driver for SES board.
 *
 * This file provides functions to initialize and control the red, green, 
 * and yellow LEDs. 
 */

/* INCLUDES ******************************************************************/

#include <avr/io.h>
#include "ses_led.h"

/* DEFINES & MACROS **********************************************************/

// LED wiring on SES board
#define LED_RED_PORT       	PORTF
#define LED_RED_DDR         DDRF
#define LED_RED_BIT         5

#define LED_GREEN_PORT      PORTD
#define LED_GREEN_DDR       DDRD
#define LED_GREEN_BIT       2

#define LED_YELLOW_PORT     PORTD
#define LED_YELLOW_DDR      DDRD
#define LED_YELLOW_BIT      3


/* FUNCTION DEFINITION *******************************************************/

/**
 * @brief Initializes the red LED by setting its pin as output and turning it off.
 * 
 * This function configures the red LED pin as an output and ensures that
 * the LED is turned off initially 
 */
void led_redInit(void) {
    LED_RED_DDR |= (1 << LED_RED_BIT);
    LED_RED_PORT |= (1 << LED_RED_BIT);
}

/**
 * @brief Toggles the red LED state.
 *
 * This function inverts the current output level of the red LED pin.
 */
void led_redToggle(void) {
    LED_RED_PORT ^= (1 << LED_RED_BIT);
}

/**
 * @brief Turns the red LED on.
 *
 * Since the LEDs are active-low, writing 0 to the pin turns the LED on.
 */
void led_redOn(void) {
    LED_RED_PORT &= ~(1 << LED_RED_BIT);
}

/**
 * @brief Turns the red LED off.
 *
 * Writing 1 to the pin turns the active-low LED off.
 */
void led_redOff(void) {
    LED_RED_PORT |= (1 << LED_RED_BIT);
}

/**
 * @brief Initializes the yellow LED by setting its pin as output and turning it off.
 * 
 * This function configures the yellow LED pin as an output and ensures that
 * the LED is turned off initially 
 */
void led_yellowInit(void) {
    LED_YELLOW_DDR |= (1 << LED_YELLOW_BIT);
    LED_YELLOW_PORT |= (1 << LED_YELLOW_BIT);
}

/**
 * @brief Toggles the yello LED state.
 *
 * This function inverts the current output level of the yellow LED pin.
 */
void led_yellowToggle(void) {
    LED_YELLOW_PORT ^= (1 << LED_YELLOW_BIT);
}

/**
 * @brief Turns the yellow LED on.
 *
 * Since the LEDs are active-low, writing 0 to the pin turns the LED on.
 */
void led_yellowOn(void) {
    LED_YELLOW_PORT &= ~(1 << LED_YELLOW_BIT);
}

/**
 * @brief Turns the yellow LED off.
 *
 * Writing 1 to the pin turns the active-low LED off.
 */
void led_yellowOff(void) {
    LED_YELLOW_PORT |= (1 << LED_YELLOW_BIT);
}

/**
 * @brief Initializes the green LED by setting its pin as output and turning it off.
 * 
 * This function configures the green LED pin as an output and ensures that
 * the LED is turned off initially 
 */
void led_greenInit(void) {
    LED_GREEN_DDR |= (1 << LED_GREEN_BIT);
    LED_GREEN_PORT |= (1 << LED_GREEN_BIT);
}

/**
 * @brief Toggles the green LED state.
 *
 * This function inverts the current output level of the green LED pin.
 */
void led_greenToggle(void) {
    LED_GREEN_PORT ^= (1 << LED_GREEN_BIT);
}

/**
 * @brief Turns the green LED on.
 *
 * Since the LEDs are active-low, writing 0 to the pin turns the LED on.
 */
void led_greenOn(void) {
    LED_GREEN_PORT &= ~(1 << LED_GREEN_BIT);
}

/**
 * @brief Turns the green LED off.
 *
 * Writing 1 to the pin turns the active-low LED off.
 */
void led_greenOff(void) {
    LED_GREEN_PORT |= (1 << LED_GREEN_BIT);
}
