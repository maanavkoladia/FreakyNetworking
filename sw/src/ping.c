#include "ping.h"
#include "OS.h"
#include "ICMP.h"
#include "internet_checksum.h"
#include "string_lite.h"

#include "Networking_Globs.h"

#define PING_Q_SIZE 10

OS_FIFO_t ping_q;
sema4_t ping_semas[3];
ping_msg_t ping_buf[PING_Q_SIZE];

static uint8_t ping_tx_networkBuf[MTU+50];

void ping_init(){
    OS_Fifo_Init(PING_Q_SIZE, &ping_q, (uint8_t*)ping_buf, sizeof(ping_msg_t), ping_semas);
}

void Task_Ping(void){
    ping_init();

    LOG("Ping task started; FIFO initialized");

    while(1){
        ping_msg_t data;
        OS_Fifo_Get((uint8_t*)&data, &ping_q);
        memcpy(ping_tx_networkBuf, data.data, data.data_size);
        icmp_tx(ping_tx_networkBuf, data.data_size, data.send_ip, data.type, 0, 0);
    }
}
