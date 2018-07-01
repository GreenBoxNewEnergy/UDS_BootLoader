/*******************************************************************************
File - UDS_Driver.h
CAN module object for Microchip STM32F10x microcontroller.
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Jan. 24th 2017
Version: V1.0.0
*******************************************************************************/
#ifndef _UDS_DRIVER_H_
#define _UDS_DRIVER_H_
#include <stdint.h>
#include <string.h>
#include "bsp.h"

/* stored in ROM */
#define UDS_CONST_STORAGE               const

#include "UDS_Types.h"

/* CAN receive initialization */
UDS_ReturnError UDS_CANrxInit(UDS_CANrxBuf_t *rxBuffer, U8 idx,
                              U32 ident, U32 mask, U8 rtr, U8 ide,
                              void *object, 
                              UDS_ReturnError (*pFunct)(void *object, UDS_CANrxMsg_t *message, U8 idx));
/* CAN receive Interrupt */
void UDS_CAN_Interrupt(CAN_TypeDef* CANx);

/* Send CAN message */
UDS_ReturnError UDS_CAN_Send(UDS_CANtxBuf_t *buf);

#endif /* _UDS_DRIVER_H_ */
