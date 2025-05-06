#ifndef SENSORIN_H
#define SENSORIN_H

/* ================================================== */
/*                      INCLUDES                      */
/* ================================================== */
#include "stdint.h"
/* ================================================== */
/*            GLOBAL VARIABLE DECLARATIONS            */
/* ================================================== */
#define NUMBER_OF_SENSORS 24
#define NUMBER_OF_CHANNELS 8
#define ADC_SAMPLING_PERIOD 80000 //1 ms

extern uint16_t ADC_Vals_Buf[NUMBER_OF_SENSORS];
/* ================================================== */
/*                 FUNCTION PROTOTYPES                */
/* ================================================== */
void ADC_IC_Init(void);
void SSI0_Init(void);
void SSI1_Init(void);
void SSI2_Init(void);
void SSI3_Init(void);
void SensorsInit(void);
#endif 

