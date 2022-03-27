/** \file */

#include "blink.h"

/* OPERATIONS
 * ==========
 */

void blink_toggleLed(void)
{
    led_toggle(); /* Call the "toggle" LED driver function */

    return;
}
