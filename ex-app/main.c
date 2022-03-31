/** \file */

#include "main.h"

#include "TKLtskLst.h"
#include "led.h"

/* OPERATIONS
 * ==========
 */

int MAIN(void) {
    TKLINT_DIS(); /* Crit. region start (disable ISRs) */

    led_init(); /* Init. LED */

    TKLtick_init(); /* Init. BSP’s rel. sys. time tick */

    /* Init. scheduler */
    TKLsdlr_setTickSrc(&TKLtick_getTick);
    TKLsdlr_setTskLst(TKLtskLst_getTskLst(), TKLtskLst_cntTsk());

    TKLINT_ENA(); /* Crit. region end (enable ISRs) */

    /* Reset time tick count *directly* before starting scheduler,
       otherwise first execution might run all tasks (e.g., if all offsets are
       0 and current tick count is already larger than biggest period in
       task list), potentially messing up schedulability */
    TKLtick_clrTick();

    do { /* Endless "super loop" */
        TKLsdlr_exec(); /* Scheduling algorithm exec. cycle */
    } while (TESTABLE_ENDLESSLOOP_CONDITION);

#ifdef TEST
    return (0); /* Never reached (except in unit tests) */
#endif /* TEST */
}
