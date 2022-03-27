/** \file */

#include "TKLcs0.h"

/* ATTRIBUTES
 * ==========
 */

static volatile uint8_t pv_u8_lockCnt;

/* OPERATIONS
 * ==========
 */

void TKLcs0_enter(void)
{
    pv_u8_lockCnt++; /* Atomic operation (no concurrency issues); must be first
                      */
    TKLCS0CFG_DIS_INT();
    assert(0u != pv_u8_lockCnt); /* No rollover must occur */

    return;
}

void TKLcs0_exit(void)
{
    assert(0u != pv_u8_lockCnt); /* `cs0_enter()` must have been called
                                    previously */
    pv_u8_lockCnt--;
    if(0u == pv_u8_lockCnt)
    {
        TKLCS0CFG_ENA_INT();
    }

    return;
}
