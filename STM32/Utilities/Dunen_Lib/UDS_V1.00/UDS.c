/*******************************************************************************
File - UDS.c
Unified diagnostic services.
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Jan. 24th 2017
Version: V1.0.0
*******************************************************************************/
#include "UDS.h"

UDS_t UDS_handles;
NWS_t NWS_handles;

/**
* Initializes UDS
*
* @param ppUDS - Pointer to address of UDS object <UDS_t>.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError UDS_Init(UDS_t **ppUDS)
{
  UDS_t *UDS;
  (*ppUDS) = &UDS_handles;
  UDS = *ppUDS; //pointer to (newly created) object
  
  UDS->UDS_RAM = &UDS_RAM;

  NWS_Init(&UDS->NWS);
  
  return UDS_ERROR_NO;
}

/**
* Process UDS objects 
*
* @param UDS - Pointer to UDS object <UDS_t>.
*        timeStamp - Time interval.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @node Function must be called cyclically. 
* @date Jan. 23th 2017
*/
UDS_ReturnError UDS_Process(UDS_t *UDS, U32 timeStamp)
{
  NWS_Process(UDS->NWS, timeStamp);
  UDS_APP_Process(timeStamp);
  return UDS_ERROR_NO;
}
