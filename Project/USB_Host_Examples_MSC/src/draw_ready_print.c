#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_ready_print.h"
#include "draw_ui.h"
#include "draw_print_file.h"
#include "pic.h"
#include "fontLib.h"
#include "draw_move_motor.h"
#include "text.h"
#include "printer.h"
#include "sd_usr.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif


extern volatile unsigned char *codebufpoint;
extern char cmd_code[201];


extern uint8_t leveling_first_time;

extern uint32_t iconlogo_addroffset;

extern uint8_t Get_Temperature_Flg;
extern volatile uint8_t get_temp_flag;

static BUTTON_Handle buttonTool, buttonPreHeat, buttonPrint, buttonExtruse, buttonMove, buttonLeveling, buttonMore, buttonZero, buttonSet;

static GUI_HWIN hReadyPrintWnd;
static TEXT_Handle textPrintTemp;


extern int8_t curSprayerTemp[10];
extern int8_t curBedTemp[10];
	

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;


static void cbReadPrintWin(WM_MESSAGE * pMsg) {
	
	switch (pMsg->MsgId) {
		case WM_PAINT:
			#if 0
			GUI_SetColor(GUI_WHITE);
			GUI_DrawRect(LCD_WIDTH/4 + 1, 1, LCD_WIDTH *3 / 4 -3, imgHeight /2 - 3);
			GUI_SetColor(GUI_STATE_COLOR);
			GUI_FillRect(LCD_WIDTH/4 + 2, 2, LCD_WIDTH *3 / 4 -4, imgHeight /2 - 4);
			#endif
			break;
		
		  
		case WM_NOTIFY_PARENT:
			if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				if(pMsg->hWinSrc == buttonPrint)
				{
					last_disp_state = PRINT_READY_UI;
					Clear_ready_print();
					draw_print_file();
				}
				else if(pMsg->hWinSrc == buttonMove)
				{
					gCfgItems.getzpos_flg = 1;
					last_disp_state = PRINT_READY_UI;
					Clear_ready_print();
					draw_move_motor();			
				}
				else if(pMsg->hWinSrc == buttonPreHeat)
				{
					last_disp_state = PRINT_READY_UI;
					Get_Temperature_Flg = 1;
					get_temp_flag = 1;
					Clear_ready_print();
					draw_preHeat();
				}
				else if(pMsg->hWinSrc == buttonExtruse)
				{
					last_disp_state = PRINT_READY_UI;
					Get_Temperature_Flg = 1;
					get_temp_flag = 1;
					Clear_ready_print();
					draw_extrusion();
				}
				else if(pMsg->hWinSrc == buttonZero)
				{
					last_disp_state = PRINT_READY_UI;
					Clear_ready_print();
					draw_Zero();
				}
				else if(pMsg->hWinSrc == buttonSet)
				{
					last_disp_state = PRINT_READY_UI;
					Clear_ready_print();
					draw_Set();
				}
				else if(pMsg->hWinSrc == buttonMore)
				{
					last_disp_state = PRINT_READY_UI;
					Clear_ready_print();
					draw_More();
				}
				else if(pMsg->hWinSrc == buttonLeveling)
				{
					if(gCfgItems.leveling_mode == 1)
					{
						SPI_FLASH_BufferRead(cmd_code,BUTTON_AUTOLEVELING_ADDR,201);
						codebufpoint = cmd_code;								
					}
					else
					{
						leveling_first_time = 1;
						last_disp_state = PRINT_READY_UI;
						Clear_ready_print();
						draw_leveling();
					}
				}
				#if SIMPLE_DISPLAY
				else if(pMsg->hWinSrc == buttonTool)
				{
					last_disp_state = PRINT_READY_UI;
					Clear_ready_print();
					draw_tool();
				}
				#endif
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}



void disp_sprayer_temp_main()
{
	int8_t buf[30] = {0};
	volatile uint8_t i0 = 0;
	
	if(gCfgItems.sprayerNum == 2)
	{
		GUI_SetBkColor(gCfgItems.printingstate_word_background_color);
		/*
		for(i0 = 0;i0<7;i0++)
		{
			buf[i0] = ' '; 
		}
		GUI_DispStringAt(buf, 15, 110);
		*/
		memset(buf, ' ', sizeof(buf));
		GUI_DispStringAt(buf, 15, 110);
		sprintf(buf, "%3d/%d", (int)gCfgItems.curSprayerTemp[0], (uint16_t)gCfgItems.desireSprayerTemp[0]);
		GUI_DispStringAt(buf, 15, 110);	
		/*
		for(i0 = 0;i0<7;i0++)
		{
			buf[i0] = ' '; 
		}
		GUI_DispStringAt(buf, 15+78, 110);	
		*/
		//HC-chen
		//不支持两进一出
		if(gCfgItems.single_nozzle==0)
		{
			memset(buf,' ',sizeof(buf));
			GUI_DispStringAt(buf, 15+78, 110);
			sprintf(buf, "%3d/%d", (int)gCfgItems.curSprayerTemp[1], (uint16_t)gCfgItems.desireSprayerTemp[1]);
			GUI_DispStringAt(buf, 15+78, 110);	
		}			
	}
	else
	{
		GUI_SetBkColor(gCfgItems.printingstate_word_background_color);
		/*
		for(i0 = 0;i0<7;i0++)
		{
			buf[i0] = ' '; 
		}
		GUI_DispStringAt(buf, 10, 110);	
		*/
		memset(buf, ' ', sizeof(buf));
		GUI_DispStringAt(buf, 10, 110);
		sprintf(buf, "%3d/%d", (int)gCfgItems.curSprayerTemp[0], (uint16_t)gCfgItems.desireSprayerTemp[0]);
		GUI_DispStringAt(buf, 10, 110);
	}
}


void disp_bed_temp_main()
{
	int8_t buf[30] = {0};
	//volatile uint8_t i1 = 0;	
	
	if(gCfgItems.custom_bed_flag == 1)
	{
	/*
		for(i1 = 0;i1<7;i1++)
		{
			buf[i1] = ' '; 
		}
		GUI_DispStringAt(buf, 170, 110);	
		*/
		memset(buf, ' ', sizeof(buf));
		GUI_DispStringAt(buf, 170, 110);	
		sprintf(buf, "%3d/%d", (int)gCfgItems.curBedTemp, (uint16_t)gCfgItems.desireBedTemp);
		GUI_DispStringAt(buf, 170, 110);
	}
	//TEXT_SetText(BedTem, buf);
}
void disp_fan_speed_main()
{
	int8_t buf[30] = {0};
	volatile uint8_t i2 = 0;
	/*
	for(i2 = 0;i2<5;i2++)
	{
		buf[i2] = ' '; 
	}
	GUI_DispStringAt(buf, 255, 110);
	*/
	memset(buf,' ', sizeof(buf));
	GUI_DispStringAt(buf, 255, 110);
	sprintf(buf, "%3d", gCfgItems.fanSpeed);
	GUI_DispStringAt(buf, 255, 110);

}
static int8_t fan_dir = 0;

void disp_fan_move_main()
{
	if(gCfgItems.fanOnoff == 1)
	{
		fan_dir++;
		if(fan_dir%2)
		{
			Pic_Read("bmp_fan_move.bin",bmp_public_buf);
			GUI_DrawBitmap(&bmp_struct,  239, titleHeight);
		}
		else
		{
			Pic_Read("bmp_fan_no_words.bin",bmp_public_buf);
			GUI_DrawBitmap(&bmp_struct,  239, titleHeight);
		}
	}
	disp_fan_speed_main();
}


void draw_ready_print()
{
	int index; 
	int x_off = 0, y_off = 0;
	int _x, _y;
	uint16_t *p_index;
	int i;

	disp_state_stack._disp_index = 0;
	memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
	disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINT_READY_UI;
	
	disp_state = PRINT_READY_UI;
	
	//GUI_SetFont(&GUI_Font8x18);
	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
	#if 0
	//GUI_SetFont(&FONT_TITLE);
	if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		GUI_SetFont(&GUI_FontHZ16);
	}
	else
	{
		GUI_SetFont(&FONT_TITLE);
	}
	
	
	GUI_SetFont(&FONT_TITLE);
#endif

#if (SIMPLE_DISPLAY == 0)
	GUI_DispStringAt(creat_title_text(), 0, 0);
	hReadyPrintWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbReadPrintWin, 0);

	buttonPreHeat = BUTTON_CreateEx(0, 0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonMove = BUTTON_CreateEx(LCD_WIDTH / 4, 0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonZero = BUTTON_CreateEx(LCD_WIDTH / 2,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonPrint = BUTTON_CreateEx(LCD_WIDTH  * 3 / 4,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonExtruse = BUTTON_CreateEx(0,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	if(gCfgItems.leveling_mode!=2)
	{
		buttonLeveling = BUTTON_CreateEx(LCD_WIDTH / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		buttonSet = BUTTON_CreateEx(LCD_WIDTH / 2,	imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		buttonMore = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	
	}
	else
	{
		buttonSet = BUTTON_CreateEx(LCD_WIDTH / 4,	imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());	
		buttonMore = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	
	}
	
#if VERSION_WITH_PIC	
	BUTTON_SetBmpFileName(buttonPreHeat, "bmp_preHeat.bin",0);	
	BUTTON_SetBmpFileName(buttonPrint, "bmp_printing.bin",0);
	BUTTON_SetBmpFileName(buttonMove, "bmp_mov.bin",0);
	
	BUTTON_SetBmpFileName(buttonZero, "bmp_zero.bin",0);
	if(gCfgItems.leveling_mode!=2)
	{
		if(gCfgItems.leveling_mode == 1)
		{
			BUTTON_SetBmpFileName(buttonLeveling, "bmp_autoleveling.bin",1);
		}
		else
		{
			BUTTON_SetBmpFileName(buttonLeveling, "bmp_leveling.bin",0);
		}
	}
	BUTTON_SetBmpFileName(buttonMore, "bmp_more.bin",0);
	BUTTON_SetBmpFileName(buttonExtruse, "bmp_extruct.bin",0);
	BUTTON_SetBmpFileName(buttonSet, "bmp_set.bin",0);

	
	BUTTON_SetBitmapEx(buttonPreHeat, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonPrint, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
	BUTTON_SetBitmapEx(buttonMove, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
	BUTTON_SetBitmapEx(buttonMore, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
	BUTTON_SetBitmapEx(buttonZero, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
	if(gCfgItems.leveling_mode != 2)
	{
		BUTTON_SetBitmapEx(buttonLeveling, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
	}
	BUTTON_SetBitmapEx(buttonExtruse, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
	BUTTON_SetBitmapEx(buttonSet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);

#endif

		if(gCfgItems.multiple_language != 0)
		{
			BUTTON_SetText(buttonPreHeat,main_menu.preheat);
			BUTTON_SetText(buttonMove,main_menu.move);
			BUTTON_SetText(buttonZero,main_menu.home);
			BUTTON_SetText(buttonPrint,main_menu.print);
			BUTTON_SetText(buttonExtruse,main_menu.extrude);
			if(gCfgItems.leveling_mode != 2)
			{
				if(gCfgItems.leveling_mode == 1)
					BUTTON_SetText(buttonLeveling,main_menu.autoleveling);
				else
					BUTTON_SetText(buttonLeveling,main_menu.leveling);
			}
			
			BUTTON_SetText(buttonSet,main_menu.set);
			BUTTON_SetText(buttonMore,main_menu.more);
	
		}

#else
hReadyPrintWnd = WM_CreateWindow(0, 135, LCD_WIDTH, 105, WM_CF_SHOW, cbReadPrintWin, 0);

buttonTool= BUTTON_CreateEx(38, 0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
buttonSet = BUTTON_CreateEx(121, 0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
buttonPrint= BUTTON_CreateEx(204,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hReadyPrintWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

BUTTON_SetBmpFileName(buttonTool, "bmp_tool.bin",0);
BUTTON_SetBmpFileName(buttonSet, "bmp_set.bin",0);
BUTTON_SetBmpFileName(buttonPrint, "bmp_printing.bin",0);

BUTTON_SetBitmapEx(buttonTool, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
BUTTON_SetBitmapEx(buttonPrint, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y); 
BUTTON_SetBitmapEx(buttonSet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);

if(gCfgItems.multiple_language != 0)
{
	BUTTON_SetText(buttonTool,main_menu.tool);
	BUTTON_SetText(buttonSet,main_menu.set);
	BUTTON_SetText(buttonPrint,main_menu.print);
}

#if(SIMPLE_DISPLAY == 1)
iconlogo_addroffset = 0;
for(index = 0; index < 9; index ++)
{
	Icon_Logo_Read("bmp_iconlogo.bin", bmp_public_buf, 9600); 
	i = 0;
	LCD_setWindowArea(0, y_off * 15, 320, 15);
	LCD_SetCursor(0, y_off * 15);
	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */ 
	for(_y = y_off * 15; _y < (y_off + 1) * 15; _y++)
	{
		for (x_off = 0; x_off < 320; x_off++) 
		{
			p_index = (uint16_t *)(&bmp_public_buf[i]); 					
			LCD_WriteRAM(*p_index);
			i += 2;
		}
		if(i >= 9600)
				break;
	}
	y_off++;		
}
LCD_setWindowArea(0, 0, 319, 135);

#else(SIMPLE_DISPLAY == 2)
Pic_Read("bmp_extru1_no_words.bin",bmp_public_buf);
GUI_DrawBitmap(&bmp_struct,  5, titleHeight);
if(gCfgItems.sprayerNum == 2)
{
	//HC-chen
	//不支持两进一出
	if(gCfgItems.single_nozzle==0)
	{
		Pic_Read("bmp_extru2_no_words.bin",bmp_public_buf);
		GUI_DrawBitmap(&bmp_struct,  83, titleHeight);	
	}
}
if(gCfgItems.custom_bed_flag == 1)
{
	Pic_Read("bmp_bed_no_words.bin",bmp_public_buf);
	GUI_DrawBitmap(&bmp_struct,  161, titleHeight);
}
Pic_Read("bmp_fan_no_words.bin",bmp_public_buf);
GUI_DrawBitmap(&bmp_struct,  239, titleHeight);

Get_Temperature_Flg = 1;
get_temp_flag = 1;

disp_sprayer_temp_main();
disp_bed_temp_main();
disp_fan_move_main();

#endif
#endif	
}

void Clear_ready_print()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hReadyPrintWnd))
	{
		WM_DeleteWindow(hReadyPrintWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}

//static int8_t fan_dir = 0;
/*void disp_fan_move()
{
	if(gCfgItems.fanOnoff == 1)
	{
		fan_dir++;
		if(fan_dir%2)
		{
			BUTTON_SetBitmapEx(buttonFan, 0, &bmbmp_fan_move,BMP_PIC_X, BMP_PIC_Y);
		}
		else
		{
			BUTTON_SetBitmapEx(buttonFan, 0, &bmbmp_fan,BMP_PIC_X, BMP_PIC_Y);
		}
	}
}*/

#if 0
void disp_temp_ready_print()
{
	int8_t buf[30] = {0};
	
	TEXT_SetTextColor(textPrintTemp, GUI_WHITE);
	TEXT_SetDefaultFont(&FONT_STATE_INF);
	TEXT_SetBkColor(textPrintTemp, GUI_STATE_COLOR);

	if(gCfgItems.sprayerNum == 2)
	{
		sprintf(buf, "%.1f   %.1f  %.1f  %d", gCfgItems.curSprayerTemp[0], gCfgItems.curSprayerTemp[1], gCfgItems.curBedTemp, gCfgItems.fanSpeed);
	
	}
	else
	{
		sprintf(buf, "%.1f  %.1f  %d", gCfgItems.curSprayerTemp[0], gCfgItems.curBedTemp, gCfgItems.fanSpeed);
	
	}
	TEXT_SetText(textPrintTemp, buf);
}
#endif
