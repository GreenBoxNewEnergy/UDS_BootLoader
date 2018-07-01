/*******************************************************************************
File - UDS_APP.c
UDS application.
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Mar. 20th 2017
Version: V1.0.0
*******************************************************************************/
#include "UDS_Driver.h"
#include "UDS_Config.h"
#include "UDS_Primitive.h"
#if USE_REQ_DOWNLOAD || USE_XFER_DATA || USE_REQ_XFER_EXIT || USE_RT_ERASEMEM \
    || (USE_ROUTINE_CNTL && USE_RT_ERASEMEM)
#include "flash.h"
#endif
#if USE_ROUTINE_CNTL && USE_RT_ERASEMEM
#include "eeprom.h"
#endif

/**
* ReceiveMessageHook
*
* @date Feb. 18th 2017
*/
void ReceiveMessageHook(void)
{
#if USE_S3_SERVER
  UDS_RAM.S3_Server_cnt = UDS_RAM.S3_Server_max;
#endif
}

/**
* Process APP objects 
*
* @return Pointer of type <Routine_t> pointing to found object, NULL if not found. 
* @date Mar. 21th 2017
*/
UDS_ReturnError UDS_APP_Process(U32 timeStamp)
{
  UDS_ReturnError err = UDS_ERROR_NO;
#if USE_ECU_RST
  switch(UDS_RAM.resetType)
  {
  case HARD_RST:
  case SOFT_RST:
    __set_FAULTMASK(1);
    NVIC_SystemReset();
    break;
  default:
    break;
  }
#endif
  
#if USE_SEC_ACCS
  if(UDS_RAM.requiredTimeDelayCnt > 0)
  {
    UDS_RAM.requiredTimeDelayCnt>timeStamp?(UDS_RAM.requiredTimeDelayCnt-=timeStamp):(UDS_RAM.requiredTimeDelayCnt=0);
  }
#endif
  
#if USE_S3_SERVER
  if(UDS_RAM.Session != DFLT_SESSN)
  {
    if(UDS_RAM.S3_Server_cnt > 0)
    {
      UDS_RAM.S3_Server_cnt>timeStamp?(UDS_RAM.S3_Server_cnt-=timeStamp):(UDS_RAM.S3_Server_cnt=0);
      if(UDS_RAM.S3_Server_cnt == 0)
      {
        __set_FAULTMASK(1);
        NVIC_SystemReset();
      }
    }
  }
#endif
  
#if USE_DIAG_SESSN_CNTL
  if(UDS_RAM.Session == ECU_PROG_SESSN)
  {
    __set_FAULTMASK(1);
    NVIC_SystemReset();
  }
#endif
  return err;
}

/**
* Process Long Operation objects 
*
* @return Pointer of type <Routine_t> pointing to found object, NULL if not found. 
* @date Mar. 21th 2017
*/
UDS_ReturnError UDS_LongOperation_Process(void)
{
  UDS_ReturnError err = UDS_ERROR_NO;
#if USE_ROUTINE_CNTL && USE_RT_ERASEMEM
  if(UDS_RAM.FlashEraseFlag)
  {
    __disable_irq();
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    if(Flash_Erase(UDS_RAM.eraseMemoryAddress, UDS_RAM.eraseMemorySize))
    {
      EE_WriteVariable(APP_FLAG_ADDR, 0x0000);
      UDS_RAM.FlashEraseFlag = 0;
    }
    FLASH_Lock();
    __enable_irq();
  }
#endif
  return err;
}
