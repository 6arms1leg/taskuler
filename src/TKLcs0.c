/** \file */

#include "TKLcs0.h"

/* ATTRIBUTES
 * ==========
 */

static volatile uint8_t pv_u8_lockCnt;

/* OPERATIONS
 * ==========
 */

void cs0_enter(void)
{
    pv_u8_lockCnt++; /* Atomic operation (no concurrency issues); must be first
                      */
    CS0_DISINTS();
    assert(0u != pv_u8_lockCnt); /* No rollover must occur */

    return;
}

void cs0_exit(void)
{
    assert(0u != pv_u8_lockCnt); /* `cs0_enter()` must have been called
                                    previously */
    pv_u8_lockCnt--;
    if(0u == pv_u8_lockCnt)
    {
        CS0_ENAINTS();
    }

    return;
}