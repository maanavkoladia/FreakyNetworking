#include "Networking_Globs.h"
#include <stdint.h>

uint8_t host_mac_address[6];
uint32_t host_ip_address = 0;
uint32_t subnet_mask;
uint32_t gateway;
sema4_t log_mtx;

uint16_t packet_ntohs(uint16_t network_short) {
    return ((network_short & 0x00FF) << 8) |
           ((network_short & 0xFF00) >> 8);
}

uint32_t packet_ntohl(uint32_t network_long) {
    return ((network_long & 0x000000FF) << 24) |
           ((network_long & 0x0000FF00) << 8)  |
           ((network_long & 0x00FF0000) >> 8)  |
           ((network_long & 0xFF000000) >> 24);
}

uint16_t packet_htons(uint16_t host_short) {
    return ((host_short & 0x00FF) << 8) |
           ((host_short & 0xFF00) >> 8);
}

uint32_t packet_htonl(uint32_t host_long) {
    return ((host_long & 0x000000FF) << 24) |
           ((host_long & 0x0000FF00) << 8)  |
           ((host_long & 0x00FF0000) >> 8)  |
           ((host_long & 0xFF000000) >> 24);
}
