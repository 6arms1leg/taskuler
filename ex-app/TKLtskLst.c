/** \file */

#include "TKLtskLst.h"

/* ATTRIBUTES
 * ==========
 */

/**
 * \brief Task (configuration) list, see \ref stc_tsk_t
 *
 * Holds the task’s active status (`true`/`false`), period (not `0`!), deadline
 * (not `0`!), offset (last run), and the pointer to function to be executed
 * (i.e., task runner).
 */
static stc_tsk_t a_stc_tsk_pv_taskList[] =
{
    /* Task 0 */
    { true,
      2u * U32_TASKULERTIMER_1S,
      U32_TASKULERTIMER_250MS,
      U32_TASKULERTIMER_0S - U32_TASKULERTIMER_1S,
      TKLtsk_blinkRunner },
    /* Task 1 */
    { true,
      2u * U32_TASKULERTIMER_1S,
      U32_TASKULERTIMER_250MS,
      U32_TASKULERTIMER_0S,
      TKLtsk_blinkRunner }
};

/* OPERATIONS
 * ==========
 */

stc_tsk_t* TKLtskLst_getTskLst(void)
{
    return(a_stc_tsk_pv_taskList);
}

uint8_t TKLtskLst_cntTsk(void)
{
    /* Evaluated at compile time */
    return( (uint8_t)( sizeof(a_stc_tsk_pv_taskList)
                       / sizeof(*a_stc_tsk_pv_taskList) ) );
}
