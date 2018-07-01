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
#ifndef _UDS_CALLBACK_H_
#define _UDS_CALLBACK_H_

#if USE_ROUTINE_CNTL && USE_RT_ERASEMEM
U8 EraseFlash_Callback(U8 *pData, U16 *uwLen);
#endif

#endif /* _UDS_CALLBACK_H_ */
