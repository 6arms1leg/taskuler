/** \file */

#ifndef INCLUDE_SCHEDULER_TESTMOCKS_H
#define INCLUDE_SCHEDULER_TESTMOCKS_H

/**
 * \brief Function mock to simulate and manipulate the relative system time
 * tick source of the MCU
 *
 * \return Relative system time tick count
 */
uint32_t fn_getTickCount(void);

/**
 * \{
 * \brief Function mock to assert expected function calls through pointers to
 * function
 */
void fn_taskRunner(void);
void fn_taskRunner0(void);
void fn_taskRunner1(void);
void fn_taskRunner2(void);
/** \} */

#endif /* INCLUDE_SCHEDULER_TESTMOCKS_H */
