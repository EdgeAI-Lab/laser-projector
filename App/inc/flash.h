#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f10x.h"

#define RED_CHANNEL       ((uint8_t)0x01)
#define GREEN_CHANNEL     ((uint8_t)0x02)
#define BLUE_CHANNEL      ((uint8_t)0x03)
#define GREEN532_CHANNEL  ((uint8_t)0x04)

//#define TRUE              ((uint8_t)0x01)
//#define FLASE             ((uint8_t)0x00)


void save_enable_state(uint8_t channel, uint8_t state);
uint8_t get_enable_state(void);
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);


#endif

