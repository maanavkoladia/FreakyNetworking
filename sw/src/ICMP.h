#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>

typedef enum {
    ICMP_ECHO_REPLY = 0,
    ICMP_DEST_UNREACH = 3,
    ICMP_SOURCE_QUENCH = 4,
    ICMP_REDIRECT = 5,
    ICMP_ECHO_REQUEST = 8,
    ICMP_TIME_EXCEEDED = 11,
    ICMP_PARAMETER_PROBLEM = 12,
    ICMP_TIMESTAMP_REQUEST = 13,
    ICMP_TIMESTAMP_REPLY = 14
} icmpType_t;

typedef struct __attribute__((packed)) {
    icmpType_t type : 8;
    uint8_t code;
    uint16_t checksum;
    uint32_t rest;
} icmpHeader_t;

typedef enum{
    ICMP_SUCCESS,
    ICMP_TX_FAIL,
    ICMP_RX_FAIL,
    ICMP_PKT_DROPPED,
} errICMP_t;

errICMP_t icmp_tx(uint8_t *payload, uint16_t payloadsize, uint32_t destinationIP, icmpType_t type, uint8_t code, uint32_t rest);

errICMP_t icmp_rx(uint8_t *payload, uint16_t payloadsize);

#endif
