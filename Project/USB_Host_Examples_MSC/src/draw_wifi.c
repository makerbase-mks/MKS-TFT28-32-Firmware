#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_wifi.h"
#include "draw_ui.h"
#include "pic.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "usbh_usr.h"
#include "printer.h"
#include "printing_thread.h"
#include "wifi_module.h"
#include "text.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif

static GUI_HWIN hWifiWnd;


extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;


static TEXT_Handle textWifiIP,textWifiName,textWifiKey,textWifiState;
	
static BUTTON_Handle  buttonCloud, buttonRet,buttonWifiEnable;
static void cbWifiWin(WM_MESSAGE * pMsg) {

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
				if(pMsg->hWinSrc == buttonCloud)
				{
					Clear_Wifi();
					draw_bind();
				}
				else if(pMsg->hWinSrc == buttonRet)
				{
					Clear_Wifi();
					draw_return_ui();
					
				}
				else if(pMsg->hWinSrc == buttonWifiEnable)
				{
					
					if(gCfgItems.wifi_state==wifiEnble)
					{
						disp_wifi_enable(wifiDisable);
						gCfgItems.wifi_state=wifiDisable;
						WIFI_RESET();
					}
					else
					{
						disp_wifi_enable(wifiEnble);
						gCfgItems.wifi_state=wifiEnble;
						WIFI_SET();
					}
					I2C_EE_BufferWrite((uint8_t *)&gCfgItems.wifi_state, BAK_WIFI_STATE_ADDR,  1);
				}
				
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}

//every key:22*23
#define PAD_KEY_WIDTH	22
#define PAD_KEY_HEIGH	23
#define PAD_KEY_DIST	2

void draw_Wifi()
{
		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	int row;

	
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != WIFI_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = WIFI_UI;
	}
	disp_state = WIFI_UI;
	GUI_Clear();	
/*		
	GUI_SetBkColor(gCfgItems.state_background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();


	
	GUI_DispStringAt(creat_title_text(), 0, 0);
	
	GUI_SetBkColor(gCfgItems.state_background_color);
	GUI_SetColor(gCfgItems.state_text_color);
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
	
	GUI_SetFont(&FONT_TITLE);
	#endif
	GUI_DispStringAt(creat_title_text(), 0, 0);

	//disp_wifi_state();
	
	
	
	
	//GUI_SetFont(&FONT_TITLE);
	//GUI_SetFont(&GUI_FontHZ16);

	//GUI_SetColor(gCfgItems.state_text_color);

	//GUI_DrawRect(1, titleHeight + 1, LCD_WIDTH  - 3, LCD_HEIGHT - 3);

	//GUI_DispStringAt("准备打印->设置", 0, 0);
	
	//hWifiWnd = WM_CreateWindow(0 , titleHeight + imgHeight / 2, LCD_WIDTH - 1, imgHeight / 2 - 1, WM_CF_SHOW, cbWifiWin, 0);
	hWifiWnd = WM_CreateWindow(0 , titleHeight, LCD_WIDTH - 1, imgHeight, WM_CF_SHOW, cbWifiWin, 0);

	textWifiIP = TEXT_CreateEx(15, 0, LCD_WIDTH * 3/4 - 20, titleHeight-5, hWifiWnd, WM_CF_SHOW, TEXT_CF_LEFT,  GUI_ID_TEXT0, "");	
	textWifiName = TEXT_CreateEx(15, titleHeight-5, LCD_WIDTH * 3/4 - 20, titleHeight-5, hWifiWnd, WM_CF_SHOW, TEXT_CF_LEFT,  GUI_ID_TEXT0, "");
	textWifiKey = TEXT_CreateEx(15, 2*titleHeight-10, LCD_WIDTH * 3/4 - 20, titleHeight-5, hWifiWnd, WM_CF_SHOW, TEXT_CF_LEFT,  GUI_ID_TEXT0, "");	
	textWifiState = TEXT_CreateEx(15, 3*titleHeight-15, LCD_WIDTH * 3/4 - 20, titleHeight-5, hWifiWnd, WM_CF_SHOW, TEXT_CF_LEFT,  GUI_ID_TEXT0, "");	

	if((gCfgItems.wifi_type == 0x02)&&(gCfgItems.cloud_enable == 1))
	{
		buttonCloud = BUTTON_CreateEx(LCD_WIDTH  / 2  ,imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hWifiWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		BUTTON_SetBmpFileName(buttonCloud, "bmp_cloud.bin",1);
		BUTTON_SetBitmapEx(buttonCloud, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);

	}
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hWifiWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	
#if VERSION_WITH_PIC	
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);

	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);	
#endif
	//HC-chen
	if(gCfgItems.cloud_enable == 1)
	{
		buttonWifiEnable=BUTTON_CreateEx(LCD_WIDTH  / 4  , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hWifiWnd, BUTTON_CF_SHOW, 0, alloc_win_id());	
	}
	else
	{
		buttonWifiEnable=BUTTON_CreateEx(LCD_WIDTH  / 2  , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hWifiWnd, BUTTON_CF_SHOW, 0, alloc_win_id());		
	}
	if(gCfgItems.multiple_language != 0)
	{
			if((gCfgItems.wifi_type == 0x02)&&(gCfgItems.cloud_enable == 1))
				BUTTON_SetText(buttonCloud,wifi_menu.cloud); 
			BUTTON_SetText(buttonRet,common_menu.text_back); 	
	}

	TEXT_SetTextColor(textWifiIP, gCfgItems.state_text_color);
	TEXT_SetBkColor(textWifiIP,  gCfgItems.state_background_color);

	TEXT_SetTextColor(textWifiKey, gCfgItems.state_text_color);
	TEXT_SetBkColor(textWifiKey,  gCfgItems.state_background_color);

	TEXT_SetTextColor(textWifiName, gCfgItems.state_text_color);
	TEXT_SetBkColor(textWifiName,  gCfgItems.state_background_color);

	TEXT_SetTextColor(textWifiState, gCfgItems.state_text_color);
	TEXT_SetBkColor(textWifiState,  gCfgItems.state_background_color);
	
	disp_wifi_state();
	disp_wifi_enable(gCfgItems.wifi_state);
	
}

//HC-chen
void disp_wifi_enable(uint8_t state)
{
	if(state==wifiEnble)
	{
		//TEXT_SetText(textWifiEnable, wifi_menu.wifi_state_enable);
		/*BUTTON_SetText(buttonWifiEnable,wifi_menu.disable_wifi); 
		//WM_EnableWindow(buttonWifiEnable);
		BUTTON_SetTextColor(buttonWifiEnable, BUTTON_CI_UNPRESSED, GUI_WHITE);
		BUTTON_SetTextColor(buttonWifiEnable, BUTTON_CI_PRESSED, GUI_WHITE);
		
		BUTTON_SetBkColor(buttonWifiEnable, BUTTON_CI_UNPRESSED, GUI_BLUE);
		BUTTON_SetBkColor(buttonWifiEnable, BUTTON_CI_PRESSED, GUI_LIGHTBLUE);*/
		BUTTON_SetBmpFileName(buttonWifiEnable, "bmp_wifi_enable.bin",0);
		BUTTON_SetBitmapEx(buttonWifiEnable, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
		if(gCfgItems.multiple_language != 0)
		{
			BUTTON_SetText(buttonWifiEnable,wifi_menu.disable_wifi); 	
		}
	}
	else
	{
		//TEXT_SetText(textWifiEnable, wifi_menu.wifi_state_disable);
		/*BUTTON_SetText(buttonWifiEnable,wifi_menu.enable_wifi); 
		//WM_DisableWindow(buttonWifiEnable);
		BUTTON_SetTextColor(buttonWifiEnable, BUTTON_CI_UNPRESSED, GUI_BLACK);
		BUTTON_SetTextColor(buttonWifiEnable, BUTTON_CI_PRESSED, GUI_BLACK);
		
		BUTTON_SetBkColor(buttonWifiEnable, BUTTON_CI_UNPRESSED, GUI_WHITE);
		BUTTON_SetBkColor(buttonWifiEnable, BUTTON_CI_PRESSED, GUI_WHITE);*/
		BUTTON_SetBmpFileName(buttonWifiEnable, "bmp_wifi_disable.bin",0);
		BUTTON_SetBitmapEx(buttonWifiEnable, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
		if(gCfgItems.multiple_language != 0)
		{
			BUTTON_SetText(buttonWifiEnable,wifi_menu.enable_wifi);	
		}
		
	}	
}

void disp_wifi_state()
{
		#if 0
		char buf[100];
		memset(buf, ' ', sizeof(buf)-1);
		GUI_DispStringAt(buf, 30, titleHeight  + 5);
		GUI_DispStringAt(wifi_menu.ip, 30, titleHeight	+ 5);
		sprintf(buf, "%s", (char *)ipPara.ip_addr); 	
		GUI_DispStringAt(buf, 30+30, titleHeight	+ 5);
		
		memset(buf, ' ', sizeof(buf)-1);
		GUI_DispStringAt(buf, 30, titleHeight  + 30);
		GUI_DispStringAt(wifi_menu.wifi, 30, titleHeight	+ 30);
		sprintf(buf, "%s", wifiPara.ap_name);
		GUI_DispStringAt(buf, 30+50, titleHeight	+ 30);
		

		if(wifiPara.mode == 0x01)
		{
			memset(buf, ' ', sizeof(buf)-1);
			GUI_DispStringAt(buf, 30, titleHeight  + 55);
			GUI_DispStringAt(wifi_menu.key, 30, titleHeight  + 55);		
			sprintf(buf, "%s", wifiPara.keyCode);
			GUI_DispStringAt(buf, 30+40, titleHeight	+ 55);
		
			memset(buf, ' ', sizeof(buf)-1);
			GUI_DispStringAt(buf, 30, titleHeight + 80);
			GUI_DispStringAt(wifi_menu.state_ap, 30, titleHeight + 80);

			memset(buf, ' ', sizeof(buf)-1);
			GUI_DispStringAt(buf, 160, titleHeight + 80);		
			if(wifi_link_state == WIFI_CONNECTED)
				GUI_DispStringAt(wifi_menu.connected, 160, titleHeight + 80);
			else if(wifi_link_state == WIFI_NOT_CONFIG)
				GUI_DispStringAt(wifi_menu.disconnected, 160, titleHeight + 80);
			else 
				GUI_DispStringAt(wifi_menu.exception, 160, titleHeight + 80);
		}
		else
		{
			memset(buf, ' ', sizeof(buf)-1);
			GUI_DispStringAt(buf, 30, titleHeight + 55);
			GUI_DispStringAt(wifi_menu.state_sta, 30, titleHeight + 55);
		
			memset(buf, ' ', sizeof(buf)-1);
			GUI_DispStringAt(buf, 160, titleHeight + 55);		
			if(wifi_link_state == WIFI_CONNECTED)
				GUI_DispStringAt(wifi_menu.connected, 160, titleHeight + 55);
			else if(wifi_link_state == WIFI_NOT_CONFIG)
				GUI_DispStringAt(wifi_menu.disconnected, 160, titleHeight + 55);
			else 
				GUI_DispStringAt(wifi_menu.exception, 160, titleHeight + 55);			
		}
		#endif
		char buf[100];
		memset(buf,' ', sizeof(buf)-1);
		TEXT_SetText(textWifiIP, buf);
		sprintf(buf,wifi_menu.ip);
		strcat(buf,(char *)ipPara.ip_addr);
		TEXT_SetText(textWifiIP, buf);
		#if 1
		memset(buf,' ', sizeof(buf)-1);
		TEXT_SetText(textWifiName, buf);
		sprintf(buf,wifi_menu.wifi);
		strcat(buf,(char *)wifiPara.ap_name);
		TEXT_SetText(textWifiName, buf);

		if(wifiPara.mode == 0x01)
		{
			memset(buf,' ', sizeof(buf)-1);
			TEXT_SetText(textWifiKey, buf);
			sprintf(buf,wifi_menu.key);
			strcat(buf,(char *)wifiPara.keyCode);
			TEXT_SetText(textWifiKey, buf);

			memset(buf,' ', sizeof(buf)-1);
			TEXT_SetText(textWifiState, buf);
			sprintf(buf,wifi_menu.state_ap);
			strcat(buf,"     ");
			if(wifi_link_state == WIFI_CONNECTED)
				strcat(buf,wifi_menu.connected);
			else if(wifi_link_state == WIFI_NOT_CONFIG)
				strcat(buf,wifi_menu.disconnected);
			else 
				strcat(buf,wifi_menu.exception);
			
			TEXT_SetText(textWifiState, buf);
		}
		else
		{
			memset(buf, ' ', sizeof(buf)-1);
			TEXT_SetText(textWifiState, buf);
			TEXT_SetText(textWifiKey, buf);
			sprintf(buf,wifi_menu.state_sta);
			strcat(buf,"     ");
				
			if(wifi_link_state == WIFI_CONNECTED)
				strcat(buf,wifi_menu.connected);
			else if(wifi_link_state == WIFI_NOT_CONFIG)
				strcat(buf,wifi_menu.disconnected);
			else 
				strcat(buf,wifi_menu.exception);
			//显示状态
			TEXT_SetText(textWifiKey, buf);
		}
		#endif
}

void refresh_wifi_ui()
{
	
}

void Clear_Wifi()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hWifiWnd))
	{
		WM_DeleteWindow(hWifiWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}


