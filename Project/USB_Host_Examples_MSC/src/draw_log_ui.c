#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_log_ui.h"
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

GUI_HWIN hLogWnd;


extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static BUTTON_Handle button9600, button250000, button115200, buttonRet,button57600;

 void setUartBaud(int8_t baud_id);

static void cbLogWin(WM_MESSAGE * pMsg) {

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
					setUartBaud(gCfgItems.baud);
					Clear_Connect();
					draw_return_ui();
				}
				else if(pMsg->hWinSrc == button9600)
				{
					gCfgItems.baud= 1;
					bakup_cfg_inf(BAK_BAUD_ID,  gCfgItems.baud);
					disp_sel_baud();
				}
				else if(pMsg->hWinSrc == button57600)
				{
					gCfgItems.baud = 2;
					bakup_cfg_inf(BAK_BAUD_ID,  gCfgItems.baud);
					disp_sel_baud();
				}
				else if(pMsg->hWinSrc == button115200)
				{
					gCfgItems.baud = 3;
					bakup_cfg_inf(BAK_BAUD_ID,  gCfgItems.baud);
					disp_sel_baud();
				}
				else if(pMsg->hWinSrc == button250000)
				{
					gCfgItems.baud = 4;
					bakup_cfg_inf(BAK_BAUD_ID,  gCfgItems.baud);
					disp_sel_baud();
					
				}
					
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}


void draw_Connect()
{
		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	
	int i;

	
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != LOG_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = LOG_UI;
	}
	disp_state = LOG_UI;
		
	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
	//GUI_SetFont(&FONT_TITLE);
	if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		GUI_SetFont(&GUI_FontHZ16);
	}
	else
	{
		GUI_SetFont(&FONT_TITLE);
	}


	//GUI_DispStringAt("正在打印->设置->连接", 0, 0);
	GUI_DispStringAt(creat_title_text(), 0, 0);
	
	hLogWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbLogWin, 0);


	
	button9600 = BUTTON_CreateEx(0 , 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLogWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	button57600 = BUTTON_CreateEx(LCD_WIDTH / 4 , 0, LCD_WIDTH / 4 -2, imgHeight / 2 - 1, hLogWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	button115200 = BUTTON_CreateEx(LCD_WIDTH /2 , 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLogWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	button250000 = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLogWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLogWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	//TEXT_GetDefaultFont(&GUI_FontHZ_fontHz14);
	#if VERSION_WITH_PIC	
	
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);	
	
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);

	#endif
	disp_sel_baud();

	//TEXT_SetDefaultFont(&FONT_TITLE);
	
	//GUI_Exec();


	
}

void disp_sel_baud()
{
	if(gCfgItems.baud == 1)
	{
		#if VERSION_WITH_PIC	
		BUTTON_SetBmpFileName(button9600, "bmp_baud9600_sel.bin",0);	
		BUTTON_SetBmpFileName(button57600, "bmp_baud57600.bin",0);
		BUTTON_SetBmpFileName(button115200, "bmp_baud115200.bin",0);	
		BUTTON_SetBmpFileName(button250000, "bmp_baud250000.bin",0);	
		
		BUTTON_SetBitmapEx(button9600, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(button57600, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(button115200, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(button250000, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		#endif
	}
	else if(gCfgItems.baud == 2)
	{
		#if VERSION_WITH_PIC	
		BUTTON_SetBmpFileName(button9600, "bmp_baud9600.bin",0);	
		BUTTON_SetBmpFileName(button57600, "bmp_baud57600_sel.bin",0);	
		BUTTON_SetBmpFileName(button115200, "bmp_baud115200.bin",0);	
		BUTTON_SetBmpFileName(button250000, "bmp_baud250000.bin",0);

		BUTTON_SetBitmapEx(button57600, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(button115200, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(button9600, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(button250000, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		#endif
	}
	else if(gCfgItems.baud == 3)
	{
		#if VERSION_WITH_PIC	
		BUTTON_SetBmpFileName(button9600, "bmp_baud9600.bin",0);	
		BUTTON_SetBmpFileName(button57600, "bmp_baud57600.bin",0);
		BUTTON_SetBmpFileName(button115200, "bmp_baud115200_sel.bin",0);	
		BUTTON_SetBmpFileName(button250000, "bmp_baud250000.bin",0);

		BUTTON_SetBitmapEx(button57600, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(button250000, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(button115200, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(button9600, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		#endif
	}
	else
	{
		#if VERSION_WITH_PIC	
		BUTTON_SetBmpFileName(button9600, "bmp_baud9600.bin",0);	
		BUTTON_SetBmpFileName(button57600, "bmp_baud57600.bin",0);
		BUTTON_SetBmpFileName(button115200, "bmp_baud115200.bin",0);	
		BUTTON_SetBmpFileName(button250000, "bmp_baud250000_sel.bin",0);

		BUTTON_SetBitmapEx(button57600, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(button250000, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(button115200, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(button9600, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		#endif
	}
		
}

void Clear_Connect()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hLogWnd))
	{
		WM_DeleteWindow(hLogWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}


