/** \file */

#ifndef TKLTICK_H
#define TKLTICK_H

#include <stdint.h>

/* OPERATIONS
 * ==========
 */

/**
 * \brief Function mock to simulate and manipulate MCU’s relative system time
 * tick source
 *
 * \return Relative system time tick count
 */
uint32_t TKLtick_getTick(void);

#endif /* TKLTICK_H */
