#include "mcp3202.h"
#include "stm32f10x.h"
#include "systick.h"
#include "delay.h"


void mcp3202_init(void)
{
//	SPI_InitTypeDef  SPI_InitStructure;

  mcp3202_low_level_init();
    
  /*!< Deselect the mcp3202: Chip Select high */
   //GPIO_SetBits(GPIOB, GPIO_Pin_2);
	CS_H;
	//CS_L;
	//MOSI_L;
	SCLK_L;
	
#if 0
  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; // SPI Mode 0: CPOL = 0; CPHA = 0;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI3, &SPI_InitStructure);

  /*!< Enable the mcp3202_SPI  */
  SPI_Cmd(SPI3, ENABLE);
	
#endif
}


void mcp3202_low_level_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  /*!< mcp3202_SPI_GPIO enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
	
	
  /*!< mcp3202_SPI Periph clock enable */
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
  
  /*!< Configure mcp3202_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure mcp3202_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure mcp3202_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE) ;
  
  /*!< Configure mcp3202_CS_PIN pin: mcp3202 CS pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void output_control(uint8_t data)
{
	uint8_t Mask = 0x08;                /* mask to test for 0/1 */
	uint8_t Count;                      /* gen purpose bit counter */
	
	for(Count = 0x00; Count < 0x04; Count++)  /* count 4 bits */
	{
		MOSI_L;                                 /* pre-clear data line */
		if(data & Mask)                         /* check if bit should be high or low */
		{
			MOSI_H;                               /* set data line */
		}
		
		SCLK_H;                                 /* send clock line high */
		
		Mask >>= 0x01;                          /* rotate mask for next bit */
		                                        /* also used to burn time for clock */
		SCLK_L;                                 /* send clock line low */
	}
}


uint16_t input_data(void)
{
	uint8_t Count;                       /* gen purpose bit counter */
	uint16_t Mask = 0x8000;                /* mask to insert ‘1’ at bit position */
	uint16_t Result = 0x0000;              /* A/D result register */

	for(Count = 0x00; Count < 0x0D; Count++)   /* count 13 bits */
	{                                          /* 12-bit result + 1 null bit */
		if(MISO)                                 /* check if bit is high or low */
		{
			Result |= Mask;                         /* bit high, set bit in result */
		}
		
		SCLK_H;                                   /* send clock line high */
		
		Mask >>= 0x01;                            /* rotate mask for next bit */
		                                          /* also used to burn time for clock */
		SCLK_L;                                   /* send clock line low */
	}
	
	Result >>= 0x03; /* rotate bits into position */
	Result &= 0x0FFF; /* mask out 12-bit result */
	return(Result); /* return result to caller */
}



uint8_t mcp3202_send_byte(uint8_t byte)
{

	
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI3 peripheral */
  SPI_I2S_SendData(SPI3, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
	
  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI3);
	
}



uint16_t read_temperature(uint8_t channelNo)
{
  
	uint8_t cmd = 0;
	
	uint16_t result = 0;
	
	uint8_t i=0;
  uint32_t sum = 0;
	
	switch(channelNo){
		case 0:
			cmd = 0x0D; // 0000 1111
		break;

		case 1:
			cmd = 0x0F; // 0000 1101
		break;
		
		default:
			cmd = 0x0D;
		break;
	}

	/************* ²ÉÑù50´Î£¬È¡Æ½¾ùÖµ *************/
	for(i=0;i<50;i++)
	{
	  SCLK_L;
		/*!< Select the mcp3202: Chip Select high */
		GPIO_ResetBits(GPIOB, GPIO_Pin_2);

#if 0
		mcp3202_send_byte(0x01);
		read_byte[1] = mcp3202_send_byte(second_byte) & 0x0F;  // receive High byte
		read_byte[0] = mcp3202_send_byte(0xFF);               // receive Low byte
#else	

		output_control((char)cmd);
	  result = input_data();
	
#endif
		
		/*!< Deselect the mcp3202: Chip Select high */
		GPIO_SetBits(GPIOB, GPIO_Pin_2);
		
		sum += result;
	}
	
	result = sum / 50;
	result = (uint16_t)( (result) * 330 / 4096 );
	
	if(result > 255)
	{
		result = 255;
	}

	return result;
}


unsigned int filter(unsigned int ddata[])	   
{
  unsigned long int al = 0;
  unsigned int i,temp;
  unsigned char x,y;
   for(y=0;y<8;y++)								
    {
	 for(x=1;x<9-y;x++)
	  {
	   if((*(ddata+x))>(*(ddata+x-1)))
	    {
				temp=(*(ddata+x-1));
				(*(ddata+x-1))=(*(ddata+x)); 
				(*(ddata+x))=temp;
			}
	  }
	}
  for(i=2;i<7;i++)								
   {
   	 al+=ddata[i];
   }
   al/=5;al=(unsigned int)al;
   return (al);
}

