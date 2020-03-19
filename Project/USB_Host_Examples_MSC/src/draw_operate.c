#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_operate.h"
#include "draw_ui.h"
#include "pic.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "usbh_usr.h"
#include "printer.h"
#include "printing_thread.h"
#include "text.h"


#ifndef GUI_FLASH
#define GUI_FLASH
#endif

static GUI_HWIN hOperateWnd, hStopDlgWnd;

extern int8_t curFilePath[30];

extern reset_tx_fifo();

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static PROGBAR_Handle printingBar;

static TEXT_Handle textPrintTemp1, textPrintTemp2, printStopDlgText;
static BUTTON_Handle buttonPause, buttonStop, buttonPreHeat, buttonFilament_P, buttonFan, buttonRet, buttonOk, buttonCancle, buttonMore_printing;
static BUTTON_Handle buttonSpeed;

uint8_t pause_flag;
static void cbOperateWin(WM_MESSAGE * pMsg) {

switch (pMsg->MsgId)
{
	case WM_PAINT:
		GUI_SetColor(gCfgItems.state_background_color);
		GUI_DrawRect(LCD_WIDTH/4, 0, LCD_WIDTH *3 / 4 -3, imgHeight /2 - 2);
		GUI_SetColor(gCfgItems.state_background_color);
		GUI_FillRect(LCD_WIDTH/4, 0, LCD_WIDTH *3 / 4 -3, imgHeight /2 - 2);
		
		break;
	case WM_TOUCH:
	 	
		break;
	case WM_TOUCH_CHILD:
		
		break;
		
	case WM_NOTIFY_PARENT:
		if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
		{
			if(pMsg->hWinSrc == buttonPause)
			{
				stop_print_time();
				if(printerStaus == pr_working)
				{
					pause_flag = 1;
					I2C_EE_Init(100000);
					/////print_task_flag = TASK_PAUSE;
					last_disp_state = OPERATE_UI;
					printerStaus = pr_pause;
					Clear_operate();
					draw_pause();
				}
			}
			else if(pMsg->hWinSrc == buttonStop)
			{	
				last_disp_state = OPERATE_UI;
				Clear_operate();
				
				draw_dialog(DIALOG_TYPE_STOP);
				
			}
			else if(pMsg->hWinSrc == buttonRet)
			{
				last_disp_state = OPERATE_UI;
				Clear_operate();
				//draw_return_ui();
				draw_printing();
			}
			else if(pMsg->hWinSrc == buttonPreHeat)
			{
				last_disp_state = OPERATE_UI;
				Clear_operate();
				draw_preHeat();
			}
			else if(pMsg->hWinSrc == buttonFan)
			{
				last_disp_state = OPERATE_UI;
				Clear_operate();
				draw_fan();
			}
			else if (pMsg->hWinSrc == buttonMore_printing)
			{
				last_disp_state = OPERATE_UI;
				Clear_operate();
				draw_printmore();				
			}

			else if(pMsg->hWinSrc == buttonFilament_P)
			{
				stop_print_time();
				if(printerStaus == pr_working)
				{
					last_disp_state = PAUSE_UI;
					I2C_EE_Init(100000);
					printerStaus = pr_pause;				
					Clear_operate();
					draw_FilamentChange();
				}
			}


		}
		break;
		
	default:
		WM_DefaultProc(pMsg);
	}
}


void draw_operate()
{
#if 1		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	

	//int8_t buf[50] = {0};
	
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != OPERATE_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = OPERATE_UI;
	}
	
	disp_state = OPERATE_UI;
		
	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
	//GUI_SetFont(&FONT_TITLE);
	#if 0
	if(gCfgItems.language == LANG_ENGLISH)
	{
		GUI_SetFont(&FONT_TITLE);
	}
	else
	{
		GUI_SetFont(&GUI_FontHZ16);
	}
	#endif

	//sprintf(buf, "正在打印->操作:%s", curFilePath);
	//GUI_DispStringAt(buf, 0, 0);
	GUI_UC_SetEncodeNone();
	GUI_SetFont(&GUI_FontHZ16);
	GUI_DispStringAt(creat_title_text(), 0, 0);
	GUI_SetFont(&FONT_TITLE);
	GUI_UC_SetEncodeUTF8();

	hOperateWnd=0;
	buttonPause=0;
	buttonStop=0;
	buttonPreHeat=0;
	buttonFilament_P=0;
	buttonMore_printing=0;
	buttonRet=0;
	
	hOperateWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbOperateWin, 0);
	
	buttonPause = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hOperateWnd, BUTTON_CF_SHOW, 0, 301);
	buttonStop = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hOperateWnd, BUTTON_CF_SHOW, 0, 302);
	buttonPreHeat = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hOperateWnd, BUTTON_CF_SHOW, 0, 303);

	buttonFilament_P = BUTTON_CreateEx(LCD_WIDTH / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hOperateWnd, BUTTON_CF_SHOW, 0, 106);

	buttonMore_printing = BUTTON_CreateEx(LCD_WIDTH / 2,	imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hOperateWnd, BUTTON_CF_SHOW, 0, 107);

	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hOperateWnd, BUTTON_CF_SHOW, 0, 306);
	//TEXT_SetDefaultFont(&FONT_STATE_INF);	
	

	#if VERSION_WITH_PIC	

	BUTTON_SetBmpFileName(buttonPause, "bmp_pause.bin",0);
	BUTTON_SetBmpFileName(buttonStop, "bmp_stop.bin",0);
	BUTTON_SetBmpFileName(buttonPreHeat, "bmp_temp.bin",0);
	/*
	if(gCfgItems.print_finish_close_machine_flg == 1)
	{
		BUTTON_SetBmpFileName(buttonMore_printing, "bmp_more.bin",0);
	}
	else
	{
		BUTTON_SetBmpFileName(buttonFan, "bmp_fan.bin",0);
	}
	*/
	BUTTON_SetBmpFileName(buttonMore_printing, "bmp_more.bin",0);

	BUTTON_SetBmpFileName(buttonFilament_P, "bmp_filamentchange.bin",0);

	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);
	
	BUTTON_SetBitmapEx(buttonPause, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonStop, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonPreHeat, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	/*
	if(gCfgItems.print_finish_close_machine_flg == 1)
	{
		BUTTON_SetBitmapEx(buttonMore_printing, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	}
	else
	{
		BUTTON_SetBitmapEx(buttonFan, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	}
	*/
	BUTTON_SetBitmapEx(buttonMore_printing, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);

	BUTTON_SetBitmapEx(buttonFilament_P, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);

	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);

	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetText(buttonPause,operation_menu.pause);
		BUTTON_SetText(buttonStop,operation_menu.stop);
		BUTTON_SetText(buttonPreHeat,operation_menu.temp);

		BUTTON_SetText(buttonFilament_P,operation_menu.filament);

		BUTTON_SetText(buttonMore_printing,operation_menu.more);
		BUTTON_SetText(buttonRet,common_menu.text_back); 	
	}

	#endif


	//TEXT_SetDefaultFont(&GUI_FontHZ_fontHz14);

	
	printingBar = PROGBAR_CreateEx(LCD_WIDTH  / 4 + 5, 15,  LCD_WIDTH / 2 - 10, 30, hOperateWnd, WM_CF_SHOW, 0, 0);

	textPrintTemp1 = TEXT_CreateEx(LCD_WIDTH  / 4 + 5, 60, LCD_WIDTH / 4 - 4, 40, hOperateWnd, WM_CF_SHOW, TEXT_CF_LEFT,	GUI_ID_TEXT1, " ");

	textPrintTemp2 = TEXT_CreateEx(LCD_WIDTH  / 2 , 60, LCD_WIDTH / 4 - 4, 40, hOperateWnd, WM_CF_SHOW, TEXT_CF_LEFT,	GUI_ID_TEXT2, " ");

	TEXT_SetTextAlign(textPrintTemp1,  GUI_TA_VCENTER | GUI_TA_LEFT);
	TEXT_SetTextAlign(textPrintTemp2,  GUI_TA_VCENTER | GUI_TA_LEFT);
	
	PROGBAR_SetBarColor(printingBar, 0, GUI_GREEN);
	
	PROGBAR_SetTextColor(printingBar, 0, GUI_BLACK);
	PROGBAR_SetFont(printingBar, &GUI_FontHZ_fontHz14);
	
	
	PROGBAR_SetValue(printingBar, get_printing_rate(srcfp));
	
	disp_temp_operate();
	
	//GUI_Exec();

#endif
	
}


void setProBarRateOpera(int rate)
{
	if(rate <= 0)
		return;
	gCurFileState.totalSend = rate;
	
	if(disp_state == OPERATE_UI)
	{
		PROGBAR_SetValue(printingBar, rate );
	}
	
}

void disp_temp_operate()
{
	int8_t buf[50] = {0};
	
	TEXT_SetTextColor(textPrintTemp1, gCfgItems.state_text_color);
	TEXT_SetTextColor(textPrintTemp2, gCfgItems.state_text_color);
	//GUI_SetFont(&FONT_STATE_INF);

	TEXT_SetBkColor(textPrintTemp1, gCfgItems.state_background_color);
	TEXT_SetBkColor(textPrintTemp2, gCfgItems.state_background_color);
/*
	#ifdef SPRAYER_NUM_LIMIT	
	if(gCfgItems.sprayerNum == 2)
	#endif
	{
		sprintf(buf, " E1:%d\n B:%d", (int)gCfgItems.curSprayerTemp[0], (int)gCfgItems.curBedTemp);
	
	}
	#ifdef SPRAYER_NUM_LIMIT	
	else
	{
		sprintf(buf, "%.1f  %.1f  %d", gCfgItems.curSprayerTemp[0], gCfgItems.curBedTemp, gCfgItems.fanSpeed);
	
	}
	#endif
	TEXT_SetText(textPrintTemp1, buf);
	
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "E2:%d\nFAN:%d", (int)gCfgItems.curSprayerTemp[1], gCfgItems.fanSpeed);
	TEXT_SetText(textPrintTemp2, buf);
*/
	//HC-chen
	if(gCfgItems.sprayerNum == 2 && gCfgItems.single_nozzle==0)
	{
		if(gCfgItems.custom_bed_flag == 1)
		{
			if(gCfgItems.language != LANG_SPANISH)
			{
				sprintf(buf, " E1:%d\n B:%d", (int)gCfgItems.curSprayerTemp[0], (int)gCfgItems.curBedTemp);
			}
			else
			{
				sprintf(buf, " E1:%d\n C:%d", (int)gCfgItems.curSprayerTemp[0], (int)gCfgItems.curBedTemp);
			}
			TEXT_SetText(textPrintTemp1, buf);
		}
		else
		{
			sprintf(buf, " E1:%d\n", (int)gCfgItems.curSprayerTemp[0]);
			TEXT_SetText(textPrintTemp1, buf);			
		}
		memset(buf, 0, sizeof(buf));
		if(gCfgItems.language != LANG_SPANISH)
		{
			sprintf(buf, "E2:%d\nFAN:%d", (int)gCfgItems.curSprayerTemp[1],gCfgItems.fanSpeed);
		}
		else
		{
			sprintf(buf, "E2:%d\nVent:%d", (int)gCfgItems.curSprayerTemp[1],gCfgItems.fanSpeed);
		}
		TEXT_SetText(textPrintTemp2, buf);
		
	}
	else
	{
		if(gCfgItems.custom_bed_flag == 1)
		{
			if(gCfgItems.language != LANG_SPANISH)
			{
				sprintf(buf, " E1:%d\n B:%d", (int)gCfgItems.curSprayerTemp[0], (int)gCfgItems.curBedTemp);
			}
			else
			{
				sprintf(buf, " E1:%d\n C:%d", (int)gCfgItems.curSprayerTemp[0], (int)gCfgItems.curBedTemp);
			}
			TEXT_SetText(textPrintTemp1, buf);
		}
		else
		{
			sprintf(buf, " E1:%d\n", (int)gCfgItems.curSprayerTemp[0]);
			TEXT_SetText(textPrintTemp1, buf);			
		}
		memset(buf, 0, sizeof(buf));
		if(gCfgItems.language != LANG_SPANISH)
			sprintf(buf, "FAN:%d\n", gCfgItems.fanSpeed);
		else
			sprintf(buf, "Vent:%d\n", gCfgItems.fanSpeed);
		TEXT_SetText(textPrintTemp2, buf);
	}

}


void Clear_operate()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hOperateWnd))
	{
		WM_DeleteWindow(hOperateWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}
