#include "ses_display.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "ses_adc.h"
#include "ses_display.h"

int main(void) {
    adc_init();          // Initialize the ADC
	display_init();
       
    while (1) {
        uint16_t poti = adc_read(ADC_POTI_CH);
        uint16_t light = adc_read(ADC_LIGHT_CH);
        int16_t tempVal = adc_getTemperature();  // Get temperature in 1/10 °C
        display_clear();
        display_setCursor(0,0);

        fprintf(displayout,  "Poti:  %4d\n", poti);
        display_update();

        fprintf(displayout, "Light: %4d\n", light);
        display_update();
        // Print as whole degrees with 1 decimal
        fprintf(displayout, "Temperature: %d.%d °C\n", tempVal / 10, abs(tempVal % 10));
        display_update();
        _delay_ms(1000);  // Delay 1 second (optional)
    }
}
