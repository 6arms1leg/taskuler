/** \file */

#ifndef TKLTSKLST_H
#define TKLTSKLST_H

#include <stdint.h>
#include <stdbool.h>

#include "TKLtyp.h"
#include "TKLtimer.h"

/* ATTRIBUTES
 * ==========
 */

/**
 * \brief Global opaque pointer to task (cfg.) list
 *
 * Used to register task list with scheduler.
 */
extern TKLtyp_tsk_t* const TKLtskLst_p_tskLst;

/**
 * \brief Global opaque pointer to number of tasks in task list
 *
 * Used to register task list with scheduler.
 */
extern const uint8_t* const TKLtskLst_p_tskCnt;

#endif /* TKLTSKLST_H */
