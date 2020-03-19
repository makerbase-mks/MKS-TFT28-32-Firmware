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
* Source file: bmp_extru2H
* Dimensions:  80 * 25
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

static GUI_CONST_STORAGE GUI_COLOR Colorsbmp_extru2H[] = {
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

static GUI_CONST_STORAGE GUI_LOGPALETTE Palbmp_extru2H = {
  256,	/* number of entries */
  0, 	/* No transparency */
  &Colorsbmp_extru2H[0]
};

static GUI_CONST_STORAGE unsigned char acbmp_extru2H[] = {
  /* RLE: 352 Pixels @ 000,000*/ 254, 0xE0, 98, 0xE0, 
  /* RLE: 001 Pixels @ 032,004*/ 1, 0xE1, 
  /* RLE: 011 Pixels @ 033,004*/ 11, 0xFF, 
  /* RLE: 001 Pixels @ 044,004*/ 1, 0xE1, 
  /* RLE: 066 Pixels @ 045,004*/ 66, 0xE0, 
  /* RLE: 005 Pixels @ 031,005*/ 5, 0xFF, 
  /* RLE: 001 Pixels @ 036,005*/ 1, 0xE9, 
  /* RLE: 004 Pixels @ 037,005*/ 4, 0xE1, 
  /* RLE: 005 Pixels @ 041,005*/ 5, 0xFF, 
  /* RLE: 001 Pixels @ 046,005*/ 1, 0xE1, 
  /* RLE: 062 Pixels @ 047,005*/ 62, 0xE0, 
  /* RLE: 004 Pixels @ 029,006*/ 4, 0xFF, 
  /* RLE: 011 Pixels @ 033,006*/ 11, 0xE0, 
  /* RLE: 004 Pixels @ 044,006*/ 4, 0xFF, 
  /* RLE: 001 Pixels @ 048,006*/ 1, 0xE1, 
  /* RLE: 059 Pixels @ 049,006*/ 59, 0xE0, 
  /* RLE: 003 Pixels @ 028,007*/ 3, 0xFF, 
  /* RLE: 005 Pixels @ 031,007*/ 5, 0xE0, 
  /* RLE: 001 Pixels @ 036,007*/ 1, 0xE9, 
  /* RLE: 004 Pixels @ 037,007*/ 4, 0xFF, 
  /* RLE: 001 Pixels @ 041,007*/ 1, 0xE9, 
  /* RLE: 004 Pixels @ 042,007*/ 4, 0xE0, 
  /* RLE: 003 Pixels @ 046,007*/ 3, 0xFF, 
  /* RLE: 058 Pixels @ 049,007*/ 58, 0xE0, 
  /* RLE: 003 Pixels @ 027,008*/ 3, 0xFF, 
  /* RLE: 006 Pixels @ 030,008*/ 6, 0xE0, 
  /* ABS: 006 Pixels @ 036,008*/ 0, 6, 0xFF, 0xFF, 0xE1, 0xE1, 0xFF, 0xFF, 
  /* RLE: 005 Pixels @ 042,008*/ 5, 0xE0, 
  /* RLE: 003 Pixels @ 047,008*/ 3, 0xFF, 
  /* RLE: 057 Pixels @ 050,008*/ 57, 0xE0, 
  /* ABS: 002 Pixels @ 027,009*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 010 Pixels @ 029,009*/ 10, 0xE0, 
  /* ABS: 003 Pixels @ 039,009*/ 0, 3, 0xE1, 0xFF, 0xFF, 
  /* RLE: 006 Pixels @ 042,009*/ 6, 0xE0, 
  /* ABS: 002 Pixels @ 048,009*/ 0, 2, 0xFE, 0xFF, 
  /* RLE: 057 Pixels @ 050,009*/ 57, 0xE0, 
  /* RLE: 003 Pixels @ 027,010*/ 3, 0xFF, 
  /* RLE: 007 Pixels @ 030,010*/ 7, 0xE0, 
  /* RLE: 004 Pixels @ 037,010*/ 4, 0xFF, 
  /* RLE: 006 Pixels @ 041,010*/ 6, 0xE0, 
  /* RLE: 003 Pixels @ 047,010*/ 3, 0xFF, 
  /* RLE: 057 Pixels @ 050,010*/ 57, 0xE0, 
  /* RLE: 004 Pixels @ 027,011*/ 4, 0xFF, 
  /* RLE: 005 Pixels @ 031,011*/ 5, 0xE0, 
  /* RLE: 003 Pixels @ 036,011*/ 3, 0xFF, 
  /* ABS: 003 Pixels @ 039,011*/ 0, 3, 0xEA, 0xE9, 0xE9, 
  /* RLE: 004 Pixels @ 042,011*/ 4, 0xE0, 
  /* ABS: 004 Pixels @ 046,011*/ 0, 4, 0xE1, 0xFF, 0xFF, 0xFF, 
  /* RLE: 058 Pixels @ 050,011*/ 58, 0xE0, 
  /* RLE: 004 Pixels @ 028,012*/ 4, 0xFF, 
  /* RLE: 004 Pixels @ 032,012*/ 4, 0xE0, 
  /* RLE: 006 Pixels @ 036,012*/ 6, 0xFF, 
  /* RLE: 003 Pixels @ 042,012*/ 3, 0xE0, 
  /* RLE: 004 Pixels @ 045,012*/ 4, 0xFF, 
  /* RLE: 060 Pixels @ 049,012*/ 60, 0xE0, 
  /* ABS: 004 Pixels @ 029,013*/ 0, 4, 0xE1, 0xFF, 0xFF, 0xFF, 
  /* RLE: 011 Pixels @ 033,013*/ 11, 0xE0, 
  /* RLE: 004 Pixels @ 044,013*/ 4, 0xFF, 
  /* RLE: 063 Pixels @ 048,013*/ 63, 0xE0, 
  /* RLE: 004 Pixels @ 031,014*/ 4, 0xFF, 
  /* RLE: 007 Pixels @ 035,014*/ 7, 0xE0, 
  /* RLE: 004 Pixels @ 042,014*/ 4, 0xFF, 
  /* RLE: 001 Pixels @ 046,014*/ 1, 0xE1, 
  /* RLE: 065 Pixels @ 047,014*/ 65, 0xE0, 
  /* RLE: 001 Pixels @ 032,015*/ 1, 0xE1, 
  /* RLE: 004 Pixels @ 033,015*/ 4, 0xFF, 
  /* RLE: 003 Pixels @ 037,015*/ 3, 0xE0, 
  /* RLE: 001 Pixels @ 040,015*/ 1, 0xE1, 
  /* RLE: 004 Pixels @ 041,015*/ 4, 0xFF, 
  /* RLE: 069 Pixels @ 045,015*/ 69, 0xE0, 
  /* RLE: 009 Pixels @ 034,016*/ 9, 0xFF, 
  /* RLE: 001 Pixels @ 043,016*/ 1, 0xE1, 
  /* RLE: 072 Pixels @ 044,016*/ 72, 0xE0, 
  /* RLE: 001 Pixels @ 036,017*/ 1, 0xE1, 
  /* RLE: 004 Pixels @ 037,017*/ 4, 0xFF, 
  /* RLE: 001 Pixels @ 041,017*/ 1, 0xE1, 
  /* RLE: 076 Pixels @ 042,017*/ 76, 0xE0, 
  /* RLE: 001 Pixels @ 038,018*/ 1, 0xE1, 
  /* RLE: 077 Pixels @ 039,018*/ 77, 0xE0, 
  /* RLE: 001 Pixels @ 036,019*/ 1, 0xE1, 
  /* RLE: 004 Pixels @ 037,019*/ 4, 0xFF, 
  /* RLE: 001 Pixels @ 041,019*/ 1, 0xE1, 
  /* RLE: 074 Pixels @ 042,019*/ 74, 0xE0, 
  /* RLE: 006 Pixels @ 036,020*/ 6, 0xFF, 
  /* RLE: 074 Pixels @ 042,020*/ 74, 0xE0, 
  /* ABS: 006 Pixels @ 036,021*/ 0, 6, 0xE1, 0xFF, 0xF6, 0xF6, 0xFF, 0xE1, 
  /* RLE: 278 Pixels @ 042,021*/ 254, 0xE0, 24, 0xE0, 


  0};  /* 208 for 2000 pixels */

GUI_CONST_STORAGE GUI_BITMAP bmbmp_extru2H = {
  80, /* XSize */
  25, /* YSize */
  80, /* BytesPerLine */
  GUI_COMPRESS_RLE8, /* BitsPerPixel */
  acbmp_extru2H,  /* Pointer to picture data (indices) */
  &Palbmp_extru2H  /* Pointer to palette */
 ,GUI_DRAW_RLE8
};

/* *** End of file *** */
