#ifndef STDIO_LITE_H
#define STDIO_LITE_H

#if (SIM_MODE == 0)
/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include <stdarg.h>
#include <stdint.h>
/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */
#define printf   printf_lite
#define sprintf  sprintf_lite
#define snprintf snprintf_lite
#define itoa     itoa_lite
/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */
void printf_lite(const char* format, ...);

void sprintf_lite(char* buffer, const char* format, ...);

void snprintf_lite(char* buffer, int buffer_size, const char* format, ...);

void itoa_lite(int32_t num, char* buffer);

void itoa_hex_lite(unsigned int value, char *buf, int uppercase);
#else
#include <stdio.h>
#endif

#endif 

