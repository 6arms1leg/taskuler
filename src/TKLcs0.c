/** \file */

#include "TKLcs0.h"

/* ATTRIBUTES
 * ==========
 */

static volatile uint8_t pv_lockCnt;

/* OPERATIONS
 * ==========
 */

void TKLcs0_enter(void) {
    pv_lockCnt++; /* Atomic operation (no concurrency issues); must be first */
    TKLCS0CFG_DIS_INT();
    assert(0u != pv_lockCnt); /* Sanity check (Design by Contract); no rollover
                                 must occur */
}

void TKLcs0_exit(void) {
    assert(0u != pv_lockCnt); /* Sanity check (Design by Contract); crit.
                                 section must have been entered prev. */
    pv_lockCnt--;
    if (0u == pv_lockCnt) {
        TKLCS0CFG_ENA_INT();
    }
}
