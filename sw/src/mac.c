/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "mac.h"
#include "arp.h"
#include "ip.h"
#include "stubs.h"
#include "Networking_Globs.h"
#include <stdint.h>
#include "string_lite.h"
#include "arp.h"

/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */

#define HEADER_SIZE (14) 
uint8_t mac_broadcast_addr[MAC_ADDR_SIZE] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t test_MAC_ADDR[MAC_ADDR_SIZE] = { 0xA4, 0x72, 0xFC, 0xD2, 0xB4, 0x08 };
/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */

uint8_t* LookUpMac(uint32_t ip);

static void headerToBigEndian(macHeader_t* header);
static void headerTolittleEndian(macHeader_t* header);
void mac_print_header(const macHeader_t* header);
extern void ethernetTX(uint8_t* payload, uint16_t size);
extern void ethernetRX(void);
/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */
int macAddrComp(uint8_t* mac1, uint8_t* mac2){
    for(int i = 0; i < 6; i++){
      if(mac1[i] != mac2[i]) return 1;
    }
    return 0;
}

errMAC_t macRX(uint8_t* payload, uint16_t size){

    macHeader_t* macheader = (macHeader_t*)payload;
    headerTolittleEndian(macheader);
    mac_print_header(macheader);
    if(macAddrComp(macheader->dest_mac, host_mac_address)){
        LOG("Dropped ethenet pkt bc destionation mac didnt match ours");
    }
    switch (macheader->ethertype) {
        case ETHERTYPE_IPV4:
            int ret = ip4_rx(payload + HEADER_SIZE);
            if(ret){
                LOG("Failed with ip rx");
            }
            break;
        case ETHERTYPE_ARP:
            arpRX(payload + sizeof(macHeader_t), size - sizeof(macHeader_t));
            break;
        default:
            LOG("Unsupported EtherType Received: 0x%04X\n", macheader->ethertype);
    }
    return MAC_SUCCESS;
}

errMAC_t macTX(uint8_t* payload, uint16_t payloadsize, mac_EtherType_t ethertype_protocol){
    //omve the paylaod
    memmove(payload + HEADER_SIZE, payload, payloadsize);
    macHeader_t* macheader = (macHeader_t*)payload;
    
    switch (ethertype_protocol) {
        case ETHERTYPE_IPV4:
            macheader->ethertype = ETHERTYPE_IPV4;
            ipHeader_t* ipheader = (ipHeader_t*)(payload + HEADER_SIZE);
            uint8_t* destinationMAC = LookUpMac(packet_ntohl(ipheader->destinationIP));
            memcpy(macheader->dest_mac, destinationMAC, MAC_ADDR_SIZE);
            memcpy(macheader->src_mac, host_mac_address, MAC_ADDR_SIZE);
            mac_print_header(macheader);
            headerToBigEndian(macheader);
            ethernetTX(payload, payloadsize + HEADER_SIZE);
            break;

        case ETHERTYPE_ARP:
            macheader->ethertype = ETHERTYPE_ARP;
            arp_header_t* arpheader = (arp_header_t*)(payload + HEADER_SIZE);
            memcpy(macheader->dest_mac, arpheader->target_mac, MAC_ADDR_SIZE);
            memcpy(macheader->src_mac, host_mac_address, MAC_ADDR_SIZE);
            mac_print_header(macheader);
            headerToBigEndian(macheader);
            ethernetTX(payload, payloadsize + HEADER_SIZE);
            break;

        default:
            LOG("Unsupported EtherType Attempted to be transmitted: 0x%04X\n", ethertype_protocol);
            break;
    }
    return MAC_SUCCESS;
}

// SHOULD BE PASSED AS BIG ENDIAN
void setHostMac(uint8_t mac[6]){
    reverse_mac(mac);
    memcpy(host_mac_address, mac, 6);
}

uint8_t *getHostMac(){
    return host_mac_address;
}

uint8_t* LookUpMac(uint32_t ip){
    if(ip == 0xFFFFFFFF){
        return mac_broadcast_addr;
    }
    return test_MAC_ADDR;
}

void reverse_mac(uint8_t* addr) {
    for (int i = 0; i < 3; ++i) {
        uint8_t tmp = addr[i];
        addr[i] = addr[5 - i];
        addr[5 - i] = tmp;
    }
}

static void ntoh_macaddr(uint8_t* addr) {
    reverse_mac(addr);
}

static void hton_macaddr(uint8_t* addr) {
    reverse_mac(addr);
}

static void headerTolittleEndian(macHeader_t* header){
    header->ethertype = packet_ntohs(header->ethertype);
    ntoh_macaddr(header->dest_mac);    
    ntoh_macaddr(header->src_mac);    
}

static void headerToBigEndian(macHeader_t* header) {
    header->ethertype = packet_htons(header->ethertype);
    hton_macaddr(header->dest_mac);    
    hton_macaddr(header->src_mac);
}

void mac_print_header(const macHeader_t* header) {
    LOG("========== MAC HEADER ==========\n\r");

    LOG("Destination MAC     : %02X:%02X:%02X:%02X:%02X:%02X\n\r",
           header->dest_mac[5], header->dest_mac[4], header->dest_mac[3],
           header->dest_mac[2], header->dest_mac[1], header->dest_mac[0]);

    LOG("Source MAC          : %02X:%02X:%02X:%02X:%02X:%02X\n\r",
           header->src_mac[5], header->src_mac[4], header->src_mac[3],
           header->src_mac[2], header->src_mac[1], header->src_mac[0]);

    LOG("EtherType           : 0x%04X", header->ethertype);

    // Optional: print a description if known
    switch (header->ethertype) {
        case 0x0800:
            LOG(" (IPv4)\n\r");
            break;
        case 0x0806:
            LOG(" (ARP)\n\r");
            break;
        case 0x86DD:
            LOG(" (IPv6)\n\r");
            break;
        default:
            LOG(" (Unknown)\n\r");
            break;
    }

    LOG("================================\n\r");
}
