/*******************************************************************************
File - CAN_Types.h
This is common module for body netword
Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: Byron
Date: 2017/01/01
Version: V1.0.0
*******************************************************************************/
#ifndef _CAN_TYPES_H_
#define _CAN_TYPES_H_

#include "stm32f10x.h"

/* CAN mailbox index type definition */
typedef unsigned char tCanMailboxIdx;
typedef unsigned char tCanCtrlStatus;

#define CAN_BUS_OFF	0U
#define CAN_BUS_ON	1U

/*add by bwf start*/
#define SOFT_TIMER_STOP	    0U
#define SOFT_TIMER_START	1U
#define BUSOFF_TIME_OVER    6U
/*add by bwf end*/

#define CAN_DATA_MAX                    8U

// CAN protocol handler mailbox number
#define CAN_MAILBOX_NB                  3U//32U

// Protocol handler Mailbox number used for transmission
// Range : (1 .. CAN_MAILBOX_NB) : imposed by hardware
//         0 : user configuration
#define CAN_TX_MAILBOX_NB               0U

// Protocol handler Mailbox number used for reception
// Range : (1 .. CAN_MAILBOX_NB) : imposed by hardware
//         0 : user configuration
#define CAN_RX_MAILBOX_NB               0U

#define CAN_WAKE_UP_ACTIVATE      1
#define CAN_WAKE_UP_DESACTIVATE      0

/* Upper layers automatic configuration with these next constants */
#define CAN_WAKE_UP_DETECTION      CAN_WAKE_UP_DESACTIVATE
/* CAN protocol handler mailboxes with individual mask (one mask per mailbox) :
 declare this constant if this functionnality is needed */
#define CAN_MAILBOX_INDIVIDUAL_MASK

/* Communication frames in reception number managed by this software layer */
/* Range : (0 .. 2^32) */
#define CAN_RX_FRAME_MAX                  2U
#define CAN_RX_FRAME_MAX_RAM              CAN_RX_FRAME_MAX

/* Communication frames in transmission number managed by this software layer */
/* Range : (0 .. 2^32) */
#define CAN_TX_FRAME_MAX                  1U

/* Communication frames in extension number managed by this software layer */
/* Range : (0 .. 2^32) */
#define CAN_EXT_FRAME_MAX              0U

/* Communication frames number managed by this software layer */
/* Range : (0 .. 2^32) */
#define CAN_FRAME_MAX                  3U
#define CAN_FRAME_MAX_RAM              CAN_FRAME_MAX




/* Frame transmission mode */
#define CAN_DIR                         0x01U
#define CAN_DIR_RX                      0x00U
#define CAN_DIR_TX                      0x01U

#define CAN_MODE                        0x06U
#define CAN_MODE_DIRECT                 0x00U
#define CAN_MODE_PERIODIC               0x02U
#define CAN_MODE_MIXED                  0x04U


typedef u32 tCanFrameIdx;



typedef enum {
    CAN_ERR_OK = 0,
    CAN_ERR_OFF = 0x11,
    CAN_ERR_MODE,
    CAN_ERR_IDX,
    CAN_ERR_FRAME_MODE,
    CAN_ERR_NODE,
    CAN_ERR_TX_MSG_LOST,
    CAN_ERR_TX_FIFO_FULL,
    CAN_ERR_RX_MSG_LOST,
    CAN_ERR_NULL,
    CAN_ERR_BUF,
    CAN_ERR_COHE,
    CAN_RX_IND_CHECK_OK,
    CAN_RX_IND_CHECK_REJECT,
    CAN_ERR_ABORT,
} tCanStatus;



/* COM software layer status */
#define CAN_MODE_OFF                    SDF_U8_VALUE1
#define CAN_MODE_BUS_ON                 SDF_U8_VALUE2
#define CAN_MODE_BUS_OFF                SDF_U8_VALUE3
typedef unsigned char tCanMode;


#endif
