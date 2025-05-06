#ifndef PING_H
#define PING_H

#include "ICMP.h"
#define MAX_PING_PACKET_SIZE 64

typedef struct {
    icmpType_t type;
    uint8_t data[MAX_PING_PACKET_SIZE];
    uint16_t data_size;
    uint16_t send_ip;
} ping_msg_t;

void Task_Ping(void);

#endif
