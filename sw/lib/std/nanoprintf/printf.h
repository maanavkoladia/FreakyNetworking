#ifndef PRINTF_H
#define PRINTF_H

#if (SIM_MODE == 0)
#include "nanoprintf.h"
#include "OS.h"

#define printf(...) npf_pprintf((npf_putc)&npfputc, NULL, __VA_ARGS__)
#define snprintf(...) npf_snprintf(__VA_ARGS__)

/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */

/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */

/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */

#elif (SIM_MODE == 1)
#include <stdio.h>
#endif

#endif 

