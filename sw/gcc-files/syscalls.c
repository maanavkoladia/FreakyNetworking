#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>

// Weak definition for _sbrk
void* _sbrk(ptrdiff_t incr) __attribute__((weak));
void* _sbrk(ptrdiff_t incr) {
    GPIO_PORTF_DATA_R |= 0x04;
    while (1);
}

// Weak definition for _close
int _close(int file) __attribute__((weak));
int _close(int file) {
    GPIO_PORTF_DATA_R |= 0x04;
    while (1);
}

// Weak definition for _fstat
int _fstat(int file, void *st) __attribute__((weak));
int _fstat(int file, void *st) {
  GPIO_PORTF_DATA_R |= 0x04;
    while (1);
}

// Weak definition for _isatty
int _isatty(int file) __attribute__((weak));
int _isatty(int file) {
GPIO_PORTF_DATA_R |= 0x04;
    while (1);
}

// Weak definition for _lseek
int _lseek(int file, int ptr, int dir) __attribute__((weak));
int _lseek(int file, int ptr, int dir) {
GPIO_PORTF_DATA_R |= 0x04;
    while (1);
}

// Weak definition for _read
int _read(int file, char *ptr, int len) __attribute__((weak));
int _read(int file, char *ptr, int len) {
GPIO_PORTF_DATA_R |= 0x04;
    while (1);
}

// Weak definition for _write
int _write( int file, char *ptr, int len ){
  GPIO_PORTF_DATA_R ^= 0x08; //toogle pf4
  UART_OutString(ptr);
  return 0;
}

// Weak definition for _exit
void _exit(int status) __attribute__((weak));
void _exit(int status) {
GPIO_PORTF_DATA_R |= 0x04;
    while (1);
}

// Weak definition for _kill
int _kill(int pid, int sig) __attribute__((weak));
int _kill(int pid, int sig) {
  GPIO_PORTF_DATA_R |= 0x04;
    while (1);
}

// Weak definition for _getpid
int _getpid(void) __attribute__((weak));
int _getpid(void) {
GPIO_PORTF_DATA_R ^= 0x04;
    while (1);
}

