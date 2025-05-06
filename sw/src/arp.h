#ifndef ARP_H
#define ARP_H
/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include <stdint.h>
/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */
typedef enum{
    APR_SUCCESS,
    APR_FAILED_BROADCAST,
    APR_FAILED_RESOLVE,
    APR_FAILED_CACHE_INIT
} errARP_t;

typedef struct __attribute__((__packed__)) {
    uint16_t htype;
    uint16_t ptype;
    uint8_t  hlen;
    uint8_t  plen;
    uint16_t opcode;
    uint8_t  sender_mac[6];
    uint32_t  sender_ip;
    uint8_t  target_mac[6];
    uint32_t  target_ip;
} arp_header_t;

/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */
errARP_t arpRX(uint8_t * payload, uint16_t payloadsize);
void arp_init(void);

#endif 

