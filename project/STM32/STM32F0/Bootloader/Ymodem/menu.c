/******************************************************************************
****版本：1.0.0
****平台：
****日期：2020-07-29
****作者：Qitas
****版权：
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "common.h"
#include "flash_if.h"
#include "menu.h"
#include "ymodem.h"
#include "xmodem.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
pFunction JumpToApplication;
uint32_t JumpAddress;
uint32_t FlashProtection = 0;
uint8_t aFileName[FILE_NAME_LENGTH];

/* Private function prototypes -----------------------------------------------*/
void SerialDownload(void);
void SerialUpload(void);


/*-------------------------(C) COPYRIGHT 2020 QITAS --------------------------*/
