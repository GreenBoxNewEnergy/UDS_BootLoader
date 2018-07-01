/*******************************************************************************
File - TP_Prg.c
This is common module for body netword
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: Byron
Date: 2017/01/01
Version: V1.0.0
*******************************************************************************/
#include "TP_Types.h"
#include "TP_Cfg.h"
#include "TP_Pub.h"
#include "CAN_Types.h"
#include "CAN_Pub.h"

/*******************************************************************************
*
*       Private Types
*
*/

/* Frame N_PCI definition */
#define TP_NPCI                         0xF0U
#define TP_NPCI_SINGLE_FRAME            0x00U
#define TP_NPCI_FIRST_FRAME             0x10U
#define TP_NPCI_CONSECUTIVE_FRAME       0x20U
#define TP_NPCI_FLOW_CONTROL            0x30U

/* Frame length definition */
#define TP_LEN_SINGLE_FRAME             8U
#define TP_LEN_FIRST_FRAME              8U
#define TP_LEN_MIN_CONSECUTIVE_FRAME    2U
#define TP_LEN_MAX_CONSECUTIVE_FRAME    8U
#define TP_LEN_FLOW_CONTROL             3U

/* Data length definition */
#define TP_DATA_MAX_SINGLE_FRAME        7U
#define TP_DATA_MAX_FIRST_FRAME         6U
#define TP_DATA_MAX_CONSECUTIVE_FRAME   7U
#define TP_DATA_MAX_FLOW_CONTROL        0U

/* Flow Control Flow Status definition */
#define TP_FS_CLEAR_TO_SEND             0U
#define TP_FS_WAIT                      1U
#define TP_FS_OVERFLOW                  2U

#define TP_STMIN_MAX                    170U




/* TP message internal status */
#define TP_MSG_STATUS_NONE              0U

#define TP_MSG_STATUS_TX_SF             1U
#define TP_MSG_STATUS_TX_FF             2U
#define TP_MSG_STATUS_TX_CF             3U
#define TP_MSG_STATUS_TX_FC             4U

#define TP_MSG_STATUS_RX_SF             5U
#define TP_MSG_STATUS_RX_FF             6U
#define TP_MSG_STATUS_RX_CF             7U
#define TP_MSG_STATUS_RX_FC             8U

#define TP_MSG_STATUS_TX_FC_OVERFLOW    9U
#define TP_MSG_STATUS_TX_CF_STMIN       10U

/*-------------------------------------------------------------------------------*/
/* - Private Variables                                                           */

/* Transport Protocol messages */
tTpMsg m_astTpMsg[TP_MSG_MAX];
/* Transport Protocol counters */
tTpBufferData m_uTpBufferCnt[TP_MSG_MAX];

/* Rx enable counter */
/* If diffent from 0, the reception is not allowed for the channel. */
u16 m_aubRxEnableCnt[TP_MSG_MAX];

tTpStatus TP_SendSingleFrame(tTpMsgIdx uMsgIdx);

void TP_Init (void)
{
    tTpMsgIdx uMsgIdx;

    for (uMsgIdx = 0; uMsgIdx < TP_MSG_MAX; uMsgIdx ++)
    {
        /* Source code optimisation : pointer on TP message */
        tTpMsg *const pstTpMsg = &m_astTpMsg[uMsgIdx];

        /* Source code optimisation : pointer on TP message configuration */
        tTpMsgCfg const *const pstTpMsgCfg = &m_astTpMsgCfg[uMsgIdx];

        pstTpMsg->ebTxStatus = TP_MSG_STATUS_NONE;
        pstTpMsg->ebRxStatus = TP_MSG_STATUS_NONE;
        pstTpMsg->uTxCnt = 0;
        pstTpMsg->uRxCnt = 0;
        pstTpMsg->paubTxData = pstTpMsgCfg->paubTxData;
        pstTpMsg->paubRxData = pstTpMsgCfg->paubRxData;
        pstTpMsg->ubRxSTmin = pstTpMsgCfg->ubRxSTmin;
        pstTpMsg->ubRxBs = pstTpMsgCfg->ubRxBs;

        m_uTpBufferCnt[uMsgIdx] = 0;
        #if (TP_TXRETRANSMISSION == TP_TXRETRANSMISSION_ACTIVATE)
        #ifndef BOOTLOADER
        m_auNodeStatus[uMsgIdx] = NM_MON_NOK;
        m_aubTxCnt[uMsgIdx] = 0;
        #endif
        #endif
        m_aubRxEnableCnt[uMsgIdx] = 0;
    }
}

/**
 * Send a message using the Transport Protocol
 *
 * @param uMsgIdx - TP message index.
 * @return eStatus - function status:
 *		     - TP_ERR_OK - no error
 *    	   - TP_ERR_IDX - message index is out of range
 *    	   - TP_ERR_COM_OFF - transmission is not allowed
 *    	   - TP_ERR_COM_IDX - frame index is out of range
 *    	   - TP_ERR_COM_FRAME_MODE - frame is in periodic mode
 *    	   - TP_ERR_COM_TX_MSG_LOST - transmission is pending
 */
tTpStatus TP_SendMsg (tTpMsgIdx uMsgIdx)
{
    /* Return status (no initialization because value is alwais change before return) */
    tTpStatus eStatus;

    /* The message index input parameter is in expected range */
    if (uMsgIdx < TP_MSG_MAX)
    {
        /* If the message length is lower to TP_DATA_MAX_SINGLE_FRAME,
           transmit a Single Frame */
        if (m_astTpMsg[uMsgIdx].uwTxLen <= TP_DATA_MAX_SINGLE_FRAME)
        {
            eStatus = TP_SendSingleFrame (uMsgIdx);
        }
        /* If the message length is upper to TP_DATA_MAX_SINGLE_FRAME,
           transmit a First Frame followed by Consecutive Frames */
        else
        {
            eStatus = TP_SendFirstFrame (uMsgIdx);
        }

        /* Cancel segmentation if the COM layer rejects the transmission request */
        if (TP_MSG_STATUS_OK != eStatus)
        {
            (void)TP_AbortMsg(uMsgIdx);
            TP_TxConf (uMsgIdx, TP_MSG_STATUS_COM_NOK);
#if (TP_TXRETRANSMISSION == TP_TXRETRANSMISSION_ACTIVATE)
            TP_TxRetransmission(uMsgIdx, TP_MSG_STATUS_COM_NOK);
#endif
        }
    }

    /* The message index input parameter is out of range, then error */
    else
    {
        eStatus = TP_ERR_IDX;
    }


    /* Return the status */
    return (eStatus);
}

/**
 * Send a Single Frame
 *
 * @param uMsgIdx - TP message index.
 * @return eStatus - function status:
 *		     - TP_ERR_OK - no error
 *    	   - TP_ERR_CAN_OFF - transmission is not allowed
 *    	   - TP_ERR_CAN_IDX - frame index is out of range
 *    	   - TP_ERR_CAN_FRAME_MODE - frame is in periodic mode
 *    	   - TP_ERR_CAN_TX_MSG_LOST - transmission is pending
 */

tTpStatus TP_SendSingleFrame(tTpMsgIdx uMsgIdx)
{
  /* Temporary data buffer for TP frame constitution */
  u8 aubData[CAN_DATA_MAX];

  /* Index to use the data buffer */
  u8 ubIdx;
  
  {
    /* Source code optimisation : pointer on TP message */
    tTpMsg *const pstTpMsg = &m_astTpMsg[uMsgIdx];
    
    
    /* The status is : Single Frame transmission */
    pstTpMsg->ebTxStatus = TP_MSG_STATUS_TX_SF;
    
    /* Timeout and delay transmission counter initialisation */
    /* (there is no frame to wait) */
    pstTpMsg->uTxCnt = 0;
    
    
    /* Write the N_PCI and SF_DL fields of the Single Frame */
    aubData[0] = (u8)((pstTpMsg->uwTxLen & 0x0FU) | TP_NPCI_SINGLE_FRAME);
    
    /* Copy the TP message data to the temporary buffer for COM frame transmission */
    /* (there is no need to check that uwLen < TP_DATA_MAX_SINGLE_FRAME, because */
    /* this verification is done before TP_SendSingleFrame () function call) */
    for (ubIdx = 0; ubIdx < pstTpMsg->uwTxLen; ubIdx ++)
    {
      aubData[ubIdx + 1] = pstTpMsg->paubTxData[ubIdx];
    }
  }
  
  {
    tCanStatus eCanStatus;
    
    /* Source code optimisation : pointer on TP message transmission frame index */
    tComFrameIdx const *const puTxFrameIdx = (const tComFrameIdx *)(&m_astTpMsgCfg[uMsgIdx].uTxFrameIdx);
    
    
    /* Write the COM frame length (because it is dynamic) */
    /* (the ubIdx is equal to m_astTpMsg[uMsgIdx].uwLen after the for loop) */
    /* Frame length is only written when padding is not used */
    if ((m_astTpMsgCfg[uMsgIdx].ubConfig & TP_MSG_PADDING) == TP_MSG_PADDING_FALSE)
    {
#ifdef TP_COM_TX_DYNAMIC_FRAME_MAX
#if (TP_COM_TX_DYNAMIC_FRAME_MAX > 0U)
      eComStatus = COM_SetDynamicFrameLen (*puTxFrameIdx, (u8)(ubIdx + 1));
#else
      /* Initialization of eComStatus to avoid warning */
      eCanStatus = CAN_ERR_OK;
#endif
#else
#error TP_COM_TX_DYNAMIC_FRAME_MAX must be defined
#endif
    }
    else
    {
      /* to continue the program */
      eCanStatus = CAN_ERR_OK;
    }
    
    if(eCanStatus == CAN_ERR_OK)
    {
      /* Fill not used byte with padding value */
      /* ubIdx already points on the last databyte */
      for (; ubIdx < (CAN_DATA_MAX - 1); ubIdx++)
      {
        aubData[ubIdx + 1] = TP_PADDING_BYTE_VALUE;
      }
      /* Update the COM frame data buffer */
      eCanStatus = CAN_SetFrameData (*puTxFrameIdx, aubData);
      
      if(eCanStatus == CAN_ERR_OK)
      {
        /* Request COM frame transmission */
        eCanStatus = CAN_SendFrame (*puTxFrameIdx);
      }
    }
    
    return (tTpStatus)(eCanStatus);
    
  }
}
