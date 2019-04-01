/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEBUG_H
#define __DEBUG_H

/* Platform config -----------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

/* Define config -------------------------------------------------------------*/
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

//#define SWV_ENABLE

/* Macro ---------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#endif  /* __DEBUG_H */

