/**
 * @file main.c
 * @brief Main application to demonstrate cooperative multitasking using a scheduler.
 *
 * This program implements a stopwatch display, pushbutton-controlled LED toggling,
 * and rotary button-controlled stopwatch start/stop using a simple task scheduler.
 * The system runs on the SES platform with Timer interrupts.
 *
 * Features:
 * - Green LED toggles every second.
 * - Pushbutton toggles yellow LED with a 5-second auto-off timer.
 * - Rotary button starts/stops a stopwatch and displays the same.
 * - Stopwatch updates every 100ms.
 * - Debouncing is handled every 2ms.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "ses_led.h"
#include "ses_timer.h"
#include "ses_display.h"
#include "ses_scheduler.h"
#include "ses_button.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

/* ENUM *********************************************************************/

typedef enum {
    LED_GREEN,
    LED_YELLOW,
    LED_RED
} LedColor;

/* TASK DESCRIPTORS *********************************************************/

task_descriptor_t toggleLedTask;
task_descriptor_t debounceTask;
task_descriptor_t stopwatchTask;
task_descriptor_t yellowOffTask;

/* GLOBAL VARIABLES *********************************************************/

static uint8_t yellowLedOn = 0;
static uint16_t stopwatchCounter = 0;  
static uint8_t stopwatchRunning = 0;

/* FUNCTION DECLARATIONS ****************************************************/

void toggleLed(void* param);
void updateStopwatch(void* param);
void turnOffYellowLed(void* param);
void pushButtonHandler(void);
void rotaryButtonHandler(void);

/* FUNCTION DEFINITIONS *****************************************************/

/**
 * @brief Toggles the specified LED.
 * @param param Pointer to a LedColor enum.
 */
void toggleLed(void* param) {
    LedColor color = *(LedColor*)param;
    switch(color) {
        case LED_GREEN:
            led_greenToggle();
            break;
        case LED_YELLOW:
            led_yellowToggle();
            break;
        case LED_RED:
            led_redToggle();
            break;
    }
}

/**
 * @brief Displays the stopwatch time on the OLED.
 * @param seconds Seconds part of the time.
 * @param tenths Tenths of a second.
 */
void displayTime(uint8_t seconds, uint8_t tenths) {
    // Clear display
    display_clear();
    display_setCursor(0, 0);
    // Draw "Time: XX.X"
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "Time: %u.%u", seconds, tenths);
    
    for (uint8_t i = 0; buffer[i] != '\0'; i++) {
        display_putc(buffer[i]);
    }
    display_update();
}

/**
 * @brief Updates the stopwatch time.
 * 
 * Increments counter and updates display if stopwatch is running.
 */
void updateStopwatch(void* param) {
    if (stopwatchRunning) {
        stopwatchCounter++;
        uint8_t seconds = stopwatchCounter / 10;    //10 counts = 1 sec
        uint8_t tenths = stopwatchCounter % 10;

        displayTime(seconds, tenths);
    }
}

/**
 * @brief Turns off the yellow LED after timeout.
 */
void turnOffYellowLed(void* param) {
    yellowLedOn = 0;
    led_yellowOff();
}

/**
 * @brief Handles pushbutton press.
 * 
 * Turns yellow LED on for 5 seconds or cancels timer if already on.
 */
void pushButtonHandler(void) {
    if (!yellowLedOn) {
        yellowLedOn = 1;
        led_yellowOn();

        yellowOffTask.task = turnOffYellowLed;
        yellowOffTask.param = NULL;
        yellowOffTask.expire = 5000;  // 5 seconds
        yellowOffTask.period = 0;     // one-shot
        scheduler_add(&yellowOffTask);
    } else {
        scheduler_remove(&yellowOffTask);
        led_yellowOff();
        yellowLedOn = 0;
    }
}

/**
 * @brief Toggles stopwatch state on rotary button press.
 */
void rotaryButtonHandler(void) {
    stopwatchRunning = !stopwatchRunning;
}

/* MAIN *********************************************************************/
/**
 * @brief Entry point of the program.
 */
int main(void) {
//Initializes peripherals and tasks, then starts the scheduler.
    led_redInit();
    led_greenInit();
    led_yellowInit();
    button_init();
    display_init();
    scheduler_init();
    sei();   // Enable global interrupts

    // Set callbacks. Register button event handlers
    button_setPushButtonCallback(pushButtonHandler);
    button_setRotaryButtonCallback(rotaryButtonHandler);

    // GREEN LED TOGGLE TASK (0.5Hz = every 1000ms)
    static LedColor greenLed = LED_GREEN;
    toggleLedTask.task = toggleLed;  //pointer to the callback function toggleLed
    toggleLedTask.param = &greenLed;
    toggleLedTask.expire = 0;
    toggleLedTask.period = 1000;  // 1s
    scheduler_add(&toggleLedTask);

    // DEBOUNCE TASK (2ms period)
    debounceTask.task = (task_t)button_checkState;
    debounceTask.param = NULL;
    debounceTask.expire = 2;
    debounceTask.period = 2;
    scheduler_add(&debounceTask);

    // STOPWATCH TASK (every 100ms)
    stopwatchTask.task = updateStopwatch;
    stopwatchTask.param = NULL;
    stopwatchTask.expire = 100;
    stopwatchTask.period = 100;
    scheduler_add(&stopwatchTask);

    // RUN SCHEDULER
    scheduler_run();

    return 0;
}
