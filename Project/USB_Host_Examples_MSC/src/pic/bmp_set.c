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
* Source file: bmp_set
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

static GUI_CONST_STORAGE GUI_COLOR Colorsbmp_set[] = {
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

static GUI_CONST_STORAGE GUI_LOGPALETTE Palbmp_set = {
  256,	/* number of entries */
  0, 	/* No transparency */
  &Colorsbmp_set[0]
};

static GUI_CONST_STORAGE unsigned char acbmp_set[] = {
  /* RLE: 758 Pixels @ 000,000*/ 254, 0xE0, 254, 0xE0, 250, 0xE0, 
  /* ABS: 005 Pixels @ 038,009*/ 0, 5, 0x94, 0xB5, 0xB5, 0xB5, 0x94, 
  /* RLE: 075 Pixels @ 043,009*/ 75, 0xE0, 
  /* ABS: 005 Pixels @ 038,010*/ 0, 5, 0xDE, 0xFF, 0xFF, 0xFF, 0xDE, 
  /* RLE: 074 Pixels @ 043,010*/ 74, 0xE0, 
  /* RLE: 001 Pixels @ 037,011*/ 1, 0x94, 
  /* RLE: 005 Pixels @ 038,011*/ 5, 0xFF, 
  /* RLE: 001 Pixels @ 043,011*/ 1, 0x94, 
  /* RLE: 067 Pixels @ 044,011*/ 67, 0xE0, 
  /* RLE: 001 Pixels @ 031,012*/ 1, 0xB5, 
  /* RLE: 005 Pixels @ 032,012*/ 5, 0xE0, 
  /* RLE: 001 Pixels @ 037,012*/ 1, 0xB5, 
  /* RLE: 005 Pixels @ 038,012*/ 5, 0xFF, 
  /* RLE: 001 Pixels @ 043,012*/ 1, 0xB5, 
  /* RLE: 005 Pixels @ 044,012*/ 5, 0xE0, 
  /* RLE: 001 Pixels @ 049,012*/ 1, 0xB5, 
  /* RLE: 060 Pixels @ 050,012*/ 60, 0xE0, 
  /* ABS: 008 Pixels @ 030,013*/ 0, 8, 0xB5, 0xFF, 0xFF, 0xB5, 0xE0, 0xE0, 0xE0, 0xDE, 
  /* RLE: 005 Pixels @ 038,013*/ 5, 0xFF, 
  /* ABS: 008 Pixels @ 043,013*/ 0, 8, 0xDE, 0xE0, 0xE0, 0xE0, 0xB5, 0xFF, 0xFF, 0xB5, 
  /* RLE: 058 Pixels @ 051,013*/ 58, 0xE0, 
  /* RLE: 001 Pixels @ 029,014*/ 1, 0xB5, 
  /* RLE: 004 Pixels @ 030,014*/ 4, 0xFF, 
  /* ABS: 003 Pixels @ 034,014*/ 0, 3, 0xDE, 0xB5, 0xB5, 
  /* RLE: 007 Pixels @ 037,014*/ 7, 0xFF, 
  /* ABS: 003 Pixels @ 044,014*/ 0, 3, 0xB5, 0xB5, 0xDE, 
  /* RLE: 004 Pixels @ 047,014*/ 4, 0xFF, 
  /* RLE: 001 Pixels @ 051,014*/ 1, 0xB5, 
  /* RLE: 056 Pixels @ 052,014*/ 56, 0xE0, 
  /* RLE: 001 Pixels @ 028,015*/ 1, 0x94, 
  /* RLE: 023 Pixels @ 029,015*/ 23, 0xFF, 
  /* RLE: 001 Pixels @ 052,015*/ 1, 0x94, 
  /* RLE: 056 Pixels @ 053,015*/ 56, 0xE0, 
  /* RLE: 001 Pixels @ 029,016*/ 1, 0xDE, 
  /* RLE: 021 Pixels @ 030,016*/ 21, 0xFF, 
  /* RLE: 001 Pixels @ 051,016*/ 1, 0xDE, 
  /* RLE: 057 Pixels @ 052,016*/ 57, 0xE0, 
  /* RLE: 001 Pixels @ 029,017*/ 1, 0xB4, 
  /* RLE: 021 Pixels @ 030,017*/ 21, 0xFF, 
  /* RLE: 001 Pixels @ 051,017*/ 1, 0xB4, 
  /* RLE: 058 Pixels @ 052,017*/ 58, 0xE0, 
  /* RLE: 001 Pixels @ 030,018*/ 1, 0xDE, 
  /* RLE: 019 Pixels @ 031,018*/ 19, 0xFF, 
  /* RLE: 001 Pixels @ 050,018*/ 1, 0xDE, 
  /* RLE: 059 Pixels @ 051,018*/ 59, 0xE0, 
  /* RLE: 001 Pixels @ 030,019*/ 1, 0x94, 
  /* RLE: 007 Pixels @ 031,019*/ 7, 0xFF, 
  /* ABS: 005 Pixels @ 038,019*/ 0, 5, 0xDE, 0xB5, 0xB5, 0xB5, 0xDE, 
  /* RLE: 007 Pixels @ 043,019*/ 7, 0xFF, 
  /* RLE: 001 Pixels @ 050,019*/ 1, 0x94, 
  /* RLE: 059 Pixels @ 051,019*/ 59, 0xE0, 
  /* RLE: 001 Pixels @ 030,020*/ 1, 0xB5, 
  /* RLE: 006 Pixels @ 031,020*/ 6, 0xFF, 
  /* RLE: 001 Pixels @ 037,020*/ 1, 0xB5, 
  /* RLE: 005 Pixels @ 038,020*/ 5, 0xE0, 
  /* RLE: 001 Pixels @ 043,020*/ 1, 0xB5, 
  /* RLE: 006 Pixels @ 044,020*/ 6, 0xFF, 
  /* RLE: 001 Pixels @ 050,020*/ 1, 0xB5, 
  /* RLE: 056 Pixels @ 051,020*/ 56, 0xE0, 
  /* ABS: 003 Pixels @ 027,021*/ 0, 3, 0x94, 0xB5, 0xDE, 
  /* RLE: 006 Pixels @ 030,021*/ 6, 0xFF, 
  /* RLE: 001 Pixels @ 036,021*/ 1, 0xB5, 
  /* RLE: 007 Pixels @ 037,021*/ 7, 0xE0, 
  /* RLE: 001 Pixels @ 044,021*/ 1, 0xB5, 
  /* RLE: 006 Pixels @ 045,021*/ 6, 0xFF, 
  /* ABS: 003 Pixels @ 051,021*/ 0, 3, 0xDE, 0xB5, 0x94, 
  /* RLE: 051 Pixels @ 054,021*/ 51, 0xE0, 
  /* ABS: 002 Pixels @ 025,022*/ 0, 2, 0x94, 0xDE, 
  /* RLE: 008 Pixels @ 027,022*/ 8, 0xFF, 
  /* RLE: 001 Pixels @ 035,022*/ 1, 0xDE, 
  /* RLE: 009 Pixels @ 036,022*/ 9, 0xE0, 
  /* RLE: 001 Pixels @ 045,022*/ 1, 0xDE, 
  /* RLE: 008 Pixels @ 046,022*/ 8, 0xFF, 
  /* ABS: 002 Pixels @ 054,022*/ 0, 2, 0xDE, 0x94, 
  /* RLE: 049 Pixels @ 056,022*/ 49, 0xE0, 
  /* RLE: 001 Pixels @ 025,023*/ 1, 0xB4, 
  /* RLE: 009 Pixels @ 026,023*/ 9, 0xFF, 
  /* RLE: 001 Pixels @ 035,023*/ 1, 0xD6, 
  /* RLE: 009 Pixels @ 036,023*/ 9, 0xE0, 
  /* RLE: 001 Pixels @ 045,023*/ 1, 0xD6, 
  /* RLE: 009 Pixels @ 046,023*/ 9, 0xFF, 
  /* RLE: 001 Pixels @ 055,023*/ 1, 0xB4, 
  /* RLE: 049 Pixels @ 056,023*/ 49, 0xE0, 
  /* RLE: 001 Pixels @ 025,024*/ 1, 0xB4, 
  /* RLE: 009 Pixels @ 026,024*/ 9, 0xFF, 
  /* RLE: 001 Pixels @ 035,024*/ 1, 0xB5, 
  /* RLE: 009 Pixels @ 036,024*/ 9, 0xE0, 
  /* RLE: 001 Pixels @ 045,024*/ 1, 0xB5, 
  /* RLE: 009 Pixels @ 046,024*/ 9, 0xFF, 
  /* RLE: 001 Pixels @ 055,024*/ 1, 0xB4, 
  /* RLE: 049 Pixels @ 056,024*/ 49, 0xE0, 
  /* RLE: 001 Pixels @ 025,025*/ 1, 0xB4, 
  /* RLE: 009 Pixels @ 026,025*/ 9, 0xFF, 
  /* RLE: 001 Pixels @ 035,025*/ 1, 0xD6, 
  /* RLE: 009 Pixels @ 036,025*/ 9, 0xE0, 
  /* RLE: 001 Pixels @ 045,025*/ 1, 0xD6, 
  /* RLE: 009 Pixels @ 046,025*/ 9, 0xFF, 
  /* RLE: 001 Pixels @ 055,025*/ 1, 0xB4, 
  /* RLE: 050 Pixels @ 056,025*/ 50, 0xE0, 
  /* RLE: 001 Pixels @ 026,026*/ 1, 0xDE, 
  /* RLE: 009 Pixels @ 027,026*/ 9, 0xFF, 
  /* RLE: 009 Pixels @ 036,026*/ 9, 0xE0, 
  /* RLE: 009 Pixels @ 045,026*/ 9, 0xFF, 
  /* RLE: 001 Pixels @ 054,026*/ 1, 0xDE, 
  /* RLE: 053 Pixels @ 055,026*/ 53, 0xE0, 
  /* ABS: 002 Pixels @ 028,027*/ 0, 2, 0xB5, 0xD6, 
  /* RLE: 006 Pixels @ 030,027*/ 6, 0xFF, 
  /* RLE: 001 Pixels @ 036,027*/ 1, 0xD6, 
  /* RLE: 007 Pixels @ 037,027*/ 7, 0xE0, 
  /* RLE: 001 Pixels @ 044,027*/ 1, 0xD6, 
  /* RLE: 006 Pixels @ 045,027*/ 6, 0xFF, 
  /* ABS: 002 Pixels @ 051,027*/ 0, 2, 0xD6, 0xB5, 
  /* RLE: 057 Pixels @ 053,027*/ 57, 0xE0, 
  /* RLE: 001 Pixels @ 030,028*/ 1, 0xB4, 
  /* RLE: 006 Pixels @ 031,028*/ 6, 0xFF, 
  /* RLE: 001 Pixels @ 037,028*/ 1, 0xD6, 
  /* RLE: 005 Pixels @ 038,028*/ 5, 0xE0, 
  /* RLE: 001 Pixels @ 043,028*/ 1, 0xD6, 
  /* RLE: 006 Pixels @ 044,028*/ 6, 0xFF, 
  /* RLE: 001 Pixels @ 050,028*/ 1, 0xB4, 
  /* RLE: 059 Pixels @ 051,028*/ 59, 0xE0, 
  /* RLE: 001 Pixels @ 030,029*/ 1, 0xB4, 
  /* RLE: 008 Pixels @ 031,029*/ 8, 0xFF, 
  /* ABS: 003 Pixels @ 039,029*/ 0, 3, 0xD6, 0xD5, 0xD6, 
  /* RLE: 008 Pixels @ 042,029*/ 8, 0xFF, 
  /* RLE: 001 Pixels @ 050,029*/ 1, 0xB4, 
  /* RLE: 059 Pixels @ 051,029*/ 59, 0xE0, 
  /* RLE: 001 Pixels @ 030,030*/ 1, 0xDE, 
  /* RLE: 019 Pixels @ 031,030*/ 19, 0xFF, 
  /* RLE: 001 Pixels @ 050,030*/ 1, 0xDE, 
  /* RLE: 058 Pixels @ 051,030*/ 58, 0xE0, 
  /* RLE: 001 Pixels @ 029,031*/ 1, 0xB5, 
  /* RLE: 021 Pixels @ 030,031*/ 21, 0xFF, 
  /* RLE: 001 Pixels @ 051,031*/ 1, 0xB5, 
  /* RLE: 057 Pixels @ 052,031*/ 57, 0xE0, 
  /* RLE: 001 Pixels @ 029,032*/ 1, 0xDE, 
  /* RLE: 021 Pixels @ 030,032*/ 21, 0xFF, 
  /* RLE: 001 Pixels @ 051,032*/ 1, 0xDE, 
  /* RLE: 056 Pixels @ 052,032*/ 56, 0xE0, 
  /* RLE: 001 Pixels @ 028,033*/ 1, 0x94, 
  /* RLE: 023 Pixels @ 029,033*/ 23, 0xFF, 
  /* RLE: 001 Pixels @ 052,033*/ 1, 0x94, 
  /* RLE: 056 Pixels @ 053,033*/ 56, 0xE0, 
  /* RLE: 001 Pixels @ 029,034*/ 1, 0xB5, 
  /* RLE: 004 Pixels @ 030,034*/ 4, 0xFF, 
  /* ABS: 003 Pixels @ 034,034*/ 0, 3, 0xDE, 0x94, 0xB4, 
  /* RLE: 007 Pixels @ 037,034*/ 7, 0xFF, 
  /* ABS: 003 Pixels @ 044,034*/ 0, 3, 0xB4, 0x94, 0xDE, 
  /* RLE: 004 Pixels @ 047,034*/ 4, 0xFF, 
  /* RLE: 001 Pixels @ 051,034*/ 1, 0xB5, 
  /* RLE: 058 Pixels @ 052,034*/ 58, 0xE0, 
  /* ABS: 008 Pixels @ 030,035*/ 0, 8, 0xB5, 0xFF, 0xDE, 0xB5, 0xE0, 0xE0, 0xE0, 0xD6, 
  /* RLE: 005 Pixels @ 038,035*/ 5, 0xFF, 
  /* ABS: 008 Pixels @ 043,035*/ 0, 8, 0xD6, 0xE0, 0xE0, 0xE0, 0xB5, 0xDE, 0xFF, 0xB5, 
  /* RLE: 060 Pixels @ 051,035*/ 60, 0xE0, 
  /* RLE: 001 Pixels @ 031,036*/ 1, 0x94, 
  /* RLE: 005 Pixels @ 032,036*/ 5, 0xE0, 
  /* RLE: 001 Pixels @ 037,036*/ 1, 0xB5, 
  /* RLE: 005 Pixels @ 038,036*/ 5, 0xFF, 
  /* RLE: 001 Pixels @ 043,036*/ 1, 0xB5, 
  /* RLE: 005 Pixels @ 044,036*/ 5, 0xE0, 
  /* RLE: 001 Pixels @ 049,036*/ 1, 0x94, 
  /* RLE: 068 Pixels @ 050,036*/ 68, 0xE0, 
  /* RLE: 005 Pixels @ 038,037*/ 5, 0xFF, 
  /* RLE: 075 Pixels @ 043,037*/ 75, 0xE0, 
  /* ABS: 005 Pixels @ 038,038*/ 0, 5, 0xDE, 0xFF, 0xFF, 0xFF, 0xDE, 
  /* RLE: 076 Pixels @ 043,038*/ 76, 0xE0, 
  /* RLE: 003 Pixels @ 039,039*/ 3, 0x94, 
  /* RLE: 838 Pixels @ 042,039*/ 254, 0xE0, 254, 0xE0, 254, 0xE0, 76, 0xE0, 


  0};  /* 429 for 4000 pixels */

GUI_CONST_STORAGE GUI_BITMAP bmbmp_set = {
  80, /* XSize */
  50, /* YSize */
  80, /* BytesPerLine */
  GUI_COMPRESS_RLE8, /* BitsPerPixel */
  acbmp_set,  /* Pointer to picture data (indices) */
  &Palbmp_set  /* Pointer to palette */
 ,GUI_DRAW_RLE8
};

/* *** End of file *** */
