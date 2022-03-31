/** \file */

#include "TKLtskLst.h"

#include "TKLtsk.h"

/* ATTRIBUTES
 * ==========
 */

/**
 * \brief Task (config.) list array
 *
 * Holds task’s
 * * active status (`true`/`false`)
 * * period (not `0`!)
 * * deadline (not `0`!)
 * * offset (last run)
 * * function pointer to exec. (task runner)
 */
static TKLtyp_tsk_t pv_tskLst[] = {
    /* Tsk 0 */
    {.active = true,
     .period = 2u * TKLTIMER_1S,
     .deadline = 250u * TKLTIMER_1MS,
     .lastRun = TKLTIMER_0S - TKLTIMER_1S,
     .p_tskRunner = &TKLtsk_blinkRunner},
    /* Tsk 1 */
    {.active = true,
     .period = 2u * TKLTIMER_1S,
     .deadline = 250u * TKLTIMER_1MS,
     .lastRun = TKLTIMER_0S,
     .p_tskRunner = &TKLtsk_blinkRunner}
};

/* OPERATIONS
 * ==========
 */

TKLtyp_tsk_t* TKLtskLst_getTskLst(void) {
    return (pv_tskLst);
}

uint8_t TKLtskLst_cntTsk(void) {
    return ((uint8_t)(sizeof(pv_tskLst) / sizeof(*pv_tskLst)));
}
