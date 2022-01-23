/** \file */

#ifndef INCLUDE_TIMETICK_H
#define INCLUDE_TIMETICK_H

/* libc */
#include <stdint.h>

/* Include Taskuler BSP interfaces */
#include "taskulerInterrupt.h"
#include "taskulerTimer.h"

/* OPERATIONS
 * ==========
 */

/** \brief Initialize the Time Tick module */
void fn_tmTck_init(void);

/**
 * \brief Initialize the Time Tick module
 *
 * \return Current relative system time tick count
 */
uint32_t fn_tmTck_getTickCount(void);

/** \brief Increment the Time Tick module’s relative system time tick count */
void fn_tmTck_incrementTickCount(void);

/**
 * \brief Reset the Time Tick module’s relative system time tick count
 *
 * This is useful to reset the relative system time tick count right before
 * starting the Taskuler scheduler for the first time.  This way, not all tasks
 * will be due to run at the very first scheduler algorithm execution cycle (if
 * the time tick count has progressed far enough), as this can cause
 * schedulability issues.
 */
void fn_tmTck_resetTickCount(void);

#endif /* INCLUDE_TIMETICK_H */
