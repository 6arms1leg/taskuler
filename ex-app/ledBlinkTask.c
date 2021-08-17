/** \file */

#include "ledBlinkTask.h"

/* OPERATIONS
 * ==========
 */

void fn_ledBlkTsk_taskRunner(void)
{
    fn_ledBlk_blinkLedL(); /* Call blink function of the LED Blink module */

    return;
}
