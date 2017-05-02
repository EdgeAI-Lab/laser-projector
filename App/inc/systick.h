#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "stm32f10x.h"


void init_tick(void);
void HAL_IncTick(void);
void HAL_Delay(__IO uint32_t Delay);


#endif
