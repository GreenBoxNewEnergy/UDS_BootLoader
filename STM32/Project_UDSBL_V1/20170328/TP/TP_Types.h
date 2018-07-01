/*******************************************************************************
File - TP_Types.h
This is common module for body netword
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: Byron
Date: 2017/01/01
Version: V1.0.0
*******************************************************************************/
#ifndef _TP_TYPES_H_
#define _TP_TYPES_H_
#include "stm32f10x.h"

    
    
#define U8_MAX          0xFFU
#define S8_MIN          (-127-1)
#define S8_MAX          127

#define U16_MAX         0xFFFFU
#define S16_MIN         (-32767-1)
#define S16_MAX         32767

#define U32_MAX         0xFFFFFFFFU
#define S32_MIN         (-2147483647-1)
#define S32_MAX         2147483647
/*******************************************************************************
*
*       Configuration Constants
*
*/

/* Timer period in milliseconds for the TP periodic task */
/* Range : (1 .. 10) */
#define TP_MANAGE_PERIOD                5U

/* Timer period in milliseconds for forbidden reception of frames */
#define TP_RX_ENABLE_CPT_VALUE          (u16)(1000U / TP_MANAGE_PERIOD)

/*Constant for precompile condition  */
/*utilisation of functions TP_SetMsgData and TP_GetMsgData */
#define TP_DATA_COPY_ADR                0
/*utilisation of functions TP_SetMsgDataCopy and TP_GetMsgDataCopy */
#define TP_DATA_COPY_BUF                1
#define TP_DATA_COPY                    TP_DATA_COPY_ADR

/* Maximum value of TP timeouts and delays */
/* Range : (0 .. 2^32) */
#define TP_TIMEOUT_MAX                  (150U / TP_MANAGE_PERIOD )

/*max data in Tp message */
#define TP_DIAG_MSG_DATA_MAX            300U

/* TP timeout and delay definition */
/* The type size depends on the number of managed messages */
#if (TP_TIMEOUT_MAX <= U8_MAX)
    typedef u8 tTpTimeout;
#elif (TP_TIMEOUT_MAX <= U16_MAX)
    typedef u16 tTpTimeout;
#elif (TP_TIMEOUT_MAX <= U32_MAX)
    typedef u32 tTpTimeout;
#endif

/* TX Retransmission */
#define TP_TXRETRANSMISSION_ACTIVATE            1U
#define TP_TXRETRANSMISSION_DEACTIVATE          0U

#define TP_TXRETRANSMISSION         TP_TXRETRANSMISSION_DEACTIVATE

    
/*******************************************************************************
*
*       Public Types
*
*/
/* Frame reception authorization for a TP channel */
typedef u8 tTpRxEnable;

/* TP functions status */
typedef u8 tTpStatus;

/* TP message index definition */
typedef u8 tTpMsgIdx;

/* TP Buffer data type de finition */
/* The type size TP buffer size */
#if (TP_DIAG_MSG_DATA_MAX <= U8_MAX)
    typedef u8 tTpBufferData;
#elif (TP_DIAG_MSG_DATA_MAX <= U16_MAX)
    typedef u16 tTpBufferData;
#endif

/* Transport Protocol message configuration, stored in Read-Only memory */
typedef struct
{
    u8          ebTxStatus;
    u8          ebRxStatus;
    u8          *paubTxData;       /* Pointer on the transmission data buffer */
    u8          *paubRxData;       /* Pointer on the reception data buffer */
    u16         uwTxLen;
    u16         uwTxLenCnt;
    u16         uwRxLen;
    u16         uwRxLenCnt;
    tTpTimeout  uTxCnt;
    tTpTimeout  uRxCnt;
    u8          ubRxBs;
    u8          ubRxBsCnt;
    u8          ubRxSnCnt;
    u8          ubTxSnCnt;
    u8          ubTxBs;
    u8          ubTxBsCnt;
    u8          ubTxSTmin;
    u8          ubRxSTmin;
    u8          ubRxFCWaitCnt;       /* Number of FCWait frame received in a row */
} tTpMsg;

typedef u32 tComFrameIdx;
/* Transport Protocol message configuration, stored in Read-Only memory */
typedef struct
{
    tComFrameIdx uTxFrameIdx;       /* COM frame index for transmission */
    tComFrameIdx uRxFrameIdx;       /* COM frame index for reception */
    u8           *paubTxData;       /* Pointer on the transmission data buffer */
    u8           *paubRxData;       /* Pointer on the reception data buffer */
    u16          uwDataMax;         /* Maximum data number for a transfer */
    tTpTimeout   uNBs;              /* Timeout between (Tx FF or Tx CF) and Rx FC */
    tTpTimeout   uNCr;              /* Timeout between (Tx FC or Rx CF) and Rx CF */
                                    /* the value 0 desactivates the timeout */
    u8           ubRxSTmin;         /* STmin value of the transmitted FC */
    u8           ubRxBs;            /* BS value of the transmitted FC */
    u8           ubConfig;          /* Config */
    u8           ubRxFCWaitMax;      /* Maximum number of FCWait frame received in a row */
} tTpMsgCfg;

#endif
