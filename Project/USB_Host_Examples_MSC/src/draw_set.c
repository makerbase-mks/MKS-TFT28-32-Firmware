#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_set.h"
#include "draw_ui.h"
#include "pic.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "usbh_usr.h"
#include "printer.h"
#include "printing_thread.h"
#include "sd_usr.h"


#ifndef GUI_FLASH
#define GUI_FLASH
#endif

static GUI_HWIN hSetWnd;

extern volatile uint8_t get_temp_flag;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

extern volatile unsigned char *codebufpoint;
extern char cmd_code[201];


	
static BUTTON_Handle buttonDisk, buttonVarify, buttonMachine, buttonFan, buttonWifi, buttonBreakpoint, buttonAbout,buttonFunction_1, buttonFunction_2,buttonFunction_3,buttonRet,buttonFilamentChange;
static BUTTON_Handle buttonLanguage;

static void cbSetWin(WM_MESSAGE * pMsg) {

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
					Clear_Set();
					draw_return_ui();
					
				}
				else if(pMsg->hWinSrc == buttonDisk)
				{
					last_disp_state = SET_UI;
					Clear_Set();
					draw_Disk();
				}
				#if 0
				else if(pMsg->hWinSrc == buttonVarify)
				{
					last_disp_state = SET_UI;
					Clear_Set();
					draw_calibrate();
				}
				#endif
				else if(pMsg->hWinSrc == buttonLanguage)
				{
					last_disp_state = SET_UI;
					Clear_Set();
					draw_Language();
				}
				
				else if(pMsg->hWinSrc == buttonFan)
				{
					last_disp_state = SET_UI;
					Clear_Set();
					draw_fan();
				}
				
				else if(pMsg->hWinSrc == buttonBreakpoint)
				{
					last_disp_state = SET_UI;
					Clear_Set();
					gCfgItems.pwd_reprint_flg = 1;
					draw_print_file();
				}
				
				else if(pMsg->hWinSrc == buttonAbout)
				{
					last_disp_state = SET_UI;
					Clear_Set();
					draw_About();
				}
				else if(pMsg->hWinSrc == buttonWifi)
				{
					last_disp_state = SET_UI;
					Clear_Set();
					draw_Wifi();
				}
				#if SIMPLE_DISPLAY
				else if(pMsg->hWinSrc == buttonFunction_1)
				{
					SPI_FLASH_BufferRead(cmd_code,BUTTON_FUNCTION1_ADDR,201);
					codebufpoint = cmd_code;
				}
				//else if(pMsg->hWinSrc == buttonFunction_2)
				//{

				//	SPI_FLASH_BufferRead(cmd_code,BUTTON_FUNCTION2_ADDR,201);
				//	codebufpoint = cmd_code;
				//}

				#else
				else if(pMsg->hWinSrc == buttonFilamentChange)
				{
					//gCfgItems.desireSprayerTempBak_1[gCfgItems.curSprayerChoose] = gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose];
					last_disp_state = SET_UI;
					Get_Temperature_Flg = 1;
					get_temp_flag = 1;
					Clear_Set();
					draw_FilamentChange();
				}
				#if defined (STM32F10X_HD)
				else if(pMsg->hWinSrc == buttonFunction_1)
				{
					SPI_FLASH_BufferRead(cmd_code,BUTTON_FUNCTION1_ADDR,201);
					codebufpoint = cmd_code;
				}
				#endif
				#endif
				
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}


void draw_Set()
{
		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != SET_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = SET_UI;
	}
	disp_state = SET_UI;
	GUI_Clear();

	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
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
		
	
	hSetWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbSetWin, 0);
#if defined (STM32F10X_CL)
	buttonDisk = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 301);
	//buttonConnect = BUTTON_CreateEx(LCD_WIDTH  / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 302);
	buttonWifi = BUTTON_CreateEx(LCD_WIDTH  / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 302);
	buttonFan = BUTTON_CreateEx(LCD_WIDTH /2 , 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 303);
	buttonAbout = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 304);
	buttonBreakpoint = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 305);
	#if SIMPLE_DISPLAY
	if(gCfgItems.multiple_language != 0)
		buttonLanguage = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hSetWnd, BUTTON_CF_SHOW, 0, 308); 			

	if(gCfgItems.func_btn1_display_flag == 1)
	{
		buttonFunction_1 = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 306);
			
	}
		//if(gCfgItems.multiple_language != 0)
		//{
			//buttonFunction_2 = 0;
			//buttonLanguage = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hSetWnd, BUTTON_CF_SHOW, 0, 308);				

		//}
		//else
		//{
		//	if(gCfgItems.func_btn2_display_flag == 1)
		//	{
		//		buttonLanguage = 0;
		//		buttonFunction_2 = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hSetWnd, BUTTON_CF_SHOW, 0, 308);
		//	}		
		//}
	#else
		buttonFilamentChange = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 306);
		if(gCfgItems.multiple_language != 0)
			buttonLanguage = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 307);
		#if 0
		if(gCfgItems.multiple_language != 0)
		{
			buttonFunction_1 = 0;
			buttonLanguage = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 307);
		}
		else
		{
			if(gCfgItems.func_btn1_display_flag == 1)
			{
				buttonLanguage = 0;
				buttonFunction_1 = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 307);
			}
		}
		#endif	
	#endif
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 309);

#if VERSION_WITH_PIC	

	BUTTON_SetBmpFileName(buttonDisk, "bmp_fileSys.bin",0);
	BUTTON_SetBmpFileName(buttonWifi, "bmp_wifi.bin",0);
	BUTTON_SetBmpFileName(buttonFan, "bmp_fan.bin");
	BUTTON_SetBmpFileName(buttonAbout, "bmp_about.bin",0);
	BUTTON_SetBmpFileName(buttonBreakpoint, "bmp_breakpoint.bin",0);
	#if SIMPLE_DISPLAY
	BUTTON_SetBmpFileName(buttonFunction_1, "bmp_function1.bin",1);
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetBmpFileName(buttonLanguage, "bmp_language.bin",1);
	}
	//else
	//{
	//	BUTTON_SetBmpFileName(buttonFunction_2, "bmp_function2.bin",1);	
	//}
	
	#else
	BUTTON_SetBmpFileName(buttonFilamentChange, "bmp_filamentchange.bin",0);
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetBmpFileName(buttonLanguage, "bmp_language.bin",1);
	}
	//else
	//{
		//BUTTON_SetBmpFileName(buttonFunction_1, "bmp_function1.bin",1);
	//}
	#endif
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);

	BUTTON_SetBitmapEx(buttonDisk, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonWifi, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonFan, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonAbout, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonBreakpoint, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	#if SIMPLE_DISPLAY
	BUTTON_SetBitmapEx(buttonFunction_1, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	if(gCfgItems.multiple_language != 0)
	{
			BUTTON_SetBitmapEx(buttonLanguage, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	}
	//else
	//{
	//	BUTTON_SetBitmapEx(buttonFunction_2, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	//}
	#else
	BUTTON_SetBitmapEx(buttonFilamentChange, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetBitmapEx(buttonLanguage, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	}
	//else
	//{
	//	BUTTON_SetBitmapEx(buttonFunction_1, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	//}
	#endif

	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	if(gCfgItems.multiple_language != 0)
	{
			BUTTON_SetText(buttonWifi,set_menu.wifi);
			BUTTON_SetText(buttonDisk,set_menu.filesys);
			//BUTTON_SetText(buttonWifi,WIFI_TEXT_CN);
			BUTTON_SetText(buttonFan,set_menu.fan);
			BUTTON_SetText(buttonAbout,set_menu.about);
			BUTTON_SetText(buttonBreakpoint,set_menu.breakpoint);
			#if SIMPLE_DISPLAY
			BUTTON_SetText(buttonFunction_1,set_menu.motoroff);
			BUTTON_SetText(buttonLanguage,set_menu.language);
			#else
			BUTTON_SetText(buttonFilamentChange,set_menu.filament);
			BUTTON_SetText(buttonLanguage,set_menu.language);
			#endif
			BUTTON_SetText(buttonRet,common_menu.text_back);			
	}

#endif
#endif

#if defined (STM32F10X_HD)
	buttonWifi = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 301);
	
	buttonFan = BUTTON_CreateEx(LCD_WIDTH  / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 302);
		
	buttonAbout = BUTTON_CreateEx(LCD_WIDTH /2 , 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 303);
		
	buttonBreakpoint = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 304);
		
	#if SIMPLE_DISPLAY
		if(gCfgItems.func_btn1_display_flag == 1)
		{
			buttonFunction_1 = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 305);
		}
		//if(gCfgItems.func_btn2_display_flag == 1)
		//{
		//	buttonFunction_2 = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 306);
		//}	
		if(gCfgItems.multiple_language != 0)
		{
			buttonLanguage = BUTTON_CreateEx(LCD_WIDTH / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hSetWnd, BUTTON_CF_SHOW, 0, 308);
		}
	#else
		buttonFilamentChange = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 305);
		
		if(gCfgItems.func_btn1_display_flag == 1)
		{
			buttonFunction_1 = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 306);
		}
		if(gCfgItems.multiple_language != 0)
		{
			buttonLanguage = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hSetWnd, BUTTON_CF_SHOW, 0, 308);
		}
		//else
		//{
		//	if(gCfgItems.func_btn2_display_flag == 1)
		//	{
		//		buttonFunction_2 = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hSetWnd, BUTTON_CF_SHOW, 0, 308);
		//	}
		//}
	#endif
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hSetWnd, BUTTON_CF_SHOW, 0, 309);

#if VERSION_WITH_PIC	

	//BUTTON_SetBmpFileName(buttonDisk, "bmp_fileSys.bin",0);
	BUTTON_SetBmpFileName(buttonWifi, "bmp_wifi.bin",0);
	BUTTON_SetBmpFileName(buttonFan, "bmp_fan.bin");
	BUTTON_SetBmpFileName(buttonAbout, "bmp_about.bin",0);
	BUTTON_SetBmpFileName(buttonBreakpoint, "bmp_breakpoint.bin",0);
	#if SIMPLE_DISPLAY
	BUTTON_SetBmpFileName(buttonFunction_1, "bmp_function1.bin",1);
	BUTTON_SetBmpFileName(buttonFunction_2, "bmp_function2.bin",1);	
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetBmpFileName(buttonLanguage, "bmp_language.bin",1);	
	}
	#else
	BUTTON_SetBmpFileName(buttonFilamentChange, "bmp_filamentchange.bin",0);
	BUTTON_SetBmpFileName(buttonFunction_1, "bmp_function1.bin",1);
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetBmpFileName(buttonLanguage, "bmp_language.bin",1);	
	}
	else
	{
		BUTTON_SetBmpFileName(buttonFunction_2, "bmp_function2.bin",1);
	}
	#endif
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);

	//BUTTON_SetBitmapEx(buttonDisk, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonWifi, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonFan, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonAbout, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonBreakpoint, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	#if SIMPLE_DISPLAY
	BUTTON_SetBitmapEx(buttonFunction_1, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonFunction_2, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetBitmapEx(buttonLanguage, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	}
	#else
	BUTTON_SetBitmapEx(buttonFilamentChange, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonFunction_1, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetBitmapEx(buttonLanguage, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	}
	else
	{
		BUTTON_SetBitmapEx(buttonFunction_2, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	}
	#endif

	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
#endif
#endif
	if(gCfgItems.multiple_language != 0)
	{
			BUTTON_SetText(buttonWifi,set_menu.wifi);
			BUTTON_SetText(buttonFan,set_menu.fan);
			BUTTON_SetText(buttonAbout,set_menu.about);
			BUTTON_SetText(buttonBreakpoint,set_menu.breakpoint);
			#if SIMPLE_DISPLAY
			BUTTON_SetText(buttonFunction_1,set_menu.motoroff);
			BUTTON_SetText(buttonFunction_2,set_menu.shutdown);
			BUTTON_SetText(buttonLanguage,set_menu.language);
			#else
			BUTTON_SetText(buttonFilamentChange,set_menu.filament);
			BUTTON_SetText(buttonFunction_1,set_menu.motoroff);
			BUTTON_SetText(buttonLanguage,set_menu.language);
			#endif
			BUTTON_SetText(buttonRet,common_menu.text_back); 	
	}
	//GUI_Exec();

	
}

void Clear_Set()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hSetWnd))
	{
		WM_DeleteWindow(hSetWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}


