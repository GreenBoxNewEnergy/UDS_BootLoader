/*******************************************************************************
File - UDS_NWS.h
Network layer services.
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Jan. 24th 2017
Version: V1.0.0
*******************************************************************************/
#ifndef _UDS_NWS_H_
#define _UDS_NWS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Flow Control Flow Status definition */
#define NWS_FS_CLEAR_TO_SEND          0U
#define NWS_FS_WAIT                   1U
#define NWS_FS_OVERFLOW               2U

#define GET_MSG_RX(idx)               NWS->pUDS_CFGS[idx].pNWS_MSG->pMSG_RX
#define GET_MSG_TX(idx)               NWS->pUDS_CFGS[idx].pNWS_MSG->pMSG_TX
#define GET_MSG_TIM(idx)              NWS->pUDS_CFGS[idx].pNWS_MSG->pMSG_TIM

#define GET_CAN_RX(idx)               NWS->pUDS_CFGS[idx].pCANrxBuf
#define GET_CAN_TX(idx)               NWS->pUDS_CFGS[idx].pCANtxBuf

#define RX_N_PCI_IDX(i)               NWS->pUDS_CFGS[i].pCANrxBuf->N_PCI_OFFSET
#define TX_N_PCI_IDX(i)               NWS->pUDS_CFGS[i].pCANtxBuf->N_PCI_OFFSET

typedef struct s_NWS NWS_t;

struct s_NWS{
  UDS_CONST_STORAGE s_UDS_CFGS *pUDS_CFGS;
  UDS_RX_LONG_RSP_t *pUDS_RX_LONG_RSP;
  N_PDU_t *pN_PDU_in;
  N_PDU_t *pN_PDU_out;
};

/* Read received message from CAN */
UDS_ReturnError NWS_Receive(void *object, UDS_CANrxMsg_t *msg, U8 idxf);
/* Stop TP message transmission */
void NWS_AbortMsg(NWS_t *NWS, U8 msgIdx);
/* Send CAN message */
UDS_ReturnError NWS_SendFrame(UDS_CANtxBuf_t *buf);
/* Message transmission buffer update */
void NWS_SetTxMsgData(NWS_MSG_TX_t *pMsg, U8 *pData, U16 uwLen);
/* Send a message using the Transport Protocol */
UDS_ReturnError NWS_SendMsgData(NWS_t *NWS, U8 msgIdx);
/* Initializes NWS */
UDS_ReturnError NWS_Init(NWS_t **ppNWS);
/* Processing NWS */
UDS_ReturnError NWS_Process(NWS_t *NWS, U32 timeStamp);
/* Message reception indication */
UDS_ReturnError NWS_RxIndication(NWS_t *NWS, U8 msgIdx, N_Result_t Result);
/* Message reception request */
UDS_ReturnError NWS_RxRequest(NWS_t *NWS, U8 msgIdx);
/* Message reception indication (long request) */
UDS_ReturnError UDS_LongRxIndication(NWS_t *NWS);
/* Message transmit confirm */
UDS_ReturnError NWS_TxConfirm(NWS_t *NWS, U8 msgIdx, N_Result_t Result);
/* Processing a message */
void NWS_ReceiveMessage_Do(NWS_t *NWS, U8 msgIdx);
/* Pesponse a message */
void NWS_PesponseMessage(NWS_t *NWS, U8 msgIdx, UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 NRC);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _UDS_NWS_H_ */
