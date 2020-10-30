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
typedef __packed struct{               // �Ĵ���ӳ�����ƿ� ��������
	INT8U       *pbData;               // ���ݴ洢�� 
	OPT_STATUS	emStatus;               // ���ݲ���״̬
}RTU_REGISTER_MAP_CB;

typedef __packed struct{               // ������������ƿ� ��������
	INT8U       bComLostStatus;         // ��DSP֮���ͨѶ�Ƿ�ʧ��
	INT8U       bComAddr;               // ͨѶ��ַ
}RTU_INVERTER_PARA_CB;

/* Exported constants --------------------------------------------------------*/  

/* Exported variables --------------------------------------------------------*/
extern RTU_INVERTER_PARA_CB stInvPara;      // ���������(��Ҫ����LCD��ʾ�����̿���)

/* Exported macro ------------------------------------------------------------*/ 
#define 	cMinRespLen			    5       // Modbus��Ӧ֡����С����
#define 	cMinReqLen			    8       // Modbus����֡����С����

#define 	cMax03ReqLen			8       // Modbus��03��������֡����󳤶� 
#define 	cMax04ReqLen			8       // Modbus��04��������֡����󳤶� 
#define 	cMax10ReqLen			13      // Modbus��10��������֡����󳤶� 


#define 	cMaxOutputMapLen        400//2000    // ����Ĵ���ӳ���ĳ���
#define 	cMaxInputMapLen         240//2000    // ����Ĵ���ӳ���ĳ���

#define 	cHexCmd03 			    0x03    // Modbus 0x03������
#define 	cHexCmd04 			    0x04    // Modbus 0x04������
#define 	cHexCmd06 			    0x06    // Modbus 0x06������
#define 	cHexCmd10 			    0x10    // Modbus 0x10������
#define 	cHexCmd15 			    0x15    // Modbus 0x15������

#define 	cErrCode01 			    0x01    // Modbus �쳣��: �Ƿ�������
#define 	cErrCode02 			    0x02    // Modbus �쳣��: �Ƿ����ݵ�ַ
#define 	cErrCode03 			    0x03    // Modbus �쳣��: �Ƿ�����ֵ
#define 	cErrCode04 			    0x04    // Modbus �쳣��: ��д����
#define 	cErrCode06 			    0x06    // Modbus �쳣��: ��վ�豸æ 
#define     cErrCode07              0x07    // Modbus �쳣��: �̼����Ͳ�֧��
#define     cErrCode08              0x08    // Modbus �쳣��: �����ʲ�֧��

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

