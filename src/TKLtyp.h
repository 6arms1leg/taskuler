/** \file */

#ifndef TKLTYP_H
#define TKLTYP_H

/* `"` used intentionally.  This allows the user to override and provide his
 * own type definitions before falling back to libc.
 */
#include "stdint.h"
#include "stdbool.h"

/** \brief Defines parameters needed by scheduler for scheduled task execution */
typedef struct
{
    volatile bool active; /**< \brief Task activation status

                               If `true`, task is enabled and will be run as
                               scheduled.
                               If `false`, task is disabled and will not be run. */
    const uint32_t period; /**< \brief Time (since previous task run) after
                                which task will be run again

                                Must not be `0`! */
    const uint32_t deadline; /**< \brief Deadline when task has to finish
                                      its execution

                                  This should be equal to task’s WCET + some
                                  buffer time.  Must not be `0`!  The allowed
                                  range is *[WCET, period]*. */
    volatile uint32_t lastRun; /**< \brief Time stamp of last task run

                                    Helper variable necessary for scheduling
                                    algorithm.  Normally, initialized to `0`.
                                    Can also be used to define an offset (from
                                    0) to other tasks, when initially defined
                                    in task list.  To off-set a task define the
                                    offset as a substraction from 0.  That is,
                                    a periodic task can be phase-shifted to the
                                    left by *delta* by defining an offset *0 -
                                    delta*.  The useful range for the offset
                                    *delta* is *]0, period[*. */
    void (* const p_fn_taskRunner)(void); /**< \brief Function pointer to task
                                               runner */
} TKLtyp_tsk_t;

#endif /* TKLTYP_H */
