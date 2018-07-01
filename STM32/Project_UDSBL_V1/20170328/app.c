/*******************************************************************************
File - app.c

Copyright (C) 2017 Dunen; Nanjing
All rights reserved.  Protected by international copyright laws.
*/
/*
Author: WenHui Wu

Date: Jan. 24th 2017
Version: V1.0.0
*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "bsp.h"
#include "app.h"

/* pointer to UDS object */
UDS_t *UDS;

/* bootloader states */
volatile TASK_STATES task_states = CHECK_EXIT_PROG;

/* variable increments each millisecond */
volatile U32 UDS_timer1ms=0; 

/**
* UDS_GetTimeDiff
*
* @param timer1msPrevious - previous stored timestamp 
* @return compares two time intervals and returns the time. 
*         difference in milliseconds.
* @date Jan. 24th 2017
*/
U32 UDS_GetTimeDiff(U32 *timer1msPrevious)
{
  U32 timer1msCopy, timer1msDiff;
  timer1msCopy = UDS_timer1ms;
  if(timer1msCopy>=*timer1msPrevious)
  {
    timer1msDiff = timer1msCopy - *timer1msPrevious;
  }
  else
  {
    timer1msDiff = U32_MAX - *timer1msPrevious;
    timer1msDiff += timer1msCopy;
  }
  *timer1msPrevious = timer1msCopy;
  return timer1msDiff;
}

/**
* main
*
* @date Feb. 17th 2017
*/
int main(void)
{
  U32 timer1msPrevious = 0;
  /* Init CAN1 at 500kbps */ 
  STM_BSP_CAN1Init(500);
  /* Enable BKP and PWR clocks */ 
  /* SysTick 1ms */
  SysTick_Config(9000);
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
  __disable_irq();
  FLASH_Unlock();
  EE_Init();
  U16 uwData;
  U16 uwData1;
  EE_ReadVariable(APP_FLAG_ADDR, &uwData);
  EE_ReadVariable(EXT_PROG_FLAG_ADDR, &uwData1);
  FLASH_Lock();
  __enable_irq();
  /* Backup data register value is 0xA5A5 */
  if(uwData==0x5A5A && uwData1!=0x0001)
	{
    /* Wait 20ms until a specific CAN frame being received */
    while(task_states==CHECK_EXIT_PROG && UDS_timer1ms<=20);
    if(task_states == ENTER_BOOTLOADER)
    {
      CanTxMsg TxMessage;
      TxMessage.DLC = 8;
      TxMessage.IDE = CAN_Id_Standard;
      TxMessage.RTR = CAN_RTR_Data;
      TxMessage.StdId = PARTICULAR_CAN_TX_ID;
      TxMessage.Data[0] = 0x05;
      TxMessage.Data[1] = 0x71;
      TxMessage.Data[2] = 0x01;
      TxMessage.Data[3] = 0x02;
      TxMessage.Data[4] = 0x03;
      TxMessage.Data[5] = 0x00;
      TxMessage.Data[6] = 0x00;
      TxMessage.Data[7] = 0x00;
      STM_BSP_CAN_WriteData(&TxMessage);
      __disable_irq();
      FLASH_Unlock();
      EE_WriteVariable(APP_FLAG_ADDR, 0x0000);
      FLASH_Lock();
      __enable_irq();
    }
  }
  else
  {
    task_states = ENTER_BOOTLOADER;
  }
  while(1)
  {
    switch(task_states)
    {
    case CHECK_EXIT_PROG:
      task_states = RUN_APP;
    case RUN_APP:
      /* Test if user code is programmed starting from address "ApplicationAddress" */
      if ((*(vu32*)ApplicationAddress & 0x2FFE0000 ) == 0x20000000)
      { /* Jump to user application */
        JumpAddress = *(vu32*) (ApplicationAddress + 4);
        Jump_To_Application = (pFunction) JumpAddress;
        NVIC_SetVectorTable(0x8000000, 0x4000);
        STM_BSP_CAN1DeInit();
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        __set_MSP(*(volatile u32*) ApplicationAddress);
        Jump_To_Application();
      }
      else
      {
        __disable_irq();
        FLASH_Unlock();
        EE_WriteVariable(APP_FLAG_ADDR, 0x0000);
        FLASH_Lock();
        __enable_irq();
        task_states = ENTER_BOOTLOADER;
      }
      break;
    case ENTER_BOOTLOADER:
      deskey("29650280",DE1);
      /* Init UDS */
      UDS_Init(&UDS);
      if(uwData1 == 0x0001)
      {
        __disable_irq();
        FLASH_Unlock();
        EE_WriteVariable(EXT_PROG_FLAG_ADDR, 0x0000);
        FLASH_Lock();
        __enable_irq();
        UDS->UDS_RAM->Session = ECU_PROG_SESSN;
        UDS->UDS_RAM->PreProgCondition = UDS_TRUE;
      }
      timer1msPrevious = UDS_timer1ms;
      task_states = RUN_BOOTLOADER;
      break;
    case RUN_BOOTLOADER:
      UDS_Process(UDS,UDS_GetTimeDiff(&timer1msPrevious));
      UDS_LongOperation_Process();
      break;
    }
  }
}

#define RETARGET_PRINTF 0
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#if RETARGET_PRINTF
/**
* @brief  Retargets the C library printf function to the USART.
* @param  None
* @retval None
*/
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  //  USART_SendData(BSP_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  //  while (USART_GetFlagStatus(BSP_COM1, USART_FLAG_TC) == RESET)
  //  {}
  return ch;
}
#endif

#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

