/**
 * @file mock_avr_io.c
 * @brief Mock AVR I/O implementations for testing
 */

#include "mock_avr_io.h"
#include <string.h>

/* Mock PORT registers */
uint8_t PORTF = 0xFF;
uint8_t DDRF = 0x00;
uint8_t PORF = 0x00;

uint8_t PORTD = 0xFF;
uint8_t DDRD = 0x00;
uint8_t PORD = 0x00;

uint8_t PORTE = 0xFF;
uint8_t DDRE = 0x00;
uint8_t PORE = 0x00;

uint8_t PORTA = 0xFF;
uint8_t DDRA = 0x00;
uint8_t PORA = 0x00;

uint8_t PORTB = 0xFF;
uint8_t DDRB = 0x00;
uint8_t PORB = 0x00;

uint8_t PORTC = 0xFF;
uint8_t DDRC = 0x00;
uint8_t PORC = 0x00;

/* Mock Timer/Counter registers */
uint8_t TCCR0A = 0x00;
uint8_t TCCR0B = 0x00;
uint8_t TCNT0 = 0x00;
uint8_t OCR0A = 0x00;
uint8_t OCR0B = 0x00;

uint8_t TCCR1A = 0x00;
uint8_t TCCR1B = 0x00;
uint16_t TCNT1 = 0x0000;
uint16_t OCR1A = 0x0000;
uint16_t OCR1B = 0x0000;

uint8_t TCCR2A = 0x00;
uint8_t TCCR2B = 0x00;
uint8_t TCNT2 = 0x00;
uint8_t OCR2A = 0x00;
uint8_t OCR2B = 0x00;

/* Mock ADC registers */
uint16_t ADC = 0x0000;
uint8_t ADCSRA = 0x00;
uint8_t ADMUX = 0x00;

/* Mock Interrupt registers */
uint8_t SREG = 0x00;
uint8_t EIMSK = 0x00;
uint8_t EICRA = 0x00;

/**
 * @brief Reset all mock registers to default state
 */
void mock_avr_io_reset(void) {
    PORTF = 0xFF; DDRF = 0x00;
    PORTD = 0xFF; DDRD = 0x00;
    PORTE = 0xFF; DDRE = 0x00;
    PORTA = 0xFF; DDRA = 0x00;
    PORTB = 0xFF; DDRB = 0x00;
    PORTC = 0xFF; DDRC = 0x00;
    
    TCCR0A = 0x00; TCCR0B = 0x00; TCNT0 = 0x00; OCR0A = 0x00; OCR0B = 0x00;
    TCCR1A = 0x00; TCCR1B = 0x00; TCNT1 = 0x0000; OCR1A = 0x0000; OCR1B = 0x0000;
    TCCR2A = 0x00; TCCR2B = 0x00; TCNT2 = 0x00; OCR2A = 0x00; OCR2B = 0x00;
    
    ADC = 0x0000; ADCSRA = 0x00; ADMUX = 0x00;
    SREG = 0x00; EIMSK = 0x00; EICRA = 0x00;
}

/**
 * @brief Get port register value for testing
 */
uint8_t mock_get_port(char port) {
    switch (port) {
        case 'A': return PORTA;
        case 'B': return PORTB;
        case 'C': return PORTC;
        case 'D': return PORTD;
        case 'E': return PORTE;
        case 'F': return PORTF;
        default: return 0x00;
    }
}

/**
 * @brief Set port register value (for testing)
 */
void mock_set_port(char port, uint8_t value) {
    switch (port) {
        case 'A': PORTA = value; break;
        case 'B': PORTB = value; break;
        case 'C': PORTC = value; break;
        case 'D': PORTD = value; break;
        case 'E': PORTE = value; break;
        case 'F': PORTF = value; break;
    }
}
