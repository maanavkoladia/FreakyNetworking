#ifndef CONF_WILC_H_INCLUDED
#define CONF_WILC_H_INCLUDED

#include "tm4c123gh6pm.h"

#define CONF_WILC_USE_1000_REV_B
#define NM_EDGE_INTERRUPT

/*  */
#define WILC_DFL_CTRL_PORT (GPIO_PORTE_DATA_R)

//make pe0 the reset pin
#define WILC_RESET_PORT (WILC_DFL_CTRL_PORT)
#define WILC_RESET_PIN (1<<0)

// pe1 the chipen 
#define WILC_CHIPEN_PORT (WILC_DFL_CTRL_PORT)
#define WILC_CHIPEN_PIN (1<<1)

//pe2 the wake pin
#define WILC_WAKE_PORT (WILC_DFL_CTRL_PORT)
#define WILC_WAKE_PIN (1<<2)

// Set all pins directly by manipulating the register
#define WILC_RESET_HI     (WILC_DFL_CTRL_PORT |= WILC_RESET_PIN)
#define WILC_RESET_LO     (WILC_DFL_CTRL_PORT &= ~WILC_RESET_PIN)

#define WILC_CHIPEN_HI    (WILC_DFL_CTRL_PORT |= WILC_CHIPEN_PIN)
#define WILC_CHIPEN_LO    (WILC_DFL_CTRL_PORT &= ~WILC_CHIPEN_PIN)

#define WILC_WAKE_HI      (WILC_DFL_CTRL_PORT |= WILC_WAKE_PIN)
#define WILC_WAKE_LO      (WILC_DFL_CTRL_PORT &= ~WILC_WAKE_PIN)

#define CONF_WILC_USE_SPI				

/** SPI pin and instance settings. */
#define CONF_WILC_SPI					SPI
#define CONF_WILC_SPI_ID				ID_SPI
#define CONF_WILC_SPI_MISO_GPIO			SPI_MISO_GPIO
#define CONF_WILC_SPI_MISO_FLAGS		SPI_MISO_FLAGS
#define CONF_WILC_SPI_MOSI_GPIO			SPI_MOSI_GPIO
#define CONF_WILC_SPI_MOSI_FLAGS		SPI_MOSI_FLAGS
#define CONF_WILC_SPI_CLK_GPIO			SPI_SPCK_GPIO
#define CONF_WILC_SPI_CLK_FLAGS			SPI_SPCK_FLAGS
#define CONF_WILC_SPI_CS_GPIO			SPI_NPCS0_GPIO
#define CONF_WILC_SPI_CS_FLAGS			PIO_OUTPUT_1
#define CONF_WILC_SPI_NPCS				(0)

/** SPI delay before SPCK and between consecutive transfer. */
#define CONF_WILC_SPI_DLYBS				(0)
#define CONF_WILC_SPI_DLYBCT 			(0)

/** SPI interrupt pin. */
#define CONF_WILC_SPI_INT_PIN			IOPORT_CREATE_PIN(PIOA, 1)
#define CONF_WILC_SPI_INT_PIO			PIOA
#define CONF_WILC_SPI_INT_PIO_ID		ID_PIOA
#define CONF_WILC_SPI_INT_MASK			(1 << 1)
/** Highest INT priority that can call FreeRTOS FromISR() functions. */
#define CONF_WILC_SPI_INT_PRIORITY		(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY)

/** Clock polarity & phase. */
#define CONF_WILC_SPI_POL				(0)
#define CONF_WILC_SPI_PHA				(1)

/** SPI clock. */
#define CONF_WILC_SPI_CLOCK				(48000000)

#define CONF_WILC_IRQ_PRI          (2)
#define IRQ_PIN_BIT (0x10) // PE4
/*
   ---------------------------------
   ----------- OS options ----------
   ---------------------------------
*/

// #define CONF_DRIVER_LOCK				1
// #if CONF_DRIVER_LOCK
// // #  include <os/include/os_hook.h>
// #  define DRIVER_SIGNAL_ISR				os_hook_isr
// #else
// #  define DRIVER_SIGNAL_ISR()
// #endif

/*
   ---------------------------------
   --------- Debug options ---------
   ---------------------------------
*/

#define CONF_WILC_DEBUG					(0)
#define CONF_WILC_PRINTF				printf

#define NM_DEBUG				CONF_WILC_DEBUG
#define NM_BSP_PRINTF			CONF_WILC_PRINTF

#ifdef __cplusplus
}
#endif

#endif /* CONF_WILC_H_INCLUDED */
