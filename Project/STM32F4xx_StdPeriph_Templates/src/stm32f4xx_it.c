/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "stm32f4xx_it.h"
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//extern __IO uint32_t uwTimingDelay;

static __IO uint32_t TIM2_Counter = 0;
static __IO uint32_t TIM3_Counter = 0;

static uint16_t tmpCC4[2] = {0, 0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
__weak void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
__weak void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
__weak void SysTick_Handler(void)
{
//	TimingDelay_Decrement();
}

void USART2_IRQHandler(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken; 
	xHigherPriorityTaskWoken = pdFALSE; 
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	
		if (!UartRxFlag)
		{
			UartRxBuffer = USART_ReceiveData(USART2);
			printf("temp = 0x%x \n\r",UartRxBuffer);		
			UartRxFlag = 1;
		}

		xSemaphoreGiveFromISR(xSemaphore_UART, &xHigherPriorityTaskWoken ); 	
		if( xHigherPriorityTaskWoken == pdTRUE ) 
		{ 
		  	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
		}
		
	}
}

void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM5, TIM_IT_CC4) != RESET)
	{    
		/* Get the Input Capture value */
		tmpCC4[uwCaptureNumber++] = TIM_GetCapture4(TIM5);

		/* Clear CC4 Interrupt pending bit */
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC4);

		if (uwCaptureNumber >= 2)
		{
			/* Compute the period length */
			uwPeriodValue = (uint16_t)(0xFFFF - tmpCC4[0] + tmpCC4[1] + 1);
		}
	}
}

void DMA2_Stream0_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0))
	{
		Custom_ADC_Filter();
		Custom_ADC_Filter_FFT();

		DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);		
	}
}

void TIM2_IRQHandler(void)
{ 
	if(TIM_GetITStatus(TIM1, TIM_IT_CC2) == SET) 
	{
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
		TIM_Input_FreqCalculate();
	}
}

void TIM3_IRQHandler(void)
{
	static uint16_t cnt_iwdg = 0;
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{ 
		//Insert TIMx function
		TimingDelay_Decrement();

		if (cnt_iwdg++>=200)
		{
			cnt_iwdg = 0;
			IWDG_ReloadCounter();
		}
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);		
	}
}


/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
