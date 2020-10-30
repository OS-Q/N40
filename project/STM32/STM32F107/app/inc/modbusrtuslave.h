/**********************    COPYRIGHT 2014-2015, EYBOND    ************************
  * @File    : modbusrtu.h 
  * @Author  : Mencius 
  * @Date    : 2015-09-02
  * @Brief   : Header for modbusrtu.c module
**********************************************************************************/
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MODBUSRTU_H
#define MODBUSRTU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported types ------------------------------------------------------------*/
typedef __packed enum {
	emIDLE = 0,
	emREAD = 1,
	emWRITE = 2
}OPT_STATUS;
typedef __packed struct{               // 寄存器映射表控制块 数据类型
	INT8U       *pbData;               // 数据存储区 
	OPT_STATUS	emStatus;               // 数据操作状态
}RTU_REGISTER_MAP_CB;

typedef __packed struct{               // 逆变器参数控制块 数据类型
	INT8U       bComLostStatus;         // 与DSP之间的通讯是否失败
	INT8U       bComAddr;               // 通讯地址
}RTU_INVERTER_PARA_CB;

/* Exported constants --------------------------------------------------------*/  

/* Exported variables --------------------------------------------------------*/
extern RTU_INVERTER_PARA_CB stInvPara;      // 逆变器参数(主要用于LCD显示及流程控制)

/* Exported macro ------------------------------------------------------------*/ 
#define 	cMinRespLen			    5       // Modbus响应帧的最小长度
#define 	cMinReqLen			    8       // Modbus请求帧的最小长度

#define 	cMax03ReqLen			8       // Modbus对03命令请求帧的最大长度 
#define 	cMax04ReqLen			8       // Modbus对04命令请求帧的最大长度 
#define 	cMax10ReqLen			13      // Modbus对10命令请求帧的最大长度 


#define 	cMaxOutputMapLen        400//2000    // 输出寄存器映射表的长度
#define 	cMaxInputMapLen         240//2000    // 输入寄存器映射表的长度

#define 	cHexCmd03 			    0x03    // Modbus 0x03功能码
#define 	cHexCmd04 			    0x04    // Modbus 0x04功能码
#define 	cHexCmd06 			    0x06    // Modbus 0x06功能码
#define 	cHexCmd10 			    0x10    // Modbus 0x10功能码
#define 	cHexCmd15 			    0x15    // Modbus 0x15功能码

#define 	cErrCode01 			    0x01    // Modbus 异常码: 非法功能码
#define 	cErrCode02 			    0x02    // Modbus 异常码: 非法数据地址
#define 	cErrCode03 			    0x03    // Modbus 异常码: 非法数据值
#define 	cErrCode04 			    0x04    // Modbus 异常码: 读写错误
#define 	cErrCode06 			    0x06    // Modbus 异常码: 从站设备忙 
#define     cErrCode07              0x07    // Modbus 异常码: 固件类型不支持
#define     cErrCode08              0x08    // Modbus 异常码: 波特率不支持

/* Exported functions ------------------------------------------------------- */
extern void     RTU_Initial(void);
extern BOOLEAN  RTU_RS232ParseRequest(void);
extern INT16U   RTU_CalCRC16(INT8U *pucFrame, INT16U usDataLen);
extern BOOLEAN  RTU_ParseReq03Cmd(void);
extern BOOLEAN  RTU_ParseReq04Cmd(void);
extern void     RTU_SendInvalidResp(INT8U ucFuncCode,INT8U ucErrCode);

#ifdef __cplusplus
    }
#endif

#endif


/******************************************************************************
 End of the File (EOF). Do not put anything after this part!
******************************************************************************/

