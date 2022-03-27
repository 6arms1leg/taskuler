/** \file */

#ifndef BLINK_H
#define BLINK_H

/* Include the LED driver interface */
#include "led.h"

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

#endif /* BLINK_H */
