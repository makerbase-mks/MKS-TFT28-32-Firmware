#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_zero.h"
#include "draw_ui.h"
#include "pic.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "usbh_usr.h"
#include "printer.h"
#include "printing_thread.h"
#include "gcode.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif

static GUI_HWIN hZeroWnd;


extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static BUTTON_Handle buttonAllZero, buttonXZero, buttonYZero, buttonZZero, buttonRet,M_off_Button;

static void cbZeroWin(WM_MESSAGE * pMsg) {

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
					Clear_Zero();
					draw_return_ui();

					
				}
				else if(pMsg->hWinSrc == buttonAllZero)
				{
				//if(printer_state >= PRINTER_CONNECT_OK)
					{						
						////////printf(HOME_A_COMMAND);
						pushFIFO(&gcodeCmdTxFIFO, HOME_A_COMMAND);
					}					
				}
				else if(pMsg->hWinSrc == buttonXZero)
				{
				//if(printer_state >= PRINTER_CONNECT_OK)
					{						
						/////printf(HOME_X_COMMAND);
						pushFIFO(&gcodeCmdTxFIFO, HOME_X_COMMAND);
					}					
				}
				else if(pMsg->hWinSrc == buttonYZero)
				{
				//if(printer_state >= PRINTER_CONNECT_OK)
					{						
						//////printf(HOME_Y_COMMAND);
						pushFIFO(&gcodeCmdTxFIFO, HOME_Y_COMMAND);
					}					
				}
				else if(pMsg->hWinSrc == buttonZZero)
				{
				//if(printer_state >= PRINTER_CONNECT_OK)
					{						
						///////printf(HOME_Z_COMMAND);
						pushFIFO(&gcodeCmdTxFIFO, HOME_Z_COMMAND);
					}					
				}
				//HC-chen
				#if SIMPLE_DISPLAY==0
				else if(pMsg->hWinSrc == M_off_Button)
				{
					pushFIFO(&gcodeCmdTxFIFO, M_OFF_COMMAND);				
				}
				#endif
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}


void draw_Zero()
{
		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != ZERO_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = ZERO_UI;
	}
	disp_state = ZERO_UI;
		
	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
	//GUI_SetFont(&FONT_TITLE);
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

	//GUI_DispStringAt("准备打印->回零", 0, 0);
	GUI_DispStringAt(creat_title_text(), 0, 0);
	
	hZeroWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbZeroWin, 0);

/*	
	buttonAllZero = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hZeroWnd, BUTTON_CF_SHOW, 0, 301);
	buttonXZero = BUTTON_CreateEx(LCD_WIDTH  / 4,  0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hZeroWnd, BUTTON_CF_SHOW, 0, 302);
	buttonYZero = BUTTON_CreateEx(LCD_WIDTH /2 , 0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hZeroWnd, BUTTON_CF_SHOW, 0, 303);
	buttonZZero = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 ,0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hZeroWnd, BUTTON_CF_SHOW, 0, 304);
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hZeroWnd, BUTTON_CF_SHOW, 0, 308);
*/
	buttonAllZero = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hZeroWnd, BUTTON_CF_SHOW, 0, 301);
	buttonXZero = BUTTON_CreateEx(LCD_WIDTH  / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hZeroWnd, BUTTON_CF_SHOW, 0, 302);
	buttonYZero = BUTTON_CreateEx(LCD_WIDTH /2 , 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hZeroWnd, BUTTON_CF_SHOW, 0, 303);
	buttonZZero = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 ,0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hZeroWnd, BUTTON_CF_SHOW, 0, 304);
	#if SIMPLE_DISPLAY==0
	M_off_Button=BUTTON_CreateEx(0,imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hZeroWnd, BUTTON_CF_SHOW, 0, 305);
	#endif
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hZeroWnd, BUTTON_CF_SHOW, 0, 308);

	//TEXT_SetDefaultFont(&FONT_STATE_INF);	

	#if VERSION_WITH_PIC	
	BUTTON_SetBmpFileName(buttonAllZero, "bmp_zeroA.bin",1);
	BUTTON_SetBmpFileName(buttonXZero, "bmp_zeroX.bin",1);
	BUTTON_SetBmpFileName(buttonYZero, "bmp_zeroY.bin",1);
	BUTTON_SetBmpFileName(buttonZZero, "bmp_zeroZ.bin",1);
	#if SIMPLE_DISPLAY==0
	BUTTON_SetBmpFileName(M_off_Button, "bmp_function1.bin",1);
	#endif
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);
	
	BUTTON_SetBitmapEx(buttonAllZero, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonXZero, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonYZero, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonZZero, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	#if SIMPLE_DISPLAY==0
	BUTTON_SetBitmapEx(M_off_Button, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	#endif
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	#endif
	
	//GUI_Exec();
	if(gCfgItems.multiple_language != 0)
	{
			BUTTON_SetText(buttonXZero,home_menu.home_x);
			BUTTON_SetText(buttonYZero,home_menu.home_y);
			BUTTON_SetText(buttonZZero,home_menu.home_z);
			BUTTON_SetText(buttonAllZero,home_menu.home_all);
			#if SIMPLE_DISPLAY==0
			BUTTON_SetText(M_off_Button,set_menu.motoroff);
			#endif
			BUTTON_SetText(buttonRet,common_menu.text_back);
	}
	
}

void Clear_Zero()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hZeroWnd))
	{
		WM_DeleteWindow(hZeroWnd);
		GUI_Exec();
	}
	//GUI_Clear();
}


