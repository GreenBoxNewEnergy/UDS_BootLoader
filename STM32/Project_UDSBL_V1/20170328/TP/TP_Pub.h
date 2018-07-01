/*******************************************************************************
File - TP_Pub.h
This is common module for body netword
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: Byron
Date: 2017/01/01
Version: V1.0.0
*******************************************************************************/
#ifndef _TP_PUB_H_
#define _TP_PUB_H_
#include "stm32f10x.h"

/*******************************************************************************
*
*       Configuration Constants
*
*/

/* TP message external status */
#define TP_MSG_STATUS_OK                0U
#define TP_MSG_STATUS_NAS               1U
#define TP_MSG_STATUS_NBS               2U
#define TP_MSG_STATUS_NAR               3U
#define TP_MSG_STATUS_NCR               4U
#define TP_MSG_STATUS_NPDU              5U
#define TP_MSG_STATUS_SN                6U
#define TP_MSG_STATUS_FS                7U
#define TP_MSG_STATUS_WFT_OVRN          8U
#define TP_MSG_STATUS_OVFLOW            9U
#define TP_MSG_STATUS_NPCI              10U  /* Wrong NPCI frame type */
#define TP_MSG_STATUS_DLC               11U /* Wrong DLC */
#define TP_MSG_STATUS_COM_NOK           12U  /* COM layer reject transmission request */

/* Message mode */
#define TP_MSG_MODE                     0x80U
#define TP_MSG_MODE_HALF_DUPLEX         0x80U
#define TP_MSG_MODE_FULL_DUPLEX         0x00U

/* Message STmin configuration */
#define TP_MSG_STMIN                    0x40U
#define TP_MSG_STMIN_FULL               0x40U
#define TP_MSG_STMIN_PARTIAL            0x00U

/* Message type */
#define TP_MSG_TYPE                     0x20U
#define TP_MSG_TYPE_DIAG                0x20U
#define TP_MSG_TYPE_APP                 0x00U
/* Message buffer reloadable configuration */
#define TP_MSG_RELOAD                   0x08U
#define TP_MSG_RELOAD_TRUE              0x08U
#define TP_MSG_RELOAD_FALSE             0x00U

/* Message retransmission configuration */
#define TP_MSG_RETRANSMISSION           0x04U
#define TP_MSG_RETRANSMISSION_TRUE      0x04U
#define TP_MSG_RETRANSMISSION_FALSE     0x00U

/* Message padding configuration */
#define TP_MSG_PADDING                  0x02U
#define TP_MSG_PADDING_TRUE             0x02U
#define TP_MSG_PADDING_FALSE            0x00U

/* Number of authorised retransmission for a diagnostic message */
#define TP_MSG_RETRANSMISSION_MAX       2U

/* Value of the padding bytes */
#define TP_PADDING_BYTE_VALUE           0xAAU   //changed by bwf

/* Frame reception authorization for a TP channel */
#define TP_RX_ENABLE_ON                 0x00U
#define TP_RX_ENABLE_OFF                0x01U
#define TP_RX_ENABLE_MAX                0x02U

/*******************************************************************************
*
*       Public Types
*
*/

/* TP functions status */
#define TP_ERR_BASE                     0x50U
#define TP_ERR_OK                       COM_ERR_OK
#define TP_ERR_IDX                      ( TP_ERR_BASE + 0x00U )
#define TP_ERR_LEN                      ( TP_ERR_BASE + 0x01U )
#define TP_ERR_RX_DISABLED              ( TP_ERR_BASE + 0x02U )
#define TP_ERR_COM_OFF                  ( COM_ERR_OFF )
#define TP_ERR_COM_IDX                  ( COM_ERR_IDX )
#define TP_ERR_COM_FRAME_MODE           ( COM_ERR_FRAME_MODE )
#define TP_ERR_COM_TX_MSG_LOST          ( COM_ERR_TX_MSG_LOST )

/*******************************************************************************
*
*       Public Function Prototypes
*
*/

void TP_Init (void);
tTpStatus TP_RxEnable (tTpMsgIdx uMsgIdx, tTpRxEnable uRxEnable);
tTpStatus TP_SendMsg (tTpMsgIdx uMsgIdx);

#if (TP_DATA_COPY == TP_DATA_COPY_ADR)
    tTpStatus TP_SetMsgData (tTpMsgIdx uMsgIdx, u16 uwLen, u8 *paubData);
    tTpStatus TP_GetMsgData (tTpMsgIdx uMsgIdx, u16 *puwLen, u8 **paubData);
#endif

#if (TP_DATA_COPY == TP_DATA_COPY_BUF)
    tTpStatus TP_SetMsgDataCopy (tTpMsgIdx uMsgIdx, u16 uwLen, u8 *paubData);
    tTpStatus TP_GetMsgDataCopy (tTpMsgIdx uMsgIdx, u16 *puwLen, u8 *paubData);
#endif

tTpStatus TP_ReceiveFrame (tTpMsgIdx uMsgIdx);
tTpStatus TP_SetStmin (tTpMsgIdx uMsgIdx, u8 ubRxStmin);
tTpStatus TP_SetBS (tTpMsgIdx uMsgIdx, u8 ubRxBs);

tTpStatus TP_AbortMsg(tTpMsgIdx uMsgIdx);
#if (TP_TXRETRANSMISSION == TP_TXRETRANSMISSION_ACTIVATE)
#ifndef BOOTLOADER
tTpStatus TP_MonNodeStatus (tTpMsgIdx uMsgIdx, tNmMonStatus eNodeStatus);
#endif
#endif

void TP_Manage (void);
void TP_ComTxConf (tTpMsgIdx uMsgIdx);
void TP_ComNAsInd (tTpMsgIdx uMsgIdx);


#endif
