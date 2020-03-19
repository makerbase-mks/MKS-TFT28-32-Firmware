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
* Source file: bmp_extruct_sel
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

static GUI_CONST_STORAGE GUI_COLOR Colorsbmp_extruct_sel[] = {
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

static GUI_CONST_STORAGE GUI_LOGPALETTE Palbmp_extruct_sel = {
  256,	/* number of entries */
  0, 	/* No transparency */
  &Colorsbmp_extruct_sel[0]
};

static GUI_CONST_STORAGE unsigned char acbmp_extruct_sel[] = {
  /* RLE: 744 Pixels @ 000,000*/ 254, 0x10, 254, 0x10, 236, 0x10, 
  /* RLE: 032 Pixels @ 024,009*/ 32, 0xFF, 
  /* RLE: 048 Pixels @ 056,009*/ 48, 0x10, 
  /* RLE: 032 Pixels @ 024,010*/ 32, 0xFF, 
  /* RLE: 048 Pixels @ 056,010*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,011*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 026,011*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 034,011*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 036,011*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 044,011*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 046,011*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,011*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,011*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,012*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 026,012*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 034,012*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 036,012*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 044,012*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 046,012*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,012*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,012*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,013*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 026,013*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 034,013*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 036,013*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 044,013*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 046,013*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,013*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,013*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,014*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 026,014*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 034,014*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 036,014*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 044,014*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 046,014*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,014*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,014*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,015*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 026,015*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 034,015*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 036,015*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 044,015*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 046,015*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,015*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,015*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,016*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 026,016*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 034,016*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 036,016*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 044,016*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 046,016*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,016*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,016*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,017*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 026,017*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 034,017*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 036,017*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 044,017*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 046,017*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,017*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,017*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,018*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 026,018*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 034,018*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 036,018*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 044,018*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 046,018*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,018*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,018*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,019*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 026,019*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 034,019*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 036,019*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 044,019*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 046,019*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,019*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,019*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,020*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 026,020*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 034,020*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 036,020*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 044,020*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 046,020*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,020*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,020*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,021*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 026,021*/ 8, 0x10, 
  /* RLE: 003 Pixels @ 034,021*/ 3, 0xFF, 
  /* RLE: 006 Pixels @ 037,021*/ 6, 0x10, 
  /* RLE: 003 Pixels @ 043,021*/ 3, 0xFF, 
  /* RLE: 008 Pixels @ 046,021*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,021*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,021*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,022*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 009 Pixels @ 026,022*/ 9, 0x10, 
  /* RLE: 003 Pixels @ 035,022*/ 3, 0xFF, 
  /* RLE: 004 Pixels @ 038,022*/ 4, 0x10, 
  /* RLE: 003 Pixels @ 042,022*/ 3, 0xFF, 
  /* RLE: 009 Pixels @ 045,022*/ 9, 0x10, 
  /* ABS: 002 Pixels @ 054,022*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,022*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,023*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 010 Pixels @ 026,023*/ 10, 0x10, 
  /* RLE: 003 Pixels @ 036,023*/ 3, 0xFF, 
  /* ABS: 005 Pixels @ 039,023*/ 0, 5, 0x10, 0x10, 0xFF, 0xFF, 0xFF, 
  /* RLE: 010 Pixels @ 044,023*/ 10, 0x10, 
  /* ABS: 002 Pixels @ 054,023*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,023*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,024*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 011 Pixels @ 026,024*/ 11, 0x10, 
  /* RLE: 006 Pixels @ 037,024*/ 6, 0xFF, 
  /* RLE: 011 Pixels @ 043,024*/ 11, 0x10, 
  /* ABS: 002 Pixels @ 054,024*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,024*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,025*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 012 Pixels @ 026,025*/ 12, 0x10, 
  /* RLE: 004 Pixels @ 038,025*/ 4, 0xFF, 
  /* RLE: 012 Pixels @ 042,025*/ 12, 0x10, 
  /* ABS: 002 Pixels @ 054,025*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,025*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,026*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 013 Pixels @ 026,026*/ 13, 0x10, 
  /* ABS: 002 Pixels @ 039,026*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 013 Pixels @ 041,026*/ 13, 0x10, 
  /* ABS: 002 Pixels @ 054,026*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,026*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,027*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 028 Pixels @ 026,027*/ 28, 0x10, 
  /* ABS: 002 Pixels @ 054,027*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,027*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,028*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 014 Pixels @ 026,028*/ 14, 0x10, 
  /* ABS: 002 Pixels @ 040,028*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 012 Pixels @ 042,028*/ 12, 0x10, 
  /* ABS: 002 Pixels @ 054,028*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,028*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,029*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 016 Pixels @ 026,029*/ 16, 0x10, 
  /* ABS: 002 Pixels @ 042,029*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 010 Pixels @ 044,029*/ 10, 0x10, 
  /* ABS: 002 Pixels @ 054,029*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,029*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,030*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 018 Pixels @ 026,030*/ 18, 0x10, 
  /* ABS: 002 Pixels @ 044,030*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 008 Pixels @ 046,030*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,030*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,030*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,031*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 020 Pixels @ 026,031*/ 20, 0x10, 
  /* RLE: 001 Pixels @ 046,031*/ 1, 0xFF, 
  /* RLE: 007 Pixels @ 047,031*/ 7, 0x10, 
  /* ABS: 002 Pixels @ 054,031*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,031*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,032*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 020 Pixels @ 026,032*/ 20, 0x10, 
  /* RLE: 001 Pixels @ 046,032*/ 1, 0xFF, 
  /* RLE: 007 Pixels @ 047,032*/ 7, 0x10, 
  /* ABS: 002 Pixels @ 054,032*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,032*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,033*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 019 Pixels @ 026,033*/ 19, 0x10, 
  /* RLE: 001 Pixels @ 045,033*/ 1, 0xFF, 
  /* RLE: 008 Pixels @ 046,033*/ 8, 0x10, 
  /* ABS: 002 Pixels @ 054,033*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,033*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,034*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 017 Pixels @ 026,034*/ 17, 0x10, 
  /* ABS: 002 Pixels @ 043,034*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 009 Pixels @ 045,034*/ 9, 0x10, 
  /* ABS: 002 Pixels @ 054,034*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,034*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,035*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 016 Pixels @ 026,035*/ 16, 0x10, 
  /* ABS: 002 Pixels @ 042,035*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 010 Pixels @ 044,035*/ 10, 0x10, 
  /* ABS: 002 Pixels @ 054,035*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,035*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,036*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 015 Pixels @ 026,036*/ 15, 0x10, 
  /* ABS: 002 Pixels @ 041,036*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 011 Pixels @ 043,036*/ 11, 0x10, 
  /* ABS: 002 Pixels @ 054,036*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,036*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,037*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 013 Pixels @ 026,037*/ 13, 0x10, 
  /* ABS: 002 Pixels @ 039,037*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 013 Pixels @ 041,037*/ 13, 0x10, 
  /* ABS: 002 Pixels @ 054,037*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,037*/ 48, 0x10, 
  /* ABS: 002 Pixels @ 024,038*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 028 Pixels @ 026,038*/ 28, 0x10, 
  /* ABS: 002 Pixels @ 054,038*/ 0, 2, 0xFF, 0xFF, 
  /* RLE: 048 Pixels @ 056,038*/ 48, 0x10, 
  /* RLE: 032 Pixels @ 024,039*/ 32, 0xFF, 
  /* RLE: 048 Pixels @ 056,039*/ 48, 0x10, 
  /* RLE: 032 Pixels @ 024,040*/ 32, 0xFF, 
  /* RLE: 744 Pixels @ 056,040*/ 254, 0x10, 254, 0x10, 236, 0x10, 


  0};  /* 577 for 4000 pixels */

GUI_CONST_STORAGE GUI_BITMAP bmbmp_extruct_sel = {
  80, /* XSize */
  50, /* YSize */
  80, /* BytesPerLine */
  GUI_COMPRESS_RLE8, /* BitsPerPixel */
  acbmp_extruct_sel,  /* Pointer to picture data (indices) */
  &Palbmp_extruct_sel  /* Pointer to palette */
 ,GUI_DRAW_RLE8
};

/* *** End of file *** */
