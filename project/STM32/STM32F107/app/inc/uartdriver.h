/**********************    COPYRIGHT 2014-2015, EYBOND    ************************
  * @File    : uartdriver.h 
  * @Author  : Mencius 
  * @Date    : 2015-08-04
  * @Brief   : Header for uartdriver.c module
**********************************************************************************/
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UARTDRIVER_H
#define UARTDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported macro ------------------------------------------------------------*/ 
#define cMaxUartDataLen                 256

#define cUartTxRdy				        0
#define cUartTxBusy				        1

#define cUartRxRdy				        0
#define cUartRxBufEmpty			        1

// ------------------------- GPIO define -------------------------
#define UART5_TxPort					GPIOC
#define UART5_RxPort					GPIOD
#define UART5_TxPin						GPIO_Pin_12
#define UART5_RxPin						GPIO_Pin_2
#define UART5_GPIO_CLK  			    RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC
#define UART5_UART_CLK  			    RCC_APB1Periph_UART5

/* Exported types ------------------------------------------------------------*/

typedef enum {
	emCOM_TX = 0,
	emCOM_RX = 1, 
}COM_DIRECT;

typedef /*__packed*/ struct{                 // 数据发送控制块
	INT8U	bTxStatus;          // 数据发送状态
	INT8U   *pbTxBuffer;        // 发送数据缓冲区
	INT16U	wTxDataLen;         // 发送缓冲区中的有效数据长度
	INT32U  dwTxLastTime;       // 最后一次发送数据时间(Tick数)
}UART_TX_CB;

typedef /*__packed*/ struct{                 // 数据接收控制块
	INT8U	bRxStatus;          // 数据接收状态 
	INT8U   *pbRxBuffer;        // 接收数据缓冲区
	INT16U	wRxDataLen;         // 接收缓冲区中的有效数据长度
	INT32U  dwRxLastTime;       // 最后一个数据的接收时间(Tick数)
}UART_RX_CB;

/* Exported constants --------------------------------------------------------*/ 
extern const INT8U bSpace;
extern const INT8U bReturn;
extern const INT8U bSem;

/* Exported variables --------------------------------------------------------*/ 
extern UART_TX_CB UartTxCbTbl;
extern UART_RX_CB UartRxCbTbl;

/* Exported functions ------------------------------------------------------- */ 
extern void     UART_DRV_Initial(void);
extern void     UART_DRV_RxInit(void);
extern void     UART_DRV_TxInit(void);
extern void     UART_DRV_ProcessISR(void);
extern void     UART_DRV_Config(INT32U BaudRate);
extern void     UART_DRV_Config_NoBoot(INT32U BaudRate);
extern void     UART_DRV_Config_Boot(INT32U BaudRate); 
extern BOOLEAN  UART_DRV_SendData(void);

#ifdef __cplusplus
    }
#endif

#endif

/******************************************************************************
 End of the File (EOF). Do not put anything after this part!
******************************************************************************/

