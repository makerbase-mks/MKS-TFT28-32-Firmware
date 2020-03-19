/******************************************************************************
 * 文件名  ：SPI_FLASH.c
 * 描述    ：SPI存储器W25Q16_W2XQ16底层函数
 * 标准库  ：STM32F10x_StdPeriph_Driver V3.5.0
 * 作者    ：
*******************************************************************************/
#include "stm32f10x.h"
#include "SPI_FLASH.h"
#include <string.h>
#include <stdio.h>

#ifdef _SPI_FLASH_H

/********************************************************************************/
//#define PIC_MAX_CN		90				//最大的图片数


union union32
{
	uint8_t bytes[4];
	uint32_t dwords;
};
//图片信息结构体
struct pic_msg
{
	uint8_t name[PIC_NAME_MAX_LEN];
	union union32 size;
};

typedef struct pic_msg PIC_MSG;

uint8_t DMA_ERRO_FLAG = 0;

//定义图片结构体数组
//PIC_MSG PIC[PIC_MAX_CN];

// uint8_t Pic_cnt;//图片总数值变量

#define PER_PIC_MAX_SPACE		(16*1024)	//为了防止出现越界的问题，每张小图片的区间划分应该取能够整除4K的值

/*********************************************************************************/
#define SPI_FLASH_SPI                   SPI1
#define SPI_FLASH_SPI_CLK               RCC_APB2Periph_SPI1
#define SPI_FLASH_SPI_SCK_PIN           GPIO_Pin_5              /* PA.05 */
#define SPI_FLASH_SPI_SCK_GPIO_PORT     GPIOA
#define SPI_FLASH_SPI_SCK_GPIO_CLK      RCC_APB2Periph_GPIOA
#define SPI_FLASH_SPI_MISO_PIN          GPIO_Pin_6              /* PA.06 */
#define SPI_FLASH_SPI_MISO_GPIO_PORT    GPIOA
#define SPI_FLASH_SPI_MISO_GPIO_CLK     RCC_APB2Periph_GPIOA
#define SPI_FLASH_SPI_MOSI_PIN          GPIO_Pin_7              /* PA.07 */
#define SPI_FLASH_SPI_MOSI_GPIO_PORT    GPIOA
#define SPI_FLASH_SPI_MOSI_GPIO_CLK     RCC_APB2Periph_GPIOA
#define SPI_FLASH_CS_PIN_NUM            9                       /* PB.09 */
#define SPI_FLASH_CS_PIN                GPIO_Pin_9  
#define SPI_FLASH_CS_GPIO_PORT          GPIOB
#define SPI_FLASH_CS_GPIO_CLK           RCC_APB2Periph_GPIOB


/* Private typedef -----------------------------------------------------------*/
#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256

/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable                0x06 
#define W25X_WriteDisable               0x04 
#define W25X_ReadStatusReg              0x05 
#define W25X_WriteStatusReg             0x01 
#define W25X_ReadData                   0x03 
#define W25X_FastReadData               0x0B 
#define W25X_FastReadDual               0x3B 
#define W25X_PageProgram                0x02 
#define W25X_BlockErase                 0xD8 
#define W25X_SectorErase                0x20 
#define W25X_ChipErase                  0xC7 
#define W25X_PowerDown                  0xB9 
#define W25X_ReleasePowerDown           0xAB 
#define W25X_DeviceID                   0xAB 
#define W25X_ManufactDeviceID           0x90 
#define W25X_JedecDeviceID              0x9F 

#define WIP_FlagMask                    0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte                      0xA5

/* Private macro -------------------------------------------------------------*/
#if defined(STM32F10X_CL)
	#define SPI_FLASH_CS_LOW()       GPIOB->BRR |= 1<<9;//GPIO_ResetBits(GPIOB, GPIO_Pin_9)	//片选引脚/CS拉低
	#define SPI_FLASH_CS_HIGH()      GPIOB->BSRR |= 1<<9;//GPIO_SetBits(GPIOB, GPIO_Pin_9)  //片选引脚/CS拉高
#endif

#if defined(STM32F10X_HD)
	#define SPI_FLASH_CS_LOW()       GPIOA->BRR |= 1<<3;//GPIO_ResetBits(GPIOA, GPIO_Pin_3)	//片选引脚/CS拉低
	#define SPI_FLASH_CS_HIGH()      GPIOA->BSRR |= 1<<3;//GPIO_SetBits(GPIOA, GPIO_Pin_3)  //片选引脚/CS拉高
#endif
// /* Private variables ---------------------------------------------------------*/
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = PASSED;

/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_Init
  * @功能   初始化与外部SPI FLASH接口的驱动函数
  *         Initializes the peripherals used by the SPI FLASH driver.
  * @参数   无
  * @返回值 无
***----------------------------------------------------------------*/
void SPI_FLASH_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能SPI1和GPIOA、GPIOB的RCC时钟*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    /*!初始化时钟信号线SCK */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*初始化SPI_FLASH_SPI管脚: MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*初始化SPI_FLASH_SPI管脚: MISO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

#if defined(STM32F10X_CL)
    /*初始化片选信号管脚: /CS*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
#if defined(STM32F10X_HD)
    /*初始化片选信号管脚: /CS*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

    /*拉高片选信号*/
    SPI_FLASH_CS_HIGH();

    /* SPI1初始化配置*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  //主从模式选择，SCK信号有主机产生
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //SPI通信数据的大小
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//时钟极性
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;   //时钟相位
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	  //片选信号，硬件模式还是软件模式
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	//SPI分频值，分频后的值为SCK的时钟频率
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//选择SPI通信时，是高位数据在前还是低位数据在前
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//与CRC校验有关
    SPI_Init(SPI1, &SPI_InitStructure);

    /*使能SPI1 */
    SPI_Cmd(SPI1, ENABLE);
	
		/*唤醒SPI FLASH*/
    SPI_Flash_WAKEUP();
}

/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_SectorErase
  * @功能   擦除SPI FLASH一个扇区的驱动函数
  *         Erases the specified FLASH sector.
  * @参数   SectorAddr: 扇区地址 address of the sector to erase.
  * @返回值 无
***----------------------------------------------------------------*/
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
    SPI_FLASH_WriteEnable();

    /* 拉低片选信号*/
    SPI_FLASH_CS_LOW();
    /* Send Sector Erase instruction */
    SPI_FLASH_SendByte(W25X_SectorErase);
    /* Send SectorAddr high nibble address byte */
    SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
    /* Send SectorAddr medium nibble address byte */
    SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
    /* Send SectorAddr low nibble address byte */
    SPI_FLASH_SendByte(SectorAddr & 0xFF);

    /* 拉高片选信号 */
    SPI_FLASH_CS_HIGH();

    /* 等待写入SPI的操作完成*/
    SPI_FLASH_WaitForWriteEnd();
}

/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_ChipErase
  * @功能   擦除整个SPI FLASH的驱动函数
  *         Erases the entire FLASH.
  * @参数   无
  * @返回值 无
***----------------------------------------------------------------*/
void SPI_FLASH_ChipErase(void)
{
    /* Send write enable instruction */
    SPI_FLASH_WriteEnable();

    /* Bulk Erase */
    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();
    /* Send Bulk Erase instruction  */
    SPI_FLASH_SendByte(W25X_ChipErase);
    /* Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    /* Wait the end of Flash writing */
    SPI_FLASH_WaitForWriteEnd();
}

/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_PageWrite
  * @功能   单个写周期写入大于一个字节而小于等于一页大小的数据
  *         Writes more than one byte to the FLASH with a single 
  *         WRITE cycle(Page WRITE sequence). The number of byte 
  *         can't exceed the FLASH page size.
  * @参数   - pBuffer : 指向包含写入数据缓冲器的地址指针
  *             pointer to the buffer  containing the data to be
  *             written to the FLASH.
  *         - WriteAddr : flash的写入地址
  *             FLASH's internal address to write to.
  *         - NumByteToWrite : 写入的字节数
  *             number of bytes to write to the FLASH, must be
  *             equal or less than "SPI_FLASH_PageSize" value.
  * @返回值 无
***----------------------------------------------------------------*/
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    /* Enable the write access to the FLASH */
    SPI_FLASH_WriteEnable();

    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();
    /* Send "Write to Memory " instruction */
    SPI_FLASH_SendByte(W25X_PageProgram);
    /* Send WriteAddr high nibble address byte to write to */
    SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
    /* Send WriteAddr medium nibble address byte to write to */
    SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
    /* Send WriteAddr low nibble address byte to write to */
    SPI_FLASH_SendByte(WriteAddr & 0xFF);

    if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
    {
        NumByteToWrite = SPI_FLASH_PerWritePageSize;
    }

    /* while there is data to be written on the FLASH */
    while (NumByteToWrite--)
    {
        /* Send the current byte */
        SPI_FLASH_SendByte(*pBuffer);
        /* Point on the next byte to be written */
        pBuffer++;
    }

    /* Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    /* Wait the end of Flash writing */
    SPI_FLASH_WaitForWriteEnd();
}

/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_BufferWrite
  * @功能   向SPI FLASH写入一堆数据，写入的字节数可以大于一页容量
  *         Writes block of data to the FLASH. In this function,
  *         the number of WRITE cycles are reduced,
  *         using Page WRITE sequence.
  * @参数   - pBuffer : 指向包含写入数据缓冲器的地址指针
  *             pointer to the buffer  containing the data to be
  *             written to the FLASH.
  *         - WriteAddr : flash的写入地址
  *             FLASH's internal address to write to.
  *         - NumByteToWrite : 写入的字节数
  *             number of bytes to write to the FLASH.
  * @返回值 无
***----------------------------------------------------------------*/
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    Addr = WriteAddr % SPI_FLASH_PageSize;
    count = SPI_FLASH_PageSize - Addr;
    NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
    NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

    if (Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
    {
        if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
        {
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            while (NumOfPage--)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        }
    }
    else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
    {
        if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
        {
            if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
            {
                temp = NumOfSingle - count;

                SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
                WriteAddr +=  count;
                pBuffer += count;

                SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
            }
            else
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
            }
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
            NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

            SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
            WriteAddr +=  count;
            pBuffer += count;

            while (NumOfPage--)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            if (NumOfSingle != 0)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_BufferRead
  * @功能   从SPI FLASH读出一段数据，
  *         Reads a block of data from the FLASH.
  * @参数   - pBuffer : 指向包含写入数据缓冲器的地址指针
  *             pointer to the buffer that receives the data read
  *             from the FLASH.
  *         - ReadAddr : flash的读起始地址
  *             FLASH's internal address to read from.
  *         - NumByteToWrite : 读出的字节数
  *             number of bytes to read from the FLASH.
  * @返回值 无
***----------------------------------------------------------------*/
#define SPI1_DR_Addr ((u32)0x4001300C)
static uint8_t DuumyClock[1]={0xff};
extern unsigned char bmp_public_buf[16 * 1024];
static uint32_t dummynum = 100000;
void dma_init()
{
#if 1
	DMA_InitTypeDef  DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Channel2);
	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)SPI1_DR_Addr;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)bmp_public_buf;//pBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 16*1024;//NumByteToRead;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);

	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)bmp_public_buf;  //512字节的dummy
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);

	SPI_I2S_DMACmd (SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_I2S_DMACmd (SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
	#endif
}
#if 0
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send ReadAddr high nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte to read from */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* while there is data to be read */
  {
    /* Read a byte from the FLASH */
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    /* Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}
#else
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	DMA_InitTypeDef  DMA_InitStructure;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send ReadAddr high nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte to read from */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
#if 0
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Channel2);
	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)SPI1_DR_Addr;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)pBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = NumByteToRead;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);

	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)DuumyClock;  //512字节的dummy
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	

	
	SPI_I2S_DMACmd (SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_I2S_DMACmd (SPI1, SPI_I2S_DMAReq_Rx, ENABLE);

	DMA_Cmd(DMA1_Channel3,ENABLE); 
	DMA_Cmd(DMA1_Channel2,ENABLE); 			 
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC3));
	
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC2));

	DMA_ClearFlag(DMA1_FLAG_TC2); 

	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA_Cmd(DMA1_Channel3, DISABLE);
#endif

	DMA1_Channel3->CCR &= ~( 1 << 0 ) ;         //关闭DMA通道3
	DMA1_Channel2->CCR &= ~( 1 << 0 ) ;         //关闭DMA通道2


	DMA1_Channel3->CMAR = (u32)pBuffer ; //设置DMA存储器地址，注意MSIZE
	DMA1_Channel2->CMAR = (u32)pBuffer ; //设置DMA存储器地址，注意MSIZE

	DMA1_Channel2->CNDTR = 0x0000   ;           //传输数量寄存器清零
	DMA1_Channel2->CNDTR = NumByteToRead ;         //传输数量设置为buffersize个

	DMA1_Channel3->CNDTR = 0x0000   ;           //传输数量寄存器清零
	DMA1_Channel3->CNDTR = NumByteToRead ;         //传输数量设置为buffersize个

	DMA1->IFCR = 0xF0 ;                         //清除通道3的标志位
	DMA1->IFCR = 0xF00 ;                        //清除通道3的标志位

	SPI1->DR ;									//接送前读一次SPI1->DR，保证接收缓冲区为空
	dummynum = 200000;
	while(((SPI1->SR & 0x02) == 0)&&((dummynum--)!=0));
	
	DMA1_Channel3->CCR |= 1 << 0 ;              //开启DMA通道3
	DMA1_Channel2->CCR |= 1 << 0 ;              //开启DMA通道2	
	
	dummynum = 200000;
	while(((DMA1->ISR & 0x20)== 0)&&((dummynum--)!=0));

	if(DMA1_Channel4->CNDTR != 0)
	{
		DMA_ERRO_FLAG = 1;
	}

	DMA1_Channel3->CCR &= ~( 1 << 0 ) ;         //关闭DMA通道3
	DMA1_Channel2->CCR &= ~( 1 << 0 ) ;         //关闭DMA通道2

	

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

#endif
/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_ReadID
  * @功能   读取SPI FLASH厂商ID和设备ID(设备ID包含类型和容量)
  *         Reads Manufacturer ID and two Device ID bytes
  * @参数   无
  * @返回值 24bit，高到底依次为厂商ID、类型和容量
***----------------------------------------------------------------*/
u32 SPI_FLASH_ReadID(void)
{
    u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

    /*拉低SPI_FLASH片选信号线*/
    SPI_FLASH_CS_LOW();

    /* 遵循读取ID 时序，发送命令*/
    SPI_FLASH_SendByte(W25X_JedecDeviceID);
    Temp0 = SPI_FLASH_SendByte(Dummy_Byte);
    Temp1 = SPI_FLASH_SendByte(Dummy_Byte);
    Temp2 = SPI_FLASH_SendByte(Dummy_Byte);

     /*拉高SPI_FLASH片选信号线*/
    SPI_FLASH_CS_HIGH();

    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
    return Temp;
}

/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_ReadDeviceID
  * @功能   读取SPI FLASH设备ID
  *         Read one Device ID bytes
  * @参数   无
  * @返回值 一个字节的Device ID
***----------------------------------------------------------------*/
u32 SPI_FLASH_ReadDeviceID(void)
{
    u32 Temp = 0;

    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();

    /* Send "RDID " instruction */
    SPI_FLASH_SendByte(W25X_DeviceID);
    SPI_FLASH_SendByte(Dummy_Byte);
    SPI_FLASH_SendByte(Dummy_Byte);
    SPI_FLASH_SendByte(Dummy_Byte);

    /* Read a byte from the FLASH */
    Temp = SPI_FLASH_SendByte(Dummy_Byte);

    /* Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    return Temp;
}

/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_StartReadSequence
  * @功能   发起一个读取SPI FLASH的访问，包括发送读命令和起始地址
  *         Initiates a read data byte (READ) sequence from the Flash.
  *         This is done by driving the /CS line low to select the device,
  *         then the READ instruction is transmitted followed by 3 bytes
  *         address. This function exit and keep the /CS line low, so the
  *         Flash still being selected. With this technique the whole
  *         content of the Flash is read with a single READ instruction.
  *         Read one Device ID bytes
  * @参数   ReadAddr FLASH的访问地址
  *                  FLASH's internal address to read from.
  * @返回值 无
***----------------------------------------------------------------*/
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();

    /* Send "Read from Memory " instruction */
    SPI_FLASH_SendByte(W25X_ReadData);

    /* Send the 24-bit address of the address to read from -----------------------*/
    /* Send ReadAddr high nibble address byte */
    SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
    /* Send ReadAddr medium nibble address byte */
    SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
    /* Send ReadAddr low nibble address byte */
    SPI_FLASH_SendByte(ReadAddr & 0xFF);
}


/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_ReadByte
  * @功能   读取SPI FLASH的一个字节，未包含发送读命令和起始地址
  * @参数   无
  * @返回值 从SPI_FLASH读取的一个字节
***----------------------------------------------------------------*/
u8 SPI_FLASH_ReadByte(void)
{
    return (SPI_FLASH_SendByte(Dummy_Byte));
}


/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_SendByte
  * @功能   通过SPI总线发送一个字节数据(顺便接收一个字节数据)
  *         Sends a byte through the SPI interface and return the byte
  *         received from the SPI bus.
  * @参数   要写入的一个字节数据
  * @返回值 在发数据时，MISO信号线上接收的一个字节
***----------------------------------------------------------------*/
u8 SPI_FLASH_SendByte(u8 byte)
{
	volatile uint32_t spi_cnt = 15000;
	
    /* Loop while DR register in not emplty */
    //while ((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)&&(spi_cnt != 0))
   while(((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET)&&(spi_cnt != 0))
	{
		spi_cnt--;
	}

    /* Send byte through the SPI1 peripheral */
    //SPI_I2S_SendData(SPI1, byte);
    SPI1->DR = byte;

    /* Wait to receive a byte */
	spi_cnt = 15000;
   // while ((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)&&(spi_cnt != 0))
   while(((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET)&&(spi_cnt != 0))
	{
		spi_cnt--;
    }

    /* Return the byte read from the SPI bus */
    //return SPI_I2S_ReceiveData(SPI1);
    return SPI1->DR;
}

/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_SendHalfWord
  * @功能   通过SPI总线发送一个半字(16bit=2个字节数据)(顺便接收数据)
  *         Sends a Half Word through the SPI interface and return the
  *         Half Word received from the SPI bus.
  * @参数   要写入的一个半字数据(16bit)
  * @返回值 在发数据时，MISO信号线上接收的一个半字数据(16bit)
***----------------------------------------------------------------*/
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
		{
			#if debug_flg == 1
						T_function(1050,1);
			#endif
		}

    /* Send Half Word through the SPI1 peripheral */
    SPI_I2S_SendData(SPI1, HalfWord);

    /* Wait to receive a Half Word */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
		{
			#if debug_flg == 1
				T_function(1100,1);
			#endif
  	}

    /* Return the Half Word read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}

/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_WriteEnable
  * @功能   SPI FLASH写使能
  *         Enables the write access to the FLASH.
  * @参数   无
  * @返回值 无
***----------------------------------------------------------------*/
void SPI_FLASH_WriteEnable(void)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Write Enable" instruction */
  SPI_FLASH_SendByte(W25X_WriteEnable);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/**-----------------------------------------------------------------
  * @函数名 SPI_FLASH_WaitForWriteEnd
  * @功能   通过反复读取SPI FLASH的状态寄存器判断写入是否执行结束
  *         Polls the status of the Write In Progress (WIP) flag in the
  *         FLASH's status  register  and  loop  until write  opertaion
  *         has completed.
  * @参数   无
  * @返回值 无
***----------------------------------------------------------------*/
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(W25X_ReadStatusReg);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);
		
		#if debug_flg == 1
		T_function(1150,1);
		#endif
  }
  while ((FLASH_Status & WIP_FlagMask) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/**-----------------------------------------------------------------
  * @函数名 SPI_Flash_PowerDown
  * @功能   SPI FLASH进入掉电模式
  * @参数   无
  * @返回值 无
***----------------------------------------------------------------*/
void SPI_Flash_PowerDown(void)   
{ 
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_PowerDown);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}   

/**-----------------------------------------------------------------
  * @函数名 SPI_Flash_WAKEUP
  * @功能   唤醒SPI FLASH
  * @参数   无
  * @返回值 无
***----------------------------------------------------------------*/
void SPI_Flash_WAKEUP(void)   
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_ReleasePowerDown);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}   

/**-------------------------------------------------------
  * @函数名 Buffercmp
  * @功能   比较两个缓冲区给定长度的内容是否相同的函数
  * @参数1  pBuffer1 和 pBuffer2 需要比较两个缓冲区起始地址
  * @参数2  BufferLength 给定的比较长度，字节为单位
  * @返回值 PASSED: 缓冲器给定长度的内容相同
  *         FAILED: 缓冲器给定长度的内容不相同
***------------------------------------------------------*/
TestStatus Buffercmp(u8* pBuffer1, u8* pBuffer2, u16 BufferLength)
{
    while(BufferLength--)
    {
        if(*pBuffer1 != *pBuffer2)
        {
            return FAILED;
        }

        pBuffer1++;
        pBuffer2++;
    }

    return PASSED;
}

#if 0
void SPI_DMA_Init_config()
{
	
}

/**************************************************************************************
函数名：Pic_Write();
输入参数：*Pname:图片名称
					P_size:图片大小
					*P_Wbuff：图片数据
返回值：0：写失败
				1：写成功
功能描述：实现图片写flash功能
**************************************************************************************/
uint8_t pic_array_temp[2];//4k临时缓存区间

uint8_t Pic_Write(uint8_t *P_name,uint32_t P_size,uint8_t *P_Wbuff)
{
	uint8_t pic_counter = 0;
	uint32_t Pic_SaveAddr;
	uint32_t Pic_SizeSaveAddr;
	uint32_t Pic_NameSaveAddr;
	uint8_t temp;
	uint8_t Pname_temp;
	uint32_t i,j,k;
	uint32_t name_len = 0;
	uint32_t SaveName_len = 0;
	union union32 size_tmp,size_tmp1;
	union union32 size1;
	uint8_t Pn[PIC_NAME_MAX_LEN];
	uint8_t cnt_temp;
	uint16_t n0;//临时计算值
	uint32_t Name_saveAddr = 0;
	uint8_t pic_position;
	
	SPI_FLASH_BufferRead(&pic_counter,PIC_COUNTER_ADDR,1);
	
	if(pic_counter == 0xff)
	{
		pic_counter = 0;		
	}
	/*********************************************************/
	Name_saveAddr = PIC_NAME_ADDR;
	for(i=0;i<pic_counter;i++)
	{
		pic_position = i;
		j=0;
		do
		{
			SPI_FLASH_BufferRead(&Pn[j],Name_saveAddr+j,1);
		}while(Pn[j++] !='\0');
		if(strcmp((char *)Pn,(char*)P_name)==0)
		{
			Pic_SaveAddr = PIC_DATA_ADDR + i * PER_PIC_MAX_SPACE;
			n0 = PER_PIC_MAX_SPACE/4096;//计算图片占用n个扇区。
			for(k=0;k<n0;k++)//擦除扇区
			{
				SPI_FLASH_SectorErase(Pic_SaveAddr+k*4096);
			}
			//更新图片数据
			SPI_FLASH_BufferWrite(P_Wbuff,Pic_SaveAddr,P_size);
			//读取比较，确保正确写入
			for(i=0;i<P_size;i++)
			{
				SPI_FLASH_BufferRead(&temp,Pic_SaveAddr+i,1);
				if(*(P_Wbuff+i) != temp)
				{
					return 0;
				}
			}
			//更新图片大小到flash中
			SPI_FLASH_BufferRead(pic_array_temp,PIC_SIZE_ADDR,4096);

			SPI_FLASH_SectorErase(PIC_SIZE_ADDR);

			//更新大小值
			size_tmp1.dwords = P_size;
			pic_array_temp[pic_position*4] = size_tmp1.bytes[0];
			pic_array_temp[pic_position*4+1] = size_tmp1.bytes[1];
			pic_array_temp[pic_position*4+2] = size_tmp1.bytes[2];
			pic_array_temp[pic_position*4+3] = size_tmp1.bytes[3];
			//写入flash
			SPI_FLASH_BufferWrite(pic_array_temp,PIC_SIZE_ADDR,4096);
			//读取比较，确保正确写入
			for(i=0;i<4096;i++)
			{
				SPI_FLASH_BufferRead(&temp,PIC_SIZE_ADDR+i,1);
				
				if(pic_array_temp[i] != temp)
				{
					return 0;
				}
			}		
			return 1;
		}
		else
		{
			Name_saveAddr += j;//图片名称间隔
		}
	}
	/*********************************************************/
	//计算图片的存储地址
	Pic_SaveAddr = PIC_DATA_ADDR + pic_counter * PER_PIC_MAX_SPACE;
	//读取比较，确保正确写入
	SPI_FLASH_BufferWrite(P_Wbuff,Pic_SaveAddr,P_size);
	for(i=0;i<P_size;i++)
	{
		SPI_FLASH_BufferRead(&temp,Pic_SaveAddr+i,1);
		if(*(P_Wbuff+i) != temp)
		{
			return 0;
		}
	}
	//计算已存图片信息占的空间
	for(j = 0;j < pic_counter;j ++)
	{
		do
		{
			SPI_FLASH_BufferRead(&Pname_temp,PIC_NAME_ADDR + SaveName_len,1);
			SaveName_len ++;
		}
		while(Pname_temp != '\0');	
	}
	
	//计算当前需存储图片信息名称字符串的长度
	i=0;
	while((*(P_name+i) != '\0'))
	{
		i++;
		name_len++;
	}
	
	//获取存储当前图片名称的存储地址
	Pic_NameSaveAddr = PIC_NAME_ADDR + SaveName_len;
	SPI_FLASH_BufferWrite(P_name,Pic_NameSaveAddr,name_len+1);
	//读取比较，确保正确写入
	SPI_FLASH_BufferRead(Pn,Pic_NameSaveAddr,name_len+1);
	if(strcmp((char*)Pn,(char*)P_name) != 0)
	{
		return 0;
	}
	//获取存储当前图片大小的存储地址
	Pic_SizeSaveAddr = PIC_SIZE_ADDR+4*pic_counter;
	size_tmp.dwords = P_size;
	SPI_FLASH_BufferWrite(size_tmp.bytes,Pic_SizeSaveAddr,4);
	//读取比较，确保正确写入
	SPI_FLASH_BufferRead(size1.bytes,Pic_SizeSaveAddr,4);
	if(size_tmp.dwords != size1.dwords)
	{
		return 0;
	}
	
	pic_counter++;
	SPI_FLASH_SectorErase(PIC_COUNTER_ADDR);
	SPI_FLASH_BufferWrite(&pic_counter,PIC_COUNTER_ADDR,1);	
	//读取比较，确保正确写入
	SPI_FLASH_BufferRead(&cnt_temp,PIC_COUNTER_ADDR,1);
	if(pic_counter != cnt_temp)
	{
		return 0;
	}	

	return 1;
}
#endif
/***************************************************************************
 函数名：PicMsg_Init()
输入参数：无
输出参数：无
功能描述：实现读取存储在flash中的图片信息，进行初始化赋值
****************************************************************************/
void PicMsg_Init(void)
{
	uint8_t i,j;
	uint32_t tmp_cnt = 0;
	uint8_t Pic_cnt1;
#if 0	
	SPI_FLASH_BufferRead(&Pic_cnt1,PIC_COUNTER_ADDR,1);
	if(Pic_cnt1 == 0xff)
	{
		Pic_cnt1 = 0;
	}

	for(i = 0;i < Pic_cnt1;i++)
	{
		//读取图片名称
		j = 0;
		do
		{
			SPI_FLASH_BufferRead(&PIC[i].name[j],PIC_NAME_ADDR + tmp_cnt,1);
			tmp_cnt++;
		}while(PIC[i].name[j++] != '\0');
		//读取图片大小值
		SPI_FLASH_BufferRead(PIC[i].size.bytes,PIC_SIZE_ADDR+i*4,4);
	}
#endif
}
/***************************************************************************
函数名：Pic_Read(uint8_t *Pname,uint8_t *P_Rbuff)
输入参数：
				uint8_t *Pname:图片名称（字符串）
				uint8_t *P_Rbuff：读取数据的暂存地址
输出参数：无
功能描述：
				实现图片数据读取功能。
****************************************************************************/
//static uint32_t sp_read_begin;
//static uint32_t sp_read_end;
//static uint32_t sp_last;
//static uint32_t sp_diff,sp_diff1;
#if 1
void Pic_Read(uint8_t *Pname,uint8_t *P_Rbuff)
{
	uint8_t i, j;
	uint8_t Pic_cnt;
	uint32_t tmp_cnt = 0;
	PIC_MSG PIC;
	uint8_t flash_read_repeat_cnt=0;
	
	
//	sp_read_begin = getTick();
//	sp_diff1 = getTickDiff(sp_last,sp_read_begin); 
//	sp_last = sp_read_begin;
	
	SPI_FLASH_BufferRead(&Pic_cnt,PIC_COUNTER_ADDR,1);
	if(Pic_cnt == 0xff)
	{
		Pic_cnt = 0;
	}
	
	for(i=0;i<Pic_cnt;i++)
	{
		//读取图片名称
		j = 0;
		do
		{
			SPI_FLASH_BufferRead(&PIC.name[j],PIC_NAME_ADDR + tmp_cnt,1);
			tmp_cnt++;
			j++;
		//}while((PIC.name[j++] != '\0'));
		}while((PIC.name[j-1] != '\0') && (PIC.name[j-1] != 0xff));
		//读取图片大小值
		SPI_FLASH_BufferRead(PIC.size.bytes,PIC_SIZE_ADDR+i*4,4);
		
		if((strcmp((char*)Pname,(char*)PIC.name))==0)
		{
		#if defined(STM32F10X_CL)
			if(i>=PIC_PART1_CNT)
				SPI_FLASH_BufferRead(P_Rbuff,PIC2_DATA_ADDR+(i-PIC_PART1_CNT)*PER_PIC_MAX_SPACE,PIC.size.dwords);
			else
		#endif
			SPI_FLASH_BufferRead(P_Rbuff,PIC_DATA_ADDR+i*PER_PIC_MAX_SPACE,PIC.size.dwords);	
			while(DMA_ERRO_FLAG == 1)
			{
				DMA_ERRO_FLAG = 0;
			#if defined(STM32F10X_CL)
				if(i >= PIC_PART1_CNT)
					SPI_FLASH_BufferRead(P_Rbuff,PIC2_DATA_ADDR+(i-PIC_PART1_CNT)*PER_PIC_MAX_SPACE,PIC.size.dwords);
				else
			#endif
					SPI_FLASH_BufferRead(P_Rbuff,PIC_DATA_ADDR+i*PER_PIC_MAX_SPACE,PIC.size.dwords);
				
				flash_read_repeat_cnt++;
				if(flash_read_repeat_cnt >= 3)
				{
					break;
				}
			}
			break;
		}
	}

//	sp_read_end = getTick();

//	sp_diff = getTickDiff(sp_read_begin,sp_read_end );
	
}

#endif
#if 0
uint32_t Get_Font_Flash_addr(uint8_t *Pname,uint8_t *P_Rbuff)
{
	uint8_t i;
	uint8_t Font_cnt2;
	PIC_MSG PIC1;
	
	SPI_FLASH_BufferRead(&Font_cnt2,SINGLE_BYTE_CNT_ADDR,1);
	if(Font_cnt2 == 0xff)
	{
		Font_cnt2 = 0;
	}
	
	for(i=0;i<Font_cnt2;i++)
	{
		if((strcmp((char*)Pname,(char*)PIC1[i].name))==0)
		{
			
			return (SINGLE_BYTE_LANGUAGE_ADDR+PER_PIC_MAX_SPACE*i);
		}
	}
}
#endif
//2015年04月23日
/***************************************************************************
函数名：Pic_Logo_Write(uint8_t *LogoName,uint32_t LogoSize,uint8_t *Logo_Wbuff)
输入参数：
				uint8_t *LogoName:logo图片名称（字符串）				
				uint8_t *LogoWbuff:logo图片数据
				uint32_t WriteSize:logo图片大小，最大值为152k。
输出参数：
				1：写入成功
				0：写入失败
功能描述：
				实现logo图片数据写入flash功能。起始地址为0x000000，
****************************************************************************/
uint32_t LogoWrite_Addroffset = 0;

uint8_t Pic_Logo_Write(uint8_t *LogoName,uint8_t *Logo_Wbuff,uint32_t LogoWriteSize)
{
	uint16_t n;
	uint32_t i;
	uint8_t temp1;
	
	if(LogoWrite_Addroffset == 0)//只在第一次擦除logo空间。
	{
		n = 38;//logo一共最大占扇区数目为152/4=38;
		for(i=0;i<n;i++)
		{
			SPI_FLASH_SectorErase(PIC_LOGO_ADDR+i*4096);
		}
	}
	SPI_FLASH_BufferWrite(Logo_Wbuff,PIC_LOGO_ADDR+LogoWrite_Addroffset,LogoWriteSize);
	for(i=0;i<LogoWriteSize;i++)
	{
		SPI_FLASH_BufferRead(&temp1,PIC_LOGO_ADDR+LogoWrite_Addroffset+i,1);
		if(*(Logo_Wbuff+i) != temp1)
		{
			return 0;
		}
	}		
	LogoWrite_Addroffset += LogoWriteSize;
	if(LogoWrite_Addroffset >= LOGO_MAX_SIZE)
	{	
		LogoWrite_Addroffset = 0;
	}
	return 1;
}
//2015年04月18日
/***************************************************************************
函数名：Pic_Logo_Read(uint8_t *Pname,uint8_t *P_Rbuff,uint32_t Readsize)
输入参数：
				uint8_t *Pname:图片名称（字符串）
				uint8_t *P_Rbuff：读取数据的暂存地址
				uint32_t Readsize:读取数据的大小
输出参数：无
功能描述：
				实现logo图片数据读取功能。
****************************************************************************/
uint32_t logo_addroffset = 0;
void Pic_Logo_Read(uint8_t *LogoName,uint8_t *Logo_Rbuff,uint32_t LogoReadsize)
{
	SPI_FLASH_BufferRead(Logo_Rbuff,PIC_LOGO_ADDR+logo_addroffset,LogoReadsize);
	logo_addroffset += LogoReadsize;
	if(logo_addroffset >= LOGO_MAX_SIZE)
	{
		logo_addroffset = 0;
	}
}

#if 1
/***************************************************************************
函数名：Icon_Logo_Write(uint8_t *iconLogoName,uint32_t iconLogoSize,uint8_t *iconLogo_Wbuff)
输入参数：
				uint8_t *iconLogoName:小logo图片名称（字符串）				
				uint8_t *iconLogoWbuff:小logo图片数据
				uint32_t iconWriteSize:小logo图片大小，最大值为152k。
输出参数：
				1：写入成功
				0：写入失败
功能描述：
				实现logo图片数据写入flash功能。起始地址为0x000000，
****************************************************************************/
uint32_t iconLogoWrite_Addroffset = 0;
uint8_t Icon_Logo_Write(uint8_t *iconLogoName,uint8_t *iconLogo_Wbuff,uint32_t iconLogoWriteSize)
{
	uint32_t i;
	uint8_t temp1;
	
	if(iconLogoWriteSize <= 0)
		return 0;
	
	SPI_FLASH_BufferWrite(iconLogo_Wbuff,PIC_ICON_LOGO_ADDR+iconLogoWrite_Addroffset,iconLogoWriteSize);
	
	iconLogoWrite_Addroffset += iconLogoWriteSize;
	
	if(iconLogoWrite_Addroffset >= ICONLOGO_MAX_SIZE)
	{	
		iconLogoWrite_Addroffset = 0;
	}
	return 1;	
}

/***************************************************************************
函数名：Icon_Logo_Read(uint8_t *iconPname,uint8_t *iconP_Rbuff,uint32_t iconReadsize)
输入参数：
				uint8_t *iconPname:图片名称（字符串）
				uint8_t *iconP_Rbuff：读取数据的暂存地址
				uint32_t iconReadsize:读取数据的大小
输出参数：无
功能描述：
				实现logo图片数据读取功能。
****************************************************************************/
uint32_t iconlogo_addroffset = 0;
void Icon_Logo_Read(uint8_t *iconLogoName,uint8_t *iconLogo_Rbuff,uint32_t iconLogoReadsize)
{
	SPI_FLASH_BufferRead(iconLogo_Rbuff,PIC_ICON_LOGO_ADDR+iconlogo_addroffset,iconLogoReadsize);
	iconlogo_addroffset += iconLogoReadsize;
	/*
	if(iconlogo_addroffset >= ICONLOGO_MAX_SIZE)
	{
		iconlogo_addroffset = 0;
	}
	*/
}
#endif

#endif
