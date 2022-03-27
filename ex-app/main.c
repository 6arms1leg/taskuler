/** \file */

#include "main.h"

/* Application interfaces */
#include "TKLtskLst.h"
#include "led.h"

int MAIN(void)
{
    TASKULERINTERRUPT_DISABLE(); /* Crit. region start (disable all ISRs) */

    /* Initialize application LED */
    fn_ledL_init();

    /* Initialize Taskuler BSP’s relative system time tick */
    fn_tmTck_init();

    /* Initialize Taskuler */
    fn_sdlr_setTickCountSource(fn_tmTck_getTickCount);
    fn_sdlr_setTaskAttributes( fn_tskLst_getTaskList(),
                               fn_tskLst_getTaskCount() );

    TASKULERINTERRUPT_ENABLE(); /* Crit. region end (enable all ISRs) */

    /* Reset time tick count *directly* before starting the scheduler,
     * otherwise first execution might run all tasks (e.g., if all offsets are
     * 0 and current tick count is already larger than biggest period in
     * tasklist), potentially messing up the schedulability
     */
    fn_tmTck_resetTickCount();

    while(true) /* Main endless loop */
    {
        fn_sdlr_execute(); /* Taskuler scheduling algorithm execution cycle */
    }

    /*return(1);*/ /* Never reached */
}
