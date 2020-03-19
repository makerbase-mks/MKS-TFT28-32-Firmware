#if 0
#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_Sprayer.h"
#include "draw_ui.h"
#include "pic.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "usbh_usr.h"
#include "printer.h"
#include "printing_thread.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif

static GUI_HWIN hSprayerWnd;


extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static BUTTON_Handle buttonSingle, buttonDouble, buttonRet;

static void cbSprayerWin(WM_MESSAGE * pMsg) {

	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			//GUI_SetBkColor(GUI_BLUE);
			//GUI_Clear();
			//GUI_DispString("window");
			break;
		case WM_TOUCH:
		 	
			break;
		case WM_TOUCH_CHILD:
			
			break;
			
		case WM_NOTIFY_PARENT:
			if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				if(pMsg->hWinSrc == buttonRet)
				{
					Clear_Sprayer();
					draw_return_ui();
				}
				else if(pMsg->hWinSrc == buttonSingle)
				{
					gCfgItems.sprayerNum = 1;
					disp_sel_sprayer_num();
				}
				else if(pMsg->hWinSrc == buttonDouble)
				{
					gCfgItems.sprayerNum = 2;
					disp_sel_sprayer_num();
				}
				
			
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}


void draw_Sprayer()
{
		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != SPRAYER_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = SPRAYER_UI;
	}
	disp_state = SPRAYER_UI;
		
	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
	GUI_SetFont(&FONT_TITLE);


	//GUI_DispStringAt("准备打印->设置->喷头数", 0, 0);
	GUI_DispStringAt(creat_title_text(), 0, 0);
	
	hSprayerWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbSprayerWin, 0);

	
	buttonSingle = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hSprayerWnd, BUTTON_CF_SHOW, 0, 301);
	buttonDouble = BUTTON_CreateEx(LCD_WIDTH  / 4,  0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hSprayerWnd, BUTTON_CF_SHOW, 0, 302);
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hSprayerWnd, BUTTON_CF_SHOW, 0, 308);
	TEXT_SetDefaultFont(&FONT_STATE_INF);	

	if(gCfgItems.custom_pic_flag != 1)
	{
		if(gCfgItems.language == LANG_ENGLISH)
		{
			BUTTON_SetText(buttonSingle, "Single");
			BUTTON_SetText(buttonDouble, "Double");
			BUTTON_SetText(buttonRet, "Back");
		}
		else
		{
			BUTTON_SetText(buttonSingle, "单喷头");
			BUTTON_SetText(buttonDouble, "双喷头");
			BUTTON_SetText(buttonRet, "返回");
		}


		BUTTON_SetBkColor(buttonRet, BUTTON_CI_UNPRESSED, GUI_RET_BUTTON_COLOR);
		BUTTON_SetBkColor(buttonRet, BUTTON_CI_PRESSED, GUI_RET_BUTTON_COLOR);
	}

	disp_sel_sprayer_num();					
	
	//GUI_Exec();


	
}

void disp_sel_sprayer_num()
{
	if(gCfgItems.sprayerNum == 2)
	{
		BUTTON_SetBkColor(buttonDouble, BUTTON_CI_UNPRESSED, GUI_FOCUS_CLOLOR);
		BUTTON_SetBkColor(buttonSingle, BUTTON_CI_UNPRESSED, GUI_BLUE);
	}
	else
	{
		BUTTON_SetBkColor(buttonSingle, BUTTON_CI_UNPRESSED, GUI_FOCUS_CLOLOR);
		BUTTON_SetBkColor(buttonDouble, BUTTON_CI_UNPRESSED, GUI_BLUE);
	}
}
	

void Clear_Sprayer()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hSprayerWnd))
	{
		WM_DeleteWindow(hSprayerWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}

#endif
