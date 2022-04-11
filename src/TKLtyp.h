/** \file */

#ifndef TKLTYP_H
#define TKLTYP_H

/* `"` used intentionally.  This allows the user to override and provide his
   own type definitions before falling back to libc. */
#include "stdint.h"
#include "stdbool.h"

/**
 * \brief Helper to calc. positive offset from `0` for \ref TKLtyp_tsk_t.lastRun
 *
 * `offset_`, here, specifies time of first task exec.
 * `period_` and `offset_` args. must both be of unsigned integer type.
 *
 * See \ref TKLtyp_tsk_t.lastRun for more details.
 */
#define TKLTYP_CALC_OFFSET(period_, offset_) (0u - (period_) + (offset_))

/** \brief Task runner function signature */
typedef void (* TKLtyp_p_tskRunner_t)(void);

/** \brief Parameters needed by scheduler for scheduled task execution */
typedef struct {
    /**
     * \brief Task activation status
     *
     * If `true`, task is enabled and will be run as scheduled.  If `false`,
     * task is disabled and will not be run.
     */
    volatile bool active;

    /**
     * \brief Time (since previous task run) after which task will be run
     * again Must not be `0`!
     */
    const uint32_t period;

    /**
     * \brief Deadline when task has to finish its execution
     *
     * This should be equal to task’s WCET + some buffer time.  Must not be
     * `0`!  The allowed range is `[WCET, period]`.
     */
    const uint32_t deadline;

    /** \brief Time stamp of last task run
     *
     * Helper variable necessary for scheduling algorithm.  Normally, init. to
     * `0`.  Can also be used to define offset (from `0`) to other tasks, when
     * initially defined in task list.  To off-set a task define offset as
     * substraction from 0.  I.e., a periodic task can be phase-shifted to left
     * by `delta` by defining offset `0 - delta`.  The useful range for offset
     * `delta` is `]0, period[`.
     */
    volatile uint32_t lastRun;

    /** \brief Function pointer to task runner */
    const TKLtyp_p_tskRunner_t p_tskRunner;
} TKLtyp_tsk_t;

#endif /* TKLTYP_H */
