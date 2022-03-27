/** \file */

#include "ledL.h"

/* OPERATIONS
 * ==========
 */

void fn_ledL_init(void)
{
    DDRB |= 1u<<DDB5; /* Set PORTB5 as output (Arduino Uno LED "L") */

    return;
}

void fn_ledL_on(void)
{
    PORTB |= 1u<<PB5; /* PORTB5 on */

    return;
}

void fn_ledL_off(void)
{
    PORTB &= ~( 1u<<PB5 ); /* PORTB5 off */

    return;
}

void fn_ledL_toggle(void)
{
    PORTB ^= 1u<<PB5; /* PORTB5 toggle */

    return;
}
