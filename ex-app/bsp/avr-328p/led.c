/** \file */

#include "led.h"

/* OPERATIONS
 * ==========
 */

/* Type casts for safety (MISRA-required if bit positions are implemented as
 * `enum`s)
 */

void led_init(void) {
    DDRB |= 1u << (uint8_t)DDB5; /* Set PORTB5 as output (Arduino Uno LED "L") */
}

void led_on(void) {
    PORTB |= 1u << (uint8_t)PB5; /* PORTB5 on */
}

void led_off(void) {
    PORTB &= ~(1u << (uint8_t)PB5); /* PORTB5 off */
}

void led_toggle(void) {
    PORTB ^= 1u << (uint8_t)PB5; /* PORTB5 toggle */
}
