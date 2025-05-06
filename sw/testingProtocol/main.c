#include <stdint.h>
#include "printf.h"
#include <stdio.h>
#include <string.h>
#include "UDP.h"
#include "ip.h"
#include "mac.h"
#include "Networking_Globs.h"
#include "stubs.h"
#include "DHCP.h"

#define MSG(...) printf("\n\r"); printf("[%s][%d]", __FUNCTION__, __LINE__); printf(__VA_ARGS__); printf("\n\r")

#ifdef DHCPTEST
char* tx_outfile = "temp/dhcp_req.txt";
char* rx_infile = "temp/dhcp_ack.txt";
#endif /* ifdef DHCPTEST */

#ifdef ECHOTEST
char* tx_outfile = "temp/echoresp.txt";
char* rx_infile = "temp/echoreq.txt";
#endif /* ifdef ECHOTEST */

#define PAYLOAD_SIZE 33
#define DESTINATION_IP 0xC0A80001 //192.0.1.168
uint32_t HOST_IP  = 0xC0A80064;  // 192.168.0.100

uint8_t testhostmac[6] = {0x3F, 0x18, 0x87, 0x8F, 0x47, 0xD8};

uint8_t payload[PAYLOAD_SIZE] = {
    0x3A, 0xB7, 0x1F, 0x92, 0x6C, 0xD0, 0x8E, 0x15,
    0xF4, 0x2D, 0x7A, 0xE1, 0x9B, 0x43, 0xC6, 0x5E,
    0x01, 0xAD, 0x7F, 0x68, 0x94, 0xD7, 0x3C, 0x2B,
    0xEF, 0x88, 0x36, 0x10, 0x77, 0x5A, 0xC9, 0x42,
    0xE5
};

uint8_t message[] = "hi, shitter";

#define MESSAGE_SIZE sizeof(message)

//bc MTU is ip layer, so adding some for mac header
uint8_t networkBuf[MTU+50];

inline static void testTX(void){
    MSG("Starting up testing");

    memcpy(networkBuf, message, MESSAGE_SIZE);
    //ethernetTX(payload, PAYLOAD_SIZE);
    int ret = udp_tx(MESSAGE_SIZE, networkBuf, DESTINATION_IP, 0, 0x4567);
    if(ret == IP_SUCCESS){
        MSG("message sent succesfully");
    }else{
        MSG("message sent not sent"); 
    }

    MSG("oopsie poopsie packet recived");  
}

inline static void testPingReq(void){
    //rx_infile = "temp/echoreq.txt";
    ethernetRX();
}

inline static void testRX(void){
    ethernetRX();
}

inline static void testDHCP_DISC(void){
    //tx_outfile = "temp/dhcp_disc.txt";
    dhcp_send_discover();
}

inline static void testDHCP_REQ(void){
    ethernetRX();
    dhcp_send_request();
}

int main(int argc, char *argv[]){
    setHostMac(testhostmac);
    //setHostIP(HOST_IP);
    //testPingReq();

    //testRX();    
    // testDHCP_REQ();
    ethernetRX();
    return 0;
}
