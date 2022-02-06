/** \file */

#ifndef CS0_H
#define CS0_H

/* libc
 *
 * `"` used intentionally.  This allows the user to override and provide his
 * own type definitions before falling back to libc.
 */
#include "stdint.h"
#include "assert.h"

/** \brief User-provided macros to dis-/enable all relevant interrupts */
#include "cs0_conf.h"

/* OPERATIONS
 * ==========
 */

/** \brief Mark start of critical section */
void cs0_enter(void);

/** \brief Mark end of critical section */
void cs0_exit(void);

#endif /* CS0_H */
