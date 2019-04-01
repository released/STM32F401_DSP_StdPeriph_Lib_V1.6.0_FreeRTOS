/* Includes ------------------------------------------------------------------*/
#include "Hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

#if !defined (SWV_ENABLE)
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
  #define GETCHAR_PROTOTYPE int __io_getchar(void)  
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  #define GETCHAR_PROTOTYPE int fgetc(FILE *f)  
#endif /* __GNUC__ */

// TODO: for printf function , need to confirm use USART2 or USART2
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);

  /* Loop until transmit data register is empty */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}

  return ch;
}
#endif /*SWV_ENABLE*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t uwTimingDelay;

/*IWDG variable*/
static __IO unsigned long int uwLsiFreq = 0;
__IO unsigned long int uwCaptureNumber = 0;
__IO unsigned long int uwPeriodValue = 0;	

/*uart variable*/
__IO uint8_t UartRxBuffer = 0;
__IO uint8_t UartRxFlag = FALSE;

/*FreeRTOS variable*/
TaskHandle_t			xTask3Sub;
SemaphoreHandle_t 	xSemaphore_UART;

/*golobal flag*/
unsigned char 		Flag_Func = 0;		//for KeilC LA monitor

/*Counter variable*/
unsigned int 		Cnt_Packet = 0;
unsigned int 		Cnt_LogMsg = 0;

/*button variable*/
unsigned int 		IsBTN4Press = 0;	
unsigned int 		IsBTN4PressLong = 0;

/*PWM input capture*/
unsigned int 		uhICReadValue1 = 0;
unsigned int 		uhICReadValue2 = 0;
unsigned int 		uhCaptureNumber = 0;
unsigned long int 	uwCapture = 0;
unsigned long int 	uwTIMFreq = 0;

/*PWM output variable*/
unsigned int 		Channel1Period = 0;

/*ADC variable*/
extern __IO uint16_t After_filter[ADC_CH];	//manual extern the array
extern __IO uint16_t AD_Value[ADC_SAMPLE][ADC_CH];

/*PWM output variable*/
TIM_OCInitTypeDef  	TIM1_OC1InitStructure;
TIM_OCInitTypeDef  	TIM4_OC1InitStructure;

/*ADC FFT*/
extern float32_t Input[FFT_SAMPLES]; 
extern float32_t Output[FFT_SAMPLES/2]; 

/*Bit flag variable*/
uint8_t BitFlag = 0;
#define BitFlag_ON(flag)						(BitFlag|=flag)
#define BitFlag_OFF(flag)						(BitFlag&=~flag)
#define BitFlag_READ(flag)					((BitFlag&flag)?1:0)

#define ReadBit(bit)							(uint8_t)(1<<bit)

/*analog PWM */
const uint32_t RGBColorTable[300]=
{
	0x000000,0x272727,0x3C3C3C,0x4F4F4F,0x5B5B5B,0x6C6C6C,0x7B7B7B,0x8E8E8E,0x9D9D9D,0xADADAD,0xBEBEBE,0xd0d0d0,0xE0E0E0,0xF0F0F0,0xFCFCFC,0xFFFFFF,
	0x2F0000,0x4D0000,0x600000,0x750000,0x930000,0xAE0000,0xCE0000,0xEA0000,0xFF0000,0xFF2D2D,0xFF5151,0xff7575,0xFF9797,0xFFB5B5,0xFFD2D2,0xFFECEC,
	0x600030,0x820041,0x9F0050,0xBF0060,0xD9006C,0xF00078,0xFF0080,0xFF359A,0xFF60AF,0xFF79BC,0xFF95CA,0xffaad5,0xFFC1E0,0xFFD9EC,0xFFECF5,0xFFF7FB,
	0x460046,0x5E005E,0x750075,0x930093,0xAE00AE,0xD200D2,0xE800E8,0xFF00FF,0xFF44FF,0xFF77FF,0xFF8EFF,0xffa6ff,0xFFBFFF,0xFFD0FF,0xFFE6FF,0xFFF7FF,
	0x28004D,0x3A006F,0x4B0091,0x5B00AE,0x6F00D2,0x8600FF,0x921AFF,0x9F35FF,0xB15BFF,0xBE77FF,0xCA8EFF,0xd3a4ff,0xDCB5FF,0xE6CAFF,0xF1E1FF,0xFAF4FF,
	0x000079,0x000093,0x0000C6,0x0000C6,0x0000E3,0x2828FF,0x4A4AFF,0x6A6AFF,0x7D7DFF,0x9393FF,0xAAAAFF,0xB9B9FF,0xCECEFF,0xDDDDFF,0xECECFF,0xFBFBFF,
	0x000079,0x003D79,0x004B97,0x005AB5,0x0066CC,0x0072E3,0x0080FF,0x2894FF,0x46A3FF,0x66B3FF,0x84C1FF,0x97CBFF,0xACD6FF,0xC4E1FF,0xD2E9FF,0xECF5FF,
	0x003E3E,0x005757,0x007979,0x009393,0x00AEAE,0x00CACA,0x00E3E3,0x00FFFF,0x4DFFFF,0x80FFFF,0xA6FFFF,0xBBFFFF,0xCAFFFF,0xD9FFFF,0xECFFFF,0xFDFFFF,
	0x006030,0x01814A,0x019858,0x01B468,0x02C874,0x02DF82,0x02F78E,0x1AFD9C,0x4EFEB3,0x7AFEC6,0x96FED1,0xADFEDC,0xC1FFE4,0xD7FFEE,0xE8FFF5,0xFBFFFD,
	0x006000,0x007500,0x009100,0x00A600,0x00BB00,0x00DB00,0x00EC00,0x28FF28,0x53FF53,0x79FF79,0x93FF93,0xA6FFA6,0xBBFFBB,0xCEFFCE,0xDFFFDF,0xF0FFF0,
	0x467500,0x548C00,0x64A600,0x73BF00,0x82D900,0x8CEA00,0x9AFF02,0xA8FF24,0xB7FF4A,0xC2FF68,0xCCFF80,0xD3FF93,0xDEFFAC,0xE8FFC4,0xEFFFD7,0xF5FFE8,
	0x424200,0x5B5B00,0x737300,0x8C8C00,0xA6A600,0xC4C400,0xE1E100,0xF9F900,0xFFFF37,0xFFFF6F,0xFFFF93,0xFFFFAA,0xFFFFB9,0xFFFFCE,0xFFFFDF,0xFFFFF4,
	0x5B4B00,0x796400,0x977C00,0xAE8F00,0xC6A300,0xD9B300,0xEAC100,0xFFD306,0xFFDC35,0xFFE153,0xFFE66F,0xFFED97,0xFFF0AC,0xFFF4C1,0xFFF8D7,0xFFFCEC,
	0x844200,0x9F5000,0xBB5E00,0xD26900,0xEA7500,0xFF8000,0xFF9224,0xFFA042,0xFFAF60,0xFFBB77,0xFFC78E,0xFFD1A4,0xFFDCB9,0xFFE4CA,0xFFEEDD,0xFFFAF4,
	0x642100,0x842B00,0xA23400,0xBB3D00,0xD94600,0xF75000,0xFF5809,0xFF8040,0xFF8F59,0xFF9D6F,0xFFAD86,0xFFBD9D,0xFFCBB3,0xFFDAC8,0xFFE6D9,0xFFF3EE,
	0x613030,0x743A3A,0x804040,0x984B4B,0xAD5A5A,0xB87070,0xC48888,0xCF9E9E,0xD9B3B3,0xE1C4C4,0xEBD6D6,0xF2E6E6,							
	0x616130,0x707038,0x808040,0x949449,0xA5A552,0xAFAF61,0xB9B973,0xC2C287,0xCDCD9A,0xD6D6AD,0xDEDEBE,0xE8E8D0,							
	0x336666,0x3D7878,0x408080,0x4F9D9D,0x5CADAD,0x6FB7B7,0x81C0C0,0x95CACA,0xA3D1D1,0xB3D9D9,0xC4E1E1,0xD1E9E9,							
	0x484891,0x5151A2,0x5A5AAD,0x7373B9,0x8080C0,0x9999CC,0xA6A6D2,0xB8B8DC,0xC7C7E2,0xD8D8EB,0xE6E6F2,0xF3F3FA,							
	0x6C3365,0x7E3D76,0x8F4586,0x9F4D95,0xAE57A4,0xB766AD,0xC07AB8,0xCA8EC2,0xD2A2CC,0xDAB1D5,0xE2C2DE,0xEBD3E8,						
};

#define	MixedLIghtLED1_R_ON(void)	(GPIO_SetBits(GPIOB,GPIO_Pin_5))
#define	MixedLIghtLED1_G_ON(void)	(GPIO_SetBits(GPIOB,GPIO_Pin_4))
#define	MixedLIghtLED1_B_ON(void)	(GPIO_SetBits(GPIOB,GPIO_Pin_0))
#define	MixedLIghtLED1_R_OFF(void)	(GPIO_ResetBits(GPIOB,GPIO_Pin_5))
#define	MixedLIghtLED1_G_OFF(void)	(GPIO_ResetBits(GPIOB,GPIO_Pin_4))
#define	MixedLIghtLED1_B_OFF(void)	(GPIO_ResetBits(GPIOB,GPIO_Pin_0))

#if 0
BITS_8 ServiceFlag;

#define AAA  bit0
#define BBB  bit1
#define CCC  bit2

void FunctionA(void)
{
	if (ServiceFlag.fbit.AAA)
	{
	    ServiceFlag.fbit.BBB = 1;
	    ServiceFlag.fbit.CCC = 0;
	}
}
#endif

/* Private functions ---------------------------------------------------------*/
static void delay(__IO uint32_t nCount);

CommPacket gPacket =
{
	Type_NULL,
	{
		{1,1},
		{2,2},
		{3,3,3,3}
	}
};

//Send( (char *)&sendCommPacket , sizeof(CommuPacket));
//Recv( (char *)&recvCommPacket , sizeof(CommuPacket));

void vTask15AnalogPWM( void * pvParameters )
{
	uint32_t millisec = DELAY_MS_1;	
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.	
		MixedLIghtLED1_Demo();
		
		#if defined (DEBUG_15_ANALOG_PWM)
		printf("15:Analog PWM !\r\n");			
		#endif	/*DEBUG_15_ANALOG_PWM*/			
	}
  
}

void vTask14BitFlag( void * pvParameters )
{
	static uint8_t OnOff = 0;
	uint32_t millisec = DELAY_MS_1000;	
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.	
		OnOff = (OnOff&1)?(0):(1);
		BitFlag_SwitchTest(OnOff);		
		
		#if defined (DEBUG_14_BIT_FLAG)
		printf("\r\n\r\n14:BitFlag_Test (0x%2X)!\r\n",BitFlag);
		printf("0x%2X\r\n",BitFlag_READ(ReadBit(BitFlag_ADC)));
		printf("0x%2X\r\n",BitFlag_READ(ReadBit(BitFlag_GPIO)));	
		printf("0x%2X\r\n",BitFlag_READ(ReadBit(BitFlag_UART)));	
		printf("0x%2X\r\n",BitFlag_READ(ReadBit(BitFlag_AAA)));		
		printf("0x%2X\r\n",BitFlag_READ(ReadBit(BitFlag_BBB)));
		printf("0x%2X\r\n",BitFlag_READ(ReadBit(BitFlag_CCC)));	
		printf("0x%2X\r\n",BitFlag_READ(ReadBit(BitFlag_DDD)));	
		printf("0x%2X\r\n\r\n\r\n",BitFlag_READ(ReadBit(BitFlag_EEE)));	
		#endif	/*DEBUG_13_PWM_MODULATION*/		
	
	}
  
}

void vTask13PWMModulation( void * pvParameters )
{
	uint32_t millisec = DELAY_MS_1;	
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.	
		PWM_Modulation_Start();
		
		#if defined (DEBUG_13_PWM_MODULATION)
		printf("13:Modulation !\r\n");			
		#endif	/*DEBUG_13_PWM_MODULATION*/		
	
	}
  
}


void vTask12ButtonBebounce( void * pvParameters )
{
	uint32_t millisec = DELAY_MS_1;	
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.	
		#if 1
		Custom_ButtonIRQ();
		#else
		Custom_ButtonDebounce();	//button delay use 1 ms to count
		Button_Procedure();

		if (IsBTN4Press)
		{
		
			#if defined (DEBUG_12_BUTTON_DEBOUNCE)
			printf("12:Button pressed !\r\n");			
			#endif	/*DEBUG_12_BUTTON_DEBOUNCE*/		
			
			IsBTN4Press = 0 ;
		}

		if (IsBTN4PressLong)
		{
		
			#if defined (DEBUG_12_BUTTON_DEBOUNCE)
			printf("12:Button LONGGGGGG pressed !\r\n");			
			#endif	/*DEBUG_12_BUTTON_DEBOUNCE*/		
			
			IsBTN4PressLong = 0 ;
		}		
		#endif
		
	}
  
}

void vTask11ADCRAND( void * pvParameters )
{
	uint16_t seed = 0;
	uint16_t num = 0;
	
	uint32_t millisec = DELAY_MS_100*5;	
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.	
		seed = ADC_Random_GetSeed();
		srand(seed);
		num = rand();
		
		#if defined (DEBUG_11_ADC_RANDOM)
		printf("11:Random (0x%4X,0x%4X)\r\n",seed,num);		
		#else
		( void ) num;		//Remove compiler warning about Id being set but never used.
		#endif	/*DEBUG_11_ADC_RANDOM*/		
	}
  
}


void vTask10ADCFFT( void * pvParameters )
{
//  	uint16_t i;	
	float32_t maxValue;				/* Max FFT value is stored here */
	uint32_t maxIndex;				/* Index in Output array where max value is */
	float32_t meanValue;				/* Max FFT value is stored here */
	
	uint32_t millisec = DELAY_MS_100*5;	
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.	

		/* Process the data through the CFFT/CIFFT module */
		if (FFT_SIZE == 16)
		{
			arm_cfft_f32(&arm_cfft_sR_f32_len16, Input, IFFT_FLAG, DOBITREVERSE);
		}
		else if (FFT_SIZE == 32)
		{
			arm_cfft_f32(&arm_cfft_sR_f32_len32, Input, IFFT_FLAG, DOBITREVERSE);
		}
		else if (FFT_SIZE == 64)
		{
			arm_cfft_f32(&arm_cfft_sR_f32_len64, Input, IFFT_FLAG, DOBITREVERSE);
		}
		else if (FFT_SIZE == 128)
		{
			arm_cfft_f32(&arm_cfft_sR_f32_len128, Input, IFFT_FLAG, DOBITREVERSE);		
		}
		else if (FFT_SIZE == 256)
		{
			arm_cfft_f32(&arm_cfft_sR_f32_len256, Input, IFFT_FLAG, DOBITREVERSE);	
		}
		else if (FFT_SIZE == 512)
		{
			arm_cfft_f32(&arm_cfft_sR_f32_len512, Input, IFFT_FLAG, DOBITREVERSE);
		}
		else if (FFT_SIZE == 1024)
		{
			arm_cfft_f32(&arm_cfft_sR_f32_len1024, Input, IFFT_FLAG, DOBITREVERSE);	
		}
		else if (FFT_SIZE == 2048)
		{
			arm_cfft_f32(&arm_cfft_sR_f32_len2048, Input, IFFT_FLAG, DOBITREVERSE);	
		}
		else if (FFT_SIZE == 4096)
		{
			arm_cfft_f32(&arm_cfft_sR_f32_len4096, Input, IFFT_FLAG, DOBITREVERSE);			
		}
		
		/* Process the data through the Complex Magnitude Module for  
		calculating the magnitude at each bin */ 
		arm_cmplx_mag_f32(Input, Output, FFT_SIZE);  

		arm_max_f32(Output, FFT_SIZE, &maxValue, &maxIndex);		
		arm_mean_f32(Output, FFT_SIZE, &meanValue);		
		
		#if defined (DEBUG_10_ADC_FFT)
		printf("10:ADC FFT (%6d,%4d,%6d,%4d)\r\n",(uint32_t)maxValue,maxIndex,(uint32_t)meanValue,After_filter[0]);			
		#endif	/*DEBUG_10_ADC_FFT*/		
		
	}
  
}


void vTask9TIMMeasure( void * pvParameters )
{
	uint32_t freq = 0;	
	
//	uint32_t millisec = DELAY_MS_10;	
//	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
//	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
//		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.	
		freq = uwTIMFreq;
		
		#if defined (DEBUG_9_TIM_MEASURE)
		printf("9:TIM (%6d)\r\n",freq);
		#else
		( void ) freq;		//Remove compiler warning about freq being set but never used.
		#endif	/*DEBUG_9_TIM_MEASURE*/				
	}
  
}

void vTask8PWMAdjust( void * pvParameters )
{
	static uint32_t duty = 50;

	uint32_t millisec = DELAY_MS_1;	
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.	
		#if 1
		duty = (After_filter[0]*100)>>12;
		#else
		if (++duty >=100)
		{
			duty = 0;
		}
		#endif
		TIM1_OC1InitStructure.TIM_Pulse = (uint16_t) (((uint32_t) duty * (Channel1Period - 1)) / 100);
		TIM_OC1Init(TIM1, &TIM1_OC1InitStructure);

		
		#if defined (DEBUG_8_PWM_ADJUST)
		printf("8:PWM (%4d,%4d)\r\n",duty,After_filter[0]);			
		#endif	/*DEBUG_8_PWM_ADJUST*/		
		
	}
  
}

void vTask7ADCAverage( void * pvParameters )
{
	uint32_t millisec = DELAY_MS_100;	
	portTickType xLastWakeTime;
	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.	
		Custom_ADC_Voltage();
		
		#if defined (DEBUG_7_ADC_AVERAGE)
		printf("7:ADC (%4d,%4d,%4d)\r\n",After_filter[0],After_filter[1],After_filter[2]);			
		#endif	/*DEBUG_7_ADC_AVERAGE*/		
		
	}
  
}

void vTask6Tasklist( void * pvParameters )
{
	char buff[512];
	
	uint32_t millisec = DELAY_MS_1000;
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));
		
		// Perform action here.
		vTaskList(buff);

		#if defined (DEBUG_6_DISPLAY_TASKLIST)
		printf("\r\n6:Tasklist 	State Priority Stack   Num\r\n");
		printf("-------------------------------------------\r\n");
		printf("%s\r\n\r\n\r\n\r\n" ,buff);

		vTaskGetRunTimeStats(buff);
		printf("\r\n6:Tasklist 	RUN times   Usage\r\n");
		printf("-------------------------------------------\r\n");
		printf("%s\r\n\r\n\r\n\r\n" ,buff);	
		
		#endif	/*DEBUG_6_DISPLAY_TASKLIST*/

	}
  
}

void vTask5RecvPacket( void * pvParameters )
{
	CommPacket lPacket;
	
	uint32_t millisec = DELAY_MS_100*5;	
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;	
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));
		
		// Perform action here
		RecvPacket(&lPacket,sizeof(lPacket));		

		#if defined (DEBUG_5_RECEIVE_PACKET)
		printf("5:>>>> \r\n");		
		printf("5:(A:%4d)\r\n",lPacket.structutre.Struct_A.var_a);
		printf("5:(A:%4d)\r\n",lPacket.structutre.Struct_A.var_b);

		printf("5:(B:%4d)\r\n",lPacket.structutre.Struct_B.var_a);
		printf("5:(B:%4d)\r\n",lPacket.structutre.Struct_B.var_b);

		printf("5:(C:%4d)\r\n",lPacket.structutre.Struct_C.var_a);
		printf("5:(C:%4d)\r\n",lPacket.structutre.Struct_C.var_b);
		printf("5:(C:%4d)\r\n",lPacket.structutre.Struct_C.var_c);	
		printf("5:<<<< \r\n");		
		#endif /*DEBUG_5_RECEIVE_PACKET*/
	
	}
  
}

void vTask4SendPacket( void * pvParameters )
{
	uint32_t millisec = DELAY_MS_100*5;
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;	
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));
		
		// Perform action here.
		++Cnt_Packet;

		#if 1
		if((Cnt_Packet%3)==0)
		{
			gPacket.type = Type_A;
			SendPacket(&gPacket,sizeof(gPacket));

			#if defined (DEBUG_4_SEND_PACKET)
			printf("4:vTask4 (%4d,A+1)\r\n",Cnt_Packet);
			#endif /*DEBUG_4_SEND_PACKET*/
		}
		else if ((Cnt_Packet%3)==1)
		{
			gPacket.type = Type_B;
			SendPacket(&gPacket,sizeof(gPacket));	

			#if defined (DEBUG_4_SEND_PACKET)
			printf("4:vTask4 (%4d,B+10)\r\n",Cnt_Packet);		
			#endif /*DEBUG_4_SEND_PACKET*/
		}
		else if ((Cnt_Packet%3)==2)
		{
			gPacket.type = Type_C;
			SendPacket(&gPacket,sizeof(gPacket));

			#if defined (DEBUG_4_SEND_PACKET)
			printf("4:vTask4 (%4d,C+100)\r\n",Cnt_Packet);	
			#endif /*DEBUG_4_SEND_PACKET*/
		}
		

		#endif
	
	}
  
}

void vTask3CreateSubTask(void * pvParameters)
{

	(void) pvParameters;
	for( ;; )
	{

		#if defined (DEBUG_3_CREATE_TASK)
		printf("3:vTask3Sub Delete\r\n");
		#endif /*DEBUG_3_CREATE_TASK*/
		
		Flag_Func = 0 ;

		/* Delete the Init Thread */ 
		vTaskDelete(xTask3Sub);

	}
	
}

void vTask3CreateTask( void * pvParameters )
{

	(void) pvParameters;	
	for( ;; )
	{
		
		if (Flag_Func)
		{
			// Perform action here.			
			xTaskCreate(vTask3CreateSubTask, "vTask3Sub", configMINIMAL_STACK_SIZE/2, NULL, osPriorityAboveNormal, &xTask3Sub ); 
			
			#if defined (DEBUG_3_CREATE_TASK)
			printf("3:vTask3Sub Create\r\n");		
			#endif /*DEBUG_3_CREATE_TASK*/
		}
	}
  
}

void vTask2RisingFlag( void * pvParameters )
{
	uint32_t millisec = DELAY_MS_100*5;
	static uint16_t cnt = 0;
	
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.	
		Flag_Func = 1;		

		#if defined (DEBUG_2_RISING_FLAG)
		printf("2:Func(%4dms) :%4d\r\n" ,millisec, ++cnt);
		#else
		( void ) cnt;		//Remove compiler warning about cnt being set but never used.
		#endif	/*DEBUG_2_RISING_FLAG*/

		
	}
  
}

void vTask1LogMsg( void * pvParameters )
{	
	uint32_t millisec = DELAY_MS_1000;
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		// Perform action here.
		GPIO_ToggleBits(GPIOA,GPIO_Pin_5);
		
		#if defined (DEBUG_1_LOGMSG)
		printf("1:Log(%4dms):%4d (%d bytes)\r\n" ,millisec ,++Cnt_LogMsg,xPortGetFreeHeapSize());
		#endif	/*DEBUG_1_LOGMSG*/

	}
  
}

void vTask0InitUart(void * pvParameters)
{
	uint32_t millisec = DELAY_MS_1;
	portTickType xLastWakeTime;
//	Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	xSemaphoreTake(xSemaphore_UART,0);

	(void) pvParameters;
	for( ;; )
	{
		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, ( millisec *configTICK_RATE_HZ / ( ( TickType_t ) 1000 ) ));

		xSemaphoreTake(xSemaphore_UART, portMAX_DELAY);//portMAX_DELAY
		USART_Test();
	}
	
}

void initBSP(void)
{
	vSemaphoreCreateBinary(xSemaphore_UART);
	xSemaphoreTake(xSemaphore_UART,10);
	
	USART_Config();
	SysTickConfig();
	#if 0		//for test
	void (*pFun)() = SysTickConfig;	//function name = address
	(*pFun)();             
	pFun();	//same behavior
	#endif

	IWDG_Config();
	TIM3_Config();
	
	LED_Config();	//task1

	Custom_ADC_DMA_Config();	//task7

	PWM_Output_Config();//task8

	// TODO:
	TIM_Input_Config();//task9

	#if 0	//STM32F401 dont have True random number generator ...
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE);
	#endif

	ADC_Random_Config();//task11

	Custom_ButtonConfig();//task12

	PWM_Modulation_Config();//task13

	MixedLIghtLED1_Config();	//task15
	
}


void vPrintStringAndNumber( const portCHAR *pcString ,const portLONG pcNum ) 
{
	#if 1
	vTaskSuspendAll();
	{
		printf("%s%d\r\n", pcString,pcNum); 
		fflush(stdout); 
	}
	xTaskResumeAll();
	#else
	
	taskENTER_CRITICAL(); 
	{ 
		printf( "%s%d\r\n", pcString,pcNum); 
//		printf( "%s \r\n",pcNumString);		
		fflush( stdout ); 
	} 
	taskEXIT_CRITICAL(); 
	#endif	
} 

void vPrintString( const portCHAR *pcString ) 
{
	#if 1
	vTaskSuspendAll();
	{
		printf("%s",pcString);
		fflush(stdout);
	}
	xTaskResumeAll();
	#else
	taskENTER_CRITICAL(); 
	{ 
		printf( "%s", pcString ); 
		fflush( stdout ); 
	} 
	taskEXIT_CRITICAL(); 
	#endif
}

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}


void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}

void SendPacket(CommPacket* pData, unsigned int iLen)
{
	switch(pData->type)
	{
		case Type_A:
			pData->structutre.Struct_A.var_a += 1;
			pData->structutre.Struct_A.var_b += 1;			
			break;
			
		case Type_B:
			pData->structutre.Struct_B.var_a += 10;
			pData->structutre.Struct_B.var_b += 10;	
			break;
			
		case Type_C:
			pData->structutre.Struct_C.var_a += 100;
			pData->structutre.Struct_C.var_b += 100;
			pData->structutre.Struct_C.var_c += 100;			
			break;
	}
}

unsigned int RecvPacket(CommPacket* pData, unsigned int iLen)
{

	memcpy(pData,&gPacket,sizeof(gPacket));
	
	return 0;
}

void LEDx_Loop(int32_t bright,uint8_t LEDx,uint8_t Color)
{
	#define usDelay		(50)

	switch(LEDx)
	{
		case LED1:
			switch(Color)
			{
				case COLOR_R:
				    MixedLIghtLED1_R_ON();
					delay(bright *usDelay);
				    MixedLIghtLED1_R_OFF();
					delay((255 - bright) * usDelay);
					
				    if((bright++) >= 255) 
				    {
						bright = 0;
				    }					
					break;
				case COLOR_G:
				    MixedLIghtLED1_G_ON();
					delay(bright *usDelay);
				    MixedLIghtLED1_G_OFF();
					delay((255 - bright) * usDelay);
				    if((bright++) >= 255) 
				    {
						bright = 0;
				    }					
					break;
				case COLOR_B:
				    MixedLIghtLED1_B_ON();
					delay(bright *usDelay);
				    MixedLIghtLED1_B_OFF();
					delay((255 - bright) * usDelay);
				    if((bright++) >= 255) 
				    {
						bright = 0;
				    }					
					break;					
			}			
			break;
	}

}

void LEDx_Handler(uint8_t LEDx,uint32_t Hex)
{
	uint8_t hex_R = 0;
	uint8_t hex_G = 0;
	uint8_t hex_B = 0;

	hex_R =  (Hex>>16);
	hex_G =  ((Hex>>8)& 0x00FF);
	hex_B =  ((Hex)&0x0000FF);

	LEDx_Loop(hex_R,LEDx,COLOR_R);
	LEDx_Loop(hex_G,LEDx,COLOR_G);
	LEDx_Loop(hex_B,LEDx,COLOR_B);
}

void MixedLIghtLED1_Demo(void)	//RRGGBB
{
	static uint16_t MixedLIghtLED1Index = 0;
	
	//table refer to http://www.ifreesite.com/color/
	
	/*
		Duty: 0 ~ 100
		Hex : 0 ~ 0xFF

		formula : 
		Duty = (Hex*100) >>8 
		Hex = (Duty<<8)/100
	*/

	LEDx_Handler(LED1,RGBColorTable[MixedLIghtLED1Index]);
	MixedLIghtLED1Index = (MixedLIghtLED1Index>=(300-1))?(0):(MixedLIghtLED1Index+1);
			
}

/*
	B : TIM3_CH3/PB0
	R :	TIM3_CH2/PB5
	G :	TIM3_CH1/PB4
*/
void MixedLIghtLED1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
//	RCC_PCLK1Config(RCC_HCLK_Div2);
	
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void BitFlag_SwitchTest(uint8_t On)
{
	//BitFlag TEST
	if (On)
	{
		BitFlag_ON(ReadBit(BitFlag_ADC));
		BitFlag_ON(ReadBit(BitFlag_GPIO));
		BitFlag_ON(ReadBit(BitFlag_UART));
		BitFlag_ON(ReadBit(BitFlag_AAA));
		BitFlag_OFF(ReadBit(BitFlag_BBB));
		BitFlag_OFF(ReadBit(BitFlag_CCC));
		BitFlag_OFF(ReadBit(BitFlag_DDD));
		BitFlag_OFF(ReadBit(BitFlag_EEE));
	}
	else
	{
		BitFlag_OFF(ReadBit(BitFlag_ADC));
		BitFlag_OFF(ReadBit(BitFlag_GPIO));
		BitFlag_OFF(ReadBit(BitFlag_UART));
		BitFlag_OFF(ReadBit(BitFlag_AAA));
		BitFlag_ON(ReadBit(BitFlag_BBB));
		BitFlag_ON(ReadBit(BitFlag_CCC));
		BitFlag_ON(ReadBit(BitFlag_DDD));
		BitFlag_ON(ReadBit(BitFlag_EEE));
	}
}

void PWM_Modulation_Start(void)
{
	static uint16_t Counter_20ms = 0;
	static uint16_t Counter_3ms = 0;
	
	if (Counter_20ms++ >= (20))
	{
		TIM_OC1Init(TIM4, &TIM4_OC1InitStructure);
  		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);	
		TIM_CtrlPWMOutputs(TIM4, ENABLE);			
		if (++Counter_3ms >= (3))
		{
			Counter_20ms = 0;
			Counter_3ms = 0;
		}
	}
	else 
	{
		TIM_ForcedOC1Config(TIM4,TIM_ForcedAction_InActive);
	}
}

void PWM_Modulation_Config(void)	//PB6:38KHz , TIM4_CH1
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	uint16_t TimerPeriod = 0;
	uint16_t Channel1Pulse = 0;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);

	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);

	/*************************************************************************
		TIMx_ARR = TIM_Period , Auto-Reload Register
		TIMx_CCR1 = TIM_Pulse , Capture Compare Register

		** Prescaler = (TIMxCLK / TIMx counter clock) - 1
		
		** Timer clock = PWM frequency * 2^PWM resolution

		The objective is to generate PWM signal at xx KHz:
		** TIMx Frequency = TIMx counter clock/(TIM_Period + 1)
			-> 1/ TIMx Frequency = (TIM_Period + 1) * (TIM_Prescaler + 1) / TIMxCLK
			-> TIMx Frequency =  TIMxCLK / ((TIM_Period + 1) * (TIM_Prescaler + 1))
				=> example : 
					Freq = 100 Hz = TIMxCLK / ((TIM_Period + 1) * (TIM_Prescaler + 1))
								  = 48000000 / (4000-1     + 1) * (120 -1        + 1))
			
		** TIMx_Period = (SystemCoreClock / TIMx Frequency) - 1 , if prescaler = 1-1

		** duty cycle = (TIM_Pulse/ TIM_Period + 1)* 100
		** ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100	
	*************************************************************************/

	TimerPeriod = ((SystemCoreClock/2) / 38000 ) - 1;
	/* Compute CCR1 value to generate a duty cycle at 40% for channel 1 */
	Channel1Pulse = (uint16_t) (((uint32_t) 40 * (TimerPeriod - 1)) / 100);
	
	/* Time Base configuration */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 1 -1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;	
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 1;	//RM0316 , Figure 161	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCStructInit(&TIM4_OC1InitStructure);
	TIM4_OC1InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM4_OC1InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM4_OC1InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM4_OC1InitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;	
	TIM4_OC1InitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OC1Init(TIM4, &TIM4_OC1InitStructure);

	/* TIM4 counter enable */
	TIM_Cmd(TIM4, ENABLE);

	/* TIM4 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM4, DISABLE);

}


void Button_Procedure(void)
{
	Custom_ButtonScan();

	if (Custom_Button4PressedOnce())
	{
		IsBTN4Press = 1;
		
//		printf("4444\r\n");	
	}	

	if (Custom_Button4PressedLong()&& !Custom_Button4PressedOnce())
	{
		IsBTN4PressLong = 1;
		
//		printf("4444 long\r\n");
	}
}

uint16_t ADC_Random_GetSeed(void)
{
	uint8_t Count;
	uint16_t ADC_RandomSeek = 0;

	/* ADC1 regular Software Start Conv */ 
	ADC_SoftwareStartConv(ADC1);

	for(Count = 0; Count < 4; Count++)
	{

		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		ADC_RandomSeek <<= 4;
		ADC_RandomSeek += ADC_GetConversionValue(ADC1) & 0x000f;
	}

	return ADC_RandomSeek;
}


void ADC_Random_Config(void)	//use PC2 ADC to generate random number
{
	ADC_InitTypeDef       	ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef      	GPIO_InitStructure;
  
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_56Cycles);

	ADC_Cmd(ADC1, ENABLE);

	/* Wait the ADRDY flag */
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_STRT)); 
		
}

void LED_Config(void)	//PA5
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Configure USART Tx as alternate function  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TIM_Input_FreqCalculate(void)
{
	if(uhCaptureNumber == 0)
	{
		/* Get the Input Capture value */
		uhICReadValue1 = TIM_GetCapture2(TIM1);
		uhCaptureNumber = 1;
	}
	else if(uhCaptureNumber == 1)
	{
		/* Get the Input Capture value */
		uhICReadValue2 = TIM_GetCapture2(TIM1); 

		/* Capture computation */
		if (uhICReadValue2 > uhICReadValue1)
		{
			uwCapture = (uhICReadValue2 - uhICReadValue1); 
		}
		else if (uhICReadValue2 < uhICReadValue1)
		{
			uwCapture = ((0xFFFF - uhICReadValue1) + uhICReadValue2); 
		}
		else
		{
			uwCapture = 0;
		}
		/* Frequency computation */ 
		uwTIMFreq = (uint32_t) SystemCoreClock / uwCapture;
		uhCaptureNumber = 0;
	}
}

/*
	PA1 : TIM2_CH2
*/
void TIM_Input_Config(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;
	TIM_ICInitTypeDef  	TIM_ICInitStructure;

	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* GPIOA clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* TIM2 chennel2 configuration : PA.01 */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect TIM pin to AF2 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);

	/* Enable the TIM2 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;

  	TIM_ICInit(TIM2, &TIM_ICInitStructure);

	/* TIM enable counter */
	TIM_Cmd(TIM2, ENABLE);

	/* Enable the CC2 Interrupt Request */
	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
	
}


/*
	Freq : 10K
	PA8 : TIM1_CH1
*/
void PWM_Output_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint16_t Channel1Pulse = 0;

	/* GPIOA and GPIOB Clocks enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* GPIOA Configuration: Channel 1, 2, 3 and 4 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM1);

	/* Compute the value to be set in ARR regiter to generate signal frequency at 10 Khz */
	Channel1Period = ((SystemCoreClock) / 10000 ) - 1;
	/* Compute CCR3 value to generate a duty cycle at 50%  for channel 3*/
	Channel1Pulse = (uint16_t) (((uint32_t) 5 * (Channel1Period - 1)) / 10);

	/* TIM2 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
	TIM_TimeBaseStructure.TIM_Period = Channel1Period;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 1;	//RM0316 , Figure 161

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM1_OC1InitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM1_OC1InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM1_OC1InitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM1_OC1InitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM1_OC1InitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM1_OC1InitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM1_OC1InitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM1_OC1InitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OC1Init(TIM1, &TIM1_OC1InitStructure);

  	TIM_CCPreloadControl(TIM1, ENABLE);

	/* TIM1 counter enable */
	TIM_Cmd(TIM1, ENABLE);

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
}

uint32_t GetLSIFrequency(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	RCC_ClocksTypeDef  RCC_ClockFreq;

	/* Enable the LSI oscillator ************************************************/
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{
	}

	/* TIM5 configuration *******************************************************/ 
	/* Enable TIM5 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* Connect internally the TIM5_CH4 Input Capture to the LSI clock output */
	TIM_RemapConfig(TIM5, TIM5_LSI);

	/* Configure TIM5 presclaer */
	TIM_PrescalerConfig(TIM5, 0, TIM_PSCReloadMode_Immediate);

	/* TIM5 configuration: Input Capture mode ---------------------
	The LSI oscillator is connected to TIM5 CH4
	The Rising edge is used as active edge,
	The TIM5 CCR4 is used to compute the frequency value 
	------------------------------------------------------------ */
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TIM5, &TIM_ICInitStructure);

	/* Enable TIM5 Interrupt channel */
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable TIM5 counter */
	TIM_Cmd(TIM5, ENABLE);

	/* Reset the flags */
	TIM5->SR = 0;

	/* Enable the CC4 Interrupt Request */  
	TIM_ITConfig(TIM5, TIM_IT_CC4, ENABLE);


	/* Wait until the TIM5 get 2 LSI edges (refer to TIM5_IRQHandler() in 
	stm32f4xx_it.c file) ******************************************************/
	while(uwCaptureNumber != 2)
	{
	}
	/* Deinitialize the TIM5 peripheral registers to their default reset values */
	TIM_DeInit(TIM5);


	/* Compute the LSI frequency, depending on TIM5 input clock frequency (PCLK1)*/
	/* Get SYSCLK, HCLK and PCLKx frequency */
	RCC_GetClocksFreq(&RCC_ClockFreq);

	/* Get PCLK1 prescaler */
	if ((RCC->CFGR & RCC_CFGR_PPRE1) == 0)
	{ 
		/* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
		return ((RCC_ClockFreq.PCLK1_Frequency / uwPeriodValue) * 8);
	}
	else
	{ /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
		return (((2 * RCC_ClockFreq.PCLK1_Frequency) / uwPeriodValue) * 8) ;
	}
}

void IWDG_Config(void)
{

	/* Check if the system has resumed from IWDG reset */
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		/* Clear reset flags */
		RCC_ClearFlag();
	}

	/* Get the LSI frequency:  TIM5 is used to measure the LSI frequency */
	uwLsiFreq = GetLSIFrequency();

	/* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
	dispersion) */
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	/* IWDG counter clock: LSI/32 */
	IWDG_SetPrescaler(IWDG_Prescaler_32);

	/* Set counter reload value to obtain 250ms IWDG TimeOut.
	IWDG counter clock Frequency = LsiFreq/32
	Counter Reload Value = 250ms/IWDG counter clock period
	                  = 0.25s / (32/LsiFreq)
	                  = LsiFreq/(32 * 4)
	                  = LsiFreq/128
	*/   
//	IWDG_SetReload(uwLsiFreq/128);
	IWDG_SetReload(uwLsiFreq/107);	//300ms	
//	IWDG_SetReload(uwLsiFreq/64);	//500ms	
	
	/* Reload IWDG counter */
	IWDG_ReloadCounter();

	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();

}


/*
	TIMxCLK = PCLK1 = HCLK = SystemCoreClock
	TIMx counter clock = TIMxCLK /((Prescaler + 1)*(Period + 1))
	                = 84 MHz / ((11+1)*(6999+1))
	                = 1000 Hz 
     ==> TIMx counter period = 1 ms
*/
void TIM2_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_PCLK1Config(RCC_HCLK_Div2);
	
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* TIMx base configuration */
	TIM_TimeBaseStructure.TIM_Period = (7000 -1);
	TIM_TimeBaseStructure.TIM_Prescaler = (12 -1);
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/* TIMx Interrupts enable */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	/* TIMx enable counter */
	TIM_Cmd(TIM2, ENABLE);

	/* Enable the TIMx gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

}

/*
	TIMxCLK = PCLK1 = HCLK = SystemCoreClock
	TIMx counter clock = TIMxCLK /((Prescaler + 1)*(Period + 1))
	                = 84 MHz / ((119+1)*(6999+1))
	                = 100 Hz 
     ==> TIMx counter period = 10 ms
*/
void TIM3_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_PCLK1Config(RCC_HCLK_Div2);
	
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* TIMx base configuration */
	TIM_TimeBaseStructure.TIM_Period = (7000 -1);
	TIM_TimeBaseStructure.TIM_Prescaler = (12 -1);	//	120
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* TIMx Interrupts enable */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	/* TIMx enable counter */
	TIM_Cmd(TIM3, ENABLE);

	/* Enable the TIMx gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

}

void USART_Test(void)
{	
	if (UartRxFlag)
	{
		switch (UartRxBuffer)
		{
			case '1':
	
				break;
				
			case 'Z':
			case 'z':				
				NVIC_SystemReset();
				break;				
			default : 
				UartRxFlag = FALSE;				
				break;
		}
		UartRxFlag = FALSE;
	}
}

void USART_Config(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Enable UART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);

	/* Connect PXx to USARTx_Rx*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);

	/* Configure USART Tx as alternate function  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USARTx configured as follows:
	- BaudRate = 115200 baud  
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* USART configuration */
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 

	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
	{}
	
	/* NVIC configuration */
	/* Enable the USARRx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);


	printf("\n\rUSART Printf Example: retarget the C library printf function to the USART\n\r");


}

void SysTickConfig(void)
{
	RCC_ClocksTypeDef RCC_Clocks;
	/* This function fills the RCC_ClockFreq structure with the current
	 frequencies of different on chip clocks (for debug purpose) */	
	RCC_GetClocksFreq(&RCC_Clocks);
	
	printf("===========================\r\n");
	printf( "Compiled on %s at %s\n", __TIME__, __DATE__ );    
	printf ("Function = %s,%s\n", __FUNCTION__,__func__);   	
	printf("SYSCLK_Frequency = %d Hz\n\r",RCC_Clocks.SYSCLK_Frequency);	
	printf("AHB = %d Hz\n\r",RCC_Clocks.HCLK_Frequency);
	printf("APB1 = %d Hz\n\r",RCC_Clocks.PCLK1_Frequency);
	printf("APB2 = %d Hz\n\r",RCC_Clocks.PCLK2_Frequency);

	#if 1	//monitor reset from ...
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		RCC_ClearFlag();
		printf("RCC_FLAG_IWDGRST\r\n");
	}
	if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
	{
		RCC_ClearFlag();
		printf("RCC_FLAG_PORRST\r\n");
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
	{
		RCC_ClearFlag();
		printf("RCC_FLAG_SFTRST\r\n");
	}	
	else if (RCC_GetFlagStatus(RCC_FLAG_BORRST) != RESET)
	{
		printf("RCC_FLAG_BORRST\r\n");
	}	
	#endif
	
//	/* Setup SysTick Timer for 1ms interrupts  */
//	if (SysTick_Config(SystemCoreClock / 1000))
//	{
//		/* Capture error */
//		while (1);
//	}
//	
//	/* Configure the SysTick handler priority */
//	NVIC_SetPriority(SysTick_IRQn, 0x01);
	
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t uTime)
{ 
	uwTimingDelay = uTime;
	while(uwTimingDelay != 0);
}

static void delay(__IO uint32_t nCount)
{
	__IO uint32_t index = 0; 
	for(index = (1 * nCount); index != 0; index--)
	{
	}
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
  }
}

//currently not use
/*

void SystemClkDelay(void)
{
	uint32_t i;
	i = 0xffff;
	while(i--);
}

void wtPutChar(uint8_t ccc)
{
	UART1_SendData8(ccc);
	// Loop until the end of transmission 
	while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);	
}

u16 GetAbsTime(u16 a,u16 b)
{
	u16 c;
	if(a>=b) c=(a-b);
	else c=65535-(b-a);	
	
	return c;
}
*/
uint8_t UART_GetByte(void)
{
	while ( USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
	{
	}
	return (uint8_t)USART_ReceiveData(USART2);
}

void UART_SendByte(uint8_t Data)
{
	USART_SendData(USART2 , (unsigned char)Data);
	while (USART_GetFlagStatus(USART2 , USART_FLAG_TC)==RESET);
	{
	}
}

void UART_SendString(uint8_t* Data,uint16_t len)
{
	#if 1
	uint16_t i=0;
	for(i=0;i<len;i++ )
	{
		UART_SendByte(Data[i]);
	}
	#else	//ignore len
    while(*Data)  
    {  
        USART_SendData(USART2, (unsigned char) *Data++);  
        /* Loop until the end of transmission */  
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);  //USART_FLAG_TXE
    } 
	#endif
}

void SystemClkDelay(uint32_t u32Delay)
{
	//uint32_t i;
	//i = 0xffff;
	while(u32Delay--);
}




