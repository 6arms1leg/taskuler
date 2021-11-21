#include "cs.h"

/* ATTRIBUTES
 * ==========
 */

static uint8_t pv_u8_lockCnt = 0U;

/* OPERATIONS
 * ==========
 */

void csEntry(void)
{
    pv_u8_lockCnt++; /* Atomic operation (no concurrency issues); must be first
                      */
    CS_DISINTS
    assert(0U != pv_u8_lockCnt); /* No rollover must occur */

    return;
}

void csExit(void)
{
    assert(0U != pv_u8_lockCnt); /* `csEntry()` must have been called
                                    previously */
    pv_u8_lockCnt--;
    if(0U == pv_u8_lockCnt)
    {
        CS_ENAINTS
    }
    else {}; /* Do nothing */

    return;
}
