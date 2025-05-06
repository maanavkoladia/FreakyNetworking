#include "internet_checksum.h"
#include <stdint.h>

uint16_t generate_checksum(void* header, uint16_t headersize) {
    uint32_t sum = 0;
    uint16_t* data = (uint16_t*)header;
    for (uint16_t i = 0; i < headersize/2; i++) {
        sum += data[i]; // Convert each 16-bit word from network to host byte order
        if (sum > 0xFFFF) {
            sum = (sum & 0xFFFF) + 1; // Wrap around carry
        }
    }
    return ~((uint16_t)sum); // Final ones' complement
}

