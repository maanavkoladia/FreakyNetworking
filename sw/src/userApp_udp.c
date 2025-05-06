/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "OS.h"
#include "Networking_Globs.h"
#include <stdint.h>
#include "UDP.h"
#include "string_lite.h"
#include "userApp_udp.h"
/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */
#define USERAPP_DATA_FIFO_SIZE (3)
#define USERAPP_PAYLOAD_SIZE (64)
#define TEST_TX_IP (0xAC140A07)
#define TEST_TX_PORT (54321)


OS_FIFO_t userAppFifo;
sema4_t userAppFifo_sema4s[3];
uint8_t userAppFifo_buffer[USERAPP_DATA_FIFO_SIZE * sizeof(userAppRX_t)];

userAppRX_t userAppRxBuf;
uint8_t userAppTxBuf[MTU];

const uint8_t testpayload[] = {
    "ishan is a shitter\n\n"
};
/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */
void userPrint_RXData(uint8_t* payload, uint16_t size);
/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */
void Task_UserAPP_UdpRX(void){
    while(1){
        OS_Fifo_Get((uint8_t*)&userAppRxBuf, &userAppFifo);
        userPrint_RXData(userAppRxBuf.payloadRXBuf, userAppRxBuf.datasize);
    }
}

void Task_TestUDPTx(void){
    while(1){
        LOG("Sending User App Data");
        OS_Sleep(5000);
        memcpy(userAppTxBuf, testpayload, sizeof(testpayload));
        udp_tx(sizeof(testpayload), userAppTxBuf, TEST_TX_IP, USERAPP_UDP_PORT, TEST_TX_PORT);
    }
}

void userAppApp_UdpExInit(void){
    LOG("User App started")
    OS_Fifo_Init(USERAPP_DATA_FIFO_SIZE, &userAppFifo, userAppFifo_buffer, sizeof(userAppRX_t), userAppFifo_sema4s);
    OS_AddThread(Task_TestUDPTx, STACKSIZE, 5);
    OS_AddThread(Task_UserAPP_UdpRX, STACKSIZE, 5);
}


void userPrint_RXData(uint8_t* payload, uint16_t size){
    LOG("========== Received Payload (%u bytes) ==========\n", size);
    for(int i = 0; i < size; i++){
        printf("%s", payload);
    }
    LOG("=================================================\n");
}
