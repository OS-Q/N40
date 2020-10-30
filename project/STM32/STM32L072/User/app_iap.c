#include "stdbool.h"
#include "app_iap.h"
#include "usart.h"
#include "main.h"

#define PACK_SIZE 128

/**********************************************
//relocate vector in ram
//It's not necessary when mcu is stm32L071cb.

//__IO uint32_t vector_t[48] __attribute__((at(SRAM_BASE)));
__align(512) uint32_t vector_t[48];
void iap_relocate_nvic_ram(uint32_t address)
{
 uint32_t i;
 for (i = 0;i < 48;i++)
 {
   vector_t[i] = *((uint32_t*)(address + (i << 2)));
 }
 SCB->VTOR = (uint32_t)vector_t;
}
**********************************************/

typedef enum
{
	X_STATE_START,	 //xmodem start
	X_STATE_REQ_SEND,  //request send data
	X_STATE_RECV_DATA, //receiving data
	X_STATE_RECV_OK,   //received complete
	X_STATE_DATA_ACK,  //received data ack
	X_STATE_DATA_NACK, //received data nack
	X_STATE_EOT_ACK,   //received eot ack
	X_STATE_CAN,	   //xmodem cancel
	X_STATE_END,	   //xmodem end
} xmodem_state_t;

/**************XMODEM PACK STRUCT**************
|___SOH(1)___|___NUMBER(1)___|___~NUMBER(1)___|___DATA(128)___|___CRC(2)___|
****************************/
typedef enum
{
	X_PACK_STATE_IDLE,
	X_PACK_STATE_SOH,
	X_PACK_STATE_NUMBER,
	X_PACK_STATE_DATA,
	X_PACK_STATE_CRC,
} x_pack_state_t;

uint8_t xmod_state;
uint8_t xpack_state;
uint8_t pack_number;
uint8_t recv_buf[PACK_SIZE + 10]; //实际上只需要128+2
uint8_t recv_counter;
uint32_t base_add;


/**********************************
 * @biref:if not set memory protection,set RPD level=1
 *
 * *******************************/
void iap_check_memory_protection()
{
	FLASH_OBProgramInitTypeDef ob_config;
	HAL_FLASHEx_OBGetConfig(&ob_config);
	if(ob_config.RDPLevel==OB_RDP_LEVEL_0)
	{
		ob_config.OptionType=OPTIONBYTE_RDP;
		ob_config.RDPLevel=OB_RDP_LEVEL_1;
		HAL_FLASH_OB_Unlock();
		HAL_FLASHEx_OBProgram(&ob_config);
		HAL_FLASH_OB_Launch();
	}
	
}

/**********************************
 * @biref:switch boot from which bank
 * @param bank: OB_BOOT_BANK1 OB_BOOT_BANK2
 * *******************************/
void iap_flash_bank_switch(uint16_t bank)
{
	FLASH_AdvOBProgramInitTypeDef adv_config;
	HAL_FLASHEx_AdvOBGetConfig(&adv_config);
	adv_config.OptionType=OPTIONBYTE_BOOTCONFIG;
	adv_config.BootConfig = bank;

	HAL_FLASH_OB_Unlock();
	HAL_FLASHEx_AdvOBProgram(&adv_config);
	HAL_FLASH_OB_Launch();
}

/**********************************
 * @biref:Gets the currently booted bank
 *  
 * *******************************/
uint8_t iap_flash_get_boot_bank()
{
	if((FLASH->OPTR & FLASH_OPTR_BFB2) >> 23U)
	{
		return OB_BOOT_BANK2;
	}
	else 
	{
		return OB_BOOT_BANK1;
	}	
}
/**********************************
 * @biref:Automatically switch the next start bank
 * 		  now bank1->bank2   now bank2->bank1	  
 * *******************************/
void iap_set_next_boot_bank()
{
	if(iap_flash_get_boot_bank()==OB_BOOT_BANK1)
	{
		iap_flash_bank_switch(OB_BOOT_BANK2);	
	}
	else
	{
		iap_flash_bank_switch(OB_BOOT_BANK1);
	}
}
/**********************************
 * @biref:flash pages erase 
 * @param add:page start address,must align to 128 
 * @param pages:how much pages need to be erased
 * *******************************/
void iap_flash_erase(uint32_t add, uint16_t pages)
{
	uint32_t PageError = 0;
	FLASH_EraseInitTypeDef f;

	HAL_FLASH_Unlock();
	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.PageAddress = add;
	f.NbPages = pages;
	HAL_FLASHEx_Erase(&f, &PageError);
	HAL_FLASH_Lock();
}
/**********************************
 * @biref:write data to flash
 * @param add:The address to write to 
 * @param len:data length
 * @note: Data does not have to be 4-byte aligned 
 * *******************************/

void iap_flash_write(uint32_t add, uint8_t *pdata, uint16_t len)
{
	uint32_t data;
	
	iap_flash_erase(add,1);
	
	HAL_FLASH_Unlock();
	for (uint8_t i = 0; i < len / 4; i++)
	{
		data = (pdata[i * 4 + 3] << 24) + (pdata[i * 4 + 2] << 16) + (pdata[i * 4 + 1] << 8) + (pdata[i * 4]);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, add + i * 4, data);
	}
	HAL_FLASH_Lock();
}
#if 0
/**********************************
 * @biref:copy bank2 data to bank1
 * 
 * *******************************/
void iap_flash_copy_check()
{
	if(iap_flash_get_boot_bank()==OB_BOOT_BANK2)
	{
		HAL_FLASH_Unlock();
		iap_flash_erase(APP_BANK2, 512);        
		HAL_FLASH_Lock();
		
		HAL_FLASH_Unlock();
		for(uint32_t i=0;i<16384;i++)
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, APP_BANK2+i*4, *((uint32_t*)(APP_BANK1 + (i << 2))));
			HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		}
		HAL_FLASH_Lock();
		iap_flash_bank_switch(OB_BOOT_BANK1);
	}
	
}
#endif

void _iap_send_onebyte(uint8_t ch)
{
	HAL_UART_Transmit(&huart2, &ch, 1, 1000);
}

uint16_t xmodem_crc16(uint8_t *pdata, uint32_t data_len)
{
	uint16_t wCRCin = 0x0000;
	uint16_t wCPoly = 0x1021;
	uint8_t wChar = 0;

	while (data_len--)
	{
		wChar = *(pdata++);
		wCRCin ^= (wChar << 8);
		for (int i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	return (wCRCin);
}

bool iap_check_firmware()
{
	if(xmodem_crc16((uint8_t *)APP_BANK2,*((uint32_t *)(APP_BANK2+16))+4)==0x00)
	{
		return true;
	}
	return false;
}

//开始执行IAP
uint8_t iap_start()
{
	static uint16_t timer = 0;
	xmod_state = X_STATE_START;

	while (1)
	{
		switch (xmod_state)
		{
		case X_STATE_START:
			timer = 0;
			pack_number = 1;
			//iap_flash_erase(APP_BANK2 , 512);
			xmod_state = X_STATE_REQ_SEND;
			break;
		case X_STATE_REQ_SEND:
			_iap_send_onebyte('C');
			xpack_state = X_PACK_STATE_SOH;
			recv_counter = 0;
			xmod_state = X_STATE_RECV_DATA;
			break;
		case X_STATE_RECV_DATA:
			timer++;
			if (timer >= 200) //3sec
			{
				HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
				timer = 0;
				xmod_state = X_STATE_REQ_SEND;
			}
			break;
		case X_STATE_RECV_OK:
			if (recv_buf[1] == pack_number)
			{
				HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
				iap_flash_write(APP_BANK2 + PACK_SIZE * (pack_number - 1), (recv_buf + 3), PACK_SIZE);
				pack_number++;
				xmod_state = X_STATE_DATA_ACK;
			}
			else
			{
				xmod_state = X_STATE_CAN;
			}
			xpack_state = X_PACK_STATE_SOH;
			recv_counter = 0;
			break;
		case X_STATE_DATA_ACK:
			_iap_send_onebyte(ACK);
			xmod_state = X_STATE_RECV_DATA;
			break;
		case X_STATE_DATA_NACK:
			_iap_send_onebyte(NAK);
			xmod_state = X_STATE_RECV_DATA;
			break;
		case X_STATE_EOT_ACK:
			_iap_send_onebyte(ACK);
			xmod_state = X_STATE_END;
			//iap_flash_bank_switch(OB_BOOT_BANK2);
			if(iap_check_firmware()!=true)return 0;
			iap_set_next_boot_bank();
			return 1;
		case X_STATE_CAN:
			_iap_send_onebyte(CANC);
			xmod_state = X_STATE_END;
			return 0;
		case X_STATE_END:
			break;
		}
		HAL_Delay(10);
	}
}

void iap_uart_handler()
{
	uint8_t ch;
	uint16_t crc;
	uint16_t rcrc;
	if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
	{
		ch = (uint8_t)(huart2.Instance->RDR & 0x00FF);
	}
	if (xmod_state != X_STATE_RECV_DATA)
		return;

	switch (xpack_state)
	{
	case X_PACK_STATE_SOH:
		if (ch == EOT)
		{
			xmod_state = X_STATE_EOT_ACK;
		}
		else if (ch == SOH)
		{
			recv_buf[recv_counter++] = ch;
			xpack_state = X_PACK_STATE_NUMBER;
		}
		break;
	case X_PACK_STATE_NUMBER:
		recv_buf[recv_counter++] = ch;
		if (recv_counter == 3)
			xpack_state = X_PACK_STATE_DATA;
		break;
	case X_PACK_STATE_DATA:
		recv_buf[recv_counter++] = ch;
		if (recv_counter == (PACK_SIZE + 3))
		{
			xpack_state = X_PACK_STATE_CRC;
		}
		break;
	case X_PACK_STATE_CRC:
		recv_buf[recv_counter++] = ch;
		if (recv_counter == (PACK_SIZE + 5))
		{
			crc = xmodem_crc16(recv_buf + 3, PACK_SIZE);
			rcrc = (recv_buf[PACK_SIZE + 3] << 8) + recv_buf[PACK_SIZE + 4];
			if (crc == rcrc)
				xmod_state = X_STATE_RECV_OK;
			xpack_state = X_PACK_STATE_IDLE;
		}
		break;
	default:
		break;
	}
}
