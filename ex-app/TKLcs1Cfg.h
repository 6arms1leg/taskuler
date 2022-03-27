/** \file */

#ifndef TKLCS1CFG_H
#define TKLCS1CFG_H

/* libc */
#include <stdint.h>

/* Include memory mapped interrupt register (and optionally driver) interfaces */
#include /* Add register interface name here (and  optionally also include
            interrupt driver interface */

/** \brief Interrupt register type/size of target MCU */
typedef /* Add data type here */ cs1_intReg_t;

/**
 * \brief Current interrupt status register
 *
 * This is the global memory mapped register that holds the current interrupt
 * status.
 */
#define TKLCS1CFG_GET_INT_STATUS /* Add register name here */

/**
 * \brief Deactivate (disable/mask/block) interrupts
 *
 * This can be an atomic function call or a global memory mapped register
 * assignment.
 * If some interrupts should stay untouched, they can be skipped (using a logic
 * operator and a bit mask) in the assignment.
 */
#define TKLCS1CFG_DIS_INT /* Add function call/assignment here */

/**
 * \brief Interrupt activation (enable/unmask/unblock) register
 *
 * This is the global memory mapped register that activates interrupts.
 */
#define TKLCS1CFG_ENA_INT_REG /* Add register name here */

#endif /* TKLCS1CFG_H */
