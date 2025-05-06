/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "main.h"
#include "../inc/tm4c123gh6pm.h"
#include "../inc/PLL.h"
#include "../inc/Timer0A.h"
#include "../inc/Timer1A.h"
#include "../inc/LaunchPad.h"
#include "../inc/UART.h"
#include "../inc/SysTick.h"

#include "bsp/include/nm_bsp.h"
#include "bus_wrapper/include/nm_bus_wrapper.h"
#include "driver/include/m2m_wifi.h"

#include "dhcp_client.h"
#include "ping.h"
#include "arp.h"

#include "Networking.h"
#include "Networking_Globs.h"
#include "printf.h"
#include <stdint.h>
#include "OS.h"
#include "userApp_udp.h"
/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */
#define PD0  (*((volatile uint32_t *)0x40007004))
#define PD1  (*((volatile uint32_t *)0x40007008))
#define PD2  (*((volatile uint32_t *)0x40007010))
#define PD3  (*((volatile uint32_t *)0x40007020))

/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */
void PortD_Init(void);
/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */
void StartupDelay(void);

/* ================================================== */
/*                    MAIN FUNCTION                   */
/* ================================================== */

static uint8_t inbuf[64]; // into the chip (out of TM4c)
static uint8_t outbuf[64]; // out of the chip (into TM4c)

int test_spi(void){
  tstrNmSpiRw spi_rw = {
    .pu8InBuf = inbuf,
    .pu8OutBuf = outbuf,
    .u16Sz = 16
  };
    
  for(int i=0;i<16;i++) inbuf[i] = i*2;

    while(1){
        printf("statring printf test\n\r");
        nm_bus_ioctl(0, &spi_rw);
        nm_bsp_sleep(80000000);
        printf("passed spi test\n\r");
    }
}

int test_spi_dma(void){
  tstrNmSpiRw spi_rw = {
    .pu8InBuf = inbuf,
    .pu8OutBuf = outbuf,
    .u16Sz = 64
  };

  for(int i=0;i<64;i++) inbuf[i] = i*2;
    while(1){
        nm_bus_ioctl(0, &spi_rw);
        nm_bsp_sleep(80000000);
    }
  while(1){}
}

void TestIRQPin(void){
    printf("IRQ function reached\n\r");
}

void HeartBeat(void){
    PD0 ^= 0x1; // toggle PF2, blue led
    //printf("thump\n\r");
}

// Only after success on DHCP
void StartUserApps(void){
    LOG("Starting User Apps");
    OS_AddThread(Task_Ping, STACKSIZE, 3);
    arp_init();
    userAppApp_UdpExInit();
}

void Task_Heartbeat(void){
    while(1){
      //printf("TestThread Sleeping\n\r");
      GPIO_PORTF_DATA_R ^= 0x04;
      OS_Sleep(1000);
    }
}

void Task_TestNetworking(void){
    Network_Get_Mac();
    OS_Sleep(1000);
    // Network_Scan();
    // OS_Sleep(1000);
    Network_Connect("iPhone (3)", "abcdefgh");

    OS_AddThread(Task_DHCPClient, STACKSIZE, 5);
    OS_AddThread(Task_Heartbeat, STACKSIZE, 7);
    OS_Kill();
}

void IdleThread(void){
  while(1){
  }
}

int main(){
    OS_Init();
    PortD_Init();
    //OS_AddPeriodicThread(HeartBeat, 80000000, 1);
    
    OS_AddThread(Task_NetworkingInit, STACKSIZE, 1);

    OS_AddThread(IdleThread, STACKSIZE, 7);
    //OS_AddThread(HeartBeat, 128, 6);
    OS_Launch(TIME_2MS);
    while(1){

    }
}

void PortD_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x08;       // activate port D
  while((SYSCTL_RCGCGPIO_R&0x08)==0){};      
  GPIO_PORTD_DIR_R |= 0xFF;        // make PD3-0 output heartbeats
  GPIO_PORTD_AFSEL_R &= ~0xFF;     // disable alt funct on PD3-0
  GPIO_PORTD_DEN_R |= 0xFF;        // enable digital I/O on PD3-0
  GPIO_PORTD_PCTL_R = ~0xFFFFFFFF;
  GPIO_PORTD_AMSEL_R &= ~0xFF;;    // disable analog functionality on PD
}

void _exit(int status)
{
    (void)status;
    while (1) {
        __asm__("wfi");
    }
}
