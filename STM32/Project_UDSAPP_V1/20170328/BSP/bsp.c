/*******************************************************************************
File - bsp.c
--
Copyright (C) 2014 Dunen Electric; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: Byron Wu
Date: 2014/11/26
Version: V1.0.0
*******************************************************************************/
#include "bsp.h"
//-----------------------------------------------------------------------------
// Private typedef
//-----------------------------------------------------------------------------
CANbitRateData_t  CANbitRateData[8] = {CANbitRateDataInitializers};

//-----------------------------------------------------------------------------
// Private define
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private macro
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------
/**
 * Init CAN module.
 *
 * @param CANbitRate - CAN baudrate.
 */
void STM_BSP_CAN1Init(u16 CANbitRate)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  CAN_InitTypeDef        CAN_InitStructure;
  NVIC_InitTypeDef nvic_init; 
  
  /* Enable CAN1 and GPIO clocks */
  RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd (RCC_APB1Periph_CAN1, ENABLE);
  
  /* Configure CAN pins: TX and RX */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU; 
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  
  nvic_init.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_init.NVIC_IRQChannelSubPriority = 1;
  nvic_init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_init);
  
  nvic_init.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
  NVIC_Init(&nvic_init);
  
  // Configure CAN timing
  u8 i;
  switch(CANbitRate){
  case 10:   i=0; break;
  case 20:   i=1; break;
  case 50:   i=2; break;
  case 125:  i=3; break;
  case 250:  i=4; break;
  case 500:  i=5; break;
  case 800:  i=6; break;
  case 1000: i=7; break;
  default: i=7; break;
  }
  
  /* CAN1 register init */
  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  /* CAN1 cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = ENABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  CAN_InitStructure.CAN_SJW = CANbitRateData[i].SJW;
  CAN_InitStructure.CAN_BS1 = CANbitRateData[i].BS1;
  CAN_InitStructure.CAN_BS2 = CANbitRateData[i].BS2;
  CAN_InitStructure.CAN_Prescaler = CANbitRateData[i].PRESCALE;
  CAN_Init(CAN1, &CAN_InitStructure);
  
  //CAN module hardware filters
  STM_BSP_CAN_ConfigFilter(0x602, 0x7FF, 0, 0, 0);
  
  // Enable FIFO message pending interrupt
  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE); 
  
  CAN_ITConfig(CAN1, CAN_IT_EWG, ENABLE); /* Enable Error warning interrupt */
	CAN_ITConfig(CAN1, CAN_IT_EPV, ENABLE); /* Enable Error passive interrupt */
	CAN_ITConfig(CAN1, CAN_IT_ERR, ENABLE); /* Enable Error interrupt */
	CAN_ITConfig(CAN1, CAN_IT_BOF, ENABLE); /* Enable Bus off interrupt */
	CAN_ITConfig(CAN1, CAN_IT_LEC, ENABLE); /* Enable Last Error code interrupt */
}

u8 STM_BSP_CAN_WriteData(CanTxMsg *TxMessage)
{
  u8 ret;
	u8 TransmitMailbox;   
	u32	TimeOut=0;
  TransmitMailbox = CAN_Transmit(CAN1,TxMessage);
	while((ret=CAN_TransmitStatus(CAN1,TransmitMailbox))!=CAN_TxStatus_Ok){
		TimeOut++;
		if(TimeOut > 10000000){
			break;
		}
	}
	return ret;
}

void STM_BSP_CAN_ConfigFilter(u32 ident, u32 mask, u8 idx, u8 rtr, u8 ide)
{
  u32 RXF, RXM;
  if(ide)
  {
    RXF = ident<<3;
    RXM = mask<<3;
  }
  else
  {
    RXF = ident<<21;
    RXM = mask<<21;
  }
  RXM |= 0x7;
  CAN_FilterInitTypeDef  can_filterinit;
  can_filterinit.CAN_FilterNumber=idx;
  can_filterinit.CAN_FilterMode=CAN_FilterMode_IdMask;
  can_filterinit.CAN_FilterScale=CAN_FilterScale_32bit;
  can_filterinit.CAN_FilterIdHigh=(RXF>>16)&0xFFFF;
  if(ide)
    can_filterinit.CAN_FilterIdLow=(RXF|CAN_ID_EXT|(!rtr?CAN_RTR_DATA:CAN_RTR_REMOTE))&0xFFFF;
  else
    can_filterinit.CAN_FilterIdLow=(RXF|CAN_ID_STD|(!rtr?CAN_RTR_DATA:CAN_RTR_REMOTE))&0xFFFF;
  can_filterinit.CAN_FilterMaskIdHigh=(RXM>>16)&0xFFFF;
  can_filterinit.CAN_FilterMaskIdLow=RXM&0xFFFF;
  can_filterinit.CAN_FilterFIFOAssignment=CAN_FIFO0;
  can_filterinit.CAN_FilterActivation=ENABLE;
  CAN_FilterInit(&can_filterinit);
}
