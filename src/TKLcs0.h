/** \file */

#ifndef TKLCS0_H
#define TKLCS0_H

/* `"` used intentionally.  This allows the user to override and provide his
   own implementation before falling back to libc. */
#include "stdint.h"
#include "assert.h"

/** \brief User-provided macros to dis-/enable all relevant interrupts */
#include "TKLcs0Cfg.h"

/* OPERATIONS
 * ==========
 */

/** \brief Mark start of critical section */
void TKLcs0_enter(void);

/** \brief Mark end of critical section */
void TKLcs0_exit(void);

#endif /* TKLCS0_H */
