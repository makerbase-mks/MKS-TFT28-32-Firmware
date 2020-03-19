#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_fan.h"
#include "draw_ui.h"
#include "pic.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "usbh_usr.h"
#include "printer.h"
#include "printing_thread.h"
#include "text.h"
#include "gcode.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif

GUI_HWIN hFanWnd;
static TEXT_Handle TextFanSpeed;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static BUTTON_Handle buttonAdd, buttonDec, buttonOpen, buttonHalf, buttonOff,  buttonRet;

static void cbFanWin(WM_MESSAGE * pMsg) {

	uint8_t buf[30] = {0};

	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetColor(gCfgItems.state_background_color);
			GUI_DrawRect(LCD_WIDTH/4, 0, LCD_WIDTH *3 / 4 -4, imgHeight /2 - 2);
			GUI_SetColor(gCfgItems.state_background_color);
			GUI_FillRect(LCD_WIDTH/4, 0, LCD_WIDTH *3 / 4 -4, imgHeight /2 - 2);
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
						if(gCfgItems.print_finish_close_machine_flg == 1)
						{
							
							if(printerStaus != pr_idle)
							{
								disp_state_stack._disp_index -= 1;
								Clear_fan();
								draw_return_ui();
							}
							else
							{
								Clear_fan();
								draw_return_ui();
							}
								
						}
						else
						{
							last_disp_state = FAN_UI;
							Clear_fan();
							draw_return_ui();
						}
				}
				else if(pMsg->hWinSrc == buttonOpen)
				{
					#if 0
					if(last_disp_state == SET_UI)
					{
						gCfgItems.fanOnoff = 1;
						gCfgItems.pre_fanSpeed = 255;
						SET_FAN_SPEED_COMMAND(buf, gCfgItems.pre_fanSpeed);
						pushFIFO(&gcodeCmdTxFIFO, buf);
					}
					else
						#endif
					{
						gCfgItems.fanOnoff = 1;
						gCfgItems.fanSpeed = 255;
						SET_FAN_SPEED_COMMAND(buf, gCfgItems.fanSpeed);
						pushFIFO(&gcodeCmdTxFIFO, buf);			
					}
					disp_fan_speed();
				}
				else if(pMsg->hWinSrc == buttonOff)
				{
					#if 0
					if(last_disp_state == SET_UI)
					{
						gCfgItems.fanOnoff = 0;
						gCfgItems.pre_fanSpeed = 0;
						pushFIFO(&gcodeCmdTxFIFO, SET_FAN_OFF_COMMAND);
					}
					else
					#endif
					{
						gCfgItems.fanOnoff = 0;
						gCfgItems.fanSpeed = 0;
						pushFIFO(&gcodeCmdTxFIFO, SET_FAN_OFF_COMMAND);
					}					
					disp_fan_speed();
				}
				else if(pMsg->hWinSrc == buttonHalf)
				{
					#if 0
					//last_disp_state = FAN_UI;
					if(last_disp_state == SET_UI)
					{
						gCfgItems.fanOnoff = 1;
						gCfgItems.pre_fanSpeed = 127;
						SET_FAN_SPEED_COMMAND(buf, gCfgItems.pre_fanSpeed);
						pushFIFO(&gcodeCmdTxFIFO, buf);
					}
					else
					#endif
					{
						gCfgItems.fanOnoff = 1;
						gCfgItems.fanSpeed = 127;
						SET_FAN_SPEED_COMMAND(buf, gCfgItems.fanSpeed);
						pushFIFO(&gcodeCmdTxFIFO, buf);					
					}
					disp_fan_speed();
				}
				else if(pMsg->hWinSrc == buttonAdd)
				{
				#if 0
					if(last_disp_state ==SET_UI)
					{
						if(gCfgItems.pre_fanSpeed < 255)
						{
							gCfgItems.pre_fanSpeed++;
							SET_FAN_SPEED_COMMAND(buf, gCfgItems.pre_fanSpeed);
							pushFIFO(&gcodeCmdTxFIFO, buf);
						}
					}
					else
				#endif
					{
						if(gCfgItems.fanSpeed < 255)
						{
							//HC-chen
							gCfgItems.fanOnoff = 1;
							gCfgItems.fanSpeed++;
							SET_FAN_SPEED_COMMAND(buf, gCfgItems.fanSpeed);
							pushFIFO(&gcodeCmdTxFIFO, buf);		
						}
					}
					disp_fan_speed();
				
				}
				else if(pMsg->hWinSrc == buttonDec)
				{
				#if 0
					if(last_disp_state ==SET_UI)
					{
						if(gCfgItems.pre_fanSpeed > 0)
						{
							gCfgItems.pre_fanSpeed--;
							SET_FAN_SPEED_COMMAND(buf, gCfgItems.pre_fanSpeed);						
							pushFIFO(&gcodeCmdTxFIFO, buf);
						}
					}
					else
						#endif
					{
						if(gCfgItems.fanSpeed > 0)
						{
							//HC-chen
							gCfgItems.fanOnoff = 1;
							gCfgItems.fanSpeed--;
							SET_FAN_SPEED_COMMAND(buf, gCfgItems.fanSpeed);						
							pushFIFO(&gcodeCmdTxFIFO, buf);
						}				
					}
					disp_fan_speed();
				
				}
			}
			
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}


void draw_fan()
{
#if 1		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != FAN_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = FAN_UI;
	}
	disp_state = FAN_UI;
		
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
	
	hFanWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbFanWin, 0);

	buttonAdd = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hFanWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonDec = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hFanWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonOpen = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 2, hFanWnd, BUTTON_CF_SHOW, 0, 303);
	buttonHalf = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hFanWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonOff = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hFanWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hFanWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

	//TEXT_SetDefaultFont(&FONT_STATE_INF);

	

	TextFanSpeed = TEXT_CreateEx(LCD_WIDTH  / 4, 0, LCD_WIDTH / 2-2, imgHeight / 2-1, hFanWnd, WM_CF_SHOW, TEXT_CF_LEFT,	GUI_ID_TEXT1, "ÅçÍ·ÎÂ¶È");

	TEXT_SetTextAlign(TextFanSpeed,  GUI_TA_VCENTER | GUI_TA_HCENTER);

	#if VERSION_WITH_PIC	
	BUTTON_SetBmpFileName(buttonAdd, "bmp_Add.bin",1);	
	BUTTON_SetBmpFileName(buttonDec, "bmp_Dec.bin",1);	
	BUTTON_SetBmpFileName(buttonOpen, "bmp_speed255.bin",1);	
	BUTTON_SetBmpFileName(buttonHalf, "bmp_speed127.bin",1);	
	BUTTON_SetBmpFileName(buttonOff, "bmp_speed0.bin",1);	
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);	
	
	BUTTON_SetBitmapEx(buttonAdd, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonDec, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonOpen, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonHalf, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonOff, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);

	#endif
	
	disp_fan_speed();
	if(gCfgItems.multiple_language != 0)
	{
			BUTTON_SetText(buttonOpen,fan_menu.full);
			BUTTON_SetText(buttonHalf,fan_menu.half);
			BUTTON_SetText(buttonOff,fan_menu.off);	
			BUTTON_SetText(buttonAdd,fan_menu.add);
			BUTTON_SetText(buttonDec,fan_menu.dec);
			BUTTON_SetText(buttonRet,common_menu.text_back);	
	}

#endif	
}

void disp_fan_speed()
{
	int8_t buf[25] = {0};
	int8_t buf1[5] = {0};
	
	TEXT_SetTextColor(TextFanSpeed, gCfgItems.state_text_color);
	TEXT_SetBkColor(TextFanSpeed, gCfgItems.state_background_color);
	{
			if(gCfgItems.fanOnoff)
			{
				//sprintf(buf, "FAN\n%d", gCfgItems.fanSpeed);
				sprintf(buf, fan_menu.state);
				sprintf(buf1, "\n%d", gCfgItems.fanSpeed);
				strcat(buf,buf1);
			}
			else
			{
				//sprintf(buf, "FAN\n0");
				sprintf(buf, fan_menu.state);
				sprintf(buf1, "\n0");
				strcat(buf,buf1);
			}
	}		
	TEXT_SetText(TextFanSpeed, buf);
}


void Clear_fan()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hFanWnd))
	{
		WM_DeleteWindow(hFanWnd);
		GUI_Exec();
	}
	//GUI_Clear();
}



