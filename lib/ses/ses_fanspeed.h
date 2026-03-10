#ifndef SES_FANSPEED_H_
#define SES_FANSPEED_H_

/*INCLUDES *******************************************************************/

#include <stdint.h>

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes the fan speed measurement module.
 */
void fanspeed_init(void);

/**
 * Gets the most recent RPM measurement.
 */
uint16_t fanspeed_getRecent(void);

/**
 * Gets the filtered RPM using a median filter.
 */
uint16_t fanspeed_getFiltered(void);
#endif /* SES_FANSPEED_H_ */
