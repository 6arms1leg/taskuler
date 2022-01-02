/** \file */

#include "timeTick.h"

/* ATTRIBUTES
 * ==========
 */

/**
 * \brief Relative system time tick count
 *
 * Attribute is accessed from an ISR *and* "normal" code, hence `volatile`.
 */
static volatile uint32_t u32_pv_tickCount = (uint32_t)0U;

/* OPERATIONS
 * ==========
 */

void fn_tmTck_init(void)
{
    fn_tsklrTmr_init(); /* Call initialization of the MCU timer */

    return;
}

uint32_t fn_tmTck_getTickCount(void)
{
    TASKULERINTERRUPT_DISABLE /* Crit. region start (disable all ISRs) */
    const uint32_t u32_tickCount = u32_pv_tickCount;
    TASKULERINTERRUPT_ENABLE /* Crit. region end (enable all ISRs) */

    return(u32_tickCount);
}

void fn_tmTck_incrementTickCount(void)
{
    /* No crit. region needed, only called from rel. system time tick ISR */
    u32_pv_tickCount++;

    return;
}

void fn_tmTck_resetTickCount(void)
{
    TASKULERINTERRUPT_DISABLE /* Crit. region start (disable all ISRs) */
    u32_pv_tickCount = (uint32_t)0U;
    TASKULERINTERRUPT_ENABLE /* Crit. region end (enable all ISRs) */

    return;
}
