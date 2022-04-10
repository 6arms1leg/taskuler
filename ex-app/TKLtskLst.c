/** \file */

#include "TKLtskLst.h"

#include "TKLtsk.h"

#define TSK_CNT (sizeof(pv_tskLst) / sizeof(*pv_tskLst)) /* Num. of tasks in
                                                            task list */

/* ATTRIBUTES
 * ==========
 */

/**
 * \brief Private task (cfg.) list
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
     .lastRun = TKLTIMER_0S - TKLTIMER_1S, /* Offset 1 s */
     .p_tskRunner = &TKLtsk_blinkRunner},
    /* Tsk 1 */
    {.active = true,
     .period = 2u * TKLTIMER_1S,
     .deadline = 250u * TKLTIMER_1MS,
     .lastRun = TKLTIMER_0S, /* Offset 2 s */
     .p_tskRunner = &TKLtsk_blinkRunner}
};

/* Global opaque pointer to task (cfg.) list */
TKLtyp_tsk_t* const TKLtskLst_p_tskLst = pv_tskLst;

/* Private num. of tasks in task list */
static const uint8_t pv_tskCnt = (uint8_t)TSK_CNT; /* Explicit type cast needed
                                                      (safe here) */

/* Global opaque pointer to num. of tasks in task list */
const uint8_t* const TKLtskLst_p_tskCnt = &pv_tskCnt;
