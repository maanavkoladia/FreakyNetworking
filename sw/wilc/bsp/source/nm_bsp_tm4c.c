#include "bsp/include/nm_bsp.h"
#include "tm4c123gh6pm.h"
#include "common/include/nm_common.h"
// #include "component/component_cmcc.h"
// #include "asf.h"
#include "conf_wilc.h"

#include "OS.h"

static tpfNmBspIsr gpfIsr;

void GPIOPortE_Handler(void){
    GPIO_PORTE_ICR_R |= IRQ_PIN_BIT;//awcknoleding interupt
    gpfIsr();
}

// static void chip_isr(uint32_t id, uint32_t mask)
// {
//     if ((id == CONF_WILC_SPI_INT_PIO_ID) && (mask == CONF_WILC_SPI_INT_MASK)) {
//         if (gpfIsr) {
//             gpfIsr();
//         }
//     }
// }

/*
*	@fn		init_chip_pins
*	@brief	Initialize reset, chip enable and wake pin
*/
static void init_chip_pins(void)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4) == 0);

    GPIO_PORTE_DIR_R |=  0x2F;       // Set PE0–3 and PE5 as outputs
    GPIO_PORTE_DIR_R &= ~0x10;       // Clear PE4 for input

    GPIO_PORTE_DEN_R |= 0x3F;

    GPIO_PORTE_AFSEL_R &= ~0x3F;
    GPIO_PORTE_AMSEL_R &= ~0x3F;
    GPIO_PORTE_PCTL_R &= ~0x00FFFFFF;  // Clear PCTL for PE0–PE5

    GPIO_PORTE_IS_R &= ~0x10;        // PE4 edge-sensitive
    GPIO_PORTE_IBE_R &= ~0x10;       // Not both edges
    GPIO_PORTE_IEV_R &= ~0x10;       // Falling edge
    GPIO_PORTE_ICR_R |=  0x10;       // Clear any prior interrupt flag
    //GPIO_PORTE_IM_R |=  0x10;        // Arm interrupt on PE4

    NVIC_PRI1_R = (NVIC_PRI1_R & ~0x000000E0) | (CONF_WILC_IRQ_PRI << 5);  // Priority 2

    NVIC_EN0_R |= (1 << 4);
    
    WILC_RESET_LO;
    WILC_CHIPEN_LO;
    WILC_WAKE_LO;

    // pio_configure_pin(CONF_WILC_PIN_RESET, PIO_TYPE_PIO_OUTPUT_0);
    // pio_configure_pin(CONF_WILC_PIN_CHIP_ENABLE, PIO_TYPE_PIO_OUTPUT_0);
    // pio_configure_pin(CONF_WILC_PIN_WAKE, PIO_TYPE_PIO_OUTPUT_0);
}

/*
*	@fn		nm_bsp_init
*	@brief	Initialize BSP
*	@return	0 in case of success and -1 in case of failure
*/
sint8 nm_bsp_init(void)
{
    gpfIsr = NULL;
    
    /* Initialize chip IOs. */
    init_chip_pins();

    /* Perform chip reset. */
    nm_bsp_reset();

    return 0;
}

/**
 *	@fn		nm_bsp_reset
*	@brief	Reset NMC1500 SoC by setting CHIP_EN and RESET_N signals low,
*           CHIP_EN high then RESET_N high
*/
void nm_bsp_reset(void)
{
    WILC_CHIPEN_LO;
    WILC_RESET_LO;
    nm_bsp_sleep(100);
    WILC_CHIPEN_HI;
    nm_bsp_sleep(100);
    WILC_RESET_HI;
    nm_bsp_sleep(100);
}

/*
*	@fn		nm_bsp_sleep
*	@brief	Sleep in units of mSec
*	@param[IN]	u32TimeMsec
*				Time in milliseconds
*/
void nm_bsp_sleep(uint32 u32TimeMsec)
{
    OS_Sleep(u32TimeMsec);
}

/*
*	@fn		nm_bsp_register_isr
*	@brief	Register interrupt service routine
*	@param[IN]	pfIsr
*				Pointer to ISR handler
*/
void nm_bsp_register_isr(tpfNmBspIsr pfIsr)
{
    gpfIsr = pfIsr;
    nm_bsp_interrupt_ctrl(1);

//     /* Configure PGIO pin for interrupt from SPI slave, used when slave has data to send. */
//     sysclk_enable_peripheral_clock(CONF_WILC_SPI_INT_PIO_ID);
//     pio_configure_pin(CONF_WILC_SPI_INT_PIN, PIO_TYPE_PIO_INPUT);
//     pio_pull_up(CONF_WILC_SPI_INT_PIO, CONF_WILC_SPI_INT_MASK, PIO_PULLUP);
// //	pio_set_debounce_filter(CONF_WILC_SPI_INT_PIO, CONF_WILC_SPI_INT_MASK, 10);
//     pio_handler_set_pin(CONF_WILC_SPI_INT_PIN, PIO_IT_LOW_LEVEL, chip_isr);
//     /* The status register of the PIO controller is cleared prior to enabling the interrupt */
//     pio_get_interrupt_status(CONF_WILC_SPI_INT_PIO);
//     pio_enable_interrupt(CONF_WILC_SPI_INT_PIO, CONF_WILC_SPI_INT_MASK);
//     pio_handler_set_priority(CONF_WILC_SPI_INT_PIO, (IRQn_Type)CONF_WILC_SPI_INT_PIO_ID,
//             CONF_WILC_SPI_INT_PRIORITY);
}

/*
*	@fn		nm_bsp_interrupt_ctrl
*	@brief	Enable/Disable interrupts
*	@param[IN]	u8Enable
*				'0' disable interrupts. '1' enable interrupts
*/
void nm_bsp_interrupt_ctrl(uint8 u8Enable)
{
    if(u8Enable){
        GPIO_PORTE_IM_R |= 0x10; // Enable interrupt on PE4
    } else {
        GPIO_PORTE_IM_R &= ~0x10; // Disable interrupt on PE4
    }
    
}

/*
*	@fn		nm_bsp_malloc
*	@brief	Allocate memory
*	@param [in]   u32Size
*               Size of the requested memory 
*	@return       Pointer to the allocated buffer, or NULL otherwise
*/
// void* nm_bsp_malloc(uint32 u32Size)
// {
//     while(1){
//         printf("malloc %d\n", u32Size);
//     };
//     // return malloc(u32Size);
//     return 0;
// }

// /*
// *	@fn		nm_bsp_free
// *	@brief	Free memory
// *	@param [in]   pvMemBuffer
// *               Pointer to the buffer to be freed 
// */
// void nm_bsp_free(void* pvMemBuffer)
// {
//     while(1){
//         printf("free %p\n", pvMemBuffer);
//     };
//     // free(pvMemBuffer);
//     return 0;
// }
