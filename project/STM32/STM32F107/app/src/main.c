/**********************    COPYRIGHT 2014-2015, EYBOND    ************************ 
  * @File    : main.c 
  * @Author  : R.Y.
  * @Date    : 2016-05-17
  * @Brief   : The entery of the system
**********************************************************************************/
#include "main.h"
#include "xmodem_client.h"
#include "uartdriver.h"
#include "modbusrtuslave.h"

/* Private types ------------------------------------------------------------*/
/* Private constants --------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/ 
INT32U dwCurrentTime = 0;
INT8U  bBoot = FALSE;
INT16U wSwType = 0;
INT16U wSerialBaud   = 0xffff;
INT8U  bReboot = FALSE;
/* Private functions ------------------------------------------------------- */
/*****************************************************************************
 Function : memCpy
 Brief    : copy data.
 Input    : void
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void memCpy(INT8U* pDest, INT8U* pSrc, INT16U wSize)
{
	while(wSize--){
		if(*pSrc == 0){
			break;
		}
		*pDest++ = *pSrc++;
	};
}
/*****************************************************************************
 Function : memCpyAll
 Brief    : copy data.
 Input    : void
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void memCpyAll(INT8U* pDest, INT8U* pSrc, INT16U wSize)
{
	while(wSize--){
		*pDest++ = *pSrc++;
	};
}

/*****************************************************************************
 Function : memClr
 Brief    : clear data.
 Input    : void
 Output   : null
 Return   : void
 Author   : R.Y.
 Date     : 2016-05-17
*****************************************************************************/
void memClr(INT8U* pDest, INT16U wSize)
{
	while(wSize--){
		*pDest++ = 0;
	};
}

INT32U getTime(void)
{
	return dwCurrentTime;
}

void sysInit(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;

	// 系统时钟初始化(SysTick: 10ms)
	SysTick_Config(SystemCoreClock/100);

	// NVIC初始化
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
	// 设置优先级配置的模式
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	// 使能RTC中断，并设置优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// 串口初始化
	UART_DRV_Config(9600);

	// LED初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13);
}

int main(void)
{
	static UART_RX_CB *pUartRxCB = NULL;
	static INT16U     wBaud      = 9600;
	INT8S  bErr = 0;

	// 系统硬环境初始化
	sysInit();

	// 系统软环境初始化
	UART_DRV_Initial();
	RTU_Initial();

	// 任务主体
	pUartRxCB = &UartRxCbTbl;
	while(1){
		// MODBUS RTU处理
		if(!bBoot){
			if((getTime()-pUartRxCB->dwRxLastTime)>5 &&\
				(pUartRxCB->bRxStatus != cUartRxBufEmpty)){
				RTU_RS232ParseRequest();
				
				if(bBoot){
					INT16U wCycle;
					for(wCycle=0; wCycle<10000; wCycle++);
					if(wSerialBaud != 0x0000){
						UART_DRV_Config_Boot(wSerialBaud*100);
						UART_DRV_Initial();
					}else{
						UART_DRV_Config_Boot(wBaud);
						UART_DRV_Initial();
					}
				}
			}
		}
		// xmodem处理
		if(bBoot){
			bErr = xmodem_1K_client();
			if(bErr==-2 || bErr==0 || bErr==-4){
				bBoot = FALSE;
				UART_DRV_Config_NoBoot(wBaud);
				UART_DRV_Initial();
			}
		}
	}
}
/******************************************************************************
 End of the File (EOF). Do not put anything after this part!
******************************************************************************/
