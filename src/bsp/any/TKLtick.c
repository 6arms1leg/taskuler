/** \file */

#include "TKLtick.h"

/* ATTRIBUTES
 * ==========
 */

/**
 * \brief Relative system time tick count
 *
 * Attribute is accessed from an ISR *and* "normal" code, hence `volatile`.
 */
static volatile uint32_t u32_pv_tickCount;

/* OPERATIONS
 * ==========
 */

void TKLtick_init(void)
{
    TKLtimer_init(); /* Call initialization of the MCU timer */

    return;
}

uint32_t TKLtick_getTick(void)
{
    TASKULERINTERRUPT_DISABLE(); /* Crit. region start (disable all ISRs) */
    const uint32_t u32_tickCount = u32_pv_tickCount;
    TASKULERINTERRUPT_ENABLE(); /* Crit. region end (enable all ISRs) */

    return(u32_tickCount);
}

void TKLtick_incrTick(void)
{
    /* No crit. region needed, only called from rel. system time tick ISR */
    u32_pv_tickCount++;

    return;
}

void TKLtick_clrTick(void)
{
    TASKULERINTERRUPT_DISABLE(); /* Crit. region start (disable all ISRs) */
    u32_pv_tickCount = 0u;
    TASKULERINTERRUPT_ENABLE(); /* Crit. region end (enable all ISRs) */

    return;
}
