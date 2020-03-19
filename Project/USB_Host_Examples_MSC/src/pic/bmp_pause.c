/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
*                           www.segger.com                           *
**********************************************************************
*
* C-file generated by
*
*        �C/GUI-BitmapConvert V3.96.
*        Compiled Jul 19 2005, 13:50:35
*          (c) 2002-2005  Micrium, Inc.
  www.micrium.com

  (c) 1998-2005  Segger
  Microcontroller Systeme GmbH
  www.segger.com
*
**********************************************************************
*
* Source file: bmp_pause
* Dimensions:  80 * 50
* NumColors:   256
*
**********************************************************************
*/

#include "stdlib.h"

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

/*   Palette
The following are the entries of the palette table.
Every entry is a 32-bit value (of which 24 bits are actually used)
the lower   8 bits represent the Red component,
the middle  8 bits represent the Green component,
the highest 8 bits (of the 24 bits used) represent the Blue component
as follows:   0xBBGGRR
*/

static GUI_CONST_STORAGE GUI_COLOR Colorsbmp_pause[] = {
     0x000000,0x000024,0x000048,0x00006D
    ,0x000091,0x0000B6,0x0000DA,0x0000FF
    ,0x005500,0x005524,0x005548,0x00556D
    ,0x005591,0x0055B6,0x0055DA,0x0055FF
    ,0x00AA00,0x00AA24,0x00AA48,0x00AA6D
    ,0x00AA91,0x00AAB6,0x00AADA,0x00AAFF
    ,0x00FF00,0x00FF24,0x00FF48,0x00FF6D
    ,0x00FF91,0x00FFB6,0x00FFDA,0x00FFFF
    ,0x240000,0x240024,0x240048,0x24006D
    ,0x240091,0x2400B6,0x2400DA,0x2400FF
    ,0x245500,0x245524,0x245548,0x24556D
    ,0x245591,0x2455B6,0x2455DA,0x2455FF
    ,0x24AA00,0x24AA24,0x24AA48,0x24AA6D
    ,0x24AA91,0x24AAB6,0x24AADA,0x24AAFF
    ,0x24FF00,0x24FF24,0x24FF48,0x24FF6D
    ,0x24FF91,0x24FFB6,0x24FFDA,0x24FFFF
    ,0x480000,0x480024,0x480048,0x48006D
    ,0x480091,0x4800B6,0x4800DA,0x4800FF
    ,0x485500,0x485524,0x485548,0x48556D
    ,0x485591,0x4855B6,0x4855DA,0x4855FF
    ,0x48AA00,0x48AA24,0x48AA48,0x48AA6D
    ,0x48AA91,0x48AAB6,0x48AADA,0x48AAFF
    ,0x48FF00,0x48FF24,0x48FF48,0x48FF6D
    ,0x48FF91,0x48FFB6,0x48FFDA,0x48FFFF
    ,0x6D0000,0x6D0024,0x6D0048,0x6D006D
    ,0x6D0091,0x6D00B6,0x6D00DA,0x6D00FF
    ,0x6D5500,0x6D5524,0x6D5548,0x6D556D
    ,0x6D5591,0x6D55B6,0x6D55DA,0x6D55FF
    ,0x6DAA00,0x6DAA24,0x6DAA48,0x6DAA6D
    ,0x6DAA91,0x6DAAB6,0x6DAADA,0x6DAAFF
    ,0x6DFF00,0x6DFF24,0x6DFF48,0x6DFF6D
    ,0x6DFF91,0x6DFFB6,0x6DFFDA,0x6DFFFF
    ,0x910000,0x910024,0x910048,0x91006D
    ,0x910091,0x9100B6,0x9100DA,0x9100FF
    ,0x915500,0x915524,0x915548,0x91556D
    ,0x915591,0x9155B6,0x9155DA,0x9155FF
    ,0x91AA00,0x91AA24,0x91AA48,0x91AA6D
    ,0x91AA91,0x91AAB6,0x91AADA,0x91AAFF
    ,0x91FF00,0x91FF24,0x91FF48,0x91FF6D
    ,0x91FF91,0x91FFB6,0x91FFDA,0x91FFFF
    ,0xB60000,0xB60024,0xB60048,0xB6006D
    ,0xB60091,0xB600B6,0xB600DA,0xB600FF
    ,0xB65500,0xB65524,0xB65548,0xB6556D
    ,0xB65591,0xB655B6,0xB655DA,0xB655FF
    ,0xB6AA00,0xB6AA24,0xB6AA48,0xB6AA6D
    ,0xB6AA91,0xB6AAB6,0xB6AADA,0xB6AAFF
    ,0xB6FF00,0xB6FF24,0xB6FF48,0xB6FF6D
    ,0xB6FF91,0xB6FFB6,0xB6FFDA,0xB6FFFF
    ,0xDA0000,0xDA0024,0xDA0048,0xDA006D
    ,0xDA0091,0xDA00B6,0xDA00DA,0xDA00FF
    ,0xDA5500,0xDA5524,0xDA5548,0xDA556D
    ,0xDA5591,0xDA55B6,0xDA55DA,0xDA55FF
    ,0xDAAA00,0xDAAA24,0xDAAA48,0xDAAA6D
    ,0xDAAA91,0xDAAAB6,0xDAAADA,0xDAAAFF
    ,0xDAFF00,0xDAFF24,0xDAFF48,0xDAFF6D
    ,0xDAFF91,0xDAFFB6,0xDAFFDA,0xDAFFFF
    ,0xFF0000,0xFF0024,0xFF0048,0xFF006D
    ,0xFF0091,0xFF00B6,0xFF00DA,0xFF00FF
    ,0xFF5500,0xFF5524,0xFF5548,0xFF556D
    ,0xFF5591,0xFF55B6,0xFF55DA,0xFF55FF
    ,0xFFAA00,0xFFAA24,0xFFAA48,0xFFAA6D
    ,0xFFAA91,0xFFAAB6,0xFFAADA,0xFFAAFF
    ,0xFFFF00,0xFFFF24,0xFFFF48,0xFFFF6D
    ,0xFFFF91,0xFFFFB6,0xFFFFDA,0xFFFFFF
};

static GUI_CONST_STORAGE GUI_LOGPALETTE Palbmp_pause = {
  256,	/* number of entries */
  0, 	/* No transparency */
  &Colorsbmp_pause[0]
};

static GUI_CONST_STORAGE unsigned char acbmp_pause[] = {
  /* RLE: 1147 Pixels @ 000,000*/ 254, 0xE0, 254, 0xE0, 254, 0xE0, 254, 0xE0, 131, 0xE0, 
  /* RLE: 008 Pixels @ 027,014*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,014*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,014*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,014*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,015*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,015*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,015*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,015*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,016*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,016*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,016*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,016*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,017*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,017*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,017*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,017*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,018*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,018*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,018*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,018*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,019*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,019*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,019*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,019*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,020*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,020*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,020*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,020*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,021*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,021*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,021*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,021*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,022*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,022*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,022*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,022*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,023*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,023*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,023*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,023*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,024*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,024*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,024*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,024*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,025*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,025*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,025*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,025*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,026*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,026*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,026*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,026*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,027*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,027*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,027*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,027*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,028*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,028*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,028*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,028*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,029*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,029*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,029*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,029*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,030*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,030*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,030*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,030*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,031*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,031*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,031*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,031*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,032*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,032*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,032*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,032*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,033*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,033*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,033*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,033*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,034*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,034*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,034*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,034*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,035*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,035*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,035*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,035*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,036*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,036*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,036*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,036*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,037*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,037*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,037*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,037*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,038*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,038*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,038*/ 8, 0xFF, 
  /* RLE: 053 Pixels @ 054,038*/ 53, 0xE0, 
  /* RLE: 008 Pixels @ 027,039*/ 8, 0xFF, 
  /* RLE: 011 Pixels @ 035,039*/ 11, 0xE0, 
  /* RLE: 008 Pixels @ 046,039*/ 8, 0xFF, 
  /* RLE: 826 Pixels @ 054,039*/ 254, 0xE0, 254, 0xE0, 254, 0xE0, 64, 0xE0, 


  0};  /* 224 for 4000 pixels */

GUI_CONST_STORAGE GUI_BITMAP bmbmp_pause = {
  80, /* XSize */
  50, /* YSize */
  80, /* BytesPerLine */
  GUI_COMPRESS_RLE8, /* BitsPerPixel */
  acbmp_pause,  /* Pointer to picture data (indices) */
  &Palbmp_pause  /* Pointer to palette */
 ,GUI_DRAW_RLE8
};

/* *** End of file *** */
