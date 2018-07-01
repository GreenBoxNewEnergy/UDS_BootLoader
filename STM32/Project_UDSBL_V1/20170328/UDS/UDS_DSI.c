/*******************************************************************************
File - UDS_DSI.c
UDS Diagnostic Services Implementation.
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Feb. 16th 2017
Version: V1.0.0
*******************************************************************************/
#include "UDS_Driver.h"
#include "UDS_Config.h"
#include "UDS_Primitive.h"
#if USE_RD_DATA_BYID || USE_WR_DATA_BYID
#include "eeprom.h"
#endif
#if USE_REQ_DOWNLOAD || USE_XFER_DATA || USE_REQ_XFER_EXIT || USE_RT_ERASEMEM
#include "flash.h"
#endif
#if USE_XFER_DATA
#include "d3des.h"
#endif
#if USE_ROUTINE_CNTL && USE_RT_CHECKPROGINTEGRITY
#include "CRC32.h"
#endif
#if USE_SEC_ACCS
#include <stdlib.h>
#include "EncryptAlg1.h"
extern volatile U32 UDS_timer1ms; 
#endif

/**
* Find Diagnostic Services Implementation
*
* @param pCFGS - pointer to uds configuration object <s_UDS_CFGS>.
*        ServID - request service id.
* @return Pointer of type <s_UDS_DSI> pointing to found object, NULL if not found.
* @date Jan. 18th 2017
*/
UDS_CONST_STORAGE s_UDS_DSI *Find_DSI(UDS_CONST_STORAGE s_UDS_CFGS *pCFGS, U8 ServID, U8 *errCode)
{
  U8 i;
  UDS_CONST_STORAGE s_UDS_DSI *pDSI = UDS_NULL;
  for(i=0; i<pCFGS->DSI_MAX; i++)
  {
    if(pCFGS->pUDS_DSI[i].ServId == ServID)
    {
      pDSI = &pCFGS->pUDS_DSI[i];
      break;
    }
  }
  if(pDSI)
  {
    *errCode = FIND_ERR_NO;
    /* Grant the right to access the Service */
    if(UDS_RAM.Session==DFLT_SESSN && (pDSI->Access&ACCS_DFLT))
    {
    }
    else if(UDS_RAM.Session==ECU_PROG_SESSN && (pDSI->Access&ACCS_PROG))
    {
    }
    else if(UDS_RAM.Session==ECU_EXT_DIAG_SESSN && (pDSI->Access&ACCS_EXT_DIAG))
    {
    }
    else
    {
      *errCode = FIND_ERR_COND;
      return UDS_NULL;
    }
  }
  else
  {
    *errCode = FIND_ERR_NULL;
  }
  return pDSI;
}

/*******************************************************************************
*
*       Request Diagnostic Services Implementation
*
*/

/**
* DiagnosticSessionControl (10 hex)
*
* @param pDSI - Pointer to pDSI object <s_UDS_DSI>.
*        pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         COND_NOT_CORR - conditionsNotCorrect
*         SUB_FUNC_NOT_SUP - sub-functionNotSupported
* @date Feb. 16th 2017
*/
#if USE_DIAG_SESSN_CNTL
U8 DiagnosticSessionControl(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  if(*uwLen!=2)
    return INCORR_MSG_LEN_O_INVLD_FM;
  U8 SubFunc = pData[1]&SUB_FUNC_PARAM_MASK;
  switch(SubFunc)
  {
  case DFLT_SESSN:
    if(UDS_RAM.Session == ECU_PROG_SESSN)
    {
      /* softreset ECU when in ECUProgrammingSession */
#if USE_ECU_RST
      UDS_RAM.resetType = SOFT_RST;
#else
      /* Denied entry to defaultSession */
      return COND_NOT_CORR;
#endif
    }
    else
    {
      UDS_RAM.Session = DFLT_SESSN;
    }
    break;
  case ECU_PROG_SESSN:
    /* Under the ECUExtendedDiagnosticSession, 
    and is currently a secure access mode */
#if USE_ROUTINE_CNTL && USE_RT_CHECKPREPROG
    if(UDS_RAM.Session==ECU_EXT_DIAG_SESSN && UDS_RAM.PreProgCondition==1)
#else
    if(UDS_RAM.Session==ECU_EXT_DIAG_SESSN)
#endif
    {
      /* enter ECUProgrammingSession */
      UDS_RAM.Session = ECU_PROG_SESSN;
    }
    else if(UDS_RAM.Session != ECU_PROG_SESSN)
    {
      /* Denied entry to ECUProgrammingSession */
      return COND_NOT_CORR;
    }
    break;
  case ECU_EXT_DIAG_SESSN:
    if(UDS_RAM.Session == DFLT_SESSN)
    {
      UDS_RAM.Session = ECU_EXT_DIAG_SESSN;
    }
    else if(UDS_RAM.Session != ECU_EXT_DIAG_SESSN)
    {
      /* Denied entry to ECUExtendedDiagnosticSession */
      return COND_NOT_CORR;
    }
    break;
  default:
    return SUB_FUNC_NOT_SUP;
  }
  pData[0] = UDS_DIAG_SESSN_CNTL_RSP;
  pData[2] = UDS_RAM.P2CAN_Server_max>>8;
  pData[3] = UDS_RAM.P2CAN_Server_max&0xFF;
  pData[4] = UDS_RAM.EP2CAN_Server_max>>8;
  pData[5] = UDS_RAM.EP2CAN_Server_max&0xFF;
  *uwLen = 6;
  return POS_RSP;
}
#endif /* USE_DIAG_SESSN_CNTL */

/**
* ECUReset (11 hex)
*
* @param pDSI - Pointer to pDSI object <s_UDS_DSI>.
*        pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         SUB_FUNC_NOT_SUP - sub-functionNotSupported
* @date Jan. 18th 2017
*/
#if USE_ECU_RST
U8 ECUReset(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  if(*uwLen!=2)
    return INCORR_MSG_LEN_O_INVLD_FM;
  
  switch(pData[1])
  {
  case HARD_RST:
    UDS_RAM.resetType = HARD_RST;
    *uwLen = 2;
    break;
  case SOFT_RST:
    UDS_RAM.resetType = SOFT_RST;
    *uwLen = 2;
    break;
  default:
    return SUB_FUNC_NOT_SUP;
  }
  pData[0] = UDS_ECU_RST_RSP;
  return POS_RSP;
}
#endif /* USE_ECU_RST */

/**
* SecurityAccess (27 hex)
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         SUB_FUNC_NOT_SUP - sub-functionNotSupported
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         COND_NOT_CORR - conditionsNotCorrect
*         REQ_SEQ_ERR - requestSequenceError
*         REQ_OR - requestOutOfRange
*         INVLD_KEY - Invalid Key
*         EXCD_NUM_ATMP - exceededNumberOfAttempts
*         REQD_TD_NOT_EXPR - requiredTimeDelayNotExpired
* @date Feb. 17th 2017
*/
#if USE_SEC_ACCS
U8 SecurityAccess(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{ 
  U8 i;
  if(*uwLen!=6 && *uwLen!=2)
    return INCORR_MSG_LEN_O_INVLD_FM;
  U8 SubFunc = pData[1]&SUB_FUNC_PARAM_MASK;
  if(SubFunc==REQ_SEED_07)
  {
    if(*uwLen!=2)
      return INCORR_MSG_LEN_O_INVLD_FM;
    UDS_RAM.requestSeed = 0;
    srand(UDS_timer1ms);
    UDS_RAM.requestSeed = rand();
    pData[2] = (U8)(UDS_RAM.requestSeed>>24 & 0x000000FF);
    pData[3] = (U8)(UDS_RAM.requestSeed>>16 & 0x000000FF);
    pData[4] = (U8)(UDS_RAM.requestSeed>>8 & 0x000000FF);
    pData[5] = (U8)(UDS_RAM.requestSeed & 0x000000FF);
    UDS_RAM.requestSeedReceived = UDS_TRUE;
    UDS_RAM.numOfAttempts = 0;
    UDS_RAM.requiredTimeDelayCnt = 0;
    *uwLen = 6;
  }
  else if(SubFunc==SEND_KEY_08)
  {
    if(*uwLen!=6)
      return INCORR_MSG_LEN_O_INVLD_FM;
    if(UDS_RAM.requestSeedReceived == UDS_FALSE)
    {
      return REQ_SEQ_ERR;
    }
    if(UDS_RAM.requiredTimeDelayCnt > 0)
    {
      return REQD_TD_NOT_EXPR;
    }
    U32 sendKey = 0;
    for(i=0;i<4;i++)
    {
      sendKey<<=8;
      sendKey+=pData[i+2];
    }
    if(seedToKeyLevel1(UDS_RAM.requestSeed, ECU_MASK)==sendKey)
    {
      UDS_RAM.SecureAccess = 1;
      pData[2] = 0x34;
      *uwLen = 3;
    }
    else
    {
      if(UDS_RAM.numOfAttempts++ >= UDS_RAM.maxNumOfAttempts)
      {
        UDS_RAM.numOfAttempts = 0;
        UDS_RAM.requiredTimeDelayCnt = UDS_RAM.requiredTimeDelay;
        return EXCD_NUM_ATMP;
      }
      return INVLD_KEY;
    }
  }
  else
  {
    return SUB_FUNC_NOT_SUP;
  }
  pData[0] = UDS_SEC_ACCS_RSP;
  return POS_RSP;
}
#endif /* USE_SEC_ACCS */

/**
* CommunicationControl (28 hex)
*
* @param pDSI - Pointer to pDSI object <s_UDS_DSI>.
*        pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         SUB_FUNC_NOT_SUP - sub-functionNotSupported
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         COND_NOT_CORR - conditionsNotCorrect
*         REQ_OR - requestOutOfRange
* @date Jan. 19th 2017
*/
#if USE_COMM_CNTL
U8 CommunicationControl(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  U8 SubFunc = pData[1]&SUB_FUNC_PARAM_MASK; /* controlType */
  U8 communicationType;
  if(*uwLen==3 && (SubFunc!=EN_RX_DIS_TX_EAI && SubFunc!=EN_RX_TX_EAI))
  {
    communicationType = pData[2];
    if(SubFunc==EN_RX_TX)
    {
      if(communicationType==3)
      {
      }
      else
      {
        return REQ_OR;
      }
    }
    else if(SubFunc==DIS_RX_TX)
    {
      if(communicationType==3)
      {
      }
      else
      {
        return REQ_OR;
      }
    }
    else
    {
      return SUB_FUNC_NOT_SUP;
    }
  }
  else if(*uwLen==5 && (SubFunc==EN_RX_DIS_TX_EAI && SubFunc==EN_RX_TX_EAI))
  {
    return SUB_FUNC_NOT_SUP;
  }
  else
  {
    return INCORR_MSG_LEN_O_INVLD_FM;
  }
  pData[0] = UDS_COMM_CNTL_RSP;
  *uwLen = 2;
  return POS_RSP;
}
#endif /* USE_SEC_ACCS */

/**
* TesterPresent (3E hex)
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         SUB_FUNC_NOT_SUP - sub-functionNotSupported
* @date Feb. 19th 2017
*/
#if USE_TSTER_PRST
U8 TesterPresent(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  if(*uwLen!=2)
    return INCORR_MSG_LEN_O_INVLD_FM;
  U8 SubFunc = pData[1]&SUB_FUNC_PARAM_MASK;
  if(SubFunc==ZERO_SUB_FUNC)
  {
    /* To do something */
  }
  else
  {
    return SUB_FUNC_NOT_SUP;
  }
  pData[0] = UDS_TSTER_PRST_RSP;
  return POS_RSP;
}
#endif /* USE_TSTER_PRST */

/**
* AccessTimingParameter (83 hex)
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         SUB_FUNC_NOT_SUP - sub-functionNotSupported
* @date Mar. 15th 2017
*/
#if USE_ACCS_TIM_PARM
U8 AccessTimingParameter(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  if(*uwLen<2)
    return INCORR_MSG_LEN_O_INVLD_FM;
  U8 SubFunc = pData[1]&SUB_FUNC_PARAM_MASK;
  if(SubFunc==RD_EXT_TIM_PARAMSET)
  {
    /* To do something */
  }
  else if(SubFunc==SET_TIM_PARAM_TO_DFLT_VAL)
  {
    /* To do something */
  }
  else if(SubFunc==RD_CURR_ACT_TIM_PARAM)
  {
    /* To do something */
  }
  else if(SubFunc==SET_TIM_PARAM_TO_GIVEN_VAL)
  {
    /* To do something */
  }
  else
  {
    return SUB_FUNC_NOT_SUP;
  }
  pData[0] = UDS_ACCS_TIM_PARM_RSP;
  return POS_RSP;
}
#endif /* USE_ACCS_TIM_PARM */

/**
* SecuredDataTransmission (84 hex)
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
* @date Mar. 15th 2017
*/
#if USE_SEC_DATA_TX
U8 SecuredDataTransmission(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  if(*uwLen<1)
    return INCORR_MSG_LEN_O_INVLD_FM;
  /* To do something */
  pData[0] = UDS_SEC_DATA_TX_RSP;
  return POS_RSP;
}
#endif /* USE_SEC_DATA_TX */

/**
* ControlDTCSetting (85 hex)
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         COND_NOT_CORR - conditionsNotCorrect
*         REQ_OR - requestOutOfRange
* @date Jan. 19th 2017
*/
#if USE_CNTL_DTC_SET
U8 ControlDTCSetting(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  if(*uwLen!=2)
    return INCORR_MSG_LEN_O_INVLD_FM;
  U8 SubFunc = pData[1]&SUB_FUNC_PARAM_MASK;
  if(SubFunc==DTC_SET_ON)
  {
    UDS_RAM.DTCSet = SubFunc;
  }
  else if(SubFunc==DTC_SET_OFF)
  {
    UDS_RAM.DTCSet = SubFunc;
  }
  else
  {
    return REQ_OR;
  }
  pData[0] = UDS_CNTL_DTC_SET_RSP;
  *uwLen = 2;
  return POS_RSP;
}
#endif /* USE_CNTL_DTC_SET */

/**
* ResponseOnEvent (86 hex)
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         COND_NOT_CORR - conditionsNotCorrect
*         REQ_OR - requestOutOfRange
* @date Mar. 15th 2017
*/
#if USE_RESP_ON_EVNT
U8 ResponseOnEvent(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  if(*uwLen<2)
    return INCORR_MSG_LEN_O_INVLD_FM;
  U8 SubFunc = pData[1]&SUB_FUNC_PARAM_MASK;
  if(SubFunc==DONOT_STORE_EVENT)
  {
    /* To do something */
  }
  else if(SubFunc==STORE_EVENT)
  {
    /* To do something */
  }
  else
  {
    return REQ_OR;
  }
  pData[0] = UDS_RESP_ON_EVNT_RSP;
  return POS_RSP;
}
#endif /* USE_RESP_ON_EVNT */

/**
* LinkControl (87 hex)
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         COND_NOT_CORR - conditionsNotCorrect
*         REQ_OR - requestOutOfRange
* @date Mar. 15th 2017
*/
#if USE_LINK_CNTL
U8 LinkControl(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  if(*uwLen<2)
    return INCORR_MSG_LEN_O_INVLD_FM;
  U8 SubFunc = pData[1]&SUB_FUNC_PARAM_MASK;
  if(SubFunc==VERF_BAUDTX_WITH_FIXBAUD)
  {
    /* To do something */
  }
  else if(SubFunc==VERF_BAUDTX_WITH_SPECBAUD)
  {
    /* To do something */
  }
  else
  {
    return REQ_OR;
  }
  pData[0] = UDS_LINK_CNTL_RSP;
  return POS_RSP;
}
#endif /* USE_LINK_CNTL */

/**
* ReadDataByIdentifier (22 hex)
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         RSP_TOOLONG - responseTooLong
*         COND_NOT_CORR - conditionsNotCorrect
*         REQ_OR - requestOutOfRange
*         SCY_ACCS_DENY - securityAccessDenied
* @date Jan. 19th 2017
*/
#if USE_RD_DATA_BYID
U8 ReadDataByIdentifier(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  U8 NRC;
  U8 errCode;
  if(*uwLen!=3)
    return INCORR_MSG_LEN_O_INVLD_FM;
  U16 dataIdentifier = ((U16)pData[1]<<8)+pData[2];
  UDS_CONST_STORAGE RDBI_t *pRdbi = Find_Rdbi(dataIdentifier, &errCode);
  if(pRdbi && pRdbi->pFunct)
  {
    NRC = pRdbi->pFunct(pData, uwLen);
  }
  else if(errCode==FIND_ERR_COND)
  {
    return COND_NOT_CORR;
  }
  else
  {
    return REQ_OR; 
  }
  if(NRC==POS_RSP)
    pData[0] = UDS_RD_DATA_BYID_RSP;
  return NRC;
}
#endif /* USE_RD_DATA_BYID */

/**
* WriteDataByIdentifier
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         COND_NOT_CORR - conditionsNotCorrect
*         REQ_OR - requestOutOfRange
*         SCY_ACCS_DENY - securityAccessDenied
*         GEN_PROG_FAIL - generalProgrammingFailure
* @date Jan. 19th 2017
*/
#if USE_WR_DATA_BYID
U8 WriteDataByIdentifier(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  U8 NRC;
  U8 errCode;
  if(*uwLen<=3)
    return INCORR_MSG_LEN_O_INVLD_FM;
  U16 dataIdentifier = ((U16)pData[1]<<8)+(U16)pData[2];
  UDS_CONST_STORAGE WDBI_t *pWdbi = Find_Wdbi(dataIdentifier, &errCode);
  if(pWdbi && pWdbi->pFunct)
  {
    if(*uwLen != pWdbi->Len+3)
      return INCORR_MSG_LEN_O_INVLD_FM;
    NRC = pWdbi->pFunct(pData, uwLen);
  }
  else if(errCode==FIND_ERR_COND)
  {
    return COND_NOT_CORR;
  }
  else
  {
    return REQ_OR; 
  }
  if(NRC==POS_RSP)
    pData[0] = UDS_WR_DATA_BYID_RSP;
  return NRC;
}
#endif /* USE_WR_DATA_BYID */

/**
* ClearDiagnosticInformation (14 hex)
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         REQ_OR - requestOutOfRange
* @date Mar. 23th 2017
*/
#if USE_CLR_DIAG_INFO
U8 ClearDiagnosticInformation(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  if(*uwLen!=4)
    return INCORR_MSG_LEN_O_INVLD_FM;
  if(pData[1]!=0xFF && pData[2]!=0xFF &&pData[3]!=0xFF)
    return REQ_OR;
  pData[0] = UDS_CLR_DIAG_INFO_RSP;
  return POS_RSP;
}
#endif

/**
* RoutineControl (31 hex)
*
* @param pDSI - Pointer to pDSI object <s_UDS_DSI>.
*        pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         SUB_FUNC_NOT_SUP - sub-functionNotSupported
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         COND_NOT_CORR - conditionsNotCorrect
*         REQ_OR - requestOutOfRange
*         SCY_ACCS_DENY - securityAccessDenied
*         GEN_PROG_FAIL - GeneralProgrammingFailure
* @date Jan. 18th 2017
*/
#if USE_ROUTINE_CNTL
U8 RoutineControl(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
  U8 NRC;
  U8 errCode;
  U16 routineIdentifier = ((U16)pData[2]<<8)+(U16)pData[3];
  UDS_CONST_STORAGE Routine_t *pRoutine = Find_Routine(routineIdentifier, &errCode);
  if(pRoutine && pRoutine->pFunct)
  {
    /* 1. The routine does not need to secure access,
    2. The routine need to secure access and 
    the server¡¯s security feature is currently active */
    
#if USE_SEC_ACCS
    if(pRoutine->bSecureAccess==0 || (pRoutine->bSecureAccess==1&&UDS_RAM.SecureAccess==1))
#else
    if(1)
#endif
    {
      NRC = pRoutine->pFunct(pData, uwLen);
    }
    else
    {
      return SCY_ACCS_DENY;
    }
  }
  else if(errCode==FIND_ERR_COND)
  {
    return COND_NOT_CORR;
  }
  else
  {
    return SUB_FUNC_NOT_SUP; 
  }
  if(NRC==POS_RSP)
    pData[0] = UDS_ROUTINE_CNTL_RSP;
  return NRC;
}
#endif /* USE_ROUTINE_CNTL */

/**
* RequestDownload (34 hex)
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         COND_NOT_CORR - conditionsNotCorrect
*         REQ_OR - requestOutOfRange
*         SCY_ACCS_DENY - securityAccessDenied
*         UL_DL_NOT_ACCEPT - uploadDownloadNotAccepted
* @date Feb. 20th 2017
*/
#if USE_REQ_DOWNLOAD
U8 RequestDownload(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
#if USE_SEC_ACCS
  if(UDS_RAM.SecureAccess==0)
    return SCY_ACCS_DENY;
#endif
  if(*uwLen<5 || *uwLen>11)
    return INCORR_MSG_LEN_O_INVLD_FM;
#if USE_ROUTINE_CNTL && USE_RT_ERASEMEM
  if(UDS_RAM.FlashEraseFlag)
    return UL_DL_NOT_ACCEPT;
#endif
  U8 i;
  /* dataFormatIdentifier 
  The high nibble specifies the "compressionMethod", 
  and the low nibble specifies the "encryptingMethod". */
  U8 DFI = pData[1];
  if(DFI!=0x00 && DFI!=0x01)
    return REQ_OR;
  UDS_RAM.encryptingMethod = DFI&0x0F;
  U8 ALFID = pData[2]; /* addressAndLengthFormatIdentifier */
  U8 SizeLen = ALFID>>4&0x0F;
  U8 AddressLen = ALFID&0x0F;
  if(SizeLen<1 || SizeLen>4 || AddressLen<1 || AddressLen>4)
    return REQ_OR;
  if(*uwLen!=3+SizeLen+AddressLen)
    return INCORR_MSG_LEN_O_INVLD_FM;
  U32 memoryAddress=0, memorySize=0;
  for(i=0;i<AddressLen;i++)
  {
    memoryAddress<<=8;
    memoryAddress+=pData[i+3]; /* memoryAddress */
  }
  for(i=0;i<SizeLen;i++)
  {
    memorySize<<=8;
    memorySize+=pData[i+3+AddressLen]; /* memorySize */
  }
  if(UDS_RAM.encryptingMethod==1 && memorySize%8)
    return REQ_OR;
  if(memoryAddress == UDS_ROM.driverAddress)
  {
    UDS_RAM.dlType = DL_TYPE_DRIVER;
    if(memorySize>UDS_ROM.driverSizeMax)
      return REQ_OR;
  }
  else if(memoryAddress == UDS_ROM.progAddress)
  {
    UDS_RAM.dlType = DL_TYPE_PROG;
    if(memorySize>UDS_ROM.progSizeMax || memorySize==0)
      return REQ_OR;
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
  }
  else
  {
    return REQ_OR; 
  }
  UDS_RAM.memorySize = memorySize;
  UDS_RAM.memorySizeCnt = 0;
  UDS_RAM.blockSequenceCnt = 1;
  UDS_RAM.UL_DL_Step = STEP_REQ_DL;
  TotalSize = memorySize;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
  /* Reset CRC generator */  
  CRC_ResetDR();
  pData[0] = UDS_REQ_DOWNLOAD_RSP;
  U8 maxNumberOfBlockLength = sizeof(BlockLen_t);
  U8 LFID = maxNumberOfBlockLength<<4;
  pData[1] = LFID; /* lengthFormatIdentifier */
  U32 blockLength = UDS_ROM.blockLengthMax; /* maxNumberOfBlockLength */
  for(i=0;i<maxNumberOfBlockLength;i++)
  {
    pData[2+i] = (U8)(blockLength>>(maxNumberOfBlockLength-1-i)*8);
  }
  *uwLen = 2+maxNumberOfBlockLength;
  return POS_RSP;
}
#endif /* USE_REQ_DOWNLOAD */

/**
* TransferData (36 hex)
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         REQ_SEQ_ERR - requestSequenceError
*         REQ_OR - requestOutOfRange
*         XFER_DATA_SUS - transferDataSuspended
*         GEN_PROG_FAIL - GeneralProgrammingFailure
*         WRONG_BLOCK_SEQ_COUNT - wrongBlockSequenceCounter
*         VOLT_TOO_HIGH - voltageTooHigh
*         VOLT_TOO_LOW - voltageTooLow
* @date Feb. 19th 2017
*/
#if USE_XFER_DATA
U8 TransferData(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
#if USE_SEC_ACCS
  if(UDS_RAM.SecureAccess==0)
    return SCY_ACCS_DENY;
#endif
  if(*uwLen!=UDS_ROM.blockLengthMax && *uwLen!=2+UDS_RAM.memorySize-UDS_RAM.memorySizeCnt)
    return INCORR_MSG_LEN_O_INVLD_FM;
  if(STEP_REQ_DL == UDS_RAM.UL_DL_Step)
  {
    if(UDS_RAM.memorySizeCnt>=UDS_RAM.memorySize)
    {
      return REQ_SEQ_ERR;
    }
    U8 blockSequenceCounter = pData[1];
    if(blockSequenceCounter == UDS_RAM.blockSequenceCnt++)
    {
      U16 i;
      U8 sndec[8];
      U16 len = *uwLen-2;
      UDS_RAM.memorySizeCnt += len;
      /* To do something */
      if(UDS_RAM.encryptingMethod == 0)
      {
          /* Write data to flash */
          switch(UDS_RAM.dlType)
          {
          case DL_TYPE_DRIVER:
            /* nothing to do */
            break;
          case DL_TYPE_PROG:
            if(Flash_Write(pData+2,len))
            {
              /* Success */
            }
            else
            {
              return GEN_PROG_FAIL;
            }
            break;
          default:
            return GEN_PROG_FAIL;
          }
      }
      else if(UDS_RAM.encryptingMethod == 1)
      {
        for(i=0;i<len;i+=8)
        {
          des(pData+2+i,sndec);
          /* Write data to flash */
          switch(UDS_RAM.dlType)
          {
          case DL_TYPE_DRIVER:
            /* nothing to do */
            break;
          case DL_TYPE_PROG:
            if(Flash_Write(sndec,8))
            {
              /* Success */
            }
            else
            {
              return GEN_PROG_FAIL;
            }
            break;
          default:
            return GEN_PROG_FAIL;
          }
        }
      }
      else
      {
        /* invalid encrypting method */
        return REQ_OR;
      }
#if USE_ROUTINE_CNTL && USE_RT_CHECKPROGINTEGRITY
      U32 crc32 = CRC32_ForBytes(pData+2,len);
      if(UDS_RAM.memorySize==UDS_RAM.memorySizeCnt)
      {
        UDS_RAM.crc32 = crc32;
      }
#endif
    }
    else
    {
      return WRONG_BLOCK_SEQ_COUNT;
    }
  }
  else
  {
    return REQ_SEQ_ERR;
  }
  pData[0] = UDS_XFER_DATA_RSP;
  *uwLen=2;
  return POS_RSP;
}
#endif /* USE_XFER_DATA */

/**
* RequestTransferExit (37 hex)
*
* @param pData - Pointer to message data buffer.
*        uwLen - Message data length.
* @return POS_RSP - positiveResponse
*         INCORR_MSG_LEN_O_INVLD_FM - incorrectMessageLengthOrInvalidFormat
*         REQ_SEQ_ERR - requestSequenceError
*         REQ_OR - requestOutOfRange
*         GEN_PROG_FAIL - GeneralProgrammingFailure
* @date Jan. 19th 2017
*/
#if USE_REQ_XFER_EXIT
U8 RequestTransferExit(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen)
{
#if USE_SEC_ACCS
  if(UDS_RAM.SecureAccess==0)
    return SCY_ACCS_DENY;
#endif
  if(*uwLen!=1)
    return INCORR_MSG_LEN_O_INVLD_FM;
  if(UDS_RAM.UL_DL_Step == STEP_REQ_DL && UDS_RAM.memorySizeCnt==UDS_RAM.memorySize)
  {
    switch(UDS_RAM.dlType)
    {
    case DL_TYPE_DRIVER:
      break;
    case DL_TYPE_PROG:
      FLASH_Lock();
      UDS_RAM.UL_DL_Step = STEP_READY;
      break;
    default:
      return GEN_PROG_FAIL;
    }
  }
  else
  {
    return REQ_SEQ_ERR;
  }
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, DISABLE);
  pData[0] = UDS_REQ_XFER_EXIT_RSP;
  return POS_RSP;
}
#endif /* USE_REQ_XFER_EXIT */

