#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_extrusion.h"
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

GUI_HWIN hExtrusionWnd;
static TEXT_Handle textPrintTemp, textExtruAmount;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static BUTTON_Handle buttonStepForward, buttonStepBack, buttonSprayType, buttonStep, buttonSpeed,  buttonStepExtru, buttonRet;

static int32_t extructAmount;

static void cbExtrusionWin(WM_MESSAGE * pMsg) {

	uint8_t buf[30] = {0};

	switch (pMsg->MsgId)
	{
		case WM_PAINT:			
			GUI_SetColor(gCfgItems.state_background_color);
			GUI_DrawRect(LCD_WIDTH/4, 0, LCD_WIDTH *3 / 4 - 4, imgHeight /2 - 2);
			GUI_SetColor(gCfgItems.state_background_color);
			GUI_FillRect(LCD_WIDTH/4, 0, LCD_WIDTH *3 / 4 - 4, imgHeight /2 - 2);			
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
					if(last_disp_state != PAUSE_UI)
					{
						Get_Temperature_Flg = 0;
					}
					last_disp_state = EXTRUSION_UI;
					Clear_extrusion();
					draw_return_ui();
				}
				else if(pMsg->hWinSrc == buttonStepForward)
				{
				
					/////printf(RELATIVE_COORD_COMMAN);
					pushFIFO(&gcodeCmdTxFIFO, RELATIVE_COORD_COMMAN);
					
					if(gCfgItems.sprayerNum == 2)
					{							
						sprintf(buf,"T%d\n",gCfgItems.curSprayerChoose);
						pushFIFO(&gcodeCmdTxFIFO, buf);
						memset(buf,0,sizeof(buf));
						sprintf(buf, "G1 E%d F%d\n", gCfgItems.extruStep, 60 * gCfgItems.extruSpeed);
						pushFIFO(&gcodeCmdTxFIFO, buf);	
					}
					else
					{
						MOVE_E_COMMAN(buf, gCfgItems.extruStep, 60 * gCfgItems.extruSpeed);
						pushFIFO(&gcodeCmdTxFIFO, buf);
					} 	
					///////printf(buf);
					//pushFIFO(&gcodeCmdTxFIFO, buf);
					////////printf(ABSOLUTE_COORD_COMMAN);
					pushFIFO(&gcodeCmdTxFIFO, ABSOLUTE_COORD_COMMAN);

					extructAmount += gCfgItems.extruStep;
					disp_extru_amount();
					//移动后马上保存数据
					if(printerStaus== pr_pause)
						positionSaveFlag = 1;		
				}
				else if(pMsg->hWinSrc == buttonStepBack)
				{
					//////////printf(RELATIVE_COORD_COMMAN);
					pushFIFO(&gcodeCmdTxFIFO, RELATIVE_COORD_COMMAN);

					if(gCfgItems.sprayerNum == 2)
					{
						sprintf(buf,"T%d\n",gCfgItems.curSprayerChoose);
						pushFIFO(&gcodeCmdTxFIFO, buf);
						memset(buf,0,sizeof(buf));
						sprintf(buf, "G1 E%d F%d\n",0 - gCfgItems.extruStep, 60 * gCfgItems.extruSpeed);						
						pushFIFO(&gcodeCmdTxFIFO, buf);
					}
					else
					{
						MOVE_E_COMMAN(buf, 0 - gCfgItems.extruStep, 60 * gCfgItems.extruSpeed);
						pushFIFO(&gcodeCmdTxFIFO, buf);
					}					
					////printf(buf);
					//pushFIFO(&gcodeCmdTxFIFO, buf);
					////////printf(ABSOLUTE_COORD_COMMAN);
					pushFIFO(&gcodeCmdTxFIFO, ABSOLUTE_COORD_COMMAN);

					extructAmount -= gCfgItems.extruStep;
					disp_extru_amount();
					//移动后马上保存数据
					if(printerStaus== pr_pause)
						positionSaveFlag = 1;		
				}
				else if(pMsg->hWinSrc == buttonSprayType)
				{
					if(gCfgItems.sprayerNum == 2)
					{
						if(gCfgItems.curSprayerChoose == 0)
							gCfgItems.curSprayerChoose = 1;
						else
							gCfgItems.curSprayerChoose = 0;
					}
					else
					{
						gCfgItems.curSprayerChoose = 0;
					}
					
					extructAmount = 0;
					disp_sprayer_temp();

					disp_sprayer_type();
					
					disp_extru_amount();
					
				}
				else if(pMsg->hWinSrc == buttonStep)
				{					
					switch(gCfgItems.extruStep)
					{
						case 1:
							gCfgItems.extruStep = 5;
							break;

						case 5:
							gCfgItems.extruStep = 10;
							break;
							
						case 10:
							gCfgItems.extruStep = 1;
							break;

						default:
							break;
					}
					disp_extru_step();
				}
				else if(pMsg->hWinSrc == buttonSpeed)
				{					
					switch(gCfgItems.extruSpeed)
					{
						case 1:
							gCfgItems.extruSpeed = 10;
							break;

						case 10:
							gCfgItems.extruSpeed = 20;
							break;

						case 20:
							gCfgItems.extruSpeed = 1;
							break;
						
						default:
							break;
					}
					disp_extru_speed();
				}
			}
			
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}


void draw_extrusion()
{
#if 1		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != EXTRUSION_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = EXTRUSION_UI;
	}
	disp_state = EXTRUSION_UI;
		
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

	//GUI_DispStringAt("准备打印->挤出", 0, 0);
	GUI_DispStringAt(creat_title_text(), 0, 0);
	
	hExtrusionWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbExtrusionWin, 0);
/*
	buttonStepForward = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hExtrusionWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonStepBack = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hExtrusionWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	
	
	buttonSprayType = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hExtrusionWnd, BUTTON_CF_SHOW, 0, 303);
	buttonStep = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hExtrusionWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonSpeed = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hExtrusionWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hExtrusionWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
*/
	buttonStepForward = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hExtrusionWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonStepBack = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hExtrusionWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	
	
	buttonSprayType = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hExtrusionWnd, BUTTON_CF_SHOW, 0, 303);
	buttonStep = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hExtrusionWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonSpeed = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hExtrusionWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hExtrusionWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
#if 0
	//TEXT_SetDefaultFont(&FONT_STATE_INF);
	if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		TEXT_SetDefaultFont(&GUI_FontHZ16);
	}
	else
	{
		TEXT_SetDefaultFont(&FONT_TITLE);
	}
#endif

	#if VERSION_WITH_PIC	
	BUTTON_SetBmpFileName(buttonStepForward, "bmp_in.bin",1);
	BUTTON_SetBmpFileName(buttonStepBack, "bmp_out.bin",1);
	
	
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);
	
	BUTTON_SetBitmapEx(buttonStepForward, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonStepBack, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonSpeed, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonStep, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetText(buttonStepForward,extrude_menu.in);
		BUTTON_SetText(buttonStepBack,extrude_menu.out);
		BUTTON_SetText(buttonRet,common_menu.text_back);	
	}
	
	#endif

	textExtruAmount = TEXT_CreateEx(LCD_WIDTH  / 4, 0, LCD_WIDTH / 2-2, imgHeight / 4, hExtrusionWnd, WM_CF_SHOW, TEXT_CF_LEFT,	GUI_ID_TEXT2, "总挤出量");

	textPrintTemp = TEXT_CreateEx(LCD_WIDTH  / 4, imgHeight / 4, LCD_WIDTH / 2-2, imgHeight / 4, hExtrusionWnd, WM_CF_SHOW, TEXT_CF_LEFT,	GUI_ID_TEXT1, "喷头温度");

	TEXT_SetTextAlign(textExtruAmount,  GUI_TA_VCENTER | GUI_TA_HCENTER);
	TEXT_SetTextAlign(textPrintTemp,  GUI_TA_VCENTER | GUI_TA_HCENTER);
	
	extructAmount = 0;
	
	disp_sprayer_temp();
	disp_extru_amount();
	disp_sprayer_type();

	disp_extru_speed();
	disp_extru_step();
	
	//GUI_Exec();

#endif
	
}

void disp_extru_step()
{
	//int8_t buf[30] = {0};

	//sprintf(buf, "%d mm", gCfgItems.extruStep);

	if(gCfgItems.extruStep == 1)
		BUTTON_SetBmpFileName(buttonStep, "bmp_step1_mm.bin",0);
	else if(gCfgItems.extruStep == 5)
		BUTTON_SetBmpFileName(buttonStep, "bmp_step5_mm.bin",0);
	else if(gCfgItems.extruStep == 10)
		BUTTON_SetBmpFileName(buttonStep, "bmp_step10_mm.bin",0);
	
	if(gCfgItems.multiple_language != 0)	
	{
			if(gCfgItems.extruStep == 1)
				BUTTON_SetText(buttonStep,extrude_menu.step_1mm);	
			else if(gCfgItems.extruStep == 5)
				BUTTON_SetText(buttonStep,extrude_menu.step_5mm);
			else if(gCfgItems.extruStep == 10)
				BUTTON_SetText(buttonStep,extrude_menu.step_10mm);		
	}
}

void disp_extru_speed()
{
	//int8_t buf[30] = {0};
		if(gCfgItems.extruSpeed == 20)
			BUTTON_SetBmpFileName(buttonSpeed, "bmp_speed_high.bin",0);
		else if(gCfgItems.extruSpeed == 1)
			BUTTON_SetBmpFileName(buttonSpeed, "bmp_speed_slow.bin",0);
		else
			BUTTON_SetBmpFileName(buttonSpeed, "bmp_speed_normal.bin",0);
		
		if(gCfgItems.multiple_language != 0)
		{
			if(gCfgItems.extruSpeed == 20)
				BUTTON_SetText(buttonSpeed,extrude_menu.high);
			else if(gCfgItems.extruSpeed == 1)	
				BUTTON_SetText(buttonSpeed,extrude_menu.low);
			else
				BUTTON_SetText(buttonSpeed,extrude_menu.normal);	
		}


}

void disp_sprayer_type()
{
	//#ifdef SPRAYER_NUM_LIMIT		
	//if(gCfgItems.sprayerNum == 2)
	//#endif
	//{
	if(gCfgItems.curSprayerChoose == 0)
	{
		#if VERSION_WITH_PIC
		BUTTON_SetBmpFileName(buttonSprayType, "bmp_extru1.bin",0);	
		BUTTON_SetBitmapEx(buttonSprayType, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		#endif
	}
	else if(gCfgItems.curSprayerChoose == 1)
	{
		#if VERSION_WITH_PIC
		BUTTON_SetBmpFileName(buttonSprayType, "bmp_extru2.bin",0);	
		BUTTON_SetBitmapEx(buttonSprayType, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		#endif
	}
	if(gCfgItems.multiple_language != 0)
	{
			if(gCfgItems.curSprayerChoose == 0)
					BUTTON_SetText(buttonSprayType,extrude_menu.ext1);
			else if(gCfgItems.curSprayerChoose == 1)
					BUTTON_SetText(buttonSprayType,extrude_menu.ext2); 
	}
}


void disp_sprayer_temp()
{
	int8_t buf[30] = {0};

	TEXT_SetTextColor(textPrintTemp, gCfgItems.state_text_color);
	TEXT_SetBkColor(textPrintTemp, gCfgItems.state_background_color);
#if 0
	//#ifdef SPRAYER_NUM_LIMIT		
	//if(gCfgItems.sprayerNum == 2)
	//#endif
	//{
		
	if(gCfgItems.language == LANG_ENGLISH)
	{
		sprintf(buf, "T: %d", (int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose]);

	}
	else 	if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		sprintf(buf, "溫度: %d", (int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose]);

	}
	else
	{
		sprintf(buf, "温度: %d", (int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose]);
	}
	//}
#endif
	//HC-chen
	if(gCfgItems.single_nozzle==0)
	{
		sprintf(buf, "Temp.: %d", (int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose]); 	
	}
	else
	{
		sprintf(buf, "Temp.: %d", (int)gCfgItems.curSprayerTemp[0]); 	
	}
					

	TEXT_SetText(textPrintTemp, buf);
}

void disp_extru_amount()
{
	int8_t buf[30] = {0};

	TEXT_SetTextColor(textExtruAmount, gCfgItems.state_text_color);
	TEXT_SetBkColor(textExtruAmount, gCfgItems.state_background_color);	
/*
	if(gCfgItems.language == LANG_ENGLISH)
	{
		if(extructAmount < 999 && extructAmount > -99)
			sprintf(buf, "E%d:%dmm",  gCfgItems.curSprayerChoose + 1, extructAmount);	
		else if(extructAmount < 9999 && extructAmount > -999)
			sprintf(buf, "E%d:%dcm",  gCfgItems.curSprayerChoose + 1, extructAmount/10);	
		else
			sprintf(buf, "E%d:%d m",  gCfgItems.curSprayerChoose + 1, extructAmount/1000);
	}
	else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		if(extructAmount < 999 && extructAmount > -99)
			sprintf(buf, "噴頭%d:%dmm",  gCfgItems.curSprayerChoose + 1, extructAmount);	
		else if(extructAmount < 9999 && extructAmount > -999)
			sprintf(buf, "噴頭%d:%dcm",  gCfgItems.curSprayerChoose + 1, extructAmount/10);	
		else
			sprintf(buf, "噴頭%d:%d m",  gCfgItems.curSprayerChoose + 1, extructAmount/1000);
	}
	else
	{
		if(extructAmount < 999 && extructAmount > -99)
			sprintf(buf, "喷头%d:%dmm", gCfgItems.curSprayerChoose + 1, extructAmount);	
		else if(extructAmount < 9999 && extructAmount > -999)
			sprintf(buf, "喷头%d:%dcm",  gCfgItems.curSprayerChoose + 1, extructAmount/10);	
		else
			sprintf(buf, "喷头%d:%d m",  gCfgItems.curSprayerChoose + 1, extructAmount/1000);
		
	}
	*/
	if(extructAmount < 999 && extructAmount > -99)
		sprintf(buf, "E%d:%dmm",	gCfgItems.curSprayerChoose + 1, extructAmount); 
	else if(extructAmount < 9999 && extructAmount > -999)
		sprintf(buf, "E%d:%dcm",	gCfgItems.curSprayerChoose + 1, extructAmount/10);	
	else
		sprintf(buf, "E%d:%dm",	gCfgItems.curSprayerChoose + 1, extructAmount/1000);

	TEXT_SetText(textExtruAmount, buf);
}


void Clear_extrusion()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hExtrusionWnd))
	{
		WM_DeleteWindow(hExtrusionWnd);
		GUI_Exec();
	}
	//GUI_Clear();
}



