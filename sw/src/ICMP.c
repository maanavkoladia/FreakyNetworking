#include "ICMP.h"
#include "ping.h"
#include "ip.h"
#include "internet_checksum.h"
#include "Networking_Globs.h"
#include "string.h"
#include <stdint.h>

static inline void headerTolittleEndian(icmpHeader_t* header){
    header->checksum = packet_ntohs(header->checksum);
    header->rest = packet_ntohl(header->rest);
}

static inline void headerToBigEndian(icmpHeader_t* header){
    header->rest = packet_htonl(header->rest);
}

static inline void icmp_print_header(const icmpHeader_t* header) {
    LOG("========== ICMP HEADER (HEX + DECIMAL) ==========\n");
    LOG("Type              : 0x%02X (%u)\n", header->type, header->type);
    LOG("Code              : 0x%02X (%u)\n", header->code, header->code);
    LOG("Checksum          : 0x%04X (%u)\n", header->checksum, header->checksum);
    LOG("Rest              : 0x%08X (%u)\n", header->rest, header->rest);
    LOG("===============================================\n");
}


char echoresp[]= "ping echo response";

errICMP_t icmp_tx(uint8_t *payload, uint16_t payloadsize, uint32_t destinationIP, icmpType_t type, uint8_t code, uint32_t rest){
    icmpHeader_t *header = (icmpHeader_t *)payload;
   
    memmove(payload + sizeof(icmpHeader_t), payload, payloadsize);

    header->type = type;
    header->code = code;
    header->rest = rest;
    header->checksum = 0;

    // Step 1: Convert header to big-endian
    icmp_print_header(header);
    headerToBigEndian(header);
    
    // Step 2: Generate checksum in big-endian
    header->checksum = generate_checksum(payload, sizeof(icmpHeader_t) + payloadsize);
    
    // Step 3: Send to IP layer
    int ret = ip4_tx(payloadsize + sizeof(icmpHeader_t), payload, IP_PROTOCOL_ICMP, destinationIP);
    return ret == IP_SUCCESS ? ICMP_SUCCESS : ICMP_TX_FAIL;
}

extern OS_FIFO_t ping_q;

uint8_t icmp_tx_networkBuf[MTU+50];
errICMP_t icmp_rx(uint8_t *payload, uint16_t payloadsize){
    icmpHeader_t *header = (icmpHeader_t *)payload;
    
    uint16_t savedCksm = header->checksum;
    header->checksum = 0;
    uint16_t computed_checksum = generate_checksum(header, payloadsize);

    headerTolittleEndian(header);
    icmp_print_header(header);

    //if (savedCksm != computed_checksum){
    //    LOG("Packet Dropped: Checksum Invalid: %x, %x", savedCksm, computed_checksum);
    //    return ICMP_RX_FAIL;
    //}

    if(header->code != 0){
        LOG("Packet Dropped: code is not servable: %u", header->code);
        return ICMP_PKT_DROPPED;
    }

    switch (header->type) {
        case ICMP_ECHO_REQUEST:
            LOG("revieced a echo req, sending out a resp now \n\n");
            
            ipHeader_t* iphdr = (ipHeader_t*)(payload - IP_HEADER_SIZE);
            memcpy(icmp_tx_networkBuf, echoresp, sizeof(echoresp));
            icmp_tx(icmp_tx_networkBuf, sizeof(echoresp), iphdr->sourceIP, ICMP_ECHO_REPLY, 0, header->rest);

            //ping_msg_t msg;
            //msg.type = ICMP_ECHO_REPLY;
            //msg.data_size = payloadsize - sizeof(icmpHeader_t);
            //msg.send_ip = iphdr->sourceIP;
            //memcpy(msg.data, payload + sizeof(icmpHeader_t), msg.data_size);
            //
            //OS_Fifo_Put((uint8_t*)&msg, &ping_q);
            //LOG("Ping message sent to queue");
            break;
        default:
            LOG("Packet Dropped: type is not servable: %u", header->type);
            return ICMP_PKT_DROPPED;            
            break;
    }
}


