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
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
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
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
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
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	/* Send STOP Condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
	
	
}


