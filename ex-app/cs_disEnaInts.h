#ifndef CS_DISENAINTS_H
#define CS_DISENAINTS_H

/* Include required interfaces here, e.g. `#include <interruptInterface.h>` */
#include <avr/interrupt.h> /* Provides AVR MCU interrupt handling */

/**
 * \brief User-defined macros to dis-/enable all relevant interrupts
 *
 * \{
 */
#define CS_DISINTS cli(); /* Add code to disable relevant interrupts here, e.g.
                             `disableAllInterrupts();` */
#define CS_ENAINTS sei(); /* Add code to enable relevant interrupts here, e.g.
                             `enableAllInterrupts();` */
/** \} */

#endif /* CS_DISENAINTS_H */
