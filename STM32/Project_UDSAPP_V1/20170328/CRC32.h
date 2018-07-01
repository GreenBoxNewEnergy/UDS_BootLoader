/*******************************************************************************
File - CRC32.h

Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Jan. 24th 2017
Version: V1.0.0
*******************************************************************************/
#ifndef _CRC32_H_
#define _CRC32_H_
#include "bsp.h"

#define CRC32_POLYNOMIAL                        ((uint32_t)0xEDB88320)  
#define RCC_CRC_BIT                             ((uint32_t)0x00001000)  

#define USE_BIG_ENDIAN                          0U

u32 CRC32_ForBytes(u8 *pData,u32 uLen);

#endif /* _CRC32_H_ */
