/** \file */

#include "TKLsdlr.h"

/* ATTRIBUTES
 * ==========
 */

static TKLtyp_p_getTick_t pv_p_getTick;
    /**< \brief Pointer to access function that provides the current relative
         system time tick count */
static TKLtyp_tsk_t* volatile pv_p_tskLst; /**< \brief Registered task list */
static volatile uint8_t pv_tskCnt; /**< \brief Number of tasks within
                                        registered task list */
static volatile uint8_t pv_tskOverrunCnt; /**< \brief Task deadline overrun
                                               counter */

/* OPERATIONS
 * ==========
 */

void TKLsdlr_setTickSrc(const TKLtyp_p_getTick_t p_getTick)
{
    pv_p_getTick = p_getTick;

    return;
}

void TKLsdlr_setTskLst(TKLtyp_tsk_t* const p_tskLst, const uint8_t tskCnt)
{
    pv_p_tskLst = p_tskLst;
    pv_tskCnt = tskCnt;

    return;
}

TKLtyp_tsk_t* TKLsdlr_getTskLst(void)
{
    return(pv_p_tskLst);
}

uint8_t TKLsdlr_cntTsk(void)
{
    return(pv_tskCnt);
}

uint8_t TKLsdlr_cntTskOverrun(void)
{
    return(pv_tskOverrunCnt);
}

void TKLsdlr_clrTskOverrun(void)
{
    pv_tskOverrunCnt = 0u;

    return;
}

void TKLsdlr_setTskAct(const TKLtyp_p_tskRunner_t p_tskRunner,
                       const bool active,
                       const bool updLastRun)
{
    /* Set pointer to task list’s list */
    TKLtyp_tsk_t* const p_tskLst = pv_p_tskLst;

    /* Number of tasks in task list */
    const uint8_t tskCnt = pv_tskCnt;

    /* Find all tasks (matching function pointer) and set them to on/off
     * (`true`/`false`)
     */
    for(uint8_t i = 0u; tskCnt > i; i++)
    {
        if( *p_tskRunner == (*p_tskLst[i].p_tskRunner) )
        {
            p_tskLst[i].active = active;

            /* Update time stamp of last task run, if requested */
            if(true == updLastRun)
            {
                p_tskLst[i].lastRun = (*pv_p_getTick)();
            }
        } /* if(...) */
    } /* for(...) */

    return;
}

void TKLsdlr_exec(void)
{
    /* Set pointer to task list’s list */
    TKLtyp_tsk_t* const p_tskLst = pv_p_tskLst;

    /* Number of tasks in task list */
    const uint8_t tskCnt = pv_tskCnt;

    /* Get current time tick count */
    const uint32_t tickCnt = (*pv_p_getTick)();

    /* Loop through all tasks in task list.
     * During one full loop ("cycle")
     * * run tasks only if they are due to run (according to their period) and
     *   enabled,
     * * ignore disabled tasks (but still update their `lastRun` time),
     * * check for task deadline overrun and keep count,
     * * if a task was run, end the cycle.
     */
    for(uint8_t i = 0u; i < tskCnt; i++)
    {
        /* Check if new execution period for task has started
         * (this is still correct on time tick rollover)
         */
        if( (tickCnt - p_tskLst[i].lastRun) >= p_tskLst[i].period )
        {
            /* Save (ideal) time of when task was "ready-to-run" */
            p_tskLst[i].lastRun =
                tickCnt - ( (tickCnt - p_tskLst[i].lastRun) % p_tskLst[i].period );

            /* Check if task is enabled */
            if(true == p_tskLst[i].active)
            {
                /* Run periodic task */
                (*p_tskLst[i].p_tskRunner)();

                /* Check for task deadline overrun
                 * (this is still correct on time tick rollover)
                 */
                if( ( (*pv_p_getTick)() - p_tskLst[i].lastRun )
                    > p_tskLst[i].deadline )
                {
                    /* Increment task deadline overrun counter */
                    pv_tskOverrunCnt++;

                    /* Run custom task deadline overrun hook, if defined */
                    TKLSDLRCFG_OVERRUN_HOOK( p_tskLst[i].p_tskRunner);
                }

                /* End cycle to allow starting a new one as soon as possible.
                 * This gives better schedulability.
                 */
                break;
            } /* if(...) */
        } /* if(...) */
    } /* for(...) */

    return;
}
