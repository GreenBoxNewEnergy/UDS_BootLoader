/*******************************************************************************
File - UDS_NWS.c
Network layer services.
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
#include "UDS_Primitive.h"
#include "UDS_NWS.h"

/*******************************************************************************
*
*       NWS Variates
*
*/
NWS_t nws;
N_PDU_t m_N_PDU[N_PDU_RING_BUF_SIZE]; /* Ring Buffer */
extern UDS_CONST_STORAGE s_UDS_CFGS UDS_CFGS_1[];  /* Services Information */
extern UDS_RX_LONG_RSP_t UDS_RX_LONG_RSP;
extern NWS_t NWS_handles;

/*******************************************************************************
*
*       NWS Extern Functions
*
*/
/* ReceiveMessageHook */
void ReceiveMessageHook(void);

/*******************************************************************************
*
*       NWS Functions
*
*/

/**
* Read received message from CAN
*
* @param object - Pointer to object.
*        msg - Pointer to msg object <UDS_CANrxMsg_t>.
*        idxf - Filter index.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
*         UDS_ERROR_RX_MSG_LENGTH - Wrong receive message length.
*         UDS_ERROR_RX_OVERFLOW - Previous message was not processed yet.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_Receive(void *object, UDS_CANrxMsg_t *msg, U8 idxf){
  U8 i;
  NWS_t *NWS;
  /* this is the correct pointer type of the first argument */
  NWS = (NWS_t*)object;
  /* verify message overflow (previous message was not processed yet) */
  if(NWS->pN_PDU_in->Renew == UDS_TRUE) return UDS_ERROR_RX_OVERFLOW;
  /* copy data */
  NWS->pN_PDU_in->idx = idxf;
  msg->IDE==UDS_CAN_ID_STD?(NWS->pN_PDU_in->Id = msg->stdid):(NWS->pN_PDU_in->Id = msg->extid);
  for(i=0; i<msg->DLC; i++)
    NWS->pN_PDU_in->Data[i] = msg->data[i];
  for(i=msg->DLC; i<8; i++)
    NWS->pN_PDU_in->Data[i] = NWS_PADDING_BYTE_VALUE;
  NWS->pN_PDU_in->DLC = msg->DLC;
  /* set 'new message' flag */
  NWS->pN_PDU_in->Renew = UDS_TRUE;
  /* point to next buffer */
  NWS->pN_PDU_in = NWS->pN_PDU_in->next;
  return UDS_ERROR_NO;
}

/**
* Stop TP message transmission
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @date Jan. 21th 2017
*/
void NWS_AbortMsg(NWS_t *NWS, U8 msgIdx)
{
  GET_MSG_TX(msgIdx)->Status = NWS_MSG_STATUS_NONE;
  GET_MSG_RX(msgIdx)->Status = NWS_MSG_STATUS_NONE;
  GET_MSG_TX(msgIdx)->uCnt = 0;
  GET_MSG_RX(msgIdx)->uCnt = 0;
  GET_MSG_TX(msgIdx)->uBufferCnt = 0;
}

/**
* Send CAN message
*
* @param buf - Pointer to buf object <UDS_CANtxMsg_t>.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_SendFrame(UDS_CANtxBuf_t *buf)
{
#if USE_CAN_FRAME_DATA_OPT == UDS_FALSE
  U8 i;
  for(i=buf->DLC; i<8; i++)
  {
    buf->Data[i] = NWS_PADDING_BYTE_VALUE;
  }
  buf->DLC = 8;
#endif
  return UDS_CAN_Send(buf);
}

/**
* Message transmission buffer update
*
* @param pMsg - Pointer to pMsg object <NWS_MSG_t>.
*        pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @date Jan. 21th 2017
*/
void NWS_SetTxMsgData(NWS_MSG_TX_t *pMsg, U8 *pData, U16 uwLen)
{
  memcpy(pMsg->pData, pData, uwLen);
  pMsg->uwLen = uwLen;
}

/**
* Send a message using the Transport Protocol
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @note With the message length, it chooses to transmit a Single Frame
*       or a First Frame followed by Consecutive Frames
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_SendMsgData(NWS_t *NWS, U8 msgIdx)
{
  U8 i;
  UDS_ReturnError err = UDS_ERROR_NO;
  /* byte0 is N_AE or N_TA */
  if(TX_N_PCI_IDX(msgIdx)==1)
    GET_CAN_TX(msgIdx)->Data[0] = GET_CAN_TX(msgIdx)->ExtraAddr;
  /* If the message length is lower to uMaxSFDL,
  transmit a Single Frame */
  if(GET_MSG_TX(msgIdx)->uwLen<=GET_MSG_TX(msgIdx)->uMaxSFDL)
  {
    /* The status is : Single Frame transmission */
    GET_MSG_TX(msgIdx)->Status = NWS_MSG_STATUS_TX_SF;
    /* Timeout and delay transmission counter initialisation */
    /* (there is no frame to wait) */
    GET_MSG_TX(msgIdx)->uCnt = 0;
    GET_CAN_TX(msgIdx)->Data[TX_N_PCI_IDX(msgIdx)] = (U8)(GET_MSG_TX(msgIdx)->uwLen&0x000FU) | N_PCI_SF;
    for(i=0; i<GET_MSG_TX(msgIdx)->uwLen; i++)
    {
      GET_CAN_TX(msgIdx)->Data[TX_N_PCI_IDX(msgIdx)+1+i] = GET_MSG_TX(msgIdx)->pData[i];
    }
    GET_CAN_TX(msgIdx)->DLC = GET_MSG_TX(msgIdx)->uwLen+1+TX_N_PCI_IDX(msgIdx);
    err = NWS_SendFrame(GET_CAN_TX(msgIdx));
    /* After a Single Frame correct transmission,
    the message transmission is finished => transmission indication */
    GET_MSG_TX(msgIdx)->Status = NWS_MSG_STATUS_NONE;
    if (err == UDS_ERROR_NO)
    {
      NWS_TxConfirm(NWS, msgIdx, N_OK);
    }
  }
  /* If the message length is upper to uMaxSFDL,
  transmit a First Frame followed by Consecutive Frames */
  else
  {
    /* The status is : First Frame transmission */
    GET_MSG_TX(msgIdx)->Status = NWS_MSG_STATUS_TX_FF;
    /* Write the N_PCI and FF_DL fields of the First Frame */
    GET_CAN_TX(msgIdx)->Data[TX_N_PCI_IDX(msgIdx)] = (U8)(GET_MSG_TX(msgIdx)->uwLen>>8U & 0x000FU) | N_PCI_FF;
    GET_CAN_TX(msgIdx)->Data[TX_N_PCI_IDX(msgIdx)+1] = (U8)(GET_MSG_TX(msgIdx)->uwLen);
    /* Copy the TP message data to the temporary buffer for COM frame transmission
    (there is no need to check that ubIdx < uwLen, because at TP_SendFirstFrame ()
    function call, uwLen is upper to TP_DATA_MAX_FIRST_FRAME) */
    for(i=0; i<GET_MSG_TX(msgIdx)->uMaxFFDL; i++)
    {
      GET_CAN_TX(msgIdx)->Data[TX_N_PCI_IDX(msgIdx)+2+i] = GET_MSG_TX(msgIdx)->pData[i];
    }
    /* SN parameter for Consecutive Frames transmission */
    GET_MSG_TX(msgIdx)->ubSnCnt = 1;
    /* Current transmitted data length */
    GET_MSG_TX(msgIdx)->uwLenCnt = GET_MSG_TX(msgIdx)->uMaxFFDL;
    GET_MSG_TX(msgIdx)->uBufferCnt = GET_MSG_TX(msgIdx)->uMaxFFDL;
    /* Timeout and delay transmission counter initialisation
    (there is no frame to wait before transmission confirmation) */
    GET_MSG_TX(msgIdx)->uCnt = 0;
    /* FC Wait counter initialization */
    GET_MSG_TX(msgIdx)->ubRxFCWaitCnt = GET_MSG_TX(msgIdx)->ubRxFCWaitMax;
    GET_CAN_TX(msgIdx)->DLC = 8;
    /* Request CAN frame transmission */
    err = NWS_SendFrame(GET_CAN_TX(msgIdx));
    /* After a First Frame correct transmission,
    the timeout for Flow Control reception is started */
    if (err == UDS_ERROR_NO)
    {
      GET_MSG_TX(msgIdx)->Status = NWS_MSG_STATUS_RX_FC;
      GET_MSG_TX(msgIdx)->uCnt = GET_MSG_TIM(msgIdx)->uNBs;
    }
  }
  return err;
}

/**
* Message reception indication
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_RxIndication(NWS_t *NWS, U8 msgIdx, N_Result_t Result)
{
  if(N_OK==Result)
  {
    return NWS_RxRequest(NWS, msgIdx);
  }
  return UDS_ERROR_NO;
}


/**
* Message reception request
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Mar. 30th 2017
*/
UDS_ReturnError NWS_RxRequest(NWS_t *NWS, U8 msgIdx)
{
  if (NWS->pUDS_RX_LONG_RSP->uRspPendingCnt > 0)
  {
    U8 ServID = GET_MSG_RX(NWS->pUDS_RX_LONG_RSP->idx)->pData[0];
    GET_MSG_TX(NWS->pUDS_RX_LONG_RSP->idx)->pData[0] = UDS_NEG_RSP;
    GET_MSG_TX(NWS->pUDS_RX_LONG_RSP->idx)->pData[1] = ServID;
    GET_MSG_TX(NWS->pUDS_RX_LONG_RSP->idx)->pData[2] = REQ_CORR_RECV_RSP_PEND;
    GET_MSG_TX(NWS->pUDS_RX_LONG_RSP->idx)->uwLen = 3;
    return NWS_SendMsgData(NWS, NWS->pUDS_RX_LONG_RSP->idx);
  }
  else if(GET_MSG_RX(msgIdx)->uwLen>0)
  {
    NWS_SetTxMsgData(GET_MSG_TX(msgIdx), GET_MSG_RX(msgIdx)->pData, GET_MSG_RX(msgIdx)->uwLen);
    return NWS_SendMsgData(NWS, msgIdx);
  }
  return UDS_ERROR_NO;
}


/**
* Message reception indication (long request)
*
* @param NWS - Pointer to NWS object <NWS_t>.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Feb. 23th 2017
*/
UDS_ReturnError UDS_LongRxIndication(NWS_t *NWS)
{
  if(NWS->pUDS_RX_LONG_RSP->idx<NWS_MSG_MAX && NWS->pUDS_RX_LONG_RSP->pLongRspCb != UDS_NULL)
  {
    U8 NRC = NWS->pUDS_RX_LONG_RSP->pLongRspCb(GET_MSG_RX(NWS->pUDS_RX_LONG_RSP->idx)->pData, &GET_MSG_RX(NWS->pUDS_RX_LONG_RSP->idx)->uwLen);
    if(NRC == REQ_CORR_RECV_RSP_PEND)
    {
      /* nothing to do */
      /* NRC_78 will be sent */
    }
    else
    {
      NWS->pUDS_RX_LONG_RSP->uRspPendingCnt = 0;
      NWS_PesponseMessage(NWS, NWS->pUDS_RX_LONG_RSP->idx, NWS->pUDS_RX_LONG_RSP->dsi, NRC);
    }
  }
  return UDS_ERROR_NO;
}

/**
* Message transmit confirm
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_TxConfirm(NWS_t *NWS, U8 msgIdx, N_Result_t Result)
{
  return UDS_ERROR_NO;
}

/**
* NWS_SendFlowControl
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_SendFlowControl(NWS_t *NWS, U8 msgIdx, U8 uFlowStatus)
{
  UDS_ReturnError err = UDS_ERROR_NO;
  /* The status is : Flow Control transmission */
  if (uFlowStatus == NWS_FS_OVERFLOW)
  {
    /* For FS = OVERFLOW, wait for TxOK and end reception */
    GET_MSG_RX(msgIdx)->Status = NWS_MSG_STATUS_TX_FC_OVERFLOW;
  }
  else
  {
    /* For other FS, wait for TxOK and wait for Consecutive Frame */
    GET_MSG_RX(msgIdx)->Status = NWS_MSG_STATUS_TX_FC;
  }
  if(TX_N_PCI_IDX(msgIdx)==1)
    GET_CAN_TX(msgIdx)->Data[0] = GET_CAN_TX(msgIdx)->ExtraAddr;
  /* Write the N_PCI, BS and STmin fields of the Flow Control */
  GET_CAN_TX(msgIdx)->Data[TX_N_PCI_IDX(msgIdx)] = (U8)(N_PCI_FC | uFlowStatus);
  GET_CAN_TX(msgIdx)->Data[TX_N_PCI_IDX(msgIdx)+1] = GET_MSG_RX(msgIdx)->ubBs;
  GET_CAN_TX(msgIdx)->Data[TX_N_PCI_IDX(msgIdx)+2] = GET_MSG_RX(msgIdx)->ubSTmin;
  GET_CAN_TX(msgIdx)->DLC = TX_N_PCI_IDX(msgIdx)+3;
  /* BS parameter for Consecutive Frames reception */
  GET_MSG_RX(msgIdx)->ubBsCnt = GET_MSG_RX(msgIdx)->ubBs;
  /* Timeout and delay reception counter initialisation
  (there is no frame to wait before transmission confirmation) */
  GET_MSG_RX(msgIdx)->uCnt = 0;
  err = NWS_SendFrame(GET_CAN_TX(msgIdx));
  
  return err;
}

/**
* NWS_SendConsecutiveFrame
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_SendConsecutiveFrame(NWS_t *NWS, U8 msgIdx)
{
  U8 i;
  UDS_ReturnError err;
  /* The status is : Consecutive Frame transmission */
  GET_MSG_TX(msgIdx)->Status = NWS_MSG_STATUS_TX_CF;
  
  /* Timeout and delay transmission counter initialisation
  (there is no frame to wait before transmission confirmation) */
  GET_MSG_TX(msgIdx)->uCnt = 0;
  
  if(TX_N_PCI_IDX(msgIdx)==1)
    GET_CAN_TX(msgIdx)->Data[0] = GET_CAN_TX(msgIdx)->ExtraAddr;
  
  /* Write the N_PCI and SN fields of the Consecutive Frame */
  GET_CAN_TX(msgIdx)->Data[TX_N_PCI_IDX(msgIdx)] = (U8)(GET_MSG_TX(msgIdx)->ubSnCnt & 0x0FU | N_PCI_CF);
  
  /* Copy the TP message data to the temporary buffer for COM frame transmission
  (it is necessary to check the Consecutive Frame length) */
  for (i = 1; GET_MSG_TX(msgIdx)->uwLenCnt<GET_MSG_TX(msgIdx)->uwLen && i<GET_MSG_TX(msgIdx)->uMaxFFL; i++)
  {
    GET_CAN_TX(msgIdx)->Data[TX_N_PCI_IDX(msgIdx)+i] = GET_MSG_TX(msgIdx)->pData[GET_MSG_TX(msgIdx)->uBufferCnt++];
    
    /*Reload data buffer if it is empty */
    if(GET_MSG_TX(msgIdx)->uBufferCnt ==  GET_MSG_TX(msgIdx)->uwDataMax)
    {
      //TP_DataBufferEmptyInd (uMsgIdx);
      /*Restart form the begining of the buffer point by pstTpMsg */
      GET_MSG_TX(msgIdx)->uBufferCnt = 0;
    }
    
    /* Increment uwTxLenCnt */
    /* Shall not be done in the for(), MISRA restriction */
    GET_MSG_TX(msgIdx)->uwLenCnt++;
  }
  /* DLC */
  GET_CAN_TX(msgIdx)->DLC = TX_N_PCI_IDX(msgIdx)+i;
  
  /* SN parameter for Consecutive Frames transmission */
  GET_MSG_TX(msgIdx)->ubSnCnt++;
  
  /* FC Wait counter initialization */
  GET_MSG_TX(msgIdx)->ubRxFCWaitCnt = GET_MSG_TX(msgIdx)->ubRxFCWaitMax;
  
  /* Request COM frame transmission */
  err = NWS_SendFrame(GET_CAN_TX(msgIdx));
  /* After a Consecutive Frame correct transmission */
  if (err == UDS_ERROR_NO)
  {
	  /* All the message datas have been correctly transmitted
	  => transmission indication */
	  if (GET_MSG_TX(msgIdx)->uwLenCnt == GET_MSG_TX(msgIdx)->uwLen)
	  {
		  GET_MSG_TX(msgIdx)->Status = NWS_MSG_STATUS_NONE;
		  NWS_TxConfirm(NWS, msgIdx, N_OK);
	  }
	  /* All the message datas have been correctly transmitted
	  => message transmission continues */
	  else
	  {
		  /* The bloc is not complete, then Consecutive Frame transmission
		  after waiting the received STmin delay */
		  GET_MSG_TX(msgIdx)->ubBsCnt--;
      
		  if (GET_MSG_TX(msgIdx)->ubBs == 0 || GET_MSG_TX(msgIdx)->ubBsCnt>0)
		  {
			  /* The STmin is equal to 0, then the Consecutive Frame
			  is transmitted immediatly */
			  if (GET_MSG_TX(msgIdx)->ubSTmin == 0)
			  {
				  if (UDS_ERROR_NO != NWS_SendConsecutiveFrame(NWS, msgIdx))
				  {
					  NWS_AbortMsg(NWS, msgIdx);
					  NWS_TxConfirm(NWS, msgIdx, N_ERROR);
				  }
			  }
			  /* The STmin is different of 0, then wait for STmin delay */
			  else
			  {
				  GET_MSG_TX(msgIdx)->Status = NWS_MSG_STATUS_TX_CF_STMIN;
          
				  /* If inferior to 127ms, use the value */
				  if (GET_MSG_TX(msgIdx)->ubSTmin <= 0x7FU)
				  {
					  GET_MSG_TX(msgIdx)->uCnt = NWS_MS(GET_MSG_TX(msgIdx)->ubSTmin);
				  }
				  /* Range in steps of 100us */
				  else if (GET_MSG_TX(msgIdx)->ubSTmin >= 0xF1U && GET_MSG_TX(msgIdx)->ubSTmin <= 0xF9U)
				  {
					  GET_MSG_TX(msgIdx)->uCnt = NWS_MS(msgIdx);
				  }
				  /* Invalid range, use maximum value */
				  else
				  {
					  GET_MSG_TX(msgIdx)->uCnt = NWS_MS(0x7FU);
				  }
			  }
		  }
		  else
		  {
			  GET_MSG_TX(msgIdx)->Status = NWS_MSG_STATUS_RX_FC;
			  GET_MSG_TX(msgIdx)->uCnt = GET_MSG_TIM(msgIdx)->uNBs;
			  GET_MSG_TX(msgIdx)->ubRxFCWaitCnt = GET_MSG_TX(msgIdx)->ubRxFCWaitMax;
		  }
	  }
  }
  return err;
}

/**
* Receive a Single Frame
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_ReceiveSingleFrame(NWS_t *NWS, U8 msgIdx)
{
  U8 i;
  UDS_ReturnError err = UDS_ERROR_NO;
  if(GET_MSG_TX(msgIdx)->Status == NWS_MSG_STATUS_NONE)
  {
    /* If a reception is in progress, the previous reception is terminated */
    if(GET_MSG_RX(msgIdx)->Status != NWS_MSG_STATUS_NONE)
    {
      GET_MSG_RX(msgIdx)->Status = NWS_MSG_STATUS_NONE;
      NWS_RxIndication(NWS, msgIdx, N_UNEXP_PDU);
    }
    GET_MSG_RX(msgIdx)->uwLen = NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)] & 0x0F; /* SF_DL */
    for(i=0;i<GET_MSG_RX(msgIdx)->uwLen;i++)
    {
      GET_MSG_RX(msgIdx)->pData[i] = NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)+1+i];
    }
    NWS_ReceiveMessage_Do(NWS, msgIdx);
  }
  return err;
}

/**
* NWS_ReceiveFirstFrame
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Feb. 19th 2017
*/
UDS_ReturnError NWS_ReceiveFirstFrame(NWS_t *NWS, U8 msgIdx)
{
  U8 i;
  if(GET_MSG_TX(msgIdx)->Status == NWS_MSG_STATUS_NONE)
  {
    /* If a reception is in progress, the previous reception is terminated */
    if(GET_MSG_RX(msgIdx)->Status != NWS_MSG_STATUS_NONE)
    {
      GET_MSG_RX(msgIdx)->Status = NWS_MSG_STATUS_NONE;
      NWS_RxIndication(NWS, msgIdx, N_UNEXP_PDU);
    }
    
    if(NWS->pN_PDU_out->DLC==8 && NWS->pUDS_CFGS[msgIdx].pAFR->N_TAtype==PHY_ADDR)
    {
      GET_MSG_RX(msgIdx)->uwLenCnt = ((U16)(NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)]&0x0FU)<<8U)
        +(U16)(NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)+1]); /* FF_DL */
      
      if(GET_MSG_RX(msgIdx)->uwLenCnt>GET_MSG_RX(msgIdx)->uMaxSFDL)
      {
        /* The status is : First Frame reception */
        GET_MSG_RX(msgIdx)->Status = NWS_MSG_STATUS_RX_FF;
        /* Timeout and delay reception counter initialisation */
        /* (there is no frame to wait) */
        GET_MSG_RX(msgIdx)->uCnt = 0;
        if(GET_MSG_RX(msgIdx)->uwLenCnt<=GET_MSG_RX(msgIdx)->uwDataMax)
        {
          /* Message length memorisation */
          GET_MSG_RX(msgIdx)->uwLen = GET_MSG_RX(msgIdx)->uwLenCnt;
          /* The next frame that will be received is the first Consecutive Frame */
          GET_MSG_RX(msgIdx)->ubSnCnt = 1;
          for(i=0;i<GET_MSG_RX(msgIdx)->uMaxFFDL;i++)
          {
            GET_MSG_RX(msgIdx)->pData[i] = NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)+2+i];
          }
          /* Received data bytes number memorisation */
          GET_MSG_RX(msgIdx)->uwLenCnt = GET_MSG_RX(msgIdx)->uMaxFFDL;
          /* Send the Flow Control (Clear To Send) */
          if(UDS_ERROR_NO != NWS_SendFlowControl(NWS, msgIdx, NWS_FS_CLEAR_TO_SEND))
          {
            NWS_AbortMsg(NWS, msgIdx);
            NWS_RxIndication(NWS, msgIdx, N_ERROR);
          }
        }
        /* The reception buffer size is not sufficient to receive this message,
        then transmission of an Overflow Flow Control */
        else
        {
          /* Send the Flow Control (Overflow) */
          if(UDS_ERROR_NO != NWS_SendFlowControl(NWS, msgIdx, NWS_FS_OVERFLOW))
          {
            NWS_AbortMsg(NWS, msgIdx);
            NWS_RxIndication(NWS, msgIdx, N_ERROR);
          }
        }
      }else if (NWS->pUDS_CFGS[msgIdx].pAFR->Mtype == DIAG_TYPE)
      {
        /* A FF is received but the complete message data size is lower than 7. */
        /* Therefore, it should be managed with a SF */
        NWS_RxIndication(NWS, msgIdx, N_ERROR);
      }
    }else if (NWS->pUDS_CFGS[msgIdx].pAFR->Mtype == DIAG_TYPE)
    {
      NWS_RxIndication(NWS, msgIdx, N_ERROR);
    }
  }
  return UDS_ERROR_NO;
}

/**
* NWS_ReceiveConsecutiveFrame
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_ReceiveConsecutiveFrame(NWS_t *NWS, U8 msgIdx)
{
  U8 i;
  if(GET_MSG_TX(msgIdx)->Status == NWS_MSG_STATUS_NONE)
  {
    /* If a reception is in progress (that can be waiting for the TxOK 
    of a Flow Control or for the reception of a Consecutive Frame),
    this Consecutive Frame is used for the message reception */
    
    if (GET_MSG_RX(msgIdx)->Status!=NWS_MSG_STATUS_NONE &&
        GET_MSG_RX(msgIdx)->Status!=NWS_MSG_STATUS_TX_FC_OVERFLOW)
    {
      U16 uwLenRemain;
      uwLenRemain = GET_MSG_RX(msgIdx)->uwLen - GET_MSG_RX(msgIdx)->uwLenCnt;
      if(NWS->pN_PDU_out->DLC==8 || NWS->pN_PDU_out->DLC>=uwLenRemain+1)
      {
        /* If the SN field is correct */
        if ((GET_MSG_RX(msgIdx)->ubSnCnt&0x0FU) == (NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)] & 0x0FU))
        {
          /* The Status variable must not be put to the TP_MSG_STATUS_RX_CF value
          because it has been already done after a Flow Control transmission (COM_TpTxConf)
          But if the Consecutive Frame is received before the Flow Control TxConf
          (this is due to Software architecture), it must be done */
          GET_MSG_RX(msgIdx)->Status = NWS_MSG_STATUS_RX_CF;
          
          /* Timeout and delay reception counter initialisation */
          /* (there is no frame to wait) */
          GET_MSG_RX(msgIdx)->uCnt = 0;
          
          /* The next Consecutive Frame SN is calculated */
          GET_MSG_RX(msgIdx)->ubSnCnt ++;
          
          for(i=1; GET_MSG_RX(msgIdx)->uwLenCnt<GET_MSG_RX(msgIdx)->uwLen && i<GET_MSG_RX(msgIdx)->uMaxCFDL+1; i++)
          {
            /* The byte is copied */
            GET_MSG_RX(msgIdx)->pData[GET_MSG_RX(msgIdx)->uwLenCnt] = NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)+i];
            /* Increment uwRxLenCnt */
            /* Shall not be done in the for(), MISRA restriction */
            GET_MSG_RX(msgIdx)->uwLenCnt++;
          }
          
          /* All the data bytes have been received, end of message reception */
          if (GET_MSG_RX(msgIdx)->uwLenCnt == GET_MSG_RX(msgIdx)->uwLen)
          {
            /* End of message reception */
            GET_MSG_RX(msgIdx)->Status = NWS_MSG_STATUS_NONE;
            NWS_ReceiveMessage_Do(NWS, msgIdx);
          }
          /* There are other data bytes to receive */
          else
          {
            /* Next step is :
            Wait for another Consecutive Frame because
            the Flow Control is not required (1)
            or the bloc is not complete (2) */
            
            if (GET_MSG_RX(msgIdx)->ubBs != 0)
            {
              GET_MSG_RX(msgIdx)->ubBsCnt--;
            }
            
            if (GET_MSG_RX(msgIdx)->ubBs==0 || GET_MSG_RX(msgIdx)->ubBsCnt!=0)
            {
              /* Timeout and delay reception counter initialisation */
              /* (there is another Consecutive Frame to wait) */
              GET_MSG_RX(msgIdx)->uCnt = GET_MSG_TIM(msgIdx)->uNCr;
            }
            /* Transmit a Flow Control because the bloc is complete */
            else
            {
              /* Send the Flow Control (Clear To Send) */
              if(UDS_ERROR_NO != NWS_SendFlowControl(NWS, msgIdx, NWS_FS_CLEAR_TO_SEND))
              {
                NWS_AbortMsg(NWS, msgIdx);
                NWS_RxIndication(NWS, msgIdx, N_ERROR);
              }
            }
          }
        }
      }else if (NWS->pUDS_CFGS[msgIdx].pAFR->Mtype == DIAG_TYPE)
      {
        NWS_RxIndication(NWS, msgIdx, N_ERROR);
      }
    }
  }
  return UDS_ERROR_NO;
}

/**
* NWS_ReceiveFlowControl
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_ReceiveFlowControl(NWS_t *NWS, U8 msgIdx)
{
  /* If a reception is in progress (that can be waiting for the TxOK 
  of a Flow Control or for the reception of a Consecutive Frame),
  this Consecutive Frame is used for the message reception */
  if (GET_MSG_TX(msgIdx)->Status==NWS_MSG_STATUS_TX_FF || GET_MSG_TX(msgIdx)->Status==NWS_MSG_STATUS_RX_FC
      || (GET_MSG_TX(msgIdx)->Status!=NWS_MSG_STATUS_TX_FC_OVERFLOW && 
          GET_MSG_TX(msgIdx)->ubBs!=0 && GET_MSG_TX(msgIdx)->ubBsCnt==0))
  {
    if(NWS->pN_PDU_out->DLC==8 || NWS->pN_PDU_out->DLC>=GET_MSG_RX(msgIdx)->uMinFCDL)
    {
      /* The ebRxStatus variable must not be put to the TP_MSG_STATUS_RX_FC value
      because it has been already done after a First Frame or
      Consecutive Frame transmission (COM_TpTxConf) */
      /* Timeout and delay reception counter initialisation */
      /* (there is no frame to wait) */
      GET_MSG_TX(msgIdx)->uCnt = 0;
      
      U8 bFirstFC = UDS_FALSE;
      /* If it is the first Flow Control of the message transfer, */
      /* its parameters must be memorised for all this transfer */
      if (GET_MSG_TX(msgIdx)->uwLenCnt == GET_MSG_TX(msgIdx)->uMaxFFDL)
      {
        /* Memorise the Flow Control parameters for transmission */
        GET_MSG_TX(msgIdx)->ubBs = NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)+1];
        GET_MSG_TX(msgIdx)->ubSTmin = NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)+2];
        bFirstFC = UDS_TRUE;
      }        
      /* If it is not the first Flow Control of the message transfer,
      its parameters (BS and STmin) must be identical as in
      the first received Flow Control */
      else if (GET_MSG_TX(msgIdx)->ubBs==NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)+1] &&
               GET_MSG_TX(msgIdx)->ubSTmin==NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)+2])
      {
        bFirstFC = UDS_FALSE;
      }
      else
      {
        return UDS_ERROR_NO;
      }
      /* The Flow Status is ClearToSend */
      if((NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)]&0x0FU) == NWS_FS_CLEAR_TO_SEND)
      {
        /* It is the beginning of a Block */
        GET_MSG_TX(msgIdx)->ubBsCnt = GET_MSG_TX(msgIdx)->ubBs;
        
        /* Send a Consecutive Frame */
        if(UDS_ERROR_NO!=NWS_SendConsecutiveFrame(NWS, msgIdx))
        {
          NWS_AbortMsg(NWS, msgIdx);
          NWS_TxConfirm(NWS, msgIdx, N_ERROR);
        }
      }
      /* The Flow Status is Wait, then wait for another Flow Control */
      else if((NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)]&0x0FU) == NWS_FS_WAIT)
      {
        /* Timeout and delay reception counter initialisation */
        /* (there is a Flow Control to wait) */
        GET_MSG_TX(msgIdx)->uCnt = GET_MSG_TIM(msgIdx)->uNBs;
        
        /* Check maximum of FC Wait received */
        if ( GET_MSG_TX(msgIdx)->ubRxFCWaitCnt > 0 )
        {
          /* Decrement FC Wait counter */
          GET_MSG_TX(msgIdx)->ubRxFCWaitCnt--;
        }
        else
        {
          /* End of message transmission */
          GET_MSG_TX(msgIdx)->Status = NWS_MSG_STATUS_NONE;
          NWS_TxConfirm(NWS, msgIdx, N_WFT_OVRN);
        }
      }
      /* The Flow Status is Overflow, then end of message transmission */
      else if(bFirstFC==UDS_TRUE && (NWS->pN_PDU_out->Data[RX_N_PCI_IDX(msgIdx)]&0x0FU)==NWS_FS_OVERFLOW)
      {
        /* End of message transmission */
        /* (no interrupt protection is necessary because the uTxCnt variable
        is equal to 0 and TP_Manage does not read ebTxStatus in this case) */
        GET_MSG_TX(msgIdx)->Status = NWS_MSG_STATUS_NONE;
        
        /* Message transmission confirmation */
        NWS_TxConfirm(NWS, msgIdx, N_BUFFER_OVFLW);
      }
      /* The Flow Status is invalid, then end of message transmission */
      else
      {
        /* End of message transmission */
        /* (no interrupt protection is necessary because the uTxCnt variable
        is equal to 0 and TP_Manage does not read ebTxStatus in this case) */
        GET_MSG_TX(msgIdx)->Status = NWS_MSG_STATUS_NONE;
        
        /* Message transmission confirmation */
        NWS_TxConfirm(NWS, msgIdx, N_INVALID_FS);
      }
    }
    else if (NWS->pUDS_CFGS[msgIdx].pAFR->Mtype == DIAG_TYPE)
    {
      NWS_AbortMsg(NWS, msgIdx);
      NWS_TxConfirm(NWS, msgIdx, N_ERROR);
    }
  }
  return UDS_ERROR_NO;
}

/**
* NWS_InitMsg
*
* @param obj - Pointer to obj object <NWS_MSG_RX_t> or <NWS_MSG_TX_t>.
*        ADDR_TYPE - address type <ADDR_TYPE_t>.
*        dir - Message direction.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
static UDS_ReturnError NWS_InitMsg(void *obj, ADDR_TYPE_t ADDR_TYPE, U8 dir)
{
  if(dir==0)  /* Request */
  {
    NWS_MSG_RX_t *MSG;
    /* this is the correct pointer type of the first argument */
    MSG = (NWS_MSG_RX_t*)obj;
    if(MSG->Status == NWS_MSG_STATUS_UNINIT)
    {
      switch(ADDR_TYPE)
      {
      case NORMAL_ADDR: case NORMAL_FIXED_ADDR:
        MSG->uMaxSFDL = 7;
        MSG->uMaxFFDL = 6;
        MSG->uMaxCFDL = 7;
        MSG->uMaxFCDL = 0;
        MSG->uMinFCDL = 2;
        MSG->uMaxSFL = 8;
        MSG->uMaxFFL = 8;
        MSG->uMaxCFL = 8;
        MSG->uMinCFL = 2;
        MSG->uFCL = 2;
        break;
      case EXTENDED_ADDR: case EXTID_MIXED_ADDR: case STDID_MIXED_ADDR:
        MSG->uMaxSFDL = 6;
        MSG->uMaxFFDL = 5;
        MSG->uMaxCFDL = 6;
        MSG->uMaxFCDL = 0;
        MSG->uMinFCDL = 3;
        MSG->uMaxSFL = 8;
        MSG->uMaxFFL = 8;
        MSG->uMaxCFL = 8;
        MSG->uMinCFL = 3;
        MSG->uFCL = 3;
        break;
      }
      MSG->Status = NWS_MSG_STATUS_NONE;
    }
  }
  else  /* Response */
  {
    NWS_MSG_TX_t *MSG;
    /* this is the correct pointer type of the first argument */
    MSG = (NWS_MSG_TX_t*)obj;
    if(MSG->Status == NWS_MSG_STATUS_UNINIT)
    {
      switch(ADDR_TYPE)
      {
      case NORMAL_ADDR: case NORMAL_FIXED_ADDR:
        MSG->uMaxSFDL = 7;
        MSG->uMaxFFDL = 6;
        MSG->uMaxCFDL = 7;
        MSG->uMaxFCDL = 0;
        
        MSG->uMaxSFL = 8;
        MSG->uMaxFFL = 8;
        MSG->uMaxCFL = 8;
        MSG->uMinCFL = 2;
        MSG->uFCL = 2;
        break;
      case EXTENDED_ADDR: case EXTID_MIXED_ADDR: case STDID_MIXED_ADDR:
        MSG->uMaxSFDL = 6;
        MSG->uMaxFFDL = 5;
        MSG->uMaxCFDL = 6;
        MSG->uMaxFCDL = 0;
        
        MSG->uMaxSFL = 8;
        MSG->uMaxFFL = 8;
        MSG->uMaxCFL = 8;
        MSG->uMinCFL = 3;
        MSG->uFCL = 3;
        MSG->Status = NWS_MSG_STATUS_NONE;
        break;
      }
      MSG->Status = NWS_MSG_STATUS_NONE;
    }
  }
  return UDS_ERROR_NO;
}

/**
* Initializes NWS
*
* @param ppNWS - Pointer to address of NWS object <NWS_t>.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_Init(NWS_t **ppNWS)
{
  U8 i;
  U32 Ident;
  NWS_t *NWS;
  (*ppNWS) = &NWS_handles;
  NWS = *ppNWS; //pointer to (newly created) object
  UDS_ReturnError err=UDS_ERROR_NO;
  /* Initialize Ring Buffer */
  for(i=0;i<N_PDU_RING_BUF_SIZE;i++)
  {
    m_N_PDU[i].Renew = UDS_FALSE;
    m_N_PDU[i].next = &m_N_PDU[(i+1)%N_PDU_RING_BUF_SIZE];
  }
  NWS->pN_PDU_in = m_N_PDU;
  NWS->pN_PDU_out = m_N_PDU;
  NWS->pUDS_CFGS = UDS_CFGS_1;
  NWS->pUDS_RX_LONG_RSP = &UDS_RX_LONG_RSP;
  
  /* Register Services */
  for(i=0;i<NWS_MSG_MAX;i++)
  {
    if(NWS->pUDS_CFGS[i].pCANrxBuf == UDS_NULL)
      continue;
    switch(NWS->pUDS_CFGS[i].pAFR->ADDR_TYPE)
    {
    case NORMAL_ADDR:
      err = UDS_CANrxInit(NWS->pUDS_CFGS[i].pCANrxBuf, NWS->pUDS_CFGS[i].idxf,
                          NWS->pUDS_CFGS[i].pAFR->p.pDiag_N_AI_UID_N->Ident, 
                          NWS->pUDS_CFGS[i].pAFR->Mask,
                          0, NWS->pUDS_CFGS[i].pAFR->p.pDiag_N_AI_UID_N->bExtID, 
                          NWS, NWS_Receive);
      NWS->pUDS_CFGS[i].pCANrxBuf->N_PCI_OFFSET = 0;
      break;
    case NORMAL_FIXED_ADDR:
      Ident = (NWS->pUDS_CFGS[i].pAFR->p.pDiag_N_AI->PRIO << 26) +
        (NWS->pUDS_CFGS[i].pAFR->p.pDiag_N_AI->N_TA << 8) +
          NWS->pUDS_CFGS[i].pAFR->p.pDiag_N_AI->N_SA;
      switch(NWS->pUDS_CFGS[i].pAFR->N_TAtype)
      {
      case PHY_ADDR:
        Ident += NFA_N_TATYPE_PHY_VAL << 16;
        break;
      case FUN_ADDR:
        Ident += NFA_N_TATYPE_FUN_VAL << 16;
        break;
      default:
        return UDS_ERROR_ILLEGAL_ARGUMENT;
      }
      err = UDS_CANrxInit(NWS->pUDS_CFGS[i].pCANrxBuf, NWS->pUDS_CFGS[i].idxf,
                          Ident, 
                          NWS->pUDS_CFGS[i].pAFR->Mask,
                          0, UDS_TRUE, NWS, NWS_Receive);
      NWS->pUDS_CFGS[i].pCANrxBuf->N_PCI_OFFSET = 0;
      break;
    case EXTENDED_ADDR:
      err = UDS_CANrxInit(NWS->pUDS_CFGS[i].pCANrxBuf, NWS->pUDS_CFGS[i].idxf,
                          NWS->pUDS_CFGS[i].pAFR->p.pDiag_N_AI_UID_E->Ident, 
                          NWS->pUDS_CFGS[i].pAFR->Mask,
                          0, NWS->pUDS_CFGS[i].pAFR->p.pDiag_N_AI_UID_E->bExtID, 
                          NWS, NWS_Receive);
      NWS->pUDS_CFGS[i].pCANrxBuf->N_PCI_OFFSET = 1;
      NWS->pUDS_CFGS[i].pCANrxBuf->ExtraAddr = NWS->pUDS_CFGS[i].pAFR->p.pDiag_N_AI_UID_E->N_TA;
      break;
    case EXTID_MIXED_ADDR:
      Ident = (NWS->pUDS_CFGS[i].pAFR->p.pR_Diag_N_AI->PRIO << 26) +
        (NWS->pUDS_CFGS[i].pAFR->p.pR_Diag_N_AI->N_TA << 8) +
          NWS->pUDS_CFGS[i].pAFR->p.pR_Diag_N_AI->N_SA;
      switch(NWS->pUDS_CFGS[i].pAFR->N_TAtype)
      {
      case PHY_ADDR:
        Ident += EMA_N_TATYPE_PHY_VAL << 16;
        break;
      case FUN_ADDR:
        Ident += EMA_N_TATYPE_FUN_VAL << 16;
        break;
      default:
        return UDS_ERROR_ILLEGAL_ARGUMENT;
      }
      err = UDS_CANrxInit(NWS->pUDS_CFGS[i].pCANrxBuf, NWS->pUDS_CFGS[i].idxf,
                          Ident, 
                          NWS->pUDS_CFGS[i].pAFR->Mask,
                          0, UDS_TRUE, NWS, NWS_Receive);
      NWS->pUDS_CFGS[i].pCANrxBuf->N_PCI_OFFSET = 1;
      NWS->pUDS_CFGS[i].pCANrxBuf->ExtraAddr = NWS->pUDS_CFGS[i].pAFR->p.pR_Diag_N_AI->N_AE;
      break;
    case STDID_MIXED_ADDR:
      err = UDS_CANrxInit(NWS->pUDS_CFGS[i].pCANrxBuf, NWS->pUDS_CFGS[i].idxf,
                          NWS->pUDS_CFGS[i].pAFR->p.pR_Diag_N_AI_UID->Ident, 
                          NWS->pUDS_CFGS[i].pAFR->Mask,
                          0, UDS_FALSE, 
                          NWS, NWS_Receive);
      NWS->pUDS_CFGS[i].pCANrxBuf->N_PCI_OFFSET = 1;
      NWS->pUDS_CFGS[i].pCANrxBuf->ExtraAddr = NWS->pUDS_CFGS[i].pAFR->p.pR_Diag_N_AI_UID->N_AE;
      break;
    default:
      return UDS_ERROR_ILLEGAL_ARGUMENT;
    }
    if(err!=UDS_ERROR_NO)
      break;
    NWS_InitMsg((void *)NWS->pUDS_CFGS[i].pNWS_MSG->pMSG_RX, 
                NWS->pUDS_CFGS[i].pAFR->ADDR_TYPE, 0);
    
    if(NWS->pUDS_CFGS[i].pCANtxBuf == UDS_NULL)
      continue;
    switch(NWS->pUDS_CFGS[i].pAFT->ADDR_TYPE)
    {
    case NORMAL_ADDR:
      NWS->pUDS_CFGS[i].pCANtxBuf->bExtID = NWS->pUDS_CFGS[i].pAFT->p.pDiag_N_AI_UID_N->bExtID;
      NWS->pUDS_CFGS[i].pCANtxBuf->Ident = NWS->pUDS_CFGS[i].pAFT->p.pDiag_N_AI_UID_N->Ident;
      NWS->pUDS_CFGS[i].pCANtxBuf->bRemote = UDS_FALSE;
      NWS->pUDS_CFGS[i].pCANtxBuf->N_PCI_OFFSET = 0;
      break;
    case NORMAL_FIXED_ADDR:
      Ident = (NWS->pUDS_CFGS[i].pAFT->p.pDiag_N_AI->PRIO << 26) +
        (NWS->pUDS_CFGS[i].pAFT->p.pDiag_N_AI->N_TA << 8) +
          NWS->pUDS_CFGS[i].pAFT->p.pDiag_N_AI->N_SA;
      switch(NWS->pUDS_CFGS[i].pAFT->N_TAtype)
      {
      case PHY_ADDR:
        Ident += NFA_N_TATYPE_PHY_VAL << 16;
        break;
      case FUN_ADDR:
        Ident += NFA_N_TATYPE_FUN_VAL << 16;
        break;
      default:
        return UDS_ERROR_ILLEGAL_ARGUMENT;
      }
      NWS->pUDS_CFGS[i].pCANtxBuf->bExtID = UDS_TRUE;
      NWS->pUDS_CFGS[i].pCANtxBuf->Ident = Ident;
      NWS->pUDS_CFGS[i].pCANtxBuf->bRemote = UDS_FALSE;
      NWS->pUDS_CFGS[i].pCANtxBuf->N_PCI_OFFSET = 0;
      break;
    case EXTENDED_ADDR:
      NWS->pUDS_CFGS[i].pCANtxBuf->bExtID = NWS->pUDS_CFGS[i].pAFT->p.pDiag_N_AI_UID_E->bExtID;
      NWS->pUDS_CFGS[i].pCANtxBuf->Ident = NWS->pUDS_CFGS[i].pAFT->p.pDiag_N_AI_UID_E->Ident;
      NWS->pUDS_CFGS[i].pCANtxBuf->bRemote = UDS_FALSE;
      NWS->pUDS_CFGS[i].pCANtxBuf->N_PCI_OFFSET = 1;
      NWS->pUDS_CFGS[i].pCANtxBuf->ExtraAddr = NWS->pUDS_CFGS[i].pAFT->p.pDiag_N_AI_UID_E->N_TA;
      break;
    case EXTID_MIXED_ADDR:
      Ident = (NWS->pUDS_CFGS[i].pAFT->p.pR_Diag_N_AI->PRIO << 26) +
        (NWS->pUDS_CFGS[i].pAFT->p.pR_Diag_N_AI->N_TA << 8) +
          NWS->pUDS_CFGS[i].pAFT->p.pR_Diag_N_AI->N_SA;
      switch(NWS->pUDS_CFGS[i].pAFT->N_TAtype)
      {
      case PHY_ADDR:
        Ident += EMA_N_TATYPE_PHY_VAL << 16;
        break;
      case FUN_ADDR:
        Ident += EMA_N_TATYPE_FUN_VAL << 16;
        break;
      default:
        return UDS_ERROR_ILLEGAL_ARGUMENT;
      }
      NWS->pUDS_CFGS[i].pCANtxBuf->bExtID = UDS_TRUE;
      NWS->pUDS_CFGS[i].pCANtxBuf->Ident = Ident;
      NWS->pUDS_CFGS[i].pCANtxBuf->bRemote = UDS_FALSE;
      NWS->pUDS_CFGS[i].pCANtxBuf->N_PCI_OFFSET = 1;
      NWS->pUDS_CFGS[i].pCANtxBuf->ExtraAddr = NWS->pUDS_CFGS[i].pAFT->p.pR_Diag_N_AI->N_AE;
      break;
    case STDID_MIXED_ADDR:
      NWS->pUDS_CFGS[i].pCANtxBuf->bExtID = UDS_FALSE;
      NWS->pUDS_CFGS[i].pCANtxBuf->Ident = NWS->pUDS_CFGS[i].pAFT->p.pR_Diag_N_AI_UID->Ident;
      NWS->pUDS_CFGS[i].pCANtxBuf->bRemote = UDS_FALSE;
      NWS->pUDS_CFGS[i].pCANtxBuf->N_PCI_OFFSET = 1;
      NWS->pUDS_CFGS[i].pCANtxBuf->ExtraAddr = NWS->pUDS_CFGS[i].pAFT->p.pR_Diag_N_AI_UID->N_AE;
      break;
    default:
      return UDS_ERROR_ILLEGAL_ARGUMENT;
    }
    NWS_InitMsg((void *)NWS->pUDS_CFGS[i].pNWS_MSG->pMSG_TX, 
                NWS->pUDS_CFGS[i].pAFT->ADDR_TYPE, 1);
  }
  return err;
}

/**
* CheckReceive
*
* @param NWS - Pointer to NWS object <NWS_t>.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_CheckReceive(NWS_t *NWS)
{
  if(NWS->pN_PDU_out->Renew == UDS_TRUE)
  {
    if(NWS->pN_PDU_out->idx<NWS_MSG_MAX && NWS->pUDS_RX_LONG_RSP->uRspPendingCnt==0)
    {
      U8 N_PCI = NWS->pN_PDU_out->Data[RX_N_PCI_IDX(NWS->pN_PDU_out->idx)] & N_PCI_TYPE_MASK;
      switch(N_PCI)
      {
        /* Single Frame reception */
      case N_PCI_SF:
        NWS_ReceiveSingleFrame(NWS, NWS->pN_PDU_out->idx);
        break;
        /* First Frame reception */
      case N_PCI_FF:
        NWS_ReceiveFirstFrame(NWS, NWS->pN_PDU_out->idx);
        break;
        /* Consecutive Frame reception */
      case N_PCI_CF:
        NWS_ReceiveConsecutiveFrame(NWS, NWS->pN_PDU_out->idx);
        break;
        /* Flow Control reception */
      case N_PCI_FC:
        NWS_ReceiveFlowControl(NWS, NWS->pN_PDU_out->idx);
        break;
        /* Unknown N_PDU reception : it is ignored */
      default :
        /* Nothing to do */
        break;
      }
    }
    NWS->pN_PDU_out->Renew = UDS_FALSE;
    /* point to next buffer */
    NWS->pN_PDU_out = NWS->pN_PDU_out->next;
  }
  return UDS_ERROR_NO;
}

/**
* CheckTimeout
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        timeStamp - Time interval.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_CheckTimeout(NWS_t *NWS, U32 timeStamp)
{
  U8 i;
  /* Decrease response pending counter to unlock reception channel, if required */
  if (NWS->pUDS_RX_LONG_RSP->uRspPendingCnt > 0)
  {
    NWS->pUDS_RX_LONG_RSP->nrc78TimeCnt += timeStamp;
    if(NWS->pUDS_RX_LONG_RSP->nrc78TimeCnt>=NWS->pUDS_RX_LONG_RSP->nrc78Time)
    {
      NWS_RxRequest(NWS, NWS->pUDS_RX_LONG_RSP->idx);
      NWS->pUDS_RX_LONG_RSP->nrc78TimeCnt = 0;
    }
    /* Callback must be managed before setting counter to 0 */
    NWS->pUDS_RX_LONG_RSP->uRspPendingCnt>timeStamp?(NWS->pUDS_RX_LONG_RSP->uRspPendingCnt-=timeStamp):(NWS->pUDS_RX_LONG_RSP->uRspPendingCnt=0);
  }
  for(i=0; i<NWS_MSG_MAX; i++)
  {
    /* If a reception is in progress (that can be waiting for the TxOK 
    of a Flow Control or for the reception of a Consecutive Frame),
    this Consecutive Frame is used for the message reception */
    if(GET_MSG_RX(i)->Status==NWS_MSG_STATUS_RX_CF)
    {
      if(GET_MSG_RX(i)->uCnt == 0)
      { 
        GET_MSG_RX(i)->Status = NWS_MSG_STATUS_NONE;
        NWS_RxIndication(NWS, i, N_TIMEOUT_Cr);
      }
      GET_MSG_RX(i)->uCnt>timeStamp?(GET_MSG_RX(i)->uCnt-=timeStamp):(GET_MSG_RX(i)->uCnt=0);
    }
    /* The timeout for Flow Control reception is elapsed
    (after a First Frame or Consecutive Frame transmission)
    => error, then message transmission stop */
    if(GET_MSG_TX(i)->Status==NWS_MSG_STATUS_RX_FC || GET_MSG_TX(i)->Status == NWS_MSG_STATUS_TX_CF_STMIN)
    {
      if(GET_MSG_TX(i)->uCnt == 0 && GET_MSG_TX(i)->Status==NWS_MSG_STATUS_RX_FC)
      { 
        GET_MSG_TX(i)->Status = NWS_MSG_STATUS_NONE;
        NWS_TxConfirm(NWS, i, N_TIMEOUT_Bs);
      }
      GET_MSG_TX(i)->uCnt>timeStamp?(GET_MSG_TX(i)->uCnt-=timeStamp):(GET_MSG_TX(i)->uCnt=0);
      
    }
  }
  return UDS_ERROR_NO;
}

/**
* CheckTransmit
*
* @param NWS - Pointer to NWS object <NWS_t>.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 21th 2017
*/
UDS_ReturnError NWS_CheckTransmit(NWS_t *NWS)
{
  U8 i;
  if (NWS->pUDS_RX_LONG_RSP->uRspPendingCnt > 0)
  {
    UDS_LongRxIndication(NWS);
  }
  for(i=0; i<NWS_MSG_MAX; i++)
  {
    if(GET_MSG_TX(i)->Status == NWS_MSG_STATUS_TX_READY)
    {
      if (UDS_ERROR_NO != NWS_SendMsgData(NWS, i))
      {
        NWS_AbortMsg(NWS, i);
        NWS_TxConfirm(NWS, i, N_ERROR);
      }
    }
    /* The minimal delay between two Consecutive Frames (STmin) is elapsed */
    /* => next Consecutive Frame transmission */
    if (GET_MSG_TX(i)->Status == NWS_MSG_STATUS_TX_CF_STMIN)
    {
      if (GET_MSG_TX(i)->uCnt == 0)
      {
        if (UDS_ERROR_NO!=NWS_SendConsecutiveFrame(NWS, i))
        {
          NWS_AbortMsg(NWS, i);
          NWS_TxConfirm(NWS, i, N_ERROR);
        }
      }
    }
  }
  return UDS_ERROR_NO;
}

/**
* Processing NWS
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        timeStamp - Time interval.
* @return UDS_ReturnError:
*         UDS_ERROR_NO - Operation completed successfully.
* @date Jan. 16th 2017
*/
UDS_ReturnError NWS_Process(NWS_t *NWS, U32 timeStamp)
{
  UDS_ReturnError err = UDS_ERROR_NO;
  NWS_CheckReceive(NWS);
  NWS_CheckTransmit(NWS);
  NWS_CheckTimeout(NWS, timeStamp);
  return err;
}

/**
* Processing a message
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @date Jan. 18th 2017
*/
void NWS_ReceiveMessage_Do(NWS_t *NWS, U8 msgIdx)
{
  ReceiveMessageHook();
  U8 NRC = SERV_NOT_SUP; /* Service Not Supported */
  U8 errCode;
  UDS_CONST_STORAGE s_UDS_DSI *pDSI = Find_DSI(&NWS->pUDS_CFGS[msgIdx], GET_MSG_RX(msgIdx)->pData[0], &errCode);
  if (pDSI && pDSI->pFunct)
  {
    NRC = pDSI->pFunct(pDSI, GET_MSG_RX(msgIdx)->pData, &GET_MSG_RX(msgIdx)->uwLen);
    if(NRC==POS_RSP && GET_MSG_RX(msgIdx)->uwLen>GET_MSG_TX(msgIdx)->uwDataMax) /* >TX:uwDataMax */
      NRC = RSP_TOOLONG;
  }
  else if(errCode==FIND_ERR_COND)
  {
    NRC = COND_NOT_CORR;
  }
  else
  {
    /* Service Not Supported */
  }
#if UDS_MASTER == UDS_FALSE
  NWS_PesponseMessage(NWS, msgIdx, pDSI, NRC);
#endif
}

/**
* Pesponse a message
*
* @param NWS - Pointer to NWS object <NWS_t>.
*        msgIdx - Message index.
* @date Feb. 23th 2017
*/
void NWS_PesponseMessage(NWS_t *NWS, U8 msgIdx, UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 NRC)
{
  switch(NWS->pUDS_CFGS[msgIdx].pAFR->N_TAtype)
  {
  case PHY_ADDR: /* Refer to ISO 14229-1 Table 4 & Table 6 */
    if(NRC == POS_RSP) /* Positive Response */
    {
      if(pDSI->bSubFCN) /* SubFunction supported */
      {
        U8 SubFunc = GET_MSG_RX(msgIdx)->pData[1];
        /* suppressPosRspMsgIndicationBit */
        if(SubFunc&SPRS_POS_RSP_MSG_IND_BIT)
        {
          /* Positive Response is suppressed */
          break;
        }
        else
        {
          /* nothing to do */
          /* Positive Response will be send */
        }
      }
      else
      {
        /* nothing to do */
        /* Positive Response will be send */
      }
    }
    else if(NRC == REQ_CORR_RECV_RSP_PEND)
    {
      /* NRC_78 will be sent */
      NWS->pUDS_RX_LONG_RSP->idx = msgIdx;
      NWS->pUDS_RX_LONG_RSP->dsi = pDSI;
      NWS->pUDS_RX_LONG_RSP->uRspPendingCnt = NWS->pUDS_RX_LONG_RSP->uRspPendingMax;
    }
    else /* Negative Response */
    {
      U8 ServID = GET_MSG_RX(msgIdx)->pData[0];
      GET_MSG_RX(msgIdx)->pData[0] = UDS_NEG_RSP;
      GET_MSG_RX(msgIdx)->pData[1] = ServID;
      GET_MSG_RX(msgIdx)->pData[2] = NRC;
      GET_MSG_RX(msgIdx)->uwLen = 3;
    }
    /* Message reception indication */
    NWS_RxIndication(NWS, msgIdx, N_OK);
    break;
  case FUN_ADDR: /* Refer to ISO 14229-1 Table 5 & Table 7 */
    
    if(NRC == POS_RSP) /* Positive Response */
    {
      if(pDSI->bSubFCN) /* SubFunction supported */
      {
        U8 SubFunc = GET_MSG_RX(msgIdx)->pData[1];
        /* suppressPosRspMsgIndicationBit */
        if(SubFunc&SPRS_POS_RSP_MSG_IND_BIT)
        {
          /* Positive Response is suppressed */
          break;
        }
        else
        {
          /* nothing to do */
          /* Positive Response will be send */
        }
      }
      else
      {
        /* nothing to do */
        /* Positive Response will be send */
      }
    }
    else if(NRC == REQ_CORR_RECV_RSP_PEND)
    {
      /* NRC_78 will be sent */
      NWS->pUDS_RX_LONG_RSP->idx = msgIdx;
      NWS->pUDS_RX_LONG_RSP->dsi = pDSI;
      NWS->pUDS_RX_LONG_RSP->uRspPendingCnt = NWS->pUDS_RX_LONG_RSP->uRspPendingMax;
    }
    else if(NRC == SERV_NOT_SUP) /* Service Not Supported */
    {
      /* Skip send Negative Response */
      break;
    }
    else if(NRC == SUB_FUNC_NOT_SUP) /* Sub Function Not Supported - Invalid Format */
    {
      /* Skip send Negative Response */
      break;
    }
    else if(NRC == REQ_OR) /* Request Out Of Range */
    {
      /* Skip send Negative Response */
      break;
    }
    else /* Other Negative Response */
    {
      U8 ServID = GET_MSG_RX(msgIdx)->pData[0];
      GET_MSG_RX(msgIdx)->pData[0] = UDS_NEG_RSP;
      GET_MSG_RX(msgIdx)->pData[1] = ServID;
      GET_MSG_RX(msgIdx)->pData[2] = NRC;
      GET_MSG_RX(msgIdx)->uwLen = 3;
    }
    /* Message reception indication */
    NWS_RxIndication(NWS, msgIdx, N_OK);
    break;
  default:
    break;
  }
}
