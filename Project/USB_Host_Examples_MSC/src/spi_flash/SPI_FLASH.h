#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H

#include "stm32f10x.h"

 typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/**
  * @brief  SPI_FLASH SPI Interface pins
  */

//v3.0.0之后SPI FLASH为8M

#define  W25X16_FLASH_ID            0xEF3015
#define  W25Q16_FLASH_ID            0xEF4015

#if defined(STM32F10X_HD)
	#define PIC_NAME_MAX_LEN		50	//图片名字最大长度

	#define LOGO_MAX_SIZE			(150*1024) //logo最大值
	#define ICONLOGO_MAX_SIZE	 (85*1024) //小logo最大值

	#define PIC_NAME_ADDR				0x003000	//图片信息存储地址，图片名称
	#define PIC_SIZE_ADDR				0x007000	//图片信息存储地址，图片大小值
	#define PIC_COUNTER_ADDR			0x008000	//图片总数量值存储地址
	#define PIC_LOGO_ADDR 			0x009000	//图片logo存储地址
	#define PIC_DATA_ADDR 			0x02F000	//图片数据存储地址
	//tan 2017.01.05
	#define PIC_ICON_LOGO_ADDR		0x5D8000 //主界面小logo存储地址，图片大小约85Kb(320*135像素)，为方便处理取大小为88Kb。
	//
	//字库
	#define PIC_OTHER_SIZE_ADDR  0X5ee000 //预留其他尺寸存储空间约80K

	#define FONTINFOADDR								0X600000//
	#define FONTINFOSIZE_CN 						FONTINFOADDR //
	#define FONTINFOSIZE_KR 						(FONTINFOSIZE_CN+4) 	//
	#define UNIGBK_FLASH_ADDR 					(FONTINFOADDR+4*1024)//
	#define GBK_FLASH_ADDR							(UNIGBK_FLASH_ADDR+176*1024)//
	#define KR2UNIC_FLASH_ADDR					(GBK_FLASH_ADDR+748*1024)//
	#define KOREAN_FLASH_ADDR 					(KR2UNIC_FLASH_ADDR+176*1024) 
			
	#define FATFS_DATA_ADDR 						(KOREAN_FLASH_ADDR+748*1024)		//
	#define SINGLE_BYTE2UNIC_ADDR 		(FATFS_DATA_ADDR+4*1024)				//
	#define SINGLE_BYTE_NAME_ADDR 		(SINGLE_BYTE2UNIC_ADDR+8*1024)	//
	#define SINGLE_BYTE_SIZE_ADDR 		(SINGLE_BYTE_NAME_ADDR+20*1024) //
	#define SINGLE_BYTE_CNT_ADDR			(SINGLE_BYTE_SIZE_ADDR+4*1024)	//
	#define SINGLE_BYTE_LANGUAGE_ADDR (SINGLE_BYTE_CNT_ADDR+4*1024) 	//


	
#endif

#if defined(STM32F10X_CL)
	
		#define PIC_NAME_MAX_LEN		50	//图片名字最大长度

		#define LOGO_MAX_SIZE 		(152*1024) //logo最大值
		#define ICONLOGO_MAX_SIZE  (85*1024) //小logo最大值
		
		#define PIC_LOGO_ADDR					0x000000	//图片logo存储地址
		#define PIC_DATA_ADDR					0x026000	//图片数据存储地址
		#define PIC_NAME_ADDR					0x1f0000	//图片信息存储地址，图片名称
		#define PIC_SIZE_ADDR					0x1f7000	//图片信息存储地址，图片大小值
		#define PIC_COUNTER_ADDR			0x1f8000	//图片总数量值存储地址
		#define PIC_1_END							(PIC_COUNTER_ADDR+4*1024)
		
		#define FONTINFOADDR							0X200000  //兼容以前旧版本，从2M地址开始存储双字节字库内容
		#define FONTINFOSIZE_CN						FONTINFOADDR //GBK的Unicode转换表的大小值
		#define FONTINFOSIZE_KR						(FONTINFOSIZE_CN+4)		//韩文的Unicode转换表的大小值
		#define UNIGBK_FLASH_ADDR					(FONTINFOADDR+4*1024)	//GBK的转换表存储地址
		#define GBK_FLASH_ADDR 						(UNIGBK_FLASH_ADDR+176*1024)//GBK字库存储地址	
		
		#define FATFS_DATA_ADDR						(GBK_FLASH_ADDR+748*1024)	//修改了fatfs文件系统相关的数组，存储在flash中；
		#define FONTINFOADDR_EXCVT_437	(FATFS_DATA_ADDR)
		#define FONTINFOADDR_EXCVT_720	(FONTINFOADDR_EXCVT_437+128)
		#define FONTINFOADDR_EXCVT_737	(FONTINFOADDR_EXCVT_720+128)
		#define FONTINFOADDR_EXCVT_775	(FONTINFOADDR_EXCVT_737+128)
		#define FONTINFOADDR_EXCVT_850	(FONTINFOADDR_EXCVT_775+128)
		#define FONTINFOADDR_EXCVT_852	(FONTINFOADDR_EXCVT_850+128)
		#define FONTINFOADDR_EXCVT_855	(FONTINFOADDR_EXCVT_852+128)
		#define FONTINFOADDR_EXCVT_857	(FONTINFOADDR_EXCVT_855+128)
		#define FONTINFOADDR_EXCVT_858	(FONTINFOADDR_EXCVT_857+128)
		#define FONTINFOADDR_EXCVT_862	(FONTINFOADDR_EXCVT_858+128)
		#define FONTINFOADDR_EXCVT_866	(FONTINFOADDR_EXCVT_862+128)
		#define FONTINFOADDR_EXCVT_874	(FONTINFOADDR_EXCVT_866+128)
		#define FONTINFOADDR_EXCVT_1250	(FONTINFOADDR_EXCVT_874+128)	
		#define FONTINFOADDR_EXCVT_1251	(FONTINFOADDR_EXCVT_1250+128)
		#define FONTINFOADDR_EXCVT_1252	(FONTINFOADDR_EXCVT_1251+128)
		#define FONTINFOADDR_EXCVT_1253	(FONTINFOADDR_EXCVT_1252+128)
		#define FONTINFOADDR_EXCVT_1254	(FONTINFOADDR_EXCVT_1253+128)
		#define FONTINFOADDR_EXCVT_1255	(FONTINFOADDR_EXCVT_1254+128)
		#define FONTINFOADDR_EXCVT_1256	(FONTINFOADDR_EXCVT_1255+128)
		#define FONTINFOADDR_EXCVT_1257	(FONTINFOADDR_EXCVT_1256+128)
		#define FONTINFOADDR_EXCVT_1258	(FONTINFOADDR_EXCVT_1257+128)
		
		#define SINGLE_BYTE2UNIC_ADDR			(FATFS_DATA_ADDR+4*1024)   //单字节的Unicode转换表
		#define FONTINFOADDR_TABLE_437		SINGLE_BYTE2UNIC_ADDR
		#define FONTINFOADDR_TABLE_720		(FONTINFOADDR_TABLE_437+256)
		#define FONTINFOADDR_TABLE_737		(FONTINFOADDR_TABLE_720+256)
		#define FONTINFOADDR_TABLE_775		(FONTINFOADDR_TABLE_737+256)
		#define FONTINFOADDR_TABLE_850		(FONTINFOADDR_TABLE_775+256)
		#define FONTINFOADDR_TABLE_852		(FONTINFOADDR_TABLE_850+256)
		#define FONTINFOADDR_TABLE_855		(FONTINFOADDR_TABLE_852+256)
		#define FONTINFOADDR_TABLE_857		(FONTINFOADDR_TABLE_855+256)
		#define FONTINFOADDR_TABLE_858		(FONTINFOADDR_TABLE_857+256)
		#define FONTINFOADDR_TABLE_862		(FONTINFOADDR_TABLE_858+256)
		#define FONTINFOADDR_TABLE_866		(FONTINFOADDR_TABLE_862+256)
		#define FONTINFOADDR_TABLE_874		(FONTINFOADDR_TABLE_866+256)
		#define FONTINFOADDR_TABLE_1250 	(FONTINFOADDR_TABLE_874+256)	
		#define FONTINFOADDR_TABLE_1251 	(FONTINFOADDR_TABLE_1250+256)
		#define FONTINFOADDR_TABLE_1252 	(FONTINFOADDR_TABLE_1251+256)
		#define FONTINFOADDR_TABLE_1253 	(FONTINFOADDR_TABLE_1252+256)
		#define FONTINFOADDR_TABLE_1254 	(FONTINFOADDR_TABLE_1253+256)
		#define FONTINFOADDR_TABLE_1255 	(FONTINFOADDR_TABLE_1254+256)
		#define FONTINFOADDR_TABLE_1256 	(FONTINFOADDR_TABLE_1255+256)
		#define FONTINFOADDR_TABLE_1257 	(FONTINFOADDR_TABLE_1256+256)
		#define FONTINFOADDR_TABLE_1258 	(FONTINFOADDR_TABLE_1257+256)

		#define SINGLE_BYTE_NAME_ADDR			(SINGLE_BYTE2UNIC_ADDR+8*1024)//单字节语言名称
		#define SINGLE_BYTE_SIZE_ADDR			(SINGLE_BYTE_NAME_ADDR+20*1024)//单字节语言大小
		#define SINGLE_BYTE_CNT_ADDR			(SINGLE_BYTE_SIZE_ADDR+4*1024)//单字节语言数量
		#define SINGLE_BYTE_1_END					 (SINGLE_BYTE_CNT_ADDR+4*1024)
		
		#define PIC_ICON_LOGO_ADDR				0x300000 		//兼容以前旧版本，主界面小logo存储地址，图片大小约85Kb(320*135像素)，为方便处理取大小为88Kb。
		
		#define KR2UNIC_FLASH_ADDR				(PIC_ICON_LOGO_ADDR+88*1024)
		#define KOREAN_FLASH_ADDR					(KR2UNIC_FLASH_ADDR+176*1024)	
		#define SINGLE_BYTE_LANGUAGE_ADDR	(KOREAN_FLASH_ADDR+748*1024)   //单字节字库
		
		#define PIC2_DATA_ADDR			0x500000//第二部分图片存储地址
		
		#define FLAHS_ALL_SPACE			0X800000//正片flash的大小
		
		//用来保存字库基本信息，地址，大小等
		#define PIC_PART1_CNT			114//第一部分最多存储的图片数
		
		#define PIC1_DATA					PIC_LOGO_ADDR
		#define PIC2_DATA					PIC2_DATA_ADDR
		#define FONT1_DATA				FONTINFOADDR
		#define FONT2_DATA				KR2UNIC_FLASH_ADDR
		#define LITTLE_LOGO_DATA	PIC_ICON_LOGO_ADDR
		
		#define PIC1_SIZE					((PIC_1_END-PIC_LOGO_ADDR)/1024)//2020K
		#define ICON_LOGO_SIZE		((KR2UNIC_FLASH_ADDR-PIC_ICON_LOGO_ADDR)/1024)//88K
		#define FONT1_SIZE				((SINGLE_BYTE_1_END-FONTINFOADDR)/1024)//936K
		#define FONT2_SIZE				((PIC2_DATA_ADDR-KR2UNIC_FLASH_ADDR)/1024)//1960K
		#define PIC2_SIZE					((FLAHS_ALL_SPACE-PIC2_DATA_ADDR)/1024)//3072K
	#if 0
		#define PIC_NAME_MAX_LEN		50	//图片名字最大长度

		#define LOGO_MAX_SIZE				(150*1024) //logo最大值
		#define ICONLOGO_MAX_SIZE	 	(85*1024) //小logo最大值

		#define PIC_LOGO_ADDR				0x000000	//图片logo存储地址
		#define PIC_DATA_ADDR				0x026000	//图片数据存储地址
		#define PIC_NAME_ADDR				0x1f0000	//图片信息存储地址，图片名称
		#define PIC_SIZE_ADDR				0x1f7000	//图片信息存储地址，图片大小值
		#define PIC_COUNTER_ADDR			0x1f8000	//图片总数量值存储地址
		//tan 2017.01.05
		#define PIC_ICON_LOGO_ADDR		0x300000 //主界面小logo存储地址，图片大小约85Kb(320*135像素)，为方便处理取大小为88Kb。
		//
		//字库
		#define FONTINFOADDR	0x200000//2M以后地址为字库
		#define UNIGBK_FLASH_ADDR		(FONTINFOADDR+4096)//4*1024
		#define GBK_FLASH_ADDR	(UNIGBK_FLASH_ADDR+180224)//176*1024
	#endif
#endif

extern void dma_init();

/*----- High layer function -----*/
extern void SPI_FLASH_Init(void);
extern void SPI_FLASH_SectorErase(u32 SectorAddr);
extern void SPI_FLASH_ChipErase(void);
extern void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
extern void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
extern void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
extern u32 SPI_FLASH_ReadID(void);
extern u32 SPI_FLASH_ReadDeviceID(void);
extern void SPI_FLASH_StartReadSequence(u32 ReadAddr);
extern void SPI_Flash_PowerDown(void);
extern void SPI_Flash_WAKEUP(void);

/*----- Low layer function -----*/
extern u8 SPI_FLASH_ReadByte(void);
extern u8 SPI_FLASH_SendByte(u8 byte);
extern u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
extern void SPI_FLASH_WriteEnable(void);
extern void SPI_FLASH_WaitForWriteEnd(void);
extern TestStatus Buffercmp(u8* pBuffer1, u8* pBuffer2, u16 BufferLength);

extern uint8_t Pic_Write(uint8_t *P_name,uint32_t P_size,uint8_t *P_Wbuff);
extern void PicMsg_Init(void);
extern void Pic_Read(uint8_t *Pname,uint8_t *P_Rbuff);
extern uint8_t Pic_Logo_Write(uint8_t *LogoName,uint8_t *Logo_Wbuff,uint32_t LogoWriteSize);
extern void Pic_Logo_Read(uint8_t *LogoName,uint8_t *Logo_Rbuff,uint32_t LogoReadsize);
extern uint8_t Icon_Logo_Write(uint8_t *iconLogoName,uint8_t *iconLogo_Wbuff,uint32_t iconLogoWriteSize);
extern void Icon_Logo_Read(uint8_t *iconLogoName,uint8_t *iconLogo_Rbuff,uint32_t iconLogoReadsize);

#endif
