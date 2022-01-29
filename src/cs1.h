#ifndef CS1_H
#define CS1_H

/* libc
 *
 * `"` used intentionally.  This allows the user to override and provide his
 * own type definitions before falling back to libc.
 */
#include "stdbool.h"

/** \brief User-provided macros to configure (save/disable/restore) interrupts */
#include "cs1_conf.h"

/* OPERATIONS
 * ==========
 */

/**
 * \brief Mark start of critical section
 *
 * Define local variable to store interrupt status.
 * This must be called exactly once in each local (function) scope before the
 * start of the first critical section.
 */
#define CS1_INIT() cs1_intReg_t intReg_intStatus_

/**
 * \brief Mark start of critical section
 *
 * Critical sections must have been initialized previously in current scope.
 */
#define CS1_ENTER()                       \
do                                        \
{                                         \
    /* Save current interrupt status */   \
    intReg_intStatus_ = CS1_GETINTSTATUS; \
                                          \
    /* Disable/Mask/Block interrupts */   \
    CS1_DEACTINT;                         \
}                                         \
while(false)

/**
 * \brief Mark end of critical section
 *
 * Restore interrupt status from before critical section start.
 * A critical section must have been entered previously in current scope.
 */
#define CS1_EXIT() CS1_ACTINTREG = intReg_intStatus_

#endif /* CS1_H */
