
#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_print_file.h"
#include "draw_ui.h"
#include "pic.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "text.h"
#include "usbh_usr.h"
#include "draw_printing.h"
#include "mks_tft_com.h"
#include "printer.h"
#include "Sd_usr.h"


#ifndef GUI_FLASH
#define GUI_FLASH
#endif

GUI_HWIN hPrintFileWnd, hAlertWin, hPrintFileDlgWnd;

uint8_t trig_browser_flag;

int8_t curDirLever = 0;

DIR_OFFSET dir_offset[10];


uint8_t curFileName[100] = {0};
extern int8_t  gCurDir[100];

extern int8_t gFile_Browser (char* path);

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                     USB_Host;

void Clear_print_file();

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

extern void display_udisk_files();
extern uint32_t   getTick();
extern uint32_t  getTickDiff(int32_t lastTick, int32_t  curTick);


extern uint8_t gcode_cache;

static int8_t pages;
static TEXT_Handle printDlgAskText;
static BUTTON_Handle buttonPu, buttonPd, buttonR, buttonF[5], buttonOk, buttonCancle;

uint8_t back_flg = 0;	
uint8_t backup_fileAttr[5];

static void cbPrintFileWin(WM_MESSAGE * pMsg) {
int8_t sel_item;
int8_t i,j;
	
uint32_t temp;
	
//int8_t sel_file[30];

//struct PressEvt *press_event;
switch (pMsg->MsgId) {
case WM_PAINT:
//GUI_SetBkColor(GUI_BLUE);
//GUI_Clear();
//GUI_DispString("window");
break;
case WM_TOUCH:
// 	press_event = (struct PressEvt *)pMsg->Data.p;
	
break;
case WM_TOUCH_CHILD:
//  press_event = (struct PressEvt *)pMsg->Data.p;
	
  break;
  case WM_NOTIFY_PARENT:
	if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
	{
		if(pMsg->hWinSrc == buttonPd)
		{	
			GUI_UC_SetEncodeUTF8();
			if(dir_offset[curDirLever].cur_page_last_offset > 0)
			{
								
				
				//2015.05.19
				Sd_file_cnt = 0;
				gcodeFileList.index = 0;

				Sd_file_offset = dir_offset[curDirLever].cur_page_last_offset + 1;
				do
				{
					if(gCfgItems.fileSysType == FILE_SYS_SD)
					{
						ShowSDFiles();
					}
					else if(gCfgItems.fileSysType == FILE_SYS_USB)
					{
						display_udisk_files();
					}

					
					if((gcodeFileList.listVaild == 1) && (gcodeFileList.index == 1))
						dir_offset[curDirLever].cur_page_first_offset = Sd_file_offset;
				
					
					if(gcodeFileList.index >= 5)
					{
						dir_offset[curDirLever].cur_page_last_offset = Sd_file_offset;
						Sd_file_offset++;		
					
						break;
					}
					Sd_file_offset++;					
					
				} while(gcodeFileList.listVaild != 2);

				if(gcodeFileList.index != 0)
				{
					//dir_offset[curDirLever].cur_page_last_offset = 0;
					Clear_print_file();
					disp_udisk_files(0);
					dir_offset[curDirLever].curPage++;
					
				}
				
				if(gcodeFileList.listVaild == 2)
				{
						dir_offset[curDirLever].cur_page_last_offset = 0;
				}
					
		
				
		//		pages = Sd_display_file_cnt/ MAX_FILES_PER_LEVER+ 1;
			}
			
		}
		else if(pMsg->hWinSrc == buttonPu)
		{
			GUI_UC_SetEncodeUTF8();
			if(dir_offset[curDirLever].curPage > 0)
			{
				
				//2015.05.19
				Sd_file_cnt = 0;
				gcodeFileList.index = 0;
				
				if(dir_offset[curDirLever].cur_page_first_offset >= 4)
					Sd_file_offset = dir_offset[curDirLever].cur_page_first_offset - 1;
				
				do 
				{
					if(gCfgItems.fileSysType == FILE_SYS_SD)
					{
						ShowSDFiles();
					}
					else if(gCfgItems.fileSysType == FILE_SYS_USB)
					{
						display_udisk_files();
					}


					if((gcodeFileList.listVaild == 1) && (gcodeFileList.index == 1))
						dir_offset[curDirLever].cur_page_last_offset = Sd_file_offset;
										
				
					if(gcodeFileList.index >= 5)
					{
						dir_offset[curDirLever].cur_page_first_offset = Sd_file_offset;

						if(Sd_file_offset)
							Sd_file_offset--;
						
						
						break;
					}

					if(Sd_file_offset)
						Sd_file_offset--;
										
				} while(gcodeFileList.listVaild != 2);
				if(gcodeFileList.index != 0)
				{
					Clear_print_file();
					disp_udisk_files(1);
					dir_offset[curDirLever].curPage--;
				}
				
			}
		}
		else if(pMsg->hWinSrc == buttonR)
		{
			GUI_UC_SetEncodeUTF8();
			if(curDirLever == 0)
			{
				if(gCfgItems.pwd_reprint_flg == 1)
				{
					gCfgItems.pwd_reprint_flg = 0;
					last_disp_state = SET_UI;
					Clear_print_file();
					Sd_file_offset = 0;
					Sd_file_cnt = 0;
					draw_return_ui();	
				}
				else
				{
					last_disp_state = PRINT_FILE_UI;
					Clear_print_file();
					//2015.05.19
					Sd_file_offset = 0;
					Sd_file_cnt = 0;
					//Sd_display_file_cnt = 0;
					draw_ready_print();
				}
			}
			else
			{
				int8_t *ch = 0;
				
				ch =  (int8_t *)strrchr(gCurDir, '/');
				if(ch != 0)
				{
					*ch = 0;
					Clear_print_file();
					dir_offset[curDirLever].curPage = 0;
					dir_offset[curDirLever].cur_page_first_offset = 0;
					dir_offset[curDirLever].cur_page_last_offset = 0;
					curDirLever--;
					search_files();
					
				}
				
			}
		}
		else
		{
			for(i = 0; i < 5; i++)
			{
				if(back_flg == 1)
				{
					j = 4 - i;
				}
				else
				{
					j = i;
				}
				if(pMsg->hWinSrc == buttonF[i])
				{
					//if(gcodeFileList.fileName[i][0] != 0)
					if(gcodeFileList.fileName[j][0] != 0)
					{
						//if(gcodeFileList.fileAttr[i] == 1) //dir
						if(gcodeFileList.fileAttr[j] == 1) //dir
						//if(backup_fileAttr[i] == 1) //dir
						{
							memset(gCurDir, 0, sizeof(gCurDir));
							//strcpy(gCurDir,  gcodeFileList.fileName[i]);
							strcpy(gCurDir,  gcodeFileList.fileName[j]);
							Clear_print_file();
							curDirLever++;
							search_files();
							
						}
						else
						{
							
							memset(curFileName, 0, sizeof(curFileName));
							//strcpy(curFileName, gcodeFileList.fileName[i]);
							strcpy(curFileName, gcodeFileList.fileName[j]);

							Clear_print_file();
							
							draw_dialog(DIALOG_TYPE_PRINT_FILE);
						}
						break;
					}
				}
			}
		}
		
	}
	break;
default:
WM_DefaultProc(pMsg);
}
}

void draw_print_file()
{
	
	
	//int titleHeight = 30;
//	int buttonHeight = 30;
//	int buttonWidth = LCD_WIDTH/4;
	//int imgHeight = LCD_HEIGHT - titleHeight;

	
	int i;
	
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != PRINT_FILE_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINT_FILE_UI;
	}
	disp_state = PRINT_FILE_UI;

	GUI_SetBkColor(gCfgItems.background_color);
	GUI_Clear();
	GUI_SetColor(gCfgItems.title_color);
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

	hPrintFileWnd = 0;
	//hPrintFileWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbPrintFileWin, 0);

	curDirLever = 0;	
	dir_offset[curDirLever].curPage = 0;
	

	Sd_file_offset = 0;
	Sd_file_cnt = 0;
	//Sd_display_file_cnt = 0;

	memset(dir_offset, 0, sizeof(dir_offset));
	
	//GUI_SetFont(&FONT_TITLE);
	
		if(gCfgItems.fileSysType == FILE_SYS_SD)
		{
			memset(gCurDir, 0, sizeof(gCurDir));
			strcpy(gCurDir, "1:");
			//SD_Init();
			SD_Initialize();
			
		}
		else if(gCfgItems.fileSysType == FILE_SYS_USB)
		{
			memset(gCurDir, 0, sizeof(gCurDir));
			strcpy(gCurDir, "0:");
					
		}
	USB_Host.gState = HOST_DEV_DISCONNECTED;	
	
	search_files();
	
}

void search_files()
{
	uint32_t tick1, tick2;
	GUI_UC_SetEncodeUTF8();
//	dir_offset[curDirLever].curPage = 0;
	GUI_SetColor(gCfgItems.title_color);
	#if 0
	if(gCfgItems.language == LANG_ENGLISH)
	{
		GUI_DispStringAt(creat_title_text(), 0, 0);
		GUI_DispStringAt("Loading......", 80, 100);
	}
	else 	if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		GUI_DispStringAt(creat_title_text(), 0, 0);
		GUI_DispStringAt("ÕýÔÚÝdÈë......", 80, 100);
	}
	else
	{
		GUI_DispStringAt(creat_title_text(), 0, 0);
		GUI_DispStringAt("ÕýÔÚÔØÈë......", 80, 100);
	}
	#endif
	GUI_DispStringAt(creat_title_text(), 0, 0);
	GUI_DispStringAt(file_menu.file_loading, 80, 100);	

	GUI_UC_SetEncodeNone();
	
	tick1 = getTick();
	
	
	//while(1)
	{

		Sd_file_cnt = 0;
		
		gcodeFileList.listVaild = 0;
		gcodeFileList.index = 0;
		memset(gcodeFileList.fileAttr, 0, sizeof(gcodeFileList.fileAttr));
		memset(gcodeFileList.fileName, 0, sizeof(gcodeFileList.fileName));

		gcode_cache = 0;
		//if(gCfgItems.fileSysType == FILE_SYS_SD)
		{					

			Sd_file_offset = dir_offset[curDirLever].cur_page_first_offset;
			do
			{
				if(gCfgItems.fileSysType == FILE_SYS_SD)
				{
					ShowSDFiles();
				}
				else if(gCfgItems.fileSysType == FILE_SYS_USB)
				{
					display_udisk_files();
				}
				
				if((gcodeFileList.listVaild == 1) && (gcodeFileList.index == 1))
					dir_offset[curDirLever].cur_page_first_offset = Sd_file_offset;
			
				
				if(gcodeFileList.index >= 5)
				{
					dir_offset[curDirLever].cur_page_last_offset = Sd_file_offset;
					Sd_file_offset++;					
					break;
				}
				Sd_file_offset++;
				
				
				
			} while(gcodeFileList.listVaild != 2);
			
		}
		/*
		else if(gCfgItems.fileSysType == FILE_SYS_USB)
		{
		//	memset(gCurDir, 0, sizeof(gCurDir));
		//	strcpy(gCurDir, "0:");
		
			display_udisk_files();
			//gFile_Browser(gCurDir);
		}
		*/
	/*	if(gcodeFileList.index != 0)
			break;
		
		tick2 = getTick();
		if(getTickDiff(tick1, tick2) >= 2000)
		{
			break;
		}*/
	}
	
	if(gcodeFileList.index == 0)
	{
		//buttonF[0] = BUTTON_CreateEx(0,  0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hPrintFileWnd, BUTTON_CF_SHOW, 0, 103);
		//BUTTON_SetText(buttonF[0], "æ— æ–‡ä»¶");

		GUI_UC_SetEncodeUTF8();
		//GUI_Exec();
		GUI_ClearRect(80, 100, 300, 150);
		if(curDirLever == 0)
		{
			GUI_DispStringAt(file_menu.no_file_and_check, 100, 100);
		}
		else
		{
			GUI_DispStringAt(file_menu.no_file, 100, 100);	
		}

		tick2 = getTick();
		
		while(1)
		{
			tick1 = getTick();
			if(getTickDiff(tick2, tick1) >= 3000)
			{
				if(curDirLever == 0)
				{
					if(gCfgItems.pwd_reprint_flg == 1)
					{
						last_disp_state = SET_UI;
						Clear_print_file();
						draw_return_ui();
					}
					else
					{
						last_disp_state = PRINT_FILE_UI;
						Clear_print_file();
						draw_ready_print();
					}
				}
				else
				{
					int8_t *ch = 0;
					
					ch =  (int8_t *)strrchr(gCurDir, '/');
					if(ch != 0)
					{
						*ch = 0;
						Clear_print_file();
						dir_offset[curDirLever].curPage = 0;
						dir_offset[curDirLever].cur_page_first_offset = 0;
						dir_offset[curDirLever].cur_page_last_offset = 0;
						curDirLever--;
						search_files();
						
					}
					
				}
			//Clear_print_file();
				//draw_ready_print();
				break;
			}
		}
		return;
		
	}

	GUI_ClearRect(80, 100, 300, 150);
	//pages = gcodeFileList.index / 5 + 1;
//	pages = Sd_display_file_cnt/ MAX_FILES_PER_LEVER+ 1;

	disp_udisk_files(0);
}

void disp_udisk_files(int seq)
{
	int8_t tmpStr[100] = {0};
	int8_t *tmpStr1;
	int8_t *tmpStr2;
	int8_t i, j;

/*	if(hPrintFileWnd  !=  0)
	{
		Clear_print_file();
	}*/

	GUI_UC_SetEncodeUTF8();
	GUI_DispStringAt(creat_title_text(), 0, 0);
	

	hPrintFileWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbPrintFileWin, 0);

	buttonPu = BUTTON_CreateEx(LCD_WIDTH / 4,  imgHeight / 2,LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hPrintFileWnd, BUTTON_CF_SHOW, 0, 104);
	buttonPd = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hPrintFileWnd, BUTTON_CF_SHOW, 0, 105);
	buttonR = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  imgHeight / 2,LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hPrintFileWnd, BUTTON_CF_SHOW, 0, 106);


	//button1 = BUTTON_CreateEx(LCD_WIDTH  / 4 - 3, imgHeight - buttonHeight, buttonWidth - 1, buttonHeight - 1,hPrintFileWnd, BUTTON_CF_SHOW, 0, 201);
	//button2 = BUTTON_CreateEx(LCD_WIDTH  / 4 + buttonWidth - 2,  imgHeight - buttonHeight,buttonWidth - 1, buttonHeight - 1, hPrintFileWnd, BUTTON_CF_SHOW, 0, 202);
	//button3 = BUTTON_CreateEx(LCD_WIDTH  / 4 + buttonWidth * 2 ,   imgHeight - buttonHeight, buttonWidth - 1, buttonHeight - 1, hPrintFileWnd, BUTTON_CF_SHOW, 0, 203);

	
#if VERSION_WITH_PIC	

	BUTTON_SetBmpFileName(buttonPu, "bmp_pageUp.bin",1);
	BUTTON_SetBmpFileName(buttonPd, "bmp_pageDown.bin",1);
	BUTTON_SetBmpFileName(buttonR, "bmp_return.bin",0);

	BUTTON_SetBitmapEx(buttonPu, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonPd, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonR, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	if(gCfgItems.multiple_language != 0)
	{
			BUTTON_SetText(buttonPu,file_menu.page_up);
			BUTTON_SetText(buttonPd,file_menu.page_down);
			BUTTON_SetText(buttonR,common_menu.text_back);	
	}

#endif	

	BUTTON_SetDefaultTextColor(gCfgItems.filename_color, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultTextColor(gCfgItems.filename_color, BUTTON_CI_PRESSED);
	BUTTON_SetDefaultBkColor(gCfgItems.filename_background_color, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultBkColor(gCfgItems.filename_background_color, BUTTON_CI_PRESSED);

	GUI_Exec();

	GUI_UC_SetEncodeNone();
	
	for(i = 0; i < 5; i++)
	{
		if(seq)
		{
			j = 4 - i;
			back_flg = 1;
		}
		else
		{
			j = i;
			back_flg = 0;
		}
		
		//backup_fileAttr[i] = gcodeFileList.fileAttr[j];
		
		if(i  >= gcodeFileList.index)
		{
			break;
		}
	
				
		if(i <= 3)
		{
			buttonF[i] = BUTTON_CreateEx(i * LCD_WIDTH / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintFileWnd, BUTTON_CF_SHOW, 0, 103 + i);
		}
		else
		{
			buttonF[i] = BUTTON_CreateEx((i - 4) * LCD_WIDTH / 4,  imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintFileWnd, BUTTON_CF_SHOW, 0, 107 + i);
		}
		
		BUTTON_SetFont(buttonF[i],&GUI_FontHZ16);
		memset(tmpStr, 0, sizeof(tmpStr));
		//2015.05.19
		//cutFileName(gcodeFileList.fileName[curPage * 5 + i], 16, 8,  tmpStr);
		cutFileName(gcodeFileList.fileName[j], 16, 8,  tmpStr);
		/*tmpStr1 = (uint8_t *)strstr((uint8_t *)gcodeFileList.fileName[curPage * 5 + i], "/");
		tmpStr2 = tmpStr1;
		tmpStr2++;
	
		while(1)
		{
			
			tmpStr1 = (uint8_t *)strstr(tmpStr2, "/");
			if(tmpStr1 == 0)
			{
				strncpy(tmpStr, tmpStr2, 5);
				break;
			}
			tmpStr1++;
			tmpStr2 = (uint8_t *)strstr(tmpStr1, "/");
			if(tmpStr2 == 0)
			{
				strncpy(tmpStr, tmpStr1, 5);
				break;
			}
			tmpStr2++;
			
		}*/
#if VERSION_WITH_PIC	
/*//2015.05.19
		if(gcodeFileList.fileAttr[i + curPage * 5] == 1) //dir
		{
			BUTTON_SetBmpFileName(buttonF[i], "bmp_dir.bin");
			BUTTON_SetBitmapEx(buttonF[i], 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
		}
		else
		{
			BUTTON_SetBmpFileName(buttonF[i], "bmp_file.bin");
			BUTTON_SetBitmapEx(buttonF[i], 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
		}
*/
	if(gcodeFileList.fileAttr[j] == 1) //dir
	//if(backup_fileAttr[i] == 1) //dir
	{
		BUTTON_SetBmpFileName(buttonF[i], "bmp_dir.bin",0);
		BUTTON_SetBitmapEx(buttonF[i], 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	}
	else
	{
		BUTTON_SetBmpFileName(buttonF[i], "bmp_file.bin",0);
		BUTTON_SetBitmapEx(buttonF[i], 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	}

#endif

		
		
		BUTTON_SetText(buttonF[i], tmpStr);
	}
	
	BUTTON_SetDefaultBkColor(gCfgItems.background_color, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultBkColor(gCfgItems.background_color, BUTTON_CI_PRESSED);
	
	GUI_Exec();
	GUI_UC_SetEncodeUTF8();

}

void Clear_print_file()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hPrintFileWnd))
	{
		WM_DeleteWindow(hPrintFileWnd);
		GUI_Exec();
	}
	//¶ÀÁ¢Ê¹ÓÃ²»ÄÜÆÁ±Î
	GUI_Clear();
}


