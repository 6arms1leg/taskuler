#include "cs.h"

/* ATTRIBUTES
 * ==========
 */

static volatile uint8_t pv_u8_lockCnt = 0U;

/* OPERATIONS
 * ==========
 */

void cs_entry(void)
{
    pv_u8_lockCnt++; /* Atomic operation (no concurrency issues); must be first
                      */
    CS_DISINTS
    assert(0U != pv_u8_lockCnt); /* No rollover must occur */

    return;
}

void cs_exit(void)
{
    assert(0U != pv_u8_lockCnt); /* `cs_entry()` must have been called
                                    previously */
    pv_u8_lockCnt--;
    if(0U == pv_u8_lockCnt)
    {
        CS_ENAINTS
    }

    return;
}
