#ifndef __MCP3202_H__
#define __MCP3202_H__
#include "stm32f10x.h"

#define	START_BIT	0x01




#define MOSI_H GPIO_SetBits(GPIOB, GPIO_Pin_5)  
#define MOSI_L GPIO_ResetBits(GPIOB, GPIO_Pin_5)  
#define SCLK_H GPIO_SetBits(GPIOB, GPIO_Pin_3)  
#define SCLK_L GPIO_ResetBits(GPIOB, GPIO_Pin_3)  
#define MISO   GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)  

#define CS_H   GPIO_SetBits(GPIOB, GPIO_Pin_2)
#define CS_L   GPIO_ResetBits(GPIOB, GPIO_Pin_2)

void mcp3202_init(void);
void mcp3202_low_level_init(void);
uint16_t read_temperature(uint8_t channelNo);
uint8_t spi_send_byte(uint8_t byte);

#endif


