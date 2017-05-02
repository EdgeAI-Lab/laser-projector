#ifndef __MCP4728_H__
#define __MCP4728_H__
#include "stm32f10x.h"


#define DEVICE_CODE		0xC0

#define GENERAL_CALL 	0x00
#define RESET			    0x06
#define WAKEUP			  0x09
#define UPDATE			  0x08
#define READADDRESS		0x0C
#define READRESTART		0xC1

#define FASTWRITE		  0x00
#define MULTIWRITE		0x40
#define SEQWRITE		  0x50
#define SINGLEWRITE		0x58

#define WRITEADDRESS	0x60
#define WRITECURRENT	0x61
#define WRITENEW		  0x62
#define WRITECONFIRM	0x63

#define WRITEVREF		0x80
#define WRITEPD			0xA0
#define WRITEGAIN		0xC0

#define CHANNEL_A   0x00
#define CHANNEL_B   0x02
#define CHANNEL_C   0x04
#define CHANNEL_D   0x06

void mcp4728_init(void);
void mcp4728_low_evel_init(void);
void mcp4728_test(void);
void fast_write(uint16_t *value);
void mcp4728_write_vref(uint8_t vref);
void mcp4728_write_gain(uint8_t gain);
void single_write(uint8_t channel, float value);


#endif

