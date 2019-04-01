/* Includes ------------------------------------------------------------------*/
#include "debug.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#if !defined (__ICCARM__)
struct __FILE { int handle; /* Add whatever is needed */ };

FILE __stdout;
FILE __stdin;

#endif

#if defined (SWV_ENABLE)
int fputc(int ch, FILE *f) {
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
}
#endif

