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
#define U32_TASKULERTIMER_0S (0u)
#define U32_TASKULERTIMER_1MS (1u)
#define U32_TASKULERTIMER_2MS (2u)
#define U32_TASKULERTIMER_10MS (10u)
#define U32_TASKULERTIMER_25MS (25u)
#define U32_TASKULERTIMER_100MS (100u)
#define U32_TASKULERTIMER_250MS (250u)
#define U32_TASKULERTIMER_1S (1000u)
/** \} */

/* OPERATIONS
 * ==========
 */

/** \brief Initialize Arduino Uno’s Timer0 */
void fn_tsklrTmr_init(void);

#endif /* TKLTIMER_H */
