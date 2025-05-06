#include "DHCP.h"
#include "OS.h"

#include "dhcp_client.h"

sema4_t dhcp_offer_sema4;
sema4_t dhcp_ack_sema4;

void signal_dhcp_offer(void){
    // Signal the semaphore for DHCP offer
    OS_Signal(&dhcp_offer_sema4);
}

void StartUserApps(void);
void Task_DHCPClient(void){
    // DHCP client task implementation
    // This function will handle the DHCP process
    // It will send DHCP discover, request, and handle responses
    
    // Initialize DHCP semaphores
    OS_InitSemaphore(&dhcp_offer_sema4, 0);
    OS_InitSemaphore(&dhcp_ack_sema4, 0);

    register_dhcp_callback(signal_dhcp_offer);

    while(1){
        // Wait for a DHCP offer
        while(dhcp_offer_sema4.Value == 0){ // busy wait
            // Send DHCP discover
            dhcp_send_discover();
            OS_Sleep(5000);
        }
        // OS_Wait(&dhcp_offer_sema4);
        
        // Send DHCP request
        dhcp_send_request();
        
        // Sleep for ACK
        OS_Sleep(5000);
        
        StartUserApps();

        OS_Kill();
    }
}