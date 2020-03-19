#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_tool.h"
#include "draw_ui.h"
#include "draw_print_file.h"
#include "pic.h"
#include "fontLib.h"
#include "draw_move_motor.h"
#include "text.h"
#include "printer.h"
#include "draw_more.h"
#include "draw_leveling.h"
#include "draw_filamentchange.h"
#include "sd_usr.h"

#if SIMPLE_DISPLAY

#ifndef GUI_FLASH
#define GUI_FLASH
#endif

extern volatile unsigned char *codebufpoint;
extern char cmd_code[201];

extern uint8_t Get_Temperature_Flg;
extern volatile uint8_t get_temp_flag;

static BUTTON_Handle buttonPreHeat, buttonExtruse, buttonMove, buttonZero,buttonLeveling,buttonFilament,buttonMore,buttonRet;
static GUI_HWIN hToolWnd;
//static TEXT_Handle textPrintTemp;


//extern int8_t curSprayerTemp[10];
//extern int8_t curBedTemp[10];
extern uint8_t leveling_first_time;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;


static void cbDrawToolWin(WM_MESSAGE * pMsg) {
	
	switch (pMsg->MsgId) {
		case WM_PAINT:

			break;
		
		case WM_NOTIFY_PARENT:
			if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				if(pMsg->hWinSrc == buttonMove)
				{
					gCfgItems.getzpos_flg = 1;
					last_disp_state = TOOL_UI;
					Clear_Tool();
					draw_move_motor();			
				}
				else if(pMsg->hWinSrc == buttonPreHeat)
				{
					last_disp_state = TOOL_UI;
					Get_Temperature_Flg = 1;
					get_temp_flag = 1;
					Clear_Tool();
					draw_preHeat();
				}
				else if(pMsg->hWinSrc == buttonExtruse)
				{
					last_disp_state = TOOL_UI;
					Get_Temperature_Flg = 1;
					get_temp_flag = 1;
					Clear_Tool();
					draw_extrusion();
				}
				else if(pMsg->hWinSrc == buttonZero)
				{
					last_disp_state = TOOL_UI;
					Clear_Tool();
					draw_Zero();
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
						last_disp_state = TOOL_UI;
						Clear_Tool();
						draw_leveling();
					}
				}		
				else if(pMsg->hWinSrc == buttonFilament)
				{
					gCfgItems.desireSprayerTempBak_1[gCfgItems.curSprayerChoose] = gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose];
					last_disp_state = TOOL_UI;
					Clear_Tool();
					draw_FilamentChange();
				}		
				else if(pMsg->hWinSrc == buttonMore)
				{
					last_disp_state = TOOL_UI;
					Clear_Tool();
					draw_More();
				}				
				else if(pMsg->hWinSrc == buttonRet)
				{
					Clear_Tool();
					draw_return_ui();
				}
			}
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}





void draw_tool()
{
	int i;
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != TOOL_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = TOOL_UI;
	}
	disp_state = TOOL_UI;

	
	//GUI_SetFont(&GUI_Font8x18);
	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
	//GUI_SetFont(&FONT_TITLE);
	#if 0
	if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		GUI_SetFont(&GUI_FontHZ16);
	}
	else
	{
		GUI_SetFont(&FONT_TITLE);
	}
	#endif
	
	GUI_DispStringAt(creat_title_text(), 0, 0);
	

	hToolWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbDrawToolWin, 0);

	buttonPreHeat = BUTTON_CreateEx(0, 0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hToolWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonExtruse = BUTTON_CreateEx(LCD_WIDTH / 4, 0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hToolWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonMove = BUTTON_CreateEx(LCD_WIDTH / 2,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hToolWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonZero = BUTTON_CreateEx(LCD_WIDTH  * 3 / 4,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hToolWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	if(gCfgItems.leveling_mode!=2)
	{
		buttonLeveling = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hToolWnd, BUTTON_CF_SHOW, 0, 305);
		buttonFilament = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hToolWnd, BUTTON_CF_SHOW, 0, 306);
		buttonMore = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hToolWnd, BUTTON_CF_SHOW, 0, 307);	
	}
	else
	{
		buttonFilament = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hToolWnd, BUTTON_CF_SHOW, 0, 305);
		buttonMore = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hToolWnd, BUTTON_CF_SHOW, 0, 306);
	}
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hToolWnd, BUTTON_CF_SHOW, 0, 308);
	
#if VERSION_WITH_PIC	
	BUTTON_SetBmpFileName(buttonPreHeat, "bmp_preHeat.bin",0);
	BUTTON_SetBmpFileName(buttonExtruse, "bmp_extruct.bin",0);
	BUTTON_SetBmpFileName(buttonMove, "bmp_mov.bin",0);
	BUTTON_SetBmpFileName(buttonZero, "bmp_zero.bin",0);
	if(gCfgItems.leveling_mode != 2)
	{
		if(gCfgItems.leveling_mode == 1)
		{
			BUTTON_SetBmpFileName(buttonLeveling, "bmp_autoleveling.bin",1);
		}
		else
			BUTTON_SetBmpFileName(buttonLeveling, "bmp_leveling.bin",0);
	}
	
	BUTTON_SetBmpFileName(buttonFilament, "bmp_filamentchange.bin",0);	
	BUTTON_SetBmpFileName(buttonMore, "bmp_more.bin",0);	
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);
	
	BUTTON_SetBitmapEx(buttonPreHeat, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonExtruse, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
	BUTTON_SetBitmapEx(buttonMove, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
	BUTTON_SetBitmapEx(buttonZero, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
	if(gCfgItems.leveling_mode != 2)
	{
		BUTTON_SetBitmapEx(buttonLeveling, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
	}
	
	BUTTON_SetBitmapEx(buttonFilament, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
	BUTTON_SetBitmapEx(buttonMore, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
#endif
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetText(buttonPreHeat,main_menu.preheat);
		BUTTON_SetText(buttonExtruse,main_menu.extrude);
		BUTTON_SetText(buttonMove,main_menu.move);
		BUTTON_SetText(buttonZero,main_menu.home);
		if(gCfgItems.leveling_mode != 2)
		{
			if(gCfgItems.leveling_mode == 1)
				BUTTON_SetText(buttonLeveling,main_menu.autoleveling);
			else
				BUTTON_SetText(buttonLeveling,main_menu.leveling);
		}
		BUTTON_SetText(buttonFilament,set_menu.filament);
		BUTTON_SetText(buttonMore,main_menu.more);	
		BUTTON_SetText(buttonRet,common_menu.text_back);
	}

}

void Clear_Tool()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hToolWnd))
	{
		WM_DeleteWindow(hToolWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}
#endif
