/*******************************************************************************
File - UDS_Types.h
UDS Types.
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Jan. 24th 2017
Version: V1.0.0
*******************************************************************************/
#ifndef _UDS_TYPES_H_
#define _UDS_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
*
*       Public Definitions
*
*/

#define UDS_FALSE         0
#define UDS_TRUE          1

#define UDS_NULL          0

#define FIND_ERR_NO       0
#define FIND_ERR_NULL     1 /* Find None */
#define FIND_ERR_COND     2 /* RoutineControl are not met */

#define U8_MAX            0xFF
#define U16_MAX           0xFFFF
#define U32_MAX           0xFFFFFFFF

#define U16_HIBYTE(i)     ((U8)((i>>8)&0x00FF))
#define U16_LOBYTE(i)     ((U8)(i&0x00FF))

#define U32_HHIBYTE(i)     ((U8)((i>>24)&0x000000FF))
#define U32_HLOBYTE(i)     ((U8)((i>>16)&0x000000FF))
#define U32_LHIBYTE(i)     ((U8)((i>>8)&0x000000FF))
#define U32_LLOBYTE(i)     ((U8)(i&0x000000FF))
  
#define FILTER_MASK       0x7F
#define FILTER_SKIP       0x80

/**
* UDS basic data types
*
* @note U8 - Of type unsigned char
*       U16 - Of type unsigned short int
*       U32 - Of type unsigned long int
*       U64 - Of type unsigned long long int
*       S8 - Of type signed char
*       S16 - Of type signed short int
*       S32 - Of type signed long
*       S64 - Of type signed long long int
*       REAL32 - Of type float
*       REAL64 - Of type long double
*       VISIBLE_STRING - Of type char
*       OCTET_STRING - Of type unsigned char
*/
#define U8              uint8_t
#define U16             uint16_t
#define U32             uint32_t
#define U64             uint64_t
#define S8              int8_t
#define S16             int16_t
#define S32             int32_t
#define S64             int64_t
#define REAL32          float
#define REAL64          long double
#define VISIBLE_STRING  char
#define OCTET_STRING    unsigned char

#define BlockLen_t      U16

/*******************************************************************************
*
*       Public Types
*
*/

/**
* Return values of most UDS functions
*
* @note UDS_ERROR_NO - Operation completed successfully.
*       UDS_ERROR_IDX - Error in index arguments.
*       UDS_ERROR_RX_DISABLED - Receive disabled.m
*       UDS_ERROR_ILLEGAL_ARGUMENT - Error in function arguments.
*       UDS_ERROR_RX_OVERFLOW - Previous message was not processed yet.
*       UDS_ERROR_RX_MSG_LENGTH - Wrong receive message length.
*       UDS_ERROR_RX_MSG_DATA - Wrong receive message data.
*       UDS_ERROR_TX_UNCONFIGURED - Transmit buffer was not confugured properly.
*       UDS_ERROR_TX_ERROR - Transmit error.
*       UDS_ERROR_TX_OVERFLOW - Previous message is still waiting, buffer full.
*       UDS_ERROR_PARAMETERS - Error in function function parameters.
*       UDS_ERROR_DATA_CORRUPT - Stored data are corrupt.
*       UDS_ERROR_CRC - CRC does not match.
*       UDS_ERROR - Other error.
*/
typedef enum {
  UDS_ERROR_NO                = 0,
  UDS_ERROR_IDX               = -1,
  UDS_ERROR_RX_DISABLED       = -2,
  UDS_ERROR_ILLEGAL_ARGUMENT  = -3,
  UDS_ERROR_RX_OVERFLOW       = -4,
  UDS_ERROR_RX_MSG_LENGTH     = -5,
  UDS_ERROR_RX_MSG_DATA       = -6,
  UDS_ERROR_TX_UNCONFIGURED	  = -7,
  UDS_ERROR_TX_ERROR          = -8,
  UDS_ERROR_TX_OVERFLOW       = -9,
  UDS_ERROR_PARAMETERS		    = -10,
  UDS_ERROR_DATA_CORRUPT      = -11,
  UDS_ERROR_CRC               = -12,
  UDS_ERROR                   = -13
}UDS_ReturnError;
 
#define UDS_CAN_ID_STD                      0x00U
#define UDS_CAN_ID_EXT                      0x04U

#define UDS_CAN_RTR_DATA                    0x00U         
#define UDS_CAN_RTR_REMOTE                  0x02U

/**
* Download Type
*
* @note DL_TYPE_DRIVER - Download type is driver.
*       DL_TYPE_PROG - Download type is program.
*/
#define DL_TYPE_DRIVER                      0x00U
#define DL_TYPE_PROG                        0x01U

/*******************************************************************************
*
*       NWS Definitions
*
*/
#define NWS_PADDING_BYTE_VALUE              0x00U

/* message internal status */
#define NWS_MSG_STATUS_NONE                 0U

#define NWS_MSG_STATUS_TX_SF                1U
#define NWS_MSG_STATUS_TX_FF                2U
#define NWS_MSG_STATUS_TX_CF                3U
#define NWS_MSG_STATUS_TX_FC                4U

#define NWS_MSG_STATUS_RX_SF                5U
#define NWS_MSG_STATUS_RX_FF                6U
#define NWS_MSG_STATUS_RX_CF                7U
#define NWS_MSG_STATUS_RX_FC                8U

#define NWS_MSG_STATUS_TX_FC_OVERFLOW       9U
#define NWS_MSG_STATUS_TX_CF_STMIN          10U
#define NWS_MSG_STATUS_TX_READY				      11U

#define NWS_MSG_STATUS_UNINIT               0xFFU

/* Frame N_PCI definition */
#define N_PCI_TYPE_MASK                     0xF0U
#define N_PCI_VAL_MASK                      0x0FU
#define N_PCI_SF                            0x00U
#define N_PCI_FF                            0x10U
#define N_PCI_CF                            0x20U
#define N_PCI_FC                            0x30U

/* N_TAtype Normal fixed addressing */
/* ISO 15765-2 7.3.3 */
#define NFA_N_TATYPE_PHY_VAL                218U
#define NFA_N_TATYPE_FUN_VAL                219U

/* N_TAtype Mixed addressing 29 bit CAN identifier */
/* ISO 15765-2 7.3.5.1 */
#define EMA_N_TATYPE_PHY_VAL                206U
#define EMA_N_TATYPE_FUN_VAL                205U

/*******************************************************************************
*
*       NWS Types
*
*/

/**
* N_Result_t
*
* @note 
*/
typedef enum {
  N_OK = 0,
  N_TIMEOUT_A = -1,
  N_TIMEOUT_Bs = -2,
  N_TIMEOUT_Cr = -3,
  N_WRONG_SN = -4,
  N_INVALID_FS = -5,
  N_UNEXP_PDU = -6,
  N_WFT_OVRN = -7,
  N_BUFFER_OVFLW = -8,
  N_ERROR = -9
}N_Result_t;

/**
* NWS_MSG_TIM_t
*
* @note network layer timing
*/
typedef struct
{
  U32 uNBs;           /* Timeout between (Tx FF or Tx CF) and Rx FC */
  U32 uNCr;           /* Timeout between (Tx FC or Rx CF) and Rx CF */
  U32 uNBr;           /* Performance Requirement between (Rx FF or Rx CF) and Tx FC */
  U32 uNCs;           /* Performance Requirement between (Rx FC or Tx CF) and Tx CF */
  U32 uNAs;           /* Timeout between (req FF and con FF) or (req CF and con CF) */
  U32 uNAr;           /* Timeout between req FC and con FC */
}NWS_MSG_TIM_t;

/**
* NWS_MSG_TX_t
*
* @note send message configuration
*/
typedef struct
{
  U8 Status;
  U8 *pData;
  U16 uwDataMax;
  U16 uwLen;
  U16 uwLenCnt;
  U16 uCnt;
  U8 ubBs;
  U8 ubBsCnt;
  U8 ubSnCnt;
  U8 ubSTmin;
  
  U8 uMaxSFDL;      /* Max data len of single frame */
  U8 uMaxFFDL;      /* Max data len of first frame */
  U8 uMaxCFDL;      /* Max data len of consecutive frame */
  U8 uMaxFCDL;      /* Max data len of flow control */
  U8 uMinFCDL;      /* Min data len of flow control */
  
  U8 uMaxSFL;       /* Max len of single frame */
  U8 uMaxFFL;       /* Max len of first frame */
  U8 uMaxCFL;       /* Max len of consecutive frame */
  U8 uMinCFL;       /* Min len of consecutive frame */
  U8 uFCL;          /* Len of flow control */
  
  U8 ubRxFCWaitMax;   /* Maximum number of FCWait frame received in a row */
  U8 ubRxFCWaitCnt;   /* Number of FCWait frame received in a row */
  U16 uBufferCnt;     /* Transport Protocol counters */
}NWS_MSG_TX_t;

/**
* NWS_MSG_RX_t
*
* @note receive message configuration
*/
typedef struct
{
  U8 ubBs;
  U8 ubSTmin;
  
  U8 Status;
  U8 *pData;
  U16 uwDataMax;
  U16 uwLen;
  U16 uwLenCnt;
  U16 uCnt;
  U8 ubBsCnt;
  U8 ubSnCnt;
  
  U8 uMaxSFDL;      /* Max data len of single frame */
  U8 uMaxFFDL;      /* Max data len of first frame */
  U8 uMaxCFDL;      /* Max data len of consecutive frame */
  U8 uMaxFCDL;      /* Max data len of flow control */
  U8 uMinFCDL;      /* Min data len of flow control */
  
  U8 uMaxSFL;       /* Max len of single frame */
  U8 uMaxFFL;       /* Max len of first frame */
  U8 uMaxCFL;       /* Max len of consecutive frame */
  U8 uMinCFL;       /* Min len of consecutive frame */
  U8 uFCL;          /* Len of flow control */
}NWS_MSG_RX_t;

/**
* s_NWS_MSG
*
* @note 
*/
typedef struct
{
  NWS_MSG_TX_t *pMSG_TX;
  NWS_MSG_RX_t *pMSG_RX;
  NWS_MSG_TIM_t *pMSG_TIM;
} s_NWS_MSG;

/**
* Mtype ISO 15765-2 5.3.1
*
* @note Diagnostics - N_AI shall consist of the parameters N_SA, N_TA, and N_TAtype.
*       Remote Diagnostics - N_AI shall consist of the parameters N_SA, N_TA, N_TAtype, and N_AE.
*/
typedef enum {
  DIAG_TYPE,
  REMOTE_DIAG_TYPE
}Mtype_t;

/**
* N_TAtype ISO 15765-2 5.3.2.4
*
* @note Physical addressing - (1-to-1 communication), Supported for all types of network layer messages. 
*       Functional addressing - (1-to-n communication), Only supported for Single Frame communication.
*/
typedef enum {
  PHY_ADDR,
  FUN_ADDR
}N_TAtype_t;

/**
* N_AI, Address Information
*
* @note N_SA - Network Source Address.
*       N_TA - Network Target Address.
*       N_TAtype - Network Target Address type.
*       N_AE - Network Address Extension.
*/

/* Mtype == DIAGNOSTICS */

/* 11/29 bit CAN Id */
typedef struct{
  U8 bExtID;
  U32 Ident;
}DIAG_N_AI_UID_N_t;

/* 11/29 bit CAN Id */
typedef struct{
  U8 bExtID;
  U32 Ident;
  U8 N_TA;
}DIAG_N_AI_UID_E_t;

/* 29 bit CAN Id */
typedef struct{
  U8 PRIO;
  U8 N_SA;
  U8 N_TA;
}DIAG_N_AI_t;

/* Mtype == REMOTE_DIAGNOSTICS */

/* 29 bit CAN Id */
typedef struct{
  U8 PRIO;
  U8 N_SA;
  U8 N_TA;
  U8 N_AE;
}R_DIAG_N_AI_t;

/* 11 bit CAN Id */
typedef struct{
  U32 Ident;  
  U8 N_AE;
}R_DIAG_N_AI_UID_t;

/* Addressing Types */
typedef enum {
  /* Mtype == DIAGNOSTICS */
  NORMAL_ADDR = 0,
  NORMAL_FIXED_ADDR,
  EXTENDED_ADDR,
  /* Mtype == REMOTE_DIAGNOSTICS */
  EXTID_MIXED_ADDR,
  STDID_MIXED_ADDR
} ADDR_TYPE_t;

/**
* Addressing formats
*
* @note ADDR_TYPE == NORMAL_ADDR
*       - p.pDiag_N_AI_UID_N, ISO 15765.2 7.3.2
*       For each combination of N_SA, N_TA, N_TAtype and Mtype a unique CAN identifier is assigned.
*       ADDR_TYPE == NORMAL_FIXED_ADDR
*       - p.pDiag_N_AI, ISO 15765.2 7.3.3
*       For normal fixed addressing, only 29 bit CAN identifiers are allowed.
*       ADDR_TYPE == EXTENDED_ADDR
*       - p.pDiag_N_AI_UID_E, ISO 15765.2 7.3.4
*       For each each combination of N_SA, N_TAtype and Mtype a unique CAN identifier is assigned.
*       N_TA is placed in the first data byte of the CAN frame data field.
*       ADDR_TYPE == EXTID_MIXED_ADDR
*       - p.pR_Diag_N_AI, ISO 15765.2 7.3.5.1
*       Mixed addressing is the addressing format to be used if Mtype is set to remote diagnostics.
*       ADDR_TYPE == STDID_MIXED_ADDR
*       - p.pR_Diag_N_AI_UID, ISO 15765.2 7.3.5.2
*/
typedef struct {
  ADDR_TYPE_t ADDR_TYPE;
  Mtype_t Mtype;
  N_TAtype_t N_TAtype;
	union {
    UDS_CONST_STORAGE DIAG_N_AI_UID_N_t * pDiag_N_AI_UID_N;
    UDS_CONST_STORAGE DIAG_N_AI_UID_E_t * pDiag_N_AI_UID_E;
		UDS_CONST_STORAGE DIAG_N_AI_t * pDiag_N_AI;
		UDS_CONST_STORAGE R_DIAG_N_AI_t * pR_Diag_N_AI;
    UDS_CONST_STORAGE R_DIAG_N_AI_UID_t * pR_Diag_N_AI_UID;
	} p;
  U32 Mask;
} ADDR_FORMAT_t;


/* ISO 14229-2 7.3 */
typedef struct sN_PDU{
  U8 Renew;
  U8 idx;
  U32 Id;
  U8 Data[8];
  U8 DLC;
  struct sN_PDU *next;
} N_PDU_t;

/*******************************************************************************
*
*       UDS Definitions
*
*/
/* Service Id */
/* Diagnostic and communication control functional unit */
/* ISO 15765-3 9.2 ISO 14229 9*/
#define UDS_DIAG_SESSN_CNTL_REQ     0x10U /* DiagnosticSessionControl Request */
#define UDS_ECU_RST_REQ             0x11U /* ECUReset Request */
#define UDS_SEC_ACCS_REQ            0x27U /* SecurityAccess Request */
#define UDS_COMM_CNTL_REQ           0x28U /* CommunicationControl Request */
#define UDS_TSTER_PRST_REQ          0x3EU /* TesterPresent Request */
#define UDS_ACCS_TIM_PARM_REQ       0x83U /* AccessTimingParameter Request */
#define UDS_SEC_DATA_TX_REQ         0x84U /* SecuredDataTransmission Request */
#define UDS_CNTL_DTC_SET_REQ        0x85U /* ControlDTCSetting Request */
#define UDS_RESP_ON_EVNT_REQ        0x86U /* ResponseOnEvent Request */
#define UDS_LINK_CNTL_REQ           0x87U /* LinkControl Request */

#define UDS_DIAG_SESSN_CNTL_RSP     0x50U /* DiagnosticSessionControl Response */
#define UDS_ECU_RST_RSP             0x51U /* ECUReset Response Service Id */
#define UDS_SEC_ACCS_RSP            0x67U /* SecurityAccess Response */
#define UDS_COMM_CNTL_RSP           0x68U /* CommunicationControl Response */
#define UDS_TSTER_PRST_RSP          0x7EU /* TesterPresent Response */
#define UDS_ACCS_TIM_PARM_RSP       0xC3U /* AccessTimingParameter Response */
#define UDS_SEC_DATA_TX_RSP         0xC4U /* SecuredDataTransmission Response */
#define UDS_CNTL_DTC_SET_RSP        0xC5U /* ControlDTCSetting Response */
#define UDS_RESP_ON_EVNT_RSP        0xC6U /* ResponseOnEvent Response */
#define UDS_LINK_CNTL_RSP           0xC7U /* LinkControl Response */

/* Data transmission functional unit */
/* ISO 15765-3 9.3 ISO 14229 10*/
#define UDS_RD_DATA_BYID_REQ        0x22U /* ReadDataByIdentifier Request */
#define UDS_RD_MEM_BY_ADDR_REQ      0x23U /* ReadMemoryByAddress Request */
#define UDS_RD_SCAL_DATA_BYID_REQ   0x24U /* ReadScalingDataByIdentifier Request */
#define UDS_RD_DATA_BY_PD_ID_REQ    0x2AU /* ReadDataByPeriodIdentifier Request */
#define UDS_DYNA_DEF_DATA_ID_REQ    0x2CU /* DynamicallyDefineDataIdentifier Request */
#define UDS_WR_DATA_BYID_REQ        0x2EU /* WriteDataByIdentifier Request */
#define UDS_WR_MEM_BY_ADDR_REQ      0x3DU /* WriteMemoryByAddress Request */

#define UDS_RD_DATA_BYID_RSP        0x62U /* ReadDataByIdentifier Response */
#define UDS_RD_MEM_BY_ADDR_RSP      0x63U /* ReadMemoryByAddress Response */
#define UDS_RD_SCAL_DATA_BYID_RSP   0x64U /* ReadScalingDataByIdentifier Response */
#define UDS_RD_DATA_BY_PD_ID_RSP    0x6AU /* ReadDataByPeriodIdentifier */
#define UDS_DYNA_DEF_DATA_ID_RSP    0x6CU /* DynamicallyDefineDataIdentifier Response */
#define UDS_WR_DATA_BYID_RSP        0x6EU /* WriteDataByIdentifier Response */
#define UDS_WR_MEM_BY_ADDR_RSP      0x7DU /* WriteMemoryByAddress Response */

/* Stored data transmission functional unit */
/* ISO 15765-3 9.4 ISO 14229 11 */
#define UDS_CLR_DIAG_INFO_REQ       0x14U /* ClearDiagnosticInformation Request */
#define UDS_RD_DTC_INFO_REQ         0x19U /* ReadDTCInformation Request */

#define UDS_CLR_DIAG_INFO_RSP       0x54U /* ClearDiagnosticInformation Response */
#define UDS_RD_DTC_INFO_RSP         0x59U /* ReadDTCInformation Response */

/* InputOutput control functional unit */
/* ISO 15765-3 9.5 ISO 14229 12 */
#define UDS_IN_OUT_CNTL_BYID_REQ   0x2FU /* InputOutputControlByIdentifier Request */

#define UDS_IN_OUT_CNTL_BYID_RSP   0x6FU /* InputOutputControlByIdentifier Response */

/* Remote activation of routine functional unit */
/* ISO 15765-3 9.6 ISO 14229 13 */
#define UDS_ROUTINE_CNTL_REQ        0x31U /* RoutineControl Request */

#define UDS_ROUTINE_CNTL_RSP        0x71U /* RoutineControl Response */

/* Upload Download functional unit */
/* ISO 15765-3 9.7 ISO 14229 14 */
#define UDS_REQ_DOWNLOAD_REQ        0x34U /* RequestDownload Request */
#define UDS_REQ_UPLOAD_REQ          0x35U /* RequestUpload Request */
#define UDS_XFER_DATA_REQ           0x36U /* TransferData Request */
#define UDS_REQ_XFER_EXIT_REQ       0x37U /* RequestTransferExit Request */

#define UDS_REQ_DOWNLOAD_RSP        0x74U /* RequestDownload Response */
#define UDS_REQ_UPLOAD_RSP          0x75U /* RequestUpload Response */
#define UDS_XFER_DATA_RSP           0x76U /* TransferData Response */
#define UDS_REQ_XFER_EXIT_RSP       0x77U /* RequestTransferExit Response */

/* Negative response message */
#define UDS_NEG_RSP                 0x7FU /* Negative Response */

/* suppressPosRspMsgIndicationBit */
/* ISO14229-1 8.2.2 */
/* This bit indicates if a positive response message shall be suppressed by the server 
'0' = FALSE, do not suppress a positive response message (a positive response message is required)
'1' = TRUE, suppress response message (a positive response message shall not be sent; 
the server being addressed shall not send a positive response message).*/
#define SPRS_POS_RSP_MSG_IND_BIT    0x80U

/* sub-function parameter value */
#define SUB_FUNC_PARAM_MASK         0x7FU

/* DiagnosticSessionControl Request 0x10 sub-function */
#define DFLT_SESSN                  0x01U /* defaultSession */
#define ECU_PROG_SESSN              0x02U /* ECUProgrammingSession */
#define ECU_EXT_DIAG_SESSN          0x03U /* ECUExtendedDiagnosticSession */

/* ECUReset Request 0x11 sub-function */
#define HARD_RST                    0x01U /* hardReset */
#define KEY_OFF_ON_RST              0x02U /* keyOffOnReset */
#define SOFT_RST                    0x03U /* softReset */
#define EN_RAPID_PWR_SHDN           0x04U /* enableRapidPowerShutDown */
#define DIS_RAPID_PWR_SHDN          0x05U /* disableRapidPowerShutDown */

/* SecurityAccess Request 0x27 sub-function */
#define REQ_SEED_07                 0x07U /* requestSeed 0x07 */
#define SEND_KEY_08                 0x08U /* sendKey 0x08 */

/* TesterPresent Request 0x3E sub-function */
#define ZERO_SUB_FUNC               0x00U /* zeroSubFunction */

/* AccessTimingParameter Request 0x83 sub-function */
#define RD_EXT_TIM_PARAMSET         0x01U /* readExtendedTimingParameterSet */
#define SET_TIM_PARAM_TO_DFLT_VAL   0x02U /* setTimingParametersToDefaultValues */
#define RD_CURR_ACT_TIM_PARAM       0x03U /* readCurrentlyActiveTimingParameters */
#define SET_TIM_PARAM_TO_GIVEN_VAL  0x04U /* setTimingParametersToGivenValues */

/* RoutineControl Request sub-function */
#define START_ROUTINE               0x01U /* startRoutine */
#define STOP_ROUTINE                0x02U /* stopRoutine */
#define REQ_ROUTINE_RSLT            0x03U /* requestRoutineResults */

/* routineInfo */
#define ROUTINE_CORR_RSLT           0x00U /* correctResult */
#define ROUTINE_INCORR_RSLT         0x01U /* incorrectResult  */

/* ControlDTCSetting Request sub-function */
#define DTC_SET_ON                  0x01U /* on */
#define DTC_SET_OFF                 0x02U /* off */

/* ResponseOnEvent Request sub-function */
#define DONOT_STORE_EVENT           0x00U /* doNotStoreEvent */
#define STORE_EVENT                 0x01U /* storeEvent */

/* LinkControl Request sub-function */
#define VERF_BAUDTX_WITH_FIXBAUD    0x01U /* verifyBaudrateTransitionWithFixedBaudrate */
#define VERF_BAUDTX_WITH_SPECBAUD   0x02U /* verifyBaudrateTransitionWithSpecificBaudrate */
#define TX_BAUD                     0x03U /* transitionBaudrate */
#define VEH_MFR_SPEC_BASE           0x40U /* vehicleManufacturerSpecific 0x40~0x5F */
#define SYS_SUPPL_SPEC_BASE         0x60U /* systemSupplierSpecific 0x60~0x7E */

/* CommunicationControl Request sub-function */
#define EN_RX_TX                    0x00U /* enableRxAndTx */
#define EN_RX_DIS_TX                0x01U /* enableRxAndDisableTx */
#define DIS_RX_EN_TX                0x02U /* disableRxAndEnableTx  */
#define DIS_RX_TX                   0x03U /* disableRxAndTx */
#define EN_RX_DIS_TX_EAI            0x04U /* enableRxAndDisableTxWithEnhancedAddressInformation */
#define EN_RX_TX_EAI                0x05U /* enableRxAndTxWithEnhancedAddressInformation */

/* Definition of communicationType and subnetNumber byte */
#define CT_NORM_COMM_MSG            (1<<0) /* normalCommunicationMessages */
#define CT_NET_MGT_COMM_MSG         (1<<1) /* networkManagementCommunicationMessages */
#define CT_DE_SPEC_COMM_TYPE        (0x0U<<4) /* Disable / Enable specified communicationType */
#define CT_DE_SPEC_SUBID_BY_SUBNUM(i) (i<<4) /* Disable / Enable specific subnet identified by subnet number i(0x1~0xE)*/
#define CT_DE_NET_WHICH_REQ_BERECV  (0xFU<<4) /* Disable/Enable network which request is received on (Receiving node (server)) */

/* ReadDTCInformation Request 0x19 sub-function */
#define RPT_DTCNUM_BY_STSMASK       0x01U /* reportNumberOfDTCByStatusMask */
#define RPT_DTC_BY_STSMASK          0x02U /* reportDTCByStatusMask */
#define RPT_DTC_SNAPS_ID            0x03U /* reportDTCSnapshotIdentification */
#define RPT_DTC_SNAPS_REC_BYDTCNUM  0x04U /* reportDTCSnapshotRecordByDTCNumber */
#define RPT_DTC_DTCNUM_BYSVRMASKREC 0x07U /* reportNumberOfDTCBySeverityMaskRecord */
#define RPT_DTC_DTC_BYSVRMASKREC    0x08U /* reportDTCBySeverityMaskRecord */
#define RPT_SUP_DTC                 0x0AU /* reportSupportedDTC */
#define RPT_FIRST_TEST_FAIL_DTC     0x0BU /* reportFirstTestFailedDTC */
#define RPT_FIRST_TEST_CONF_DTC     0x0CU /* reportFirstTestConfirmedDTC */
#define RPT_MOST_RECT_TEST_FAIL_DTC 0x0DU /* reportMostRecentTestFailedDTC */
#define RPT_MOST_RECT_TEST_CONF_DTC 0x0EU /* reportMostRecentTestConfirmedDTC */
#define RPT_MIMENDTC_BYSTSMASK      0x0FU /* reportMirrorMemoryDTCByStatusMask */
#define RPT_MIMENDTCNUM_BYSTSMASK   0x11U /* reportNumberOfMirrorMemoryDTCByStatusMask */
#define RPT_EROBDDTCNUM_BYSTSMASK   0x12U /* reportNumberOfEmissionsRelatedOBDDTCByStatusMask */
#define RPT_EROBDDTC_BYSTSMASK      0x13U /* reportEmissionsRelatedOBDDTCByStatusMask */
#define RPT_DTC_FAULT_DETECT_COUNT  0x14U /* reportDTCFaultDetectionCounter */
#define RPT_DTC_WITH_PERM_STAT      0x15U /* reportDTCWithPermanentStatus */

/* DTCFormatIdentifier */
#define ISO15031_6DTCFORMAT         0x00U /* ISO15031_6DTCFORMAT */
#define ISO14229_1DTCFORMAT         0x01U /* ISO14229_1DTCFORMAT */
#define SAEJ1939_73DTCFORMAT        0x02U /* SAEJ1939_73DTCFORMAT */
#define ISO11992_4DTCFORMAT         0x03U /* ISO11992_4DTCFORMAT */

/* Negative Response codes */
#define POS_RSP                     0x00U /* positiveResponse */
#define SERV_NOT_SUP                0x11U	/* serviceNotSupported */
#define SUB_FUNC_NOT_SUP            0x12U /* sub-functionNotSupported */
#define INCORR_MSG_LEN_O_INVLD_FM   0x13U /* incorrectMessageLengthOrInvalidFormat */
#define RSP_TOOLONG                 0x14U /* responseTooLong */
#define COND_NOT_CORR               0x22U /* conditionsNotCorrect */
#define REQ_SEQ_ERR                 0x24U /* requestSequenceError */
#define REQ_OR                      0x31U /* requestOutOfRange */
#define SCY_ACCS_DENY               0x33U /* securityAccessDenied */
#define INVLD_KEY                   0x35U /* Invalid Key */
#define EXCD_NUM_ATMP               0x36U /* exceededNumberOfAttempts */
#define REQD_TD_NOT_EXPR            0x37U /* requiredTimeDelayNotExpired */
#define UL_DL_NOT_ACCEPT            0x70U /* uploadDownloadNotAccepted  */
#define XFER_DATA_SUS               0x71U /* transferDataSuspended */ 
#define GEN_PROG_FAIL               0x72U /* GeneralProgrammingFailure */
#define WRONG_BLOCK_SEQ_COUNT       0x73U /* wrongBlockSequenceCounter */
#define REQ_CORR_RECV_RSP_PEND      0x78U /* requestCorrectlyReceived-ResponsePending */
#define VOLT_TOO_HIGH               0x92U /* voltageTooHigh */
#define VOLT_TOO_LOW                0x93U /* voltageTooLow */

/*******************************************************************************
*
*       UDS Types
*
*/
#define ACCS_DFLT                   0x01
#define ACCS_PROG                   0x02
#define ACCS_EXT_DIAG               0x04

#define ACCS_NO_DFLT                (ACCS_PROG|ACCS_EXT_DIAG)
#define ACCS_NO_PROG                (ACCS_DFLT|ACCS_EXT_DIAG)
#define ACCS_ALL                    (ACCS_DFLT|ACCS_PROG|ACCS_EXT_DIAG)


/* Upload Download functional step */
typedef enum {
  STEP_READY,
  STEP_REQ_DL,
  STEP_REQ_UL
}UL_DL_STEP;

/* Diagnostic services implementation */
typedef struct sUDS_DSI {
  U8 Access;
  U8 ServId;
  U8 bSubFCN;
  U8 (*pFunct)(UDS_CONST_STORAGE struct sUDS_DSI *pDSI, U8 *pData, U16 *uwLen);
} s_UDS_DSI;


typedef struct UDS_CANrxBuf UDS_CANrxBuf_t;
typedef struct UDS_CANtxBuf UDS_CANtxBuf_t;

/**
* uds & network layer configs
*
* @note 
*/
typedef struct {
  U8 idxf;
  U8 idx;
  UDS_CANrxBuf_t *pCANrxBuf;
  UDS_CANtxBuf_t *pCANtxBuf;
  UDS_CONST_STORAGE ADDR_FORMAT_t *pAFR;
  UDS_CONST_STORAGE ADDR_FORMAT_t *pAFT;
  UDS_CONST_STORAGE s_NWS_MSG *pNWS_MSG;
  UDS_CONST_STORAGE s_UDS_DSI *pUDS_DSI;
  U8 DSI_MAX;
} s_UDS_CFGS;

/**
* CAN receive message structure
*
* @note 
*/
typedef struct{
  U32 stdid;
  U32 extid;
  U8 IDE;
  U8 RTR;
  U8 DLC;
  U8 data[8];
  U8 FMI;
}UDS_CANrxMsg_t;

/**
* buf for handling received CAN messages.
*
* @note 
*/
struct UDS_CANrxBuf{
  U8 bExtID;
  U32 Ident;
  U32 mask;
  U8 N_PCI_OFFSET;
  U8 ExtraAddr;
  void *object;
  UDS_ReturnError (*pRcb)(void *object, UDS_CANrxMsg_t *message, U8 idxf);
};

/**
* buf for handling send CAN messages.
*
* @note 
*/
struct UDS_CANtxBuf{
  U8 bExtID;
  U8 bRemote;
  U32 Ident;
  U8 Data[8];
  U8 DLC;
  U8 N_PCI_OFFSET;
  U8 ExtraAddr;
};


/**
* UDS_RX_LONG_RSP_t
*
*/
typedef struct
{
  U8 idx;
  U8 (*pLongRspCb)(U8 *pData, U16 *uwLen);
  U32 uRspPendingMax;
  U32 uRspPendingCnt;
  UDS_CONST_STORAGE s_UDS_DSI *dsi;
  U32 nrc78Time;
  U32 nrc78TimeCnt;
}UDS_RX_LONG_RSP_t;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
