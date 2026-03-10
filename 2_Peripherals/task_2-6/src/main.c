#include <avr/io.h>
#include <util/delay.h>
#include "ses_button.h"
#include "ses_led.h"
#include "ses_display.h"


int main(void) {
	display_init();
	led_greenInit();
	led_redInit();
	button_init();
	
	uint16_t seconds = 0;
	while(1) {
		if (button_isPushButtonPressed()) {
			led_greenOn();}
		else {
			led_greenOff();
		}

		if (button_isRotaryButtonPressed() ){
			led_redOn();}
		else {
			led_redOff();
		}
	display_setCursor(0,0);
	fprintf(displayout, "Time : %u s", seconds);
	display_update();
	_delay_ms(500);
	seconds++;
	}
return 0;
}
