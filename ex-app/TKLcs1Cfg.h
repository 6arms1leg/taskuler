/** \file */

#ifndef TKLCS1CFG_H
#define TKLCS1CFG_H

#include <stdint.h>

/* `#include` mem. mapped interrupt register (and optionally driver) interfaces */
#include /* >ADD HEADER(S) HERE< */

/** \brief Interrupt register type/size of target MCU */
typedef /* >ADD DATA TYPE HERE< */ TKLcs1Cfg_intReg_t;

/**
 * \brief Current interrupt status register
 *
 * Global memory mapped register that holds the current interrupt status.
 */
#define TKLCS1CFG_GET_INT_STATUS /* >ADD REGISTER HERE< */

/**
 * \brief Deactivate (disable/mask/block) interrupts
 *
 * Can be an atomic function call or a global memory mapped register
 * assignment.
 * If some interrupts should stay untouched, they can be skipped (using a logic
 * operator and a bit mask) in assignment.
 */
#define TKLCS1CFG_DIS_INT /* >ADD FUNCTION CALL/ASSIGNMENT HERE< */

/**
 * \brief Interrupt activation (enable/unmask/unblock) register
 *
 * Global memory mapped register that activates interrupts.
 */
#define TKLCS1CFG_ENA_INT_REG /* >ADD REGISTER HERE< */

#endif /* TKLCS1CFG_H */
