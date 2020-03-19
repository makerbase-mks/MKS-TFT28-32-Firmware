#ifndef __MYTEXT_H__
#define __MYTEXT_H__	 
#include "sys.h"
//#include "fontupd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//汉字显示 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/18
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

#define FONTINFOADDR	0x1B5000//2*1024
#define GBK_FLASH_ADDR	FONTINFOADDR+0X32//0X2D000//UNIGBK_FLASH_ADDR+0X2D000
#define UNIGBK_FLASH_ADDR		GBK_FLASH_ADDR+280*1024//FONTINFOADDR+0X32


//字库信息结构体定义
//用来保存字库基本信息，地址，大小等
__packed typedef struct 
{
	u8 fontok;				//字库存在标志，0XAA，字库正常；其他，字库不存在
	u32 ugbkaddr; 			//unigbk的地址
	u32 ugbksize;			//unigbk的大小	 
	u32 f12addr;			//gbk12地址	
	u32 gbk12size;			//gbk12的大小	 
	u32 f16addr;			//gbk16地址
	u32 gkb16size;			//gbk16的大小	 
}_font_info;

extern _font_info ftinfo;	//字库信息结构体
					     
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);//得到汉字的点阵码
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode);//在指定位置显示一个汉字
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);//在指定位置显示一个字符串 
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);
void Show_Str2(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);
#endif
