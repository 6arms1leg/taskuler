/** \file */

#ifndef TKLINT_H
#define TKLINT_H

/* libc */
#include <stdbool.h>

/* Provides AVR MCU interrupt handling */
#include <avr/interrupt.h>

/**
 * \{
 * \brief Wrapper to abstract from MCU-/compiler-specific statements
 */
#define TKLINT_ENA() \
do                   \
{                    \
    sei();           \
}                    \
while(false)

#define TKLINT_DIS() \
do                   \
{                    \
    cli();           \
}                    \
while(false)
/** \} */

#endif /* TKLINT_H */
