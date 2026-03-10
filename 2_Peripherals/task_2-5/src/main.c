#include <avr/io.h>
#include <util/delay.h>
#include "ses_button.h"
#include "ses_led.h"

int main(void) {
	button_init();
	led_greenInit();
	led_redInit();
	
	while(1) {
		if (button_isPushButtonPressed()) {
			led_greenOn();
			_delay_ms(1000);
			led_greenOff();
		}
		else if (button_isRotaryButtonPressed()) {
			led_redOn();
			_delay_ms(1000);
			led_redOff();
		}
		else {
			//do nothing
		}
	}
	
	return 0;
}
