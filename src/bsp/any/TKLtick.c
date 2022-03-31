/** \file */

#include "TKLtick.h"

/* ATTRIBUTES
 * ==========
 */

/**
 * \brief Relative system time tick count
 *
 * Attribute is accessed from ISR *and* "normal" code, hence `volatile`.
 */
static volatile uint32_t pv_tickCnt;

/* OPERATIONS
 * ==========
 */

void TKLtick_init(void) {
    TKLtimer_init(); /* Call init. of MCU timer */
}

uint32_t TKLtick_getTick(void) {
    TKLINT_DIS();
    const uint32_t tickCnt = pv_tickCnt;
    TKLINT_ENA();

    return(tickCnt);
}

void TKLtick_incrTick(void) {
    pv_tickCnt++; /* No crit. region needed, only called from ISR */
}

void TKLtick_clrTick(void) {
    TKLINT_DIS();
    pv_tickCnt = 0u;
    TKLINT_ENA();
}
