/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "SensorIn.h"
#include "../inc/tm4c123gh6pm.h"
#include "stdint.h"
#include "../inc/Timer0A.h"
#include "../inc/Timer1A.h"
#include "../inc/Timer2A.h"
#include "../inc/Timer3A.h"
#include "main.h"

/* ================================================== */
/*            GLOBAL VARIABLE DEFINITIONS             */
/* ================================================== */
#define NUMBER_OF_SENSORS 24
#define NUMBER_OF_CHANNELS 8
#define ADC_SAMPLING_PERIOD 40000 //1 s
//ssi0 8 channels, then ssi1 8 channels, then ssi2 8 channels
uint16_t ADC_Vals_Buf[NUMBER_OF_SENSORS];
uint8_t ADC_Vals_Buf_idx;

#define CS_DELAY 50

/* ================================================== */
/*            FUNCTION PROTOTYPES (DECLARATIONS)      */
/* ================================================== */


/* ================================================== */
/*                 FUNCTION DEFINITIONS               */
/* ================================================== */

void ADC_IC_Init(void) {
    // Enable SSI0, SSI2, SSI3

    // ---- Configure Port A for SSI0 (PA2-PA5) ----
    // PA2 = SSI0Clk (Clock)
    // PA3 = SSI0Fss (Chip Select/Frame Select, used pe2 instead
    // PA4 = SSI0Rx  (MISO, Master In Slave Out)
    // PA5 = SSI0Tx  (MOSI, Master Out Slave In)

    // ---- Configure Port B for SSI2 (PB4-PB7) ----
    // PB4 = SSI2Clk (Clock)
    // PB5 = SSI2Fss (Chip Select/Frame Select), used pe3 instead
    // PB6 = SSI2Rx  (MISO, Master In Slave Out)
    // PB7 = SSI2Tx  (MOSI, Master Out Slave In)

    // ---- Configure Port D for SSI3 (PD0-PD3) ----
    // PD0 = SSI3Clk (Clock)
    // PD1 = SSI3Fss (Chip Select/Frame Select), using pe4 instead
    // PD2 = SSI3Rx  (MISO, Master In Slave Out)
    // PD3 = SSI3Tx  (MOSI, Master Out Slave In)
    // Enable SSI0, SSI1, SSI2
    SYSCTL_RCGCSSI_R |= (SYSCTL_RCGCSSI_R0 | SYSCTL_RCGCSSI_R2 | SYSCTL_RCGCSSI_R3);  // Enable SSI0, SSI2, and SSI3
    while((SYSCTL_PRSSI_R & (SYSCTL_RCGCSSI_R0 | SYSCTL_RCGCSSI_R2 | SYSCTL_RCGCSSI_R3)) == 0) {}  // Wait until SSI0, SSI2, and SSI3 are ready
    
    // Enable clocks for Port A, B, D
    SYSCTL_RCGCGPIO_R |= (SYSCTL_RCGCGPIO_R0 | SYSCTL_RCGCGPIO_R1 | SYSCTL_RCGCGPIO_R3);  // Enable clocks for Port A, B, D
    while((SYSCTL_PRGPIO_R & (SYSCTL_RCGCGPIO_R0 | SYSCTL_RCGCGPIO_R1 | SYSCTL_RCGCGPIO_R3)) == 0) {}  // Wait until Ports A, B, D are ready

    // ---- Configure Port A for SSI0 (PA2-PA5) ----
    GPIO_PORTA_AFSEL_R |= 0x3C;                    // Enable alt funct on PA2-PA5 
    GPIO_PORTA_DEN_R |= 0x3C;                      // Enable digital I/O on PA2-PA5
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFF0000FF) + 0x00222200; // Configure PA2-PA5 as SSI
    GPIO_PORTA_AMSEL_R &= ~0x3C;                  // Disable analog functionality on PA2-PA5
    GPIO_PORTA_DIR_R |= 0x2C;  //make pa2,3,5 out
    GPIO_PORTA_DIR_R &= ~0x10;  //make pa4 in

    // ---- Configure Port B for SSI2 (PB4-PB7) ----
    GPIO_PORTB_AFSEL_R |= 0xF0;                    // Enable alt funct on PB4-PB7
    GPIO_PORTB_DEN_R |= 0xF0;                      // Enable digital I/O on PB4-PB7
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0x0000FFFF) + 0x22220000; // Configure PB4-PB7 as SSI
    GPIO_PORTB_AMSEL_R &= ~0xF0;                   // Disable analog functionality on PB4-PB7
    GPIO_PORTB_DIR_R |= 0xB0;  //make pb 7,5,4 out 11X1XXX
    GPIO_PORTB_DIR_R &= ~0x40;  //make pb6 in   XX0XXXXX
    //GPIO_PORTB_PDR_R |= 0x80;  // Enable pull-down on PB7


    // Set PB7 (SPI2 MOSI) to 8mA drive strength
    //GPIO_PORTB_DR8R_R |= 0x80;  // 0x80 = 0b10000000, enables 8mA drive on PB7


    // ---- Configure Port D for SSI3 (PD0-PD3) ----
    GPIO_PORTD_AFSEL_R |= 0x0F;                    // Enable alt funct on PD0-PD3
    GPIO_PORTD_DEN_R |= 0x0F;                      // Enable digital I/O on PD0-PD3
    GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0xFFFF0000) + 0x00001111; // Configure PD0-PD3 as SSI
    GPIO_PORTD_AMSEL_R &= ~0x0F;                   // Disable analog functionality on PD0-PD3
    GPIO_PORTD_DIR_R |= 0x0B;  //make pd0,1,4 out XXXX1X11
    GPIO_PORTD_DIR_R &= ~0x04;  //pd2 in XXXXX0XX
    GPIO_PORTD_PUR_R |= 0x08; 
    // ------------------- Configure SSI0 ---------------------
    SSI0_CR1_R = 0x00000000;    // Disable SSI to configure
    SSI0_CC_R = 0;  //use system clock
    SSI0_CPSR_R = 10;           // Set CPSR = 10 (prescale divisor)
    SSI0_CR0_R = 0;             // Clear the CR0 register
    SSI0_CR0_R |= (7 << 8);     // Set SCR = 7 for 1 MHz SSI clock
    SSI0_CR0_R |= (0 << 7);     // Set SPH = 0 (capture data on the first clock edge)
    SSI0_CR0_R |= (0 << 6);     // Set SPO = 0 (clock is idle low)
    SSI0_CR0_R |= (0 << 4);     // Set FRF = 00 (Freescale SPI format)
    SSI0_CR0_R |= 0x07;         // Set DSS = 8 bits (0x07 = 7 for 8-bit data)
    SSI0_CR1_R |= 0x00000002;   // Enable SSI


    //SSI0_CC_R = (SSI0_CC_R & ~SSI_CC_CS_M) + SSI_CC_CS_SYSPLL;

    // ------------------- Configure SSI2 ---------------------
    SSI2_CR1_R = 0x00000000;  // Disable SSI to configure
    SSI2_CC_R = 0;  //use system clock
    SSI2_CPSR_R = 10;       // Set CPSR = 4 (prescale divisor)
    SSI2_CR0_R = 0;           // Clear the CR0 register
    SSI2_CR0_R |= (7 << 8);  // Set SCR = 12 (serial clock rate)
    SSI2_CR0_R |= (0 << 7);   // Set SPH = 0 (capture data on the first clock edge)
    SSI2_CR0_R |= (0 << 6);   // Set SPO = 0 (clock is idle low)
    SSI2_CR0_R |= (0 << 4);   // Set FRF = 00 (Freescale SPI format)
    SSI2_CR0_R |= 0x07;       // Set DSS = 8 bits (0x07 = 7 for 8-bit data)
    SSI2_CR1_R |= 0x00000002; // Enable SSI

    // ------------------- Configure SSI3 ---------------------
    SSI3_CR1_R = 0x00000000;  // Disable SSI to configure
    SSI3_CC_R = 0;  //use system clock
    SSI3_CPSR_R = 10;       // Set CPSR = 4 (prescale divisor)
    SSI3_CR0_R = 0;           // Clear the CR0 register
    SSI3_CR0_R |= (7 << 8);  // Set SCR = 12 (serial clock rate)
    SSI3_CR0_R |= (0 << 7);   // Set SPH = 0 (capture data on the first clock edge)
    SSI3_CR0_R |= (0 << 6);   // Set SPO = 0 (clock is idle low)
    SSI3_CR0_R |= (0 << 4);   // Set FRF = 00 (Freescale SPI format)
    SSI3_CR0_R |= 0x07;       // Set DSS = 8 bits (0x07 = 7 for 8-bit data)
    SSI3_CR1_R |= 0x00000002; // Enable SSI
}

void SSI0_Init(void) {
    // Enable SSI0 and Port A
    SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;   // Enable SSI0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0; // Enable Port A
    while ((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R0) == 0) {} // Wait until Port A is ready

    // Configure Port A for SSI0 (PA2-PA5)
    GPIO_PORTA_AFSEL_R |= 0x3C;               // Enable alt funct on PA2-PA5
    GPIO_PORTA_DEN_R |= 0x3C;                 // Enable digital I/O on PA2-PA5
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFF0000FF) + 0x00222200; // Configure PA2-PA5 as SSI
    GPIO_PORTA_AMSEL_R &= ~0x3C;              // Disable analog functionality on PA2-PA5
    //GPIO_PORTA_DIR_R |= 0x2C;                 // Make PA2, PA3, PA5 output
    //GPIO_PORTA_DIR_R &= ~0x10;                // Make PA4 input (MISO)

    // Configure SSI0
    SSI0_CR1_R = 0x00000000;  // Disable SSI to configure
    SSI0_CC_R = 0;            // Use system clock
    SSI0_CPSR_R = 10;         // Set CPSR = 10 (prescale divisor)
    SSI0_CR0_R = 0;           // Clear the CR0 register
    SSI0_CR0_R |= (7 << 8);   // Set SCR = 7 for 1 MHz SSI clock
    SSI0_CR0_R |= (0 << 7);   // Set SPH = 0 (capture data on the first clock edge)
    SSI0_CR0_R |= (0 << 6);   // Set SPO = 0 (clock is idle low)
    SSI0_CR0_R |= (0 << 4);   // Set FRF = 00 (Freescale SPI format)
    SSI0_CR0_R |= 0x07;       // Set DSS = 8 bits (0x07 = 7 for 8-bit data)
    SSI0_CR1_R |= 0x00000002; // Enable SSI
}

void SSI1_Init(void) {
    // Enable SSI1 and Port F
    SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R1;   // Enable SSI1
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; // Enable Port F
    while ((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R5) == 0) {} // Wait until Port F is ready

    // Unlock Port F if necessary (for PF0)
    GPIO_PORTF_LOCK_R = 0x4C4F434B;  // Unlock Port F
    GPIO_PORTF_CR_R = 0x1F;          // Allow changes to PF0-4

    // Configure Port F pins for SSI1 (PF0: SSI1Rx, PF1: SSI1Tx, PF2: SSI1Clk, PF3: SSI1Fss)
    GPIO_PORTF_AFSEL_R |= 0x0F;                 // Enable alternate function on PF0, PF1, PF2, PF3
    GPIO_PORTF_DEN_R |= 0x0F;                   // Enable digital I/O on PF0, PF1, PF2, PF3
    GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R & 0xFFFF0000) | 0x00002222; // Configure PF0-PF3 as SSI1
    GPIO_PORTF_AMSEL_R &= ~0x0F;                // Disable analog functionality on PF0-PF3
    //GPIO_PORTF_DIR_R |= 0x0A;                   // Make PF1 (Tx) and PF3 (Fss) output
    //GPIO_PORTF_DIR_R &= ~0x05;                  // Make PF0 (Rx) and PF2 (Clk) input

    // Configure SSI1 for SPI Master Mode
    SSI1_CR1_R = 0x00000000;      // Disable SSI1 to configure it as master
    SSI1_CC_R = 0x0;              // Use system clock as the source

    // Set the prescale divisor and serial clock rate (SCR) for 1 MHz SSI Clock
    SSI1_CPSR_R = 10;             // Set CPSR = 10 for prescale divisor (1 MHz with SCR = 7)
    SSI1_CR0_R = 0;               // Clear the CR0 register
    SSI1_CR0_R |= (7 << 8);       // SCR = 7 for 1 MHz SSI clock
    SSI1_CR0_R |= (0 << 7);       // SPH = 0 (data captured on the first clock edge)
    SSI1_CR0_R |= (0 << 6);       // SPO = 0 (clock is idle low)
    SSI1_CR0_R |= (0 << 4);       // FRF = 00 (Freescale SPI format)
    SSI1_CR0_R |= 0x07;           // DSS = 8 bits (0x07 = 7 for 8-bit data)

    // Enable SSI1
    SSI1_CR1_R |= 0x00000002;     // Enable SSI1 in master mode
}


void SSI2_Init(void) {
    // Enable SSI2 and Port B
    SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R2;   // Enable SSI2
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // Enable Port B
    while ((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R1) == 0) {} // Wait until Port B is ready

    // Configure Port B for SSI2 (PB4-PB7)
    GPIO_PORTB_AFSEL_R |= 0xF0;               // Enable alt funct on PB4-PB7
    GPIO_PORTB_DEN_R |= 0xF0;                 // Enable digital I/O on PB4-PB7
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0x0000FFFF) + 0x22220000; // Configure PB4-PB7 as SSI
    GPIO_PORTB_AMSEL_R &= ~0xF0;              // Disable analog functionality on PB4-PB7
    //GPIO_PORTB_DIR_R |= 0xB0;                 // Make PB4, PB5, PB7 output (PB6 as input)
    // GPIO_PORTB_PDR_R |= 0x80;              // Optional: Enable pull-down on PB7

    // Configure SSI2
    SSI2_CR1_R = 0x00000000;  // Disable SSI to configure
    SSI2_CC_R = 0;            // Use system clock
    SSI2_CPSR_R = 10;         // Set CPSR = 10 (prescale divisor)
    SSI2_CR0_R = 0;           // Clear the CR0 register
    SSI2_CR0_R |= (7 << 8);   // Set SCR = 7 for 1 MHz SSI clock
    SSI2_CR0_R |= (0 << 7);   // Set SPH = 0 (capture data on the first clock edge)
    SSI2_CR0_R |= (0 << 6);   // Set SPO = 0 (clock is idle low)
    SSI2_CR0_R |= (0 << 4);   // Set FRF = 00 (Freescale SPI format)
    SSI2_CR0_R |= 0x07;       // Set DSS = 8 bits (0x07 = 7 for 8-bit data)
    SSI2_CR1_R |= 0x00000002; // Enable SSI
}

void SSI3_Init(void) {
    // Enable SSI3 and Port D
    SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R3;   // Enable SSI3
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3; // Enable Port D
    while ((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R3) == 0) {} // Wait until Port D is ready

    // Configure Port D for SSI3 (PD0-PD3)
    GPIO_PORTD_AFSEL_R |= 0x0F;               // Enable alt funct on PD0-PD3
    GPIO_PORTD_DEN_R |= 0x0F;                 // Enable digital I/O on PD0-PD3
    GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0xFFFF0000) + 0x00001111; // Configure PD0-PD3 as SSI
    GPIO_PORTD_AMSEL_R &= ~0x0F;              // Disable analog functionality on PD0-PD3
    //GPIO_PORTD_DIR_R |= 0x0B;                 // Make PD0, PD1, PD3 output, PD2 input
    //GPIO_PORTD_PUR_R |= 0x08;                 // Optional: Enable pull-up on PD3

    // Configure SSI3
    SSI3_CR1_R = 0x00000000;  // Disable SSI to configure
    SSI3_CC_R = 0;            // Use system clock
    SSI3_CPSR_R = 10;         // Set CPSR = 10 (prescale divisor)
    SSI3_CR0_R = 0;           // Clear the CR0 register
    SSI3_CR0_R |= (7 << 8);   // Set SCR = 7 for 1 MHz SSI clock
    SSI3_CR0_R |= (0 << 7);   // Set SPH = 0 (capture data on the first clock edge)
    SSI3_CR0_R |= (0 << 6);   // Set SPO = 0 (clock is idle low)
    SSI3_CR0_R |= (0 << 4);   // Set FRF = 00 (Freescale SPI format)
    SSI3_CR0_R |= 0x07;       // Set DSS = 8 bits (0x07 = 7 for 8-bit data)
    SSI3_CR1_R |= 0x00000002; // Enable SSI
}

void PeriodicTask_Read24KeySignals(void) {
    GPIO_PORTE_DATA_R |= 0x01;
    ADC_Vals_Buf_idx = 0;
    uint16_t result0_low, result0_high, result1_low, result1_high, result3_low, result3_high;
    uint16_t result0, result1, result3;
    uint16_t command;
    uint16_t poop;
    uint16_t dummy_read;

    for (int channel = 0; channel < NUMBER_OF_CHANNELS; channel++) {
        
        command = 0x8F | (channel << 4);
        
        GPIO_PORTE_DATA_R &= ~0x04; // PE2 low
        GPIO_PORTE_DATA_R &= ~0x08; // PE3 low
        GPIO_PORTE_DATA_R &= ~0x10; // PE4 low
        
        // Clear receive FIFO
        while (SSI0_SR_R & SSI_SR_RNE) {
            poop = SSI0_DR_R;
        }

        while (SSI1_SR_R & SSI_SR_RNE) {
            poop = SSI1_DR_R;
        }

        while (SSI3_SR_R & SSI_SR_RNE) {
            poop = SSI3_DR_R;
        }  
        
        // Send start bits
        while (SSI0_SR_R & SSI_SR_BSY) {}; // Wait for SSI0 to be ready
        SSI0_DR_R = 0x01;  // Send start bit (0x01)

        while (SSI1_SR_R & SSI_SR_BSY) {};
        SSI1_DR_R = 0x01;  // Send start bit (0x01)

        while (SSI3_SR_R & SSI_SR_BSY) {};
        SSI3_DR_R = 0x01;  // Send start bit (0x01)
        
        // Send command
        while (SSI0_SR_R & SSI_SR_BSY) {};
        SSI0_DR_R = command;

        while (SSI1_SR_R & SSI_SR_BSY) {};
        SSI1_DR_R = command;

        while (SSI3_SR_R & SSI_SR_BSY) {};
        SSI3_DR_R = command;
        
        // Send dummy writes
        while (SSI0_SR_R & SSI_SR_BSY) {};  // Wait for SSI0 to be ready
        SSI0_DR_R = 0xAA;  // Dummy write to trigger the ready

        while (SSI1_SR_R & SSI_SR_BSY) {};  // Wait for SSI1 to be ready
        SSI1_DR_R = 0xAA;  // Dummy write to trigger the ready

        while (SSI3_SR_R & SSI_SR_BSY) {};  // Wait for SSI3 to be ready
        SSI3_DR_R = 0xAA;  // Dummy write to trigger the ready
        
        for (volatile uint16_t i = 0; i < CS_DELAY; i++) {}

        GPIO_PORTE_DATA_R |= 0x04; // PE2 on
        GPIO_PORTE_DATA_R |= 0x08; // PE3 on
        GPIO_PORTE_DATA_R |= 0x10; // PE4 on

        while (SSI0_SR_R & SSI_SR_BSY);
        dummy_read = SSI0_DR_R;  // Read and discard dummy data

        while (SSI1_SR_R & SSI_SR_BSY);
        dummy_read = SSI1_DR_R;  // Read and discard dummy data

        while (SSI3_SR_R & SSI_SR_BSY);
        dummy_read = SSI3_DR_R;  // Read and discard dummy data
        
        // Read MSBs from all three SSIs
        while (SSI0_SR_R & SSI_SR_BSY);
        result0_high = SSI0_DR_R & 0x03;  
        
        while (SSI1_SR_R & SSI_SR_BSY);
        result1_high = SSI1_DR_R & 0x03;

        while (SSI3_SR_R & SSI_SR_BSY);
        result3_high = SSI3_DR_R & 0x03;
        
        // Read the 8 LSBs from all three SSIs
        while (SSI0_SR_R & SSI_SR_BSY);
        result0_low = SSI0_DR_R & 0xFF;

        while (SSI1_SR_R & SSI_SR_BSY);
        result1_low = SSI1_DR_R & 0xFF;

        while (SSI3_SR_R & SSI_SR_BSY);
        result3_low = SSI3_DR_R & 0xFF;  

        // --- Combine High and Low Bits ---
        result0 = (result0_high << 8) | result0_low;
        result1 = (result1_high << 8) | result1_low;
        result3 = (result3_high << 8) | result3_low;

        // --- Store the result ---
        ADC_Vals_Buf[ADC_Vals_Buf_idx] = result0;
        ADC_Vals_Buf[ADC_Vals_Buf_idx + NUMBER_OF_CHANNELS] = result1;
        ADC_Vals_Buf[ADC_Vals_Buf_idx + (NUMBER_OF_CHANNELS * 2)] = result3;
        ADC_Vals_Buf_idx++;
        
        //for (volatile uint16_t i = 0; i < CS_DELAY*10; i++) {}

    }

    // Toggle GPIO for debugging (optional)
    GPIO_PORTE_DATA_R &= ~0x01;

    //samplesTaken = true;
}


void PeriodicTask_ReadSSI0(void) {
    ADC_Vals_Buf_idx = 0;
    uint16_t result0_low, result0_high;
    uint16_t result0;
    uint16_t command;
    uint16_t poop;
    uint16_t dummy_read;

    for (int channel = 0; channel < NUMBER_OF_CHANNELS; channel++) {
        command = 0x8F | (2 << 4);

        GPIO_PORTE_DATA_R &= ~0x04;  // PE2 low
        
        // Clear receive FIFO
        while (SSI0_SR_R & SSI_SR_RNE) {
            poop = SSI0_DR_R;
        }

        // Send start bit
        while (SSI0_SR_R & SSI_SR_BSY) {};  // Wait for SSI0 to be ready
        SSI0_DR_R = 0x01;

        // Send command
        while (SSI0_SR_R & SSI_SR_BSY) {};
        SSI0_DR_R = command;

        // Send dummy write
        while (SSI0_SR_R & SSI_SR_BSY) {};
        SSI0_DR_R = 0xAA;
        
        for(volatile uint16_t i = 0; i < CS_DELAY; i++){}

        GPIO_PORTE_DATA_R |= 0x04;  // PE2 on

        while (SSI0_SR_R & SSI_SR_BSY);
        dummy_read = SSI0_DR_R;  // Read and discard dummy data

        // Read MSB and LSB
        while (SSI0_SR_R & SSI_SR_BSY);
        result0_high = SSI0_DR_R & 0x03;
        
        while (SSI0_SR_R & SSI_SR_BSY);
        result0_low = SSI0_DR_R & 0xFF;

        // Combine high and low bits
        result0 = (result0_high << 8) | result0_low;

        // Store result in buffer
        ADC_Vals_Buf[ADC_Vals_Buf_idx] = result0;
        ADC_Vals_Buf_idx++;
    }
    //samplesTaken = true;
}

void PeriodicTask_ReadSSI1(void) {
    ADC_Vals_Buf_idx = 0;
    uint16_t result1_low, result1_high;
    uint16_t result1;
    uint16_t command;
    uint16_t poop;
    uint16_t dummy_read;

    for (int channel = 0; channel < NUMBER_OF_CHANNELS; channel++) {
        command = 0x8F | (2 << 4);

        GPIO_PORTE_DATA_R &= ~0x08;  // PE3 low (assuming PE3 is the CS for SSI1)
        
        // Clear receive FIFO
        while (SSI1_SR_R & SSI_SR_RNE) {
            poop = SSI1_DR_R;
        }

        // Send start bit
        while (SSI1_SR_R & SSI_SR_BSY) {};  // Wait for SSI1 to be ready
        SSI1_DR_R = 0x01;

        // Send command
        while (SSI1_SR_R & SSI_SR_BSY) {};
        SSI1_DR_R = command;

        // Send dummy write
        while (SSI1_SR_R & SSI_SR_BSY) {};
        SSI1_DR_R = 0xAA;
        
        for (volatile uint16_t i = 0; i < CS_DELAY; i++) {}

        GPIO_PORTE_DATA_R |= 0x08;  // PE3 on (CS released)

        while (SSI1_SR_R & SSI_SR_BSY);
        dummy_read = SSI1_DR_R;  // Read and discard dummy data

        // Read MSB and LSB
        while (SSI1_SR_R & SSI_SR_BSY);
        result1_high = SSI1_DR_R & 0x03;
        
        while (SSI1_SR_R & SSI_SR_BSY);
        result1_low = SSI1_DR_R & 0xFF;

        // Combine high and low bits
        result1 = (result1_high << 8) | result1_low;

        // Store result in buffer
        ADC_Vals_Buf[ADC_Vals_Buf_idx + NUMBER_OF_CHANNELS] = result1;
        ADC_Vals_Buf_idx++;
    }
    //samplesTaken = true;
}

void PeriodicTask_ReadSSI2(void) {
    ADC_Vals_Buf_idx = 0;
    uint16_t result2_low, result2_high;
    uint16_t result2;
    uint16_t command;
    uint16_t poop;
    uint16_t dummy_read;

    for (int channel = 0; channel < NUMBER_OF_CHANNELS; channel++) {
        command = 0x8F | (2 << 4);

        GPIO_PORTE_DATA_R &= ~0x08;  // PE3 low

        // Clear receive FIFO
        while (SSI2_SR_R & SSI_SR_RNE) {
            poop = SSI2_DR_R;
        }

        // Send start bit
        while (SSI2_SR_R & SSI_SR_BSY) {};
        SSI2_DR_R = 0x01;

        // Send command
        while (SSI2_SR_R & SSI_SR_BSY) {};
        SSI2_DR_R = command;

        // Send dummy write
        while (SSI2_SR_R & SSI_SR_BSY) {};
        SSI2_DR_R = 0xAA;
        
        for(volatile uint16_t i = 0; i < CS_DELAY; i++){}

        GPIO_PORTE_DATA_R |= 0x08;  // PE3 on

        while (SSI2_SR_R & SSI_SR_BSY);
        dummy_read = SSI2_DR_R;  // Read and discard dummy data

        // Read MSB and LSB
        while (SSI2_SR_R & SSI_SR_BSY);
        result2_high = SSI2_DR_R & 0x03;

        while (SSI2_SR_R & SSI_SR_BSY);
        result2_low = SSI2_DR_R & 0xFF;

        // Combine high and low bits
        result2 = (result2_high << 8) | result2_low;

        // Store result in buffer
        ADC_Vals_Buf[ADC_Vals_Buf_idx + NUMBER_OF_CHANNELS] = result2;
        ADC_Vals_Buf_idx++;
    }
}

void PeriodicTask_ReadSSI3(void) {
    ADC_Vals_Buf_idx = 0;
    uint16_t result3_low, result3_high;
    uint16_t result3;
    uint16_t command;
    uint16_t poop;
    uint16_t dummy_read;

    for (int channel = 0; channel < NUMBER_OF_CHANNELS; channel++) {
        command = 0x8F | (2 << 4);

        GPIO_PORTE_DATA_R &= ~0x10;  // PE4 low

        // Clear receive FIFO
        while (SSI3_SR_R & SSI_SR_RNE) {
            poop = SSI3_DR_R;
        }

        // Send start bit
        while (SSI3_SR_R & SSI_SR_BSY) {};
        SSI3_DR_R = 0x01;

        // Send command
        while (SSI3_SR_R & SSI_SR_BSY) {};
        SSI3_DR_R = command;

        // Send dummy write
        while (SSI3_SR_R & SSI_SR_BSY) {};
        SSI3_DR_R = 0xAA;
        
        for(volatile uint16_t i = 0; i < CS_DELAY; i++){}

        GPIO_PORTE_DATA_R |= 0x10;  // PE4 on

        while (SSI3_SR_R & SSI_SR_BSY);
        dummy_read = SSI3_DR_R;  // Read and discard dummy data

        // Read MSB and LSB
        while (SSI3_SR_R & SSI_SR_BSY);
        result3_high = SSI3_DR_R & 0x03;

        while (SSI3_SR_R & SSI_SR_BSY);
        result3_low = SSI3_DR_R & 0xFF;

        // Combine high and low bits
        result3 = (result3_high << 8) | result3_low;

        // Store result in buffer
        ADC_Vals_Buf[ADC_Vals_Buf_idx + (NUMBER_OF_CHANNELS * 2)] = result3;
        ADC_Vals_Buf_idx++;
    }
}

void PeriodicTask_Test(void){
    while (SSI0_SR_R & SSI_SR_BSY){};  // Wait for SSI0 transfer to complete
    SSI0_DR_R = 0xAA;  // Dummy write to read result from SSI0
}

void SensorsInit(void){
    //ADC_IC_Init();
    SSI0_Init();
    SSI1_Init();
    //SSI2_Init();
    SSI3_Init();
    ADC_Vals_Buf_idx = 0;
    Timer0A_Init(PeriodicTask_Read24KeySignals, ADC_SAMPLING_PERIOD, 1);
    //Timer0A_Init(PeriodicTask_ReadSSI0, ADC_SAMPLING_PERIOD, 1);
    //Timer3A_Init(PeriodicTask_ReadSSI1, ADC_SAMPLING_PERIOD, 1);
    //Timer1A_Init(PeriodicTask_ReadSSI2, ADC_SAMPLING_PERIOD, 1);
    //Timer2A_Init(PeriodicTask_ReadSSI3, ADC_SAMPLING_PERIOD, 1);
}
