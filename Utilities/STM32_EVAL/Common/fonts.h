/******************** (C) COPYRIGHT 2013 www.armjishu.com  ********************
 * 文件名  ：fonts.c
 * 描述    ：ASCII字模的字库数据表
 * 实验平台：STM32神舟开发板
 * 作者    ：www.armjishu.com 
*******************************************************************************/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FONTS_H
#define __FONTS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/**
// exact-width signed integer types 
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef   signed       __int64 int64_t;

// exact-width unsigned integer types 
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned       __int64 uint64_t;
*/

typedef struct _tFont
{    
  uint8_t Width;
  uint8_t Height;
  uint8_t BytesPreChar;
  const uint8_t *table;  
} sFONT;

extern sFONT Font24x48;
extern sFONT Font16x24;
extern sFONT Font12x24;
extern sFONT Font8x16;
extern sFONT Font8x12;
extern sFONT Font8x8;

extern sFONT * pFontTable[];

/** @defgroup FONTS_Exported_Constants
  * @{
  */ 
#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height))


#ifdef __cplusplus
}
#endif
  
#endif /* __FONTS_H */
 

/**************** (C) COPYRIGHT 2010 2013 www.armjishu.com *****END OF FILE****/
