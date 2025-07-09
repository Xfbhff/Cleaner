#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f10x.h"




#define KEY   	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)



void KEY_Init(void);//IO≥ı ºªØ
				    
#endif
