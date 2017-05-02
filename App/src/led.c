#include "led.h"
#include "stm32f10x_conf.h"

/*
 * test led
 *
 */
void led_init(void)
{

	GPIO_InitTypeDef Gpio_InitStr;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	
	Gpio_InitStr.GPIO_Pin = RED_CHANNEL1_PIN | RED_CHANNEL2_PIN | RED_CHANNEL3_PIN | RED_CHANNEL4_PIN;
	Gpio_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	Gpio_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RED_CHANNEL1_4_PORT, &Gpio_InitStr);
	
	Gpio_InitStr.GPIO_Pin = RED_CHANNEL5_PIN | RED_CHANNEL6_PIN | GREEN_CHANNEL1_PIN | GREEN_CHANNEL2_PIN;
	GPIO_Init(GPIOC, &Gpio_InitStr);
	
	Gpio_InitStr.GPIO_Pin = GREEN_CHANNEL3_PIN | GREEN_CHANNEL4_PIN | GREEN_CHANNEL5_PIN | GREEN_CHANNEL6_PIN | GREEN_CHANNEL7_PIN | BLUE_CHANNEL1_PIN | BLUE_CHANNEL2_PIN | GREEN532_CHANNEL1_PIN;
	GPIO_Init(GPIOD, &Gpio_InitStr);
	
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	GPIO_ResetBits(GPIOC,GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
	GPIO_ResetBits(GPIOD,GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);

}


/*
 * init pin as input
 *
 */
void led_state_init(void)
{
	GPIO_InitTypeDef Gpio_InitStr;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	
	Gpio_InitStr.GPIO_Pin = RED_CHANNEL1_PIN | RED_CHANNEL2_PIN | RED_CHANNEL3_PIN | RED_CHANNEL4_PIN;
	//Gpio_InitStr.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	Gpio_InitStr.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &Gpio_InitStr);
	
	Gpio_InitStr.GPIO_Pin = RED_CHANNEL5_PIN | RED_CHANNEL6_PIN | GREEN_CHANNEL1_PIN | GREEN_CHANNEL2_PIN;
	GPIO_Init(GPIOC, &Gpio_InitStr);
	
	Gpio_InitStr.GPIO_Pin = GREEN_CHANNEL3_PIN | GREEN_CHANNEL4_PIN | GREEN_CHANNEL5_PIN | GREEN_CHANNEL6_PIN | GREEN_CHANNEL7_PIN | BLUE_CHANNEL1_PIN | BLUE_CHANNEL2_PIN | GREEN532_CHANNEL1_PIN;
	GPIO_Init(GPIOD, &Gpio_InitStr);
}

/*
 * get led state
 *
 */
uint16_t get_led_state(void)
{
	uint16_t state = 0;
	
	// 1111 1111 1111 1111
	// 
	if(READ_RED_CHANNEL1()){state |= 0x01<<15;}else{state &= ~(0x01<<15);}
	if(READ_RED_CHANNEL2()){state |= 0x01<<14;}else{state &= ~(0x01<<14);}
	if(READ_RED_CHANNEL3()){state |= 0x01<<13;}else{state &= ~(0x01<<13);}
	if(READ_RED_CHANNEL4()){state |= 0x01<<12;}else{state &= ~(0x01<<12);}
	if(READ_RED_CHANNEL5()){state |= 0x01<<11;}else{state &= ~(0x01<<11);}
	if(READ_RED_CHANNEL6()){state |= 0x01<<10;}else{state &= ~(0x01<<10);}
	
	if(READ_GREEN_CHANNEL1()){state |= 0x01<<9;}else{state &= ~(0x01<<9);}
	if(READ_GREEN_CHANNEL2()){state |= 0x01<<8;}else{state &= ~(0x01<<8);}
	if(READ_GREEN_CHANNEL3()){state |= 0x01<<7;}else{state &= ~(0x01<<7);}
	if(READ_GREEN_CHANNEL4()){state |= 0x01<<6;}else{state &= ~(0x01<<6);}
	if(READ_GREEN_CHANNEL5()){state |= 0x01<<5;}else{state &= ~(0x01<<5);}
	if(READ_GREEN_CHANNEL6()){state |= 0x01<<4;}else{state &= ~(0x01<<4);}
	if(READ_GREEN_CHANNEL7()){state |= 0x01<<3;}else{state &= ~(0x01<<3);}
	
	if(READ_BLUE_CHANNEL1()){state |= 0x01<<2;}else{state &= ~(0x01<<2);}
	if(READ_BLUE_CHANNEL2()){state |= 0x01<<1;}else{state &= ~(0x01<<1);}
	
	if(READ_GREEN532_CHANNEL1()){state |= 0x01<<0;}else{state &= ~(0x01<<0);}
	
	
	
	return state;
	
}


