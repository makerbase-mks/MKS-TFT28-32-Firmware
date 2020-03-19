/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SZ_STM32F107VC_LCD_H
#define __SZ_STM32F107VC_LCD_H

#ifdef __cplusplus
 extern "C" {
#endif

/**-------------------------------------------------------
  * LCD??¨¨?2¨¦¨®?o¨¢?¨¢??¨º?¡ê????D¡Á?¡À¨º¡À¨ª¨º??a(x,y)
  * x LCD¦Ì?DD¨ºy  
  * y LCD¦Ì?¨¢D¨ºy
  (0,0)     ¡À¨ª¨º?LCD¡Á¨®¨¦???¦Ì?¦Ì? 
  (239,319) ¡À¨ª¨º?LCD¨®¨°????¦Ì?¦Ì?   
  (0,2)     ¡À¨ª¨º?LCD¦Ì¨²1DD¦Ì¨²3¨¢D¦Ì?¦Ì?DD
  (238,2)   ¡À¨ª¨º?LCD¦Ì¨²239DD¦Ì¨²3¨¢D¦Ì?¦Ì?DD
          yyyy                               Y
          0123                               319             
  X = 0   |--(0,2)-------- ~~~ --------------|
  X = 1   |--------------- ~~~ --------------|
  X = 2   |--------------- ~~~ --------------|
  .....
  X = 237 |--------------- ~~~ --------------|
  X = 238 |--(238,2)------ ~~~ --------------|  
  X = 239 |--------------- ~~~ --------------|(239,319)
  
***------------------------------------------------------*/
#include "stm32f10x.h"
#include "fonts.h"

//LCDµÄ»­±ÊÑÕÉ«ºÍ±³¾°É«	   
extern u16 POINT_COLOR;	//»­±ÊÑÕÉ«
extern u16 BACK_COLOR;  //±³¾°É« 

/* ¨¦???IVo?LCD1¨¹???¨¤1??¡§¨°?  www.armjishu.com Team  */ 
#define LCD_PinNum_WR               14
#define LCD_Pin_WR                  GPIO_Pin_14
#define LCD_PORT_WR                 GPIOB
#define LCD_CLK_WR                  RCC_APB2Periph_GPIOB

#define LCD_PinNum_CS               8
#define LCD_Pin_CS                  GPIO_Pin_8
#define LCD_PORT_CS                 GPIOC
#define LCD_CLK_CS                  RCC_APB2Periph_GPIOC

#define LCD_PinNum_RS               13
#define LCD_Pin_RS                  GPIO_Pin_13
#define LCD_PORT_RS                 GPIOD
#define LCD_CLK_RS                  RCC_APB2Periph_GPIOD

#define LCD_PinNum_RD               15
#define LCD_Pin_RD                  GPIO_Pin_15
#define LCD_PORT_RD                 GPIOD
#define LCD_CLK_RD                  RCC_APB2Periph_GPIOD


#define LCD_PIN_DATA                GPIO_Pin_All
#define LCD_PORT_DAT                GPIOE
#define LCD_CLK_DATA                RCC_APB2Periph_GPIOE


/* ¨¦???IVo??a¡¤¡é¡ã?¨®¨¦¨®¨²1¨¹??¡Á¨º?¡ä¨®D?T¡ê??¨´¨°?¨°o?¡ì?¨¢¦Ì?¡À31a3¡ê¨¢¨¢ */
#define LCD_LIGHT_ON   
#define LCD_LIGHT_OFF  

/* ¨¦???IVo?LCD?¨¤1??¡§¨°?  ?¨´¡À?'¦Ì?'¦Ì??¨¢11¨¬?  www.armjishu.com Team  */ 
typedef struct 
{
    int16_t X;      // DD
    int16_t Y;      // ¨¢D (DD?¨²¦Ì?¦Ì???¨°?)
} Point, * pPoint;   


/* ¨¦???IVo?LCD??¡ä??¡Â?¨¤1??¡§¨°?  LCD Registers  www.armjishu.com Team  */ 
#define LCD_REG_0             0x00
#define LCD_REG_1             0x01
#define LCD_REG_2             0x02
#define LCD_REG_3             0x03
#define LCD_REG_4             0x04
#define LCD_REG_5             0x05
#define LCD_REG_6             0x06
#define LCD_REG_7             0x07
#define LCD_REG_8             0x08
#define LCD_REG_9             0x09
#define LCD_REG_10            0x0A
#define LCD_REG_12            0x0C
#define LCD_REG_13            0x0D
#define LCD_REG_14            0x0E
#define LCD_REG_15            0x0F
#define LCD_REG_16            0x10
#define LCD_REG_17            0x11
#define LCD_REG_18            0x12
#define LCD_REG_19            0x13
#define LCD_REG_20            0x14
#define LCD_REG_21            0x15
#define LCD_REG_22            0x16
#define LCD_REG_23            0x17
#define LCD_REG_24            0x18
#define LCD_REG_25            0x19
#define LCD_REG_26            0x1A
#define LCD_REG_27            0x1B
#define LCD_REG_28            0x1C
#define LCD_REG_29            0x1D
#define LCD_REG_30            0x1E
#define LCD_REG_31            0x1F
#define LCD_REG_32            0x20
#define LCD_REG_33            0x21
#define LCD_REG_34            0x22
#define LCD_REG_36            0x24
#define LCD_REG_37            0x25
#define LCD_REG_40            0x28
#define LCD_REG_41            0x29
#define LCD_REG_43            0x2B
#define LCD_REG_45            0x2D
#define LCD_REG_48            0x30
#define LCD_REG_49            0x31
#define LCD_REG_50            0x32
#define LCD_REG_51            0x33
#define LCD_REG_52            0x34
#define LCD_REG_53            0x35
#define LCD_REG_54            0x36
#define LCD_REG_55            0x37
#define LCD_REG_56            0x38
#define LCD_REG_57            0x39
#define LCD_REG_59            0x3B
#define LCD_REG_60            0x3C
#define LCD_REG_61            0x3D
#define LCD_REG_62            0x3E
#define LCD_REG_63            0x3F
#define LCD_REG_64            0x40
#define LCD_REG_65            0x41
#define LCD_REG_66            0x42
#define LCD_REG_67            0x43
#define LCD_REG_68            0x44
#define LCD_REG_69            0x45
#define LCD_REG_70            0x46
#define LCD_REG_71            0x47
#define LCD_REG_72            0x48
#define LCD_REG_73            0x49
#define LCD_REG_74            0x4A
#define LCD_REG_75            0x4B
#define LCD_REG_76            0x4C
#define LCD_REG_77            0x4D
#define LCD_REG_78            0x4E
#define LCD_REG_79            0x4F
#define LCD_REG_80            0x50
#define LCD_REG_81            0x51
#define LCD_REG_82            0x52
#define LCD_REG_83            0x53
#define LCD_REG_96            0x60
#define LCD_REG_97            0x61
#define LCD_REG_106           0x6A
#define LCD_REG_118           0x76
#define LCD_REG_128           0x80
#define LCD_REG_129           0x81
#define LCD_REG_130           0x82
#define LCD_REG_131           0x83
#define LCD_REG_132           0x84
#define LCD_REG_133           0x85
#define LCD_REG_134           0x86
#define LCD_REG_135           0x87
#define LCD_REG_136           0x88
#define LCD_REG_137           0x89
#define LCD_REG_139           0x8B
#define LCD_REG_140           0x8C
#define LCD_REG_141           0x8D
#define LCD_REG_143           0x8F
#define LCD_REG_144           0x90
#define LCD_REG_145           0x91
#define LCD_REG_146           0x92
#define LCD_REG_147           0x93
#define LCD_REG_148           0x94
#define LCD_REG_149           0x95
#define LCD_REG_150           0x96
#define LCD_REG_151           0x97
#define LCD_REG_152           0x98
#define LCD_REG_153           0x99
#define LCD_REG_154           0x9A
#define LCD_REG_157           0x9D
#define LCD_REG_192           0xC0
#define LCD_REG_193           0xC1
#define LCD_REG_229           0xE5

/* ¨¦???IVo?LCD2¨º?¨¢3¡ê¨®??¨´¡À???¨¦??¡§¨°?  LCD color   www.armjishu.com Team  */ 
#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xE79C
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0

/* ¨¦???IVo?LCD2¨º?¨¢??¨º?¡Á?¡¤?¦Ì?DD¨ºy¡ê?¨®?¡Á?¨¬?¨®D1?  www.armjishu.com Team  */ 
#define LCD_LINE_0               LINE(0)
#define LCD_LINE_1               LINE(1)
#define LCD_LINE_2               LINE(2)
#define LCD_LINE_3               LINE(3)
#define LCD_LINE_4               LINE(4)
#define LCD_LINE_5               LINE(5)
#define LCD_LINE_6               LINE(6)
#define LCD_LINE_7               LINE(7)
#define LCD_LINE_8               LINE(8)
#define LCD_LINE_9               LINE(9)
#define LCD_LINE_10              LINE(10)
#define LCD_LINE_11              LINE(11)
#define LCD_LINE_12              LINE(12)
#define LCD_LINE_13              LINE(13)
#define LCD_LINE_14              LINE(14)
#define LCD_LINE_15              LINE(15)
#define LCD_LINE_16              LINE(16)
#define LCD_LINE_17              LINE(17)
#define LCD_LINE_18              LINE(18)
#define LCD_LINE_19              LINE(19)
#define LCD_LINE_20              LINE(20)
#define LCD_LINE_21              LINE(21)
#define LCD_LINE_22              LINE(22)
#define LCD_LINE_23              LINE(23)
#define LCD_LINE_24              LINE(24)
#define LCD_LINE_25              LINE(25)
#define LCD_LINE_26              LINE(26)
#define LCD_LINE_27              LINE(27)
#define LCD_LINE_28              LINE(28)
#define LCD_LINE_29              LINE(29)

/* ¨¦???IVo?LCD2¨º?¨¢?-??¦Ì¨¨o¡¥¨ºy¦Ì?¡¤??¨°Direction?¡§¨°?   www.armjishu.com Team  */ 
#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001

/* ¨¦???IVo?LCD2¨º?¨¢¡¤?¡À??¨º?¡§¨°?  ?¨ªo¨ª??   www.armjishu.com Team  */ 
#define LCD_PIXEL_WIDTH          0x0140  /* ?¨ª320 */
#define LCD_PIXEL_HEIGHT         0x00F0  /* ??240 */

/* ??¨º?¡Á?¡¤?¨º¡À¨ª??¡Â¡À3?¡ã?¡ê¨º??a1? */
#define HyalineBackEn           0x01  //  ¨ª??¡Â¡À3?¡ã
#define HyalineBackDis          0x00  //  2?¨ª??¡Â¡À3?¡ã(??¨¨?)

/** 
  * @brief LCD default font 
  */ 
//#define LCD_DEFAULT_FONT         Font24x48
//#define LCD_DEFAULT_FONT         Font16x24
//#define LCD_DEFAULT_FONT         Font12x24
//#define LCD_DEFAULT_FONT         Font12x12
//#define LCD_DEFAULT_FONT         Font8x16
//#define LCD_DEFAULT_FONT         Font8x12
//#define LCD_DEFAULT_FONT         Font8x8


/** @defgroup STM3210C_EVAL_LCD_Exported_Macros
  * @{
  */
#define ASSEMBLE_RGB(R, G, B)    ((((R)& 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3))   

/**  Image2Lcd¡À¡ê¡ä?¦Ì?¨ª???¨ºy?Y typedef struct _HEADGRAY
scan: ¨¦¡§?¨¨?¡ê¨º? 
Bit7: 0:¡Á?¡Á¨®?¨¢¨®¨°¨¦¡§?¨¨¡ê?1:¡Á?¨®¨°?¨¢¡Á¨®¨¦¡§?¨¨?¡ê 
Bit6: 0:¡Á??£¤?¨¢¦Ì¡Á¨¦¡§?¨¨¡ê?1:¡Á?¦Ì¡Á?¨¢?£¤¨¦¡§?¨¨?¡ê 
Bit5: 0:¡Á??¨²?¨²?¨®??¨ºy?Y¡ä¨®????¦Ì?¦Ì¨ª????¨¢D¡ê?1:¡Á??¨²?¨²?¨®??¨ºy?Y¡ä¨®¦Ì¨ª??¦Ì???????¨¢D?¡ê 
Bit4: 0:WORD¨¤¨¤D¨ª??¦Ì¨ª??¡Á??¨²?3D¨°¨®?PC?¨¤¨ª?¡ê?1:WORD¨¤¨¤D¨ª??¦Ì¨ª??¡Á??¨²?3D¨°¨®?PC?¨¤¡¤¡ä?¡ê 
Bit3~2: ¡À¡ê¨¢??¡ê 
Bit1~0: [00]????¨¦¡§?¨¨¡ê?[01]¡ä1?¡À¨¦¡§?¨¨¡ê?[10]¨ºy?Y????,¡Á??¨²¡ä1?¡À¡ê?[11]¨ºy?Y¡ä1?¡À,¡Á??¨²?????¡ê 
gray: ?¨°?¨¨?¦Ì 
   ?¨°?¨¨?¦Ì¡ê?1:¦Ì£¤¨¦?¡ê?2:???¨°¡ê?4:¨º?¨¢¨´?¨°¡ê?8:256¨¦?¡ê?12:4096¨¦?¡ê?16:16??2¨º¨¦?¡ê?24:24??2¨º¨¦?¡ê?32:32??2¨º¨¦??¡ê
w: ¨ª???¦Ì??¨ª?¨¨?¡ê
h: ¨ª???¦Ì????¨¨?¡ê
  */ 
typedef struct _HEADGRAY
{
   unsigned char scan;
   unsigned char gray;
   unsigned short w;
   unsigned short h;
}HEADGRAY;


/*
Image2Lcd¡À¡ê¡ä?¦Ì?¨ª???¨ºy?Y¡Á¨¦?¡¥¡¤?¨º??a¡êo¨ª???¨ª¡¤¨ºy?Y-¦Ì¡Â¨¦?¡ã?¨ºy?Y-¨ª???¨ºy?Y?¡ê
?¡ã4096¨¦?/16????2¨º¨¦?/18????2¨º¨¦?/24????2¨º¨¦?/32????2¨º¨¦??¡À¦Ì?¨ª???¨ºy?Y¨ª¡¤¨¨???¡êo

scan: ¨¦¡§?¨¨?¡ê¨º? 
Bit7: 0:¡Á?¡Á¨®?¨¢¨®¨°¨¦¡§?¨¨¡ê?1:¡Á?¨®¨°?¨¢¡Á¨®¨¦¡§?¨¨?¡ê 
Bit6: 0:¡Á??£¤?¨¢¦Ì¡Á¨¦¡§?¨¨¡ê?1:¡Á?¦Ì¡Á?¨¢?£¤¨¦¡§?¨¨?¡ê 
Bit5: 0:¡Á??¨²?¨²?¨®??¨ºy?Y¡ä¨®????¦Ì?¦Ì¨ª????¨¢D¡ê?1:¡Á??¨²?¨²?¨®??¨ºy?Y¡ä¨®¦Ì¨ª??¦Ì???????¨¢D?¡ê 
Bit4: 0:WORD¨¤¨¤D¨ª??¦Ì¨ª??¡Á??¨²?3D¨°¨®?PC?¨¤¨ª?¡ê?1:WORD¨¤¨¤D¨ª??¦Ì¨ª??¡Á??¨²?3D¨°¨®?PC?¨¤¡¤¡ä?¡ê 
Bit3~2: ¡À¡ê¨¢??¡ê 
Bit1~0: [00]????¨¦¡§?¨¨¡ê?[01]¡ä1?¡À¨¦¡§?¨¨¡ê?[10]¨ºy?Y????,¡Á??¨²¡ä1?¡À¡ê?[11]¨ºy?Y¡ä1?¡À,¡Á??¨²?????¡ê 
gray: ?¨°?¨¨?¦Ì 
   ?¨°?¨¨?¦Ì¡ê?1:¦Ì£¤¨¦?¡ê?2:???¨°¡ê?4:¨º?¨¢¨´?¨°¡ê?8:256¨¦?¡ê?12:4096¨¦?¡ê?16:16??2¨º¨¦?¡ê?24:24??2¨º¨¦?¡ê?32:32??2¨º¨¦??¡ê

w: ¨ª???¦Ì??¨ª?¨¨?¡ê
h: ¨ª???¦Ì????¨¨?¡ê

is565: ?¨²4096¨¦??¡ê¨º????a0¡À¨ª¨º?¨º1¨®?[16bits(WORD)]??¨º?¡ê?¡ä?¨º¡À¨ª???¨ºy?Y?D????WORD¡À¨ª¨º?¨°????¨®??¡ê??a1
¡À¨ª¨º?¨º1¨®?[12bits(¨¢?D?¡Á??¨²¨¢¡Â)]??¨º?¡ê?¡ä?¨º¡À¨¢?D???¨¢D¦Ì???12Bits¡ä¨²¡À¨ª¨°????¨®???¡ê
?¨²16??2¨º¨¦??¡ê¨º????a0¡À¨ª¨º?R G B??¨¦?¡¤?¨¢??¨´??¨®?¦Ì???¨ºy???a5Bits¡ê??a1¡À¨ª¨º?R G B??¨¦?¡¤?¨¢??¨´??¨®?¦Ì???¨ºy¡¤?¡Àe?a
5Bits,6Bits,5Bits?¡ê
?¨²18??2¨º¨¦??¡ê¨º????a0¡À¨ª¨º?"6Bits in Low Byte"¡ê??a1¡À¨ª¨º?"6Bits in High Byte"?¡ê
?¨²24??2¨º¨¦?o¨ª32??2¨º¨¦??¡ê¨º???is565?TD¡ì?¡ê

rgb: ?¨¨¨º?R G B??¨¦?¡¤?¨¢?¦Ì???¨¢D?3D¨°¡ê?rgb?D??2Bits¡À¨ª¨º?¨°?????¨¦?¡¤?¨¢?¡ê?[00]¡À¨ª¨º???¡ã¡Á¡ê?[01]¡À¨ª¨º?Red¡ê?[10]
¡À¨ª¨º?Green¡ê?[11]¡À¨ª¨º?Blue?¡ê

?¡ã256¨¦??¡À¦Ì?¦Ì¡Â¨¦?¡ã?¨ºy?Y?¨¢11¨¨???¡êo

typedef struct _PALENTRY
{
   unsigned char red;
   unsigned char green;
   unsigned char blue;
}PALENTRY;

typedef struct _PALETTE
{
   unsigned short palnum;
   PALENTRY palentry[palnum];
}PALETTE;
???¨²256¨¦??¡ê¨º???¡ä??¨²¦Ì¡Â¨¦?¡ã?¨ºy?Y?¨¢11,¦Ì¡Â¨¦?¡ã?¨ºy?Y?¨¢11???¨²?¨²¨ºy?Y?¨¢11HEADGRAY??o¨®?¡ê
*/
typedef struct _HEADCOLOR
{
   unsigned char scan;
   unsigned char gray;
   unsigned short w;
   unsigned short h;
   unsigned char is565;
   unsigned char rgb;
}HEADCOLOR; 


/* LCD¦Ì¡À?¡ã¦Ì?¨®¡é??¡Á??a */
extern sFONT *LCD_Currentfonts;
/* ¨¦?????¨®-??¡Á?¡Á??a */
extern const uint8_t WelcomeStr[13][72];
/* ST¦Ì?LOGO?¨°?¨¨?t?¦Ì¨ª???¨ºy?Y?a */
extern const unsigned char gImage_ST_ICON[262];

/* ¨¦???IVo?LCD2¨º?¨¢o¡¥¨ºy¨¦¨´?¡Â   www.armjishu.com Team  */ 
void LCD_Setup(void);
void SZ_STM32_LCDInit(void);
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor); 
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor);
void LCD_SetTextColor(__IO uint16_t Color);
void LCD_SetBackColor(__IO uint16_t Color);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_Clear(uint16_t Color);
void LCD_SetPoint(uint16_t Xpos, uint16_t Ypos, uint16_t Color);
void PutPixel(int16_t Xpos, int16_t Ypos);
uint16_t LCD_GetPoint(uint16_t Xpos, uint16_t Ypos);
void LCD_RAM_WrRdTest(void);
void LCD_DrawBigPoint(uint16_t xScreen, uint16_t yScreen, uint16_t Color);
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction);
void LCD_DrawCross(uint16_t Xpos, uint16_t Ypos);
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void LCD_DrawMonoBinaryPic(uint16_t StartX, uint16_t StartY, uint16_t Width, uint16_t Height, const uint8_t *BmpDataAddress);
void LCD_Image2LcdDrawMonoBinaryPic(uint16_t StartX, uint16_t StartY, const uint8_t *BmpAddress);
void LCD_DrawBmp565Pic(uint16_t StartX, uint16_t StartY, uint16_t Width, uint16_t Height, const uint16_t *BmpAddress);
void LCD_Image2LcdDrawBmp565Pic(uint16_t StartX, uint16_t StartY, const uint8_t *BmpAddress);
    
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void LCD_PolyLine(pPoint Points, uint16_t PointCount);
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount);
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount);
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount);
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount);

void LCD_WriteRegIndex(uint8_t LCD_Reg);
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAMWord(uint16_t RGB_Code);
uint16_t LCD_ReadReg(uint8_t LCD_Reg);
void LCD_WriteRAM(uint16_t RGB_Code);
void LCD_PowerOn(void);
void LCD_Pins_Config(void);
void LLCD_WRITE_CMD(uint8_t LCD_Reg);

void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos,  const uint8_t *cpFontArray);
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii);
void LCD_SetFont(sFONT *fonts);
sFONT *LCD_GetFont(void);
void LCD_SetHyaline(uint8_t NewHyalineBack);
void LCD_ClearLine(uint16_t Line);
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr);
void LCD_DisplayString(uint16_t Xpos, uint16_t Ypos, uint8_t *ptr);
void LCD_DisplayWelcomeStr(uint16_t Line);
void LCD_DisplayCurrentFont(void);
void LCD_DisplayAllFont(void);
void LCD_setWindowArea(uint16_t StartX, uint16_t StartY, uint16_t Width, uint16_t Height);
void LLCD_WRITE_CMD(uint8_t LCD_Reg);

#ifdef __cplusplus
}
#endif

#endif /* __SZ_STM32F107VC_LCD_H */
