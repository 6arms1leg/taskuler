/** \file */

#include "TKLsdlr.h"

/* ATTRIBUTES
 * ==========
 */

/** \brief Pointer to function that provides curr. rel. sys. time tick */
static TKLtyp_p_getTick_t pv_p_getTick;

/** \brief Registered task list */
static TKLtyp_tsk_t* volatile pv_p_tskLst;

 /** \brief Number of tasks within registered task list */
static volatile uint8_t pv_tskCnt;

 /** \brief Task deadline overrun counter */
static volatile uint8_t pv_tskOverrunCnt;

/* OPERATIONS
 * ==========
 */

void TKLsdlr_setTickSrc(const TKLtyp_p_getTick_t p_getTick) {
    pv_p_getTick = p_getTick;
}

void TKLsdlr_setTskLst(TKLtyp_tsk_t* const p_tskLst, const uint8_t tskCnt) {
    pv_p_tskLst = p_tskLst;
    pv_tskCnt = tskCnt;
}

TKLtyp_tsk_t* TKLsdlr_getTskLst(void) {
    return (pv_p_tskLst);
}

uint8_t TKLsdlr_cntTsk(void) {
    return (pv_tskCnt);
}

uint8_t TKLsdlr_cntTskOverrun(void) {
    return (pv_tskOverrunCnt);
}

void TKLsdlr_clrTskOverrun(void) {
    pv_tskOverrunCnt = 0u;
}

void TKLsdlr_setTskAct(const TKLtyp_p_tskRunner_t p_tskRunner,
                       const bool active,
                       const bool updLastRun) {
    TKLtyp_tsk_t* const p_tskLst = pv_p_tskLst; /* Set ptr. to task list */
    const uint8_t tskCnt = pv_tskCnt; /* Number of tasks in task list */

    /* Find all tasks (matching function ptr.) and set them to "on"/"off" */
    for (uint8_t i = 0u; tskCnt > i; i++) {
        if (*p_tskRunner == (*p_tskLst[i].p_tskRunner)) { /* Task runner match? */
            p_tskLst[i].active = active;

            if (true == updLastRun) { /* Update last run? */
                p_tskLst[i].lastRun = (*pv_p_getTick)(); /* Update time stamp */
            }
        }
    } /* for (...) */
}

void TKLsdlr_exec(void) {
    TKLtyp_tsk_t* const p_tskLst = pv_p_tskLst; /* Set ptr. to task list */
    const uint8_t tskCnt = pv_tskCnt; /* Number of tasks in task list */
    const uint32_t tickCnt = (*pv_p_getTick)(); /* Get curr. tick count */

    /* Loop through all tasks in task list.
       During one full loop ("cycle"):
       * Run tasks only if due to run (according to period) and enabled
       * Ignore disabled tasks (but still update `lastRun` time)
       * Check for task deadline overrun and keep count
       * If a task was run, end cycle */
    for (uint8_t i = 0u; i < tskCnt; i++) {
        /* Check if new execution period for task has started
           (still correct on tick count rollover) */
        if (tickCnt - p_tskLst[i].lastRun >= p_tskLst[i].period) {
            /* Save (ideal) time of when task was "ready-to-run" */
            p_tskLst[i].lastRun =
                tickCnt - ((tickCnt - p_tskLst[i].lastRun) % p_tskLst[i].period);

            if (true == p_tskLst[i].active) { /* Task enabled? */
                (*p_tskLst[i].p_tskRunner)(); /* Run periodic task */

                /* Check for task deadline overrun (still correct on time tick
                   rollover) */
                if ((*pv_p_getTick)() - p_tskLst[i].lastRun >
                    p_tskLst[i].deadline) {
                    if (UCHAR_MAX > pv_tskOverrunCnt) { /* Counter unsaturated? */
                        pv_tskOverrunCnt++; /* Incr. deadline overrun counter */
                    }

                    /* Run custom deadline overrun hook, if defined */
                    TKLSDLRCFG_OVERRUN_HOOK(p_tskLst[i].p_tskRunner);
                }

                break; /* End cycle to allow starting new one as soon as
                          possible (gives better schedulability) */
            }
        } /* if (...) */
    } /* for (...) */
}
