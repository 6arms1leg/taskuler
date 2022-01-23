#ifndef CS_DISENAINTS_H
#define CS_DISENAINTS_H

/* libc */
#include <stdbool.h>

/* Include required interfaces here, e.g. `#include <interruptInterface.h>` */
#include <avr/interrupt.h> /* Provides AVR MCU interrupt handling */

/**
 * \brief User-defined macros to dis-/enable all relevant interrupts
 *
 * \{
 */
#define CS_DISINTS()                               \
do                                                 \
{ /* Add code to disable relevant interrupts here, \
   * e.g. `disableAllInterrupts();`                \
   */                                              \
    cli();                                         \
}                                                  \
while(false)

#define CS_ENAINTS()                              \
do                                                \
{ /* Add code to enable relevant interrupts here, \
   * e.g. `enableAllInterrupts();`                \
   */                                             \
    sei();                                        \
}                                                 \
while(false)
/** \} */

#endif /* CS_DISENAINTS_H */
