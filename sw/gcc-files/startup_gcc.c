//*****************************************************************************
//
// startup_gcc.c - Startup code for use with GNU tools.
//
// Copyright (c) 2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 9453 of the EK-LM4F120XL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include "../inc/hw_nvic.h"
#include "../inc/hw_types.h"
#include "../inc/tm4c123gh6pm.h"
//*****************************************************************************
//
// Forward declaration of the fault handlers.
//
//*****************************************************************************
void ResetISR(void);
void NMI_Handler(void) __attribute__((weak));
void HardFault_Handler(void) __attribute__((weak));
void MemManage_Handler(void) __attribute__((weak));
void BusFault_Handler(void) __attribute__((weak));
void UsageFault_Handler(void) __attribute__((weak));
void SVC_Handler(void) __attribute__((weak));
void DebugMon_Handler(void) __attribute__((weak));
void PendSV_Handler(void) __attribute__((weak));
void SysTick_Handler(void) __attribute__((weak));
void IntDefaultHandler(void) __attribute__((weak));

//*****************************************************************************
//
// Weak prototypes of ISR handlers
//
//*****************************************************************************

void GPIOPortA_Handler(void) __attribute__((weak));
void GPIOPortB_Handler(void) __attribute__((weak));
void GPIOPortC_Handler(void) __attribute__((weak));
void GPIOPortD_Handler(void) __attribute__((weak));
void GPIOPortE_Handler(void) __attribute__((weak));
void UART0_Handler(void) __attribute__((weak));
void UART1_Handler(void) __attribute__((weak));
void SSI0_Handler(void) __attribute__((weak));
void I2C0_Handler(void) __attribute__((weak));
void PWM0Fault_Handler(void) __attribute__((weak));
void PWM0Generator0_Handler(void) __attribute__((weak));
void PWM0Generator1_Handler(void) __attribute__((weak));
void PWM0Generator2_Handler(void) __attribute__((weak));
void Quadrature0_Handler(void) __attribute__((weak));
void ADC0Seq0_Handler(void) __attribute__((weak));
void ADC0Seq1_Handler(void) __attribute__((weak));
void ADC0Seq2_Handler(void) __attribute__((weak));
void ADC0Seq3_Handler(void) __attribute__((weak));
void WDT_Handler(void) __attribute__((weak));
void Timer0A_Handler(void) __attribute__((weak));
void Timer0B_Handler(void) __attribute__((weak));
void Timer1A_Handler(void) __attribute__((weak));
void Timer1B_Handler(void) __attribute__((weak));
void Timer2A_Handler(void) __attribute__((weak));
void Timer2B_Handler(void) __attribute__((weak));
void Comp0_Handler(void) __attribute__((weak));
void Comp1_Handler(void) __attribute__((weak));
void Comp2_Handler(void) __attribute__((weak));
void SysCtl_Handler(void) __attribute__((weak));
void FlashCtl_Handler(void) __attribute__((weak));
void GPIOPortF_Handler(void) __attribute__((weak));
void GPIOPortG_Handler(void) __attribute__((weak));
void GPIOPortH_Handler(void) __attribute__((weak));
void UART2_Handler(void) __attribute__((weak));
void SSI1_Handler(void) __attribute__((weak));
void Timer3A_Handler(void) __attribute__((weak));
void Timer3B_Handler(void) __attribute__((weak));
void I2C1_Handler(void) __attribute__((weak));
void Quadrature1_Handler(void) __attribute__((weak));
void CAN0_Handler(void) __attribute__((weak));
void CAN1_Handler(void) __attribute__((weak));
void CAN2_Handler(void) __attribute__((weak));
void Ethernet_Handler(void) __attribute__((weak));
void Hibernate_Handler(void) __attribute__((weak));
void USB0_Handler(void) __attribute__((weak));
void PWM0Generator3_Handler(void) __attribute__((weak));
void uDMA_Handler(void) __attribute__((weak));
void uDMA_Error(void) __attribute__((weak));
void ADC1Seq0_Handler(void) __attribute__((weak));
void ADC1Seq1_Handler(void) __attribute__((weak));
void ADC1Seq2_Handler(void) __attribute__((weak));
void ADC1Seq3_Handler(void) __attribute__((weak));
void I2S0_Handler(void) __attribute__((weak));
void ExtBus_Handler(void) __attribute__((weak));
void GPIOPortJ_Handler(void) __attribute__((weak));
void GPIOPortK_Handler(void) __attribute__((weak));
void GPIOPortL_Handler(void) __attribute__((weak));
void SSI2_Handler(void) __attribute__((weak));
void SSI3_Handler(void) __attribute__((weak));
void UART3_Handler(void) __attribute__((weak));
void UART4_Handler(void) __attribute__((weak));
void UART5_Handler(void) __attribute__((weak));
void UART6_Handler(void) __attribute__((weak));
void UART7_Handler(void) __attribute__((weak));
void I2C2_Handler(void) __attribute__((weak));
void I2C3_Handler(void) __attribute__((weak));
void Timer4A_Handler(void) __attribute__((weak));
void Timer4B_Handler(void) __attribute__((weak));
void Timer5A_Handler(void) __attribute__((weak));
void Timer5B_Handler(void) __attribute__((weak));
void WideTimer0A_Handler(void) __attribute__((weak));
void WideTimer0B_Handler(void) __attribute__((weak));
void WideTimer1A_Handler(void) __attribute__((weak));
void WideTimer1B_Handler(void) __attribute__((weak));
void WideTimer2A_Handler(void) __attribute__((weak));
void WideTimer2B_Handler(void) __attribute__((weak));
void WideTimer3A_Handler(void) __attribute__((weak));
void WideTimer3B_Handler(void) __attribute__((weak));
void WideTimer4A_Handler(void) __attribute__((weak));
void WideTimer4B_Handler(void) __attribute__((weak));
void WideTimer5A_Handler(void) __attribute__((weak));
void WideTimer5B_Handler(void) __attribute__((weak));
void FPU_Handler(void) __attribute__((weak));
void PECI0_Handler(void) __attribute__((weak));
void LPC0_Handler(void) __attribute__((weak));
void I2C4_Handler(void) __attribute__((weak));
void I2C5_Handler(void) __attribute__((weak));
void GPIOPortM_Handler(void) __attribute__((weak));
void GPIOPortN_Handler(void) __attribute__((weak));
void Quadrature2_Handler(void) __attribute__((weak));
void Fan0_Handler(void) __attribute__((weak));
void GPIOPortP_Handler(void) __attribute__((weak));
void GPIOPortP1_Handler(void) __attribute__((weak));
void GPIOPortP2_Handler(void) __attribute__((weak));
void GPIOPortP3_Handler(void) __attribute__((weak));
void GPIOPortP4_Handler(void) __attribute__((weak));
void GPIOPortP5_Handler(void) __attribute__((weak));
void GPIOPortP6_Handler(void) __attribute__((weak));
void GPIOPortP7_Handler(void) __attribute__((weak));
void GPIOPortQ_Handler(void) __attribute__((weak));
void GPIOPortQ1_Handler(void) __attribute__((weak));
void GPIOPortQ2_Handler(void) __attribute__((weak));
void GPIOPortQ3_Handler(void) __attribute__((weak));
void GPIOPortQ4_Handler(void) __attribute__((weak));
void GPIOPortQ5_Handler(void) __attribute__((weak));
void GPIOPortQ6_Handler(void) __attribute__((weak));
void GPIOPortQ7_Handler(void) __attribute__((weak));
void GPIOPortR_Handler(void) __attribute__((weak));
void GPIOPortS_Handler(void) __attribute__((weak));
void PWM1Generator0_Handler(void) __attribute__((weak));
void PWM1Generator1_Handler(void) __attribute__((weak));
void PWM1Generator2_Handler(void) __attribute__((weak));
void PWM1Generator3_Handler(void) __attribute__((weak));
void PWM1Fault_Handler(void) __attribute__((weak));


// Function prototypes for the inline assembly functions
void DisableInterrupts(void);
void EnableInterrupts(void);
uint32_t StartCritical(void);
void EndCritical(uint32_t primask);
void WaitForInterrupt(void);

//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern int main(void);

//*****************************************************************************
//
// Reserve space for the system stack.
//
//*****************************************************************************
static unsigned long pulStack[64];

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
    (void (*)(void))((unsigned long)pulStack + sizeof(pulStack)),
                                            // The initial stack pointer
    ResetISR,                               // The reset handler
    NMI_Handler,                                  // The NMI handler
    HardFault_Handler,                               // The hard fault handler
    MemManage_Handler,                      // The MPU fault handler
    BusFault_Handler,                      // The bus fault handler
    UsageFault_Handler,                      // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    SVC_Handler,                      // SVCall handler
    DebugMon_Handler,                      // Debug monitor handler
    0,                                      // Reserved
    PendSV_Handler,                      // The PendSV handler
    SysTick_Handler,                      // The SysTick handler
    GPIOPortA_Handler,                      // GPIO Port A
    GPIOPortB_Handler,                      // GPIO Port B
    GPIOPortC_Handler,                      // GPIO Port C
    GPIOPortD_Handler,                      // GPIO Port D
    GPIOPortE_Handler,                      // GPIO Port E
    UART0_Handler,                      // UART0 Rx and Tx
    UART1_Handler,                      // UART1 Rx and Tx
    SSI0_Handler,                      // SSI0 Rx and Tx
    I2C0_Handler,                      // I2C0 Master and Slave
    PWM0Fault_Handler,                      // PWM Fault
    PWM0Generator0_Handler,                      // PWM Generator 0
    PWM0Generator1_Handler,                      // PWM Generator 1
    PWM0Generator2_Handler,                      // PWM Generator 2
    Quadrature0_Handler,                      // Quadrature Encoder 0
    ADC0Seq0_Handler,                      // ADC Sequence 0
    ADC0Seq1_Handler,                      // ADC Sequence 1
    ADC0Seq2_Handler,                      // ADC Sequence 2
    ADC0Seq3_Handler,                      // ADC Sequence 3
    WDT_Handler,                      // Watchdog timer
    Timer0A_Handler,                      // Timer 0 subtimer A
    Timer0B_Handler,                      // Timer 0 subtimer B
    Timer1A_Handler,                      // Timer 1 subtimer A
    Timer1B_Handler,                      // Timer 1 subtimer B
    Timer2A_Handler,                      // Timer 2 subtimer A
    Timer2B_Handler,                      // Timer 2 subtimer B
    Comp0_Handler,                      // Analog Comparator 0
    Comp1_Handler,                      // Analog Comparator 1
    Comp2_Handler,                      // Analog Comparator 2
    SysCtl_Handler,                      // System Control (PLL, OSC, BO)
    FlashCtl_Handler,                      // FLASH Control
    GPIOPortF_Handler,                      // GPIO Port F
    GPIOPortG_Handler,                      // GPIO Port G
    GPIOPortH_Handler,                      // GPIO Port H
    UART2_Handler,                      // UART2 Rx and Tx
    SSI1_Handler,                      // SSI1 Rx and Tx
    Timer3A_Handler,                      // Timer 3 subtimer A
    Timer3B_Handler,                      // Timer 3 subtimer B
    I2C1_Handler,                      // I2C1 Master and Slave
    Quadrature1_Handler,                      // Quadrature Encoder 1
    CAN0_Handler,                      // CAN0
    CAN1_Handler,                      // CAN1
    CAN2_Handler,                      // CAN2
    Ethernet_Handler,                      // Ethernet
    Hibernate_Handler,                      // Hibernate
    USB0_Handler,                      // USB0
    PWM0Generator3_Handler,                      // PWM Generator 3
    uDMA_Handler,                      // uDMA Software Transfer
    uDMA_Error,                      // uDMA Error
    ADC1Seq0_Handler,                      // ADC1 Sequence 0
    ADC1Seq1_Handler,                      // ADC1 Sequence 1
    ADC1Seq2_Handler,                      // ADC1 Sequence 2
    ADC1Seq3_Handler,                      // ADC1 Sequence 3
    I2S0_Handler,                      // I2S0
    ExtBus_Handler,                      // External Bus Interface 0
    GPIOPortJ_Handler,                      // GPIO Port J
    GPIOPortK_Handler,                      // GPIO Port K
    GPIOPortL_Handler,                      // GPIO Port L
    SSI2_Handler,                      // SSI2 Rx and Tx
    SSI3_Handler,                      // SSI3 Rx and Tx
    UART3_Handler,                      // UART3 Rx and Tx
    UART4_Handler,                      // UART4 Rx and Tx
    UART5_Handler,                      // UART5 Rx and Tx
    UART6_Handler,                      // UART6 Rx and Tx
    UART7_Handler,                      // UART7 Rx and Tx
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    I2C2_Handler,                      // I2C2 Master and Slave
    I2C3_Handler,                      // I2C3 Master and Slave
    Timer4A_Handler,                      // Timer 4 subtimer A
    Timer4B_Handler,                      // Timer 4 subtimer B
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    Timer5A_Handler,                      // Timer 5 subtimer A
    Timer5B_Handler,                      // Timer 5 subtimer B
    WideTimer0A_Handler,                      // Wide Timer 0 subtimer A
    WideTimer0B_Handler,                      // Wide Timer 0 subtimer B
    WideTimer1A_Handler,                      // Wide Timer 1 subtimer A
    WideTimer1B_Handler,                      // Wide Timer 1 subtimer B
    WideTimer2A_Handler,                      // Wide Timer 2 subtimer A
    WideTimer2B_Handler,                      // Wide Timer 2 subtimer B
    WideTimer3A_Handler,                      // Wide Timer 3 subtimer A
    WideTimer3B_Handler,                      // Wide Timer 3 subtimer B
    WideTimer4A_Handler,                      // Wide Timer 4 subtimer A
    WideTimer4B_Handler,                      // Wide Timer 4 subtimer B
    WideTimer5A_Handler,                      // Wide Timer 5 subtimer A
    WideTimer5B_Handler,                      // Wide Timer 5 subtimer B
    FPU_Handler,                      // FPU
    PECI0_Handler,                      // PECI 0
    LPC0_Handler,                      // LPC 0
    I2C4_Handler,                      // I2C4 Master and Slave
    I2C5_Handler,                      // I2C5 Master and Slave
    GPIOPortM_Handler,                      // GPIO Port M
    GPIOPortN_Handler,                      // GPIO Port N
    Quadrature2_Handler,                      // Quadrature Encoder 2
    Fan0_Handler,                      // Fan 0
    0,                                      // Reserved
    GPIOPortP_Handler,                      // GPIO Port P (Summary or P0)
    GPIOPortP1_Handler,                      // GPIO Port P1
    GPIOPortP2_Handler,                      // GPIO Port P2
    GPIOPortP3_Handler,                      // GPIO Port P3
    GPIOPortP4_Handler,                      // GPIO Port P4
    GPIOPortP5_Handler,                      // GPIO Port P5
    GPIOPortP6_Handler,                      // GPIO Port P6
    GPIOPortP7_Handler,                      // GPIO Port P7
    GPIOPortQ_Handler,                      // GPIO Port Q (Summary or Q0)
    GPIOPortQ1_Handler,                      // GPIO Port Q1
    GPIOPortQ2_Handler,                      // GPIO Port Q2
    GPIOPortQ3_Handler,                      // GPIO Port Q3
    GPIOPortQ4_Handler,                      // GPIO Port Q4
    GPIOPortQ5_Handler,                      // GPIO Port Q5
    GPIOPortQ6_Handler,                      // GPIO Port Q6
    GPIOPortQ7_Handler,                      // GPIO Port Q7
    GPIOPortR_Handler,                      // GPIO Port R
    GPIOPortS_Handler,                      // GPIO Port S
    PWM1Generator0_Handler,                      // PWM 1 Generator 0
    PWM1Generator1_Handler,                      // PWM 1 Generator 1
    PWM1Generator2_Handler,                      // PWM 1 Generator 2
    PWM1Generator3_Handler,                      // PWM 1 Generator 3
    PWM1Fault_Handler                       // PWM 1 Fault
};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void
ResetISR(void)
{
    unsigned long *pulSrc, *pulDest;

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    pulSrc = &_etext;
    for(pulDest = &_data; pulDest < &_edata; )
    {
        *pulDest++ = *pulSrc++;
    }

    //
    // Zero fill the bss segment.
    //
    __asm("    ldr     r0, =_bss\n"
          "    ldr     r1, =_ebss\n"
          "    mov     r2, #0\n"
          "    .thumb_func\n"
          "zero_loop:\n"
          "        cmp     r0, r1\n"
          "        it      lt\n"
          "        strlt   r2, [r0], #4\n"
          "        blt     zero_loop");

    //
    // Enable the floating-point unit.  This must be done here to handle the
    // case where main() uses floating-point and the function prologue saves
    // floating-point registers (which will fault if floating-point is not
    // enabled).  Any configuration of the floating-point unit using DriverLib
    // APIs must be done here prior to the floating-point unit being enabled.
    //
    // Note that this does not use DriverLib since it might not be included in
    // this project.
    //
    HWREG(NVIC_CPAC) = ((HWREG(NVIC_CPAC) &
                         ~(NVIC_CPAC_CP10_M | NVIC_CPAC_CP11_M)) |
                        NVIC_CPAC_CP10_FULL | NVIC_CPAC_CP11_FULL);

    //
    // Call the application's entry point.
    //
    main();
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
void NMI_Handler(void){
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
void HardFault_Handler(void){
    //
    // Enter an infinite loop.
    //
    GPIO_PORTF_DATA_R |= 0x0E; 
    while(1)
    {
    }
}

void MemManage_Handler(void){
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

void BusFault_Handler(void){
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

void UsageFault_Handler(void){
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

void SVC_Handler(void){
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

void DebugMon_Handler(void){
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

void PendSV_Handler(void){
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

void SysTick_Handler(void){
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************

void IntDefaultHandler(void){
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//calls the default handler if the ISR is undefined
//*****************************************************************************

void GPIOPortA_Handler(void){
    IntDefaultHandler();
}

void GPIOPortB_Handler(void){
    IntDefaultHandler();
}

void GPIOPortC_Handler(void){
    IntDefaultHandler();
}

void GPIOPortD_Handler(void){
    IntDefaultHandler();
}

void GPIOPortE_Handler(void){
    IntDefaultHandler();
}

void UART0_Handler(void){
    IntDefaultHandler();
}

void UART1_Handler(void){
    IntDefaultHandler();
}

void SSI0_Handler(void){
    IntDefaultHandler();
}

void I2C0_Handler(void){
    IntDefaultHandler();
}

void PWM0Fault_Handler(void){
    IntDefaultHandler();
}

void PWM0Generator0_Handler(void){
    IntDefaultHandler();
}

void PWM0Generator1_Handler(void){
    IntDefaultHandler();
}

void PWM0Generator2_Handler(void){
    IntDefaultHandler();
}

void Quadrature0_Handler(void){
    IntDefaultHandler();
}

void ADC0Seq0_Handler(void){
    IntDefaultHandler();
}

void ADC0Seq1_Handler(void){
    IntDefaultHandler();
}

void ADC0Seq2_Handler(void){
    IntDefaultHandler();
}

void ADC0Seq3_Handler(void){
    IntDefaultHandler();
}

void WDT_Handler(void){
    IntDefaultHandler();
}

void Timer0A_Handler(void){
    IntDefaultHandler();
}

void Timer0B_Handler(void){
    IntDefaultHandler();
}

void Timer1A_Handler(void){
    IntDefaultHandler();
}

void Timer1B_Handler(void){
    IntDefaultHandler();
}

void Timer2A_Handler(void){
    IntDefaultHandler();
}

void Timer2B_Handler(void){
    IntDefaultHandler();
}

void Comp0_Handler(void){
    IntDefaultHandler();
}

void Comp1_Handler(void){
    IntDefaultHandler();
}

void Comp2_Handler(void){
    IntDefaultHandler();
}

void SysCtl_Handler(void){
    IntDefaultHandler();
}

void FlashCtl_Handler(void){
    IntDefaultHandler();
}

void GPIOPortF_Handler(void){
    IntDefaultHandler();
}

void GPIOPortG_Handler(void){
    IntDefaultHandler();
}

void GPIOPortH_Handler(void){
    IntDefaultHandler();
}

void UART2_Handler(void){
    IntDefaultHandler();
}

void SSI1_Handler(void){
    IntDefaultHandler();
}

void Timer3A_Handler(void){
    IntDefaultHandler();
}

void Timer3B_Handler(void){
    IntDefaultHandler();
}

void I2C1_Handler(void){
    IntDefaultHandler();
}

void Quadrature1_Handler(void){
    IntDefaultHandler();
}

void CAN0_Handler(void){
    IntDefaultHandler();
}

void CAN1_Handler(void){
    IntDefaultHandler();
}

void CAN2_Handler(void){
    IntDefaultHandler();
}

void Ethernet_Handler(void){
    IntDefaultHandler();
}

void Hibernate_Handler(void){
    IntDefaultHandler();
}

void USB0_Handler(void){
    IntDefaultHandler();
}

void PWM0Generator3_Handler(void){
    IntDefaultHandler();
}

void uDMA_Handler(void){
    IntDefaultHandler();
}

void uDMA_Error(void){
    IntDefaultHandler();
}

void ADC1Seq0_Handler(void){
    IntDefaultHandler();
}

void ADC1Seq1_Handler(void){
    IntDefaultHandler();
}

void ADC1Seq2_Handler(void){
    IntDefaultHandler();
}

void ADC1Seq3_Handler(void){
    IntDefaultHandler();
}

void I2S0_Handler(void){
    IntDefaultHandler();
}

void ExtBus_Handler(void){
    IntDefaultHandler();
}

void GPIOPortJ_Handler(void){
    IntDefaultHandler();
}

void GPIOPortK_Handler(void){
    IntDefaultHandler();
}

void GPIOPortL_Handler(void){
    IntDefaultHandler();
}

void SSI2_Handler(void){
    IntDefaultHandler();
}

void SSI3_Handler(void){
    IntDefaultHandler();
}

void UART3_Handler(void){
    IntDefaultHandler();
}

void UART4_Handler(void){
    IntDefaultHandler();
}

void UART5_Handler(void){
    IntDefaultHandler();
}

void UART6_Handler(void){
    IntDefaultHandler();
}

void UART7_Handler(void){
    IntDefaultHandler();
}

void I2C2_Handler(void){
    IntDefaultHandler();
}

void I2C3_Handler(void){
    IntDefaultHandler();
}

void Timer4A_Handler(void){
    IntDefaultHandler();
}

void Timer4B_Handler(void){
    IntDefaultHandler();
}

void Timer5A_Handler(void){
    IntDefaultHandler();
}

void Timer5B_Handler(void){
    IntDefaultHandler();
}

void WideTimer0A_Handler(void){
    IntDefaultHandler();
}

void WideTimer0B_Handler(void){
    IntDefaultHandler();
}

void WideTimer1A_Handler(void){
    IntDefaultHandler();
}

void WideTimer1B_Handler(void){
    IntDefaultHandler();
}

void WideTimer2A_Handler(void){
    IntDefaultHandler();
}

void WideTimer2B_Handler(void){
    IntDefaultHandler();
}

void WideTimer3A_Handler(void){
    IntDefaultHandler();
}

void WideTimer3B_Handler(void){
    IntDefaultHandler();
}

void WideTimer4A_Handler(void){
    IntDefaultHandler();
}

void WideTimer4B_Handler(void){
    IntDefaultHandler();
}

void WideTimer5A_Handler(void){
    IntDefaultHandler();
}

void WideTimer5B_Handler(void){
    IntDefaultHandler();
}

void FPU_Handler(void){
    IntDefaultHandler();
}

void PECI0_Handler(void){
    IntDefaultHandler();
}

void LPC0_Handler(void){
    IntDefaultHandler();
}

void I2C4_Handler(void){
    IntDefaultHandler();
}

void I2C5_Handler(void){
    IntDefaultHandler();
}

void GPIOPortM_Handler(void){
    IntDefaultHandler();
}

void GPIOPortN_Handler(void){
    IntDefaultHandler();
}

void Quadrature2_Handler(void){
    IntDefaultHandler();
}

void Fan0_Handler(void){
    IntDefaultHandler();
}

void GPIOPortP_Handler(void){
    IntDefaultHandler();
}

void GPIOPortP1_Handler(void){
    IntDefaultHandler();
}

void GPIOPortP2_Handler(void){
    IntDefaultHandler();
}

void GPIOPortP3_Handler(void){
    IntDefaultHandler();
}

void GPIOPortP4_Handler(void){
    IntDefaultHandler();
}

void GPIOPortP5_Handler(void){
    IntDefaultHandler();
}

void GPIOPortP6_Handler(void){
    IntDefaultHandler();
}

void GPIOPortP7_Handler(void){
    IntDefaultHandler();
}

void GPIOPortQ_Handler(void){
    IntDefaultHandler();
}

void GPIOPortQ1_Handler(void){
    IntDefaultHandler();
}

void GPIOPortQ2_Handler(void){
    IntDefaultHandler();
}

void GPIOPortQ3_Handler(void){
    IntDefaultHandler();
}

void GPIOPortQ4_Handler(void){
    IntDefaultHandler();
}

void GPIOPortQ5_Handler(void){
    IntDefaultHandler();
}

void GPIOPortQ6_Handler(void){
    IntDefaultHandler();
}

void GPIOPortQ7_Handler(void){
    IntDefaultHandler();
}

void GPIOPortR_Handler(void){
    IntDefaultHandler();
}

void GPIOPortS_Handler(void){
    IntDefaultHandler();
}

void PWM1Generator0_Handler(void){
    IntDefaultHandler();
}

void PWM1Generator1_Handler(void){
    IntDefaultHandler();
}

void PWM1Generator2_Handler(void){
    IntDefaultHandler();
}

void PWM1Generator3_Handler(void){
    IntDefaultHandler();
}

void PWM1Fault_Handler(void){
    IntDefaultHandler();
}

//*****************************************************************************
//these are the funcitons that DR.Valvano gave us in his startup.s file for keil
//*****************************************************************************
// // Disable interrupts
// void DisableInterrupts(void) {
//     __asm volatile ("CPSID I" : : : "memory");
// }

// // Enable interrupts
// void EnableInterrupts(void) {
//     __asm volatile ("CPSIE I" : : : "memory");
// }

// // Make a copy of the current interrupt state and disable interrupts
// uint32_t StartCritical(void) {
//     uint32_t primask;
//     __asm volatile ("MRS %0, PRIMASK" : "=r" (primask) :: "memory");
//     __asm volatile ("CPSID I" : : : "memory");
//     return primask;
// }

// // Restore the previous interrupt state using the copy of PRIMASK
// void EndCritical(uint32_t primask) {
//     __asm volatile ("MSR PRIMASK, %0" : : "r" (primask) : "memory");
// }

// // Wait for an interrupt (put the processor in low power mode)
// void WaitForInterrupt(void) {
//     __asm volatile ("WFI" : : : "memory");
// }

//*****************************************************************************
//these are ISR handlers
//*****************************************************************************
