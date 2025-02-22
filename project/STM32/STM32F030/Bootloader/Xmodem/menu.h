/******************************************************************************
****版本：1.0.0
****平台：
****日期：2020-07-29
****作者：Qitas
****版权：
*******************************************************************************/
#ifndef __MENU_H
#define __MENU_H

#include "stm32f0xx_hal.h"
#include "main.h"

/* Define the address from where user application will be loaded.
   Note: this area is reserved for the IAP code                  */

//#define FLASH_PAGE_STEP         FLASH_PAGE_SIZE           /* Size of page : 1 Kbytes */
#define APPLICATION_ADDRESS     (uint32_t)0x08002000      /* Start user code address: 8K */
#define USER_FLASH_END_ADDRESS  (uint32_t)0x08003F00
//#define USER_FLASH_SIZE         (uint32_t)0x00002000 /* Small default template application */

typedef void (*pFunction)(void);
extern  pFunction JumpToApplication;
extern  uint32_t JumpAddress;

extern void Boot2APP(void);
extern void Boot(void);

#endif

/*-------------------------(C) COPYRIGHT 2020 QITAS --------------------------*/
