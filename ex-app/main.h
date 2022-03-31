/** \file */

#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>

#include "TKLsdlr.h"
#include "TKLint.h"
#include "TKLtick.h"

/* Make `main()` unit testable */
#ifndef TEST
#define MAIN main
#define TESTABLE_ENDLESSLOOP_CONDITION true
#else
#define MAIN testable_main
#define TESTABLE_ENDLESSLOOP_CONDITION false
#endif /* TEST */

/* OPERATIONS
 * ==========
 */

/** \brief Program entry point */
int MAIN(void);

#endif /* MAIN_H */
