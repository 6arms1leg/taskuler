/** \file */

#include "TKLsdlr.h"

/* ATTRIBUTES
 * ==========
 */

static TKLtyp_p_getTick_t pv_p_getTick;
    /**< \brief Pointer to access function that provides the current relative
         system time tick count */
static TKLtyp_tsk_t* volatile a_stc_tsk_pv_taskList; /**< \brief Registered
                                                          task list */
static volatile uint8_t u8_pv_taskCount; /**< \brief Number of tasks within
                                              registered task list */
static volatile uint8_t u8_pv_taskOverrunCount; /**< \brief Task deadline
                                                     overrun counter */

/* OPERATIONS
 * ==========
 */

void TKLsdlr_setTickSrc(const TKLtyp_p_getTick_t p_getTick)
{
    pv_p_getTick = p_getTick;

    return;
}

void TKLsdlr_setTskLst(TKLtyp_tsk_t* const a_stc_tsk_taskList,
                       const uint8_t u8_taskCount)
{
    a_stc_tsk_pv_taskList = a_stc_tsk_taskList;
    u8_pv_taskCount = u8_taskCount;

    return;
}

TKLtyp_tsk_t* TKLsdlr_getTskLst(void)
{
    return(a_stc_tsk_pv_taskList);
}

uint8_t TKLsdlr_cntTsk(void)
{
    return(u8_pv_taskCount);
}

uint8_t TKLsdlr_cntTskOverrun(void)
{
    return(u8_pv_taskOverrunCount);
}

void TKLsdlr_clrTskOverrun(void)
{
    u8_pv_taskOverrunCount = 0u;

    return;
}

void TKLsdlr_setTskAct(const TKLtyp_p_tskRunner_t p_tskRunner,
                       const bool b_active,
                       const bool b_updateLastRun)
{
    /* Set pointer to task list’s list */
    TKLtyp_tsk_t* const a_stc_tsk_taskList = a_stc_tsk_pv_taskList;

    /* Number of tasks in task list */
    const uint8_t u8_taskCount = u8_pv_taskCount;

    /* Find all tasks (matching function pointer) and set them to on/off
     * (`true`/`false`)
     */
    for(uint8_t u8_idx = 0u; u8_taskCount > u8_idx; u8_idx++)
    {
        if( *p_tskRunner == (*a_stc_tsk_taskList[u8_idx].p_tskRunner) )
        {
            a_stc_tsk_taskList[u8_idx].active = b_active;

            /* Update time stamp of last task run, if requested */
            if(true == b_updateLastRun)
            {
                a_stc_tsk_taskList[u8_idx].lastRun = (*pv_p_getTick)();
            }
        } /* if(...) */
    } /* for(...) */

    return;
}

void TKLsdlr_exec(void)
{
    /* Set pointer to task list’s list */
    TKLtyp_tsk_t* const a_stc_tsk_taskList = a_stc_tsk_pv_taskList;

    /* Number of tasks in task list */
    const uint8_t u8_taskCount = u8_pv_taskCount;

    /* Get current time tick count */
    const uint32_t u32_tickCount = (*pv_p_getTick)();

    /* Loop through all tasks in task list.
     * During one full loop ("cycle")
     * * run tasks only if they are due to run (according to their period) and
     *   enabled,
     * * ignore disabled tasks (but still update their `lastRun` time),
     * * check for task deadline overrun and keep count,
     * * if a task was run, end the cycle.
     */
    for(uint8_t u8_idx = 0u; u8_idx < u8_taskCount; u8_idx++)
    {
        /* Check if new execution period for task has started
         * (this is still correct on time tick rollover)
         */
        if( (u32_tickCount - a_stc_tsk_taskList[u8_idx].lastRun)
            >= a_stc_tsk_taskList[u8_idx].period )
        {
            /* Save (ideal) time of when task was "ready-to-run" */
            a_stc_tsk_taskList[u8_idx].lastRun
            = u32_tickCount
            - ( (u32_tickCount - a_stc_tsk_taskList[u8_idx].lastRun)
                % a_stc_tsk_taskList[u8_idx].period );

            /* Check if task is enabled */
            if(true == a_stc_tsk_taskList[u8_idx].active)
            {
                /* Run periodic task */
                (*a_stc_tsk_taskList[u8_idx].p_tskRunner)();

                /* Check for task deadline overrun
                 * (this is still correct on time tick rollover)
                 */
                if( ( (*pv_p_getTick)()
                      - a_stc_tsk_taskList[u8_idx].lastRun )
                    > a_stc_tsk_taskList[u8_idx].deadline )
                {
                    /* Increment task deadline overrun counter */
                    u8_pv_taskOverrunCount++;

                    /* Run custom task deadline overrun hook, if defined */
                    TKLSDLRCFG_OVERRUN_HOOK(
                        a_stc_tsk_taskList[u8_idx].p_tskRunner);
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
