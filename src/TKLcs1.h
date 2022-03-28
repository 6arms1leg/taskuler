/** \file */

#ifndef TKLCS1_H
#define TKLCS1_H

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
#define TKLCS1_INIT() TKLcs1Cfg_intReg_t intStatus_

/**
 * \brief Mark start of critical section
 *
 * Critical sections must have been initialized previously in current scope.
 */
#define TKLCS1_ENTER()                     \
do                                         \
{                                          \
    /* Save current interrupt status */    \
    intStatus_ = TKLCS1CFG_GET_INT_STATUS; \
                                           \
    /* Disable/Mask/Block interrupts */    \
    TKLCS1CFG_DIS_INT;                     \
}                                          \
while(false)

/**
 * \brief Mark end of critical section
 *
 * Restore interrupt status from before critical section start.
 * A critical section must have been entered previously in current scope.
 */
#define TKLCS1_EXIT() TKLCS1CFG_ENA_INT_REG = intStatus_

#endif /* TKLCS1_H */
