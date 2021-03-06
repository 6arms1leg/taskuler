/** \file */

#include "led.h"

/* OPERATIONS
 * ==========
 */

void led_init(void) {
    DDRB |= 1u << DDB5; /* Set PORTB5 as output (Arduino Uno LED "L") */
}

void led_on(void) {
    PORTB |= 1u << PB5; /* PORTB5 on */
}

void led_off(void) {
    PORTB &= ~(1u << PB5); /* PORTB5 off */
}

void led_toggle(void) {
    PORTB ^= 1u << PB5; /* PORTB5 toggle */
}
