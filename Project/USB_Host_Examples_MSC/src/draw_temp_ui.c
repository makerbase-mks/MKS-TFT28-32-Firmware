#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_temp_ui.h"
#include "draw_ui.h"
#include "pic.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "usbh_usr.h"
#include "printer.h"
#include "printing_thread.h"
#include "draw_set.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif

static GUI_HWIN hTempWnd;


extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static BUTTON_Handle buttonInc, buttonDec, buttonSprayerTemp, buttonHeatbedTemp, buttonFanOpen, buttonRet;

static void cbTempWin(WM_MESSAGE * pMsg) {

	struct PressEvt *press_event;
	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			//GUI_SetBkColor(GUI_BLUE);
			//GUI_Clear();
			//GUI_DispString("window");
			break;
		case WM_TOUCH:
		 	press_event = (struct PressEvt *)pMsg->Data.p;
			
			break;
		case WM_TOUCH_CHILD:
			press_event = (struct PressEvt *)pMsg->Data.p;

			break;
			
		case WM_NOTIFY_PARENT:
			if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				if(pMsg->hWinSrc == buttonRet)
				{
					Clear_TempSpeed();
					draw_return_ui();
				}
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}


void draw_temp()
{
		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != TEMP_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = TEMP_UI;
	}
	disp_state = TEMP_UI;
		
	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
	GUI_SetFont(&FONT_TITLE);
	
	GUI_DispStringAt("准备打印:设置:温度", 0, 0);
	
	hTempWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbTempWin, 0);

	
	buttonInc = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hTempWnd, BUTTON_CF_SHOW, 0, 301);
	buttonDec = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hTempWnd, BUTTON_CF_SHOW, 0, 302);
	buttonSprayerTemp = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hTempWnd, BUTTON_CF_SHOW, 0, 303);
	buttonHeatbedTemp = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hTempWnd, BUTTON_CF_SHOW, 0, 304);
	buttonFanOpen = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hTempWnd, BUTTON_CF_SHOW, 0, 305);
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hTempWnd, BUTTON_CF_SHOW, 0, 306);
	TEXT_SetDefaultFont(const GUI_FONT GUI_UNI_PTR * pFont)(&GUI_FontHZ_fontHz18);
	

	if(gCfgItems.custom_pic_flag != 1)
	{
		BUTTON_SetText(buttonInc, "增加");
		BUTTON_SetText(buttonDec, "减少");
		BUTTON_SetText(buttonSprayerTemp, "喷头\n温度");
		BUTTON_SetText(buttonHeatbedTemp, "hot\nbed");
		BUTTON_SetText(buttonFanOpen, "风扇开");
		BUTTON_SetText(buttonRet, "返回");


		BUTTON_SetBkColor(buttonRet, BUTTON_CI_UNPRESSED, GUI_RET_BUTTON_COLOR);
		BUTTON_SetBkColor(buttonRet, BUTTON_CI_PRESSED, GUI_RET_BUTTON_COLOR);
	}	
	//GUI_Exec();


	
}

void Clear_TempSpeed()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hTempWnd))
	{
		WM_DeleteWindow(hTempWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}


