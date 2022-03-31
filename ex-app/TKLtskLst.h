/** \file */

#ifndef TKLTSKLST_H
#define TKLTSKLST_H

#include <stdint.h>
#include <stdbool.h>

#include "TKLtyp.h"
#include "TKLtimer.h"

/* OPERATIONS
 * ==========
 */

/**
 * \brief Retrieve task list
 *
 * Used to register task list with scheduler.
 *
 * \return Pointer to task list
 */
TKLtyp_tsk_t* TKLtskLst_getTskLst(void);

/**
 * \brief Retrieve number of tasks in task list
 *
 * Used to register task list with scheduler.
 *
 * \return Number of tasks in task list
 */
uint8_t TKLtskLst_cntTsk(void);

#endif /* TKLTSKLST_H */
