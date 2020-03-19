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
* Source file: bmp_step_extru
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

static GUI_CONST_STORAGE GUI_COLOR Colorsbmp_step_extru[] = {
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

static GUI_CONST_STORAGE GUI_LOGPALETTE Palbmp_step_extru = {
  256,	/* number of entries */
  0, 	/* No transparency */
  &Colorsbmp_step_extru[0]
};

static GUI_CONST_STORAGE unsigned char acbmp_step_extru[] = {
  /* RLE: 289 Pixels @ 000,000*/ 254, 0xE0, 35, 0xE0, 
  /* RLE: 004 Pixels @ 049,003*/ 4, 0xFF, 
  /* RLE: 001 Pixels @ 053,003*/ 1, 0xE1, 
  /* RLE: 075 Pixels @ 054,003*/ 75, 0xE0, 
  /* ABS: 002 Pixels @ 049,004*/ 0, 2, 0xFF, 0xE9, 
  /* RLE: 078 Pixels @ 051,004*/ 78, 0xE0, 
  /* ABS: 002 Pixels @ 049,005*/ 0, 2, 0xFF, 0xE9, 
  /* RLE: 078 Pixels @ 051,005*/ 78, 0xE0, 
  /* RLE: 004 Pixels @ 049,006*/ 4, 0xFF, 
  /* RLE: 078 Pixels @ 053,006*/ 78, 0xE0, 
  /* ABS: 003 Pixels @ 051,007*/ 0, 3, 0xEA, 0xFF, 0xFF, 
  /* RLE: 078 Pixels @ 054,007*/ 78, 0xE0, 
  /* ABS: 002 Pixels @ 052,008*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 066 Pixels @ 054,008*/ 66, 0xE0, 
  /* ABS: 003 Pixels @ 040,009*/ 0, 3, 0xC0, 0xFF, 0xFF, 
  /* RLE: 009 Pixels @ 043,009*/ 9, 0xE0, 
  /* ABS: 002 Pixels @ 052,009*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 066 Pixels @ 054,009*/ 66, 0xE0, 
  /* RLE: 001 Pixels @ 040,010*/ 1, 0xC0, 
  /* RLE: 004 Pixels @ 041,010*/ 4, 0xFF, 
  /* RLE: 004 Pixels @ 045,010*/ 4, 0xE0, 
  /* RLE: 004 Pixels @ 049,010*/ 4, 0xFF, 
  /* RLE: 001 Pixels @ 053,010*/ 1, 0xE1, 
  /* RLE: 063 Pixels @ 054,010*/ 63, 0xE0, 
  /* ABS: 002 Pixels @ 037,011*/ 0, 2, 0xC0, 0xE0, 
  /* RLE: 007 Pixels @ 039,011*/ 7, 0xFF, 
  /* RLE: 003 Pixels @ 046,011*/ 3, 0xE0, 
  /* ABS: 003 Pixels @ 049,011*/ 0, 3, 0xFF, 0xFF, 0xE9, 
  /* RLE: 063 Pixels @ 052,011*/ 63, 0xE0, 
  /* RLE: 001 Pixels @ 035,012*/ 1, 0xC0, 
  /* RLE: 012 Pixels @ 036,012*/ 12, 0xFF, 
  /* RLE: 067 Pixels @ 048,012*/ 67, 0xE0, 
  /* RLE: 013 Pixels @ 035,013*/ 13, 0xFF, 
  /* RLE: 004 Pixels @ 048,013*/ 4, 0xE0, 
  /* ABS: 002 Pixels @ 052,013*/ 0, 2, 0xA0, 0xC0, 
  /* RLE: 060 Pixels @ 054,013*/ 60, 0xE0, 
  /* RLE: 006 Pixels @ 034,014*/ 6, 0xFF, 
  /* RLE: 001 Pixels @ 040,014*/ 1, 0xE0, 
  /* RLE: 005 Pixels @ 041,014*/ 5, 0xFF, 
  /* RLE: 005 Pixels @ 046,014*/ 5, 0xE0, 
  /* RLE: 004 Pixels @ 051,014*/ 4, 0xFF, 
  /* RLE: 058 Pixels @ 055,014*/ 58, 0xE0, 
  /* RLE: 005 Pixels @ 033,015*/ 5, 0xFF, 
  /* ABS: 006 Pixels @ 038,015*/ 0, 6, 0xC0, 0xE0, 0xE0, 0xFF, 0xFF, 0xFF, 
  /* RLE: 007 Pixels @ 044,015*/ 7, 0xE0, 
  /* RLE: 005 Pixels @ 051,015*/ 5, 0xFF, 
  /* RLE: 056 Pixels @ 056,015*/ 56, 0xE0, 
  /* RLE: 004 Pixels @ 032,016*/ 4, 0xFF, 
  /* RLE: 005 Pixels @ 036,016*/ 5, 0xE0, 
  /* ABS: 003 Pixels @ 041,016*/ 0, 3, 0xFF, 0xFF, 0xC0, 
  /* RLE: 007 Pixels @ 044,016*/ 7, 0xE0, 
  /* RLE: 001 Pixels @ 051,016*/ 1, 0xC0, 
  /* RLE: 004 Pixels @ 052,016*/ 4, 0xFF, 
  /* RLE: 054 Pixels @ 056,016*/ 54, 0xE0, 
  /* RLE: 001 Pixels @ 030,017*/ 1, 0xC0, 
  /* RLE: 004 Pixels @ 031,017*/ 4, 0xFF, 
  /* RLE: 017 Pixels @ 035,017*/ 17, 0xE0, 
  /* RLE: 001 Pixels @ 052,017*/ 1, 0xC0, 
  /* RLE: 004 Pixels @ 053,017*/ 4, 0xFF, 
  /* RLE: 001 Pixels @ 057,017*/ 1, 0xC0, 
  /* RLE: 053 Pixels @ 058,017*/ 53, 0xE0, 
  /* RLE: 003 Pixels @ 031,018*/ 3, 0xFF, 
  /* RLE: 019 Pixels @ 034,018*/ 19, 0xE0, 
  /* RLE: 001 Pixels @ 053,018*/ 1, 0xC0, 
  /* RLE: 004 Pixels @ 054,018*/ 4, 0xFF, 
  /* RLE: 051 Pixels @ 058,018*/ 51, 0xE0, 
  /* ABS: 004 Pixels @ 029,019*/ 0, 4, 0xC0, 0xFF, 0xFF, 0xFF, 
  /* RLE: 022 Pixels @ 033,019*/ 22, 0xE0, 
  /* RLE: 003 Pixels @ 055,019*/ 3, 0xFF, 
  /* RLE: 001 Pixels @ 058,019*/ 1, 0xC0, 
  /* RLE: 051 Pixels @ 059,019*/ 51, 0xE0, 
  /* RLE: 003 Pixels @ 030,020*/ 3, 0xFF, 
  /* RLE: 001 Pixels @ 033,020*/ 1, 0xC0, 
  /* RLE: 020 Pixels @ 034,020*/ 20, 0xE0, 
  /* ABS: 004 Pixels @ 054,020*/ 0, 4, 0xC0, 0xFF, 0xFF, 0xFF, 
  /* RLE: 051 Pixels @ 058,020*/ 51, 0xE0, 
  /* RLE: 003 Pixels @ 029,021*/ 3, 0xFF, 
  /* RLE: 024 Pixels @ 032,021*/ 24, 0xE0, 
  /* RLE: 003 Pixels @ 056,021*/ 3, 0xFF, 
  /* RLE: 050 Pixels @ 059,021*/ 50, 0xE0, 
  /* RLE: 003 Pixels @ 029,022*/ 3, 0xFF, 
  /* RLE: 024 Pixels @ 032,022*/ 24, 0xE0, 
  /* RLE: 003 Pixels @ 056,022*/ 3, 0xFF, 
  /* RLE: 001 Pixels @ 059,022*/ 1, 0xC0, 
  /* RLE: 049 Pixels @ 060,022*/ 49, 0xE0, 
  /* RLE: 003 Pixels @ 029,023*/ 3, 0xFF, 
  /* RLE: 001 Pixels @ 032,023*/ 1, 0xC0, 
  /* RLE: 023 Pixels @ 033,023*/ 23, 0xE0, 
  /* RLE: 003 Pixels @ 056,023*/ 3, 0xFF, 
  /* RLE: 050 Pixels @ 059,023*/ 50, 0xE0, 
  /* ABS: 002 Pixels @ 029,024*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 025 Pixels @ 031,024*/ 25, 0xE0, 
  /* RLE: 003 Pixels @ 056,024*/ 3, 0xFF, 
  /* RLE: 046 Pixels @ 059,024*/ 46, 0xE0, 
  /* RLE: 001 Pixels @ 025,025*/ 1, 0xE1, 
  /* RLE: 004 Pixels @ 026,025*/ 4, 0xE0, 
  /* RLE: 001 Pixels @ 030,025*/ 1, 0xC0, 
  /* RLE: 025 Pixels @ 031,025*/ 25, 0xE0, 
  /* RLE: 003 Pixels @ 056,025*/ 3, 0xFF, 
  /* RLE: 044 Pixels @ 059,025*/ 44, 0xE0, 
  /* ABS: 003 Pixels @ 023,026*/ 0, 3, 0xFF, 0xFF, 0xFE, 
  /* RLE: 030 Pixels @ 026,026*/ 30, 0xE0, 
  /* RLE: 003 Pixels @ 056,026*/ 3, 0xFF, 
  /* RLE: 044 Pixels @ 059,026*/ 44, 0xE0, 
  /* ABS: 003 Pixels @ 023,027*/ 0, 3, 0xFF, 0xEA, 0xFE, 
  /* RLE: 030 Pixels @ 026,027*/ 30, 0xE0, 
  /* RLE: 003 Pixels @ 056,027*/ 3, 0xFF, 
  /* RLE: 045 Pixels @ 059,027*/ 45, 0xE0, 
  /* ABS: 002 Pixels @ 024,028*/ 0, 2, 0xE9, 0xFE, 
  /* RLE: 005 Pixels @ 026,028*/ 5, 0xE0, 
  /* RLE: 001 Pixels @ 031,028*/ 1, 0xC0, 
  /* RLE: 021 Pixels @ 032,028*/ 21, 0xE0, 
  /* RLE: 006 Pixels @ 053,028*/ 6, 0xFF, 
  /* RLE: 045 Pixels @ 059,028*/ 45, 0xE0, 
  /* ABS: 002 Pixels @ 024,029*/ 0, 2, 0xE9, 0xFE, 
  /* RLE: 004 Pixels @ 026,029*/ 4, 0xE0, 
  /* ABS: 004 Pixels @ 030,029*/ 0, 4, 0xFF, 0xFF, 0xE0, 0xC0, 
  /* RLE: 019 Pixels @ 034,029*/ 19, 0xE0, 
  /* RLE: 006 Pixels @ 053,029*/ 6, 0xFF, 
  /* RLE: 045 Pixels @ 059,029*/ 45, 0xE0, 
  /* ABS: 002 Pixels @ 024,030*/ 0, 2, 0xE9, 0xFE, 
  /* RLE: 004 Pixels @ 026,030*/ 4, 0xE0, 
  /* RLE: 004 Pixels @ 030,030*/ 4, 0xFF, 
  /* RLE: 019 Pixels @ 034,030*/ 19, 0xE0, 
  /* RLE: 007 Pixels @ 053,030*/ 7, 0xFF, 
  /* RLE: 044 Pixels @ 060,030*/ 44, 0xE0, 
  /* ABS: 002 Pixels @ 024,031*/ 0, 2, 0xE9, 0xFE, 
  /* RLE: 004 Pixels @ 026,031*/ 4, 0xE0, 
  /* RLE: 006 Pixels @ 030,031*/ 6, 0xFF, 
  /* RLE: 017 Pixels @ 036,031*/ 17, 0xE0, 
  /* RLE: 007 Pixels @ 053,031*/ 7, 0xFF, 
  /* RLE: 044 Pixels @ 060,031*/ 44, 0xE0, 
  /* ABS: 002 Pixels @ 024,032*/ 0, 2, 0xE9, 0xFE, 
  /* RLE: 004 Pixels @ 026,032*/ 4, 0xE0, 
  /* RLE: 007 Pixels @ 030,032*/ 7, 0xFF, 
  /* RLE: 001 Pixels @ 037,032*/ 1, 0xC0, 
  /* RLE: 015 Pixels @ 038,032*/ 15, 0xE0, 
  /* RLE: 006 Pixels @ 053,032*/ 6, 0xFF, 
  /* RLE: 001 Pixels @ 059,032*/ 1, 0xC0, 
  /* RLE: 043 Pixels @ 060,032*/ 43, 0xE0, 
  /* RLE: 005 Pixels @ 023,033*/ 5, 0xFF, 
  /* ABS: 002 Pixels @ 028,033*/ 0, 2, 0xE0, 0xE0, 
  /* RLE: 006 Pixels @ 030,033*/ 6, 0xFF, 
  /* RLE: 017 Pixels @ 036,033*/ 17, 0xE0, 
  /* RLE: 004 Pixels @ 053,033*/ 4, 0xFF, 
  /* RLE: 046 Pixels @ 057,033*/ 46, 0xE0, 
  /* RLE: 004 Pixels @ 023,034*/ 4, 0xE9, 
  /* ABS: 003 Pixels @ 027,034*/ 0, 3, 0xE1, 0xE0, 0xE0, 
  /* RLE: 006 Pixels @ 030,034*/ 6, 0xFF, 
  /* RLE: 018 Pixels @ 036,034*/ 18, 0xE0, 
  /* RLE: 001 Pixels @ 054,034*/ 1, 0xFF, 
  /* RLE: 055 Pixels @ 055,034*/ 55, 0xE0, 
  /* RLE: 008 Pixels @ 030,035*/ 8, 0xFF, 
  /* RLE: 016 Pixels @ 038,035*/ 16, 0xE0, 
  /* RLE: 001 Pixels @ 054,035*/ 1, 0xE1, 
  /* RLE: 005 Pixels @ 055,035*/ 5, 0xE0, 
  /* RLE: 001 Pixels @ 060,035*/ 1, 0xE9, 
  /* RLE: 049 Pixels @ 061,035*/ 49, 0xE0, 
  /* ABS: 004 Pixels @ 030,036*/ 0, 4, 0xC0, 0xA0, 0xE0, 0xC0, 
  /* RLE: 006 Pixels @ 034,036*/ 6, 0xFF, 
  /* ABS: 023 Pixels @ 040,036*/ 0, 23, 0xE0, 0xA0, 0xC0, 0xC0, 0xE0, 0xE0, 0xA0, 0xFF, 0xFF, 0xFF, 0xC0, 0xE0, 0xFF, 0xFF, 0xFE, 0xE0, 0xE0, 0xE0, 0xE9, 0xF5, 0xF5, 0xFE, 0xEA, 
  /* RLE: 051 Pixels @ 063,036*/ 51, 0xE0, 
  /* ABS: 002 Pixels @ 034,037*/ 0, 2, 0xC0, 0xE0, 
  /* RLE: 014 Pixels @ 036,037*/ 14, 0xFF, 
  /* ABS: 013 Pixels @ 050,037*/ 0, 13, 0xE0, 0xE0, 0xFF, 0xEA, 0xFF, 0xE0, 0xE0, 0xE0, 0xFF, 0xEA, 0xE0, 0xEA, 0xFF, 
  /* RLE: 053 Pixels @ 063,037*/ 53, 0xE0, 
  /* RLE: 001 Pixels @ 036,038*/ 1, 0xC0, 
  /* RLE: 013 Pixels @ 037,038*/ 13, 0xFF, 
  /* ABS: 014 Pixels @ 050,038*/ 0, 14, 0xC0, 0xE0, 0xE0, 0xE9, 0xFF, 0xE0, 0xE0, 0xE1, 0xFF, 0xE0, 0xE0, 0xE0, 0xFF, 0xE1, 
  /* RLE: 054 Pixels @ 064,038*/ 54, 0xE0, 
  /* RLE: 001 Pixels @ 038,039*/ 1, 0xC0, 
  /* RLE: 009 Pixels @ 039,039*/ 9, 0xFF, 
  /* RLE: 005 Pixels @ 048,039*/ 5, 0xE0, 
  /* ABS: 011 Pixels @ 053,039*/ 0, 11, 0xE9, 0xFF, 0xE0, 0xE0, 0xE9, 0xFF, 0xE0, 0xE0, 0xE0, 0xFF, 0xE9, 
  /* RLE: 069 Pixels @ 064,039*/ 69, 0xE0, 
  /* ABS: 011 Pixels @ 053,040*/ 0, 11, 0xE9, 0xFE, 0xE0, 0xE0, 0xE9, 0xFF, 0xE0, 0xE0, 0xE0, 0xFF, 0xE9, 
  /* RLE: 069 Pixels @ 064,040*/ 69, 0xE0, 
  /* ABS: 011 Pixels @ 053,041*/ 0, 11, 0xE9, 0xFF, 0xE0, 0xE0, 0xE1, 0xFF, 0xE0, 0xE0, 0xE0, 0xFF, 0xE1, 
  /* RLE: 069 Pixels @ 064,041*/ 69, 0xE0, 
  /* ABS: 010 Pixels @ 053,042*/ 0, 10, 0xE9, 0xFF, 0xE0, 0xE0, 0xE0, 0xFF, 0xE1, 0xE0, 0xE1, 0xFF, 
  /* RLE: 069 Pixels @ 063,042*/ 69, 0xE0, 
  /* ABS: 011 Pixels @ 052,043*/ 0, 11, 0xF5, 0xF5, 0xFF, 0xFF, 0xF4, 0xE0, 0xEB, 0xFF, 0xFF, 0xFF, 0xEA, 
  /* RLE: 069 Pixels @ 063,043*/ 69, 0xE0, 
  /* RLE: 004 Pixels @ 052,044*/ 4, 0xE9, 
  /* ABS: 006 Pixels @ 056,044*/ 0, 6, 0xE1, 0xE0, 0xE0, 0xE1, 0xEB, 0xE1, 
  /* RLE: 418 Pixels @ 062,044*/ 254, 0xE0, 164, 0xE0, 


  0};  /* 557 for 4000 pixels */

GUI_CONST_STORAGE GUI_BITMAP bmbmp_step_extru = {
  80, /* XSize */
  50, /* YSize */
  80, /* BytesPerLine */
  GUI_COMPRESS_RLE8, /* BitsPerPixel */
  acbmp_step_extru,  /* Pointer to picture data (indices) */
  &Palbmp_step_extru  /* Pointer to palette */
 ,GUI_DRAW_RLE8
};

/* *** End of file *** */
