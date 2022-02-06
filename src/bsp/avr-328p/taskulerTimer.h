/** \file */

#ifndef TASKULERTIMER_H
#define TASKULERTIMER_H

/* libc */
#include <stdint.h>
#include <avr/io.h> /* Provides easy AVR MCU register access */

/* Include Taskuler BSP interfaces */
#include "timeTick.h"

/**
 * \{
 * \brief Conversion: timeTick to time
 *
 * *1 timeTick = 1e-3 s (= 1 ms) => 1e3 = 1 s*  
 * Mind max. value of data type!
 */
#define U32_TASKULERTIMER_0S ( (uint32_t)0U )
#define U32_TASKULERTIMER_1MS ( (uint32_t)1U )
#define U32_TASKULERTIMER_2MS ( (uint32_t)2U )
#define U32_TASKULERTIMER_10MS ( (uint32_t)10U )
#define U32_TASKULERTIMER_25MS ( (uint32_t)25U )
#define U32_TASKULERTIMER_100MS ( (uint32_t)100U )
#define U32_TASKULERTIMER_250MS ( (uint32_t)250U )
#define U32_TASKULERTIMER_1S ( (uint32_t)1000U )
/** \} */

/* OPERATIONS
 * ==========
 */

/** \brief Initialize Arduino Uno’s Timer0 */
void fn_tsklrTmr_init(void);

#endif /* TASKULERTIMER_H */
