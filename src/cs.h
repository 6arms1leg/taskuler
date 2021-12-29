#ifndef CS_H
#define CS_H

/* libc
 *
 * `"` used intentionally.  This allows the user to override and provide his
 * own type definitions before falling back to libc.
 */
#include "stdint.h"
#include "assert.h"

/** \brief User-provided macros to dis-/enable all relevant interrupts */
#include "cs_disEnaInts.h"

/* OPERATIONS
 * ==========
 */

/** \brief Mark start of critical section */
void csEntry(void);

/** \brief Mark end of critical section */
void csExit(void);

#endif /* CS_H */
