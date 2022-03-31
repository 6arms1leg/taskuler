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
    assert(0u != pv_lockCnt); /* No rollover must occur */
}

void TKLcs0_exit(void) {
    assert(0u != pv_lockCnt); /* Crit. section must have been entered prev. */
    pv_lockCnt--;
    if (0u == pv_lockCnt) {
        TKLCS0CFG_ENA_INT();
    }
}
