#ifndef MAC_H 
#define MAC_H

/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include <stdint.h>
/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */

typedef enum {
    ETHERTYPE_IPV4        = 0x0800,  // Internet Protocol v4
    ETHERTYPE_ARP         = 0x0806,  // Address Resolution Protocol
    ETHERTYPE_WOL         = 0x0842,  // Wake-on-LAN
    ETHERTYPE_VLAN        = 0x8100,  // IEEE 802.1Q VLAN-tagged frame
    ETHERTYPE_IPV6        = 0x86DD,  // Internet Protocol v6
    ETHERTYPE_MPLS_UNI    = 0x8847,  // MPLS Unicast
    ETHERTYPE_MPLS_MULTI  = 0x8848,  // MPLS Multicast
    ETHERTYPE_PPPOE_DISC  = 0x8863,  // PPPoE Discovery
    ETHERTYPE_PPPOE_SESS  = 0x8864,  // PPPoE Session
    ETHERTYPE_EAPOL       = 0x888E,  // EAP over LAN (802.1X)
    ETHERTYPE_LLDP        = 0x88CC,  // Link Layer Discovery Protocol
    ETHERTYPE_LOOPBACK    = 0x9000,  // Loopback
    ETHERTYPE_TEST        = 0x88B5   // Experimental/Test use
} mac_EtherType_t;

typedef enum{
    MAC_SUCCESS,
    MAC_RX_FAIL,
    MAC_RX_FAIL_INCORRECT_DEST_MAC,
    MAC_TX_FAIL,
}errMAC_t;

typedef struct __attribute__((packed)) {
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype; 
} macHeader_t;

#define MAC_ADDR_SIZE (6)
/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */

errMAC_t macRX(uint8_t* payload, uint16_t size);
errMAC_t macTX(uint8_t* payload, uint16_t payloadsize, mac_EtherType_t ethertype_protocol);
void setHostMac(uint8_t mac[6]);
uint8_t *getHostMac();
int macAddrComp(uint8_t* mac1, uint8_t* mac2);
void reverse_mac(uint8_t* addr);
#endif 

