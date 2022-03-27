/** \file */

#ifndef TKLTSKLST_H
#define TKLTSKLST_H

/* libc */
#include <stdint.h>
#include <stdbool.h>

/* Include the Taskuler task interface */
#include "TKLtyp.h"

/* Include Taskuler BSP interfaces */
#include "TKLtimer.h"

/* Include application interfaces */
#include "TKLtsk.h"

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

#endif /* TKLTSKLST_H */
