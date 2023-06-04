#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


void Led_Init(void);

void led_disp(uint8_t led);

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

