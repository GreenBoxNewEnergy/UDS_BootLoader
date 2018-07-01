/*******************************************************************************
File - EncryptAlg1.c
Encryption algorithm.
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
#include "EncryptAlg1.h"

/**
* seedToKeyLevel1
*
* @param Seed - random seed
*       ECU_MASK - encryption parameters
* @return encryption key
* @date Jan. 18th 2017
*/
U32 seedToKeyLevel1(U32 Seed , U32 Mask)
{
	U32 Key = 0;
	U8 i = 0;
	if(Seed != 0)
	{
		for(i=0; i< 35; i++)
		{
			if(Seed & 0x80000000)
			{
				Seed = Seed<<1;
				Seed = Seed^Mask;
			}
			else
			{
				Seed = Seed<<1;
			}
		}
		Key = Seed;
	}
	return Key;
}
