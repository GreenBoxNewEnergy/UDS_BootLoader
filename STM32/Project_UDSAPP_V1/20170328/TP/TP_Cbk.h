/*******************************************************************************
File - TP_Cbk.h
This is common module for body netword
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: Byron
Date: 2017/01/01
Version: V1.0.0
*******************************************************************************/
#ifndef _TP_CBK_H_
#define _TP_CBK_H_
#include "stm32f10x.h"

/*******************************************************************************
*
*       Public Function Prototypes
*
*/

void TP_TxConf (tTpMsgIdx uMsgIdx, u8 ebStatus);
void TP_RxInd (tTpMsgIdx uMsgIdx, u8 ebStatus);

void TP_DataBufferEmptyInd (tTpMsgIdx uMsgIdx);
void TP_RxEnableInd (tTpMsgIdx uMsgIdx, tTpRxEnable uRxEnable);

#endif
