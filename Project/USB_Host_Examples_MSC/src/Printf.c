/**
  ******************************************************************************
  * @file    Printf.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm3210C_eval.h"
#include "Printf.h"
#include <stdio.h>

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Printf
  * @{
  */ 

const uint8_t STM32F10x_STR[] = {"\r\n"\
           "  _____ _______ __  __ ____ ___  ______ __  ___\r\n"\
           " / ____|__   __|  \\/  |___ \\__ \\|  ____/_ |/ _ \\\r\n"\
           "| (___    | |  | \\  / | __) | ) | |__   | | | | |_  __\r\n"\
           " \\___ \\   | |  | |\\/| ||__ < / /|  __|  | | | | \\ \\/ /\r\n"\
           " ____) |  | |  | |  | |___) / /_| |     | | |_| |>  <\r\n"\
           "|_____/   |_|  |_|  |_|____/____|_|     |_|\\___//_/\\_\\神舟系列开发板\r\n"\
           "\r\n"};

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifndef EVAL_COM1_STR
	#ifdef  USE_STM3210C_EVAL
	    #define EVAL_COM1_STR                    "USART2"
	#else
	    #define EVAL_COM1_STR                    "USART1"
	#endif
#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
unsigned char USART1_putchar(unsigned char ch);
unsigned char USART2_putchar(unsigned char ch);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void Printf_Init(void)
{
  USART_InitTypeDef USART_InitStructure;   
       
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);

  /* Output a message on Hyperterminal using printf function */
  //printf("\n\rUSART Printf Example: retarget the C library printf function to the USART\n\r");
  printf("\r\n\n\n\r WWW.ARMJISHU.COM  %s printf configured....", EVAL_COM1_STR);
  printf("\n\r ############ WWW.ARMJISHU.COM! ############ ("__DATE__ " - " __TIME__ ")");

  printf("%s", STM32F10x_STR);

  printf(" WWW.ARMJISHU.COM use __STM32F10X_STDPERIPH_VERSION %d.%d.%d",
			__STM32F10X_STDPERIPH_VERSION_MAIN,
			__STM32F10X_STDPERIPH_VERSION_SUB1,
			__STM32F10X_STDPERIPH_VERSION_SUB2);
  printf("\n\r 产品内部Flash大小为：%dK字节！ \t www.armjishu.com",
            *(__IO uint16_t*)(0x1FFFF7E0));
  SystemCoreClockUpdate();
  printf("\n\r 系统内核时钟频率(SystemCoreClock)为：%dHz.\n\r",
            SystemCoreClock);
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}
#if 0
/**
  * @}
  */ 
unsigned char USART2_putchar(unsigned char ch)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM2, (uint8_t) ch); /*发送一个字符函数*/ 

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM2, USART_FLAG_TC) == RESET)/*等待发送完成*/
  {
  
  }
  return ch;
}
#endif

unsigned char USART1_putchar(unsigned char ch)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch); /*发送一个字符函数*/ 

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)/*等待发送完成*/
  {
  
  }
  return ch;
}
/**
  * @}
  */ 

#if 0
/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param  key: The key pressed
  * @retval 1: Correct
  *         0: Error
  */
uint32_t USART2_ValidInput(void)
{

  if ( USART_GetFlagStatus(EVAL_COM2, USART_FLAG_RXNE) != RESET)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
#endif

/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param  key: The key pressed
  * @retval 1: Correct
  *         0: Error
  */
uint32_t USART1_ValidInput(void)
{

  if ( USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) != RESET)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

#if 0
uint8_t USART2_getchar(void)
{
  uint8_t key = 0;

  /* Waiting for user input */
  while (1)
  {
    if ( USART_GetFlagStatus(EVAL_COM2, USART_FLAG_RXNE) != RESET)
    {
      key = (uint8_t)EVAL_COM2->DR & 0xFF;
      break;
    }
  }
  return key;
}
#endif

uint8_t USART1_getchar(void)
{
  uint8_t key = 0;

  /* Waiting for user input */
  while (1)
  {
    if ( USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) != RESET)
    {
      key = (uint8_t)EVAL_COM1->DR & 0xFF;
      break;
    }
  }
  return key;
}

/**
  * @brief  Get Input string from the HyperTerminal
  * @param  buffP: The input string
  * @retval None
  */
uint32_t USART1_gets_nowait (uint8_t * buffP, uint32_t length)
{
    static uint32_t bytes_read = 0;
    uint8_t c = 0;
  
    if(USART1_ValidInput())
    {
        c = USART1_getchar();
        if (c == '\r')
        {
          USART1_putchar('\n');
          USART1_putchar('\r');          
          buffP[bytes_read] = '\0';
          bytes_read = 0;

          return 1;
        }

        if (c == '\b') /* Backspace */
        {
          if (bytes_read > 0)
          {
            USART1_putchar('\b');
            USART1_putchar(' ');
            USART1_putchar('\b');
            bytes_read--;
          }
          return 0;          
        }
        //if (bytes_read >= (CMD_STRING_SIZE))
        if (bytes_read >= (length))
        {
          printf("Command string size overflow\r\n");
          bytes_read = 0;
          return 0;
        }
        if (c >= 0x20 && c <= 0x7E)
        {
          buffP[bytes_read++] = c;
          USART1_putchar(c);
        }
    }
    
    return 0;
}

uint32_t USART2_gets_nowait (uint8_t * buffP, uint32_t length)
{
    static uint32_t bytes_read = 0;
    uint8_t c = 0;
  
    if(USART2_ValidInput())
    {
        c = USART2_getchar();
        if (c == '\r')
        {
          USART2_putchar('\n');
          USART2_putchar('\r');          
          buffP[bytes_read] = '\0';
          bytes_read = 0;

          return 1;
        }

        if (c == '\b') /* Backspace */
        {
          if (bytes_read > 0)
          {
            USART2_putchar('\b');
            USART2_putchar(' ');
            USART2_putchar('\b');
            bytes_read--;
          }
          return 0;          
        }
        //if (bytes_read >= (CMD_STRING_SIZE))
        if (bytes_read >= (length))
        {
          //printf("Command string size overflow\r\n");
          bytes_read = 0;
          return 0;
        }
        if (c >= 0x20 && c <= 0x7E)
        {
          buffP[bytes_read++] = c;
          USART2_putchar(c);
        }
    }
    
    return 0;
}
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
