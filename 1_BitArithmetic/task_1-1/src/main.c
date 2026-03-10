#include <avr/io.h>
#include <util/delay.h>

/** toggles the red LED of the SES-board */
int main(void) {

	DDRD |= (1 << DDD3);  //amber led (led4)
	DDRD |= (1 << DDD2);  //---Green led 
	DDRF |= (1 << DDF5);  //---Red led

	while (1) {
		_delay_ms(1000);
		
		PORTD ^= (1 << PORTD3);  // amber led (led4)
		PORTD ^= (1 << PORTD2);  //--Green led (led3)
		PORTF ^= (1 << PORTF5);  //---Red led (led2)
	}
	return 0;
}
