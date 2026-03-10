/**
 * @file    ses_adc.c
 * @brief   ADC driver for reading analog sensors on ATmega32U4.
 *          This module provides initialization and access routines for
 *          analog sensors connected to the light, potentiometer, and 
 *          temperature input channels.
 */
/* INCLUDES ******************************************************************/

#include <avr/io.h>
#include "ses_adc.h"

/* DEFINES & MACROS **********************************************************/

// ADC wiring on SES board
#define ADC_DDR_INPUT                 DDRF
#define ADC_PORT_INPUT                PORTF

// Sensor analog input pins
#define ADC_LIGHT_PIN                 PF0
#define ADC_POTI_PIN                  PF6
#define ADC_TEMP_PIN                  PF7

// Power reduction register and bit for ADC
#define ADC_POWER_REDUCTION_REG       PRR0
#define ADC_POWER_REDUCTION_BIT       PRADC

// ADC prescaler setting: F_CPU / 128 = 125kHz (for 16MHz CPU)
#define ADC_PRESCALE_BITS             ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))

// ADC reference voltage settings
#define ADC_REF_EXTERNAL              ((0 << REFS1) | (0 << REFS0))  //External 3.3V
#define ADC_REF_AVCC                  ((0 << REFS1) | (1 << REFS0))  // AVcc with external capacitor
#define ADC_REF_INTERNAL_1V1          ((1 << REFS1) | (1 << REFS0))  // Internal 1.1V reference

// ADC result alignment
#define ADC_RIGHT_ADJUST              (0 << ADLAR)
#define ADC_LEFT_ADJUST               (1 << ADLAR)

/* ADC Enable Bit */
#define ADC_ENABLE                    (1 << ADEN)

/* ADC Start Conversion */
#define ADC_START_CONVERSION          (1 << ADSC)

// ADC control and register macros
#define ADC_REG_ADMUX                 ADMUX
#define ADC_REG_ADCSRA                ADCSRA

// ADC control bit macros
#define ADC_BIT_ADATE                 ADATE
#define ADC_BIT_ADEN                  ADEN
#define ADC_BIT_ADSC                  ADSC

/* ADC Conversion Complete Flag (used for polling) */
#define ADC_CONVERSION_RUNNING        (ADCSRA & (1 << ADSC))

// Raw ADC values and corresponding temperatures for calibration (tenths of °C)
#define ADC_TEMP_RAW_LOW              512     // ADC value at 10°C
#define ADC_TEMP_RAW_HIGH             682     // ADC value at 30°C
#define ADC_TEMP_LOW                  100     // 10.0°C = 100 tenths
#define ADC_TEMP_HIGH                 300     // 30.0°C = 300 tenths

/* FUNCTION DEFINITION *******************************************************/

/**
 * @brief Initializes ADC hardware with external AREF and prescaler = 128.
 *
 * Configures ADC channels for light, potentiometer, and temperature sensors.
 * Disables internal pull-ups and enables ADC peripheral.
 */
void adc_init(void) {
    // Set sensor pins as input
    ADC_DDR_INPUT &= ~((1 << ADC_LIGHT_PIN) | (1 << ADC_POTI_PIN) | (1 << ADC_TEMP_PIN));

    // Disable internal pull-ups
    ADC_PORT_INPUT &= ~((1 << ADC_LIGHT_PIN) | (1 << ADC_POTI_PIN) | (1 << ADC_TEMP_PIN));

    // Disable power reduction for ADC
    ADC_POWER_REDUCTION_REG &= ~(1 << ADC_POWER_REDUCTION_BIT);

    // Set reference voltage and result alignment
    ADC_REG_ADMUX = ADC_REF_EXTERNAL | ADC_RIGHT_ADJUST;

    // Set ADC clock prescaler
    ADC_REG_ADCSRA = ADC_PRESCALE_BITS;

    // Disable auto trigger
    ADC_REG_ADCSRA &= ~(1 << ADC_BIT_ADATE);

    // Enable ADC
    ADC_REG_ADCSRA |= (1 << ADC_BIT_ADEN);
}

/**
 * @brief Reads the analog value from the specified ADC channel.
 * 
 * @param adc_channel The ADC channel to read (0–7).
 * @return 10-bit ADC result; returns ADC_INVALID_CHANNEL if input is invalid.
 */
uint16_t adc_read(uint8_t adc_channel) {
    // Validate channel
    if (adc_channel != ADC_LIGHT_CH &&
        adc_channel != ADC_POTI_CH &&
        adc_channel != ADC_TEMP_CH) {
        return ADC_INVALID_CHANNEL;
    }
    // Select channel (preserving upper bits of ADMUX)
    ADC_REG_ADMUX = (ADC_REG_ADMUX & 0xF0) | (adc_channel & 0x0F);

    // Start conversion
    ADC_REG_ADCSRA |= (1 << ADC_BIT_ADSC);

    // Wait until conversion is complete
    while (ADC_REG_ADCSRA & (1 << ADC_BIT_ADSC));

    // Return 10-bit result
    return ADC;
}

/**
 * @brief Converts the ADC temperature sensor reading into tenths of °C.
 * 
 * Uses calibration values for linear interpolation between 10°C and 30°C.
 * 
 * @return Temperature in tenths of degrees Celsius.
 */
int16_t adc_getTemperature(void) {
    int32_t adc = adc_read(ADC_TEMP_CH);
    return (int16_t)(((adc- ADC_TEMP_RAW_LOW) * (ADC_TEMP_HIGH- ADC_TEMP_LOW))
     / (ADC_TEMP_RAW_HIGH- ADC_TEMP_RAW_LOW)) + ADC_TEMP_LOW;
}
