#include "DHCP.h"
#include "UDP.h"
#include "mac.h"
#include "ip.h"
#include "Networking_Globs.h"
#include "string.h"
#include <stdint.h>
#include "OS.h"

#define DHCP_CLIENT_PORT 68
#define DHCP_SERVER_PORT 67
#define DHCP_BROADCAST_IP 0xFFFFFFFF
#define DHCP_FIXED_SIZE (sizeof(dhcp_packet_t) - sizeof(((dhcp_packet_t *)0)->options))

extern uint8_t host_mac_address[6];

void (*user_offer_cb)(void) = NULL;

static const dhcp_packet_t dhcp_template = {
    .OP = 1,       // BOOTREQUEST
    .HTYPE = 1,    // Ethernet
    .HLEN = 6,     // MAC address length
    .HOPS = 0,
    .XID = 0x12345678,
    .secs = 0,
    .flags = 0x8000, // Broadcast flag
    .client_ip = 0,
    .my_ip = 0,
    .server_ip = 0,
    .gateway_ip = 0,
    .mac_address = {0},
    .misc = {0},
    .cookie = 0x63538263, // DHCP magic cookie; flipped for network byte order
    .options = {0}
};

static uint8_t dhcp_tx_buf[600];

static uint32_t dhcp_server_ip = 0;
static uint32_t dhcp_offered_ip = 0;

static void prepare_dhcp_packet(dhcp_packet_t *pkt) {
    LOG("Preparing DHCP packet");
    memcpy(pkt, &dhcp_template, sizeof(dhcp_packet_t));
    
    memcpy(pkt->mac_address, host_mac_address, 6);
    reverse_mac((uint8_t *)&pkt->mac_address);

    LOG("DHCP packet prepared with MAC address: %02X:%02X:%02X:%02X:%02X:%02X",
        host_mac_address[0], host_mac_address[1], host_mac_address[2],
        host_mac_address[3], host_mac_address[4], host_mac_address[5]);
}

static void append_dhcp_discover_options(uint8_t *options) {
    LOG("Appending DHCPDISCOVER options");
    options[0] = 53; // DHCP Message Type
    options[1] = 1;
    options[2] = 1; // 1 = DHCPDISCOVER
    options[3] = 255; // End
    LOG("DHCPDISCOVER options appended");
}

static void append_dhcp_request_options(uint8_t *options, uint32_t server_ip, uint32_t requested_ip) {
    LOG("Appending DHCPREQUEST options");
    uint8_t *ptr = options;
    *ptr++ = 53; *ptr++ = 1; *ptr++ = 3; // DHCPREQUEST

    *ptr++ = 50; *ptr++ = 4;              // Requested IP address
    memcpy(ptr, &requested_ip, 4);
    ptr += 4;

    *ptr++ = 54; *ptr++ = 4;              // Server Identifier
    memcpy(ptr, &server_ip, 4);
    ptr += 4;

    *ptr++ = 255;                         // End
    LOG("DHCPREQUEST options appended with Server IP: %08X, Requested IP: %08X", server_ip, requested_ip);
}

void dhcp_send_discover(void) {
    LOG("Sending DHCPDISCOVER");
    dhcp_packet_t *pkt = (dhcp_packet_t *)dhcp_tx_buf;
    prepare_dhcp_packet(pkt);

    append_dhcp_discover_options(pkt->options);

    size_t packet_len = DHCP_FIXED_SIZE + 4; // 4 bytes of options
    udp_tx(packet_len, dhcp_tx_buf, DHCP_BROADCAST_IP, DHCP_CLIENT_PORT, DHCP_SERVER_PORT);
    LOG("DHCPDISCOVER sent, packet length: %zu", packet_len);
}

void dhcp_send_request(void) {
    LOG("Sending DHCPREQUEST");
    dhcp_packet_t *pkt = (dhcp_packet_t *)dhcp_tx_buf;
    prepare_dhcp_packet(pkt);

    append_dhcp_request_options(pkt->options, packet_htonl(dhcp_server_ip), packet_htonl(dhcp_offered_ip));

    size_t packet_len = DHCP_FIXED_SIZE + 16; // 16 bytes of options
    udp_tx(packet_len, dhcp_tx_buf, DHCP_BROADCAST_IP, DHCP_CLIENT_PORT, DHCP_SERVER_PORT);
    LOG("DHCPREQUEST sent, packet length: %zu", packet_len);
}

static int dhcp_receive_offer(dhcp_packet_t *pkt, uint16_t size) {
    LOG("Received DHCPOFFER");
    dhcp_server_ip = packet_ntohl(pkt->server_ip);
    dhcp_offered_ip = packet_ntohl(pkt->my_ip);
    LOG("DHCPOFFER processed, Server IP: %08X, Offered IP: %08X", dhcp_server_ip, dhcp_offered_ip);

    user_offer_cb();

    return 0;
}

static int dhcp_receive_ack(dhcp_packet_t *pkt, uint16_t size) {
    LOG("Received DHCPACK");
    uint32_t assigned_ip = packet_ntohl(pkt->my_ip);
    setHostIP(assigned_ip);
    LOG("DHCPACK processed, Assigned IP: %08X", assigned_ip);
    return 0;
}

int dhcpRX(uint8_t *payload, uint16_t size) {
    LOG("Processing received DHCP packet, size: %u", size);
    if (size < DHCP_FIXED_SIZE) {
        LOG("Received packet is too small, discarding");
        return -1;
    }

    dhcp_packet_t *pkt = (dhcp_packet_t *)payload;

    if (pkt->options[2] == 2) { // 2 = DHCPOFFER
        return dhcp_receive_offer(pkt, size);
    } else if (pkt->options[2] == 5) { // 5 = DHCPACK
        return dhcp_receive_ack(pkt, size);
    }

    LOG("Unhandled DHCP message type: %u", pkt->options[2]);
    return -1; // Unhandled DHCP message
}

void register_dhcp_callback(void (*callback)(void)) {
    user_offer_cb = callback;
    LOG("DHCP callback registered");
}