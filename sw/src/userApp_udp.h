#ifndef userApp_udp_c
#define userApp_udp_c


#define USERAPP_UDP_PORT (54321)
#define USERAPP_PAYLOAD_SIZE (64)
    
typedef struct{
    uint16_t datasize;
    uint8_t payloadRXBuf[USERAPP_PAYLOAD_SIZE];
}userAppRX_t;


void userAppApp_UdpExInit(void);
#endif // USERAPP
