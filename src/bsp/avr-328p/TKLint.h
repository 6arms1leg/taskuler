/** \file */

#ifndef TKLINT_H
#define TKLINT_H

#include <stdbool.h>

#include <avr/interrupt.h> /* Provides AVR MCU interrupt handling */

/**
 * \{
 * \brief Wrapper to abstract from MCU-/compiler-specific statements
 */
#define TKLINT_ENA() sei()
#define TKLINT_DIS() cli()
/** \} */

#endif /* TKLINT_H */
