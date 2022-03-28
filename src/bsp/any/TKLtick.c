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
static volatile uint32_t pv_tickCnt;

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
    TKLINT_DIS(); /* Crit. region start (disable all ISRs) */
    const uint32_t tickCnt = pv_tickCnt;
    TKLINT_ENA(); /* Crit. region end (enable all ISRs) */

    return(tickCnt);
}

void TKLtick_incrTick(void)
{
    /* No crit. region needed, only called from rel. system time tick ISR */
    pv_tickCnt++;

    return;
}

void TKLtick_clrTick(void)
{
    TKLINT_DIS(); /* Crit. region start (disable all ISRs) */
    pv_tickCnt = 0u;
    TKLINT_ENA(); /* Crit. region end (enable all ISRs) */

    return;
}
