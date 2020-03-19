#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_about.h"
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

extern uint16_t readLcdId;

static GUI_HWIN hAboutWnd;


extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;
extern GUI_CONST_STORAGE GUI_BITMAP bmp_logo;

extern char wifi_firm_ver[20];

static BUTTON_Handle buttonRet;

static void cbAboutWin(WM_MESSAGE * pMsg) {

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
					
					Clear_About();
					draw_return_ui();
				}
				
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}

#define ESP_WIFI		0x02

void draw_About()
{
		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	
	int i;

	
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != ABOUT_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = ABOUT_UI;
	}
	disp_state = ABOUT_UI;
/*
	GUI_SetBkColor(gCfgItems.state_background_color);
	GUI_SetColor(gCfgItems.state_text_color);
	GUI_Clear();
*/
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
#endif
	/*
	if(readLcdId==0x8989)
	{
		GUI_DispStringAt("TYPE: MKS TFT32", 40, titleHeight  + 75);
	}
	else
	{
		GUI_DispStringAt("TYPE: MKS TFT28", 40, titleHeight  + 75);
	}
	*/
	//GUI_SetFont(&FONT_TITLE);

	
	//GUI_FillRect(0, titleHeight, LCD_WIDTH - 1, imgHeight - 1);	
	//GUI_DispStringAt("MAKER BASE", LCD_WIDTH / 3, LCD_HEIGHT / 4);

	#if VERSION_WITH_PIC	
	//GUI_DrawBitmap(&bmp_logo,  0, 10);
	#endif
	
	//GUI_DrawRect(1, titleHeight + 1, LCD_WIDTH  - 3, LCD_HEIGHT - 3);

	//GUI_SetColor(gCfgItems.title_color);

	GUI_DispStringAt(creat_title_text(), 0, 0);
	
	hAboutWnd = WM_CreateWindow(LCD_WIDTH * 3 / 4 , titleHeight + imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, WM_CF_SHOW, cbAboutWin, 0);

	buttonRet = BUTTON_CreateEx(0 , 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hAboutWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
#if 0
	GUI_DispStringAt("TYPE: MKS TFT", 40, titleHeight  + 50);
	GUI_DispStringAt("VERSION: 2.0.1", 40, titleHeight + 75);

	if(gCfgItems.wifi_type == ESP_WIFI)
	{
		GUI_DispStringAt("WIFI: ", 40, titleHeight + 100); 
		if(wifi_firm_ver[0] == 'T')
			GUI_DispStringAt((const char *)wifi_firm_ver, 90, titleHeight + 100); 
	}
	#endif
	#if VERSION_WITH_PIC	
	
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);	
	
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);

	#endif
	
	//GUI_Exec();


			GUI_DispStringAt(about_menu.type, 40, titleHeight  + 50);
			GUI_DispStringAt(about_menu.version, 40, titleHeight + 75);	
			if(gCfgItems.wifi_type == ESP_WIFI)
			{
				GUI_DispStringAt(about_menu.wifi, 40, titleHeight + 100);
				if(wifi_firm_ver[0] != 0)
					GUI_DispStringAt((const char *)wifi_firm_ver, 90, titleHeight + 100); 
			}
			if(gCfgItems.multiple_language != 0)
			{
				BUTTON_SetText(buttonRet,common_menu.text_back);
			}
			
	
}

void Clear_About()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hAboutWnd))
	{
		WM_DeleteWindow(hAboutWnd);
		GUI_Exec();
	}
	//GUI_Clear();
}


