#include "stm32f10x.h"
#include "flash.h"
#include "uart.h"


void save_enable_state(uint8_t channel, uint8_t state)
{
	
	volatile uint8_t move_bit = 0;
	uint16_t buf[4] = {0};
	uint8_t i = 0;
	
	uint32_t addr = 0x0807F800;
	
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
	
	switch(channel)
	{
		case RED_CHANNEL:
					
					move_bit = 0;
			
			break;
		
		case GREEN_CHANNEL:
			
					move_bit = 1;
		
			break;
		
		case BLUE_CHANNEL:
			
					move_bit = 2;
		
			break;
		
		case GREEN532_CHANNEL:
			
					move_bit = 3;
				
			break;
	}
	
	/* 1. read flash */
	STMFLASH_Read(addr,buf,4);
	buf[0] = 0x10;
	buf[1] = 0x01;
	
	/* check sum */
	if ( buf[3] == (uint16_t)get_check_sum( (uint8_t *)buf, 4 ) )
	{
		 /* 2. modify the content of flash */
			if(state == TRUE)
				buf[2] |= (0x08 >> move_bit);
			else
				buf[2] &= ~(0x08 >> move_bit);
			
			buf[3] = (uint16_t)get_check_sum( (uint8_t *)buf, 4 );
	}
	else
	{
			/* 2. modify the content of flash */
			if(state == TRUE)
				buf[2] = (0x08 >> move_bit);
			else
				buf[2] = 0x00;
			
			buf[3] = (uint16_t)get_check_sum( (uint8_t *)buf, 4 );
	}

	
	/* 3. write flash */
	     
  /* Unlock the Flash Bank Program Erase controller */
  FLASH_Unlock();

  /* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  /* Erase the FLASH pages */
  FLASHStatus = FLASH_ErasePage(addr);
  

  for(i=0; i<4 ;i++)
  {
    FLASHStatus = FLASH_ProgramHalfWord(addr, buf[i]);
		addr += 2;
  }

  FLASH_Lock();
	
}


uint8_t get_enable_state(void)
{
	uint16_t buf[4] = {0};
	uint8_t state = 0;
	
	STMFLASH_Read(0x0807F800,buf,4);
	
	if(buf[3] == get_check_sum((uint8_t*)buf,4))
		state = (uint8_t)buf[2];
	else
		state = 0x00;
	
	return state;
}


void STMFLASH_Read(uint32_t addr,uint16_t *data,uint16_t length)   	
{
	uint16_t i;
	for(i=0;i<length;i++)
	{
		data[i]=STMFLASH_ReadHalfWord(addr);
		addr+=2;
	}
}

uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(uint16_t*)faddr; 
}

