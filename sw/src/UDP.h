#ifndef UDP_H
#define UDP_H

#include <stdint.h>
typedef enum {
    UDP_SUCCESS,
    UDP_TX_FAIL,
    UDP_RX_FAIL,
}errUDP_t;

typedef struct __attribute__((packed)) {
    uint16_t sourcePort;        // Source Port
    uint16_t destinationPort;   // Destination Port
    uint16_t length;            // Length of UDP header + data
    uint16_t checksum;          // Checksum
} udpHeader_t;

errUDP_t udp_tx(uint16_t payloadsize, uint8_t *payload, uint32_t destinationIP, uint16_t sourcePort, uint16_t destPort);
errUDP_t udp_rx(uint8_t* payload, uint16_t payloadsize);
#endif
