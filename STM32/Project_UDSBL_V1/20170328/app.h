/*******************************************************************************
File - app.h

Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Jan. 24th 2017
Version: V1.0.0
*******************************************************************************/
#ifndef _APP_H_
#define _APP_H_

#include "UDS.h"
#include "UDS_Const.h"
#include "flash.h"
#include "d3des.h"
#include "eeprom.h"


/* APP STATES */
typedef enum {
  CHECK_EXIT_PROG = 0,
  RUN_APP,
  ENTER_BOOTLOADER,
  RUN_BOOTLOADER,
} TASK_STATES;

extern volatile TASK_STATES task_states;
extern volatile U32 UDS_timer1ms;

#define PARTICULAR_CAN_RX_ID        0x602
#define PARTICULAR_CAN_TX_ID        0x6D2

#endif /* _APP_H_ */
