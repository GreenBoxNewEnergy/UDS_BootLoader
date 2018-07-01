/*******************************************************************************
File - UDS.h
Unified diagnostic services.
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Jan. 24th 2017
Version: V1.0.0
*******************************************************************************/
#ifndef _UDS_H_
#define _UDS_H_
#include "UDS_Driver.h"
#include "UDS_Config.h"
#include "UDS_Primitive.h"
#include "UDS_NWS.h"
#if UDS_MASTER == UDS_TRUE
#include "UDS_Master.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
NWS_t *NWS;
s_UDS_RAM *UDS_RAM;
} UDS_t;

/* Initializes UDS */
UDS_ReturnError UDS_Init(UDS_t **ppUDS);
/* Process CAN objects */
UDS_ReturnError UDS_Process(UDS_t *UDS, U32 timeStamp);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _UDS_H_ */
