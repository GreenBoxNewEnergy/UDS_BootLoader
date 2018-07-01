/*******************************************************************************
File - CRC32.c

Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Jan. 24th 2017
Version: V1.0.0
*******************************************************************************/
#include "CRC32.h"
#include <string.h>

/**
* revbit
*
* @param uData - 32bit data.
* @return converted data
* @note 对入参uData 按位倒序。如:0111-->1110 
* @date Jan. 24th 2017
*/
u32 revbit(u32 uData)  
{  
  u32 uRevData = 0,uIndex = 0;  
  uRevData |= ((uData >> uIndex) & 0x01);  
  for(uIndex = 1;uIndex < 32;uIndex++)  
  {  
    uRevData <<= 1;  
    uRevData |= ((uData >> uIndex) & 0x01);  
  }  
  return uRevData;  
}

/**
* CRC32_ForBytes
*
* @param pData - 8bits buffer.
*       uLen - buffer length.
* @return crc32 value.
* @date Jan. 24th 2017
*/
/*================================================================== 
* Function  : CRC32_ForBytes 
* Description   : CRC32输入为8bits buffer的指针及长度 
* Input Para    :  
* Output Para   :  
* Return Value:  
==================================================================*/  
u32 CRC32_ForBytes(u8 *pData,u32 uLen)  
{  
  u32 uIndex= 0,uData = 0,i;  
  uIndex = uLen >> 2;
  
  while(uIndex--)  
  {  
#if USE_BIG_ENDIAN == 1
    uData = __REV((u32*)pData);  
#else  
    memcpy((u8*)&uData,pData,4);  
#endif        
    pData += 4;  
    uData = revbit(uData);
    CRC_CalcCRC(uData);
  }
  
  uData = revbit(CRC_GetCRC());  
  uIndex = uLen & 0x03;  
  while(uIndex--)  
  {  
    uData ^= (u32)*pData++;  
    for(i = 0;i < 8;i++)  
      if (uData & 0x1)  
        uData = (uData >> 1) ^ CRC32_POLYNOMIAL;  
      else  
        uData >>= 1;  
  }  
  return uData^0xFFFFFFFF;  
}
