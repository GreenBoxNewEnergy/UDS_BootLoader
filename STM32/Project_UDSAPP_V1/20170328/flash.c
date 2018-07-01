/*******************************************************************************
File - flash.c

Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu
Date: Jan. 1th 2017
Version: V1.0.0
*******************************************************************************/
#include "flash.h"

u32 JumpAddress;
pFunction Jump_To_Application;


u32 FlashDestination = ApplicationAddress; /* Flash user program offset */

u32 TotalSize = 0;
u16 PageSize = PAGE_SIZE;
u32 EraseCounter = 0x0;
u8 NbrOfSector = 0;


/* Calculate the number of pages */
u32 FLASH_PagesMask(vu32 Size)
{
  u32 pagenumber = 0x0;
  TotalSize = Size;
  u32 size = Size;
  
  if ((size % PAGE_SIZE) != 0)
  {
    pagenumber = (size / PAGE_SIZE) + 1;
  }
  else
  {
    pagenumber = size / PAGE_SIZE;
  }
  return pagenumber;
  
}

s8 Flash_Erase (u32 addr, u32 size)
{
  FLASH_Status status = FLASH_COMPLETE;
  /* Test the size of the image to be sent */
  /* Image size is greater than Flash size */
  if (size > ApplicationMemorySizeMax)
  {
    /* End session */
    return 0;
  }
  /* Erase the needed pages where the user application will be loaded */
  /* Define the number of page to be erased */
  NbrOfSector = FLASH_PagesMask(size);
  /* Erase the FLASH pages */
  for(EraseCounter = 0; (EraseCounter < NbrOfSector) && (status == FLASH_COMPLETE); EraseCounter++)
  {
    status = FLASH_ErasePage(addr + (PageSize * EraseCounter));
  }
  return 1;
}

s8 Flash_Write (u8 *buf, u32 len)
{
  u32 i, RamSource=(u32)buf;
  for (i = 0;(i < len) && (FlashDestination <  ApplicationAddress + TotalSize);i += 4)
  {
    /* Program the data into Flash */
    FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
    if (*(u32*)FlashDestination != *(u32*)RamSource)
    {
      /* End session */
      return 0;
    }
    FlashDestination += 4;
    RamSource += 4;
  }
  return 1;
}



