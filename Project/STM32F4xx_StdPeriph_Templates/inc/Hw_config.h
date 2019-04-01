/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Platform config -----------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "Macro.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debug.h"

#include "Custom_Button.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "Queue.h"
#include "semphr.h"

#include <cmsis_os.h> 

#include "Custom_ADC.h"


/* Define config -------------------------------------------------------------*/

typedef struct
{
	unsigned int var_a;
	unsigned int var_b;
}PacketStructA;

typedef struct
{
	unsigned int var_a;
	unsigned int var_b;
}PacketStructB;

typedef struct
{
	unsigned int var_a;
	unsigned int var_b;
	unsigned int var_c;	
	float var_d;
}PacketStructC;

typedef enum
{ 
	Type_A = 0,
	Type_B,
	Type_C,  

	Type_NULL,
	
}PacketType;

typedef struct
{
	PacketStructA Struct_A;
	PacketStructB Struct_B;
	PacketStructC Struct_C;
}PacketStructX;

typedef struct
{
	PacketType type;
	PacketStructX structutre;	
}CommPacket;

typedef enum
{ 
 	BitFlag_ADC = 0,
 	BitFlag_GPIO,
 	BitFlag_UART,
 	BitFlag_AAA,
 	BitFlag_BBB,
 	BitFlag_CCC,
 	BitFlag_DDD,
 	BitFlag_EEE,
	
}BitFlagType;

#if 0
#define FIELD   unsigned char
#define BYTE   	unsigned char
typedef volatile union _BITS_8
{
    BYTE byte;
	struct
	{
		FIELD bit0: 1;
		FIELD bit1: 1;
		FIELD bit2: 1;
		FIELD bit3: 1;
		FIELD bit4: 1;
		FIELD bit5: 1;
		FIELD bit6: 1;
		FIELD bit7: 1;
	} fbit;
}BITS_8;
#endif

#define	DELAY_MS_1			(1)
#define	DELAY_MS_10		(10)
#define	DELAY_MS_100		(100)
#define	DELAY_MS_250		(250)
#define	DELAY_MS_1000		(1000)

//#define DEBUG_15_ANALOG_PWM
//#define DEBUG_14_BIT_FLAG
//#define DEBUG_13_PWM_MODULATION
//#define DEBUG_12_BUTTON_DEBOUNCE
//#define DEBUG_11_ADC_RANDOM
//#define DEBUG_10_ADC_FFT
//#define DEBUG_9_TIM_MEASURE
//#define DEBUG_8_PWM_ADJUST
//#define DEBUG_7_ADC_AVERAGE
//#define DEBUG_6_DISPLAY_TASKLIST
//#define DEBUG_5_RECEIVE_PACKET
//#define DEBUG_4_SEND_PACKET
//#define DEBUG_3_CREATE_TASK
//#define DEBUG_2_RISING_FLAG
#define DEBUG_1_LOGMSG

/* Macro ---------------------------------------------------------------------*/
#define FREERTOS_PRINTF

#if defined (FREERTOS_PRINTF)
#define printf(...)	\
{	\
	vTaskSuspendAll();	\
	printf(__VA_ARGS__);	\
	fflush(stdout); \
	xTaskResumeAll();	\
}	\

#else

#define printf(...)	\
{	\
	printf(__VA_ARGS__);	\
}	\

#endif

enum
{
	LED1 = 0x00,
	LED2 ,
};

enum
{
	COLOR_R = 0x00,
	COLOR_G ,
	COLOR_B ,	
};

/* Exported types ------------------------------------------------------------*/
extern TaskHandle_t	xTask3Sub;
extern SemaphoreHandle_t 	xSemaphore_UART;

extern __IO uint8_t UartRxBuffer;
extern __IO uint8_t UartRxFlag;
extern TIM_OCInitTypeDef  TIM4_OC1InitStructure;

extern __IO unsigned long int uwCaptureNumber;
extern __IO unsigned long int uwPeriodValue;

void vTask15AnalogPWM( void * pvParameters );
void vTask14BitFlag( void * pvParameters );
void vTask13PWMModulation( void * pvParameters );
void vTask12ButtonBebounce( void * pvParameters );
void vTask11ADCRAND( void * pvParameters );
void vTask10ADCFFT( void * pvParameters );
void vTask9TIMMeasure( void * pvParameters );
void vTask8PWMAdjust( void * pvParameters );
void vTask7ADCAverage( void * pvParameters );
void vTask6Tasklist( void * pvParameters );
void vTask5RecvPacket( void * pvParameters );
void vTask4SendPacket( void * pvParameters );

void vTask3CreateSubTask(void * pvParameters);
void vTask3CreateTask( void * pvParameters );
void vTask2RisingFlag( void * pvParameters );

void vTask1LogMsg( void * pvParameters );
void vTask0InitUart(void * pvParameters);

void initBSP(void);

void vPrintStringAndNumber( const portCHAR *pcString ,const portLONG pcNum ); 
void vPrintString( const portCHAR *pcString );

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName );
void vApplicationIdleHook( void );

void SendPacket(CommPacket* pData, unsigned int iLen);
unsigned int RecvPacket(CommPacket* pData, unsigned int iLen);

void MixedLIghtLED1_Demo(void);
void MixedLIghtLED1_Config(void);

void BitFlag_SwitchTest(uint8_t On);

void PWM_Modulation_Start(void);
void PWM_Modulation_Config(void);

void Button_Procedure(void);

uint16_t ADC_Random_GetSeed(void);
void ADC_Random_Config(void);
void TIM_Input_FreqCalculate(void);
void LED_Config(void);
void TIM_Input_Config(void);
void PWM_Output_Config(void);
void IWDG_Config(void);

void TIM2_Config(void);
void TIM3_Config(void);

void USART_Test(void);
void USART_Config(void);

void SysTickConfig(void);
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t uTime);
/* Exported constants --------------------------------------------------------*/

#endif  /* __HW_CONFIG_H */

