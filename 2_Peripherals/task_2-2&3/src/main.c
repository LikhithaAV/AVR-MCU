#include <avr/io.h>
#include <util/delay.h>
#include "ses_usbserial.h"
#include "ses_display.h"

/** toggles the red LED of the SES-board */
int main(void) {

	usbserial_init();
	display_init();
	sei(); // Activate interrupts. Required to use USB serial communication.

	while (1) { 
		_delay_ms(1000);
		fprintf(serialout, "Your USB output goes here\n");
 		display_setCursor(0,0); // Set cursor to column 0, row 0
 		fprintf(displayout, "Your display output goes here");
 		display_update();
	}
	return 0;
}
