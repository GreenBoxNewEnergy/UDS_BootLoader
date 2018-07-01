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
  SCB->VTOR = 0x08004000;
  U32 timer1msPrevious = 0;
  /* Init CAN1 at 250kbps */ 
  STM_BSP_CAN1Init(500);
  /* Enable BKP and PWR clocks */ 
  /* SysTick 1ms */
  SysTick_Config(9000);
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
  
  __disable_irq();
  FLASH_Unlock();
  EE_Init();
  //U16 uwData;
  //EE_ReadVariable(APP_FLAG_ADDR, &uwData);
  FLASH_Lock();
  __enable_irq();
  //deskey("29650280",DE1);
  /* Init UDS */
  UDS_Init(&UDS);
  timer1msPrevious = UDS_timer1ms;
  while(1)
  {
    UDS_Process(UDS, UDS_GetTimeDiff(&timer1msPrevious));
    UDS_LongOperation_Process();
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

