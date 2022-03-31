/** \file */

#include "blink.h"

#include "led.h"

/* OPERATIONS
 * ==========
 */

void blink_toggleLed(void) {
    led_toggle(); /* Call LED driver */
}
