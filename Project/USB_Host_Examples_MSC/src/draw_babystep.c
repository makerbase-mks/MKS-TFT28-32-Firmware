#include "draw_babystep.h"
#include "gui.h"
#include "button.h"
#include "draw_ui.h"
#include "draw_print_file.h"
#include "fontLib.h"
#include "printer.h"
#include "gcode.h"
#include "stdint.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif

BUTTON_Handle buttonBabystepXI, buttonBabystepXD, buttonBabystepYI, buttonBabystepYD, buttonBabystepZI, buttonBabystepZD, buttonBabystepV,  buttonBabystepRet;
GUI_HWIN hMoveBabyStepWnd;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;
extern TFT_FIFO gcodeCmdTxFIFO;		//gcode 指令发送队列
extern TFT_FIFO gcodeCmdRxFIFO;		//gcode	指令接收队列

//int16_t curXSpeed = 3000;
//int16_t curYSpeed = 3000;
//int16_t curZSpeed = 3000;
static float babystep_dist=0.01;
static uint8_t has_set_babystep = 0;

static void cbBabyStepMotorWin(WM_MESSAGE * pMsg) {
	
	struct PressEvt *press_event;
	char buf[30] = {0};

	switch (pMsg->MsgId) {
	case WM_PAINT:
	//GUI_SetBkColor(GUI_BLACK);
	//	GUI_Clear();
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
		
			press_event = (struct PressEvt *)pMsg->Data.p;

			
			if(pMsg->hWinSrc == buttonBabystepXI)
			{
				BABY_STEP_X_COMMAN(buf, babystep_dist);
				pushFIFO(&gcodeCmdTxFIFO,(unsigned char *) buf);
				//has_set_babystep = 1;
			}
			else if(pMsg->hWinSrc == buttonBabystepXD)
			{
				BABY_STEP_X_COMMAN(buf, (float)0-babystep_dist);
				pushFIFO(&gcodeCmdTxFIFO,(unsigned char *) buf);
				//has_set_babystep = 1;
			}
			else if(pMsg->hWinSrc == buttonBabystepYI)
			{
				BABY_STEP_Y_COMMAN(buf, babystep_dist);
				pushFIFO(&gcodeCmdTxFIFO,(unsigned char *) buf);
				//has_set_babystep = 1;
			}
			else if(pMsg->hWinSrc == buttonBabystepYD)
			{
				BABY_STEP_Y_COMMAN(buf, (float)0-babystep_dist);
				pushFIFO(&gcodeCmdTxFIFO,(unsigned char *) buf);
				//has_set_babystep = 1;
			}
			else if(pMsg->hWinSrc == buttonBabystepZI)
			{
				BABY_STEP_Z_COMMAN(buf, babystep_dist);
				pushFIFO(&gcodeCmdTxFIFO,(unsigned char *) buf);
				has_set_babystep = 1;
			}
			else if(pMsg->hWinSrc == buttonBabystepZD)
			{
				BABY_STEP_Z_COMMAN(buf, (float)0-babystep_dist);
				pushFIFO(&gcodeCmdTxFIFO,(unsigned char *) buf);
				has_set_babystep = 1;
			}
			else if(pMsg->hWinSrc == buttonBabystepRet)
			{
				if(has_set_babystep == 1)
				{
					has_set_babystep = 0;
					pushFIFO(&gcodeCmdTxFIFO,(unsigned char *) SAVE_EEPROM_COMMAND);
				}
				last_disp_state = BABY_STEP_UI;
				Clear_babyStep();
				draw_return_ui();
								
			}
			else if(pMsg->hWinSrc == buttonBabystepV)
			{
				if(abs((int)(100 * babystep_dist)) == 1)
				{
					babystep_dist = 0.05;
				}
				else if(abs((int)(100 * babystep_dist)) == 5)
				{
					babystep_dist = 0.1;
				}
				else 
				{
					babystep_dist = 0.01;
				}
				
				disp_babystep_dist();
				
			}
			
			
		}
		break;
	default:
	WM_DefaultProc(pMsg);
	}
}


void draw_babyStep()
{
	uint8_t buffer_z[15]={0};
  	
//	int titleHeight = 30;
//	int imgHeight = LCD_HEIGHT - titleHeight;
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != BABY_STEP_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = BABY_STEP_UI;
	}
	disp_state = BABY_STEP_UI;
	
	//GUI_SetFont(&GUI_Font8x18);
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
	//GUI_DispStringAt("准备打印->移动", 0, 0);
	GUI_DispStringAt(creat_title_text(), 0, 0);
	//显示Z轴坐标
	/*if(printerStaus != pr_pause)
	{
		sprintf(buffer_z,"Z:%0.2f",gCfgItems.zpos);
		GUI_DispStringAt(buffer_z,220, 0);
	}*/

	hMoveBabyStepWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbBabyStepMotorWin, 0);
/*
	buttonXI = BUTTON_CreateEx(0, 0,LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 101);
	buttonYI = BUTTON_CreateEx(LCD_WIDTH / 4,  0,LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 102);
	buttonZI = BUTTON_CreateEx(LCD_WIDTH / 2,  0,LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 103);


	buttonXD = BUTTON_CreateEx(0,  imgHeight / 2,LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 105);
	buttonYD = BUTTON_CreateEx(LCD_WIDTH / 4,  imgHeight / 2,LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 106);
	buttonZD = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 107);

	buttonV = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 108);
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  imgHeight / 2,LCD_WIDTH / 4 - 1, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 109);
*/
	buttonBabystepXI = BUTTON_CreateEx(0, 0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveBabyStepWnd, BUTTON_CF_SHOW, 0, 101);
	buttonBabystepYI = BUTTON_CreateEx(LCD_WIDTH / 4,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveBabyStepWnd, BUTTON_CF_SHOW, 0, 102);
	buttonBabystepZI = BUTTON_CreateEx(LCD_WIDTH / 2,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveBabyStepWnd, BUTTON_CF_SHOW, 0, 103);


	buttonBabystepXD = BUTTON_CreateEx(0,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveBabyStepWnd, BUTTON_CF_SHOW, 0, 105);
	buttonBabystepYD = BUTTON_CreateEx(LCD_WIDTH / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveBabyStepWnd, BUTTON_CF_SHOW, 0, 106);
	buttonBabystepZD = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveBabyStepWnd, BUTTON_CF_SHOW, 0, 107);

	buttonBabystepV = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveBabyStepWnd, BUTTON_CF_SHOW, 0, 108);
	buttonBabystepRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveBabyStepWnd, BUTTON_CF_SHOW, 0, 109);

	#if VERSION_WITH_PIC	

		BUTTON_SetBmpFileName(buttonBabystepXI, "bmp_xAdd.bin",1);
		BUTTON_SetBmpFileName(buttonBabystepXD, "bmp_xDec.bin",1);
		BUTTON_SetBmpFileName(buttonBabystepYI, "bmp_yAdd.bin",1);
		BUTTON_SetBmpFileName(buttonBabystepYD, "bmp_yDec.bin",1);
		BUTTON_SetBmpFileName(buttonBabystepZI, "bmp_zAdd.bin",1);
		BUTTON_SetBmpFileName(buttonBabystepZD, "bmp_zDec.bin",1);

	//
	BUTTON_SetBmpFileName(buttonBabystepRet, "bmp_return.bin",0);
	
	BUTTON_SetBitmapEx(buttonBabystepXI, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonBabystepXD, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonBabystepYI, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonBabystepYD, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonBabystepZI, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonBabystepZD, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonBabystepV, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonBabystepRet, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);

	#endif

	
	disp_babystep_dist();

	//GUI_Exec();
	if(gCfgItems.multiple_language != 0)
	{
			BUTTON_SetText(buttonBabystepXI,move_menu.x_add);
			BUTTON_SetText(buttonBabystepXD,move_menu.x_dec);
			BUTTON_SetText(buttonBabystepYI,move_menu.y_add);
			BUTTON_SetText(buttonBabystepYD,move_menu.y_dec);
			BUTTON_SetText(buttonBabystepZI,move_menu.z_add);
			BUTTON_SetText(buttonBabystepZD,move_menu.z_dec);	
			BUTTON_SetText(buttonBabystepRet,common_menu.text_back);	
	}	
}

void disp_babystep_dist()
{
	char buf[30] = {0};
		
	if((int)(100 *babystep_dist) == 1)
	{
		BUTTON_SetBmpFileName(buttonBabystepV, "bmp_babystep00_1.bin",1);
	}
	else if((int)(100*babystep_dist) == 5)
	{
		BUTTON_SetBmpFileName(buttonBabystepV, "bmp_babystep005.bin",1);
	}
	else if((int)(100*babystep_dist) == 10)
	{
		BUTTON_SetBmpFileName(buttonBabystepV, "bmp_babystep0_1.bin",1);
	}
	if(gCfgItems.multiple_language != 0)
	{
		if((int)(100 * babystep_dist) == 1)
			BUTTON_SetText(buttonBabystepV,move_menu.step_001mm);
		else if((int)(100 * babystep_dist) == 5)
			BUTTON_SetText(buttonBabystepV,move_menu.step_005mm);
		else if((int)(100 * babystep_dist) == 10)
			BUTTON_SetText(buttonBabystepV,move_menu.step_01mm);
	}	
}

void Clear_babyStep()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hMoveBabyStepWnd))
	{
		WM_DeleteWindow(hMoveBabyStepWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}

