/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CUSTOM_ADC_H
#define __CUSTOM_ADC_H

/* Platform config -----------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>

#include "arm_math.h" 
#include "arm_const_structs.h"

/* Define config -------------------------------------------------------------*/
//ADC variable
#define ADC_VREF						3300	//3300
#define ADC_BITFACTOR					0xFFF
#define ADC_RESOLUTION					ADC_Resolution_12b
#define ADC_SAMPLE						(16)
#define ADC_CH							3		//(3)

/*ADC FFT*/
#define FFT_SAMPLES 					(ADC_SAMPLE*2)//2048 
#define FFT_SIZE							(FFT_SAMPLES/2)
#define IFFT_FLAG						(0)
#define DOBITREVERSE					(1)


/* Macro ---------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
void Custom_ADC_Voltage(void);
void Custom_ADC_Filter(void);
void Custom_ADC_Filter_FFT(void);
void Custom_ADC_DMA_Config(void);

/* Exported constants --------------------------------------------------------*/

#endif  /* __CUSTOM_ADC_H */

