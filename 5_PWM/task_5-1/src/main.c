#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "ses_timer.h"
#include "ses_fan.h"
#include "ses_adc.h"
#include "ses_scheduler.h"
#include "ses_button.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

/* TASK DESCRIPTORS *********************************************************/

task_descriptor_t buttonDebounceTask ;
task_descriptor_t adcReadTask ;

/* GLOBAL VARIABLES *********************************************************/

static volatile bool fan_is_on = false;
static uint8_t duty_cycle = 0;

/* FUNCTION DECLARATIONS ****************************************************/
void toggle_fan(void);
void pushbutton_task(void *param);
void adc_task(void *param);

/* FUNCTION DEFINITIONS *****************************************************/
void toggle_fan(void) {
    if (fan_is_on) {
        fan_disable();
        fan_is_on = false;
    } else {
        fan_enable();
        fan_is_on = true;
    }
}

void pushbutton_task(void *param) {
    (void)param;
    button_checkState();
}

void adc_task(void *param) {  //allows any type of data to be passed when schedulign it
    (void)param;
    if (fan_is_on) {
        uint16_t adc_val = adc_read(ADC_POTI_CH);
        // Map ADC (0-1023) to PWM (0-255)
        duty_cycle = (adc_val >> 2); // divide by 4 approx
        fan_setDutyCycle(duty_cycle);
    }
}

int main(void) {
    adc_init();
    button_init();
    fan_init();
    fan_disable();

    scheduler_init();
    sei();
    // Use the callback API
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

    scheduler_add(&buttonDebounceTask);
    scheduler_add(&adcReadTask);

    scheduler_run();

    return 0;
}