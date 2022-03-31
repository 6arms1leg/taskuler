/** \file */

#ifndef TKLTIMER_H
#define TKLTIMER_H

#include <stdint.h>

#include <avr/io.h> /* Provides easy AVR MCU register access */

#include "TKLtick.h"

/**
 * \{
 * \brief Conversion: tick to time
 *
 * `1 tick = 1e-3 s => 1e3 = 1 s`  
 * Mind max. value of data type!
 */
#define TKLTIMER_0S 0u
#define TKLTIMER_1MS 1u
#define TKLTIMER_1S 1000u
/** \} */

/* OPERATIONS
 * ==========
 */

/** \brief Initialize Arduino Uno’s Timer0 */
void TKLtimer_init(void);

#endif /* TKLTIMER_H */
