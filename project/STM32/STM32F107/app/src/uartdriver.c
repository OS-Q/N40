/**********************    COPYRIGHT 2014-2015, EYBOND    ************************
  * @File    : uartdriver.c 
  * @Author  : R.Y. 
  * @Date    : 2016-05-17
  * @Brief   : The public types,constants,macros and functions for uart communication
**********************************************************************************/
#include "uartdriver.h"
#include "main.h"

/* Private macro ------------------------------------------------------------*/
/* Private types ------------------------------------------------------------*/
/* Private constants --------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
const INT8U bSpace  = 0x20;            // 空格字符
const INT8U bReturn = 0x0A;            // 换行字符 
const INT8U bSem    = 0x7C;            // 分隔字符

UART_TX_CB UartTxCbTbl = {0};          // 各 UART 对应的数据发送控制块
UART_RX_CB UartRxCbTbl = {0};          // 各 UART 对应的数据发送控制块

INT8U bUartTxBuf[cMaxUartDataLen];     // 各 UART 对应的数据发送缓冲区
INT8U bUartRxBuf[cMaxUartDataLen];     // 各 UART 对应的数据接收缓冲区

/* Private functions ------------------------------------------------------- */
void UART_DRV_RxInit(void);
void UART_DRV_TxInit(void);
void UART_DRV_SaveData(INT8U bData);

/*****************************************************************************
 Function : UART_DRV_Config
 Brief    : UART driver configuration
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5
            [BaudRate] baud rate
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void UART_DRV_Config(INT32U BaudRate)
{ 
	GPIO_InitTypeDef    GPIO_InitStructure; 
	USART_InitTypeDef   USART_InitStructure; 

	// ---- GPIO Configuration ----
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	// System Clocks Configuration
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);

	// Configure UART5 Tx as alternate function push-pull 
	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Configure UART5 Rx as input floating 
	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
	        

	// ---- UART Configuration ----

	// Deinitializes the peripheral registers to default values
	USART_DeInit(USART1);

	// Initialize the UART parameters
	// - 8 data bits, 1 stop bit, no parity 
	// - Hardware flow control disabled (RTS and CTS signals)
	// - Receive and transmit enabled
	USART_InitStructure.USART_BaudRate = BaudRate; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
	USART_Init(USART1, &USART_InitStructure);

	// Enable UART(enable the specified interrupts)
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE); 
	USART_Cmd(USART1, ENABLE); 

	// Clear interrupt bits 
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}

/*****************************************************************************
 Function : UART_DRV_Config
 Brief    : UART driver configuration
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5
            [BaudRate] baud rate
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void UART_DRV_Config_NoBoot(INT32U BaudRate)
{ 
//	GPIO_InitTypeDef    GPIO_InitStructure; 
	USART_InitTypeDef   USART_InitStructure; 

	// ---- GPIO Configuration ----
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	// System Clocks Configuration
//	RCC_APB2PeriphClockCmd(UART5_GPIO_CLK,  ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART1,  ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	// Configure UART5 Tx as alternate function push-pull 
//	GPIO_InitStructure.GPIO_Pin     = UART5_TxPin;
//	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_OD;
//	GPIO_Init(UART5_TxPort, &GPIO_InitStructure);

	// Configure UART5 Rx as input floating 
//	GPIO_InitStructure.GPIO_Pin     = UART5_RxPin;
//	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(UART5_RxPort, &GPIO_InitStructure);  
	        

	// ---- UART Configuration ----

	// Deinitializes the peripheral registers to default values
	USART_DeInit(USART1);

	// Initialize the UART parameters
	// - 8 data bits, 1 stop bit, no parity 
	// - Hardware flow control disabled (RTS and CTS signals)
	// - Receive and transmit enabled
	USART_InitStructure.USART_BaudRate = BaudRate; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
	USART_Init(USART1, &USART_InitStructure);

	// Enable UART(enable the specified interrupts)
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE); 
	USART_Cmd(USART1, ENABLE); 

	// Clear interrupt bits 
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}

/*****************************************************************************
 Function : UART_DRV_Config
 Brief    : UART driver configuration
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5
            [BaudRate] baud rate
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void UART_DRV_Config_Boot(INT32U BaudRate)
{ 
//	GPIO_InitTypeDef    GPIO_InitStructure; 
	USART_InitTypeDef   USART_InitStructure; 

	// ---- GPIO Configuration ----
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	// System Clocks Configuration
//	RCC_APB2PeriphClockCmd(UART5_GPIO_CLK,  ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART1,  ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	// Configure UART5 Tx as alternate function push-pull 
//	GPIO_InitStructure.GPIO_Pin     = UART5_TxPin;
//	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_OD;
//	GPIO_Init(UART5_TxPort, &GPIO_InitStructure);

	// Configure UART5 Rx as input floating 
//	GPIO_InitStructure.GPIO_Pin     = UART5_RxPin;
//	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(UART5_RxPort, &GPIO_InitStructure);  
	        

	// ---- UART Configuration ----

	// Deinitializes the peripheral registers to default values
	USART_DeInit(USART1);

	// Initialize the UART parameters
	// - 8 data bits, 1 stop bit, no parity 
	// - Hardware flow control disabled (RTS and CTS signals)
	// - Receive and transmit enabled
	USART_InitStructure.USART_BaudRate = BaudRate; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
	USART_Init(USART1, &USART_InitStructure);

	// Enable UART(enable the specified interrupts)
	//    USART_ITConfig(UART5,USART_IT_RXNE,ENABLE); 
	USART_Cmd(USART1, ENABLE); 

	// Clear interrupt bits 
	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}

/*****************************************************************************
 Function : UART_DRV_Initial
 Brief    : UART driver initial
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void UART_DRV_Initial(void)
{
	// 初始化接收控制块
	UART_DRV_RxInit();

	// 初始化发送控制块
	UART_DRV_TxInit();
}

/*****************************************************************************
 Function : UART_DRV_RxInit
 Brief    : UART Rx initial
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void UART_DRV_RxInit(void)
{
	UART_RX_CB   *pUartRxCB   = NULL;

	// 初始化接收缓冲区 
	memClr(bUartRxBuf,sizeof(INT8U)*cMaxUartDataLen);  

	// 初始化接收控制块
	pUartRxCB = &UartRxCbTbl;
	pUartRxCB->bRxStatus = cUartRxBufEmpty;
	pUartRxCB->wRxDataLen = 0;
	pUartRxCB->dwRxLastTime = getTime();
	pUartRxCB->pbRxBuffer = bUartRxBuf; 
}

/*****************************************************************************
 Function : UART_DRV_TxInit
 Brief    : UART Tx initial
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void UART_DRV_TxInit(void)
{
	UART_TX_CB   *pUartTxCB   = NULL;

	// 初始化发送缓冲区 
	memClr(bUartTxBuf,sizeof(INT8U)*cMaxUartDataLen); 

	// 初始化发送控制块
	pUartTxCB = &UartTxCbTbl;
	pUartTxCB->bTxStatus = cUartTxRdy;
	pUartTxCB->wTxDataLen = 0;
	pUartTxCB->dwTxLastTime = getTime();
	pUartTxCB->pbTxBuffer = bUartTxBuf; 
}

/*****************************************************************************
 Function : UART_DRV_ProcessISR
 Brief    : Uart interrupt service
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void UART_DRV_ProcessISR(void)
{
	INT8U   bRevData = 0;

	// 接收到数据 
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET){ 
		// 清除中断位 
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);

		// 存储数据(一次存储一个字节)
		bRevData = (INT8U)USART_ReceiveData(USART1); 
		UART_DRV_SaveData(bRevData);
	}else{
		// 其它情况丢弃数据
		bRevData = (INT8U)USART_ReceiveData(USART1); 
	}
}

/*****************************************************************************
 Function : UART_DRV_SaveData
 Brief    : Save a byte from Rx of the UART to buffer
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5
            [bData] The data to save
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void UART_DRV_SaveData(INT8U bData)
{
	UART_RX_CB  *pUartRxCB   = NULL;

	// 获取接收控制块
	pUartRxCB = &UartRxCbTbl;

	// 判断缓冲区是否已满
	if(pUartRxCB->wRxDataLen >= cMaxUartDataLen){
		return;
	}

	// 将数据存入缓冲区
	*(pUartRxCB->pbRxBuffer + pUartRxCB->wRxDataLen) =  bData; 
	pUartRxCB->wRxDataLen ++;
	pUartRxCB->bRxStatus = cUartRxRdy;
	pUartRxCB->dwRxLastTime = getTime();

	return;
}


/*****************************************************************************
 Function : UART_DRV_SendData
 Brief    : Send data to Tx of the UART from buffer
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5
 Output   : null
 Return   : [BOOLEAN]True for success, False for failure 
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
BOOLEAN UART_DRV_SendData(void)
{
	INT16U          wIndex      = 0;
	UART_TX_CB      *pUartTxCB  = NULL;

	// 获取发送控制块
	pUartTxCB = &UartTxCbTbl;

	// 判断数据的合法性
	if(pUartTxCB->wTxDataLen == 0 || pUartTxCB->wTxDataLen > cMaxUartDataLen){
		UART_DRV_TxInit(); 
		return FALSE;
	}

	// 当发送控制块状态繁忙时，拒绝处理发送任务
	if(pUartTxCB->bTxStatus == cUartTxBusy){
		return FALSE;
	}

	// 置起繁忙标识
	pUartTxCB->bTxStatus = cUartTxBusy;

	// 发送缓冲区的内容 
	for(wIndex=0; wIndex < pUartTxCB->wTxDataLen;){
		// 等待发送寄存器为空时才发送
		if(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == SET){		
			USART_SendData(USART1,*(pUartTxCB->pbTxBuffer + wIndex));
			wIndex ++;
		}
	} 

	// 复位发送控制块
	while(1){
		// 等待最后一个字节发送完成
		if(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == SET){		
			UART_DRV_TxInit(); 
			break;
		}
	}

	return TRUE;
}

/******************************************************************************
 End of the File (EOF). Do not put anything after this part!
******************************************************************************/

