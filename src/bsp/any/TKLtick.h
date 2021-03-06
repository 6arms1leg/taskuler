/** \file */

#ifndef TKLTICK_H
#define TKLTICK_H

#include <stdint.h>

#include "TKLint.h"
#include "TKLtimer.h"

/* OPERATIONS
 * ==========
 */

/** \brief Initialize the Time Tick module */
void TKLtick_init(void);

/**
 * \brief Initialize the Time Tick module
 *
 * \return Current relative system time tick count
 */
uint32_t TKLtick_getTick(void);

/** \brief Increment the Time Tick module’s relative system time tick count */
void TKLtick_incrTick(void);

/**
 * \brief Reset the Time Tick module’s relative system time tick count
 *
 * This is useful to reset the relative system time tick count right before
 * starting the Taskuler scheduler for the first time.  This way, not all tasks
 * will be due to run at the very first scheduler algorithm execution cycle (if
 * the time tick count has progressed far enough), as this can cause
 * schedulability issues.
 */
void TKLtick_clrTick(void);

#endif /* TKLTICK_H */
