/** \file */

#ifndef MAIN_H
#define MAIN_H

/* libc interface */
#include <stdbool.h>

/* Taskuler scheduler interface */
#include "TKLsdlr.h"

/* Taskuler BSP interfaces */
#include "TKLint.h"
#include "TKLtick.h"

/* Make `main()` unit testable */
#ifndef TEST
#define MAIN main
#else
#define MAIN testable_main
#endif

/** \brief Program entry point */
int MAIN(void);

#endif /* MAIN_H */
