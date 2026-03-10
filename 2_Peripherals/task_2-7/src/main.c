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
        // Read ADC value from light sensor
        uint16_t lightRaw = adc_read(ADC_LIGHT_CH);
        uint16_t tempRaw = adc_read(ADC_TEMP_CH);
        uint16_t potiRaw = adc_read(ADC_POTI_CH);

		display_setCursor(0,0);
		fprintf(displayout, "Light: %u, Temp: %u, Poti: %u\n", lightRaw, tempRaw, potiRaw);
		display_update();

        _delay_ms(1000);  // Wait 1 second
    }
}
