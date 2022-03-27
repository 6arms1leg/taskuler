/** \file */

#include "main.h"

/* Application interfaces */
#include "TKLtskLst.h"
#include "led.h"

int MAIN(void)
{
    TKLINT_DIS(); /* Crit. region start (disable all ISRs) */

    /* Initialize application LED */
    led_init();

    /* Initialize Taskuler BSP’s relative system time tick */
    TKLtick_init();

    /* Initialize Taskuler */
    TKLsdlr_setTickSrc(TKLtick_getTick);
    TKLsdlr_setTskLst( TKLtskLst_getTskLst(), TKLtskLst_cntTsk() );

    TKLINT_ENA(); /* Crit. region end (enable all ISRs) */

    /* Reset time tick count *directly* before starting the scheduler,
     * otherwise first execution might run all tasks (e.g., if all offsets are
     * 0 and current tick count is already larger than biggest period in
     * tasklist), potentially messing up the schedulability
     */
    TKLtick_clrTick();

    while(true) /* Main endless loop */
    {
        TKLsdlr_exec(); /* Taskuler scheduling algorithm execution cycle */
    }

    /*return(1);*/ /* Never reached */
}
