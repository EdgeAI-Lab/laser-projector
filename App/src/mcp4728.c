#include "stm32f10x_conf.h"
#include "mcp4728.h"




void mcp4728_init(void)
{

	I2C_InitTypeDef   I2C_InitStructure;
  
  mcp4728_low_evel_init();
  
  I2C_DeInit(I2C1);

  /*!< MCP4728_I2C Init */
  I2C_InitStructure.I2C_Mode = I2C_Mode_SMBusHost;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 100000; // 100k
  I2C_Init(I2C1, &I2C_InitStructure);

  /*!< MCP4728_I2C Init */
  I2C_Cmd(I2C1, ENABLE);

}

void mcp4728_low_evel_init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

  /*!< MCP4728_I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  /*!< Configure MCP4728_I2C pins: SCL */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure MCP4728_I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// LDAC & RDY/BSY
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	
}

/**
  * @brief  
  * @param  None
  * @retval 
  */
void mcp4728_ReadRegEEPROM(uint8_t voltage[8])
{   
	
	uint8_t mcp4728_BufferRX[24] = {0};
  /* Test on BUSY Flag */

  while (I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
  
  /* Configure DMA Peripheral */
  MCP4728_DMA_Config((uint8_t*)mcp4728_BufferRX, 24);  
  
  /* Enable DMA NACK automatic generation */
  I2C_DMALastTransferCmd(I2C1, ENABLE);
  
  /* Send START condition a second time */  
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on SB Flag */
  while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB)) ;
  
  /* Send LM75 address for read */
  I2C_Send7bitAddress(I2C1, DEVICE_CODE, I2C_Direction_Receiver);
  
  /* Test on ADDR Flag */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))   ;
  
  /* Enable I2C DMA request */
  I2C_DMACmd(I2C1,ENABLE);
  
  /* Enable DMA RX Channel */
  DMA_Cmd(DMA1_Channel7, ENABLE);
  
  /* Wait until DMA Transfer Complete */
  while (!DMA_GetFlagStatus(DMA1_FLAG_TC7));    
  
  /* Send STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
  
  /* Disable DMA RX Channel */
  DMA_Cmd(DMA1_Channel7, DISABLE);
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(I2C1,DISABLE);
  
  /* Clear DMA RX Transfer Complete Flag */
  DMA_ClearFlag(DMA1_FLAG_TC7);

	
	// red light
	voltage[0] = mcp4728_BufferRX[1];
	voltage[1] = mcp4728_BufferRX[2];
	
	// green light
	voltage[2] = mcp4728_BufferRX[7];
	voltage[3] = mcp4728_BufferRX[8];
	
	// blue light
	voltage[4] = mcp4728_BufferRX[13];
	voltage[5] = mcp4728_BufferRX[14];
	
	// green532 light
	voltage[6] = mcp4728_BufferRX[19];
	voltage[7] = mcp4728_BufferRX[20];
  
}


/**
  * @brief  Configure the DMA Peripheral used to handle communication via I2C.
  * @param  None
  * @retval None
  */

static void MCP4728_DMA_Config(uint8_t* buffer, uint8_t NumData)
{
  DMA_InitTypeDef DMA_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  /* Initialize the DMA_PeripheralBaseAddr member */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&I2C1->DR;
  /* Initialize the DMA_MemoryBaseAddr member */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
   /* Initialize the DMA_PeripheralInc member */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  /* Initialize the DMA_MemoryInc member */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  /* Initialize the DMA_PeripheralDataSize member */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /* Initialize the DMA_MemoryDataSize member */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  /* Initialize the DMA_Mode member */
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  /* Initialize the DMA_Priority member */
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  /* Initialize the DMA_M2M member */
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	/* Initialize the DMA_DIR member */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	/* Initialize the DMA_BufferSize member */
	DMA_InitStructure.DMA_BufferSize = NumData;
	
	DMA_DeInit(DMA1_Channel7);
	
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);

}



/*
 * test connect
 *
 */
void mcp4728_test(void)
{

	while (I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)) 
  {
  }
  
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on SB Flag */
  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)) 
  {
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2C1, DEVICE_CODE | 0x00, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
  }
	
  /* Send STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);

}

void fast_write(uint16_t *value)
{
	uint8_t i = 0;
	
	for(i=0;i<4;i++)
	{
		value[i] = (uint16_t) ((value[i] * 4096.0) / 5.0);
	}
	
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on SB Flag */
  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2C1, DEVICE_CODE | 0x00, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	for(i=0;i<4;i++)
	{
		// send high byte
		I2C_SendData(I2C1,value[i] / 256);
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		// send low byte
		I2C_SendData(I2C1,value[i] % 256);
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	
	
  /* Send STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
}

void single_write(uint8_t channel, float value)
{

	uint16_t temp = 0;
	
	/* Enable the I2C peripheral */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on SB Flag */
  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2C1, DEVICE_CODE | 0x00, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	// send single write cmd
	I2C_SendData(I2C1,SINGLEWRITE | channel);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	
	temp = (uint16_t) ((value * 4095.0) / 5.0);
	I2C_SendData(I2C1,(uint8_t)((temp>>8)&0x0F));
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_SendData(I2C1,(uint8_t)(temp&0xFF));
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){};
	
	/* Send STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
	
}


void mcp4728_write_vref(uint8_t vref)
{ 
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on SB Flag */
  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2C1, DEVICE_CODE | 0x00, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	// send high byte
	I2C_SendData(I2C1,WRITEVREF | (vref & 0x0F));
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){};
	
	/* Send STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
}

void mcp4728_write_gain(uint8_t gain)
{

  /* Enable the I2C peripheral */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on SB Flag */
  while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2C1, DEVICE_CODE | 0x00, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	// send high byte
	I2C_SendData(I2C1,WRITEGAIN | (gain & 0x0F));
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){};
	
	/* Send STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
	
	
}


