
#include "stm32f10x.h"
#include "enable.h"


void enable_port_init(void)
{

	GPIO_InitTypeDef Gpio_InitStr;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	Gpio_InitStr.GPIO_Pin = RED_ENABLE_PIN | GERRN_ENABLE_PIN | BLUE_ENABLE_PIN | GERRN532_ENABLE_PIN;
	Gpio_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	Gpio_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ENABLE_PORT, &Gpio_InitStr);
	
	// Disable
	GPIO_ResetBits(ENABLE_PORT,RED_ENABLE_PIN | GERRN_ENABLE_PIN | BLUE_ENABLE_PIN | GERRN532_ENABLE_PIN);

}

