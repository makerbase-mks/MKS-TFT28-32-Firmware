#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_disk.h"
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

static GUI_HWIN hDiskWnd;


extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

	
static BUTTON_Handle buttonMainSd, buttonSd, buttonUdisk, buttonRet;

static void cbDiskWin(WM_MESSAGE * pMsg) {

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
					Clear_Disk();
					draw_return_ui();
					
				}
			/*	else if(pMsg->hWinSrc == buttonMainSd)
				{
					gCfgItems.fileSysType = 2;

					bakup_cfg_inf(BAK_FILE_SYS_ID,  gCfgItems.fileSysType);
				
					disp_disk_choose();
				}*/
				else if(pMsg->hWinSrc == buttonSd)
				{
					gCfgItems.fileSysType = 1;

					bakup_cfg_inf(BAK_FILE_SYS_ID,  gCfgItems.fileSysType);
					
					disp_disk_choose();
				}
				else if(pMsg->hWinSrc == buttonUdisk)
				{
					gCfgItems.fileSysType = 0;

					bakup_cfg_inf(BAK_FILE_SYS_ID,  gCfgItems.fileSysType);
					
					disp_disk_choose();
				}
				
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}


void draw_Disk()
{
		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != DISK_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = DISK_UI;
	}
	disp_state = DISK_UI;
		
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


	//GUI_DispStringAt("准备打印->设置->文件系统", 0, 0);
	GUI_DispStringAt(creat_title_text(), 0, 0);
	
	hDiskWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbDiskWin, 0);

	
	buttonSd = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hDiskWnd, BUTTON_CF_SHOW, 0, 301);
	buttonUdisk = BUTTON_CreateEx(LCD_WIDTH  / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hDiskWnd, BUTTON_CF_SHOW, 0, 302);
	//buttonUdisk = BUTTON_CreateEx(LCD_WIDTH /2 , 0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hDiskWnd, BUTTON_CF_SHOW, 0, 303);
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hDiskWnd, BUTTON_CF_SHOW, 0, 308);
	
	#if VERSION_WITH_PIC	

	//BUTTON_SetBitmapEx(buttonSd, 0, &bmbmp_sd, BMP_PIC_X, BMP_PIC_Y);
	//BUTTON_SetBitmapEx(buttonUdisk, 0, &bmbmp_usb, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetText(buttonRet,common_menu.text_back);
	}

	#endif
	
	disp_disk_choose();

#if VERSION_WITH_PIC	

#endif
	
}

void disp_disk_choose()
{
		
	if(gCfgItems.fileSysType == 0)
	{
		#if VERSION_WITH_PIC	
		BUTTON_SetBmpFileName(buttonUdisk, "bmp_usb_sel.bin",0);
		BUTTON_SetBmpFileName(buttonSd, "bmp_sd.bin",0);
		BUTTON_SetBitmapEx(buttonUdisk, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonSd, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		#endif
	}
	else if(gCfgItems.fileSysType == 1)
	{		
		#if VERSION_WITH_PIC	
		BUTTON_SetBmpFileName(buttonUdisk, "bmp_usb.bin",0);
		BUTTON_SetBmpFileName(buttonSd, "bmp_sd_sel.bin",0);
		BUTTON_SetBitmapEx(buttonUdisk, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonSd, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		#endif
	}
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetText(buttonUdisk,filesys_menu.usb_sys);	
		BUTTON_SetText(buttonSd,filesys_menu.sd_sys);	
	}
		
}


void Clear_Disk()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hDiskWnd))
	{
		WM_DeleteWindow(hDiskWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}

