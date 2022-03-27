/** \file */

#ifndef TKLCS0CFG_H
#define TKLCS0CFG_H

/* libc */
#include <stdbool.h>

/* Include required interfaces here, e.g. `#include <interruptInterface.h>` */
#include <avr/interrupt.h> /* Provides AVR MCU interrupt handling */

/**
 * \brief User-defined macros to dis-/enable all relevant interrupts
 *
 * \{
 */
#define TKLCS0CFG_DIS_INT()                        \
do                                                 \
{ /* Add code to disable relevant interrupts here, \
   * e.g. `disableAllInterrupts();`                \
   */                                              \
    cli();                                         \
}                                                  \
while(false)

#define TKLCS0CFG_ENA_INT()                       \
do                                                \
{ /* Add code to enable relevant interrupts here, \
   * e.g. `enableAllInterrupts();`                \
   */                                             \
    sei();                                        \
}                                                 \
while(false)
/** \} */

#endif /* TKLCS0CFG_H */
