/**
 * @file ses_scheduler.c
 * @brief Cooperative task scheduler for the SES (Smart Embedded Systems) platform.
 *
 * This file implements a simple, non-preemptive (cooperative) scheduler using a
 * singly linked list of task descriptors. It supports periodic and one-shot tasks
 * and uses Timer0 to trigger periodic execution via the scheduler_update function.  
 */

/* INCLUDES *******************************************************************/
#include <stdlib.h>
#include "ses_timer.h"
#include "ses_scheduler.h"
#include "util/atomic.h"
#include <avr/interrupt.h>
#include <stdint.h>

/* PRIVATE VARIABLES *********************************************************/

/**
 *  * @brief Head of the task linked list.
 * We make sure that the list is accessed only within atomic sections
 * protected by a memory barrier --> no volatile necessary
 */
static task_descriptor_t * taskList = NULL;

static system_time_t currentTime = 0;  // milliseconds since midnight

/* FUNCTION PROTOTYPES ***********************************************/
static void incrementSystemTime(void);

/* FUNCTION DEFINITION *************************************************/

/**
 * @brief Updates task states and handles expiration.
 * 
 * This function is called every 1ms by Timer0 interrupt.
 * It decrements the `expire` counter for all tasks.
 * If a task expires, it's marked for execution and reset if periodic.
 */
void scheduler_update(void) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        incrementSystemTime();

        task_descriptor_t* current = taskList;
        while (current != NULL) {
            if (current->expire > 0) current->expire--;
            if (current->expire == 0) {
                current->execute = 1;
                if (current->period > 0)
                    current->expire = current->period;
            }
            current = current->next;
        }
    }
}

/**
 * @brief Initializes the scheduler and configures Timer0.
 *
 * This function sets up the callback for Timer0 to `scheduler_update`
 * and initializes the task list to NULL.
 */
void scheduler_init() {
    timer0_setCallback(scheduler_update);
    timer0_start();
    taskList = NULL;
    currentTime = 0;
}

/**
 * @brief Runs the scheduler loop.
 *
 * This function runs indefinitely, checking for tasks marked as executable.
 * Tasks are executed if their `execute` flag is set.
 * One-shot tasks are removed after execution.
 */
void scheduler_run() {
    while (1) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            task_descriptor_t* current = taskList;

            while (current != NULL) {
                if (current->execute) {
                    current->execute = 0;
                    current->task(current->param);  //Run the function

                    if (current->period == 0) {
                        // One-shot task: remove and restart from head
                        scheduler_remove(current);
                        break;  // taskList may have changed; restart loop
                    }
                }
                current = current->next;
            }
        }
    }
}

/**
 * @brief Adds a new task to the scheduler.
 *
 * @param toAdd Pointer to a properly initialized task_descriptor_t.
 * @return true if the task was added successfully, false if invalid or duplicate.
 */
bool scheduler_add(task_descriptor_t * toAdd) {
    if (toAdd == NULL || toAdd->task == NULL) return false;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        task_descriptor_t* current = taskList;
        while (current != NULL) {
            if (current == toAdd) return false; // prevent duplicates
            current = current->next;
        }
        toAdd->next = taskList;
        toAdd->execute = 0;
        taskList = toAdd;
    }

    return true;
}

/**
 * @brief Removes a task from the scheduler.
 *
 * @param toRemove Pointer to the task to remove.
 */
void scheduler_remove(const task_descriptor_t * toRemove) {
    if (toRemove == NULL) return;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        task_descriptor_t* current = taskList;
        task_descriptor_t* prev = NULL;

        while (current != NULL) {
            if (current == toRemove) {
                if (prev == NULL) {
                    taskList = current->next;
                } else {
                    prev->next = current->next;
                }
                return;
            }
            prev = current;
            current = current->next;
        }
    }
}

/*** CLOCK FUNCTIONS *********************************************************/
/**
 * @brief Retrieves the current system time in a thread-safe manner.
 *
 * @return The current system time as a `system_time_t`.
 */
system_time_t scheduler_getTime(void) {
    system_time_t timeCopy;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        timeCopy = currentTime;
    }
    return timeCopy;
}

/**
 * @brief Sets the system time in a thread-safe manner.
 *  
 * @param time The new system time to set, in milliseconds.
 */
void scheduler_setTime(system_time_t time) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        currentTime = time;
    }
}

/**
 * @brief Increments the system time counter.
 */
static void incrementSystemTime(void) {
    currentTime++;
    if (currentTime >= 86400000UL) {
        currentTime = 0;  // reset at midnight (24*60*60*1000 ms)
    }
}

/**
 * @brief Converts system_time_t to human-readable time_t.
 */
void systemTimeToStruct(system_time_t sysTime, time_t* t) {
    t->hour = (sysTime / 3600000UL) % 24;
    t->minute = (sysTime / 60000UL) % 60;
    t->second = (sysTime / 1000UL) % 60;
    t->milli = sysTime % 1000;
}

/**
 * @brief Converts human-readable time_t to system_time_t.
 */
system_time_t structToSystemTime(const time_t* t) {
    return ((system_time_t)t->hour * 3600000UL) +
           ((system_time_t)t->minute * 60000UL) +
           ((system_time_t)t->second * 1000UL) +
           t->milli;
}



