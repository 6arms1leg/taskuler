/** \file */

#include "blink.h"

/* OPERATIONS
 * ==========
 */

void fn_ledBlk_blinkLedL(void)
{
    fn_ledL_toggle(); /* Call the "toggle" LED driver function */

    return;
}
