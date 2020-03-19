#include "gui.h"
#include "button.h"
#include "draw_filamentchange.h"
#include "draw_ui.h"
#include "printer.h"
#include "text.h"
#include "gcode.h"

GUI_HWIN hFilamentChangeWnd;
static TEXT_Handle textExtruTemp, textExtruMsg;

static BUTTON_Handle buttonFilamentChangeIn, buttonFilamentChangeOut, buttonSprayType,buttonFilamentChangepreheat,buttonFilamentChangeStop, buttonRet;

extern volatile uint8_t get_temp_flag;

static uint8_t filamentchange_Process = 0;

extern uint8_t link_mutex_detect_time;

uint8_t filament_operation_menu_flg;
uint8_t filament_pause_menu_flg;
uint8_t filament_set_menu_flg;

void disp_filament_sprayer_type();
void disp_filament_sprayer_temp();

void disp_filament_wait_tips();

uint8_t Filament_in_flg;
uint8_t Filament_out_flg;
uint8_t filament_loading_complete;
uint8_t filament_heating_flg;
uint8_t filament_loading_flg;
uint8_t filament_load_cmd_flg;
uint8_t filament_load_timing_flg;
uint16_t filament_load_timing_cnt;

uint8_t filament_load_heat_flg;
uint8_t filament_unload_heat_flg;

extern uint8_t pause_flag;

static void cbFilamentChangeWin(WM_MESSAGE * pMsg) {

	uint8_t buf[50] = {0};

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
					//if((filament_loading_flg!=1)&&(filament_heating_flg != 1))
					//{
						#if 0
						if((last_disp_state == OPERATE_UI))
						{
							I2C_EE_Init(400000);
							start_print_time();
							printerStaus = pr_working;
						}
						
						if(last_disp_state != PAUSE_UI)
						{
							Get_Temperature_Flg = 0;
						}
						#endif
						/*
						if(printerStaus== pr_idle)
						{
							initFIFO(&gcodeCmdTxFIFO);
							sprintf(buf,"M104 T%d S%.f\n",gCfgItems.curSprayerChoose,gCfgItems.desireSprayerTempBak_1[gCfgItems.curSprayerChoose]);
							pushFIFO(&gcodeCmdTxFIFO,buf);
							
						}
						*/
						#if 0
						if((printerStaus== pr_pause)&&(pause_flag != 1))
						{
							pause_flag = 0;
							I2C_EE_Init(400000);
							start_print_time();
							printerStaus = pr_working;
						}
						#endif						
						if(gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]<gCfgItems.desireSprayerTempBak_1[gCfgItems.curSprayerChoose])
						{
							sprintf(buf,"M104 T%d S%.f\n",gCfgItems.curSprayerChoose,gCfgItems.desireSprayerTempBak_1[gCfgItems.curSprayerChoose]);
							pushFIFO(&gcodeCmdTxFIFO,buf);
						}
						gCfgItems.desireSprayerTempBak_1[gCfgItems.curSprayerChoose] = 0;						
						last_disp_state = FILAMENTCHANGE_UI;
						Clear_FilamentChange();
						if(printerStaus == pr_idle)
							draw_return_ui();
						else
							draw_pause();
					//}
					//else
					//{
					//	disp_filament_wait_tips();
					//}
				}
				else if(pMsg->hWinSrc == buttonFilamentChangeIn)
				{
					if(gcodeCmdTxFIFO.count <= (FIFO_NODE-4))
					{
						filament_load_heat_flg=1;
						//不支持双进一出
						if(gCfgItems.single_nozzle==0)
						{
							if((abs((int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose] - gCfgItems.filament_limit_temper)<=1)
								||((int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose] >= gCfgItems.filament_limit_temper))
							{
								last_disp_state = FILAMENTCHANGE_UI;
								Clear_FilamentChange();
								draw_dialog(DIALOG_TYPE_FILAMENT_HEAT_LOAD_COMPLETED);
							}
							else
							{
								last_disp_state = FILAMENTCHANGE_UI;
								Clear_FilamentChange();
								draw_dialog(DIALOG_TYPE_FILAMENT_LOAD_HEAT);

								if(gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]<gCfgItems.filament_limit_temper)
								{
									gCfgItems.desireSprayerTempBak_1[gCfgItems.curSprayerChoose] = gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose];
									if(printerStaus== pr_idle)
										initFIFO(&gcodeCmdTxFIFO);
									memset(buf,0,sizeof(buf));
									sprintf(buf,"M104 T%d S%d\n",gCfgItems.curSprayerChoose,gCfgItems.filament_limit_temper);
									pushFIFO(&gcodeCmdTxFIFO,buf);
									get_temp_flag = 1;
									link_mutex_detect_time = 5;
								}
								filament_sprayer_temp();
							}
						}
						//支持
						else
						{
							if((abs((int)gCfgItems.curSprayerTemp[0] - gCfgItems.filament_limit_temper)<=1)
								||((int)gCfgItems.curSprayerTemp[0] >= gCfgItems.filament_limit_temper))
							{
								last_disp_state = FILAMENTCHANGE_UI;
								Clear_FilamentChange();
								draw_dialog(DIALOG_TYPE_FILAMENT_HEAT_LOAD_COMPLETED);
							}
							else
							{
								last_disp_state = FILAMENTCHANGE_UI;
								Clear_FilamentChange();
								draw_dialog(DIALOG_TYPE_FILAMENT_LOAD_HEAT);

								if(gCfgItems.desireSprayerTemp[0]<gCfgItems.filament_limit_temper)
								{
									gCfgItems.desireSprayerTempBak_1[0] = gCfgItems.desireSprayerTemp[0];
									if(printerStaus== pr_idle)
										initFIFO(&gcodeCmdTxFIFO);
									memset(buf,0,sizeof(buf));
									sprintf(buf,"M104 S%d\n",gCfgItems.filament_limit_temper);
									pushFIFO(&gcodeCmdTxFIFO,buf);
									get_temp_flag = 1;
									link_mutex_detect_time = 5;
								}
								filament_sprayer_temp();
							}
						}
					}
				}
				else if(pMsg->hWinSrc == buttonFilamentChangeOut)
				{
					if(gcodeCmdTxFIFO.count <= (FIFO_NODE-4))
					{
						filament_unload_heat_flg=1;
						if(gCfgItems.single_nozzle==0)
						{
							if((abs((int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose] - gCfgItems.filament_limit_temper)<=1)
								||((int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose] >= gCfgItems.filament_limit_temper))
							{
								last_disp_state = FILAMENTCHANGE_UI;
								Clear_FilamentChange();
								draw_dialog(DIALOG_TYPE_FILAMENT_HEAT_UNLOAD_COMPLETED);
							}
							else
							{
								last_disp_state = FILAMENTCHANGE_UI;
								Clear_FilamentChange();
								draw_dialog(DIALOG_TYPE_FILAMENT_UNLOAD_HEAT);
								
								if(gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]<gCfgItems.filament_limit_temper)
								{
									if(last_disp_state == SET_UI)
										initFIFO(&gcodeCmdTxFIFO);
									sprintf(buf,"M104 T%d S%d\n",gCfgItems.curSprayerChoose,gCfgItems.filament_limit_temper);
									pushFIFO(&gcodeCmdTxFIFO,buf);
									get_temp_flag = 1;
									link_mutex_detect_time = 5;
								}
								filament_sprayer_temp();
							}
						}
						else
						{
							if((abs((int)gCfgItems.curSprayerTemp[0] - gCfgItems.filament_limit_temper)<=1)
								||((int)gCfgItems.curSprayerTemp[0] >= gCfgItems.filament_limit_temper))
							{
								last_disp_state = FILAMENTCHANGE_UI;
								Clear_FilamentChange();
								draw_dialog(DIALOG_TYPE_FILAMENT_HEAT_UNLOAD_COMPLETED);
							}
							else
							{
								last_disp_state = FILAMENTCHANGE_UI;
								Clear_FilamentChange();
								draw_dialog(DIALOG_TYPE_FILAMENT_UNLOAD_HEAT);
								
								if(gCfgItems.desireSprayerTemp[0]<gCfgItems.filament_limit_temper)
								{
									if(last_disp_state == SET_UI)
										initFIFO(&gcodeCmdTxFIFO);
									sprintf(buf,"M104 S%d\n",gCfgItems.filament_limit_temper);
									pushFIFO(&gcodeCmdTxFIFO,buf);
									get_temp_flag = 1;
									link_mutex_detect_time = 5;
								}
								filament_sprayer_temp();
							}
						}
					}
					
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
					
					disp_filament_sprayer_temp();
					disp_filament_sprayer_type();
				}
			}	
				
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

void draw_FilamentChange()
{
	int8_t buf[100] = {0};
	link_mutex_detect_time = 5;
	Get_Temperature_Flg = 1;
	get_temp_flag = 1;
	
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != FILAMENTCHANGE_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = FILAMENTCHANGE_UI;
	}
	disp_state = FILAMENTCHANGE_UI;
		
	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
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

	hFilamentChangeWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbFilamentChangeWin, 0);

	buttonFilamentChangeIn = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hFilamentChangeWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonFilamentChangeOut = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hFilamentChangeWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonSprayType = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hFilamentChangeWnd, BUTTON_CF_SHOW, 0, 303);
	//buttonFilamentChangepreheat = BUTTON_CreateEx(LCD_WIDTH / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hFilamentChangeWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	//buttonFilamentChangeStop = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hFilamentChangeWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hFilamentChangeWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

	BUTTON_SetBmpFileName(buttonFilamentChangeIn, "bmp_in.bin",1);
	BUTTON_SetBmpFileName(buttonFilamentChangeOut, "bmp_out.bin",1);
	//BUTTON_SetBmpFileName(buttonFilamentChangepreheat, "bmp_preHeat.bin",1);
	//BUTTON_SetBmpFileName(buttonFilamentChangeStop, "bmp_stop.bin",1);
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",1);	
	BUTTON_SetBitmapEx(buttonFilamentChangeIn, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonFilamentChangeOut, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	//BUTTON_SetBitmapEx(buttonFilamentChangepreheat, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	//BUTTON_SetBitmapEx(buttonFilamentChangeStop, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	
	textExtruTemp = TEXT_CreateEx(LCD_WIDTH  / 4, 0, LCD_WIDTH / 2-2, imgHeight / 4, hFilamentChangeWnd, WM_CF_SHOW, TEXT_CF_LEFT,	GUI_ID_TEXT2, "总挤出量");
	//textExtruMsg  = TEXT_CreateEx(LCD_WIDTH  / 4, imgHeight / 4, LCD_WIDTH / 2-2, imgHeight / 4, hFilamentChangeWnd, WM_CF_SHOW, TEXT_CF_LEFT,	GUI_ID_TEXT1, "喷头温度");
	if(gCfgItems.multiple_language != 0)
	{
			BUTTON_SetText(buttonFilamentChangeIn,filament_menu.in);
			BUTTON_SetText(buttonFilamentChangeOut,filament_menu.out);	
			BUTTON_SetText(buttonRet,common_menu.text_back);
	}

	
	//TEXT_SetTextAlign(textExtruMsg,  GUI_TA_VCENTER | GUI_TA_HCENTER);
	TEXT_SetTextAlign(textExtruTemp,  GUI_TA_VCENTER | GUI_TA_HCENTER);

	disp_filament_sprayer_temp();
	disp_filament_sprayer_type();
	//TEXT_SetTextColor(textExtruMsg, gCfgItems.state_text_color);
	//TEXT_SetBkColor(textExtruMsg, gCfgItems.state_background_color);
	//sprintf((char*)buf,filament_menu.ready_replace);
	//TEXT_SetText(textExtruMsg, buf);
}


void disp_filament_sprayer_temp()
{
	int8_t buf[20] = {0};

	TEXT_SetTextColor(textExtruTemp, gCfgItems.state_text_color);
	TEXT_SetBkColor(textExtruTemp, gCfgItems.state_background_color);
	//HC-chen
	//支持双进一出
	if(gCfgItems.single_nozzle==1)
	{
		sprintf(buf, "Temp.:%d/%d" ,(int)gCfgItems.curSprayerTemp[0],(int)gCfgItems.desireSprayerTemp[0]);
	}
	//不支持
	else
	{
		sprintf(buf, "E%d:%d/%d", gCfgItems.curSprayerChoose+1,(int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose],(int)gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
	}
	TEXT_SetText(textExtruTemp, buf);
}

void disp_filament_sprayer_type()
{
	
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
		{
			BUTTON_SetText(buttonSprayType,filament_menu.ext1);
		}
		else if(gCfgItems.curSprayerChoose == 1)
		{
			BUTTON_SetText(buttonSprayType,filament_menu.ext2);
		}		
	}
}
void Clear_FilamentChange()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hFilamentChangeWnd))
	{
		WM_DeleteWindow(hFilamentChangeWnd);
		GUI_Exec();
	}
	//GUI_Clear();
}

