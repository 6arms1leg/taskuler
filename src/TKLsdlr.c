/** \file */

#include "TKLsdlr.h"

/* ATTRIBUTES
 * ==========
 */

static uint32_t (*p_fn_pv_getTickCount)(void);
    /**< \brief Pointer to access function that provides the current relative
         system time tick count */
static stc_tsk_t* volatile a_stc_tsk_pv_taskList; /**< \brief Registered task
                                                       list */
static volatile uint8_t u8_pv_taskCount; /**< \brief Number of tasks within
                                              registered task list */
static volatile uint8_t u8_pv_taskOverrunCount; /**< \brief Task deadline
                                                     overrun counter */

/* OPERATIONS
 * ==========
 */

void TKLsdlr_setTickSrc( uint32_t (* const p_fn_getTickCount)(void) )
{
    p_fn_pv_getTickCount = p_fn_getTickCount;

    return;
}

void TKLsdlr_setTskLst(stc_tsk_t* const a_stc_tsk_taskList,
                       const uint8_t u8_taskCount)
{
    a_stc_tsk_pv_taskList = a_stc_tsk_taskList;
    u8_pv_taskCount = u8_taskCount;

    return;
}

stc_tsk_t* TKLsdlr_getTskLst(void)
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

void TKLsdlr_setTskAct(void (* const p_fn_taskRunner)(void),
                       const bool b_active,
                       const bool b_updateLastRun)
{
    /* Set pointer to task list’s list */
    stc_tsk_t* const a_stc_tsk_taskList = a_stc_tsk_pv_taskList;

    /* Number of tasks in task list */
    const uint8_t u8_taskCount = u8_pv_taskCount;

    /* Find all tasks (matching function pointer) and set them to on/off
     * (`true`/`false`)
     */
    for(uint8_t u8_idx = 0u; u8_taskCount > u8_idx; u8_idx++)
    {
        if( *p_fn_taskRunner
            == (*a_stc_tsk_taskList[u8_idx].p_fn_taskRunner) )
        {
            a_stc_tsk_taskList[u8_idx].b_active = b_active;

            /* Update time stamp of last task run, if requested */
            if(true == b_updateLastRun)
            {
                a_stc_tsk_taskList[u8_idx].u32_lastRun =
                    (*p_fn_pv_getTickCount)();
            }
        } /* if(...) */
    } /* for(...) */

    return;
}

void TKLsdlr_exec(void)
{
    /* Set pointer to task list’s list */
    stc_tsk_t* const a_stc_tsk_taskList = a_stc_tsk_pv_taskList;

    /* Number of tasks in task list */
    const uint8_t u8_taskCount = u8_pv_taskCount;

    /* Get current time tick count */
    const uint32_t u32_tickCount = (*p_fn_pv_getTickCount)();

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
        if( (u32_tickCount - a_stc_tsk_taskList[u8_idx].u32_lastRun)
            >= a_stc_tsk_taskList[u8_idx].u32_period )
        {
            /* Save (ideal) time of when task was "ready-to-run" */
            a_stc_tsk_taskList[u8_idx].u32_lastRun
            = u32_tickCount
            - ( (u32_tickCount - a_stc_tsk_taskList[u8_idx].u32_lastRun)
                % a_stc_tsk_taskList[u8_idx].u32_period );

            /* Check if task is enabled */
            if(true == a_stc_tsk_taskList[u8_idx].b_active)
            {
                /* Run periodic task */
                (*a_stc_tsk_taskList[u8_idx].p_fn_taskRunner)();

                /* Check for task deadline overrun
                 * (this is still correct on time tick rollover)
                 */
                if( ( (*p_fn_pv_getTickCount)()
                      - a_stc_tsk_taskList[u8_idx].u32_lastRun )
                    > a_stc_tsk_taskList[u8_idx].u32_deadline )
                {
                    /* Increment task deadline overrun counter */
                    u8_pv_taskOverrunCount++;

                    /* Run custom task deadline overrun hook, if defined */
                    TASKULER_TASKOVERRUNHOOK(
                        a_stc_tsk_taskList[u8_idx].p_fn_taskRunner);
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
