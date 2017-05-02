#include "adc.h"
#include "stm32f10x_conf.h"


/************************ global variable **********************/
uint16_t adc_value[50][16] = {0};
uint16_t after_filter[16] = {0};

/*
 * adc init
 *
 */
void adc_init(void)
{
	adc_gpio_init();
	
	adc_configure();
	
	adc_dma_configure();
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE); 

}


/*
 * adc gpio init
 *
 */
static void adc_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //The ADC input clock is generated from the PCLK2 clock divided by a prescaler and it must not exceed 14 MHz
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}

/*
 * adc configure
 *
 */
static void adc_configure(void)
{
	
	ADC_InitTypeDef ADC_InitStructure;

  ADC_DeInit(ADC1);


	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 
	ADC_InitStructure.ADC_ScanConvMode =ENABLE; 
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 
	ADC_InitStructure.ADC_NbrOfChannel = 16; 
	ADC_Init(ADC1, &ADC_InitStructure);


	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 8, ADC_SampleTime_239Cycles5 );
	
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 9, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 10, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 11, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 12, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 13, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 14, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 15, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 16, ADC_SampleTime_239Cycles5 );

	//
	ADC_DMACmd(ADC1, ENABLE);


	ADC_Cmd(ADC1, ENABLE); 

	ADC_ResetCalibration(ADC1); 

	while(ADC_GetResetCalibrationStatus(ADC1)); 

	ADC_StartCalibration(ADC1); 

	while(ADC_GetCalibrationStatus(ADC1)); 
}


/*
 * adc dma configure
 *
 */
static void adc_dma_configure(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	/* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	
	DMA_DeInit(DMA1_Channel1); 
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; 
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&adc_value; 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 50*16; 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); 
}

/*
 * convert adc value to voltage
 *
 */
uint16_t get_voltage(u16 advalue)
{

	return (uint16_t)(advalue * 330 / 4096); 

}


/*
 * rms
 *
 */
void adc_value_filter(void)
{
	int sum = 0;
	uint8_t count, i;
	for(i=0;i<16;i++)
	{

		for( count=0;count<50;count++)
		{

			sum += adc_value[count][i];

		}

		after_filter[i]=sum/50;

		sum=0;
	}

}




