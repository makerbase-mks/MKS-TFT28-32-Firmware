#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_pre_heat.h"
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

GUI_HWIN hPreHeatWnd;
static TEXT_Handle textDesireTemp;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static BUTTON_Handle buttonInc, buttonDec, buttonTempType, buttondegree, buttonOff, buttonRet;

static void cbPreHeatWin(WM_MESSAGE * pMsg) {
	uint8_t  buf[50] = {0};
	
	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetColor(gCfgItems.state_background_color);
			GUI_DrawRect(LCD_WIDTH/4, 0, LCD_WIDTH *3 / 4-4, imgHeight /2 - 2);
			GUI_SetColor(gCfgItems.state_background_color);
			GUI_FillRect(LCD_WIDTH/4, 0, LCD_WIDTH *3 / 4-4, imgHeight /2 - 2);
			break;
		case WM_TOUCH:
		 	
			break;
		case WM_TOUCH_CHILD:
			
			break;
			
		case WM_NOTIFY_PARENT:
			if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				if(pMsg->hWinSrc == buttonTempType)
				{
					if(gCfgItems.curTempType == 0)
					{
						//HC-chen
						if(gCfgItems.sprayerNum == 2 && gCfgItems.single_nozzle==0)
						{
							if(gCfgItems.curSprayerChoose == 0)
							{
								gCfgItems.curSprayerChoose = 1;
							}
							else if(gCfgItems.curSprayerChoose == 1)
							{
								if(gCfgItems.custom_bed_flag == 1)
								{
									gCfgItems.curTempType = 1;
								}
								else
								{
									gCfgItems.curTempType = 0;
									gCfgItems.curSprayerChoose = 0;
								}

							}
						}
						else
						{	
							if(gCfgItems.curSprayerChoose == 0)
							{
								if(gCfgItems.custom_bed_flag == 1)
								{
									gCfgItems.curTempType = 1;
								}
								else
								{
									gCfgItems.curTempType = 0;
								}
							}
						}
					
					}
					else if(gCfgItems.curTempType == 1)
					{
						gCfgItems.curSprayerChoose = 0;
						gCfgItems.curTempType = 0;
					}

						
					disp_desire_temp();
					disp_temp_type();
					
				}					
				else if(pMsg->hWinSrc == buttonInc)
				{
					if((last_disp_state == PRINT_READY_UI)||(last_disp_state == FILAMENTCHANGE_UI)||(last_disp_state == TOOL_UI))
					{
						if(gCfgItems.curTempType == 0)
						{
							gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose] += gCfgItems.stepHeat;
							//if((int)gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose] > MAX_SPRAYER_TEMP)
							if((int)gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose] > gCfgItems.preheat_max_desireSprayerTemp)
							{
								//gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose] = (float)MAX_SPRAYER_TEMP;
								gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose] = (float)gCfgItems.preheat_max_desireSprayerTemp;
							}
		
							if(gCfgItems.sprayerNum == 2)
							{
								if(gCfgItems.single_nozzle==1)
								{
									SET_EXTRUD_TEM_COMMAND(buf, gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
								}
								else
								{
									sprintf(buf, "M104 T%d S%.1f\n", gCfgItems.curSprayerChoose,gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
								}
								pushFIFO(&gcodeCmdTxFIFO, buf);
							}
							else
							{
								SET_EXTRUD_TEM_COMMAND(buf, gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
								pushFIFO(&gcodeCmdTxFIFO, buf);							
							}					
						}
						else
						{
							
							gCfgItems.preheat_desireBedTemp += gCfgItems.stepHeat;
							//if((int)gCfgItems.preheat_desireBedTemp > MAX_BED_TEMP)
							if((int)gCfgItems.preheat_desireBedTemp > gCfgItems.preheat_max_desireBedTemp)
							{
								//gCfgItems.preheat_desireBedTemp = (float)MAX_BED_TEMP;
								gCfgItems.preheat_desireBedTemp = (float)gCfgItems.preheat_max_desireBedTemp;
							}

							SET_BED_TEM_COMMAND(buf, gCfgItems.preheat_desireBedTemp);
							//////printf(buf);
							pushFIFO(&gcodeCmdTxFIFO, buf);//////printf(buf);
						}
					}			
					else
					{
							if(gCfgItems.curTempType == 0)
							{
								gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose] += gCfgItems.stepHeat;
								//if((int)gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose] > MAX_SPRAYER_TEMP)
								if((int)gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose] > gCfgItems.preheat_max_desireSprayerTemp)
								{
									//gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose] = (float)MAX_SPRAYER_TEMP;
									gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose] = (float)gCfgItems.preheat_max_desireSprayerTemp;
								}

			
								if(gCfgItems.sprayerNum == 2)
								{
									//HC-chen
									//双进一出不使用T0预热
									if(gCfgItems.single_nozzle==1)
									{
										SET_EXTRUD_TEM_COMMAND(buf, gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
									}
									else
									{
										sprintf(buf, "M104 T%d S%.1f\n", gCfgItems.curSprayerChoose,gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
									}
									
									pushFIFO(&gcodeCmdTxFIFO, buf);
								}
								else
								{
									SET_EXTRUD_TEM_COMMAND(buf, gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
									pushFIFO(&gcodeCmdTxFIFO, buf); 						
								} 				
							}
							else
							{
								
								gCfgItems.desireBedTemp += gCfgItems.stepHeat;
								//if((int)gCfgItems.desireBedTemp > MAX_BED_TEMP)
								if((int)gCfgItems.desireBedTemp > gCfgItems.preheat_max_desireBedTemp)
								{
									//gCfgItems.desireBedTemp = (float)MAX_BED_TEMP;
									gCfgItems.desireBedTemp = (float)gCfgItems.preheat_max_desireBedTemp;
								}

								SET_BED_TEM_COMMAND(buf, gCfgItems.desireBedTemp);
								//////printf(buf);
								pushFIFO(&gcodeCmdTxFIFO, buf);//////printf(buf);
							}
						}

					disp_desire_temp();
				}
				else if(pMsg->hWinSrc == buttonDec)
				{
					if((last_disp_state == PRINT_READY_UI)||(last_disp_state == FILAMENTCHANGE_UI)||(last_disp_state == TOOL_UI))
					{
						if(gCfgItems.curTempType == 0)
						{
								if((int)gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose] > gCfgItems.stepHeat + MIN_SPRAYER_TEMP)
								{
									gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose] -= gCfgItems.stepHeat;
									
								}
								else
								{
									gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose] = (float)MIN_SPRAYER_TEMP;
								}

							if(gCfgItems.sprayerNum == 2)
							{
								if(gCfgItems.single_nozzle==1)
								{
									SET_EXTRUD_TEM_COMMAND(buf, gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
								}
								else
								{
									sprintf(buf, "M104 T%d S%.1f\n", gCfgItems.curSprayerChoose,gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
								}
								pushFIFO(&gcodeCmdTxFIFO, buf);
							}
							else
							{
								SET_EXTRUD_TEM_COMMAND(buf, gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
								pushFIFO(&gcodeCmdTxFIFO, buf);							
							}						
						}
						
						else
						{
						
							
							if((int)gCfgItems.preheat_desireBedTemp > gCfgItems.stepHeat + MIN_BED_TEMP)
							{
								gCfgItems.preheat_desireBedTemp -= gCfgItems.stepHeat;							
							}
							else
							{
								gCfgItems.preheat_desireBedTemp = (float)MIN_BED_TEMP;
							}
							
						
							SET_BED_TEM_COMMAND(buf, gCfgItems.preheat_desireBedTemp);
							//////printf(buf);
							pushFIFO(&gcodeCmdTxFIFO, buf);//////printf(buf);
						
							
						}
					}
					else
					{
						if(gCfgItems.curTempType == 0)
						{
								if((int)gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose] > gCfgItems.stepHeat + MIN_SPRAYER_TEMP)
								{
									gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose] -= gCfgItems.stepHeat;
									
								}
								else
								{
									gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose] = (float)MIN_SPRAYER_TEMP;
								}

							if(gCfgItems.sprayerNum == 2)
							{
								if(gCfgItems.single_nozzle==1)
								{
									sprintf(buf, "M104 T%d S%.1f\n", gCfgItems.curSprayerChoose,gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
								}
								else
								{
									SET_EXTRUD_TEM_COMMAND(buf, gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
								}
								pushFIFO(&gcodeCmdTxFIFO, buf);
							}
							else
							{
								SET_EXTRUD_TEM_COMMAND(buf, gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
								pushFIFO(&gcodeCmdTxFIFO, buf);							
							}						
						}
						
						else
						{
							if((int)gCfgItems.desireBedTemp > gCfgItems.stepHeat + MIN_BED_TEMP)
							{
								gCfgItems.desireBedTemp -= gCfgItems.stepHeat;							
							}
							else
							{
								gCfgItems.desireBedTemp = (float)MIN_BED_TEMP;
							}
							
							SET_BED_TEM_COMMAND(buf, gCfgItems.desireBedTemp);
							//////printf(buf);
							pushFIFO(&gcodeCmdTxFIFO, buf);//////printf(buf);
						}
					}						
					disp_desire_temp();
				}
				
				else if(pMsg->hWinSrc == buttondegree)
				{
					switch(gCfgItems.stepHeat)
					{
						case 1:
							gCfgItems.stepHeat = 5;
							break;

						case 5:
							gCfgItems.stepHeat = 10;
							break;
							
						case 10:
							gCfgItems.stepHeat = 1;
							break;

						default:
							break;
					}
					disp_step_heat();
				}
				else if(pMsg->hWinSrc == buttonOff)
				{
					if((last_disp_state == PRINT_READY_UI)||(last_disp_state == FILAMENTCHANGE_UI)||(last_disp_state == TOOL_UI))
					{
						if(gCfgItems.curTempType == 0)
						{

							gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose] = 0;
							if(gCfgItems.sprayerNum == 2)
							{
								if(gCfgItems.single_nozzle==1)
								{
									SET_EXTRUD_TEM_COMMAND(buf, gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
								}
								else
								{
									sprintf(buf, "M104 T%d S%.1f\n", gCfgItems.curSprayerChoose,gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
								}
								pushFIFO(&gcodeCmdTxFIFO, buf);							
							}
							else
							{
								SET_EXTRUD_TEM_COMMAND(buf, gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
								pushFIFO(&gcodeCmdTxFIFO, buf);
							}						
						}
						else
						{
							
							gCfgItems.preheat_desireBedTemp = 0;
							
							SET_BED_TEM_COMMAND(buf, gCfgItems.preheat_desireBedTemp);
							//////printf(buf);
							pushFIFO(&gcodeCmdTxFIFO, buf);//////printf(buf);
						}
					}	
					else
					{
						if(gCfgItems.curTempType == 0)
						{

							gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose] = 0;
							if(gCfgItems.sprayerNum == 2)
							{
								if(gCfgItems.single_nozzle==1)
								{
									SET_EXTRUD_TEM_COMMAND(buf, gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
								}
								else
								{
									sprintf(buf, "M104 T%d S%.1f\n", gCfgItems.curSprayerChoose,gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
								}
								pushFIFO(&gcodeCmdTxFIFO, buf);							
							}
							else
							{
								SET_EXTRUD_TEM_COMMAND(buf, gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
								pushFIFO(&gcodeCmdTxFIFO, buf);
							}						
						}
						else
						{
							
							gCfgItems.desireBedTemp = 0;
							
							SET_BED_TEM_COMMAND(buf, gCfgItems.desireBedTemp);
							//////printf(buf);
							pushFIFO(&gcodeCmdTxFIFO, buf);//////printf(buf);
						}
					}
					disp_desire_temp();
				}
				
				else  if(pMsg->hWinSrc == buttonRet)
				{		
					if((last_disp_state != PAUSE_UI)&&(last_disp_state != OPERATE_UI))
					{
						Get_Temperature_Flg = 0;
					}
					Clear_preHeat();

					if(last_disp_state ==DIALOG_UI)
					{
						printerStaus = pr_reprint;
						disp_state_stack._disp_index = 0;
						memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
						disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINTING_UI;

						Get_Temperature_Flg = 0;
						draw_pause();
					}
					else
					{
						draw_return_ui();
					}
					
					
					
				}
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}


void draw_preHeat()
{
	#if 1
	int i;

	/*if(disp_state_stack._disp_state[disp_state_stack._disp_index] == OPERATE_UI)
	{
		GUI_DispStringAt("打印->操作->调整", 0, 0);
	}
	else
	{
		GUI_DispStringAt("准备打印->预热", 0, 0);
	}*/
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != PRE_HEAT_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = PRE_HEAT_UI;
	}
	disp_state = PRE_HEAT_UI;

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
	#endif
	
	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
	
	GUI_DispStringAt(creat_title_text(), 0, 0);	
	
	
	hPreHeatWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbPreHeatWin, 0);
/*
	buttonInc = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hPreHeatWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonDec = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hPreHeatWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	
	buttonTempType = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hPreHeatWnd, BUTTON_CF_SHOW, 0, 303);
	buttondegree = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hPreHeatWnd, BUTTON_CF_SHOW, 0, 304);
	buttonOff = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hPreHeatWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hPreHeatWnd, BUTTON_CF_SHOW, 0, 306);
*/
	buttonInc = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPreHeatWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonDec = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPreHeatWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	
	buttonTempType = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPreHeatWnd, BUTTON_CF_SHOW, 0, 303);
	buttondegree = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPreHeatWnd, BUTTON_CF_SHOW, 0, 304);
	buttonOff = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPreHeatWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPreHeatWnd, BUTTON_CF_SHOW, 0, 306);
	
	//TEXT_SetDefaultFont(&FONT_STATE_INF);
	
	#if VERSION_WITH_PIC	
	BUTTON_SetBmpFileName(buttonInc, "bmp_Add.bin",1);
	BUTTON_SetBmpFileName(buttonDec, "bmp_Dec.bin",1);
	
	BUTTON_SetBmpFileName(buttonOff, "bmp_speed0.bin",1);
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);
	
	BUTTON_SetBitmapEx(buttonInc, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonDec, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttondegree, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonOff, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	#endif
	//disp_fan_onoff();
	
	textDesireTemp = TEXT_CreateEx(LCD_WIDTH  / 4, 0, LCD_WIDTH / 2-2, imgHeight / 2-1, hPreHeatWnd, WM_CF_SHOW, TEXT_CF_LEFT,	GUI_ID_TEXT1, "喷头目标温度");
	TEXT_SetTextAlign(textDesireTemp,  GUI_TA_VCENTER | GUI_TA_HCENTER);
	// = TEXT_CreateEx(LCD_WIDTH  / 4 + 50, imgHeight / 2 - 30, LCD_WIDTH / 2 - 60, 22, hPreHeatWnd, WM_CF_SHOW, TEXT_CF_LEFT,	GUI_ID_TEXT1, "S:00:00:00");

	disp_desire_temp();

	disp_temp_type();

	disp_step_heat();
	
	//GUI_Exec();

		if(gCfgItems.multiple_language != 0)
		{
			BUTTON_SetText(buttonInc,preheat_menu.add);
			BUTTON_SetText(buttonDec,preheat_menu.dec);
			BUTTON_SetText(buttonOff,preheat_menu.off);
			BUTTON_SetText(buttonRet,common_menu.text_back);	
		}

#endif
	
}

void Clear_preHeat()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hPreHeatWnd))
	{
		WM_DeleteWindow(hPreHeatWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}


void disp_step_heat()
{
	if(gCfgItems.stepHeat == 1)
		BUTTON_SetBmpFileName(buttondegree, "bmp_step1_degree.bin",0);
	else if(gCfgItems.stepHeat == 5)
		BUTTON_SetBmpFileName(buttondegree, "bmp_step5_degree.bin",0);
	else if(gCfgItems.stepHeat == 10)
		BUTTON_SetBmpFileName(buttondegree, "bmp_step10_degree.bin",0);

	if(gCfgItems.multiple_language != 0)
	{
		if(gCfgItems.stepHeat == 1)
			BUTTON_SetText(buttondegree,preheat_menu.step_1c);	
		else if(gCfgItems.stepHeat == 5)
			BUTTON_SetText(buttondegree,preheat_menu.step_5c);
		else if(gCfgItems.stepHeat == 10)
			BUTTON_SetText(buttondegree,preheat_menu.step_10c);	

	}
	
}

void disp_desire_temp()
{
	int8_t buf[30] = {0};

	TEXT_SetTextColor(textDesireTemp, gCfgItems.state_text_color);
	TEXT_SetBkColor(textDesireTemp, gCfgItems.state_background_color);

	//HC-chen
	//防止挤出界面是选择E2
	if(gCfgItems.single_nozzle==1)
	{
		gCfgItems.curSprayerChoose = 0;
	}
	if(gCfgItems.curTempType == 0)
	{
		//#ifdef SPRAYER_NUM_LIMIT	
		//if(gCfgItems.sprayerNum == 2)
		//#endif
		//{
		if((last_disp_state == PRINT_READY_UI)||(last_disp_state == FILAMENTCHANGE_UI)||(last_disp_state == TOOL_UI))//预热界面为默认值
		{
			
			sprintf(buf, "E%d\n%d/%d", gCfgItems.curSprayerChoose+1, (int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose],  (int)gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
			#if 0
			if(gCfgItems.language == LANG_ENGLISH)
			{
				sprintf(buf, "E%d\n%d/%d", gCfgItems.curSprayerChoose+1, (int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose],  (int)gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
			}
			else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
			{
				sprintf(buf, "噴頭%d\n%d/%d", gCfgItems.curSprayerChoose+1, (int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose],  (int)gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
			}
			else
			{
				sprintf(buf, "喷头%d\n%d/%d", gCfgItems.curSprayerChoose+1, (int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose],  (int)gCfgItems.preheat_desireSprayerTemp[gCfgItems.curSprayerChoose]);
			}
			#endif
		}
		else
		{
			sprintf(buf, "E%d\n%d/%d", gCfgItems.curSprayerChoose+1, (int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose],  (int)gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
			#if 0
			if(gCfgItems.language == LANG_ENGLISH)
			{
				sprintf(buf, "E%d\n%d/%d", gCfgItems.curSprayerChoose+1, (int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose],  (int)gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
			}
			else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
			{
				sprintf(buf, "噴頭%d\n%d/%d", gCfgItems.curSprayerChoose+1, (int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose],  (int)gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);			
			}
			else
			{
				sprintf(buf, "喷头%d\n%d/%d", gCfgItems.curSprayerChoose+1, (int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose],  (int)gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
			}			
			#endif
		}
		
		//}
		//#ifdef SPRAYER_NUM
		//else
		//{
		//	if(gCfgItems.language == LANG_ENGLISH)
		//	{
		//		sprintf(buf, "E\n%d/%d", (int)gCfgItems.curSprayerTemp[0],  (int)gCfgItems.preheat_desireSprayerTemp[0]);
		//	}
		//	else
		//	{
		//		sprintf(buf, "喷头\n%d/%d", (int)gCfgItems.curSprayerTemp[0],  (int)gCfgItems.preheat_desireSprayerTemp[0]);
		//	}
		//}
		//#endif

		

	}
	else
	{
		if((last_disp_state == PRINT_READY_UI)||(last_disp_state == FILAMENTCHANGE_UI)||(last_disp_state == TOOL_UI))
		{
			if(gCfgItems.language != LANG_SPANISH)
			{
				sprintf(buf, "Bed\n%d/%d", (int)gCfgItems.curBedTemp,  (int)gCfgItems.preheat_desireBedTemp);
			}
			else
			{
				sprintf(buf, "Cama\n%d/%d", (int)gCfgItems.curBedTemp,  (int)gCfgItems.preheat_desireBedTemp);
			}
			#if 0
			if(gCfgItems.language == LANG_ENGLISH)
			{
				sprintf(buf, "Bed\n%d/%d", (int)gCfgItems.curBedTemp,  (int)gCfgItems.preheat_desireBedTemp);
			}
			else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
			{
				sprintf(buf, "熱床\n%d/%d", (int)gCfgItems.curBedTemp,  (int)gCfgItems.preheat_desireBedTemp);
			}
			else
			{
				sprintf(buf, "热床\n%d/%d", (int)gCfgItems.curBedTemp,  (int)gCfgItems.preheat_desireBedTemp);
			}
			#endif
		}
		else
		{
			if(gCfgItems.language != LANG_SPANISH)
			{
				sprintf(buf, "Bed\n%d/%d", (int)gCfgItems.curBedTemp,  (int)gCfgItems.desireBedTemp);
			}
			else
			{
				sprintf(buf, "Cama\n%d/%d", (int)gCfgItems.curBedTemp,  (int)gCfgItems.desireBedTemp);
			}
			#if 0
			if(gCfgItems.language == LANG_ENGLISH)
			{
				sprintf(buf, "Bed\n%d/%d", (int)gCfgItems.curBedTemp,  (int)gCfgItems.desireBedTemp);
			}
			else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
			{
				sprintf(buf, "熱床\n%d/%d", (int)gCfgItems.curBedTemp,  (int)gCfgItems.desireBedTemp);
			}
			else
			{
				sprintf(buf, "热床\n%d/%d", (int)gCfgItems.curBedTemp,  (int)gCfgItems.desireBedTemp);
			}		
			#endif
		}
	}
	
	TEXT_SetText(textDesireTemp, buf);
}

void disp_temp_type()
{
	
		
	if(gCfgItems.curTempType == 0)
	{		
		if(gCfgItems.curSprayerChoose == 1)
		{			
			#if VERSION_WITH_PIC
			BUTTON_SetBmpFileName(buttonTempType, "bmp_extru2.bin",0);
			BUTTON_SetBitmapEx(buttonTempType, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			#endif			
		}
		else
		{
			#if VERSION_WITH_PIC
			BUTTON_SetBmpFileName(buttonTempType, "bmp_extru1.bin",0);
			BUTTON_SetBitmapEx(buttonTempType, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			#endif
		}	
		if(gCfgItems.multiple_language != 0)
		{
			if(gCfgItems.curSprayerChoose == 1)
				BUTTON_SetText(buttonTempType,preheat_menu.ext2);
			else
				BUTTON_SetText(buttonTempType,preheat_menu.ext1);		
		}
	}
	else
	{	
		#if VERSION_WITH_PIC
		BUTTON_SetBmpFileName(buttonTempType, "bmp_bed.bin",0);
		BUTTON_SetBitmapEx(buttonTempType, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		#endif
		if(gCfgItems.multiple_language != 0)
		{
				BUTTON_SetText(buttonTempType,preheat_menu.hotbed);	
		}		
	}
	#if 0
	if(gCfgItems.multiple_language != 0)
	{
		switch(gCfgItems.language)
		{
			case LANG_SIMPLE_CHINESE:
					if(gCfgItems.curTempType == 0)
					{
						if(gCfgItems.curSprayerChoose == 1)
						{
							BUTTON_SetText(buttonTempType,EXTRUDER_2_TEXT_CN);
						}
						else
						{
							BUTTON_SetText(buttonTempType,EXTRUDER_1_TEXT_CN);
						}
					}
					else
					{
						BUTTON_SetText(buttonTempType,HEATBED_TEXT_CN);
					}
					
				break;
			case LANG_COMPLEX_CHINESE:
				if(gCfgItems.curTempType == 0)
				{
					if(gCfgItems.curSprayerChoose == 1)
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_2_TEXT_T_CN);
					}
					else
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_1_TEXT_T_CN);
					}
				}
				else
				{
					BUTTON_SetText(buttonTempType,HEATBED_TEXT_T_CN);
				}

				break;
			case LANG_ENGLISH:
				if(gCfgItems.curTempType == 0)
				{
					if(gCfgItems.curSprayerChoose == 1)
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_2_TEXT_EN);
					}
					else
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_1_TEXT_EN);
					}
				}
				else
				{
					BUTTON_SetText(buttonTempType,HEATBED_TEXT_EN);
				}

				break;

			case LANG_RUSSIAN:
				if(gCfgItems.curTempType == 0)
				{
					if(gCfgItems.curSprayerChoose == 1)
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_2_TEXT_RU);
					}
					else
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_1_TEXT_RU);
					}
				}
				else
				{
					BUTTON_SetText(buttonTempType,HEATBED_TEXT_RU);
				}

				break;	
			case LANG_SPANISH:
				if(gCfgItems.curTempType == 0)
				{
					if(gCfgItems.curSprayerChoose == 1)
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_2_TEXT_SP);
					}
					else
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_1_TEXT_SP);
					}
				}
				else
				{
					BUTTON_SetText(buttonTempType,HEATBED_TEXT_SP);
				}

				break;					
			#if defined (STM32F10X_HD)	
			case LANG_JAPAN:
				if(gCfgItems.curTempType == 0)
				{
					if(gCfgItems.curSprayerChoose == 1)
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_2_TEXT_JP);
					}
					else
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_1_TEXT_JP);
					}
				}
				else
				{
					BUTTON_SetText(buttonTempType,HEATBED_TEXT_JP);
				}

				break;			
			case LANG_GERMAN:
				if(gCfgItems.curTempType == 0)
				{
					if(gCfgItems.curSprayerChoose == 1)
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_2_TEXT_GN);
					}
					else
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_1_TEXT_GN);
					}
				}
				else
				{
					BUTTON_SetText(buttonTempType,HEATBED_TEXT_GN);
				}

				break;
			case LANG_FRENCH:
				if(gCfgItems.curTempType == 0)
				{
					if(gCfgItems.curSprayerChoose == 1)
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_2_TEXT_FR);
					}
					else
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_1_TEXT_FR);
					}
				}
				else
				{
					BUTTON_SetText(buttonTempType,HEATBED_TEXT_FR);
				}

				break;

			case LANG_GREEK:
				if(gCfgItems.curTempType == 0)
				{
					if(gCfgItems.curSprayerChoose == 1)
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_2_TEXT_GR);
					}
					else
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_1_TEXT_GR);
					}
				}
				else
				{
					BUTTON_SetText(buttonTempType,HEATBED_TEXT_GR);
				}

				break;	
			case LANG_TURKISH:
				if(gCfgItems.curTempType == 0)
				{
					if(gCfgItems.curSprayerChoose == 1)
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_2_TEXT_TU);
					}
					else
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_1_TEXT_TU);
					}
				}
				else
				{
					BUTTON_SetText(buttonTempType,HEATBED_TEXT_TU);
				}

				break;	
			case LANG_KOREAN:
				if(gCfgItems.curTempType == 0)
				{
					if(gCfgItems.curSprayerChoose == 1)
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_2_TEXT_KR);
					}
					else
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_1_TEXT_KR);
					}
				}
				else
				{
					BUTTON_SetText(buttonTempType,HEATBED_TEXT_KR);
				}

				break;
			
			case LANG_ITALY:
				if(gCfgItems.curTempType == 0)
				{
					if(gCfgItems.curSprayerChoose == 1)
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_2_TEXT_IT);
					}
					else
					{
						BUTTON_SetText(buttonTempType,EXTRUDER_1_TEXT_IT);
					}
				}
				else
				{
					BUTTON_SetText(buttonTempType,HEATBED_TEXT_IT);
				}

				break;
			#endif
		}
		}
	#endif
}

