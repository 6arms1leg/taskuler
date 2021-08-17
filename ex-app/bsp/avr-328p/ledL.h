/** \file */

#ifndef INCLUDE_LEDL_H
#define INCLUDE_LEDL_H

#include <stdint.h>
#include <avr/io.h> /* Provides easy AVR MCU register access */

/* OPERATIONS
 * ==========
 */

/** \brief Initialize the Arduino Uno LED "L" */
void fn_ledL_init(void);

/** \brief Turn the LED on */
void fn_ledL_on(void);

/** \brief Turn the LED off */
void fn_ledL_off(void);

/** \brief Toggle LED status from "off" to "on" and vice versa */
void fn_ledL_toggle(void);

#endif /* INCLUDE_LEDL_H */
