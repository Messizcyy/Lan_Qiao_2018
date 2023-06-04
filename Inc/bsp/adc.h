#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


extern ADC_HandleTypeDef hadc2;


void ADC2_Init(void);


uint16_t get_AO1(void);
uint16_t get_KEY(void);
uint16_t Read_Btn(void);
uint8_t Read_Key(void);


#ifdef __cplusplus
}
#endif

#endif 

