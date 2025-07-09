/********************(C) COPRIGHT 2019 Crownto electronic **************************
 * 文件名  ：main.c
 * 描述    ：        
 * 硬件连接：
 * LCD1602:RS -> PA11; RW -> PA12; E -> PA15;
 *         D0~D7 -> PB3~PB10
 * PH模块: VCC -> 5V; GND -> GND; PO -> PA2;
 *
 * 功能描述：测量PH值液晶显示(ADC1、PA2、DMA方式读取)；
             串口接收测量所得的PH值（波特率115200）；
 *           可连接上位机显示PH值；
 * 作者    ：冠拓电子
 * 淘宝    ：https://shop376667309.taobao.com
 * 网址    ：www.crownto.cn
**********************************************************************************/
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include <string.h>
#include "delay.h"
#include "driver_1602.h"
#include "math.h"
#include "bsp_adc.h"

#include "key.h"
#include "timer3.h"


GPIO_InitTypeDef  GPIO_InitStructure; 
unsigned char AD_CHANNEL=0;
unsigned long PH_num=0,PU_V=0;
float PH_Value=0;
u8 ph_temp=0,tu_temp=0;
u16 ph_result=0,tu_result=0;
u16 adc_1,adc_2;
u16 adc_v_1,adc_v_2;

u8 ISendByte(unsigned char sla,unsigned char c);
u8 IRcvByte(unsigned char sla);	
u8 SPIx_ReadWriteByte(u8 TxData);
float compensationCoefficient=1.0;//温度校准系数
float compensationVolatge;
float kValue=1.0;
unsigned char  Tx[20];   //无线发送缓存
float TDS=0.0,TDS_voltage;
float TDS_value=0.0,voltage_value;
float temp_data=0.0;

// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue;
extern __IO uint16_t ADC_ConvertedValue_;
// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal;  

/***************************************************************************
 * 描  述 : MAIN函数
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************/
 
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable the GPIO  Clock */					 		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC| RCC_APB2Periph_AFIO|RCC_APB2Periph_SPI1,ENABLE);
	
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);		//屏蔽所有作为JTAG口的GPIO口
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);		//屏蔽PB口上IO口JTAG功能
	
	
//1602 D0~D7 引脚	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|
	GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; /*I/O 方向 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; /*I/O 输出速度*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	
//1602 EN RS RW 引脚	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; /*I/O 方向 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; /*I/O 输出速度*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
   /* Configure  DRDY */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	

	
}
void TDS_Value_Conversion()
{
	TDS_voltage =(float) ADC_ConvertedValue_/4096*3.3; // 读取转换的AD值
	
	compensationCoefficient=1.0+0.02*((temp_data/10)-25.0); 
	compensationVolatge=TDS_voltage/compensationCoefficient;
	TDS_value=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 
	255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5*kValue;
	
	  if((TDS_value<=0)){TDS_value=0;}
		if((TDS_value>1400)){TDS_value=1400;}
		
		/*显示TDS*/
		Tx[0]=(int)(TDS_value)/1000+'0';
		Tx[1]=(int)(TDS_value)%1000/100+'0';	
		Tx[2]=(int)(TDS_value)%100/10+'0';
		Tx[3]=(int)(TDS_value)%10+'0';
		
		LCD_printchar(0x5,0,Tx[0]);
		LCD_printchar(0x6,0,Tx[1]);
		LCD_printchar(0x7,0,Tx[2]);
		LCD_printchar(0x8,0,Tx[3]);
}


void PH_Value_Conversion()
{
	
	  ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3; // 读取转换的AD值
	
		PH_Value=-5.7541*ADC_ConvertedValueLocal+16.654;
	
    
	if(PH_Value<=0.0){PH_Value=0.0;}
	if(PH_Value>=14.0){PH_Value=14.0;}
	
		  /*显示电压*/
	Tx[0]=(int)(PH_Value*100)/1000+'0';	
	Tx[1]=(int)(PH_Value*100)%1000/100+'0';
	Tx[2]='.';
	Tx[3]=(int)(PH_Value*100)%100/10+'0';
	Tx[4]=(int)(PH_Value*100)%10+'0';
}

void Display_Data()
{
		//显示PH值	
		LCD_printchar(3,0,Tx[0]);
		LCD_printchar(4,0,Tx[1]);
		LCD_printchar(5,0,Tx[2]);
		LCD_printchar(6,0,Tx[3]);
		LCD_printchar(7,0,Tx[4]);
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
		GPIO_Configuration();
    /* 配置USART1 */
    USART1_Config();
		// ADC 初始化
	  ADCx_Init();  
		ADCx_Init_();
    /* 初始化LCD1602 */
		LCD_init();

	
		/* 初始化按键 PA0 */
    KEY_Init(); 
		/* 初始化定时器3  周期20ms   pwm输出接口PB1*/
		PWM_Init();
	
	
		//测试 动态修改脉宽（0° → 90° → 180°）  crr为200  0.5ms~2.5ms，对应CCR值5~25  即0-180°
		SetPWM_Duty(5);
		
		
		LCD_printstring(0,0,"PH:");  

	
  while(1)
	{	
		PH_Value_Conversion();	
		Display_Data();
			
		printf("%s\n",Tx);
		TDS_Value_Conversion();	
		
		printf("TDS: %s\n",Tx);
		delay_ms(1000);
		if(KEY == 0)
		{
			delay_ms(20);
			if(KEY == 0)
			{
				SetPWM_Duty(5);
				delay_ms(1000);				
			}
		}
		if(KEY == 1)
		{
			delay_ms(20);
			if(KEY == 1)
			{
				SetPWM_Duty(25);
				delay_ms(1000);		
				
			}
		}
		
	}	
}
/*********************************************END OF FILE**********************/
