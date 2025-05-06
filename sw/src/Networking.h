#ifndef NETWORKING_H 
#define NETWORKING_H

#include <stdint.h>

typedef enum {
    NW_SCAN,
    NW_GET_SCAN_DATA,
    NW_CONNECT,
    NW_DISCONNECT,
    NW_GET_MAC,
    NW_SEND_RAW,
    NW_RECEIVE_RAW,
    NW_RECEIVE_IRQ
} network_command_type;

typedef struct {
    network_command_type command;
    uint8_t data[96]; // Example data buffer, adjust size as needed
} network_command_t;

void prettyprint_payload(uint8_t* payload, uint16_t size);

/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */

/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */
typedef enum {
    NETWORKING_SUCCESS,
    WIFI_INIT_FAIL,
    GET_MAC_FAIL,
    LIST_SSID_FAIL
}errNetworking_t;

/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */

void Task_NetworkThread(void);
void Task_ReceiveIRQ(void);
void Task_NetworkingInit(void);

void Network_Receive_IRQ(void);
void Network_Scan(void);
void Network_Connect(char *ssid, char *password);
void Network_Disconnect(void);
void Network_Send_Raw(void);
void Network_Receive_Raw(void);
void Network_Get_Mac(void);

#endif 

