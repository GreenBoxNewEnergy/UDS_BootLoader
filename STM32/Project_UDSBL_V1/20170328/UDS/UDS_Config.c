/*******************************************************************************
File - UDS_Config.c
UDS Configuration.
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
#if USE_REQ_DOWNLOAD || USE_XFER_DATA || USE_REQ_XFER_EXIT || USE_RT_ERASEMEM \
    || (USE_ROUTINE_CNTL && USE_RT_ERASEMEM)
#include "flash.h"
#endif
#if USE_ROUTINE_CNTL && USE_RT_ERASEMEM
#include "eeprom.h"
#endif

/* Definition for RAM variables */
s_UDS_RAM UDS_RAM = {
  DFLT_SESSN, /* diagnosticSessionType */
  50, /* Default P2CAN_Server_max */
  5000,  /* Enhanced (NRC 78 hex) P2CAN_Server_max */
#if USE_S3_SERVER
  5000,
  0,
#endif
#if USE_ECU_RST
  0, /* resetType */
#endif
#if USE_CNTL_DTC_SET
  DTC_SET_ON, /* DTC Setting */
#endif
#if USE_SEC_ACCS
  UDS_FALSE, /* Secure Access */
  0, /* requestSeed */
  UDS_FALSE, /* requestSeedReceived */
  3, /* maxNumOfAttempts */
  0, /* numOfAttempts */
  10000, /* requiredTimeDelay */
  0, /* requiredTimeDelayCnt */
#endif
#if USE_REQ_DOWNLOAD || USE_XFER_DATA || USE_REQ_XFER_EXIT
  DL_TYPE_DRIVER, /* dlType */
  STEP_READY,  /* Upload Download Step */
  0, /* memorySize */
  0, /* memorySizeCnt */
  0, /* blockSequenceCnt */
  0, /* encryptingMethod */
#endif
#if USE_ROUTINE_CNTL
  /* Routine CheckPreProg */
#if USE_RT_CHECKPREPROG
  UDS_FALSE, /* PreProgCondition */
#endif
#if USE_RT_ERASEMEM
  0, /* Flash Erase Flag */
  0, /* eraseMemoryAddress */
  0, /* eraseMemorySize */
#endif
#if USE_RT_CHECKPROGINTEGRITY
  0, /* crc32 */
#endif
#endif
};

/* Definition for ROM variables */
UDS_CONST_STORAGE s_UDS_ROM UDS_ROM = {
#if USE_RD_DATA_BYID
  {
    /* ReadDataByIdentifier Fingerprint */
#if USE_RDBI_FINGERPRINT
    {ACCS_ALL, 0xF021, RdbiFingerprint},
#endif
    /* ReadDataByIdentifier EcuInfo */
#if USE_RDBI_ECUINFO
    {ACCS_ALL, 0x2223, RdbiEcuInfo},
#endif
#if USE_RDBI_ECUHW
    /* ReadDataByIdentifier EcuHW */
    {ACCS_ALL, 0xF193, RdbiEcuHW},
#endif
#if USE_RDBI_ECUSW
    /* ReadDataByIdentifier EcuSW */
    {ACCS_ALL, 0xF195, RdbiEcuSW},
#endif
  },
#endif
#if USE_WR_DATA_BYID
  {
    {ACCS_ALL, 0xF011, 19, WdbiFingerprint},
  },
#endif
#if USE_ROUTINE_CNTL
  {
    /* Routine CheckPreProg */
#if USE_RT_CHECKPREPROG
    {ACCS_EXT_DIAG, 0x0202, 0, RoutineCheckPreProg},
#endif
    /* Routine EraseMem */
#if USE_RT_ERASEMEM
    {ACCS_PROG, 0xFF00, 1, RoutineEraseMem},
#endif
    /* Routine CheckProgIntegrity */
#if USE_RT_CHECKPROGINTEGRITY
    {ACCS_PROG, 0x0201, 1, RoutineCheckProgIntegrity},
#endif
    /* Routine CheckProgDependence */
#if USE_RT_CHECKPROGDEPENDENCE
    {ACCS_PROG, 0xFF01, 1, RoutineCheckProgDependence},
#endif
    /* Routine ParticularCanRx */
#if USE_RT_PARTICULAR_CAN_RX
    {ACCS_ALL, 0x0203, 0, RoutineParticularCanRx},
#endif
  }, /* Routines */
#endif
#if USE_REQ_DOWNLOAD || USE_XFER_DATA || USE_REQ_XFER_EXIT || USE_RT_ERASEMEM
  ApplicationAddress, /* memoryAddress */
  ApplicationMemorySizeMax, /* memorySizeMax */
  PAGE_SIZE+2, /* blockLengthMax */
  0x3F00, /* driverAddress */
  8, /* driverSizeMax */
#endif
};

/* Request1 */
UDS_CONST_STORAGE DIAG_N_AI_UID_N_t REQ1 = { REQ1_BEXTID, REQ1_IDENT };  /* N_AI */
UDS_CONST_STORAGE ADDR_FORMAT_t REQ1_AF = {REQ1_ADDR_FORMAT, REQ1_MTYPE, REQ1_N_TATYPE, &REQ1, REQ1_MASK};  /* Addressing formats */
U8 REQ1_DATA_BUF_RX[NWS_REQ1_DBR_MAX]; /* reception data buffer */
NWS_MSG_RX_t REQ1_MSG_RX = {REQ1_BS, REQ1_STMIN, NWS_MSG_STATUS_UNINIT, REQ1_DATA_BUF_RX, NWS_REQ1_DBR_MAX};
UDS_CANrxBuf_t CAN_REQ1_RX;

/* Request2 */
UDS_CONST_STORAGE DIAG_N_AI_UID_N_t REQ2 = { REQ2_BEXTID, REQ2_IDENT };  /* N_AI */
UDS_CONST_STORAGE ADDR_FORMAT_t REQ2_AF = {REQ2_ADDR_FORMAT, REQ2_MTYPE, REQ2_N_TATYPE, &REQ2, REQ2_MASK};  /* Addressing formats */
U8 REQ2_DATA_BUF_RX[NWS_REQ2_DBR_MAX]; /* reception data buffer */
NWS_MSG_RX_t REQ2_MSG_RX = {REQ2_BS, REQ2_STMIN, NWS_MSG_STATUS_UNINIT, REQ2_DATA_BUF_RX, NWS_REQ2_DBR_MAX};
UDS_CANrxBuf_t CAN_REQ2_RX;

/* Response1 */
UDS_CONST_STORAGE DIAG_N_AI_UID_N_t RSP1 = { RSP1_BEXTID, RSP1_IDENT };  /* N_AI */
UDS_CONST_STORAGE ADDR_FORMAT_t RSP1_AF = {RSP1_ADDR_FORMAT, RSP1_MTYPE, RSP1_N_TATYPE, &RSP1}; /* Addressing formats */
U8 RSP1_DATA_BUF_TX[NWS_RSP1_DBT_MAX];  /* transmission data buffer */
NWS_MSG_TX_t RSP1_MSG_TX = {NWS_MSG_STATUS_UNINIT, RSP1_DATA_BUF_TX, NWS_RSP1_DBT_MAX};
UDS_CANtxBuf_t CAN_RSP1_TX;

/* Timing parameters */
NWS_MSG_TIM_t NWS_MSG_TIM1 = {NWS_MS(TIM1_N_Bs), NWS_MS(TIM1_N_Cr), NWS_MS(TIM1_N_Br), NWS_MS(TIM1_N_Cs), NWS_MS(TIM1_N_As), NWS_MS(TIM1_N_Ar)};
NWS_MSG_TIM_t NWS_MSG_TIM2 = {NWS_MS(TIM2_N_Bs), NWS_MS(TIM2_N_Cr), NWS_MS(TIM2_N_Br), NWS_MS(TIM2_N_Cs), NWS_MS(TIM2_N_As), NWS_MS(TIM2_N_Ar)};

/* message configuration */
UDS_CONST_STORAGE s_NWS_MSG NWS_MSG1 = {&RSP1_MSG_TX, &REQ1_MSG_RX, &NWS_MSG_TIM1};
UDS_CONST_STORAGE s_NWS_MSG NWS_MSG2 = {&RSP1_MSG_TX, &REQ2_MSG_RX, &NWS_MSG_TIM2};

/* Diagnostic services implementation */
UDS_CONST_STORAGE s_UDS_DSI UDS_REQ1_DSI[UDS_REQ1_DSI_MAX] = {
  /* DiagnosticSessionControl (10 hex) */
#if USE_DIAG_SESSN_CNTL
  {ACCS_ALL, UDS_DIAG_SESSN_CNTL_REQ, UDS_TRUE, DiagnosticSessionControl},
#endif
  /* CommunicationControl (11 hex) */
#if USE_ECU_RST
  {ACCS_ALL, UDS_ECU_RST_REQ, UDS_TRUE, ECUReset},
#endif
  /* SecurityAccess (27 hex) */
#if USE_SEC_ACCS
  {ACCS_PROG, UDS_SEC_ACCS_REQ, UDS_TRUE, SecurityAccess},
#endif
  /* CommunicationControl (28 hex) */
#if USE_COMM_CNTL
  {ACCS_ALL, UDS_COMM_CNTL_REQ, UDS_TRUE, CommunicationControl},
#endif
  /* TesterPresent (3E hex) */
#if USE_TSTER_PRST
  {ACCS_ALL, UDS_TSTER_PRST_REQ, UDS_TRUE, TesterPresent},
#endif
  /* ReadDataByIdentifier (22 hex) */
#if USE_RD_DATA_BYID
  {ACCS_ALL, UDS_RD_DATA_BYID_REQ, UDS_FALSE, ReadDataByIdentifier},
#endif
  /* WriteDataByIdentifier (2E hex) */
#if USE_WR_DATA_BYID
  {ACCS_ALL, UDS_WR_DATA_BYID_REQ, UDS_FALSE, WriteDataByIdentifier},
#endif
  /* ClearDiagnosticInformation (14 hex) */
#if USE_CLR_DIAG_INFO
  {ACCS_ALL, UDS_CLR_DIAG_INFO_REQ, UDS_FALSE, ClearDiagnosticInformation},
#endif
  /* RoutineControl (31 hex) */
#if USE_ROUTINE_CNTL
  {ACCS_ALL, UDS_ROUTINE_CNTL_REQ, UDS_TRUE, RoutineControl},
#endif
  /* RequestDownload (34 hex) */
#if USE_REQ_DOWNLOAD
  {ACCS_PROG, UDS_REQ_DOWNLOAD_REQ, UDS_FALSE, RequestDownload},
#endif
  /* TransferData (36 hex) */
#if USE_XFER_DATA
  {ACCS_PROG, UDS_XFER_DATA_REQ, UDS_FALSE, TransferData},
#endif
  /* RequestTransferExit (37 hex) */
#if USE_REQ_XFER_EXIT
  {ACCS_PROG, UDS_REQ_XFER_EXIT_REQ, UDS_FALSE, RequestTransferExit}
#endif
};

/* Diagnostic services implementation */
UDS_CONST_STORAGE s_UDS_DSI UDS_REQ2_DSI[UDS_REQ2_DSI_MAX] = {
  /* DiagnosticSessionControl (10 hex) */
#if USE_DIAG_SESSN_CNTL
  {ACCS_ALL, UDS_DIAG_SESSN_CNTL_REQ, UDS_TRUE, DiagnosticSessionControl},
#endif
  /* CommunicationControl (11 hex) */
#if USE_ECU_RST
  {ACCS_EXT_DIAG, UDS_COMM_CNTL_REQ, UDS_TRUE, CommunicationControl},
#endif
  /* ControlDTCSetting (85 hex) */
#if USE_CNTL_DTC_SET
  {ACCS_EXT_DIAG, UDS_CNTL_DTC_SET_REQ, UDS_TRUE, ControlDTCSetting}
#endif
};

/* UDS configuration */
UDS_CONST_STORAGE s_UDS_CFGS UDS_CFGS_1[NWS_MSG_MAX] = {
  {FILTER1_IDX, REQ1_IDX, &CAN_REQ1_RX, &CAN_RSP1_TX, &REQ1_AF, &RSP1_AF, &NWS_MSG1, UDS_REQ1_DSI, UDS_REQ1_DSI_MAX},
  {FILTER2_IDX, REQ2_IDX, &CAN_REQ2_RX, &CAN_RSP1_TX, &REQ2_AF, &RSP1_AF, &NWS_MSG2, UDS_REQ2_DSI, UDS_REQ2_DSI_MAX}
};

/* UDS long response */
UDS_RX_LONG_RSP_t UDS_RX_LONG_RSP = {0, UDS_NULL, NWS_MS(5000), 0, UDS_NULL, NWS_MS(500), 0};
