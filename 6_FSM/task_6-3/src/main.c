/**
 * @file main.c
 * @brief Event-driven finite state machine (FSM) based alarm clock implementation.
 *
 * Features:
 * - Clock time setting
 * - Alarm time setting and activation
 * - Time display with second updates
 * - Alarm trigger with red LED blinking
 * - Button interaction using push and rotary encoders
 * 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "ses_display.h"    
#include "ses_scheduler.h"  
#include "ses_timer.h"
#include "ses_led.h"
#include "ses_button.h"
#include <stdbool.h>

typedef struct fsm_s fsm_t;
typedef struct event_s event_t;

/**
 * @brief FSM return codes
 */
enum return_values {
    RET_HANDLED,
    RET_IGNORED,
    RET_TRANSITION
};
typedef enum return_values fsm_return_status_t;


fsm_return_status_t state_setTimeHour(fsm_t*, const event_t*);

/**
 * @brief State handler function pointer type.
 *
 * Each state handles an incoming event and may trigger transitions.
 */
typedef fsm_return_status_t (*state_t)(fsm_t*, const event_t*);

/**
 * @brief Finite State Machine object.
 */
struct fsm_s {
    state_t state;   
    bool isAlarmEnabled;   
    time_t timeSet;   
    time_t alarmTime;
    bool settingMinutes;
    uint8_t blinkCount; 
};

/**
 * @brief Event object.
 */
struct event_s {
    uint8_t signal;    
};

/**
 * @brief FSM event signals.
 */
enum {
    ENTRY,
    EXIT,
    TICK,
    ROTARY_PRESSED,
    PUSHBUTTON_PRESSED,
    TIME_MATCHED,
    ALARM_TIMEOUT
};

fsm_t theFsm;

/* TASK DESCRIPTORS *********************************************************/

task_descriptor_t tickTask;
task_descriptor_t alarmBlinkTask;
task_descriptor_t buttontask;

/* --- FSM States --- */
fsm_return_status_t state_uninitialized(fsm_t*, const event_t*);
fsm_return_status_t state_setTimeHour(fsm_t*, const event_t*);
fsm_return_status_t state_setTimeMinute(fsm_t*, const event_t*);
fsm_return_status_t state_normalOperation(fsm_t*, const event_t*);
fsm_return_status_t state_setAlarmHour(fsm_t*, const event_t*);
fsm_return_status_t state_setAlarmMinute(fsm_t*, const event_t*);
fsm_return_status_t state_alarmActive(fsm_t*, const event_t*);

/* FUNCTION DECLARATIONS ****************************************************/

void tick_task(void* param) ;
void push_cb(void) ;
void button_task(void *param) ;
void rotary_cb(void);

/**
 * @brief Dispatches an event to the FSM.
 *
 * Handles EXIT and ENTRY transitions automatically.
 *
 * @param fsm Pointer to the FSM object.
 * @param event Pointer to the incoming event.
 */
void fsm_dispatch(fsm_t *fsm, const event_t *event) {
    const event_t exitEvent = {.signal = EXIT};
    const event_t entryEvent = {.signal = ENTRY};
    state_t s = fsm->state; //stores current state function pointer
    fsm_return_status_t r = fsm->state(fsm, event);
    if (r == RET_TRANSITION) {
        s(fsm, &exitEvent);
        fsm->state(fsm, &entryEvent);
    }
}

/**
 * @brief Initializes the FSM.
 *
 * @param fsm Pointer to the FSM object.
 * @param initState Initial state handler.
 */
inline static void fsm_init(fsm_t *fsm, state_t initState) {
    fsm->state = initState;
    fsm->isAlarmEnabled = false;
    fsm->settingMinutes = false;
    fsm->blinkCount = 0;
    const event_t entryEvent = {.signal = ENTRY};
    fsm->state(fsm, &entryEvent);
}

/* --- FSM State Implementations --- */

/**
 * @brief Clock uninitialized state.
 * 
 * Shows default screen and waits for user input to begin time setting.
 */
fsm_return_status_t state_uninitialized(fsm_t *fsm, const event_t *event) {
    switch (event->signal) {
        case ENTRY:
            display_clear();
            display_setCursor(0, 0);
            fprintf(displayout, "Uninitialized Clock");
            display_setCursor(1, 1);
            fprintf(displayout, "HH:MM:SS");
            display_update();
            return RET_HANDLED;

        case PUSHBUTTON_PRESSED:
            fsm->state = state_setTimeHour;
            return RET_TRANSITION;

        default:
            return RET_IGNORED;
    }
}

/**
 * @brief State for setting the current hour.
 */
fsm_return_status_t state_setTimeHour(fsm_t *fsm, const event_t *event) {
    switch (event->signal) {
        case ENTRY:
            display_clear();
            display_setCursor(0, 0);
            fprintf(displayout, "Set Hour: %02u", fsm->timeSet.hour);
            display_update();
            return RET_HANDLED;

        case ROTARY_PRESSED:
            fsm->timeSet.hour = (fsm->timeSet.hour + 1) % 24;
            display_clear();
            display_setCursor(0, 0);
            fprintf(displayout, "Set Hour: %02u", fsm->timeSet.hour);
            display_update();
            return RET_HANDLED;

        case PUSHBUTTON_PRESSED:
            fsm->state = state_setTimeMinute;
            return RET_TRANSITION;

        default:
            return RET_IGNORED;
    }
}

/**
 * @brief State for setting the current minute.
 */
fsm_return_status_t state_setTimeMinute(fsm_t *fsm, const event_t *event) {
    switch (event->signal) {
        case ENTRY:
            fsm->timeSet.minute = 0;
            display_clear();
            display_setCursor(0,0);
            fprintf(displayout, "Set Minute: %02u", fsm->timeSet.minute);
            display_update();
            return RET_HANDLED;
        case ROTARY_PRESSED:
            fsm->timeSet.minute = (fsm->timeSet.minute + 1) % 60;
            display_clear();
            display_setCursor(0,0);
            fprintf(displayout, "Set Minute: %02u", fsm->timeSet.minute);
            display_update();
            return RET_HANDLED;
        case PUSHBUTTON_PRESSED: {
            system_time_t now = (fsm->timeSet.hour * 3600000UL + fsm->timeSet.minute * 60000UL);
            scheduler_setTime(now);
            fsm->state = state_normalOperation;
            return RET_TRANSITION;
        }
        default:
            return RET_IGNORED;
    }
}

/**
 * @brief Default running clock state.
 * 
 * Displays time and checks for alarm match.
 */
fsm_return_status_t state_normalOperation(fsm_t *fsm, const event_t *event) {
    switch (event->signal) {
        case ENTRY:
            display_clear();
            display_update();
            return RET_HANDLED;
        case TICK: {
            time_t t;
            systemTimeToStruct(scheduler_getTime(), &t);
            display_clear();
            display_setCursor(0, 0);
            fprintf(displayout, "%02u:%02u:%02u", t.hour, t.minute, t.second);
            display_update();
            led_greenToggle();
            if (fsm->isAlarmEnabled && t.hour == fsm->alarmTime.hour && t.minute == fsm->alarmTime.minute && t.second == 0) {
                fsm->state = state_alarmActive;
                return RET_TRANSITION;
            }
            return RET_HANDLED;
        }
        case PUSHBUTTON_PRESSED:
            fsm->state = state_setAlarmHour;
            return RET_TRANSITION;
        case ROTARY_PRESSED:
            fsm->isAlarmEnabled = !fsm->isAlarmEnabled;
            if (fsm->isAlarmEnabled) led_yellowOn();
            else led_yellowOff();
            return RET_HANDLED;
        default:
            return RET_IGNORED;
    }
}

/**
 * @brief State for setting the alarm hour.
 */
fsm_return_status_t state_setAlarmHour(fsm_t *fsm, const event_t *event) {
    switch(event->signal) {
        case ENTRY:
            fsm->alarmTime.hour = 0;
            display_clear();
            display_setCursor(0,0);
            fprintf(displayout, "Set Alarm Hour: %02u", fsm->alarmTime.hour);
            display_update();
            return RET_HANDLED;
        case ROTARY_PRESSED:
            fsm->alarmTime.hour = (fsm->alarmTime.hour + 1) % 24;
            display_clear();
            display_setCursor(0,0);
            fprintf(displayout, "Set Alarm Hour: %02u", fsm->alarmTime.hour);
            display_update();
            return RET_HANDLED;
        case PUSHBUTTON_PRESSED:
            fsm->state = state_setAlarmMinute;
            return RET_TRANSITION;
        default:
            return RET_IGNORED;
    }
}

/**
 * @brief State for setting the alarm minute.
 */
fsm_return_status_t state_setAlarmMinute(fsm_t *fsm, const event_t *event) {
    switch(event->signal) {
        case ENTRY:
            fsm->alarmTime.minute = 0;
            display_clear();
            display_setCursor(0,0);
            fprintf(displayout, "Set Alarm Minute: %02u", fsm->alarmTime.minute);
            display_update();
            return RET_HANDLED;
        case ROTARY_PRESSED:
            fsm->alarmTime.minute = (fsm->alarmTime.minute + 1) % 60;
            display_clear();
            display_setCursor(0,0);
            fprintf(displayout, "Set Alarm Minute: %02u", fsm->alarmTime.minute);
            display_update();
            return RET_HANDLED;
        case PUSHBUTTON_PRESSED:
            fsm->state = state_normalOperation;
            return RET_TRANSITION;
        default:
            return RET_IGNORED;
    }
}

/**
 * @brief Alarm active state with red LED blinking.
 */
fsm_return_status_t state_alarmActive(fsm_t *fsm, const event_t *event) {
    switch (event->signal) {
        case ENTRY:
            fsm->blinkCount = 0;
            led_redOn();
            return RET_HANDLED;
        case TICK:
            if (++fsm->blinkCount >= 5) {
                led_redOff();
                fsm->state = state_normalOperation;
                return RET_TRANSITION;
            }
            led_redToggle();
            return RET_HANDLED;
        case PUSHBUTTON_PRESSED:
        case ROTARY_PRESSED:
            led_redOff();
            fsm->state = state_normalOperation;
            return RET_TRANSITION;
        default:
            return RET_IGNORED;
    }
}

// --- Event Callbacks ---

/**
 * @brief Timer tick task.
 * 
 * Triggers a TICK event every 1s.
 */
void tick_task(void* param) {
    const event_t tick = {.signal = TICK};
    fsm_dispatch(&theFsm, &tick);
}

/**
 * @brief Push button callback.
 */
void push_cb(void) {
    const event_t e = {.signal = PUSHBUTTON_PRESSED};
    fsm_dispatch(&theFsm, &e);
}

/**
 * @brief Debounce button state periodically.
 */
void button_task(void *param) {
    (void)param;
    button_checkState();
}

/**
 * @brief Rotary encoder button callback.
 */
void rotary_cb(void) {
    const event_t e = {.signal = ROTARY_PRESSED};
    fsm_dispatch(&theFsm, &e);
}

int main(void) {
    display_init();
    led_redInit();
    led_greenInit();
    led_yellowInit();
    button_init();
    button_setRotaryButtonCallback((pButtonCallback)rotary_cb);
    button_setPushButtonCallback((pButtonCallback)push_cb);
    scheduler_init();

    tickTask.task = tick_task;
    tickTask.param = NULL;
    tickTask.period = 1000;
    tickTask.expire = 1000;
    tickTask.execute = 0;
    tickTask.next = NULL;
    scheduler_add(&tickTask);

    buttontask.task = button_task;
    buttontask.param = NULL;
    buttontask.period = 20;
    buttontask.expire = 20;
    buttontask.execute = 0;
    buttontask.next = NULL;
    scheduler_add(&buttontask);

    fsm_init(&theFsm, state_uninitialized);

    sei();
    scheduler_run();
    return 0;
}