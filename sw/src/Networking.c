/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "Networking.h"
#include "Networking_Globs.h"
#include "mac.h"

#include <stdint.h>

#include "bsp/include/nm_bsp.h"
#include "bus_wrapper/include/nm_bus_wrapper.h"
#include "driver/include/m2m_wifi.h"
#include "m2m_types.h"
#include "OS.h"
#include "tm4c123gh6pm.h"

#include "string_lite.h"

#include "printf.h"

#include "main.h"

/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */
tstrWifiInitParam wifi_init_param;
uint8_t irq_rcv_buf[MTU+200];

uint8_t eth_rcv_buf[MTU+200];
uint16_t eth_rcv_size;

uint8_t *curr_send_buf;
uint16_t curr_send_size;

/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */

#define LOG(...) printf("[%s][%d]", __FUNCTION__, __LINE__); printf(__VA_ARGS__); printf("\n\r")

/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */

void prettyprint_payload(uint8_t* payload, uint16_t size) {
    // printf("Full payload (pcap format):");
    // for (int i = 0; i < size; i++) {
    //     if(i%0x10 == 0){printf("\n\r%04x ", i);}
    //     printf("%02x ", payload[i]);
    // }
    // printf("\n\r");

    // printf("Payload (not pcap):\n\r");
    // for (int i = 0; i < size; i++) {
    //     printf("%02x ", payload[i]);
    // }

    // printf("\n\r");
    // printf("Payload size: %d", size);
    // printf("\n\r");
}

sema4_t data_captured;
sema4_t data_captured2;
tenuM2mStaCmd irqReceiveType;
void *msg = NULL;
void wifi_callback(uint8 u8MsgType, void *pvMsg) {
    LOG("Wi-Fi callback triggered! Message type: %d", u8MsgType);
    
    irqReceiveType = u8MsgType;
    msg = pvMsg;
    switch (irqReceiveType) {
        case M2M_WIFI_REQ_CONNECT:
            LOG("Wi-Fi request to connect received.");
            break;
        case M2M_WIFI_REQ_GET_CONN_INFO:
            LOG("Wi-Fi request to get connection info received.");
            break;
        case M2M_WIFI_RESP_CONN_INFO:
            LOG("Wi-Fi response with connection info received.");
            break;
        case M2M_WIFI_REQ_DISCONNECT:
            LOG("Wi-Fi request to disconnect received.");
            break;
        case M2M_WIFI_RESP_CON_STATE_CHANGED:
            LOG("Wi-Fi connection state changed.");
            break;
        case M2M_WIFI_REQ_SLEEP:
            LOG("Wi-Fi request to sleep received.");
            break;
        case M2M_WIFI_REQ_SCAN:
            LOG("Wi-Fi scan request received.");
            break;
        case M2M_WIFI_RESP_SCAN_DONE:
            LOG("Wi-Fi scan done response received.");
            break;
        case M2M_WIFI_REQ_SCAN_RESULT:
            LOG("Wi-Fi scan result request received.");
            break;
        case M2M_WIFI_RESP_SCAN_RESULT:
            LOG("Wi-Fi scan result response received.");
            break;
        case M2M_WIFI_REQ_START_WPS:
            LOG("Wi-Fi request to start WPS received.");
            break;
        case M2M_WIFI_REQ_DISABLE_WPS:
            LOG("Wi-Fi request to disable WPS received.");
            break;
        case M2M_WIFI_RES_WPS:
            LOG("Wi-Fi WPS response received.");
            break;
        case M2M_WIFI_RESP_IP_CONFIGURED:
            LOG("Wi-Fi IP configured response received.");
            break;
        case M2M_WIFI_RESP_IP_CONFLICT:
            LOG("Wi-Fi IP conflict response received.");
            break;
        case M2M_WIFI_REQ_ENABLE_MONITORING:
            LOG("Wi-Fi request to enable monitoring received.");
            break;
        case M2M_WIFI_REQ_DISABLE_MONITORING:
            LOG("Wi-Fi request to disable monitoring received.");
            break;
        case M2M_WIFI_RESP_WIFI_RX_PACKET:
            LOG("Wi-Fi RX packet response received.");
            break;
        case M2M_WIFI_REQ_SEND_WIFI_PACKET:
            LOG("Wi-Fi request to send packet received.");
            break;
        case M2M_WIFI_REQ_LSN_INT:
            LOG("Wi-Fi listen interval request received.");
            break;
        case M2M_WIFI_REQ_SEND_ETHERNET_PACKET:
            LOG("Wi-Fi request to send Ethernet packet received.");
            break;
        case M2M_WIFI_RESP_ETHERNET_RX_PACKET:
            LOG("Wi-Fi Ethernet RX packet response received.");
            break;
        case M2M_WIFI_REQ_SET_SCAN_OPTION:
            LOG("Wi-Fi request to set scan option received.");
            break;
        case M2M_WIFI_REQ_SET_SCAN_REGION:
            LOG("Wi-Fi request to set scan region received.");
            break;
        case M2M_WIFI_REQ_SET_SCAN_LIST:
            LOG("Wi-Fi request to set scan list received.");
            break;
        case M2M_WIFI_REQ_SET_MAC_MCAST:
            LOG("Wi-Fi request to set MAC multicast received.");
            break;
        case M2M_WIFI_REQ_SET_P2P_IFC_ID:
            LOG("Wi-Fi request to set P2P interface ID received.");
            break;
        case M2M_WIFI_RESP_PACKET_SENT:
            LOG("Wi-Fi packet sent response received.");
            break;
        case M2M_WIFI_REQ_CERT_ADD_CHUNK:
            LOG("Wi-Fi request to add certificate chunk received.");
            break;
        case M2M_WIFI_REQ_CERT_DOWNLOAD_DONE:
            LOG("Wi-Fi certificate download done request received.");
            break;
        case M2M_WIFI_REQ_CHG_MONITORING_CHNL:
            LOG("Wi-Fi request to change monitoring channel received.");
            break;
        default:
            LOG("Unknown Wi-Fi message type received.");
            break;
    }

    if(u8MsgType != 7) OS_Signal(&data_captured);
}

extern void ethernetTX(uint8_t* payload, uint16_t size);
extern void ethernetRX(void);
void eth_callback(uint8 u8MsgType, void *pvMsg, void *pvCtrlBuf) {
    LOG("Ethernet callback triggered! Message type: %d", u8MsgType);
    switch (u8MsgType) {
        case M2M_WIFI_RESP_ETHERNET_RX_PACKET:
            LOG("Ethernet RX packet response received.");
            uint8 au8RemoteIpAddr[4];

            uint8 *au8packet = pvMsg; // skip header
            memcpy(eth_rcv_buf, au8packet - 6, MTU+200);

            tstrM2MDataBufCtrl *PstrM2mIpCtrlBuf =( tstrM2MDataBufCtrl *)pvCtrlBuf;
            eth_rcv_size = PstrM2mIpCtrlBuf->u16DataSize + 6; // skip header

            LOG("Ethernet Frame Received buffer, Size = %d, Remaining = %d, Data offset = %d, Ifc ID = %d",
                PstrM2mIpCtrlBuf->u16DataSize,
                PstrM2mIpCtrlBuf->u16RemainigDataSize,
                PstrM2mIpCtrlBuf->u8DataOffset,
                PstrM2mIpCtrlBuf->u8IfcId);

            ethernetRX();

            break;
        default:
            LOG("Unknown Ethernet message type received.");
            break;
    }
}

void print_mac(uint8_t *mac) {
    printf("%02X:%02X:%02X:%02X:%02X:%02X\n\r",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

errNetworking_t Wifi_Init(void){

    // nm_bsp_init();

    LOG("Starting Up Wifi Init\n\r");

    wifi_init_param.pfAppWifiCb = &wifi_callback;
    wifi_init_param.pfAppMonCb = NULL;
    wifi_init_param.strEthInitParam.au8ethRcvBuf = irq_rcv_buf;
    wifi_init_param.strEthInitParam.u16ethRcvBufSize = MTU+200;
    wifi_init_param.strEthInitParam.pfAppEthCb = &eth_callback;

    int8_t ret = m2m_wifi_init(&wifi_init_param);
    if(ret == M2M_SUCCESS){
        LOG("Wifi Init Sucess\n\r");
        ret = NETWORKING_SUCCESS;
    }else{
       LOG("Failed with exit code: %d\n\r", ret);
        ret = WIFI_INIT_FAIL;
    }
    return ret;    
}

errNetworking_t get_mac(void) {
    uint8_t mac_ap[6];    // MAC for AP interface
    uint8_t mac_sta[6];   // MAC for STA interface
    
    int ret = NETWORKING_SUCCESS;
    if (m2m_wifi_get_mac_address(mac_ap, mac_sta) == M2M_SUCCESS) {
        LOG("AP MAC:  ");
        print_mac(mac_ap);

        LOG("STA MAC: ");
        print_mac(mac_sta);

        // Set the host MAC address in little endian
        // Will be flipped when actually sending out
        setHostMac(mac_sta);
    } else {
        LOG("Failed to get MAC addresses\n");
        ret = GET_MAC_FAIL;
    }
    return ret;
}

// errNetworking_t List_SSID(void){
//     int ret = NETWORKING_SUCCESS;
//     LOG("Requesting Scan");
//     ret = m2m_wifi_request_scan(M2M_WIFI_CH_11);
//     if(ret == M2M_SUCCESS){
//         LOG("Scan Request Sucessful");
//     }else{
//         LOG("Scan Request Failed");
//         ret = LIST_SSID_FAIL;
//     }
//     return ret; 
// }

#define NETWORK_COMMAND_FIFO_SIZE 32
OS_FIFO_t network_command_fifo;
sema4_t network_command_sema4[3];
network_command_t network_command_fifo_buffer[NETWORK_COMMAND_FIFO_SIZE];

sema4_t receiveIRQ_sem4;

void Network_Receive_IRQ(void){
    OS_Signal(&receiveIRQ_sem4);
}

void Network_Scan(void){
    LOG("Starting Scan");
    network_command_t cmd;
    cmd.command = NW_SCAN;
    memset(cmd.data, 0, sizeof(cmd.data));

    OS_Fifo_Put((uint8_t*)&cmd, &network_command_fifo);
    OS_Wait(&data_captured);
    LOG("Received: %d", irqReceiveType);

    // parse data
    tstrM2mScanDone data1; 
    memcpy(&data1, (tstrM2mScanDone*)msg, sizeof(tstrM2mScanDone));

    LOG("Num channels: %d", data1.u8NumofCh);

    for(int i=0; i<data1.u8NumofCh; i++){
        cmd.command = NW_GET_SCAN_DATA;
        cmd.data[0] = i;
        OS_Fifo_Put((uint8_t*)&cmd, &network_command_fifo);
        LOG("Waiting for scan result %d", i);
        
        OS_Wait(&data_captured);
        
        tstrM2mWifiscanResult *data2 = (tstrM2mWifiscanResult*)msg;
        LOG("Scan Result:");
        LOG("Index: %d", data2->u8index);
        LOG("RSSI: %d", data2->s8rssi);
        LOG("Auth Type: %d", data2->u8AuthType);
        LOG("Channel: %d", data2->u8ch);
        LOG("BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
            data2->au8BSSID[0], data2->au8BSSID[1], data2->au8BSSID[2],
            data2->au8BSSID[3], data2->au8BSSID[4], data2->au8BSSID[5]);
        LOG("SSID: %s", data2->au8SSID);
        LOG("Device Name: %s", data2->au8DeviceName);
    }
    LOG("Scan Done");
}

void Network_Connect(char *ssid, char *password){
    network_command_t cmd;
    cmd.command = NW_CONNECT;
    memset(cmd.data, 0, sizeof(cmd.data));

    strncpy((char*)cmd.data, ssid, 32);
    strncpy((char*)cmd.data + 32, password, 40);

    OS_Fifo_Put((uint8_t*)&cmd, &network_command_fifo);
    OS_Wait(&data_captured);

    tstrM2mWifiStateChanged *data = (tstrM2mWifiStateChanged*)msg;
    // LOG("Connected to AP: %s %s", ssid, password);
    if(data->u8CurrState == M2M_WIFI_CONNECTED){
        LOG("Connected to AP: %s %s", ssid, password);
    }else if(data->u8CurrState == M2M_WIFI_DISCONNECTED){
        LOG("Connection failed: %s %s", ssid, password);
    }
}

void Network_Disconnect(void){
    network_command_t cmd;
    cmd.command = NW_DISCONNECT;
    memset(cmd.data, 0, sizeof(cmd.data));

    OS_Fifo_Put((uint8_t*)&cmd, &network_command_fifo);
}

void Network_Send_Raw(void){
    network_command_t cmd;
    cmd.command = NW_SEND_RAW;

    OS_Fifo_Put((uint8_t*)&cmd, &network_command_fifo);
}

void Network_Receive_Raw(void){
    network_command_t cmd;
    cmd.command = NW_RECEIVE_RAW;
    memset(cmd.data, 0, sizeof(cmd.data));

    OS_Fifo_Put((uint8_t*)&cmd, &network_command_fifo);
}

void Network_Request_Scan_result(void){
    network_command_t cmd;
    cmd.command = NW_SCAN;
    memset(cmd.data, 0, sizeof(cmd.data));

    OS_Fifo_Put((uint8_t*)&cmd, &network_command_fifo);
}

void Network_Get_Mac(void){
    network_command_t cmd;
    cmd.command = NW_GET_MAC;
    memset(cmd.data, 0, sizeof(cmd.data));
    OS_Fifo_Put((uint8_t*)&cmd, &network_command_fifo);
}

sema4_t wifi_mutex;
void Task_NetworkThread(void){
    
    while(1){
        // Handle incoming commands
        network_command_t cmd;
        OS_Fifo_Get((uint8_t*)&cmd, &network_command_fifo);
        
        // LOG("Command received: %d", cmd.command);

        // LOG("Fifo dump:");
        // OS_Fifo_Print(&network_command_fifo);
        
        OS_Wait(&wifi_mutex);
        sint8 res;
        switch(cmd.command){
            case NW_SCAN:
                LOG("Scan command received");
                // Call the scan function here
                res = m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
                if(res == M2M_SUCCESS)
                    LOG("Scan request sent");
                break;
            case NW_GET_SCAN_DATA:
                LOG("Get scan data command received");
                // Call the get scan data function here
                res = m2m_wifi_req_scan_result(cmd.data[0]);
                if(res == M2M_SUCCESS)
                    LOG("Scan data request sent");
                break;
            case NW_CONNECT:
                LOG("Connect command received");
                // Call the connect function here
                char ssid[32];
                tuniM2MWifiAuth auth_param = {0};
                
                strncpy(ssid, cmd.data, 32);
                strncpy(auth_param.au8PMK, cmd.data + 32, 40);
                LOG("SSID: %s", ssid);
                LOG("Password: %s", auth_param.au8PMK);

                LOG("Connecting to SSID: %s", ssid);
                res = m2m_wifi_connect(ssid, strlen(ssid), M2M_WIFI_SEC_WPA_PSK, &auth_param, M2M_WIFI_CH_ALL);
                if(res == M2M_SUCCESS)
                    LOG("Connection request sent");

                break;
            case NW_DISCONNECT:
                LOG("Disconnect command received");
                // Call the disconnect function here
                m2m_wifi_disconnect();
                LOG("Disconnect request sent");

                break;
            case NW_GET_MAC:
                LOG("Get MAC command received");
                get_mac();
                break;
            case NW_SEND_RAW:
                LOG("Send raw command received");
                // Call the send raw function here
                m2m_wifi_send_ethernet_pkt(curr_send_buf, curr_send_size, STATION_INTERFACE);
                break;            
            default:
                LOG("Unknown command received");
                break;
            
        }
        OS_Signal(&wifi_mutex); 
    }
}

void Task_ReceiveIRQ(void){
    while(1){
        OS_Wait(&receiveIRQ_sem4);
        LOG("IRQ received");
        
        OS_Wait(&wifi_mutex);
        m2m_wifi_handle_events(NULL);
        OS_Signal(&wifi_mutex);
    }
}

void Task_NetworkingInit(){
    OS_Fifo_Init(NETWORK_COMMAND_FIFO_SIZE, 
        &network_command_fifo, 
        (uint8_t*)network_command_fifo_buffer,
        sizeof(network_command_t),
        network_command_sema4);
    OS_InitSemaphore(&receiveIRQ_sem4, 0);
    OS_InitSemaphore(&data_captured, 0);
    OS_InitSemaphore(&data_captured2, 0);
    OS_InitSemaphore(&wifi_mutex, 1);
    OS_InitSemaphore(&log_mtx, 1);
    
    nm_bsp_init();
    LOG("NM BSP init finished\n\r");
    Wifi_Init();
    LOG("Wifi Init finished\n\r");

    m2m_wifi_set_scan_options(4, 240);

    OS_AddThread(Task_ReceiveIRQ, STACKSIZE, 2);
    OS_AddThread(Task_NetworkThread, STACKSIZE, 2);
    OS_AddThread(Task_TestNetworking, STACKSIZE, 3);

    OS_Kill();
}

void ethernetTX(uint8_t* payload, uint16_t size){
    LOG("ethernetTx reached");
    
    prettyprint_payload(payload, size);

    curr_send_buf = payload;
    curr_send_size = size;

    memmove(payload + M2M_ETHERNET_HDR_OFFSET + M2M_ETH_PAD_SIZE, payload, size);    

    // LOG("Sending ethernet packet");
    // LOG("First 6 bytes:");
    // for(int i=0; i<6; i++){
    //     printf("%02x ", payload[i]);
    // }
    // printf("\n\r");
    //for (int i = 0; i < size; i++) {
    //    printf("%02x ", payload[i]);
    //}

    Network_Send_Raw();

    return;
}

void ethernetRX(void){
    LOG("ethernetRX reached");

    macRX(eth_rcv_buf, eth_rcv_size);
    return;
}
