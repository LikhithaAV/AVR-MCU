/**
 * @file mock_avr_io.h
 * @brief Mock AVR I/O definitions for testing on native platform
 * 
 * This file provides mock definitions for AVR hardware registers
 * allowing tests to run on native x86/x64 platform without AVR toolchain.
 */

#ifndef MOCK_AVR_IO_H
#define MOCK_AVR_IO_H

#include <stdint.h>

/* Mock PORT definitions - These will be tracked in memory */
extern uint8_t PORTF;
extern uint8_t DDRF;
extern uint8_t PORF;

extern uint8_t PORTD;
extern uint8_t DDRD;
extern uint8_t PORD;

extern uint8_t PORTE;
extern uint8_t DDRE;
extern uint8_t PORE;

extern uint8_t PORTA;
extern uint8_t DDRA;
extern uint8_t PORA;

extern uint8_t PORTB;
extern uint8_t DDRB;
extern uint8_t PORB;

extern uint8_t PORTC;
extern uint8_t DDRC;
extern uint8_t PORC;

/* Mock Timer/Counter registers */
extern uint8_t TCCR0A;
extern uint8_t TCCR0B;
extern uint8_t TCNT0;
extern uint8_t OCR0A;
extern uint8_t OCR0B;

extern uint8_t TCCR1A;
extern uint8_t TCCR1B;
extern uint16_t TCNT1;
extern uint16_t OCR1A;
extern uint16_t OCR1B;

extern uint8_t TCCR2A;
extern uint8_t TCCR2B;
extern uint8_t TCNT2;
extern uint8_t OCR2A;
extern uint8_t OCR2B;

/* Mock ADC registers */
extern uint16_t ADC;
extern uint8_t ADCSRA;
extern uint8_t ADMUX;

/* Mock Interrupt registers */
extern uint8_t SREG;
extern uint8_t EIMSK;
extern uint8_t EICRA;

/* Helper function to reset all mock registers */
void mock_avr_io_reset(void);

/* Helper function to get register values for testing */
uint8_t mock_get_port(char port);
void mock_set_port(char port, uint8_t value);

#endif /* MOCK_AVR_IO_H */
