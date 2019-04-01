/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	initBSP();

	xTaskCreate(vTask0InitUart		,"vTaskInitUart"			,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL);

	//enable log message , 1000ms
	xTaskCreate(vTask1LogMsg		,"vTask1LogMsg"			,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL);   	

	//function flag enable and create sub task
	xTaskCreate(vTask2RisingFlag		,"vTask2Rising"			,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL);   	
	xTaskCreate(vTask3CreateTask		,"vTask3Create"			,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL); 

	//send data into structure and read
	xTaskCreate(vTask4SendPacket		,"vTask4Send"			,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL); 
	xTaskCreate(vTask5RecvPacket		,"vTask5Recv"			,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL); 

	//display current task list
	xTaskCreate(vTask6Tasklist		,"vTask6Tasklist"			,configMINIMAL_STACK_SIZE<<4	,NULL	,osPriorityNormal		,NULL); 	

	//enable ADC
	xTaskCreate(vTask7ADCAverage	,"vTask7ADC"			,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL); 

	//Adjust PWM duty by ADC value (PB0 , CN8:A3 PIN)
	xTaskCreate(vTask8PWMAdjust	,"vTask8PWM"			,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL); 

	//Measure PWM duty
//	xTaskCreate(vTask9TIMMeasure	,"vTask9Measure"	,configMINIMAL_STACK_SIZE*2,NULL	,osPriorityNormal		,NULL); 

	//Get ADC by FFT
	xTaskCreate(vTask10ADCFFT		,"vTask10ADCFFT"		,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL); 

	//Create random number with ADC
	xTaskCreate(vTask11ADCRAND		,"vTask11ADCRAND"		,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL); 

	//Button debounce
	xTaskCreate(vTask12ButtonBebounce,"vTask12Button"		,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL); 	

	//PWM in Timer , modulation , 38K in 50Hz
	xTaskCreate(vTask13PWMModulation,"vTask13Modulation"	,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL); 

	//bit flag switch
	xTaskCreate(vTask14BitFlag		,"vTask14BitFlag"			,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL); 

	//analog PWM
//	xTaskCreate(vTask15AnalogPWM	,"vTask15AnalogPWM"	,configMINIMAL_STACK_SIZE	,NULL	,osPriorityNormal		,NULL); 
	
	
	/* Start the scheduler. */
	vTaskStartScheduler();
		
	/* Infinite loop */
	while (1)
	{
	
	}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
