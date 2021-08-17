/** \file */

#ifndef INCLUDE_TASKULERINTERRUPT_H
#define INCLUDE_TASKULERINTERRUPT_H

/* Provides AVR MCU interrupt handling */
#include <avr/interrupt.h>

/**
 * \{
 * \brief Wrapper to abstract from MCU-/compiler-specific statements
 */
#define TASKULERINTERRUPT_ENABLE sei();
#define TASKULERINTERRUPT_DISABLE cli();
/** \} */

#endif /* INCLUDE_TASKULERINTERRUPT_H */
