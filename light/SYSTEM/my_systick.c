#include "my_systick.h"

static u32 sysTicks=0;		//时间片

void My_SysTick_Init(void)
{
	SysTick_Config(SystemCoreClock / TICK_PER_SECOND);
}
u32 My_SysTick_GetTicks(void)
{
	return sysTicks;
}

#if	FLAG_SEND_SYSTICK>0
bool mySendFlag_tick = false;//发送数据标志
#endif
#if FLAG_READ_SYSTICK>0
bool myReadFlag_tick = false;//读取数据标志
#endif
///////////////////////////////////////////
#if	!defined (USE_HAL_DRIVER)
void SysTick_Handler(void)   //SysTick中断
#else
uint32_t HAL_GetTick(void)
{
  return sysTicks*uwTickFreq;
}
void HAL_IncTick(void)	//SysTick中断回调函数
#endif
{
	sysTicks++;
#if FLAG_READ_SYSTICK>0
	if(sysTicks%((int)(500/My_SysTick_GetPeriod()))==0)
	{
		myReadFlag_tick = true;
	}	
#endif		
#if	FLAG_SEND_SYSTICK>0
	if(sysTicks%((int)(1000/My_SysTick_GetPeriod()))==1)
	{
		mySendFlag_tick = true;
	}	
#endif
	
	if(sysTicks % 1000 == 0) disFlag =1;
	if(sysTicks % 3 == 0) My_StepMotor_Go();//步进电机驱动	
}

