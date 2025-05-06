#ifndef DHCP_CLIENT_H
#define DHCP_CLIENT_H
/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include <stdint.h>

/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */

#define MAGIC_COOKIE 0x63825363

typedef struct __attribute((packed)) {
    uint8_t OP;
    uint8_t HTYPE;
    uint8_t HLEN;
    uint8_t HOPS;
    uint32_t XID;
    uint16_t secs;
    uint16_t flags;
    uint32_t client_ip;
    uint32_t my_ip;
    uint32_t server_ip;
    uint32_t gateway_ip;
    uint8_t mac_address[16];
    uint8_t misc[192];
    uint32_t cookie;
    uint8_t options[274];
} dhcp_packet_t;

/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */

void dhcp_send_discover(void);
void dhcp_send_request(void);
int dhcpRX(uint8_t *payload, uint16_t size);
void register_dhcp_callback(void (*callback)(void));

#endif 

