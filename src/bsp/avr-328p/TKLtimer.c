/** \file */

#include "TKLtimer.h"

/**
 * \brief Timer0 Preload
 *
 * Required: *1 timeTick = 1e^-3 s (= 1 ms) => f_required = 1e^3 Hz*
 *
 * *f_CPU / prescaleValue / (timer0MaxValue - timer0PreloadValue) = f_required*  
 * *=> timer0MaxValue - (f_CPU / prescaleValue / f_required)*
 * *= timer0PreloadValue*  
 * *= 2^8 - (16e^6 Hz / 64 / 1e^3 Hz) = 6*
 */
#define U8_TASKULERTIMER_PRELOAD (6u)

/* OPERATIONS
 * ==========
 */

void TKLtimer_init(void)
{
    /* Time resolution ("time tick") shall be 1 ms (= 1e^3 Hz) */
    TCCR0B |= (1u<<CS01) | (1u<<CS00); /* Set prescale value to 64 */
    TIMSK0 |= (1u<<TOIE0); /* Enable Timer0 overflow interrupt */
    TCNT0 = U8_TASKULERTIMER_PRELOAD; /* Preload the timer */

    return;
}

/**
 * \brief Relative system time tick ISR
 *
 * Provides a regular time basis for the Taskuler scheduler.
 */
ISR(TIMER0_OVF_vect)
{
    TCNT0 = U8_TASKULERTIMER_PRELOAD; /* Preload the timer */
    TKLtick_incrTick();

    return;
}
