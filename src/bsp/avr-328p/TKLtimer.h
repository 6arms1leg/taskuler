/** \file */

#ifndef TKLTIMER_H
#define TKLTIMER_H

/* libc */
#include <stdint.h>
#include <avr/io.h> /* Provides easy AVR MCU register access */

/* Include Taskuler BSP interfaces */
#include "TKLtick.h"

/**
 * \{
 * \brief Conversion: timeTick to time
 *
 * *1 timeTick = 1e-3 s (= 1 ms) => 1e3 = 1 s*  
 * Mind max. value of data type!
 */
#define TKLTIMER_0S (0u)
#define TKLTIMER_1MS (1u)
#define TKLTIMER_2MS (2u)
#define TKLTIMER_10MS (10u)
#define TKLTIMER_25MS (25u)
#define TKLTIMER_100MS (100u)
#define TKLTIMER_250MS (250u)
#define TKLTIMER_1S (1000u)
/** \} */

/* OPERATIONS
 * ==========
 */

/** \brief Initialize Arduino Uno’s Timer0 */
void TKLtimer_init(void);

#endif /* TKLTIMER_H */
