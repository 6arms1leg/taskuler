/** \file */

#ifndef LED_H
#define LED_H

#include <stdint.h>

#include <avr/io.h> /* Provides easy AVR MCU register access */

/* OPERATIONS
 * ==========
 */

/** \brief Init. Arduino Uno LED "L" */
void led_init(void);

/** \brief Turn LED on */
void led_on(void);

/** \brief Turn LED off */
void led_off(void);

/** \brief Toggle LED status from "off" to "on" and vice versa */
void led_toggle(void);

#endif /* LED_H */
