#ifndef MAIN_H
#define MAIN_H

/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */

/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */
/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */
void DisableInterrupts(void);           // Disable interrupts
void EnableInterrupts(void);            // Enable interrupts
void WaitForInterrupt(void);
void startupDelay(void);

void Task_TestNetworking(void);
#endif 

