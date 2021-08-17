/** \file */

#ifndef INCLUDE_LEDBLINK_H
#define INCLUDE_LEDBLINK_H

/* Include the LED driver interface */
#include "ledL.h"

/* OPERATIONS
 * ==========
 */

/**
 * \brief Blink the LED
 *
 * Essentially, this toggles the LED status from "off" to "on" and vice versa.
 * Calling it twice gives the blink effect.
 */
void fn_ledBlk_blinkLedL(void);

#endif /* INCLUDE_LEDBLINK_H */
