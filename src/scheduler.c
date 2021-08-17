/** \file */

#include "scheduler.h"

/* ATTRIBUTES
 * ==========
 */

static uint32_t (*p_fn_pv_getTickCount)(void) = NULL; /**< \brief Pointer to
                                                           access function that
                                                           provides the current
                                                           relative system time
                                                           tick count */
static stc_tsk_t* a_stc_tsk_pv_taskList = NULL; /**< \brief Registered task
                                                     list */
static uint8_t u8_pv_taskCount = (uint8_t)0U; /**< \brief Number of tasks within
                                                   registered task list */
static uint8_t u8_pv_taskOverrunCount = (uint8_t)0U; /**< \brief Task deadline
                                                          overrun counter */

/* OPERATIONS
 * ==========
 */

void fn_sdlr_setTickCountSource( uint32_t (*p_fn_getTickCount)(void) )
{
    p_fn_pv_getTickCount = p_fn_getTickCount;

    return;
}

void fn_sdlr_setTaskAttributes(stc_tsk_t* const a_stc_tsk_taskList,
                               uint8_t const u8_taskCount)
{
    a_stc_tsk_pv_taskList = a_stc_tsk_taskList;
    u8_pv_taskCount = u8_taskCount;

    return;
}

stc_tsk_t* fn_sdlr_getTaskList(void)
{
    return(a_stc_tsk_pv_taskList);
}

uint8_t fn_sdlr_getTaskCount(void)
{
    return(u8_pv_taskCount);
}

uint8_t fn_sdlr_getTaskOverrunCount(void)
{
    return(u8_pv_taskOverrunCount);
}

void fn_sdlr_resetTaskOverrunCount(void)
{
    u8_pv_taskOverrunCount = (uint8_t)0U;

    return;
}

void fn_sdlr_setTaskAct(void (*p_fn_taskRunner)(void), en_act_t en_act_active)
{
    /* Set pointer to task list’s list */
    stc_tsk_t* a_stc_tsk_taskList = a_stc_tsk_pv_taskList;

    /* Number of tasks in task list */
    uint8_t u8_taskCount = u8_pv_taskCount;

    uint8_t u8_idx = (uint8_t)0U;

    /* Find all tasks (matching function pointer) and set them to `ON`/`OFF` */
    for(u8_idx = (uint8_t)0U; u8_taskCount > u8_idx; u8_idx++)
    {
        if( *p_fn_taskRunner
            == (*a_stc_tsk_taskList[u8_idx].p_fn_taskRunner) )
        {
            a_stc_tsk_taskList[u8_idx].en_act_active = en_act_active;
        }
    } /* for() */

    return;
}

void fn_sdlr_execute(void)
{
    /* Set pointer to task list’s list */
    stc_tsk_t* a_stc_tsk_taskList = a_stc_tsk_pv_taskList;

    /* Number of tasks in task list */
    uint8_t u8_taskCount = u8_pv_taskCount;

    /* Get current time tick count */
    uint32_t u32_tickCount = (*p_fn_pv_getTickCount)();

    uint8_t u8_idx = (uint8_t)0U; /* Task list index */

    /* Loop through all tasks in task list.
     * During one full loop ("cycle")
     * * run tasks only if they are due to run (according to their period) and
     *   enabled,
     * * ignore disabled tasks (but still update their `lastRun` time),
     * * check for task deadline overrun and keep count,
     * * if a task was run, end the cycle.
     */
    for(u8_idx = (uint8_t)0U; u8_idx < u8_taskCount; u8_idx++)
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
            if(en_act_ON == a_stc_tsk_taskList[u8_idx].en_act_active)
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
                }
                else {} /* Do nothing; no task deadline overrun */

                /* End cycle to allow starting a new one as soon as possible.
                 * This gives better schedulability.
                 */
                break;
            } /* if(...) */
            else {} /* Do nothing; task disabled */
        } /* if(...) */
        else {} /* Do nothing; no new task execution period */
    } /* for(...) */

    return;
}
