/** \file */

#ifndef TKLTICK_H
#define TKLTICK_H

/* `"` used intentionally.  This allows the user to override and provide his
 * own type definitions before falling back to libc.
 */
#include "stdint.h"

/* OPERATIONS
 * ==========
 */

/**
 * \brief Function mock to simulate and manipulate the relative system time
 * tick source of the MCU
 *
 * \return Relative system time tick count
 */
uint32_t TKLtick_getTick(void);

#endif /* TKLTICK_H */
