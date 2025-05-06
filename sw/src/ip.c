/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "ip.h"
#include "ICMP.h"
#include "internet_checksum.h"
#include <stdint.h>
#include <string.h>
#include "string_lite.h"
#include "Networking_Globs.h"
#include "mac.h"
#include "Networking.h"
#include "UDP.h"
/* ================================================== */
/*            DEFAULTS                                */           
/* ================================================== */
#define VERSION_DEFAULT (4)
#define IHL_DEFAULT (5) 

//tos defualts
#define DSCP_DEFAULT (0)
#define ECN_DEFAULT (0)

#define FRAG_DEFAULT (0x4000)//reserved:0, DF(dont frag):1, MF(more fragement): 0, frag offeset:0
#define DF_MASK (0x4000)
#define MF_MASK (0x2000)

#define TTL_DEFAULT (64)

/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */

uint16_t identification = 0x1234;
/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */

static void headerToBigEndian(ipHeader_t* header);
static void headerTolittleEndian(ipHeader_t* header);

int dropPkt(ipHeader_t* header);
errIP_t SendPktToTransport(ipHeader_t* header, uint8_t* data, uint16_t datasize);
/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */
errIP_t ip4_tx(uint16_t payloadsize, uint8_t* payload, IpProtocol_t protocol, uint32_t destinationIP) {
    ipHeader_t* header = (ipHeader_t*)payload;

    // Step 4: Move payload to make room for header
    memmove(payload + HEADER_SIZE_DEFAULT, payload, payloadsize);

    // Step 1: Populate fields in little-endian (host order)
    header->version = VERSION_DEFAULT;
    header->ihl = IHL_DEFAULT;
    //header->version_ihl = ((VERSION_DEFAULT << 4) & 0xF0) | (IHL_DEFAULT & 0x0F);
    header->DSCP_ECN = ((ECN_DEFAULT << 6) & 0xC0) | (DSCP_DEFAULT & 0x3F);
    header->totalPacketLength = payloadsize + HEADER_SIZE_DEFAULT;
    header->identification = identification++;
    header->flags_fragmentOffset = FRAG_DEFAULT;
    header->TTL = TTL_DEFAULT;
    header->protocol = (uint8_t) protocol;
    header->headerChecksum = 0; // must be 0 before computing
    header->sourceIP = host_ip_address;
    header->destinationIP = destinationIP;

    // Step 2: Convert all 16/32-bit fields to big-endian
    ip4_print_header(header);
    headerToBigEndian(header);
    
    // Step 3: Generate checksum in little-endian
    header->headerChecksum = generate_checksum(header, HEADER_SIZE_DEFAULT); 
    //send to mac layer
    int ret = macTX(payload, packet_ntohs(header->totalPacketLength), ETHERTYPE_IPV4);
    return ret == MAC_SUCCESS ? IP_SUCCESS : IP_TX_FAIL ; // or whatever your success enum is
}

errIP_t ip4_rx(uint8_t* payload){
    ipHeader_t* header = (ipHeader_t*)payload;
    //checksum;
    uint16_t savedCksm = header->headerChecksum;
    header->headerChecksum = 0;
    uint16_t computed_checksum = generate_checksum(header, header->ihl<<2);

    // loop through and print the bytes of both header and data
    LOG("\n");

    if(savedCksm != computed_checksum){
        LOG("Packet Dropped: Checksum Invalid: %x, %x", savedCksm, computed_checksum);
        return IP_RX_FAIL;
    }

    // Convert to little endian after checksum
    headerTolittleEndian(header);
    ip4_print_header(header);

    if(dropPkt(header)){
        LOG("Packet Dropped");
        return IP_RX_PCKT_DROPPED;
    }

    SendPktToTransport(header, payload + (header->ihl << 2), header->totalPacketLength - header->ihl << 2);
    return IP_SUCCESS;
}

//expects data to have the trasnport header, not ip header
errIP_t SendPktToTransport(ipHeader_t* header, uint8_t* data, uint16_t datasize) {
    switch (header->protocol) {
        case IP_PROTOCOL_ICMP:
            int ret = icmp_rx(data, datasize);
            return ret == ICMP_SUCCESS? IP_SUCCESS: IP_RX_FAIL;

        case IP_PROTOCOL_IGMP:
            LOG("Received IGMP packet — not handled");
            return IP_RX_UNSUPPORTED_PROTOCOL;

        case IP_PROTOCOL_TCP:
            //TODO: add tcp support
            //return tcp_rx(header, data);
            return IP_RX_UNSUPPORTED_PROTOCOL;

        case IP_PROTOCOL_UDP:
            //TODO:add udp support
            return udp_rx(data, datasize);
            //return IP_RX_UNSUPPORTED_PROTOCOL;

        case IP_PROTOCOL_IPV6:
            LOG("IPv6 over IPv4 encapsulation not supported");
            return IP_RX_UNSUPPORTED_PROTOCOL;

        case IP_PROTOCOL_GRE:
            LOG("GRE packet received — not supported");
            return IP_RX_UNSUPPORTED_PROTOCOL;

        case IP_PROTOCOL_ESP:
            LOG("IPSec ESP received — not supported");
            return IP_RX_UNSUPPORTED_PROTOCOL;

        case IP_PROTOCOL_AH:
            LOG("IPSec AH received — not supported");
            return IP_RX_UNSUPPORTED_PROTOCOL;

        case IP_PROTOCOL_ICMPV6:
            LOG("ICMPv6 received — not supported on IPv4");
            return IP_RX_UNSUPPORTED_PROTOCOL;

        case IP_PROTOCOL_OSPF:
            LOG("OSPF packet received — not supported");
            return IP_RX_UNSUPPORTED_PROTOCOL;

        case IP_PROTOCOL_SCTP:
            LOG("SCTP packet received — not supported");
            return IP_RX_UNSUPPORTED_PROTOCOL;

        default:
            LOG("Unknown protocol (%u) — dropped", header->protocol);
            return IP_RX_UNSUPPORTED_PROTOCOL;
    }
}

int dropPkt(ipHeader_t* header){
    if (header->version != VERSION_DEFAULT) {
        LOG("Dropped packet: Invalid IP version (%u)", header->version);
        return 1;
    }

    //if (header->ihl != (HEADER_SIZE_DEFAULT / 4)) {
    //    LOG("Dropped packet: Invalid IHL (%u)", header->ihl);
    //    return 1;
    //}

    if (header->totalPacketLength > MTU) {
        LOG("Dropped packet: Packet length %u exceeds MTU", header->totalPacketLength);
        return 1;
    }

    //if ((header->flags_fragmentOffset & DF_MASK) == 0 ||
    //    (header->flags_fragmentOffset & 0x1FFF) != 0) {
    //    LOG("Dropped packet: Fragmented packet not supported");
    //    return 1;
    //}
    if((header->flags_fragmentOffset & MF_MASK) || (header->flags_fragmentOffset & 0x1FFF) != 0){
        LOG("Dropped packet: Fragmented packet not supported: bits= %u", header->flags_fragmentOffset);
        return 1;    
    }

    if ((header->destinationIP != host_ip_address && host_ip_address != 0)) {
        LOG("Dropped packet: IP 0x%08X not meant for us (0x%08X)", header->destinationIP, host_ip_address);
        return 1;
    }
    return 0;
}

void ip4_print_header(ipHeader_t* header) {
    LOG("========== IP HEADER (HEX + DECIMAL) ==========\n\r");

    LOG("Version           : 0x%01X (%u)\n\r", header->version, header->version);           
    LOG("IHL               : 0x%01X (%u bytes)\n\r", header->ihl, header->ihl * 4);

    uint8_t dscp = (header->DSCP_ECN & 0xFC) >> 2;
    uint8_t ecn  = (header->DSCP_ECN & 0x03);
    LOG("DSCP              : 0x%02X (%u)\n\r", dscp, dscp);
    LOG("ECN               : 0x%01X (%u)\n\r", ecn, ecn);

    LOG("Total Length      : 0x%04X (%u)\n\r", header->totalPacketLength, header->totalPacketLength);
    LOG("Identification    : 0x%04X (%u)\n\r", header->identification, header->identification);
  
    // This shifting takes place while flags_fragmentOffset is in big endian
    uint16_t frag = header->flags_fragmentOffset;
    uint8_t flags = (frag & 0xE0) >> 5;
    uint16_t offset = ((frag & 0xFF00) >> 8) | ((frag & 0x001F) << 8);

    LOG("Flags             : 0x%01X (DF=%u, MF=%u)\n\r", flags,
           (flags >> 1) & 1, flags & 1);
    LOG("Fragment Offset   : 0x%04X (%u)\n\r", offset, offset);

    LOG("TTL               : 0x%02X (%u)\n\r", header->TTL, header->TTL);
    LOG("Protocol          : 0x%02X (%u)\n\r", header->protocol, header->protocol);
    LOG("Header Checksum   : 0x%04X (%u)\n\r", header->headerChecksum, header->headerChecksum);

    uint8_t* src = (uint8_t*)&header->sourceIP;
    uint8_t* dst = (uint8_t*)&header->destinationIP;

    LOG("Source IP         : 0x%02X%02X%02X%02X (%u.%u.%u.%u)\n\r", 
            src[3], src[2], src[1], src[0], src[3], src[2], src[1], src[0]);

    LOG("Destination IP    : 0x%02X%02X%02X%02X (%u.%u.%u.%u)\n\r", 
            dst[3], dst[2], dst[1], dst[0], dst[3], dst[2], dst[1], dst[0]);

    LOG("===============================================\n\r");
}

static void headerTolittleEndian(ipHeader_t* header){
    header->totalPacketLength = packet_ntohs(header->totalPacketLength);
    header->identification = packet_ntohs(header->identification);
    header->flags_fragmentOffset = packet_ntohs(header->flags_fragmentOffset);
    header->sourceIP = packet_ntohl(header->sourceIP); 
    header->destinationIP = packet_ntohl(header->destinationIP); 
}

static void headerToBigEndian(ipHeader_t* header) {
    header->totalPacketLength     = packet_htons(header->totalPacketLength);
    header->identification        = packet_htons(header->identification);
    header->flags_fragmentOffset  = packet_htons(header->flags_fragmentOffset);
    //header->headerChecksum        = packet_htons(header->headerChecksum);
    header->sourceIP              = packet_htonl(header->sourceIP);
    header->destinationIP         = packet_htonl(header->destinationIP);
}

void setHostIP(uint32_t ip_address){
    host_ip_address = ip_address;
    //memcpy(host_ip_address, ip_address, 4);
}
