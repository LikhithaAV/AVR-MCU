/**
 * @file    main.c
 * @brief   Example program to display the system time using SES platform.
 *
 * This program initializes the system display and scheduler, sets an arbitrary start time,
 * and periodically displays the current time (HH:MM:SS.mmm) on the display every second.
 */
#include <avr/io.h>
#include <util/delay.h>
#include "ses_scheduler.h"
#include "ses_timer.h"
#include <stdio.h>
#include <ses_display.h>
#include <avr/interrupt.h>

/* TASK DESCRIPTORS *********************************************************/

task_descriptor_t displayTime ;

/**
 * @brief Task function to display the current system time on the LCD.
 *
 * @param param Unused parameter (kept for compatibility with task function signature).
 */
void display_time(void *param) {
    (void)param;
    time_t t;
    systemTimeToStruct(scheduler_getTime(), &t);

    display_clear();
    display_setCursor(0, 0);
    fprintf(displayout, "%02d:%02d:%02d.%03d", t.hour, t.minute, t.second, t.milli);
    display_update();
}

int main(void) {
    display_init();
    // Initialize the scheduler
    scheduler_init();

    // Set an arbitrary time
    time_t startTime = { .hour = 13, .minute = 54, .second = 28, .milli = 200 };
    system_time_t start = structToSystemTime(&startTime);
    scheduler_setTime(start);


    displayTime.task = display_time;
    displayTime.param = NULL;
    displayTime.expire = 50;
    displayTime.period = 50;
    displayTime.execute = 0;
    displayTime.next = NULL;

    scheduler_add(&displayTime);

    sei();
    // Run the scheduler (will execute time_task once after 1s)
    scheduler_run();

    return 0;
}
