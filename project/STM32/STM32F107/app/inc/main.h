/**********************    COPYRIGHT 2014-2015, EYBOND    ************************ 
  * @File    : main.h 
  * @Author  : Mencius 
  * @Date    : 2015-08-02
  * @Brief   : Header for main.c module
**********************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_usart.h"
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/


/* Exported macro ------------------------------------------------------------*/
#define TRUE  1
#define FALSE 0

#define NULL           ((void*)0)
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;			/* Unsigned  8 bit quantity       */
typedef signed   char  INT8S;			/* Signed    8 bit quantity       */
typedef unsigned short INT16U;			/* Unsigned 16 bit quantity       */
typedef signed   short INT16S;			/* Signed   16 bit quantity       */
typedef unsigned int   INT32U;			/* Unsigned 32 bit quantity       */
typedef signed   int   INT32S;			/* Signed   32 bit quantity       */
typedef float          FP32;			/* Single precision floating point*/
typedef double         FP64;			/* Double precision floating point*/
/* Exported variables --------------------------------------------------------*/ 
extern INT32U dwCurrentTime;
extern INT8U  bBoot;
extern INT16U wSwType;
extern INT16U wSerialBaud;
extern INT8U  bReboot;

#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports 
  *         the name of the source file and the source line number of the call 
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */
/* Exported functions ------------------------------------------------------- */ 
extern void memCpy(INT8U* pDest, INT8U* pSrc, INT16U wSize);
extern void memCpyAll(INT8U* pDest, INT8U* pSrc, INT16U wSize);
extern void memClr(INT8U* pDest, INT16U wSize);
extern INT32U getTime(void);

#ifdef __cplusplus
    }
#endif

#endif 

/******************************************************************************
 End of the File (EOF). Do not put anything after this part!
******************************************************************************/
