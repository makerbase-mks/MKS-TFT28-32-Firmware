#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_pause_ui.h"
#include "draw_ui.h"
#include "pic.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "usbh_usr.h"
#include "printer.h"
#include "printing_thread.h"
#include "text.h"
#include "draw_printing_moremenu.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif

static GUI_HWIN hPauseWnd;

extern volatile uint8_t get_temp_flag;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static TEXT_Handle textPrintTemp1, textPrintTemp2;

static PROGBAR_Handle printingBar;

static BUTTON_Handle buttonRecover, buttonStop, buttonExtruOut, buttonFilament_P, buttonMov, buttonFan,buttonmore_pause;
static BUTTON_Handle buttonPreHeat;

extern uint8_t pause_flag;

extern uint8_t pause_from_mt_high_level;
extern uint8_t pause_from_pw_high_level;

#define PW_PORT_READ    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)
#define MT_PORT_READ    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)

static void cbPauseWin(WM_MESSAGE * pMsg) {


	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetColor(gCfgItems.state_background_color);
			GUI_DrawRect(LCD_WIDTH/4 + 1, 1, LCD_WIDTH *3 / 4 -3, imgHeight /2 - 3);
			GUI_SetColor(gCfgItems.state_background_color);
			GUI_FillRect(LCD_WIDTH/4 + 1, 1, LCD_WIDTH *3 / 4 -3, imgHeight /2 - 3);
			
			break;
		case WM_TOUCH:
		 		
			break;
		case WM_TOUCH_CHILD:
			
			break;
			
		case WM_NOTIFY_PARENT:
			if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				if(pMsg->hWinSrc == buttonStop)
				{
					last_disp_state = PAUSE_UI;
					Clear_pause();
				
					draw_dialog(DIALOG_TYPE_STOP);
					
				}
				else if(pMsg->hWinSrc == buttonRecover)
				{
					Get_Temperature_Flg = 1;
					get_temp_flag = 1;
					if(printerStaus == pr_reprint)
					{
						last_disp_state = PAUSE_UI;
						Clear_pause();
						draw_return_ui();
						GUI_Exec();
						#ifdef SAVE_FROM_SD
						if(gCfgItems.pwroff_save_mode != 0)
						#endif
						{
							rePrintProcess();
						}
						#ifdef SAVE_FROM_SD
						else
						{
							if((RE_PRINT_STATUS)gCfgItems.rePrintFlag == printer_pwdwn_reprint)
							{
								rePrintProcess_pwrdwn();
							}
							else
							{
								rePrintProcess();
							}
						}
						#endif
						printerStaus = pr_working;
						start_print_time();
						I2C_EE_Init(400000);//恢复打印，将速度改成400k
					}
					if(printerStaus == pr_pause)
					{
						//断料进入暂停界面且不接料，按恢复进入提示界面
						if((pause_from_pw_high_level == 1 && PW_PORT_READ == SET)
							||(pause_from_mt_high_level == 1 && MT_PORT_READ == SET))
						{
							
							
							last_disp_state = PAUSE_UI;
							Clear_pause();
							draw_dialog(DIALOG_EMPTY_FILAMENT_TIPS);
						}
						else
						{
							pause_from_mt_high_level=0;
							pause_from_pw_high_level=0;
							
							pause_flag = 0;
							I2C_EE_Init(400000);
							start_print_time();
							////////	print_task_flag = TASK_GOING;
							last_disp_state = PAUSE_UI;
							printerStaus = pr_working;
							Clear_pause();
							//draw_printing();
							//draw_return_ui();
							draw_operate();
						}
					}
					
					/*
					if((printerStaus == pr_pause) ||( printerStaus == pr_reprint))
					{
						start_print_time();
						////////	print_task_flag = TASK_GOING;
						last_disp_state = PAUSE_UI;
						printerStaus = pr_working;
						Clear_pause();
						//draw_printing();
						draw_return_ui();
					}
					*/
			
				}
				else if(pMsg->hWinSrc == buttonExtruOut)
				{
					last_disp_state = PAUSE_UI;
					Clear_pause();
					draw_extrusion();
				}
				else if(pMsg->hWinSrc == buttonMov)
				{
					last_disp_state = PAUSE_UI;
					Clear_pause();
					draw_move_motor();
				}
				else if(pMsg->hWinSrc == buttonFan)
				{
					last_disp_state = PAUSE_UI;
					Clear_pause();
					draw_fan();
				}
				else if(pMsg->hWinSrc == buttonmore_pause)
				{
					last_disp_state = PAUSE_UI;
					Clear_pause();
					draw_printmore();
				}	
				else if(pMsg->hWinSrc == buttonFilament_P)
				{
					last_disp_state = PAUSE_UI;
					Clear_pause();
					draw_FilamentChange();
				}				
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
}


void draw_pause()
{
#if 1		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	//int8_t buf[50] = {0};
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != PAUSE_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = PAUSE_UI;
	}
	disp_state = PAUSE_UI;
//每个移动操作都已经设置了标志，所以不需要再设置了。
	//if(last_disp_state == MOVE_MOTOR_UI)
	//	positionSaveFlag = 1;
		
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



	//sprintf(buf, "正在打印->操作->暂停:%s", curFileName);
	//GUI_DispStringAt(buf, 0, 0);
	GUI_UC_SetEncodeNone();
	GUI_SetFont(&GUI_FontHZ16);
	GUI_DispStringAt(creat_title_text(), 0, 0);
	GUI_SetFont(&FONT_TITLE);
	GUI_UC_SetEncodeUTF8();

	
	hPauseWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbPauseWin, 0);

	
	buttonRecover = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPauseWnd, BUTTON_CF_SHOW, 0, 301);
	buttonStop = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPauseWnd, BUTTON_CF_SHOW, 0, 302);
	buttonExtruOut = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPauseWnd, BUTTON_CF_SHOW, 0, 303);
	buttonMov = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPauseWnd, BUTTON_CF_SHOW, 0, 304);
/*
	if(gCfgItems.print_finish_close_machine_flg == 1)
	{
		buttonPreHeat = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPauseWnd, BUTTON_CF_SHOW, 0, 305);
		buttonmore_pause = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPauseWnd, BUTTON_CF_SHOW, 0, 306);
	}
	else
	{
		buttonFan = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPauseWnd, BUTTON_CF_SHOW, 0, 305);
		buttonPreHeat = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPauseWnd, BUTTON_CF_SHOW, 0, 306);
	}
	*/
	
	buttonFilament_P = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPauseWnd, BUTTON_CF_SHOW, 0, 305);
	buttonmore_pause = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPauseWnd, BUTTON_CF_SHOW, 0, 306);

	//TEXT_SetDefaultFont(&FONT_STATE_INF);

	#if VERSION_WITH_PIC	
	BUTTON_SetBmpFileName(buttonRecover, "bmp_resume.bin",0);
	BUTTON_SetBmpFileName(buttonStop, "bmp_stop.bin",0);
	BUTTON_SetBmpFileName(buttonExtruOut, "bmp_extruct.bin",0);
	BUTTON_SetBmpFileName(buttonMov, "bmp_mov.bin",0);
	
	BUTTON_SetBmpFileName(buttonmore_pause, "bmp_more.bin",0);
	BUTTON_SetBmpFileName(buttonFilament_P, "bmp_filamentchange.bin",0); 

	/*
	if(gCfgItems.print_finish_close_machine_flg == 1)
	{
		BUTTON_SetBmpFileName(buttonmore_pause, "bmp_more.bin",0);
		BUTTON_SetBmpFileName(buttonPreHeat, "bmp_temp.bin",0);	
	}
	else
	{
		BUTTON_SetBmpFileName(buttonFan, "bmp_fan.bin",0);
		BUTTON_SetBmpFileName(buttonPreHeat, "bmp_temp.bin",0);
	}
	*/
	BUTTON_SetBitmapEx(buttonRecover, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonStop, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonExtruOut, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonMov, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	
	BUTTON_SetBitmapEx(buttonmore_pause, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonFilament_P, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);	
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetText(buttonRecover,pause_menu.resume);
		BUTTON_SetText(buttonStop,pause_menu.stop);
		BUTTON_SetText(buttonExtruOut,pause_menu.extrude);
		BUTTON_SetText(buttonMov,pause_menu.move);
		BUTTON_SetText(buttonmore_pause,pause_menu.more);
		BUTTON_SetText(buttonFilament_P,pause_menu.filament); 			
	}
	/*
	if(gCfgItems.print_finish_close_machine_flg == 1)
	{
		BUTTON_SetBitmapEx(buttonmore_pause, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonPreHeat, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);	
	}
	else
	{
		BUTTON_SetBitmapEx(buttonFan, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonPreHeat, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	}
	*/
	#endif
	
	//TEXT_SetDefaultFont(&GUI_FontHZ_fontHz14);
	printingBar = PROGBAR_CreateEx(LCD_WIDTH  / 4 + 5, 15,  LCD_WIDTH / 2 - 10, 30, hPauseWnd, WM_CF_SHOW, 0, 0);

	textPrintTemp1 = TEXT_CreateEx(LCD_WIDTH  / 4 + 5, 60, LCD_WIDTH / 4 - 4, 40, hPauseWnd, WM_CF_SHOW, TEXT_CF_LEFT,	GUI_ID_TEXT1, " ");

	textPrintTemp2 = TEXT_CreateEx(LCD_WIDTH  / 2 , 60, LCD_WIDTH / 4 - 4, 40, hPauseWnd, WM_CF_SHOW, TEXT_CF_LEFT,	GUI_ID_TEXT2, " ");

	TEXT_SetTextAlign(textPrintTemp1,  GUI_TA_VCENTER | GUI_TA_LEFT);
	TEXT_SetTextAlign(textPrintTemp2,  GUI_TA_VCENTER | GUI_TA_LEFT);
	
	PROGBAR_SetBarColor(printingBar, 0, GUI_GREEN);
	
	PROGBAR_SetTextColor(printingBar, 0, GUI_BLACK);
	PROGBAR_SetFont(printingBar, &GUI_FontHZ_fontHz14);
	

	PROGBAR_SetValue(printingBar, get_printing_rate(srcfp));
	
	
	disp_temp_pause();
	
	//GUI_Exec();

#endif
	
}

void disp_temp_pause()
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
	if(gCfgItems.sprayerNum == 2 && gCfgItems.single_nozzle==0)
	{
		if(gCfgItems.custom_bed_flag == 1)
		{
			sprintf(buf, " E1:%d\n B:%d", (int)gCfgItems.curSprayerTemp[0], (int)gCfgItems.curBedTemp);
			TEXT_SetText(textPrintTemp1, buf);
		}
		else
		{
			sprintf(buf, " E1:%d\n", (int)gCfgItems.curSprayerTemp[0]);
			TEXT_SetText(textPrintTemp1, buf);			
		}
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "E2:%d\nFAN:%d", (int)gCfgItems.curSprayerTemp[1],gCfgItems.fanSpeed);
		TEXT_SetText(textPrintTemp2, buf);
		
	}
	else
	{
		if(gCfgItems.custom_bed_flag == 1)
		{
			sprintf(buf, " E1:%d\n B:%d", (int)gCfgItems.curSprayerTemp[0], (int)gCfgItems.curBedTemp);
			TEXT_SetText(textPrintTemp1, buf);
		}
		else
		{
			sprintf(buf, " E1:%d\n", (int)gCfgItems.curSprayerTemp[0]);
			TEXT_SetText(textPrintTemp1, buf);			
		}
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "FAN:%d\n", gCfgItems.fanSpeed);
		TEXT_SetText(textPrintTemp2, buf);
	}		



}

void Clear_pause()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hPauseWnd))
	{
		WM_DeleteWindow(hPauseWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}


