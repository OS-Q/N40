#ifndef __IAP_H
#define __IAP_H

#include "main.h"

#define APP_BANK1   0X8000000
#define APP_BANK2   0X8010000

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CANC                    (0x18)  /* two of these in succession aborts transfer */
#define CHARC                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */


uint8_t iap_start(void);
void iap_uart_handler(void);
uint8_t iap_flash_get_boot_bank(void);
void iap_flash_bank_switch(uint16_t bank);
void iap_flash_copy_check(void);
void iap_check_memory_protection(void);

#endif

