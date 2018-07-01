/*******************************************************************************
File - flash.h

Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Jan. 1th 2017
Version: V1.0.0
*******************************************************************************/
#ifndef _FLASH_H_
#define _FLASH_H_
#include <stdint.h>
#include "bsp.h"
#include "stm32f10x_flash.h"
#define ApplicationAddress          0x08004000
#define ApplicationMemorySizeMax    0x0001B800

typedef  void (*pFunction)(void);
extern u32 JumpAddress;
extern pFunction Jump_To_Application;


extern u32 TotalSize;

s8 Flash_Erase (u32 addr, u32 size);
s8 Flash_Write (u8 *buf, u32 len);

#endif
