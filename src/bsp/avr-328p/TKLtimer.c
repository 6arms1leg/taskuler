/** \file */

#include "TKLtimer.h"

/**
 * \brief Timer0 Preload
 *
 * Required: `1 tick = 1e-3 s => f_req = 1e3 Hz`
 *
 * `f_CPU / prescaleVal / (timer0MaxVal - timer0PreloadVal) = f_req`  
 * `=> timer0MaxVal - (f_CPU / prescaleVal / f_req)`
 * `= timer0PreloadVal = 2^8 - (16e6 Hz / 64 / 1e3 Hz) = 6`
 */
#define PRELOAD 6u

/* OPERATIONS
 * ==========
 */

void TKLtimer_init(void) {
    /* Time resolution ("tick") shall be 1e-3 s (= 1e3 Hz) */
    TCCR0B |= (1u<<CS01) | (1u<<CS00); /* Set prescale value to `64` */
    TIMSK0 |= (1u<<TOIE0); /* Enable Timer0 overflow interrupt */
    TCNT0 = PRELOAD; /* Preload timer */
}

/**
 * \brief Relative system time tick ISR
 *
 * Provides regular time basis for Taskuler scheduler.
 */
ISR(TIMER0_OVF_vect) {
    TCNT0 = PRELOAD; /* Preload timer */
    TKLtick_incrTick();
}
