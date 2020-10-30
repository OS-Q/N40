/**********************    COPYRIGHT 2014-2015, EYBOND    ************************ 
  * @File    : xmodem_client.h 
  * @Author  : R.Y. 
  * @Date    : 2016-05-12
  * @Brief   : Header for xmodem.c module
**********************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef XMODEM_H
#define XMODEM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define XMODEM_NO_EXIT		(0)
#define XMODEM_EXIT			(1)
#define XMODEM_TIMEOUT		(5000000)		// 5 sec

// 传输过程的校验模式
#define XMODEM_CHECK_CRC    1    //  CRC校验
#define XMODEM_CHECK_SUM    0    //  累加校验

#define SOH  0x01
#define STX  0x02
#define ETX  0x03
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define DLY_1S 1000
#define MAXRETRANS 10//25

// 定义演示版相关代码开关，该部分代码与xmodem协议无关
#define DEMO_ENABLE         1
/* Exported variables --------------------------------------------------------*/ 

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */ 
extern int8_t xmodem_1K_client(void);

#ifdef __cplusplus
    }
#endif

#endif 

/******************************************************************************
 End of the File (EOF). Do not put anything after this part!
******************************************************************************/

