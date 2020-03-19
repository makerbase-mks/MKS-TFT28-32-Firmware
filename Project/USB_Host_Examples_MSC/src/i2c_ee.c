/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : i2c_ee.c
* Author             : MCD Application Team
* Version            : V2.0.1
* Date               : 06/13/2008
* Description        : This file provides a set of functions needed to manage the
*                      communication between I2C peripheral and I2C M24C08 EEPROM.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_i2c.h"
#include "i2c_ee.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define I2C_Speed_Sm		100000

//#define I2C_Speed              400000
#define I2C1_SLAVE_ADDRESS7    0xA0
#define I2C_PageSize           8

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u16 EEPROM_ADDRESS;

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void I2C_Configuration(void);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure the used I/O ports pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    
  /* Configure I2C1 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : I2C Configuration
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_Configuration(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 

  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
  
  /* Apply I2C configuration after enabling it */
  I2C_Init(I2C1, &I2C_InitStructure);

  /* I2C Peripheral Enable */
  I2C_Cmd(I2C1, ENABLE);
    
  //printf("\n\r I2C_Configuration----\n\r");
    
}

/*******************************************************************************
* Function Name  : I2C_EE_Init
* Description    : Initializes peripherals used by the I2C EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void I2C_EE_Init()
void I2C_EE_Init(uint32_t I2c_speed_value)
{
  /* GPIO configuration */
  //GPIO_Configuration();

  /* I2C configuration */
  //I2C_Configuration();

  GPIO_InitTypeDef  GPIO_InitStructure; 
  I2C_InitTypeDef  I2C_InitStructure; 
  //设置GPIO67为高电平。
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7);
  
  I2C_DeInit(I2C1);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    
  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  //I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
  I2C_InitStructure.I2C_ClockSpeed = I2c_speed_value;
  
  /* Apply I2C configuration after enabling it */
  I2C_Init(I2C1, &I2C_InitStructure);
   /* I2C Peripheral Enable */
  I2C_Cmd(I2C1, ENABLE);
   
  /* Configure I2C1 pins: SCL and SDA */  
  //GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* depending on the EEPROM Address selected in the i2c_ee.h file */
#ifdef EEPROM_Block0_ADDRESS
  /* Select the EEPROM Block0 to write on */
  EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
#endif
#ifdef EEPROM_Block1_ADDRESS
  /* Select the EEPROM Block1 to write on */
  EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
#endif
#ifdef EEPROM_Block2_ADDRESS
  /* Select the EEPROM Block2 to write on */
  EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
#endif
#ifdef EEPROM_Block3_ADDRESS
  /* Select the EEPROM Block3 to write on */
  EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
#endif
}

/*******************************************************************************
* Function Name  : I2C_EE_BufferWrite
* Description    : Writes buffer of data to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % I2C_PageSize;  //0x00%8=0
  count = I2C_PageSize - Addr;		//8-0=8
  NumOfPage =  NumByteToWrite / I2C_PageSize;	//256/8	=32
  NumOfSingle = NumByteToWrite % I2C_PageSize;	//256%8	=0
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)  //一次写8个数据
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize); 
    	I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if(NumOfSingle!=0)	//除8个以外的
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }  
}

/*******************************************************************************
* Function Name  : I2C_EE_ByteWrite
* Description    : Writes one byte to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr)
{
  /* Send STRAT condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
		#if debug_flg == 1
			T_function(250,1);
		#endif
  }

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
		#if debug_flg == 1
			T_function(300,1);
		#endif
  }      
  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2C1, WriteAddr);
  
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
	#if debug_flg == 1
			T_function(350,1);
	#endif
  }

  /* Send the byte to be written */
  I2C_SendData(I2C1, *pBuffer); 
   
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  {
		#if debug_flg == 1
			T_function(400,1);
		#endif
  } 
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_PageWrite
* Description    : Writes more than one byte to the EEPROM with a single WRITE
*                  cycle. The number of byte can't exceed the EEPROM page size.
* Input          : - pBuffer : pointer to the buffer containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) // Added by Najoua 27/08/2008
   {
		#if debug_flg == 1
			T_function(450,1);
		#endif
  }   
  /* 发送启动条件 */
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
		#if debug_flg == 1
			T_function(500,1);
		#endif
  }  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
  {
	#if debug_flg == 1
			T_function(550,1);
	#endif
  }

  /* Send the EEPROM's internal address to write to */    
  I2C_SendData(I2C1, WriteAddr);  

  /* Test on EV8 and clear it */
  while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
	#if debug_flg == 1
			T_function(600,1);
	#endif
  }

  /* While there is data to be written */
  while(NumByteToWrite--)  
  {
    /* Send the current byte */
    I2C_SendData(I2C1, *pBuffer); 

    /* Point to the next byte to be written */
    pBuffer++; 
  
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			#if debug_flg == 1
				T_function(650,1);
			#endif
  	}
  }

  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_BufferRead
* Description    : Reads a block of data from the EEPROM.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the EEPROM.
*                  - ReadAddr : EEPROM's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{  
  //*((u8 *)0x4001080c) |=0x80; 
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) // Added by Najoua 27/08/2008
  {
		#if debug_flg == 1
			T_function(700,1);
		#endif
  }    
    
  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  //*((u8 *)0x4001080c) &=~0x80;
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
		#if debug_flg == 1
				T_function(750,1);
		#endif
  }

  /* Send EEPROM address for write */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
		#if debug_flg == 1
			T_function(800,1);
		#endif
  }  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(I2C1, ReadAddr);  

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
		#if debug_flg == 1
			T_function(850,1);
		#endif
  }  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(I2C1, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
		#if debug_flg == 1
			T_function(900,1);
		#endif
  }  
  /* Send EEPROM address for read */
  I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Receiver);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
		#if debug_flg == 1
			T_function(950,1);
		#endif
  }  
  /* While there is data to be read */
  while(NumByteToRead)  
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      
      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C1, ENABLE);
    }

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  
    {      
      /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData(I2C1);

      /* Point to the next location where the byte read will be saved */
      pBuffer++; 
      
      /* Decrement the read bytes counter */
      NumByteToRead--;        
    }   
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C1, ENABLE);

  //I2C_EE_Init(100000);
}

/*******************************************************************************
* Function Name  : I2C_EE_WaitEepromStandbyState
* Description    : Wait for EEPROM Standby state
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_WaitEepromStandbyState(void)      
{
  vu16 SR1_Tmp = 0;

  do
  {
    /* Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* Read I2C1 SR1 register */
    SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
		#if debug_flg == 1
			T_function(1000,1);
		#endif
  }while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));
  
  /* Clear AF flag */
  I2C_ClearFlag(I2C1, I2C_FLAG_AF);
    /* STOP condition */    
    I2C_GenerateSTOP(I2C1, ENABLE); // Added by Najoua 27/08/2008
}

#if 0
void I2C_Test(void)
{
    u16 i;
    u8 I2c_Buf_Write[256];
    u8 I2c_Buf_Read[256];

    //printf("写入的数据\n\r");

    for(i=0;i<=255;i++) //填充缓冲
    {   
        I2c_Buf_Write[i]=i;
        //printf("0x%02X ",I2c_Buf_Write[i]);
        if(i%16 == 15)
        {
            //printf("\n\r");
        }
    }

    //将I2c_Buf_Write中顺序递增的数据写入EERPOM中 
    I2C_EE_BufferWrite(I2c_Buf_Write,EEP_Firstpage,256);	 

    //printf("\n\r读出的数据\n\r");
    //将EEPROM读出数据顺序保持到I2c_Buf_Read中 
    I2C_EE_BufferRead(I2c_Buf_Read,EEP_Firstpage,256); 

    //将I2c_Buf_Read中的数据通过串口打印
    for(i=0;i<256;i++)
    {	
        if(I2c_Buf_Read[i]!=I2c_Buf_Write[i])
        {
            //printf("0x%02X ", I2c_Buf_Read[i]);
            //printf("错误:I2C EEPROM写入与读出的数据不一致\n\r");
            return;
        }
        //printf("0x%02X ", I2c_Buf_Read[i]);
        if(i%16 == 15)
        {
            //printf("\n\r");
        }
    }
    //printf("读写测试通过PASSED\n\r");
}
#endif
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
