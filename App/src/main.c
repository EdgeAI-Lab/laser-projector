#include "stm32f10x_conf.h"

#include "led.h"
#include "adc.h"
#include "uart.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "data_handle.h"
#include "mcp4728.h"
#include "mcp3202.h"
#include "queue.h"
#include "enable.h"
#include "flash.h"
#include "mcp3202.h"
#include "delay.h"


void test_io(void);
void set_enable_state(void);

CycQueue *q = NULL;


int i = 0;
uint8_t value[16] = {0};
uint16_t dac_value[4] = {1,2,3,4};

int main(void)
{

		uint16_t ch_state = 0;
		
		
	
	q = CycQueueInit();
	
	if(q == NULL)
	{
		//printf("malloc error!\n");
	}
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	led_state_init();
	enable_port_init();
	//init_tick();
	delay_init(8);
	uart_init();
	adc_init();
	//test_io();
	mcp4728_init();
	//mcp4728_write_vref(0);// VREF = VDD
	//fast_write(dac_value);
	
	mcp3202_init();
	uint8_t adc[2] = {0};
	//volatile uint16_t adc0 = 0;
	//volatile uint16_t adc1 = 0;

	set_enable_state();
	
	while(1)
	{
		
		
		frame_data_prase();

		adc[0] = (uint8_t)read_temperature(0);
		//printf("adc0 : %d\n",adc);
		//printf("adc 0:  %d.%d\n",adc/100,adc%100) ;
		//printf("adc 0:  %d    ",adc0) ;
		
		//HAL_Delay(10);
		adc[1] = (uint8_t)read_temperature(1);
		//printf("adc1 : %d\n",adc);
		//printf("adc 1:  %d.%d\n",adc/100,adc%100) ;
		//printf("adc 1:  %d\n",adc1) ;
		
		// temperature upload
		data_upload(0x17, adc, 2);
		
		
		// get led state
		ch_state = get_led_state();
		// upload all channels state (stm32 is little end)
		data_upload(0x16, (uint8_t *)&ch_state, 2);
		
		// upload all channel adc value
		adc_value_filter();
		for(i=0;i<16;i++)
		{
			value[i] = (uint8_t)get_voltage(after_filter[i]);
			//printf("channel %d:   %d.%dv\n",i,value[i]/100,value[i]%100) ;	
		}
		// channel_1 ~ channel_16
		data_upload(0x15, value, 16);
		

	
		//HAL_Delay(100);
		delay_ms(100);

	}


}


/*
 * test led
 *
 */
void test_io(void)
{

	GPIO_InitTypeDef Gpio_InitStr;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	
	Gpio_InitStr.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	Gpio_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	Gpio_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &Gpio_InitStr);
	
	Gpio_InitStr.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &Gpio_InitStr);

	GPIO_ResetBits(GPIOA,GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5);
	GPIO_ResetBits(GPIOC,GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	
	//GPIO_SetBits(GPIOA,GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5);
	//GPIO_SetBits(GPIOC,GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);

}

void set_enable_state(void)
{
	uint8_t i;
	uint8_t state = get_enable_state();
	printf("read state: %X\n",state);
	for(i=0;i<4;i++)
	{
		switch( state & (0x08 >> i) )
		{
		
			case 0x08:
					
					RED_ENABLE();
			
				break;
			
			case 0x04:
				
					GREEN_ENABLE();
			
				break;
			
			case 0x02:
				
					BLUE_ENABLE();
			
				break;
			
			case 0x01:
				
					GREEN532_ENABLE();
			
				break;
			
			default:
				break;
			
		}
	}
	
}


