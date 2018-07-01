/*******************************************************************************
File - UDS_Driver.c
CAN module object for Microchip STM32F10x microcontroller.
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Jan. 24th 2017
Version: V1.0.0
*******************************************************************************/
#include "UDS_Driver.h"
#include "UDS_Config.h"

extern UDS_CONST_STORAGE s_UDS_CFGS UDS_CFGS_1[];  /* Services Information */

/**
* CAN receive initialization
*
* @param rxBuffer - pointer to rxBuffer object <UDS_CANrxBuf_t>.
*        idx - Index of CAN filter.
*        ident - 11-bit standard or 29-bit extended CAN Identifier.
*        mask - 11-bit mask for standard identifier. Most usually set to 0x7FF.
*               29-bit mask for extended identifier. Most usually set to 0x1FFFFFFF.
*        rtr - If true, 'Remote Transmit Request' messages will be accepted.
*        ide - If true, 'Extended identifier' messages will be accepted.
*        object - to which buffer is connected. It will be used as an argument to pFunct. 
*        pFunct - Pointer to function, which will be called. 
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 23th 2017
*/
UDS_ReturnError UDS_CANrxInit(UDS_CANrxBuf_t *rxBuffer, U8 idx,
                              U32 ident, U32 mask, U8 rtr, U8 ide,
                              void *object, 
                              UDS_ReturnError (*pFunct)(void *object, UDS_CANrxMsg_t *message, U8 idx))
{
  UDS_ReturnError err = UDS_ERROR_NO;
  if(!rxBuffer || !object || !pFunct){
    return UDS_ERROR_ILLEGAL_ARGUMENT;
  }
  /* Configure object variables */
  rxBuffer->object = object;
  rxBuffer->pRcb = pFunct;
#if USE_CAN_FILTERS == UDS_TRUE
  /* Set CAN filter and mask */
  U32 RXF, RXM;
  if(ide)
  {
    RXF = ident<<3;
    RXM = mask<<3;
  }
  else
  {
    RXF = ident<<21;
    RXM = mask<<21;
  }
  RXM |= 0x7;
  if(idx<=13)
  {
    CAN_FilterInitTypeDef  can_filterinit;
    can_filterinit.CAN_FilterNumber=idx;
    can_filterinit.CAN_FilterMode=CAN_FilterMode_IdMask;
    can_filterinit.CAN_FilterScale=CAN_FilterScale_32bit;
    can_filterinit.CAN_FilterIdHigh=(RXF>>16)&0xFFFF;
    if(ide)
      can_filterinit.CAN_FilterIdLow=(RXF|CAN_ID_EXT|(!rtr?CAN_RTR_DATA:CAN_RTR_REMOTE))&0xFFFF;
    else
      can_filterinit.CAN_FilterIdLow=(RXF|CAN_ID_STD|(!rtr?CAN_RTR_DATA:CAN_RTR_REMOTE))&0xFFFF;
    can_filterinit.CAN_FilterMaskIdHigh=(RXM>>16)&0xFFFF;
    can_filterinit.CAN_FilterMaskIdLow=RXM&0xFFFF;
    can_filterinit.CAN_FilterFIFOAssignment=CAN_FIFO0;
    can_filterinit.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&can_filterinit);
  }
  else if(idx&FILTER_SKIP)
  {
    /* nothing to do */
  }
  else
  {
    /* not enough filter */
    err = UDS_ERROR_ILLEGAL_ARGUMENT;
  }
#else
  if(idx==0)
  {
    CAN_FilterInitTypeDef can_filterinit;
    can_filterinit.CAN_FilterNumber=0;
    can_filterinit.CAN_FilterMode=CAN_FilterMode_IdMask;
    can_filterinit.CAN_FilterScale=CAN_FilterScale_32bit;
    can_filterinit.CAN_FilterIdHigh=0x0000;
    can_filterinit.CAN_FilterIdLow=0x0000;
    can_filterinit.CAN_FilterMaskIdHigh=0x0000;
    can_filterinit.CAN_FilterMaskIdLow=0x0000;
    can_filterinit.CAN_FilterFIFOAssignment=CAN_FIFO0;
    can_filterinit.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&can_filterinit);
  }
#endif
  /* CAN identifier and CAN mask, bit aligned with CAN FIFO buffers (RTR is extra) */
  rxBuffer->Ident = ident;
  if(rtr) rxBuffer->Ident |= 0x20000000;
  rxBuffer->mask = (mask & 0x1FFFFFFF) | 0x20000000;
  rxBuffer->bExtID = ide;
  return err;
}

/**
* CAN receive Interrupt
*
* @param CANx - pointer to CANx object <CAN_TypeDef>.
* @date Jan. 23th 2017
*/
void UDS_CAN_Interrupt(CAN_TypeDef* CANx){
  /* receive interrupt */
  UDS_CANrxMsg_t msg; /* received message in CAN module */
  U8 index; /* index of received message */
  U32 msgIdent; /* identifier of the received message */
  UDS_CANrxBuf_t *msgBuff;  /* receive message buffer from CO_CANmodule_t object. */
  U8 msgMatched = 0;
  CAN_Receive(CANx, CAN_FIFO0, (CanRxMsg *)&msg);
  switch(msg.IDE)
  {
  case CAN_ID_STD:
    msgIdent = msg.stdid;
    break;
  case CAN_ID_EXT:
    msgIdent = msg.extid;
    break;
  }
  if(msg.RTR == CAN_RTR_REMOTE)
    msgIdent |= 0x20000000;
#if USE_CAN_FILTERS == UDS_TRUE
  /* CAN filters are used. */
  for(index=0;index<NWS_MSG_MAX;index++)
  {
    if((UDS_CFGS_1[index].idxf&FILTER_MASK) == msg.FMI && UDS_CFGS_1[index].pCANrxBuf!=UDS_NULL)
    {
      if(!UDS_CFGS_1[index].pCANrxBuf->N_PCI_OFFSET ||
         (UDS_CFGS_1[index].pCANrxBuf->N_PCI_OFFSET &&
          UDS_CFGS_1[index].pCANrxBuf->ExtraAddr == msg.data[0]))
      {
        msgBuff = UDS_CFGS_1[index].pCANrxBuf;
        break;
      }
    }
  }
  /* verify also RTR */
  if(msgBuff && ((msgIdent ^ msgBuff->Ident) & msgBuff->mask) == 0)
    msgMatched = 1;
#else
  /* CAN filters are not used */
  for(index = 0; index < CAN_FILTER_MAX; index++){
    msgBuff = UDS_CFGS_1[index].pCANrxBuf;
    if(!msgBuff->N_PCI_OFFSET || (msgBuff->N_PCI_OFFSET && msgBuff->ExtraAddr == msg.data[0]))
    {
      if(((msgIdent ^ msgBuff->Ident) & msgBuff->mask) == 0){
        msgMatched = 1;
        break;
      }
    }
  }
#endif
  /* Call specific function, which will process the message */
  if(msgMatched && msgBuff->pRcb) msgBuff->pRcb(msgBuff->object, &msg, index);
}

/**
* Send CAN message.
*
* @param buf - pointer to buf object <UDS_CANtxBuf_t>.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
*         UDS_ERROR_TX_OVERFLOW - Previous message is still waiting, buffer full.
* @date Jan. 18th 2017
*/
UDS_ReturnError UDS_CAN_Send(UDS_CANtxBuf_t *buf)
{
  U8 i;
  CanTxMsg TxMessage;
  TxMessage.DLC = buf->DLC;
  TxMessage.IDE = buf->bExtID?CAN_Id_Extended:CAN_Id_Standard;
  TxMessage.RTR = buf->bRemote?CAN_RTR_Remote:CAN_RTR_Data;
  buf->bExtID?(TxMessage.ExtId = buf->Ident):(TxMessage.StdId = buf->Ident);
  for(i=0; i<TxMessage.DLC; i++)
    TxMessage.Data[i] = buf->Data[i];
  if(CAN_TxStatus_Ok != STM_BSP_CAN_WriteData(&TxMessage))
    return UDS_ERROR_TX_OVERFLOW;
  return UDS_ERROR_NO;
}
