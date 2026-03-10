#include <avr/io.h>
#include <util/delay.h>
#include <avr/io2313.h>
#include <avr/io90pwm161.h>

void wait(uint16_t millis)
{
	while (millis > 0)
    {
        for (uint8_t i = 0; i < 16; i++) // 16 * shortDelay ≈ 1 ms
        {
            shortDelay();
        }
        millis--;
    }
}

// Short delay function: executes a fixed small delay
void shortDelay(void)
{
    uint16_t i;
    for (i = 0x0100; i > 0; i--)
    {
        asm volatile ("nop"); // fixed delay of one CPU cycle (no operation)
    }
}

/** toggles the red LED of the SES-board */
int main(void) {

	DDRD |= (1 << DDD3);  //amber led (led4)
	while (1) {
		wait(500);
		PORTD ^= (1 << PORTD3);  // amber led (led4)
		//you can check by addign wait function here??
	}
			return 0;
}
