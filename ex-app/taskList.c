/** \file */

#include "taskList.h"

/* ATTRIBUTES
 * ==========
 */

/**
 * \brief Task (configuration) list, see \ref stc_tsk_t
 *
 * Holds the task’s active status (on/off), period (not `0`!), deadline (not
 * `0`!), offset (last run), and the pointer to function to be executed
 * (i.e., task runner).
 */
static stc_tsk_t a_stc_tsk_pv_taskList[] =
{
    /* Task 0 */
    { en_act_ON,
      (uint32_t)( (uint32_t)2U * U32_TASKULERTIMER_1S ),
      U32_TASKULERTIMER_250MS,
      (uint32_t)( U32_TASKULERTIMER_0S - U32_TASKULERTIMER_1S ),
      fn_ledBlkTsk_taskRunner },
    /* Task 1 */
    { en_act_ON,
      (uint32_t)( (uint32_t)2U * U32_TASKULERTIMER_1S ),
      U32_TASKULERTIMER_250MS,
      U32_TASKULERTIMER_0S,
      fn_ledBlkTsk_taskRunner }
};

/* OPERATIONS
 * ==========
 */

stc_tsk_t* fn_tskLst_getTaskList(void)
{
    return(a_stc_tsk_pv_taskList);
}

uint8_t fn_tskLst_getTaskCount(void)
{
    /* Evaluated at compile time */
    return( (uint8_t)( sizeof(a_stc_tsk_pv_taskList)
                       / sizeof(*a_stc_tsk_pv_taskList) ) );
}
