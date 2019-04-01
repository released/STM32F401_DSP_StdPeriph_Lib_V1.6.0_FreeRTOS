/* Includes ------------------------------------------------------------------*/
#include "Custom_ADC.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIMEOUT_MAX              		((uint32_t)10000) 

/* Private function prototypes -----------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//ADC variable
__IO uint16_t AD_Value[ADC_SAMPLE][ADC_CH];
__IO uint16_t After_filter[ADC_CH];

float32_t Input[FFT_SAMPLES]; 
float32_t Output[FFT_SAMPLES/2]; 


/* Private functions ---------------------------------------------------------*/

void Custom_ADC_Voltage(void)
{
	#if 0
	uint16_t CH1,CH2=0;
	CH1 = (After_filter[0] *ADC_VREF)/ADC_BITFACTOR;
	//CH2 = (After_filter[1] *ADC_VREF)/ADC_BITFACTOR;	

	printf("CH1 : %4d (0x%4X) , CH2 : %4d (0x%4X)\r\n",CH1,CH1,CH2,CH2);
	#endif

	uint16_t i=0;
	uint16_t CH=0;
	
	for (i=0;i<ADC_CH;i++)
	{
		CH = (After_filter[i] *ADC_VREF)/ADC_BITFACTOR;	

		#if 0		//debug only
		printf("CH%2d:%4dmv(0x%3X),",i,CH,CH);
		if (i==(ADC_CH-1))
		{
			printf("\r\n");
		}
		#endif
	}	
	

}

void Custom_ADC_Filter(void)
{
	uint32_t  	sum = 0;
	uint8_t 	count,i;

	for(i=0;i<ADC_CH;i++)
	{
		for ( count=0;count<ADC_SAMPLE;count++)
		{
		   sum += AD_Value[count][i];
		}
		
//		After_filter[i]= sum>>4;	//sum/ADC_SAMPLE;
		After_filter[i] = (sum + (ADC_SAMPLE >> 1)) >>4;	

		sum=0;
	} 
}

void Custom_ADC_Filter_FFT(void)
{
	uint16_t 	count;

	for ( count=0;count<(FFT_SIZE);count++)
	{
//		Input[(uint16_t)count] = (float32_t)((float32_t)AD_Value - (float32_t)2048.0) / (float32_t)2048.0;
		Input[count*2] = (float32_t)AD_Value[count][0];
		/* Imaginary part */
		Input[(count*2+1)] = 0;
	}
}

//PB0 : ADC1_IN8
//PC1 : ADC1_IN11
//PC0 : ADC1_IN10

void Custom_ADC_DMA_Config(void)	
{
	ADC_InitTypeDef       	ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       	DMA_InitStructure;
	GPIO_InitTypeDef      	GPIO_InitStructure;
	NVIC_InitTypeDef 		NVIC_InitStructure;
	__IO uint32_t    timeout = TIMEOUT_MAX;
  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 , ENABLE);
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC , ENABLE);
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB , ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//0x4001224C , ADC1_DR_ADDRESS
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&AD_Value;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = (ADC_SAMPLE)*(ADC_CH);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);

	#if 1	// Enable the DMA Interrupt 
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE); 
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);          
	#endif

	DMA_Cmd(DMA2_Stream0, ENABLE);	

	timeout = TIMEOUT_MAX;
	while ((DMA_GetCmdStatus(DMA2_Stream0) != ENABLE) && (timeout-- > 0))
	{
			printf("DMA_GetCmdStatus ERROR\r\n");	
	}

//	/* Check if a timeout condition occurred */
//	if (timeout == 0)
//	{
//		/* Manage the error: to simplify the code enter an infinite loop */
//		while (1)
//		{
//			printf("DMA_GetCmdStatus ERROR\r\n");
//		}
//	}	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_InitStructure.ADC_Resolution = ADC_RESOLUTION;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = ADC_CH;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 3, ADC_SampleTime_56Cycles);
	
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);

//	/* Wait the ADRDY flag */
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_STRT)); 
	
	/* ADC1 regular Software Start Conv */ 
	ADC_SoftwareStartConv(ADC1);
	
}


