/** \file */

#ifndef INCLUDE_TASKLIST_H
#define INCLUDE_TASKLIST_H

#include <stdint.h>

/* Include the Taskuler task interface */
#include "taskType.h"

/* Include Taskuler BSP interfaces */
#include "taskulerTimer.h"

/* Include application interfaces */
#include "ledBlinkTask.h"

/* OPERATIONS
 * ==========
 */

/**
 * \brief Retrieve task list
 *
 * This is used to register the task list with the Taskuler scheduler (see
 * fn_sdlr_setTaskAttributes()).
 *
 * \return Pointer to task list
 */
stc_tsk_t* fn_tskLst_getTaskList(void);

/**
 * \brief Retrieve number of tasks in task list
 *
 * This is used to register the task list with the Taskuler scheduler (see
 * fn_sdlr_setTaskAttributes()).
 *
 * \return Number of tasks in task list
 */
uint8_t fn_tskLst_getTaskCount(void);

#endif /* INCLUDE_TASKLIST_H */
