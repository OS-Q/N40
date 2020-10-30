/**********************    COPYRIGHT 2014-2015, EYBOND    ************************
  * @File    : xmodem_client.c 
  * @Author  : R.Y. 
  * @Date    : 2016-05-17
  * @Brief   : The public types,constants,macros and functions for xmodem recv
               This code needs standard functions memcpy() and memset()
               and input/output functions xmodem_inbyte() and xmodem_outbyte().
               ʹ��˵��:
               1��xmodem_lowLevelRecvByte�ǵײ���պ�������ƽ̨��أ���Ҫ�����޸ġ�
               2��xmodem_lowlevelSendByte�ǵײ㷢�ͺ�������ƽ̨��أ���Ҫ�����޸ġ�
               3����Э���ǻ���XMODEM-1KЭ�飬�������ݰ�������뱣֤����1030���ֽڡ�
               4��xmodem_1K_recv()��buf����ں�����Ϊ��ʾ�õĻ��棬���С��Ҫ����
                  ʵ�ʻ������ġ�
**********************************************************************************/
#include <stdint.h>
#include "main.h"
#include "xmodem_client.h"

/* Private macro ------------------------------------------------------------*/
/* Private types ------------------------------------------------------------*/
/* Private constants --------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
// ����Xmodem��ǰ����ģʽ
static uint8_t bXmodemExit=XMODEM_NO_EXIT;

// ����Xmodem���棬1024 for XModem 1k + 3 head chars + 2 crc + nul
static uint8_t bXmodemBuf[1030];

/* Private functions ------------------------------------------------------- */

/*****************************************************************************
 Function : xmodem_lowLevelRecvByte
 Brief    : Xmodem�ײ�������ݺ�������ֲʱ���޸ĸú�����ʹ֮��ƽ̨�����
 Input    : null
 Output   : [uint8_t* bData]: �������ݵ�ַ
 Return   : [uint8_t]       : ���ؽ��ս����TRUE�ǽ�����ȷ��FALSE�ǽ���ʧ��
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
 Brief    : Xmodem�ײ㷢�����ݺ�������ֲʱ���޸ĸú�����ʹ֮��ƽ̨�����
 Input    : [uint8_t bData]: ��������
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
 Brief    : xmodem�����ֽڣ���ֲ�����޸�
 Input    : [uint16_t wMs]: δʹ��
 Output   : [uint8_t* bData]: �������ݵ�ַ
 Return   : [uint8_t]       : ���ؽ��ս����TRUE�ǽ�����ȷ��FALSE�ǽ���ʧ��
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
 Brief    : xmodem����ֽڣ���ֲ�����޸�
 Input    : [uint8_t bData]: ����ַ�
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
 Brief    : crc16_ccittУ������㷨
 Input    : [const uint8_t *pbBuf]: ��У�������׵�ַ
            [uint32_t dwLen]      : ��У�����ݳ���
 Output   : null
 Return   : [uint16_t]: У����
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
 Brief    : xmodemУ�飬����ѡ���У�鷽ʽ���У����ۼ�У���CRCУ��
 Input    : [uint8_t bCheckMode]  : У�鷽ʽ
            [const uint8_t *pbBuf]: У�������׵�ַ
            [uint32_t dwLen]      : У�����ݳ���
 Output   : null
 Return   : [uint8_t]: ����У������TRUE�ǳɹ���FALSE��ʧ��
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
 Brief    : xmodem����������
 Input    : [uint8_t *pbBuf]    : ���ջ����׵�ַ
            [uint32_t dwBufSize]: ���ջ����С
 Output   : null
 Return   : [int8_t]: ���ս����0�ǽ��������������ǽ���ʧ�ܡ�
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
int8_t xmodemReceive(uint8_t *pbBuf, uint32_t dwBufSize)
{
	uint8_t *pbData;                  // ���ջ���ָ��
	uint32_t dwPktDataLen;            // ��Ϣ֡������������
	uint8_t  bXmodemCheckMode = 0;    // xmodemУ�鷽ʽ:0���ۼӣ�1��CRC
	uint8_t  bTryChar = 'C';          // �����������ڵķ����ַ�
	uint8_t  bPktNum = 1;             // ���հ����
	uint32_t dwCycle;                 // 
	uint32_t dwOffset = 0;            // ����ջ����׵�ַ��Ե�ƫ����
	uint8_t  bData;                   // ���յ�������
	uint8_t  bRetry;                  // ����ʱ���ظ�����
	uint8_t  bRetrans = MAXRETRANS;   // ����ظ����մ���
	
	
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
 Brief    : xmodem 1kЭ��������
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
	// | ��ʼ���� |    on     |    off    |    --     |    --     |    --     |    --     |
	// | ����ɹ� |    off    |    on     |    --     |    --     |    --     |    --     |
	// | ����ʧ�� |    off    |    off    |    --     |    --     |    --     |    --     |
	// | ���ȴ�   |    on     |    off    |    xor    |    off    |    --     |    --     |
	// | ���ɹ�   |    on     |    off    |    off    |    on     |    --     |    xor    |
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

