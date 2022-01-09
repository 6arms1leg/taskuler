/** \file */

#ifndef INCLUDE_SCHEDULER_H
#define INCLUDE_SCHEDULER_H

/* `"` used intentionally.  This allows the user to override and provide his
 * own type definitions before falling back to libc.
 */
#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

#include "taskType.h"

/* OPERATIONS
 * ==========
 */

/**
 * \brief Registers the relative system time tick with the scheduler
 *
 * \param p_fn_getTickCount Pointer to an access function that provides the
 * current relative system time tick count.
 * That time tick count access function has to be implemented by the user.
 * Typically, the time tick is generated by an ISR that periodically increments
 * a time tick count variable.  Since this time tick count variable is a shared
 * resource, it must be ensured that either:
 * * the access of this variable is atomic, or
 * * this access function is implemented with a critical region pattern inside.
 */
void fn_sdlr_setTickCountSource( uint32_t (* const p_fn_getTickCount)(void) );

/**
 * \brief Registers a task list with the scheduler
 *
 * \param p_a_stc_tsk_taskList Task list
 * \param u8_taskCount Number of tasks within the provided task list
 */
void fn_sdlr_setTaskAttributes(stc_tsk_t* const p_a_stc_tsk_taskList,
                               const uint8_t u8_taskCount);

/**
 * \brief Get task list that is registered with the scheduler
 *
 * \return Task list that is registered with the scheduler
 *
 * \see fn_sdlr_setTaskAttributes()
 */
stc_tsk_t* fn_sdlr_getTaskList(void);

/**
 * \brief Get number of tasks within the task list that is registered with the
 * scheduler
 *
 * \return Number of tasks within task the list that is registered with the
 * scheduler
 *
 * \see fn_sdlr_setTaskAttributes()
 */
uint8_t fn_sdlr_getTaskCount(void);

/**
 * \brief Get number of task deadline overruns
 *
 * \return Number of task deadline overruns
 */
uint8_t fn_sdlr_getTaskOverrunCount(void);

/** \brief Reset Task deadline overrun counter */
void fn_sdlr_resetTaskOverrunCount(void);

/**
 * \brief Activate/deactivate a task within the task list that is registered
 * with the scheduler
 *
 * \param p_fn_taskRunner Task runner that orchestrates module(s) interaction
 * sequence to achieve task goal
 * \param en_act_active Desired task activation status
 * \param b_updateLastRun Directive to update the time stamp of last task run
 * to the current relative system time tick count.
 * This is useful to start a timer (i.e., a one-shot task that deactives itself
 * at the end of its execution).
 */
void fn_sdlr_setTaskAct(void (* const p_fn_taskRunner)(void),
                        const en_act_t en_act_active,
                        const bool b_updateLastRun);

/**
 * \brief Scheduling algorithm execution cycle
 *
 * This function needs to be called from within the main `while(true)` endless
 * loop.
 */
void fn_sdlr_execute(void);

#endif /* INCLUDE_SCHEDULER_H */
