/**
 *
 * \file
 *
 * \brief This module contains NMC1000 bus wrapper APIs implementation.
 *
 * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

 #include "bsp/include/nm_bsp.h"
 #include "common/include/nm_common.h"
 #include "bus_wrapper/include/nm_bus_wrapper.h"
 #include "tm4c123gh6pm.h"
//  #include "asf.h"
 #include "conf_wilc.h"
#include <stdint.h>
 
 #define NM_BUS_MAX_TRX_SZ 4096
 
 tstrNmBusCapabilities egstrNmBusCapabilities =
 {
	 NM_BUS_MAX_TRX_SZ
 };
 
 #ifdef CONF_WILC_USE_I2C
 #define SLAVE_ADDRESS 0x60
 
 /** Number of times to try to send packet if failed. */
 #define I2C_TIMEOUT 100
 
 static sint8 nm_i2c_write(uint8 *b, uint16 sz)
 {
	 sint8 result = M2M_ERR_BUS_FAIL;
	 return result;
 }
 
 static sint8 nm_i2c_read(uint8 *rb, uint16 sz)
 {
	 sint8 result = M2M_ERR_BUS_FAIL;
	 return result;
 }
 
 static sint8 nm_i2c_write_special(uint8 *wb1, uint16 sz1, uint8 *wb2, uint16 sz2)
 {
	 static uint8 tmp[NM_BUS_MAX_TRX_SZ];
	 m2m_memcpy(tmp, wb1, sz1);
	 m2m_memcpy(&tmp[sz1], wb2, sz2);
	 return nm_i2c_write(tmp, sz1+sz2);
 }
 #endif
 
 #ifdef CONF_WILC_USE_SPI
 /** Fast CS macro. */
 #define SPI_ASSERT_CS()	GPIO_PORTE_DATA_R &= ~0x20 // Chip Select low
 #define SPI_DEASSERT_CS()	GPIO_PORTE_DATA_R |= 0x20 // Chip Select high
 
 static sint8 spi_rw(uint8 *pu8Mosi, uint8 *pu8Miso, uint16 u16Sz)
 {

    // Clear receive FIFO
    //while ((SSI0_SR_R & SSI_SR_RNE) && !(SSI0_SR_R & SSI_SR_BSY)) {
    //    uint16_t poop = SSI0_DR_R;
    //}

	uint16_t in = 0;
	SPI_ASSERT_CS(); // Assert chip select
    
	for(uint16_t i = 0; i < u16Sz; i++) {
		while (SSI0_SR_R & SSI_SR_BSY) {}; // Wait for SSI0 to be ready
		SSI0_DR_R = (pu8Mosi == NULL)?0:pu8Mosi[i];  // Send data
        while(!(SSI0_SR_R & SSI_SR_RNE)) {}
        uint32_t rx = SSI0_DR_R; // Always read to clear RX FIFO
        if (pu8Miso) {
            pu8Miso[i] = (uint8_t)rx;
        }
    }	
	for(int i=0; i<200; i++);

	SPI_DEASSERT_CS(); // De-assert chip select
	return M2M_SUCCESS;
 }
 #endif
 
 /*
 *	@fn		nm_bus_init
 *	@brief	Initialize the bus wrapper
 *	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
 */
 sint8 nm_bus_init(void *pvinit)
 {
	 sint8 result = M2M_SUCCESS;
 #ifdef CONF_WILC_USE_I2C
	 /* TODO: implement I2C. */
	 result = M2M_ERR;
 
 #elif defined CONF_WILC_USE_SPI
	// Enable SSI0 and Port A
	SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;   // Enable SSI0
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0; // Enable Port A
	while ((SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R0) == 0) {} // Wait until Port A is ready

	// Configure Port A for SSI0 (PA2-PA5) {PA2 = SSI0Clk, PA3 = SSI0Fss, PA4 = SSI0Rx, PA5 = SSI0Tx}
	GPIO_PORTA_AFSEL_R |= 0x34;               // Enable alt funct on PA2-PA5
	GPIO_PORTA_DEN_R |= 0x3C;                 // Enable digital I/O on PA2-PA5
	GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFF0000FF) + 0x00220200; // Configure PA2-PA5 as SSI
	GPIO_PORTA_AMSEL_R &= ~0x3C;              // Disable analog functionality on PA2-PA5
	GPIO_PORTA_DIR_R |= 0x8;                 // Make PA3 output
	// GPIO_PORTA_DIR_R &= ~0x10;                // Make PA4 input (MISO)

	// Configure SSI0
	SSI0_CR1_R = 0x00000000;  // Disable SSI to configure
	SSI0_CC_R = 0;            // Use system clock
	SSI0_CPSR_R = 2;         // Set CPSR = 10 (prescale divisor)
	SSI0_CR0_R = 0;           // Clear the CR0 register
	SSI0_CR0_R |= (4 << 8);   // Set SCR = 7 for 1 MHz SSI clock
	SSI0_CR0_R |= (0 << 7);   // Set SPH = 0 (capture data on the first clock edge)
	SSI0_CR0_R |= (0 << 6);   // Set SPO = 0 (clock is idle low)
	SSI0_CR0_R |= (0 << 4);   // Set FRF = 00 (Freescale SPI format)
	SSI0_CR0_R |= 0x07;       // Set DSS = 8 bits (0x07 = 7 for 8-bit data)
	SSI0_CR1_R |= 0x00000002; // Enable SSI

	SPI_DEASSERT_CS();
 #endif
	 return result;
 }
 
 /*
 *	@fn		nm_bus_ioctl
 *	@brief	send/receive from the bus
 *	@param[IN]	u8Cmd
 *					IOCTL command for the operation
 *	@param[IN]	pvParameter
 *					Arbitrary parameter depenging on IOCTL
 *	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
 *	@note	For SPI only, it's important to be able to send/receive at the same time
 */
 sint8 nm_bus_ioctl(uint8 u8Cmd, void* pvParameter)
 {
	 sint8 s8Ret = 0;
	 switch(u8Cmd)
	 {
 #ifdef CONF_WILC_USE_I2C
		 case NM_BUS_IOCTL_R: {
			 tstrNmI2cDefault *pstrParam = (tstrNmI2cDefault *)pvParameter;
			 s8Ret = nm_i2c_read(pstrParam->pu8Buf, pstrParam->u16Sz);
		 }
		 break;
		 case NM_BUS_IOCTL_W: {
			 tstrNmI2cDefault *pstrParam = (tstrNmI2cDefault *)pvParameter;
			 s8Ret = nm_i2c_write(pstrParam->pu8Buf, pstrParam->u16Sz);
		 }
		 break;
		 case NM_BUS_IOCTL_W_SPECIAL: {
			 tstrNmI2cSpecial *pstrParam = (tstrNmI2cSpecial *)pvParameter;
			 s8Ret = nm_i2c_write_special(pstrParam->pu8Buf1, pstrParam->u16Sz1, pstrParam->pu8Buf2, pstrParam->u16Sz2);
		 }
		 break;
 #elif defined CONF_WILC_USE_SPI
		 case NM_BUS_IOCTL_RW: {
			 tstrNmSpiRw *pstrParam = (tstrNmSpiRw *)pvParameter;
			 s8Ret = spi_rw(pstrParam->pu8InBuf, pstrParam->pu8OutBuf, pstrParam->u16Sz);
		 }
		 break;
 #endif
		 default:
			 s8Ret = -1;
			 M2M_ERR("invalide ioclt cmd\n");
			 break;
	 }
 
	 return s8Ret;
 }
 
 /*
 *	@fn		nm_bus_deinit
 *	@brief	De-initialize the bus wrapper
 */
 sint8 nm_bus_deinit(void)
 {
	 return M2M_SUCCESS;
 }
 
