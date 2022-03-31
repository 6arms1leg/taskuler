/** \file */

#ifndef TKLCS1_H
#define TKLCS1_H

/* `"` used intentionally.  This allows the user to override and provide his
   own implementation before falling back to libc. */
#include "stdbool.h"

/** \brief User-provided macros to config. (save/disable/restore) interrupts */
#include "TKLcs1Cfg.h"

/**
 * \brief Mark start of critical section
 *
 * Define local variable to store interrupt status.
 * Must be called exactly once in each local (function) scope before start of
 * first critical section.
 */
#define TKLCS1_INIT() TKLcs1Cfg_intReg_t intStatus_

/**
 * \brief Mark start of critical section
 *
 * Critical sections must have been initialized previously in current scope.
 */
#define TKLCS1_ENTER()                                                  \
do {                                                                    \
    intStatus_ = TKLCS1CFG_GET_INT_STATUS; /* Save curr. int. status */ \
                                                                        \
    TKLCS1CFG_DIS_INT; /* Disable/Mask/Block ints. */                   \
} while (false)

/**
 * \brief Mark end of critical section
 *
 * Restore interrupt status from before critical section start.
 * Critical section must have been entered previously in current scope.
 */
#define TKLCS1_EXIT() TKLCS1CFG_ENA_INT_REG = intStatus_

#endif /* TKLCS1_H */
