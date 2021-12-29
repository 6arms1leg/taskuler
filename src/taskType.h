/** \file */

#ifndef INCLUDE_TASKTYPE_H
#define INCLUDE_TASKTYPE_H

/* `"` used intentionally.  This allows the user to override and provide his
 * own type definitions before falling back to libc.
 */
#include "stdint.h"

/**
 * \brief Defines the "active" member of \ref stc_tsk_t (that is \ref
 * stc_tsk_t.en_act_active)
 */
typedef enum
{
    en_act_ON, /**< \brief Task is enabled and will be run as scheduled */
    en_act_OFF /**< \brief Task is disabled and will not be run */
} en_act_t;

/** \brief Defines parameters needed by scheduler for scheduled task execution */
typedef struct
{
    en_act_t en_act_active; /**< \brief Task activation */
    const uint32_t u32_period; /**< \brief Time (since previous task run) after
                                    which task will be run again

                                    Must not be `0`! */
    const uint32_t u32_deadline; /**< \brief Deadline when task has to finish
                                      its execution

                                      This should be equal to task’s WCET +
                                      some buffer time.  Must not be `0`!  The
                                      allowed range is *[WCET, period]*. */
    uint32_t u32_lastRun; /**< \brief Time stamp of last task run

                               Helper variable necessary for scheduling
                               algorithm.  Normally, initialized to `0`.  Can
                               also be used to define an offset (from 0) to
                               other tasks, when initially defined in task
                               list.  To off-set a task define the offset as a
                               substraction from 0.  That is, a periodic task
                               can be phase-shifted to the left by *delta* by
                               defining an offset *0 - delta*.  The useful
                               range for the offset *delta* is *]0, period[*. */
    void (*const p_fn_taskRunner)(void); /**< \brief Function pointer to task
                                              runner */
} stc_tsk_t;

#endif /* INCLUDE_TASKTYPE_H */
