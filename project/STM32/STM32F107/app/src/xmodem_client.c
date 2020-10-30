/**********************    COPYRIGHT 2014-2015, EYBOND    ************************
  * @File    : xmodem_client.c 
  * @Author  : R.Y. 
  * @Date    : 2016-05-17
  * @Brief   : The public types,constants,macros and functions for xmodem recv
               This code needs standard functions memcpy() and memset()
               and input/output functions xmodem_inbyte() and xmodem_outbyte().
               使用说明:
               1、xmodem_lowLevelRecvByte是底层接收函数，与平台相关，需要按需修改。
               2、xmodem_lowlevelSendByte是底层发送函数，与平台相关，需要按需修改。
               3、该协议是基于XMODEM-1K协议，单次数据包缓存必须保证大于1030个字节。
               4、xmodem_1K_recv()的buf是入口函数作为演示用的缓存，其大小需要根据
                  实际环境更改。
**********************************************************************************/
#include <stdint.h>
#include "main.h"
#include "xmodem_client.h"

/* Private macro ------------------------------------------------------------*/
/* Private types ------------------------------------------------------------*/
/* Private constants --------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
// 定义Xmodem当前工作模式
static uint8_t bXmodemExit=XMODEM_NO_EXIT;

// 定义Xmodem缓存，1024 for XModem 1k + 3 head chars + 2 crc + nul
static uint8_t bXmodemBuf[1030];

/* Private functions ------------------------------------------------------- */

/*****************************************************************************
 Function : xmodem_lowLevelRecvByte
 Brief    : Xmodem底层接收数据函数，移植时需修改该函数，使之与平台相关联
 Input    : null
 Output   : [uint8_t* bData]: 接收数据地址
 Return   : [uint8_t]       : 返回接收结果，TRUE是接收正确，FALSE是接收失败
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
static uint8_t xmodem_lowLevelRecvByte(uint8_t* pbData)
{
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)==SET){
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
		*pbData= USART_ReceiveData(USART1);
		return TRUE;
	}
	return FALSE;
}

/*****************************************************************************
 Function : xmodem_lowlevelSendByte
 Brief    : Xmodem底层发送数据函数，移植时需修改该函数，使之与平台相关联
 Input    : [uint8_t bData]: 发送数据
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
static void xmodem_lowlevelSendByte(uint8_t bData)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)!=SET);
	USART_SendData(USART1, bData);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)!=SET);
}

/*****************************************************************************
 Function : xmodem_inbyte
 Brief    : xmodem输入字节，移植无需修改
 Input    : [uint16_t wMs]: 未使用
 Output   : [uint8_t* bData]: 接收数据地址
 Return   : [uint8_t]       : 返回接收结果，TRUE是接收正确，FALSE是接收失败
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
static uint8_t xmodem_inbyte(uint16_t wMs, uint8_t* pbData)
{
	uint32_t dwCycle=1;
	
	while(dwCycle>0){
		if(xmodem_lowLevelRecvByte(pbData) == TRUE){
			return TRUE;
		}
		
		if(dwCycle++>(XMODEM_TIMEOUT)){
			bXmodemExit=XMODEM_EXIT;
			break;
		}
	}
	return FALSE;
}

/*****************************************************************************
 Function : xmodem_outbyte
 Brief    : xmodem输出字节，移植无需修改
 Input    : [uint8_t bData]: 输出字符
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
static void xmodem_outbyte(uint8_t bData)
{
	xmodem_lowlevelSendByte(bData);
}

/*****************************************************************************
 Function : xmodem_crc16_ccitt
 Brief    : crc16_ccitt校验核心算法
 Input    : [const uint8_t *pbBuf]: 待校验数据首地址
            [uint32_t dwLen]      : 待校验数据长度
 Output   : null
 Return   : [uint16_t]: 校验结果
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
static uint16_t xmodem_crc16_ccitt(const uint8_t *pbBuf, uint32_t dwLen)
{
	uint16_t wCRC = 0;
	uint8_t  bCycle;

	while (dwLen-- > 0) {
		wCRC ^= (unsigned short) *pbBuf++ << 8;
		for (bCycle = 0; bCycle < 8; bCycle++){
			if (wCRC & 0x8000){
				wCRC = wCRC << 1 ^ 0x1021;
			}else{
				wCRC <<= 1;
			}
		}
	}
	return wCRC;
}

/*****************************************************************************
 Function : xmodem_check
 Brief    : xmodem校验，根据选择的校验方式进行，有累加校验和CRC校验
 Input    : [uint8_t bCheckMode]  : 校验方式
            [const uint8_t *pbBuf]: 校验数据首地址
            [uint32_t dwLen]      : 校验数据长度
 Output   : null
 Return   : [uint8_t]: 返回校验结果，TRUE是成功，FALSE是失败
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
static uint8_t xmodem_check(uint8_t bCheckMode, const uint8_t *pbBuf, uint32_t dwLen)
{
	if (bCheckMode){
		uint16_t wCRC    = xmodem_crc16_ccitt(pbBuf, dwLen);
		uint16_t wCRCRef = (pbBuf[dwLen]<<8)+pbBuf[dwLen+1];
		if (wCRC == wCRCRef){
			return TRUE;
		}
	}else{
		uint32_t dwCycle;
		uint8_t  bSum = 0;
		for (dwCycle = 0; dwCycle < dwLen; ++dwCycle){
			bSum += pbBuf[dwCycle];
		}
		if (bSum == pbBuf[dwLen]){
			return TRUE;
		}
	}

	return FALSE;
}

/*****************************************************************************
 Function : xmodem_flushinput
 Brief    : 
 Input    : null
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
static void xmodem_flushinput(void)
{
	uint8_t  bDummyByte;
	
	while (xmodem_inbyte(((DLY_1S)*3)>>1,&bDummyByte) == TRUE){
		bXmodemExit=XMODEM_EXIT;
		break;
	}
}

/*****************************************************************************
 Function : xmodemReceive
 Brief    : xmodem接收主流程
 Input    : [uint8_t *pbBuf]    : 接收缓存首地址
            [uint32_t dwBufSize]: 接收缓存大小
 Output   : null
 Return   : [int8_t]: 接收结果，0是接收正常，负数是接收失败。
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
int8_t xmodemReceive(uint8_t *pbBuf, uint32_t dwBufSize)
{
	uint8_t *pbData;                  // 接收缓存指针
	uint32_t dwPktDataLen;            // 信息帧中数据区长度
	uint8_t  bXmodemCheckMode = 0;    // xmodem校验方式:0是累加，1是CRC
	uint8_t  bTryChar = 'C';          // 传输启动环节的发送字符
	uint8_t  bPktNum = 1;             // 接收包序号
	uint32_t dwCycle;                 // 
	uint32_t dwOffset = 0;            // 与接收缓存首地址相对的偏移量
	uint8_t  bData;                   // 接收到的数据
	uint8_t  bRetry;                  // 错误超时的重复次数
	uint8_t  bRetrans = MAXRETRANS;   // 最大重复接收次数
	
	
	for(;;) {
		for( bRetry = 0; bRetry < MAXRETRANS; ++bRetry) {
		#if DEMO_ENABLE
			GPIO_SetBits(GPIOE, GPIO_Pin_11);
			if(GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_12)==Bit_SET){
				GPIO_ResetBits(GPIOE, GPIO_Pin_12);
			}else{
				GPIO_SetBits(GPIOE, GPIO_Pin_12);
			}
		#endif
			
			if (xmodem_inbyte((DLY_1S)<<1, &bData) == TRUE) {
				switch (bData) {
				case SOH:
					dwPktDataLen = 128;
					goto start_recv;
				case STX:
					dwPktDataLen = 1024;
					goto start_recv;
				case ETX:
					xmodem_flushinput();
					xmodem_outbyte(ACK);
					for(dwCycle=0; dwCycle<50000; dwCycle++);
					return -4; /* interrupt by remote */
					break;
				case EOT:
					xmodem_flushinput();
					xmodem_outbyte(ACK);
					for(dwCycle=0; dwCycle<50000; dwCycle++);
					return 0; /* normal end */
				case CAN:
					if ((xmodem_inbyte(DLY_1S, &bData) == TRUE)&&(bData==CAN)) {
						xmodem_flushinput();
						xmodem_outbyte(ACK);
						for(dwCycle=0; dwCycle<50000; dwCycle++);
						return -1; /* canceled by remote */
					}
					break;
				default:
					break;
				}
			}
			if (bTryChar) xmodem_outbyte(bTryChar);
		}
		if (bTryChar == 'C'){
			bTryChar = NAK; 
			continue;
		}
		xmodem_flushinput();
		xmodem_outbyte(CAN);
		xmodem_outbyte(CAN);
		xmodem_outbyte(CAN);
		return -2; /* sync error */

	start_recv:
		if (bTryChar == 'C') bXmodemCheckMode = 1;
		bTryChar = 0;
		pbData = bXmodemBuf;
		*pbData++ = bData;
		for (dwCycle = 0;  dwCycle < (dwPktDataLen+(bXmodemCheckMode?1:0)+3); ++dwCycle) {
			if (xmodem_inbyte(DLY_1S, &bData) == FALSE){
				goto reject;
			}
			*pbData++ = bData;
		}

		if (bXmodemBuf[1] == (unsigned char)(~bXmodemBuf[2]) && 
			(bXmodemBuf[1] == bPktNum || bXmodemBuf[1] == (unsigned char)bPktNum-1) &&
			xmodem_check(bXmodemCheckMode, &bXmodemBuf[3], dwPktDataLen)) {
		#if DEMO_ENABLE
			if(GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12)==Bit_SET){
				GPIO_ResetBits(GPIOB, GPIO_Pin_12);
			}else{
				GPIO_SetBits(GPIOB, GPIO_Pin_12);
			}
		#endif
			if (bXmodemBuf[1] == bPktNum){
				register int count = dwBufSize - dwOffset;
				if (count > dwPktDataLen){
					count = dwPktDataLen;
				}
				if (count > 0) {
					memcpy (&pbBuf[dwOffset], &bXmodemBuf[3], count);
					dwOffset += count;
				}
				++bPktNum;
				bRetrans = MAXRETRANS+1;
			}
			if (--bRetrans == 0) {
				xmodem_flushinput();
				xmodem_outbyte(CAN);
				xmodem_outbyte(CAN);
				xmodem_outbyte(CAN);
				return -3; /* too many bRetry error */
			}
			xmodem_outbyte(ACK);
			continue;
		}
	reject:
		xmodem_flushinput();
		xmodem_outbyte(NAK);
	}
}

/*****************************************************************************
 Function : xmodem_1K_recv
 Brief    : xmodem 1k协议接收入口
 Input    : null
 Output   : null
 Return   : [int8_t]
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
int8_t xmodem_1K_client(void)

{
	int8_t st;
	static uint8_t buf[50*1024];

#if DEMO_ENABLE
	// server1_r:GPIO_Pin_14
	// server1_g:GPIO_Pin_13
	// server2_r:GPIO_Pin_12
	// server2_g:GPIO_Pin_11
	// server3_r:GPIO_Pin_10
	// server3_g:GPIO_Pin_9
	// ------------------------------------------------------------------------------------
	// |          | server1_r | server1_g | server2_r | server2_g | server3_r | server3_g |
	// | 开始传输 |    on     |    off    |    --     |    --     |    --     |    --     |
	// | 传输成功 |    off    |    on     |    --     |    --     |    --     |    --     |
	// | 传输失败 |    off    |    off    |    --     |    --     |    --     |    --     |
	// | 包等待   |    on     |    off    |    xor    |    off    |    --     |    --     |
	// | 包成功   |    on     |    off    |    off    |    on     |    --     |    xor    |
	// ------------------------------------------------------------------------------------
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
#endif
	
	st = xmodemReceive((uint8_t *)buf, 50*1024);
	if (st < 0) {
	#if DEMO_ENABLE
		//printf ("Xmodem receive error: status: %d\n", st);
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	#endif
		return st;
	}
	else{
	#if DEMO_ENABLE
		//printf ("Xmodem successfully received %d bytes\n", st);
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	#endif
		bReboot = TRUE;
	}

	return 0;
}
/******************************************************************************
 End of the File (EOF). Do not put anything after this part!
******************************************************************************/

