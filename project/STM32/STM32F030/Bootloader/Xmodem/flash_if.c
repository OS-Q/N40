/******************************************************************************
****版本：1.0.0
****平台：
****日期：2020-07-29
****作者：Qitas
****版权：
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
#include "menu.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)	//checked hiyangdong
{
  /* Unlock the Program memory */
  HAL_FLASH_Unlock();

  /* Clear all FLASH flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
  /* Unlock the Program memory */
  HAL_FLASH_Lock();
}

/**
  * @brief  This function does an erase of all user flash area
  * @param  start: start of user flash area
  * @retval FLASHIF_OK : user flash area successfully erased
  *         FLASHIF_ERASEKO : error occurred
  */
uint32_t FLASH_If_Erase(uint32_t start)		//擦除数据，因此需要修改
{
  FLASH_EraseInitTypeDef desc;
  uint32_t result = FLASHIF_OK;

  HAL_FLASH_Unlock();

  desc.PageAddress = start;
  desc.TypeErase = FLASH_TYPEERASE_PAGES;

/* NOTE: Following implementation expects the IAP code address to be < Application address */
//  if (start < FLASH_START_BANK2 )
//  {
//    desc.NbPages = (FLASH_START_BANK2 - start) / FLASH_PAGE_SIZE;
//    if (HAL_FLASHEx_Erase(&desc, &result) != HAL_OK)
//    {
      result = FLASHIF_ERASEKO;
//    }
//  }

//  if (result == FLASHIF_OK )
//  {
//    desc.PageAddress = ABS_RETURN(start, FLASH_START_BANK2);
//    desc.NbPages = (USER_FLASH_END_ADDRESS - ABS_RETURN(start, FLASH_START_BANK2)) / FLASH_PAGE_SIZE;
//    if (HAL_FLASHEx_Erase(&desc, &result) != HAL_OK)
//    {
//      result = FLASHIF_ERASEKO;
//    }
//  }

  HAL_FLASH_Lock();

  return result;
}

/* Public functions ---------------------------------------------------------*/
/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  destination: start address for target location
  * @param  p_source: pointer on buffer with data to write
  * @param  length: length of data buffer (unit is 32-bit word)
  * @retval uint32_t 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32_t FLASH_If_Write(uint32_t destination, uint32_t *p_source, uint32_t length)		//checked hiyangdong 无问题
{
  uint32_t i = 0;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  for (i = 0; (i < length) && (destination <= (USER_FLASH_END_ADDRESS-4)); i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, destination, *(uint32_t*)(p_source+i)) == HAL_OK)
    {
     /* Check the written value */
      if (*(uint32_t*)destination != *(uint32_t*)(p_source+i))
      {
        /* Flash content doesn't match SRAM content */
        return(FLASHIF_WRITINGCTRL_ERROR);
      }
      /* Increment FLASH destination address */
      destination += 4;
    }
    else
    {
      /* Error occurred while writing data in Flash memory */
      return (FLASHIF_WRITING_ERROR);
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  return (FLASHIF_OK);
}

/**
  * @brief  Returns the write protection status of application flash area.
  * @param  None
  * @retval If a sector in application area is write-protected returned value is a combinaison
            of the possible values : FLASHIF_PROTECTION_WRPENABLED, FLASHIF_PROTECTION_PCROPENABLED, ...
  *         If no sector is write-protected FLASHIF_PROTECTION_NONE is returned.
  */
uint32_t FLASH_If_GetWriteProtectionStatus(void)  //checked hiyangdong 可以删除
{
  uint32_t ProtectedPAGE = FLASHIF_PROTECTION_NONE;
  FLASH_OBProgramInitTypeDef OptionsBytesStruct;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Check if there are write protected sectors inside the user flash area ****/
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Get pages already write protected ****************************************/
  ProtectedPAGE = ~(OptionsBytesStruct.WRPPage);

  /* Check if desired pages are already write protected ***********************/
  if(ProtectedPAGE != 0)
  {
    /* Some sectors inside the user flash area are write protected */
    return FLASHIF_PROTECTION_WRPENABLED;
  }
  else
  {
    /* No write protected sectors inside the user flash area */
    return FLASHIF_PROTECTION_NONE;
  }
}

/**
  * @brief  Configure the write protection status of user flash area.
  * @param  protectionstate : FLASHIF_WRP_DISABLE or FLASHIF_WRP_ENABLE the protection
  * @retval uint32_t FLASHIF_OK if change is applied.
  */
uint32_t FLASH_If_WriteProtectionConfig(uint32_t protectionstate)
{
  uint32_t ProtectedPAGE = 0x0;
  FLASH_OBProgramInitTypeDef config_new, config_old;
  HAL_StatusTypeDef result = HAL_OK;


  /* Get pages write protection status ****************************************/
  HAL_FLASHEx_OBGetConfig(&config_old);

  /* The parameter says whether we turn the protection on or off */
  config_new.WRPState = (protectionstate == FLASHIF_WRP_ENABLE ? OB_WRPSTATE_ENABLE : OB_WRPSTATE_DISABLE);

  /* We want to modify only the Write protection */
  config_new.OptionType = OPTIONBYTE_WRP;

  /* No read protection, keep BOR and reset settings */
  config_new.RDPLevel = OB_RDP_LEVEL_0;
  config_new.USERConfig = config_old.USERConfig;
  /* Get pages already write protected ****************************************/
//  ProtectedPAGE = config_old.WRPPage | FLASH_PAGE_TO_BE_PROTECTED;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Unlock the Options Bytes *************************************************/
  HAL_FLASH_OB_Unlock();

  /* Erase all the option Bytes ***********************************************/
  result = HAL_FLASHEx_OBErase();

  if (result == HAL_OK)
  {
    config_new.WRPPage    = ProtectedPAGE;
    result = HAL_FLASHEx_OBProgram(&config_new);
  }

  return (result == HAL_OK ? FLASHIF_OK: FLASHIF_PROTECTION_ERRROR);
}

/*-------------------------(C) COPYRIGHT 2020 QITAS --------------------------*/

