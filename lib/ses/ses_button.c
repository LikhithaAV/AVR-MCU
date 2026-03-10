/**
 * @file ses_button.c
 * @brief Button driver for SES platform.
 * 
 * This file provides functionality to initialize, read, and handle
 * pushbutton and rotary button presses.
 */

/* INCLUDES ******************************************************************/

#include <avr/io.h>
#include "ses_button.h"
#include <avr/interrupt.h>
#include <stddef.h>  // For NULL definition


/* DEFINES & MACROS **********************************************************/

// BUtton wiring on SES board
#define BUTTON_ROTARY_PORT        PORTB
#define BUTTON_ROTARY_DDR         DDRB
#define BUTTON_ROTARY_PIN         PINB
#define BUTTON_ROTARY_BIT         5

#define BUTTON_PUSH_PORT          PORTB
#define BUTTON_PUSH_DDR           DDRB
#define BUTTON_PUSH_PIN           PINB
#define BUTTON_PUSH_BIT           4

#define PC_INTERRUPT_CONTROL_REGISTER          PCICR
#define PC_MASK_REGISTER_BIT                   PCMSK0
#define PC_INTERRUPT_ENABLE_BIT                PCIE0

/* STATIC VARIABLES **********************************************************/
static volatile pButtonCallback rotaryCallback = NULL;  // Static and volatile for ISR safety
static volatile pButtonCallback pushCallback = NULL;    

static volatile uint8_t prevButtonState;

/* FUNCTION DEFINITION *******************************************************/

/**
 * @brief Initializes pushbutton and rotary button hardware.
 *
 * Configures the appropriate pins as input, enables internal pull-up resistors,
 * stores the initial state, and enables pin change interrupts for both buttons.
 */
void button_init(void) {
    BUTTON_ROTARY_DDR &= ~(1 << BUTTON_ROTARY_BIT);
    BUTTON_PUSH_DDR &= ~(1 << BUTTON_PUSH_BIT); 

    //Enable internal pull-up resistors ( set logic 1). when button is not pressed, internal pull upsare held high
    BUTTON_ROTARY_PORT |= (1 << BUTTON_ROTARY_BIT);
    BUTTON_PUSH_PORT |= (1 << BUTTON_PUSH_BIT);

    // Store initial state of button port
    prevButtonState = PINB;

    //Enable pin change interrupt
    PC_INTERRUPT_CONTROL_REGISTER |= (1 << PC_INTERRUPT_ENABLE_BIT);

    //Enable pin change mask for rotary and push bit. Trigger input only when these pins change
    PC_MASK_REGISTER_BIT |= (1 << BUTTON_ROTARY_BIT);
    PC_MASK_REGISTER_BIT |= (1 << BUTTON_PUSH_BIT);
}

/**
 * @brief Checks if the pushbutton is currently pressed.
 * 
 * @return true if the pushbutton is pressed, false otherwise.
 */
bool button_isPushButtonPressed(void) {
    // Returns true if the pin is LOW (i.e., button is pressed)
    return !(BUTTON_PUSH_PIN & (1 << BUTTON_PUSH_BIT)); 
}

/**
 * @brief Checks if the rotary button is currently pressed.
 * 
 * @return true if the rotary button is pressed, false otherwise.
 */
bool button_isRotaryButtonPressed(void) {
    // Returns true if the pin is LOW (i.e., button is pressed). button is wired with active low
    return !(BUTTON_ROTARY_PIN & (1 << BUTTON_ROTARY_BIT));
}

/**
 * @brief Checks for state changes in buttons and triggers callbacks if pressed.
 *
 * This function compares the current button states to the previously stored state.
 * If a rising edge (unpressed → pressed) is detected, the corresponding registered
 * callback function is invoked, if available.
 */
void button_checkState(void) {
    uint8_t currentState = PINB;  // Read current PINB state
    // Check push button
    bool prevPush = !(prevButtonState & (1 << BUTTON_PUSH_BIT));   
    bool currPush = !(currentState & (1 << BUTTON_PUSH_BIT));

    if (currPush && !prevPush) {  // Rising edge: Button just pressed. Now pressed and previously not pressed
        if (pushCallback != NULL) {
            pushCallback();
        }
    }
    // Check rotary button
    bool prevRot = !(prevButtonState & (1 << BUTTON_ROTARY_BIT));
    bool currRot = !(currentState & (1 << BUTTON_ROTARY_BIT));

    if (currRot && !prevRot) {  // Button just pressed
        if (rotaryCallback != NULL) {
            rotaryCallback();
        }
    }
    // Update previous state
    prevButtonState = currentState;
}

/**
 * @brief Pin Change Interrupt Request 0 ISR
 *
 * Currently unused. Defined to avoid unhandled interrupt behavior.
 */
ISR(PCINT0_vect)
{
    // Intentionally left empty for now
}

/**
 * @brief Registers a callback function for the rotary button press event.
 *
 * This function sets the user-defined callback that will be executed
 * when the rotary button is pressed. The callback must be a function 
 * with no parameters and a void return type. If the given function 
 * pointer is NULL, the assignment is skipped.
 *
 * @param callback Pointer to a function to be called on rotary button press.
 *                 Must not be NULL.
 *
 * @return void
 */
void button_setRotaryButtonCallback(pButtonCallback callback) {
    if (callback != NULL) {
        rotaryCallback = callback;
    }
}

/**
 * @brief Registers a callback function for the push button press event.
 *
 * This function sets the user-defined callback that will be executed
 * when the push button is pressed. The callback must be a function 
 * with no parameters and a void return type. If the given function 
 * pointer is NULL, the assignment is skipped.
 *
 * @param callback Pointer to a function to be called on push button press.
 *                 Must not be NULL.
 *
 * @return void
 */
void button_setPushButtonCallback(pButtonCallback callback) {
    if (callback != NULL) {
        pushCallback = callback;
    }
}