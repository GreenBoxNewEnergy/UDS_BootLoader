/*******************************************************************************
File - UDS_Config.h
UDS Configuration.
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Jan. 24th 2017
Version: V1.0.0
*******************************************************************************/
#ifndef _UDS_PRIMITIVE_H_
#define _UDS_PRIMITIVE_H_


/*******************************************************************************
*
*       Type Definitions For Application Layer
*
*/

/**
* s_UDS_RAM
*
* @note memorySize
*       - memorySize of RequestDownload service
*       memorySizeCnt
*       - memorySize count for TransferData service run time
*       blockSequenceCnt
*       - blockSequence count for TransferData service run time
*       resetType
*       - resetType of ECUReset service
*       Session
*       - Session of DiagnosticSessionControl service
*       P2CAN_Server_max
*       - Default P2CAN_Server_max timing supported by the server for 
*       the activated diagnostic session. 0ms~65535ms Resolution 1ms
*       EP2CAN_Server_max
*       - Enhanced (NRC 78 hex) P2CAN_Server_max timing supported by the  
*       server for the activated diagnostic session. 0ms~655350ms Resolution 10ms
*       PreProgCondition
*       - pre-programmed condition 
*/
typedef struct{
  U8 Session;
  U16 P2CAN_Server_max;
  U16 EP2CAN_Server_max;
#if USE_S3_SERVER
  U16 S3_Server_max;
  U16 S3_Server_cnt;
#endif
#if USE_ECU_RST
  U8 resetType;
#endif
#if USE_CNTL_DTC_SET
  U8 DTCSet;
#endif
#if USE_SEC_ACCS
  U8 SecureAccess;
  U32 requestSeed;
  U32 requestSeedReceived;
  U8 maxNumOfAttempts;
  U8 numOfAttempts;
  U32 requiredTimeDelay;
  U32 requiredTimeDelayCnt;
#endif
#if USE_REQ_DOWNLOAD || USE_XFER_DATA || USE_REQ_XFER_EXIT
  U8 dlType;
  UL_DL_STEP UL_DL_Step;
  U32 memorySize;
  U32 memorySizeCnt;
  U8 blockSequenceCnt;
  U32 encryptingMethod;
#endif
#if USE_ROUTINE_CNTL
#if USE_RT_CHECKPREPROG
  U8 PreProgCondition;
#endif
#if USE_RT_ERASEMEM
  U8 FlashEraseFlag;
  U32 eraseMemoryAddress;
  U32 eraseMemorySize;
#endif
#if USE_RT_CHECKPROGINTEGRITY
  U32 crc32;
#endif
#endif
}s_UDS_RAM;

/**
 * RDBI_t
 *
 * @note 
 */
#if USE_RD_DATA_BYID
typedef struct{
  U8 Access;
  U16 Ident;
  U8 (*pFunct)(U8 *pData, U16 *uwLen);
}RDBI_t;
#endif

/**
 * WDBI_t
 *
 * @note 
 */
#if USE_WR_DATA_BYID
typedef struct{
  U8 Access;
  U16 Ident;
  U16 Len;
  U8 (*pFunct)(U8 *pData, U16 *uwLen);
}WDBI_t;
#endif

/**
 * Routine_t
 *
 * @note 
 */
#if USE_ROUTINE_CNTL
typedef struct{
  U8 Access;
  U16 Ident;
  U8 bSecureAccess;
  U8 (*pFunct)(U8 *pData, U16 *uwLen);
}Routine_t;
#endif

/**
 * s_UDS_ROM
 *
 * @note 
 */
typedef struct{
#if USE_RD_DATA_BYID
  RDBI_t Rdbi[RDBI_MAX];
#endif
#if USE_WR_DATA_BYID
  WDBI_t Wdbi[WDBI_MAX];
#endif
#if USE_ROUTINE_CNTL
  Routine_t Routines[ROUTINE_MAX];
#endif
#if USE_REQ_DOWNLOAD || USE_XFER_DATA || USE_REQ_XFER_EXIT || USE_RT_ERASEMEM
  U32 progAddress;
  U32 progSizeMax;
  BlockLen_t blockLengthMax;
  U16 driverAddress;
  U32 driverSizeMax;
#endif
}s_UDS_ROM;

/*******************************************************************************
*
*       Exported Variables
*
*/
extern s_UDS_RAM UDS_RAM;
extern UDS_CONST_STORAGE s_UDS_ROM UDS_ROM;
extern UDS_CONST_STORAGE s_UDS_CFGS UDS_CFGS_1[];
extern UDS_RX_LONG_RSP_t UDS_RX_LONG_RSP;

/*******************************************************************************
*
*       Exported Functions
*
*/
/* Process App */
UDS_ReturnError UDS_APP_Process(U32 timeStamp);

/* Process Long Operation */
UDS_ReturnError UDS_LongOperation_Process(void);

/* DSI Functions */

/* Find Diagnostic Services Implementation */
UDS_CONST_STORAGE s_UDS_DSI *Find_DSI(UDS_CONST_STORAGE s_UDS_CFGS *pCFGS, U8 ServID, U8 *errCode);

/* Diagnostic and communication control functional unit */
#if USE_DIAG_SESSN_CNTL
U8 DiagnosticSessionControl(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

#if USE_ECU_RST
U8 ECUReset(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

#if USE_SEC_ACCS
U8 SecurityAccess(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

#if USE_COMM_CNTL
U8 CommunicationControl(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

#if USE_TSTER_PRST
U8 TesterPresent(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

#if USE_ACCS_TIM_PARM
U8 AccessTimingParameter(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

#if USE_SEC_DATA_TX
U8 SecuredDataTransmission(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

#if USE_CNTL_DTC_SET
U8 ControlDTCSetting(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

#if USE_RESP_ON_EVNT
U8 ResponseOnEvent(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

#if USE_LINK_CNTL
U8 LinkControl(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

/* Data transmission functional unit */
#if USE_RD_DATA_BYID
U8 ReadDataByIdentifier(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

#if USE_WR_DATA_BYID
U8 WriteDataByIdentifier(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

/* Stored data transmission functional unit */
#if USE_CLR_DIAG_INFO
U8 ClearDiagnosticInformation(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

/* Remote activation of routine functional unit */
#if USE_ROUTINE_CNTL
U8 RoutineControl(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

/* Upload Download functional unit */
#if USE_REQ_DOWNLOAD
U8 RequestDownload(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

#if USE_XFER_DATA
U8 TransferData(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

#if USE_REQ_XFER_EXIT
U8 RequestTransferExit(UDS_CONST_STORAGE s_UDS_DSI *pDSI, U8 *pData, U16 *uwLen);
#endif

/* RDBI Functions */
#if USE_RD_DATA_BYID

UDS_CONST_STORAGE RDBI_t *Find_Rdbi(U16 RdbiIdent, U8 *errCode);

#if USE_RDBI_FINGERPRINT
U8 RdbiFingerprint(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_ECUINFO
U8 RdbiEcuInfo(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_VIN
U8 RdbiVIN(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_PN
U8 RdbiPN(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_SID
U8 RdbiSID(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_ECUHW
U8 RdbiEcuHW(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_ECUSW
U8 RdbiEcuSW(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_AMT
U8 RdbiAMT(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_AIT
U8 RdbiAIT(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_AMS
U8 RdbiAMS(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_AMTQ
U8 RdbiAMTQ(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_MMTQ
U8 RdbiMMTQ(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_AIC
U8 RdbiAIC(U8 *pData, U16 *uwLen);
#endif

#if USE_RDBI_RP
U8 RdbiRP(U8 *pData, U16 *uwLen);
#endif

#endif

/* WDBI Functions */
#if USE_WR_DATA_BYID

UDS_CONST_STORAGE WDBI_t *Find_Wdbi(U16 WdbiIdent, U8 *errCode);

#if USE_WDBI_FINGERPRINT
U8 WdbiFingerprint(U8 *pData, U16 *uwLen);
#endif

#endif

/* Routine Functions */
#if USE_ROUTINE_CNTL

UDS_CONST_STORAGE Routine_t *Find_Routine(U16 RoutineIdent, U8 *errCode);

#if USE_RT_CHECKPREPROG
U8 RoutineCheckPreProg(U8 *pData, U16 *uwLen);
#endif

#if USE_RT_ERASEMEM
U8 RoutineEraseMem(U8 *pData, U16 *uwLen);
#endif

#if USE_RT_CHECKPROGINTEGRITY
U8 RoutineCheckProgIntegrity(U8 *pData, U16 *uwLen);
#endif

#if USE_RT_CHECKPROGDEPENDENCE
U8 RoutineCheckProgDependence(U8 *pData, U16 *uwLen);
#endif

#if USE_RT_PARTICULAR_CAN_RX
U8 RoutineParticularCanRx(U8 *pData, U16 *uwLen);
#endif

#endif

#endif /* _UDS_PRIMITIVE_H_ */
