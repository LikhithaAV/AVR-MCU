#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/

#include <stdbool.h>

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes rotary encoder button and pushbutton
 */
void button_init(void);

/** 
 * Get the state of the pushbutton.
 */
bool button_isPushButtonPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryButtonPressed(void);

/**
 * Checks for state changes in buttons and triggers callbacks if pressed.
 */
void button_checkState(void);

/** 
 * Button event callbacks
 */
typedef void (*pButtonCallback)(void);

/** 
 * Registers a callback function for the rotary button press event.
 */
void button_setRotaryButtonCallback(pButtonCallback callback);

/** 
 * Registers a callback function for the push button press event.
 */
void button_setPushButtonCallback(pButtonCallback callback);

#endif /* SES_BUTTON_H_ */
