/**
 * @file    main.c
 * @brief   Fan speed control and monitoring application for SES platform.
 *
 * This file contains the main application logic for controlling and monitoring
 * a fan using PWM and external tacho feedback. The system reads potentiometer
 * values to adjust the fan's speed, debounces push button input to toggle the
 * fan state, and displays both raw and filtered RPM readings along with the
 * current duty cycle on screen.
 *
 * Features:
 *  - Toggle fan ON/OFF using pushbutton input (with debounce handling).
 *  - Dynamically adjust fan PWM duty cycle based on potentiometer input.
 *  - Measure fan speed using external interrupt-based edge counting.
 *  - Apply median filter to RPM measurements to smooth out noise.
 *  - Periodically display real-time fan speed and control data.
 */
/*INCLUDES *******************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "ses_display.h"    
#include "ses_scheduler.h"  
#include "ses_fanspeed.h"   
#include "ses_fan.h"
#include "ses_adc.h"
#include "ses_button.h"
#include "ses_led.h"

/* TASK DESCRIPTORS *********************************************************/

task_descriptor_t displayTask ;
task_descriptor_t buttonDebounceTask ;
task_descriptor_t adcReadTask ;

/* GLOBAL VARIABLES *********************************************************/

static volatile bool fan_is_on = false;
static uint8_t duty_cycle = 0;

/* DEFINES & MACROS **********************************************************/

#define DISPLAY_INTERVAL 1000  // Display update period (in ms)

/* FUNCTION DECLARATIONS ****************************************************/

void toggle_fan(void);
void pushbutton_task(void *param);
void adc_task(void *param);
void displayFanStatus(void *param);

/* FUNCTION DEFINITIONS *****************************************************/

/**
 * @brief Toggles the fan power state between ON and OFF.
 *
 * This function enables or disables the fan depending on its current state.
 */
void toggle_fan(void) {
    if (fan_is_on) {
        fan_disable();
        fan_is_on = false;
    } else {
        fan_enable();
        fan_is_on = true;
    }
}

/**
 * @brief Debounce and evaluate the state of the pushbutton.
 *
 * Called periodically by the scheduler to handle button debouncing logic.
 *
 * @param param Unused, must be NULL.
 */
void pushbutton_task(void *param) {
    (void)param;
    button_checkState();
}

/**
 * @brief Reads ADC value from the potentiometer and adjusts fan PWM duty cycle.
 *
 * @param param Unused, must be NULL.
 */
void adc_task(void *param) {  
    (void)param;
    if (fan_is_on) {
        uint16_t adc_val = adc_read(ADC_POTI_CH);
        duty_cycle = (adc_val >> 2); 
        fan_setDutyCycle(duty_cycle);
    }
}

/**
 * @brief Periodic task that displays the current fan speed (in RPM)
 *        and the PWM duty cycle.
 *
 * @param param Not used.
 */
void displayFanStatus(void *param) {
    (void)param;

    uint16_t rawRpm = fanspeed_getRecent();
    uint16_t filteredRpm = fanspeed_getFiltered();
    uint8_t dutycycle = OCR3A;
    
    display_clear();
    display_setCursor(0,0);
    fprintf(displayout, "RawRPM: %4u\n" , rawRpm);
    display_setCursor(1,1);
    fprintf(displayout, "FilteredRPM: %4u\n", filteredRpm);
    display_setCursor(2,2);
    fprintf(displayout, "Dutycycle: %3u\n",dutycycle);
    display_update();
}


/**
 * @brief Main entry point.
 *
 * Initializes the display, fan speed measurement, fan PWM control, and scheduler.
 * Then schedules the display update task and enters the scheduler run loop.
 */
int main(void) {
    adc_init();
    button_init();
    led_yellowInit();
    led_redInit();
    display_init();
    fan_init();
    fanspeed_init();
    scheduler_init();
    
    // Set the callback API
    button_setPushButtonCallback(toggle_fan);
    buttonDebounceTask.task = pushbutton_task,
    buttonDebounceTask.param = NULL,
    buttonDebounceTask.period = 20,     // 20ms debounce time
    buttonDebounceTask.expire = 20,
    buttonDebounceTask.execute = 0,
    buttonDebounceTask.next = NULL;

    adcReadTask.task = adc_task,
    adcReadTask.param = NULL,
    adcReadTask.period = 50,     // 50ms ADC polling
    adcReadTask.expire = 50,
    adcReadTask.execute = 0,
    adcReadTask.next = NULL;

    displayTask.task    = displayFanStatus,
    displayTask.param   = NULL,
    displayTask.period  = DISPLAY_INTERVAL,
    displayTask.expire  = DISPLAY_INTERVAL,
    displayTask.execute = 0,
    displayTask.next    = NULL;

    scheduler_add(&buttonDebounceTask);
    scheduler_add(&adcReadTask);
    scheduler_add(&displayTask);

    sei();

    scheduler_run();

    return 0;
}
