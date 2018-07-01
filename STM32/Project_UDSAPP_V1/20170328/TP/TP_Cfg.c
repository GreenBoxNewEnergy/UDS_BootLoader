/*******************************************************************************
File - TP_Cfg.h
This is common module for body netword
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: Byron
Date: 2017/01/01
Version: V1.0.0
*******************************************************************************/

#include "TP_Cfg.h"
#include "TP_Pub.h"
#define NCS_PRI_VAR


/* COM frames index definition (COM_Cfg.h) */
/**********************************************
*         Frames for configuration: 0*
**********************************************/
#define COM_FRAME_IDX_Diag_BCM_RQ_0x722                  0U
#define COM_FRAME_IDX_Diag_Fun_0x7DF                  1U
#define COM_FRAME_IDX_Diag_BCM_Rsp_0x723                  2U

NCS_PRI_VAR u8 aubDiag1Data[TP_DIAG_MSG_1_DATA_MAX];
NCS_PRI_VAR u8 aubDiag2Data[TP_DIAG_MSG_2_DATA_MAX];

const tTpMsgCfg m_astTpMsgCfg[TP_MSG_MAX] =
    {
     {COM_FRAME_IDX_Diag_BCM_Rsp_0x723, COM_FRAME_IDX_Diag_BCM_RQ_0x722, aubDiag1Data, aubDiag1Data, 300U, (150U / TP_MANAGE_PERIOD), (150U / TP_MANAGE_PERIOD), 1U , 0U, (TP_MSG_MODE_HALF_DUPLEX |TP_MSG_STMIN_FULL | TP_MSG_TYPE_DIAG | TP_MSG_RELOAD_FALSE | TP_MSG_PADDING_TRUE | TP_MSG_RETRANSMISSION_FALSE), 10U},
     {COM_FRAME_IDX_Diag_BCM_Rsp_0x723, COM_FRAME_IDX_Diag_Fun_0x7DF, aubDiag2Data, aubDiag2Data, 300U, (150U / TP_MANAGE_PERIOD), (150U / TP_MANAGE_PERIOD), 1U , 0U, (TP_MSG_MODE_HALF_DUPLEX |TP_MSG_STMIN_FULL | TP_MSG_TYPE_DIAG | TP_MSG_RELOAD_FALSE | TP_MSG_PADDING_TRUE | TP_MSG_RETRANSMISSION_FALSE), 10U},
    };


