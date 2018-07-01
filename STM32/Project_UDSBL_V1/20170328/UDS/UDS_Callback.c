/*******************************************************************************
File - UDS_Callback.c
UDS Configuration.
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Feb. 23th 2017
Version: V1.0.0
*******************************************************************************/
#include "UDS_Driver.h"
#include "UDS_Config.h"
#include "UDS_Primitive.h"
#include "UDS_Callback.h"

#if USE_ROUTINE_CNTL && USE_RT_ERASEMEM
U8 EraseFlash_Callback(U8 *pData, U16 *uwLen)
{
  if(UDS_RAM.FlashEraseFlag)
  {
    return REQ_CORR_RECV_RSP_PEND;
  }
  else
  {
    pData[0] = 0x71;
    pData[4] = ROUTINE_CORR_RSLT; /* correctResult */
    *uwLen = 5;
    return POS_RSP;
  }
}
#endif /* USE_RT_ERASEMEM */
