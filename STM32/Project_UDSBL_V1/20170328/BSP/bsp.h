/*******************************************************************************
File - bsp.h
--
Copyright (C) 2014 Dunen Electric; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: Byron Wu
Date: 2014/11/26
Version: V1.0.0
*******************************************************************************/
#ifndef _BSP_H_
#define _BSP_H_
#include <stdint.h>
#include "stm32f10x.h"
#include "stm32f10x_crc.h"

#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 #define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
 #define FLASH_SIZE                        (0x20000)  /* 128 KBytes */
#elif defined STM32F10X_CL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
#elif defined STM32F10X_HD || defined (STM32F10X_HD_VL)
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x80000)  /* 512 KBytes */
#elif defined STM32F10X_XL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x100000) /* 1 MByte */
#else 
 #error "Please select first the STM32 device to be used (in stm32f10x.h)"    
#endif

//-----------------------------------------------------------------------------
// Exported types
//-----------------------------------------------------------------------------
/**
 * Structure contains timing coefficients for CAN module.
 *
 * @param SJW - (1...4) SJW time.
 *        BS1 - (1...16) Bit Segment 1.
 *        BS2 - (1...8) Bit Segment 2.
 *        PRESCALE - (1...1024) Baud Rate Prescaler.
 *
 * @note CAN baud rate is calculated from following equations:
 *       K = SJW + BS1 + BS2
 *       BaudRate = PCLK1/PRESCALE/K
 */
typedef struct{
  u16  PRESCALE;
  u8   SJW;
  u8   BS1;
  u8   BS2;
}CANbitRateData_t;

//-----------------------------------------------------------------------------
// Exported constants
//-----------------------------------------------------------------------------
#define FSYS 36000

#ifdef FSYS
   //Macros, which divides K into (SJW + BS1 + BS2)
   #define TQ_x_7    CAN_SJW_1tq,CAN_BS1_5tq,CAN_BS2_1tq
   #define TQ_x_8    CAN_SJW_1tq,CAN_BS1_5tq,CAN_BS2_2tq
   #define TQ_x_9    CAN_SJW_1tq,CAN_BS1_6tq,CAN_BS2_2tq
   #define TQ_x_10   CAN_SJW_1tq,CAN_BS1_7tq,CAN_BS2_2tq
   #define TQ_x_12   CAN_SJW_1tq,CAN_BS1_9tq,CAN_BS2_2tq
   #define TQ_x_14   CAN_SJW_1tq,CAN_BS1_11tq,CAN_BS2_2tq
   #define TQ_x_15   CAN_SJW_1tq,CAN_BS1_12tq,CAN_BS2_2tq  //good timing
   #define TQ_x_16   CAN_SJW_1tq,CAN_BS1_13tq,CAN_BS2_2tq   //good timing
   #define TQ_x_17   CAN_SJW_1tq,CAN_BS1_14tq,CAN_BS2_2tq   //good timing
   #define TQ_x_18   CAN_SJW_1tq,CAN_BS1_15tq,CAN_BS2_2tq   //good timing
   #define TQ_x_19   CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_2tq   //good timing
   #define TQ_x_20   CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_3tq   //good timing
   #define TQ_x_21   CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_4tq 
   #define TQ_x_22   CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_5tq 
   #define TQ_x_23   CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_6tq 
   #define TQ_x_24   CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_7tq 
   #define TQ_x_25   CAN_SJW_1tq,CAN_BS1_16tq,CAN_BS2_8tq 

   #if FSYS == 4000
      #define CANbitRateDataInitializers  \
      {20,  TQ_x_20},   /*CAN=10kbps*/       \
      {10,   TQ_x_20},   /*CAN=20kbps*/       \
      {4,   TQ_x_20},   /*CAN=50kbps*/       \
      {2,   TQ_x_16},   /*CAN=125kbps*/      \
      {2,   TQ_x_8 },   /*CAN=250kbps*/      \
      {2,   TQ_x_8 },   /*Not possible*/     \
      {2,   TQ_x_8 },   /*Not possible*/     \
      {2,   TQ_x_8 }    /*Not possible*/
   #elif FSYS == 8000
      #define CANbitRateDataInitializers  \
      {50,  TQ_x_16},   /*CAN=10kbps*/       \
      {20,  TQ_x_20},   /*CAN=20kbps*/       \
      {10,   TQ_x_16},   /*CAN=50kbps*/       \
      {4,   TQ_x_16},   /*CAN=125kbps*/      \
      {2,   TQ_x_16},   /*CAN=250kbps*/      \
      {2,   TQ_x_8 },   /*CAN=500kbps*/      \
      {2,   TQ_x_8 },   /*Not possible*/     \
      {2,   TQ_x_8 }    /*Not possible*/
   #elif FSYS == 12000
      #define CANbitRateDataInitializers  \
      {80,  TQ_x_15},   /*CAN=10kbps*/       \
      {40,  TQ_x_15},   /*CAN=20kbps*/       \
      {16,   TQ_x_15},   /*CAN=50kbps*/       \
      {6,   TQ_x_16},   /*CAN=125kbps*/      \
      {4,   TQ_x_12},   /*CAN=250kbps*/      \
      {2,   TQ_x_12},   /*CAN=500kbps*/      \
      {2,   TQ_x_12},   /*Not possible*/     \
      {2,   TQ_x_12}    /*Not possible*/
   #elif FSYS == 16000
      #define CANbitRateDataInitializers  \
      {100,  TQ_x_16},   /*CAN=10kbps*/       \
      {50,  TQ_x_16},   /*CAN=20kbps*/       \
      {20,  TQ_x_16},   /*CAN=50kbps*/       \
      {8,   TQ_x_16},   /*CAN=125kbps*/      \
      {4,   TQ_x_16},   /*CAN=250kbps*/      \
      {2,   TQ_x_16},   /*CAN=500kbps*/      \
      {2,   TQ_x_10},   /*CAN=800kbps*/      \
      {2,   TQ_x_8 }    /*CAN=1000kbps*/
   #elif FSYS == 20000
      #define CANbitRateDataInitializers  \
      {100,  TQ_x_20},   /*CAN=10kbps*/       \
      {50,  TQ_x_20},   /*CAN=20kbps*/       \
      {20,  TQ_x_20},   /*CAN=50kbps*/       \
      {10,   TQ_x_16},   /*CAN=125kbps*/      \
      {4,   TQ_x_20},   /*CAN=250kbps*/      \
      {2,   TQ_x_20},   /*CAN=500kbps*/      \
      {2,   TQ_x_20},   /*Not possible*/     \
      {2,   TQ_x_10}    /*CAN=1000kbps*/
   #elif FSYS == 24000
      #define CANbitRateDataInitializers  \
      {126,  TQ_x_19},   /*CAN=10kbps*/       \
      {80,  TQ_x_15},   /*CAN=20kbps*/       \
      {30,  TQ_x_16},   /*CAN=50kbps*/       \
      {12,   TQ_x_16},   /*CAN=125kbps*/      \
      {6,   TQ_x_16},   /*CAN=250kbps*/      \
      {4,   TQ_x_12},   /*CAN=500kbps*/      \
      {2,   TQ_x_15},   /*CAN=800kbps*/      \
      {2,   TQ_x_12}    /*CAN=1000kbps*/
   #elif FSYS == 32000
      #define CANbitRateDataInitializers  \
      {128,  TQ_x_25},   /*CAN=10kbps*/       \
      {100,  TQ_x_16},   /*CAN=20kbps*/       \
      {40,  TQ_x_16},   /*CAN=50kbps*/       \
      {16,   TQ_x_16},   /*CAN=125kbps*/      \
      {8,   TQ_x_16},   /*CAN=250kbps*/      \
      {4,   TQ_x_16},   /*CAN=500kbps*/      \
      {4,   TQ_x_10},   /*CAN=800kbps*/      \
      {2,   TQ_x_16}    /*CAN=1000kbps*/
   #elif FSYS == 36000
      #define CANbitRateDataInitializers  \
      {200,  TQ_x_18},   /*CAN=10kbps*/       \
      {100,  TQ_x_18},   /*CAN=20kbps*/       \
      {40,  TQ_x_18},   /*CAN=50kbps*/       \
      {16,   TQ_x_18},   /*CAN=125kbps*/      \
      {8,   TQ_x_18},   /*CAN=250kbps*/      \
      {4,   TQ_x_18},   /*CAN=500kbps*/      \
      {4,   TQ_x_18},   /*Not possible*/     \
      {2,   TQ_x_18}    /*CAN=1000kbps*/
   #elif FSYS == 40000
      #define CANbitRateDataInitializers  \
      {100,  TQ_x_20},   /*Not possible*/     \
      {100,  TQ_x_20},   /*CAN=20kbps*/       \
      {50,  TQ_x_16},   /*CAN=50kbps*/       \
      {20,  TQ_x_16},   /*CAN=125kbps*/      \
      {10,   TQ_x_16},   /*CAN=250kbps*/      \
      {4,   TQ_x_20},   /*CAN=500kbps*/      \
      {2,   TQ_x_25},   /*CAN=800kbps*/      \
      {2,   TQ_x_20}    /*CAN=1000kbps*/
   #elif FSYS == 48000
      #define CANbitRateDataInitializers  \
      {126,  TQ_x_19},   /*Not possible*/     \
      {126,  TQ_x_19},   /*CAN=20kbps*/       \
      {60,  TQ_x_16},   /*CAN=50kbps*/       \
      {24,  TQ_x_16},   /*CAN=125kbps*/      \
      {12,   TQ_x_16},   /*CAN=250kbps*/      \
      {6,   TQ_x_16},   /*CAN=500kbps*/      \
      {4,   TQ_x_15},   /*CAN=800kbps*/      \
      {4,   TQ_x_12}    /*CAN=1000kbps*/
   #elif FSYS == 56000
      #define CANbitRateDataInitializers  \
      {122,  TQ_x_23},   /*Not possible*/     \
      {122,  TQ_x_23},   /*CAN=20kbps*/       \
      {70,  TQ_x_16},   /*CAN=50kbps*/       \
      {28,  TQ_x_16},   /*CAN=125kbps*/      \
      {14,   TQ_x_16},   /*CAN=250kbps*/      \
      {8,   TQ_x_14},   /*CAN=500kbps*/      \
      {10,   TQ_x_7 },   /*CAN=800kbps*/      \
      {4,   TQ_x_14}    /*CAN=1000kbps*/
   #elif FSYS == 64000
      #define CANbitRateDataInitializers  \
      {128,  TQ_x_25},   /*Not possible*/     \
      {128,  TQ_x_25},   /*CAN=20kbps*/       \
      {80,  TQ_x_16},   /*CAN=50kbps*/       \
      {32,  TQ_x_16},   /*CAN=125kbps*/      \
      {16,   TQ_x_16},   /*CAN=250kbps*/      \
      {8,   TQ_x_16},   /*CAN=500kbps*/      \
      {4,   TQ_x_20},   /*CAN=800kbps*/      \
      {4,   TQ_x_16}    /*CAN=1000kbps*/
   #elif FSYS == 72000
      #define CANbitRateDataInitializers  \
      {80,  TQ_x_18},   /*Not possible*/     \
      {80,  TQ_x_18},   /*Not possible*/     \
      {80,  TQ_x_18},   /*CAN=50kbps*/       \
      {32,  TQ_x_18},   /*CAN=125kbps*/      \
      {16,   TQ_x_18},   /*CAN=250kbps*/      \
      {8,   TQ_x_18},   /*CAN=500kbps*/      \
      {6,   TQ_x_15},   /*CAN=800kbps*/      \
      {4,   TQ_x_18}    /*CAN=1000kbps*/
   #else
      #error define_FSYS FSYS not supported
   #endif
#endif

//-----------------------------------------------------------------------------
// Exported macro
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported functions
//-----------------------------------------------------------------------------
void STM_BSP_CAN1Init(u16 CANbitRate);
void STM_BSP_CAN1DeInit(void);
u8 STM_BSP_CAN_WriteData(CanTxMsg *TxMessage);
void STM_BSP_CAN_ConfigFilter(u32 ident, u32 mask, u8 idx, u8 rtr, u8 ide);

#endif  /*_BSP_H_*/

