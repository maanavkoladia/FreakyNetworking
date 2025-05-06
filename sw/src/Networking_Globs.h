#ifndef NETWORKING_GLOBS_H
#define NETWORKING_GLOBS_H

#include <stdint.h>
#include "printf.h"

#define MTU (1500)

extern sema4_t log_mtx;
//#define LOG(...) OS_Wait(&log_mtx); printf("[%s][%d]", __FUNCTION__, __LINE__); printf(__VA_ARGS__); printf("\n\r"); OS_Signal(&log_mtx);
//#define LOG(...) OS_Wait(&log_mtx); printf(__VA_ARGS__); printf("\n\r"); OS_Signal(&log_mtx);
#define LOG(...) while(0);
#define ASSERT(x) do {if(!(x)){ LOG("ASSERT FAILED: %s", #x); while(1){} }} while(0)

extern uint32_t host_ip_address;
extern uint8_t host_mac_address[6];
// extern uint8_t dhcp_tx_buf[MTU];
extern uint8_t irq_rcv_buf[MTU+200];

uint16_t packet_ntohs(uint16_t network_short);
uint32_t packet_ntohl(uint32_t network_long);
uint16_t packet_htons(uint16_t host_short);
uint32_t packet_htonl(uint32_t host_long);
#endif
