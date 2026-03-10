#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "ses_display.h"    
#include "ses_scheduler.h"  
#include "ses_fanspeed.h"   
#include "ses_fan.h"

/* TASK DESCRIPTORS *********************************************************/

task_descriptor_t displayTask ;



// Display update period (in ms)
#define DISPLAY_INTERVAL 1000

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
    uint8_t duty = OCR3A;
    display_clear();
    display_setCursor(0,0);
    fprintf(displayout, "Raw: %4u  Filt: %4u\nPWM: %3u\n", rawRpm, filteredRpm, duty);
    display_update();
}


/**
 * @brief Main entry point.
 *
 * Initializes the display, fan speed measurement, fan PWM control, and scheduler.
 * Then schedules the display update task and enters the scheduler run loop.
 */
int main(void) {
    display_init();
    fan_init();
    fanspeed_init();

    scheduler_init();
    fan_enable();
    // Optionally, start the fan running at a given duty cycle (e.g., 50%).
    fan_setDutyCycle(128);  // 0 = 0%, 255 = 100%, so 128 is ~50%

    displayTask.task    = displayFanStatus,
    displayTask.param   = NULL,
    displayTask.period  = DISPLAY_INTERVAL,
    displayTask.expire  = DISPLAY_INTERVAL,
    displayTask.execute = 0,
    displayTask.next    = NULL;

    // Add the display task to the scheduler.
    scheduler_add(&displayTask);

    // Globally enable interrupts.
    sei();
    // Run the scheduler indefinitely.
    scheduler_run();

    // Should never be reached.
    return 0;
}
