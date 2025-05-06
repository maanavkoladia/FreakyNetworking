#ifndef INTERNET_CHECKSUM_H
#define INTERNET_CHECKSUM_H

#include <stdint.h>

uint16_t generate_checksum(void* header, uint16_t headersize);

#endif
