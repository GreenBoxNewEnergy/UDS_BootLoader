/*******************************************************************************
File - UDS_WDBI.c
UDS write data by ident.
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
#include "eeprom.h"
#include "UDS_Const.h"

#if USE_WR_DATA_BYID

/**
* Find_Wdbi
*
* @param WdbiIdent - Wdbi Identifier.
*        errCode - Pointer to errCode.
* @return Pointer of type <WDBI_T> pointing to found object, NULL if not found. 
* @date Mar. 20th 2017
*/
UDS_CONST_STORAGE WDBI_t *Find_Wdbi(U16 WdbiIdent, U8 *errCode)
{
  UDS_CONST_STORAGE WDBI_t *Wdbi = UDS_NULL;
  U16 i;
  for(i=0; i<WDBI_MAX; i++)
  {
    if(UDS_ROM.Wdbi[i].Ident == WdbiIdent)
    {
      Wdbi = &UDS_ROM.Wdbi[i];
      break;
    }
  }
  if(Wdbi)
  {
    *errCode = FIND_ERR_NO;
    /* Grant the right to access the Service */
    if(UDS_RAM.Session==DFLT_SESSN && (Wdbi->Access&ACCS_DFLT))
    {
    }
    else if(UDS_RAM.Session==ECU_PROG_SESSN && (Wdbi->Access&ACCS_PROG))
    {
    }
    else if(UDS_RAM.Session==ECU_EXT_DIAG_SESSN && (Wdbi->Access&ACCS_EXT_DIAG))
    {
    }
    else
    {
      /* conditionsNotCorrect */
      *errCode = FIND_ERR_COND;
      return UDS_NULL;
    }
  }
  else
  {
    *errCode = FIND_ERR_NULL;
  }
  return Wdbi;
}

/**
* WdbiFingerprint
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         RSP_TOOLONG - responseTooLong
* @date Mar. 20th 2017
*/
#if USE_WDBI_FINGERPRINT
U8 WdbiFingerprint(U8 *pData, U16 *uwLen)
{
  U8 i;
  /* Unlock the Flash Program Erase controller */
  __disable_irq();
  FLASH_Unlock();
  U16 uwData;
  /* #3 - programmingDate Year (BCD-coded) 
#4 - programmingDate Month (BCD-coded) 
#5 - programmingDate Date (BCD-coded) 
#6 - testerSerialNumber (Byte 1, ASCII)
#7 - testerSerialNumber (Byte 2, ASCII)
  ...
#21 - testerSerialNumber (Byte 16, ASCII) */
  for(i=0;i<10;i++)
  {
    uwData = ((U16)pData[i*2+3]<<8)+pData[i*2+4];
    EE_WriteVariable(FP_ADDR_START+i, uwData);
  }
  FLASH_Lock();
  __enable_irq();
  *uwLen = 3;
  return POS_RSP;
}
#endif /* USE_WDBI_FINGERPRINT */

#endif /* USE_WR_DATA_BYID */
