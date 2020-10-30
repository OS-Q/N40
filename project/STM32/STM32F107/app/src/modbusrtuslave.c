/**********************    COPYRIGHT 2014-2015, EYBOND    ************************
  * @File    : modbusrtu.c 
  * @Author  : R.Y.
  * @Date    : 2016-05-117
  * @Brief   : Modbus RTU Protocol
**********************************************************************************/
#include "modbusrtuslave.h"
#include "uartdriver.h"

/* Private types ------------------------------------------------------------*/
/* Private constants --------------------------------------------------------*/
// CRC16 高位字节值表
const INT8U rtu_aucCRCHi[256] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40
};

// CRC16 低位字节值表
const INT8U rtu_aucCRCLo[256] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
	0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
	0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
	0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
	0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
	0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
	0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
	0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
	0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
	0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
	0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 
	0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
	0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
	0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
	0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
	0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
	0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
	0x41, 0x81, 0x80, 0x40
};

/* Private variables --------------------------------------------------------*/
RTU_REGISTER_MAP_CB ORegMapCb = {0};                    // 输出寄存器映射表控制块
RTU_REGISTER_MAP_CB IRegMapCb = {0};                    // 输入寄存器映射表控制块

INT8U bOutputRegMap[cMaxOutputMapLen]   = {0};          // 输出寄存器映射表
INT8U bInputRegMap[cMaxInputMapLen]     = {0};          // 输入寄存器映射表

RTU_INVERTER_PARA_CB stInvPara;      // 逆变器参数(主要用于LCD显示及流程控制)
INT8U cInAddr = 0;
/* Private macro ------------------------------------------------------------*/


/* Private functions ------------------------------------------------------- */
INT16U  RTU_CalCRC16(INT8U *pucFrame, INT16U usDataLen); 
BOOLEAN RTU_ParseReq03Cmd(void);
BOOLEAN RTU_ParseReq04Cmd(void);
BOOLEAN RTU_ParseReq10Cmd(void);
void    RTU_SendInvalidResp(INT8U ucFuncCode,INT8U ucErrCode);

/*****************************************************************************
 Function : RTU_Initial
 Brief    : RTU initial
 Input    : void
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void RTU_Initial(void)
{
	RTU_REGISTER_MAP_CB *pRegMapCB  = NULL;
	INT16U wCycle;

	// 初始化输出寄存器映射表控制块 
	memClr((INT8U *)bOutputRegMap,sizeof(bOutputRegMap)); 

	pRegMapCB = &ORegMapCb;
	pRegMapCB->pbData = bOutputRegMap;
	pRegMapCB->emStatus = emIDLE;

	// 初始化输入寄存器映射表控制块 
	memClr((INT8U *)bInputRegMap,sizeof(bInputRegMap)); 

	pRegMapCB = &IRegMapCb;
	pRegMapCB->pbData = bInputRegMap;
	pRegMapCB->emStatus = emIDLE;

	stInvPara.bComAddr = 0x01;
	cInAddr = stInvPara.bComAddr;

	// 产生模拟值
	for(wCycle=0; wCycle<sizeof(bOutputRegMap); wCycle++){
		bOutputRegMap[wCycle] = wCycle;
	}
	for(wCycle=0; wCycle<sizeof(bInputRegMap); wCycle++){
		bInputRegMap[wCycle] = wCycle;
	}
}

/*****************************************************************************
 Function : RTU_CalCRC16
 Brief    : To calculate the 16 bits CRC value of the modbus protocol 
 Input    : [pucFrame]The pointer of the data frame
            [usDataLen] valid data length
 Output   : null
 Return   : INT16U
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
INT16U RTU_CalCRC16(INT8U *pucFrame, INT16U usDataLen) 
{
	INT8U   ucCRCHi = 0xFF;     // 高CRC字节初始化
	INT8U   ucCRCLo = 0xFF;     // 低CRC字节初始化
	INT16U  wIndex  = 0;        // CRC循环中的索引

	while(usDataLen--){
		wIndex = ucCRCLo ^ *(pucFrame++);
		ucCRCLo = (INT8U)(ucCRCHi ^ rtu_aucCRCHi[wIndex]);
		ucCRCHi = rtu_aucCRCLo[wIndex];
	}

	return (INT16U)(ucCRCHi << 8 | ucCRCLo);
}

/*****************************************************************************
 Function : RTU_RS232ParseRequest
 Brief    : Parse the modbus request command via RS232
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5 
 Output   : null
 Return   : BOOLEAN
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
BOOLEAN RTU_RS232ParseRequest(void)
{
	INT8U           ucFuncCode      = 0;
	INT8U           *pucDataBuf     = NULL;
	UART_RX_CB      *pUartRxCB      = NULL; 
	INT16U          wCRCTemp        = 0;
	BOOLEAN         bResult         = FALSE;

	// 获取接收控制块和接收缓冲区
	pUartRxCB = &UartRxCbTbl;
	pucDataBuf = pUartRxCB->pbRxBuffer;

	// 判断数据长度
	if(pUartRxCB->wRxDataLen < cMinReqLen || pUartRxCB->wRxDataLen > cMaxUartDataLen){
		// 初始化接收控制块，将数据丢弃
		UART_DRV_RxInit();
		return FALSE;
	}

	// 计算校验码，并判断校验码的合法性
	wCRCTemp = RTU_CalCRC16(pucDataBuf,pUartRxCB->wRxDataLen - 2); 
	if(((wCRCTemp&0xFF) != *(pucDataBuf + pUartRxCB->wRxDataLen - 2))
		|| ((wCRCTemp>>8) != *(pucDataBuf + pUartRxCB->wRxDataLen - 1))){
		// 初始化接收控制块，将数据丢弃
		UART_DRV_RxInit();
		return FALSE;
	}

	// 获取功能码
	ucFuncCode = *(pucDataBuf + 1);

	// 根据功能码分别进行解析  
	switch(ucFuncCode){
		case cHexCmd03: 
			bResult = RTU_ParseReq03Cmd();
			break;
		case cHexCmd04:
		    bResult = RTU_ParseReq04Cmd();
		    break;
		case cHexCmd10:
			bResult = RTU_ParseReq10Cmd();
			break;
		default: 
		    bResult = FALSE;
		    RTU_SendInvalidResp(ucFuncCode,cErrCode01);
		    break;
	}

	// 初始化接收控制块, 接收新数据
	UART_DRV_RxInit(); 

	return bResult;
}

/*****************************************************************************
 Function : RTU_SendInvalidResp
 Brief    : Send invalid response
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5
            [ucFuncCode]Function code
            [ucErrCode]Error code
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void RTU_SendInvalidResp(INT8U ucFuncCode,INT8U ucErrCode)
{
	INT8U           *pucDataBuf = NULL;
	UART_TX_CB      *pUartTxCB  = NULL;
	INT16U          wIndex      = 0;
	INT16U          wCRCTemp    = 0;

	// 获取发送控制块和发送缓冲区
	pUartTxCB = &UartTxCbTbl;
	pucDataBuf = pUartTxCB->pbTxBuffer;

	// 填充发送缓冲区
	*(pucDataBuf + wIndex++) = stInvPara.bComAddr;  // 从机地址(1字节) 

	*(pucDataBuf + wIndex++) = (ucFuncCode|0x80);   // 差错码(1字节) 

	*(pucDataBuf + wIndex++) = ucErrCode;           // 异常码(1字节)  
	 
	wCRCTemp = RTU_CalCRC16(pucDataBuf,wIndex);     // CRC(2字节，低字节在前，高字节在后)
	*(pucDataBuf + wIndex++) = (INT8U)(wCRCTemp);   
	*(pucDataBuf + wIndex++) = (INT8U)(wCRCTemp>>8);

	// 更新发送控制块中的数据长度
	pUartTxCB->wTxDataLen = wIndex; 

	// 发送数据
	UART_DRV_SendData();
}

/*****************************************************************************
 Function : RTU_ParseReq03Cmd
 Brief    : Parse the modbus request from 0x03 command 
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5 
 Output   : null
 Return   : BOOLEAN
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
BOOLEAN RTU_ParseReq03Cmd(void)
{ 
	INT8U           *pucRxDataBuf   = NULL;
	INT8U           *pucTxDataBuf   = NULL;
	INT16U          wStartRegNO     = 0;
	INT16U          wRegCount       = 0;
	INT16U          wCRCTemp        = 0;
	INT16U          wIndex          = 0;
	UART_RX_CB      *pUartRxCB      = NULL; 
	UART_TX_CB      *pUartTxCB      = NULL;

	// 获取接收控制块和接收缓冲区
	pUartRxCB = &UartRxCbTbl;
	pucRxDataBuf = pUartRxCB->pbRxBuffer;

	// 获取发送控制块和发送缓冲区
	pUartTxCB = &UartTxCbTbl;
	pucTxDataBuf = pUartTxCB->pbTxBuffer; 

	// 判断数据长度
	if(pUartRxCB->wRxDataLen != cMax03ReqLen){
		return FALSE;
	}

	// 判断通讯地址 
	if(*pucRxDataBuf != stInvPara.bComAddr && *pucRxDataBuf != cInAddr){
		return FALSE;
	}

	// 获取起始寄存器的地址
	wStartRegNO = (INT16U)(*(pucRxDataBuf + 2) << 8 | *(pucRxDataBuf + 3));

	// 获取寄存器数量
	wRegCount = (INT16U)(*(pucRxDataBuf + 4) << 8 | *(pucRxDataBuf + 5));

	// 判断寄存器数量的合法性
	if(wRegCount > 0x00c8 || wRegCount == 0){
		RTU_SendInvalidResp(cHexCmd03,cErrCode03);
		return FALSE;
	}

	// 判断起始寄存器地址与寄存器数量组合后的合法性
	if((wStartRegNO + wRegCount)*2 > cMaxOutputMapLen){
		RTU_SendInvalidResp(cHexCmd03,cErrCode02);
		return FALSE;
	}

	// 若存储区处于写状态，则回应系统繁忙
	if(ORegMapCb.emStatus == emWRITE){
		RTU_SendInvalidResp(cHexCmd03,cErrCode06);
		return FALSE;
	} 

	// 若与DSP之间的通讯失败，则回应系统繁忙
	if(stInvPara.bComLostStatus == TRUE){
		RTU_SendInvalidResp(cHexCmd03,cErrCode06);
		return FALSE;
	} 

	// 填充发送缓冲区(地址、功能码、字节长度)
	*(pucTxDataBuf + wIndex++) = stInvPara.bComAddr;    // 从机地址(1字节，使用本机实际地址) 

	*(pucTxDataBuf + wIndex++) = cHexCmd03;             // 功能码(1字节) 

	*(pucTxDataBuf + wIndex++) = wRegCount*2;           // 字节数(1字节)  

	// 填充发送缓冲区(寄存器值，2*wRegCount字节) 
	ORegMapCb.emStatus = emREAD;

	memCpyAll((INT8U *)(pucTxDataBuf + wIndex), (INT8U *)(ORegMapCb.pbData + wStartRegNO*2), wRegCount*2); 

	ORegMapCb.emStatus = emIDLE; 

	wIndex+= wRegCount*2;

	// 填充发送缓冲区(CRC)
	wCRCTemp = RTU_CalCRC16(pucTxDataBuf,wIndex);   // CRC(2字节，低字节在前，高字节在后)
	*(pucTxDataBuf + wIndex++) = (INT8U)(wCRCTemp);   
	*(pucTxDataBuf + wIndex++) = (INT8U)(wCRCTemp>>8);

	// 更新发送控制块中的数据长度
	pUartTxCB->wTxDataLen = wIndex; 

	// 发送数据
	UART_DRV_SendData();

	return TRUE;
}

/*****************************************************************************
 Function : RTU_ParseReq04Cmd
 Brief    : Parse the modbus request from 0x04 command 
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5 
 Output   : null
 Return   : BOOLEAN
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
BOOLEAN RTU_ParseReq04Cmd(void)
{ 
	INT8U           *pucRxDataBuf   = NULL;
	INT8U           *pucTxDataBuf   = NULL;
	INT16U          wStartRegNO     = 0;
	INT16U          wRegCount       = 0;
	INT16U          wCRCTemp        = 0;
	INT16U          wIndex          = 0;
	UART_RX_CB      *pUartRxCB      = NULL; 
	UART_TX_CB      *pUartTxCB      = NULL;

	// 获取接收控制块和接收缓冲区
	pUartRxCB = &UartRxCbTbl;
	pucRxDataBuf = pUartRxCB->pbRxBuffer;

	// 获取发送控制块和发送缓冲区
	pUartTxCB = &UartTxCbTbl;
	pucTxDataBuf = pUartTxCB->pbTxBuffer; 

	// 判断数据长度
	if(pUartRxCB->wRxDataLen != cMax04ReqLen){
		return FALSE;
	}

	// 判断通讯地址
	if(*pucRxDataBuf != stInvPara.bComAddr && *pucRxDataBuf != cInAddr){
		return FALSE;
	}

	// 获取起始寄存器的地址
	wStartRegNO = (INT16U)(*(pucRxDataBuf + 2) << 8 | *(pucRxDataBuf + 3));

	// 获取寄存器数量
	wRegCount = (INT16U)(*(pucRxDataBuf + 4) << 8 | *(pucRxDataBuf + 5));

	// 判断寄存器数量的合法性
	if(wRegCount > 0x007D || wRegCount == 0){
		RTU_SendInvalidResp(cHexCmd04,cErrCode03);
		return FALSE;
	}

	// 判断起始寄存器地址与寄存器数量组合后的合法性
	if((wStartRegNO + wRegCount)*2 >= cMaxOutputMapLen){
		RTU_SendInvalidResp(cHexCmd04,cErrCode02);
		return FALSE;
	}

	// 若存储区处于写状态，则回应系统繁忙
	if(IRegMapCb.emStatus == emWRITE){
		RTU_SendInvalidResp(cHexCmd04,cErrCode06);
		return FALSE;
	} 

	// 若与DSP之间的通讯失败，则回应系统繁忙
	if(stInvPara.bComLostStatus == TRUE){
		RTU_SendInvalidResp(cHexCmd04,cErrCode06);
		return FALSE;
	} 

	// 填充发送缓冲区(地址、功能码、字节长度)
	*(pucTxDataBuf + wIndex++) = stInvPara.bComAddr;    // 从机地址(1字节，使用本机实际地址) 

	*(pucTxDataBuf + wIndex++) = cHexCmd04;             // 功能码(1字节) 

	*(pucTxDataBuf + wIndex++) = wRegCount*2;           // 字节数(1字节)  

	// 填充发送缓冲区(寄存器值，2*wRegCount字节)  
	IRegMapCb.emStatus = emREAD;

	memCpyAll((INT8U *)(pucTxDataBuf + wIndex), (INT8U *)(IRegMapCb.pbData + wStartRegNO*2), wRegCount*2); 

	IRegMapCb.emStatus = emIDLE; 

	wIndex+= wRegCount*2;

	// 填充发送缓冲区(CRC)
	wCRCTemp = RTU_CalCRC16(pucTxDataBuf,wIndex);   // CRC(2字节，低字节在前，高字节在后)
	*(pucTxDataBuf + wIndex++) = (INT8U)(wCRCTemp);   
	*(pucTxDataBuf + wIndex++) = (INT8U)(wCRCTemp>>8);

	// 更新发送控制块中的数据长度
	pUartTxCB->wTxDataLen = wIndex; 

	// 发送数据
	UART_DRV_SendData();

	return TRUE; 
}

/*****************************************************************************
 Function : RTU_ParseReq10Cmd
 Brief    : Parse the modbus request from 0x04 command 
 Input    : [USARTx] USART1,USART2,USART3,UART4 or UART5 
 Output   : null
 Return   : BOOLEAN
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
BOOLEAN RTU_ParseReq10Cmd(void)
{ 
	INT8U           *pucRxDataBuf   = NULL;
	INT8U           *pucTxDataBuf   = NULL;
	INT16U          wStartRegNO     = 0;
	INT16U          wRegCount       = 0;
	INT16U          wCRCTemp        = 0;
	INT16U          wIndex          = 0;
	INT16U          wBaud           = 0;
	UART_RX_CB      *pUartRxCB      = NULL; 
	UART_TX_CB      *pUartTxCB      = NULL;

    // 获取接收控制块和接收缓冲区
    pUartRxCB = &UartRxCbTbl;
    pucRxDataBuf = pUartRxCB->pbRxBuffer;

	// 获取发送控制块和发送缓冲区
	pUartTxCB = &UartTxCbTbl;
	pucTxDataBuf = pUartTxCB->pbTxBuffer; 

    // 判断数据长度
    if(pUartRxCB->wRxDataLen != cMax10ReqLen)
    {
        return FALSE;
    }

    // 判断通讯地址
    if(*pucRxDataBuf != stInvPara.bComAddr && *pucRxDataBuf != cInAddr)
    {
        return FALSE;
    }

    // 获取起始寄存器的地址
    wStartRegNO = (INT16U)(*(pucRxDataBuf + 2) << 8 | *(pucRxDataBuf + 3));

    // 获取寄存器数量
    wRegCount = (INT16U)(*(pucRxDataBuf + 4) << 8 | *(pucRxDataBuf + 5));
 
    // 判断寄存器数量的合法性
    if(wRegCount != 0x0002)
    {
        RTU_SendInvalidResp(cHexCmd10,cErrCode03);
        return FALSE;
    }

    // 判断起始寄存器地址与寄存器数量组合后的合法性
    if(wStartRegNO != 0x0438)
    {
        RTU_SendInvalidResp(cHexCmd10,cErrCode02);
        return FALSE;
    }

    // 若存储区处于写状态，则回应系统繁忙
    if(IRegMapCb.emStatus == emWRITE)
    {
        RTU_SendInvalidResp(cHexCmd10,cErrCode06);
        return FALSE;
    } 

    // 若与DSP之间的通讯失败，则回应系统繁忙
    if(stInvPara.bComLostStatus == TRUE)
    {
        RTU_SendInvalidResp(cHexCmd10,cErrCode06);
        return FALSE;
    }

	// 判断固件类型
	if((INT16U)(*(pucRxDataBuf + 7) << 8 | *(pucRxDataBuf + 8))!=0x0000 &&\
		(INT16U)(*(pucRxDataBuf + 7) << 8 | *(pucRxDataBuf + 8))!=0x0001 &&\
		(INT16U)(*(pucRxDataBuf + 7) << 8 | *(pucRxDataBuf + 8))!=0x0002){
		RTU_SendInvalidResp(cHexCmd10,cErrCode07);
        return FALSE;
	}

	// 判断波特率
	wBaud = (INT16U)(*(pucRxDataBuf + 9) << 8 | *(pucRxDataBuf + 10));
	if(wBaud>2560){
		RTU_SendInvalidResp(cHexCmd10,cErrCode08);
        return FALSE;
	}

	// 做相应初始化，然后切换到Boot
	bBoot = TRUE;
	wSwType = (INT16U)(*(pucRxDataBuf + 7) << 8 | *(pucRxDataBuf + 8));
	wSerialBaud = wBaud;

	// 填充发送缓冲区(地址、功能码、字节长度)
	*(pucTxDataBuf + wIndex++) = stInvPara.bComAddr;    // 从机地址(1字节，使用本机实际地址) 
	*(pucTxDataBuf + wIndex++) = cHexCmd10;             // 功能码(1字节) 
	*(pucTxDataBuf + wIndex++) = *(pucRxDataBuf + 2);   // 起始寄存器高地址(1字节)  
	*(pucTxDataBuf + wIndex++) = *(pucRxDataBuf + 3);   // 起始寄存器低地址(1字节)  
	*(pucTxDataBuf + wIndex++) = *(pucRxDataBuf + 4);   // 寄存器数量高地址(1字节)  
	*(pucTxDataBuf + wIndex++) = *(pucRxDataBuf + 5);   // 寄存器数量低地址(1字节)  

	// 填充发送缓冲区(CRC)
	wCRCTemp = RTU_CalCRC16(pucTxDataBuf,wIndex);   // CRC(2字节，低字节在前，高字节在后)
	*(pucTxDataBuf + wIndex++) = (INT8U)(wCRCTemp);   
	*(pucTxDataBuf + wIndex++) = (INT8U)(wCRCTemp>>8);

	// 更新发送控制块中的数据长度
	pUartTxCB->wTxDataLen = wIndex; 

	pUartTxCB->bTxStatus  = cUartTxRdy;

	// 发送数据
	UART_DRV_SendData();
    
    return TRUE; 
}

/******************************************************************************
 End of the File (EOF). Do not put anything after this part!
******************************************************************************/
