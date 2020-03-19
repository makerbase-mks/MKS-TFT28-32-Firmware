#include "stdint.h"
#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_dialog.h"
#include "draw_ui.h"
#include "pic.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "usbh_usr.h"
#include "printer.h"
#include "printing_thread.h"
#include "text.h"
#include "checkbox.h"
#include "gcode.h"
#ifndef GUI_FLASH
#define GUI_FLASH
#endif
extern char cmd_code[201];

extern unsigned char codebuff[100];

extern FIL fp_reprint_rw;

extern uint8_t temp_update_flag;

extern unsigned char path_bak[15];
extern unsigned char *path_reprint;

extern volatile unsigned long BeeperFreq;
extern volatile unsigned char BeeperCnt;
extern volatile unsigned char mksBpAlrmEn;


extern uint8_t filament_loading_time_flg;
extern uint32_t filament_loading_time_cnt;
extern uint8_t filament_loading_completed;
extern uint8_t filament_load_heat_flg;
extern uint8_t filament_unload_heat_flg;
extern uint8_t filament_unloading_time_flg;
extern uint32_t filament_unloading_time_cnt;
extern uint8_t filament_unloading_completed;

extern uint8_t Chk_close_machine_flg;

static GUI_HWIN hStopDlgWnd;

extern int8_t curFilePath[30];

extern uint8_t gCurDir[100];


extern uint32_t rePrintOffset;

extern volatile int16_t logo_time;

extern uint32_t upload_time ;
extern uint32_t upload_size;

extern reset_tx_fifo();


uint8_t filament_heat_completed_load;
uint8_t filament_heat_completed_unload;


extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static TEXT_Handle printStopDlgText,filament_temper,printfilename;
static BUTTON_Handle buttonOk, buttonCancle;

//static CHECKBOX_Handle Chk_close_machine;

static PROGBAR_Handle FilamentBar;

uint32_t filament_rate;	
static uint8_t DialogType;

extern unsigned char SendOneTime;

extern volatile uint8_t get_temp_flag;
extern unsigned char breakpoint_homeFlag;

extern int upload_result ;
uint8_t empty_filament_tips_flag=0;

extern uint8_t pause_from_mt_high_level;
extern uint8_t pause_from_pw_high_level;



static void cbDlgWin(WM_MESSAGE * pMsg)
{
	int8_t sel_item;
	int8_t i;
	//int8_t sel_file[30];
	uint8_t i2c_cnt =  0;
	volatile uint8_t tmp = 0xff;
	uint8_t buf[20];
	
	//struct PressEvt *press_event;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		//GUI_SetBkColor(GUI_DARKBLUE);
		//GUI_Clear();
		break;

	case WM_TOUCH:		
		break;
		
	case WM_TOUCH_CHILD:
	 	break;

	case WM_NOTIFY_PARENT:
		if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
		{
			if(pMsg->hWinSrc == buttonOk)
			{			
				Clear_dialog();
				//hAlertWin = GUI_CreateDialogBox(aDialogAlert, GUI_COUNTOF(aDialogAlert), _cbAlert, 0, 0, 0);
				if(DialogType == DIALOG_TYPE_STOP)				
				{
					memset(gCfgItems.z_display_pos,0,sizeof(gCfgItems.z_display_pos));	
					breakpoint_homeFlag=0;
					gCfgItems.pwd_reprint_flg = 0;

					Get_Temperature_Flg = 0;
					
					if(printerStaus == pr_reprint)
					{
						//rePrintProcess();
						//清除EEPROM的数据
						for(i2c_cnt=0;i2c_cnt<(256-BAK_REPRINT_S);i2c_cnt++)
						{
							I2C_EE_BufferWrite(&tmp,BAK_REPRINT_S+i2c_cnt,1);
						}
					}
					
					printerStaus = pr_stop;
					stop_print_time();
					f_close(srcfp);
					#ifdef SAVE_FROM_SD
					//删除续打数据文件
					if(gCfgItems.pwroff_save_mode == 0)
					{
						if(gCfgItems.fileSysType == FILE_SYS_SD)
						{
							strcpy(path_bak, "1:");
							strcat(path_bak,path_reprint);						
							f_unlink(path_bak);
						}
						else
						{
							//strcpy(path_bak, "0:");
						}

					}
					#endif
					SPEAKER = 0;
					mksBpAlrmEn=0;
					BeeperCnt=0;
					BeeperFreq = 0;

					reset_file_info();
					reset_tx_fifo();
					draw_ready_print();
				}
				else if(DialogType == DIALOG_TYPE_PRINT_FILE)
				{
					//20150917
					I2C_EE_Init(400000);
					//第二次初始化，保证IIC能提速
					I2C_EE_Init(400000);

					//HC-chen 4.21
					/*
					gCfgItems.printSpeed = 100;
					SET_MOVE_SPEED_COMMAND(buf, gCfgItems.printSpeed);
					pushFIFO(&gcodeCmdTxFIFO, buf);
					gCfgItems.printExtSpeed0 = 100;
					SET_EXTRU_SPEED_COMMAND(buf, gCfgItems.printExtSpeed0);
					pushFIFO(&gcodeCmdTxFIFO, buf);
					*/
					gCfgItems.desireSprayerTemp[0] = 0;
					gCfgItems.desireSprayerTemp[1] = 0;
					gCfgItems.desireBedTemp = 0;	
					/*
					if(strlen(curFileName)>150)
					{
						draw_dialog(DIALOG_TYPE_MESSEGE_ERR1);
					}
					else
					*/
					{
						//chen
						//置0,防止恢复界面按停止
						pause_from_mt_high_level=0;
						pause_from_pw_high_level=0;
						//一开始打印没接料,提示界面标志位
						//打印10s后清0
						empty_filament_tips_flag=1;
						
						Get_Temperature_Flg = 1;
						get_temp_flag = 1;
						SendOneTime = 1;
						reset_print_time();
						start_print_time();
						draw_printing();
					}
					if(gCfgItems.pwd_reprint_flg == 1)
					{
						usart2Data.printer = printer_waiting;
						usart2Data.prWaitStatus == pr_wait_idle;
						printerStaus = breakpoint_reprint;
					}

				}
				else if(DialogType == DIALOG_TYPE_REPRINT_NO_FILE)
				{
					int8_t res;
					static FIL curFile;

					memset(gCurDir, 0, sizeof(gCurDir));

					if(gCfgItems.fileSysType == FILE_SYS_SD)
					{
						strcpy(gCurDir, "1:");
						SD_Initialize();
						ShowSDFiles();
					}
					else
					{
						strcpy(gCurDir, "0:");
					}
					
					logo_time = 3000 / TICK_CYCLE;;

					res = f_open(&curFile, curFileName, FA_OPEN_EXISTING | FA_READ);
					if(res == FR_OK)
					{
						srcfp = &curFile;
						gCurFileState.file_open_flag = 0xaa;
						gCurFileState.totalSend = (unsigned char)((float)rePrintOffset  * 100 / f_size(&curFile));
						

						//if((RE_PRINT_STATUS)gCfgItems.rePrintFlag == printer_pause_reprint)
						{
							//20150710
							//printerStaus = pr_pause;
							printerStaus = pr_reprint;
							disp_state_stack._disp_index = 0;
							memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
							disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINTING_UI;
							draw_pause();
							
						}
					/*	else if((RE_PRINT_STATUS)gCfgItems.rePrintFlag == printer_pwdwn_reprint)
						{
							printerStaus = pr_working;
							start_print_time();
							draw_printing();
						}*/
						
						logo_time = 3000 / TICK_CYCLE;
					}
					else
					{
						disp_state_stack._disp_index = 0;
						memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
						disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINT_READY_UI;
						draw_dialog(DIALOG_TYPE_REPRINT_NO_FILE);
						
					}
					
				}
				else if(DialogType == DIALOG_TYPE_REPRINT_NO_TEMP)
				{
					gCfgItems.desireSprayerTemp[0]=180;  //手动添加默认温度
					gCfgItems.reprint_no_temp_flag=1;
					Clear_dialog();
					printerStaus = pr_reprint;
					disp_state_stack._disp_index = 0;
					memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
					disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINTING_UI;
					draw_pause();
				}
				else if(DialogType == DIALOG_TYPE_UNBIND)
				{
					cloud_unbind();
					draw_return_ui();
				}
				else if(DialogType == DIALOG_TYPE_FILAMENT_HEAT_LOAD_COMPLETED)
				{
					//filament_heat_completed_load = 1;
					Clear_dialog();
					draw_dialog(DIALOG_TYPE_FILAMENT_LOADING);
					
					filament_loading_time_flg = 1;
					filament_loading_time_cnt = 0;

					//initFIFO(&gcodeCmdTxFIFO);
					pushFIFO(&gcodeCmdTxFIFO, RELATIVE_COORD_COMMAN);
					if(gCfgItems.sprayerNum == 2)
					{
							memset(buf,0,sizeof(buf));
							sprintf(buf,"T%d\n",gCfgItems.curSprayerChoose);
							pushFIFO(&gcodeCmdTxFIFO, buf);				
					}
					memset(buf,0,sizeof(buf));
					sprintf(buf,"G1 E%d F%d\n",gCfgItems.filament_load_length,gCfgItems.filament_load_speed);
					pushFIFO(&gcodeCmdTxFIFO,buf);
					pushFIFO(&gcodeCmdTxFIFO, ABSOLUTE_COORD_COMMAN);					

				}
				else if(DialogType == DIALOG_TYPE_FILAMENT_HEAT_UNLOAD_COMPLETED)
				{
					//filament_heat_completed_unload = 1;
					Clear_dialog();
					draw_dialog(DIALOG_TYPE_FILAMENT_UNLOADING);
					filament_unloading_time_flg = 1;
					filament_unloading_time_cnt = 0;	

					//initFIFO(&gcodeCmdTxFIFO);
					pushFIFO(&gcodeCmdTxFIFO, RELATIVE_COORD_COMMAN);
					if(gCfgItems.sprayerNum == 2)
					{
							memset(buf,0,sizeof(buf));
							sprintf(buf,"T%d\n",gCfgItems.curSprayerChoose);
							pushFIFO(&gcodeCmdTxFIFO, buf);				
					}
					//chen 9.10 先进gCfgItems.unload_retract_length
					memset(buf,0,sizeof(buf));
					sprintf(buf,"G1 E%d F%d\n",gCfgItems.unload_retract_length,gCfgItems.unload_retract_speed);
					pushFIFO(&gcodeCmdTxFIFO,buf);

					
					memset(buf,0,sizeof(buf));
					sprintf(buf,"G1 E-%d F%d\n",gCfgItems.filament_unload_length,gCfgItems.filament_unload_speed);
					pushFIFO(&gcodeCmdTxFIFO,buf);
					pushFIFO(&gcodeCmdTxFIFO, ABSOLUTE_COORD_COMMAN);						
				}	
				else if(DialogType == DIALOG_EMPTY_FILAMENT_TIPS)
				{
					last_disp_state=DIALOG_UI;
					Clear_dialog();
					draw_pause();
				}
				else
				{
					/*
					if(printerStaus == pr_idle)
					{
						if((DialogType == DIALOG_TYPE_FILAMENT_LOAD_HEAT)
							||(DialogType == DIALOG_TYPE_FILAMENT_UNLOAD_HEAT)
							||(DialogType == DIALOG_TYPE_FILAMENT_HEAT_LOAD_COMPLETED)
							||(DialogType == DIALOG_TYPE_FILAMENT_UNLOAD_COMPLETED))
						{
							sprintf(buf,"M104 T%d S%.f\n",gCfgItems.curSprayerChoose,gCfgItems.desireSprayerTempBak_1[gCfgItems.curSprayerChoose]);
							pushFIFO(&gcodeCmdTxFIFO,buf);

						}
					}
					*/
					draw_return_ui();
				}

			
			}
			else if(pMsg->hWinSrc == buttonCancle)
			{		
				unsigned int tmpFlag;//取消之后则对打完关机标志位清零，防止出错
				Chk_close_machine_flg = 0;
				if(DialogType == DIALOG_TYPE_REPRINT_NO_TEMP)
				{
					last_disp_state=DIALOG_UI;
					Get_Temperature_Flg = 1;
					get_temp_flag = 1;
					gCfgItems.reprint_no_temp_flag=1;
					Clear_dialog();
					draw_preHeat();
				}
				else
				{
					
					Clear_dialog();
					draw_return_ui();
					/*
					if(printerStaus == pr_idle)
					{
						if((DialogType == DIALOG_TYPE_FILAMENT_LOAD_HEAT)
							||(DialogType == DIALOG_TYPE_FILAMENT_UNLOAD_HEAT)
							||(DialogType == DIALOG_TYPE_FILAMENT_HEAT_LOAD_COMPLETED)
							||(DialogType == DIALOG_TYPE_FILAMENT_UNLOAD_COMPLETED))
						{
							sprintf(buf,"M104 T%d S%.f\n",gCfgItems.curSprayerChoose,gCfgItems.desireSprayerTempBak_1[gCfgItems.curSprayerChoose]);
							pushFIFO(&gcodeCmdTxFIFO,buf);

						}
					}
					*/

					if(DialogType == DIALOG_TYPE_REPRINT_NO_FILE)
					{	
					 #ifdef SAVE_FROM_SD
							//删除续打数据文件
							if(gCfgItems.pwroff_save_mode == 0)
							{
								if(gCfgItems.fileSysType == FILE_SYS_SD)
								{
									strcpy(path_bak, "1:");
									strcat(path_bak,path_reprint);						
									f_unlink(path_bak);
								}
								else
								{
									//strcpy(path_bak, "0:");
								}
							}
						#endif
						//draw_main_ui();	
						//logo_time = 0;
						I2C_EE_BufferRead(&tmpFlag, BAK_REPRINT_INFO,  4);
						tmpFlag &= ~(0xff << 24);
						tmpFlag |= printer_normal << 24;
						I2C_EE_BufferWrite(&tmpFlag, BAK_REPRINT_INFO,  4); 		//续打标志(uint8_t) | checkXor(uint8_t) | checkSum(uint16_t)
							
					}
				}
			}
		}
	}
}

void draw_dialog(uint8_t type)
{
		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	

	int8_t buf[50] = {0};
	
	int i;
#if 1
	if((DialogType == DIALOG_TYPE_MAINBOAR_ERROR)
		&& (type == DIALOG_TYPE_MAINBOAR_ERROR)
		&&(disp_state == DIALOG_UI) )
		{
			return;
		}
#endif

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != DIALOG_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = DIALOG_UI;
	}
	
	disp_state = DIALOG_UI;
	buttonOk = 0;
	buttonCancle = 0;

	DialogType = type;
		
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
	#endif


	//sprintf(buf, "正在打印->操作:%s", curFilePath);
	//GUI_DispStringAt(buf, 0, 0);
	if(disp_state_stack._disp_index > 1)
		GUI_DispStringAt(creat_title_text(), 0, 0);
	if(DialogType == DIALOG_TYPE_UPDATE_ESP_FIRMARE) //更新esp固件
	{
	//	if(gCfgItems.language == LANG_ENGLISH)
		{			
			//TEXT_SetDefaultFont(&FONT_TITLE);
			GUI_DispStringAt(DIALOG_UPDATE_WIFI_FIRMWARE_EN, 40, 120);
		}
/*		else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
		{
			 TEXT_SetDefaultFont(&GUI_FontHZ16);
			 GUI_DispStringAt( "正在更新wifi模块固件", 60, 120);
		}
		else
		{
			TEXT_SetDefaultFont(&FONT_TITLE);
			 GUI_DispStringAt( "正在更新wifi模块固件", 60, 120);
		   
		}*/
	}
	else if(DialogType == DIALOG_TYPE_UPDATE_ESP_DATA) //更新esp数据
	{
	//	if(gCfgItems.language == LANG_ENGLISH)
		{
			//TEXT_SetDefaultFont(&FONT_TITLE);
			GUI_DispStringAt(DIALOG_UPDATE_WIFI_WEB_EN, 40, 100);
		}
	/*	else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
		{
			 TEXT_SetDefaultFont(&GUI_FontHZ16);
			  GUI_DispStringAt( "正在更新wifi模块数据", 60, 100);
		}
		else
		{
			TEXT_SetDefaultFont(&FONT_TITLE);
			 GUI_DispStringAt( "正在更新wifi模块数据", 60, 100);
		}*/
	}
	else if(DialogType == DIALOG_TYPE_UPLOAD_FILE)// esp 上传文件
	{
		hStopDlgWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbDlgWin, 0);
		printStopDlgText = TEXT_CreateEx(0,0, LCD_WIDTH, 90, hStopDlgWnd, WM_CF_SHOW, TEXT_CF_LEFT,  alloc_win_id(), "Uploading......");
		TEXT_SetBkColor(printStopDlgText, gCfgItems.background_color);
		TEXT_SetTextColor(printStopDlgText, gCfgItems.title_color);
		TEXT_SetTextAlign(printStopDlgText, GUI_TA_VCENTER | GUI_TA_HCENTER);				
		if(upload_result == 1)
		{
		//	if(gCfgItems.language == LANG_ENGLISH)
			{				
				TEXT_SetText(printStopDlgText, DIALOG_UPLOAD_ING_EN);
			}
		/*	else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
			{
				TEXT_SetText(printStopDlgText, "正在传送.....");
			}
			else
			{
				TEXT_SetText(printStopDlgText, "正在传送.....");
			}*/
		}
		else if(upload_result == 2)
		{
			buttonOk = BUTTON_CreateEx(120,  90, 80, 40,hStopDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
			//if(gCfgItems.language == LANG_ENGLISH)
			{				
				TEXT_SetText(printStopDlgText, DIALOG_UPLOAD_ERROR_EN);
				//BUTTON_SetFont(buttonOk,&FONT_TITLE);
				BUTTON_SetText(buttonOk, DIALOG_CONFIRM_EN);
			}
			/*else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
			{
				TEXT_SetText(printStopDlgText, "文件传送失败");
	       		 BUTTON_SetFont(buttonOk,&GUI_FontHZ16);
				 BUTTON_SetText(buttonOk, "确定");
			}
			else
			{
				TEXT_SetText(printStopDlgText, "文件传送失败");
	       		 BUTTON_SetFont(buttonOk,&FONT_TITLE);
				 BUTTON_SetText(buttonOk, "确定");
			}*/
       
		}
		else if(upload_result == 3)
		{
			char buf[200];
			int _index = 0;
			memset(buf,0,sizeof(200));
			buttonOk = BUTTON_CreateEx(120,  90, 80, 40,hStopDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
			//if(gCfgItems.language == LANG_ENGLISH)
			{			
				strcpy(buf, DIALOG_UPLOAD_FINISH_EN);
				_index = strlen(buf);
				buf[_index] = '\n';
				_index++;
				strcat(buf, DIALOG_UPLOAD_SIZE_EN);
				
				_index = strlen(buf);
				buf[_index] = ':';
				_index++;
				sprintf(&buf[_index], "%.1d KBytes\n", upload_size / 1024);

				
				strcat(buf, DIALOG_UPLOAD_TIME_EN);
				_index = strlen(buf);
				buf[_index] = ':';
				_index++;
				sprintf(&buf[_index], "%d s\n", upload_time);
				
				strcat(buf, DIALOG_UPLOAD_SPEED_EN);
				_index = strlen(buf);
				buf[_index] = ':';
				_index++;
				sprintf(&buf[_index], "%d KBytes/s\n", upload_size / upload_time / 1024);				
				
				TEXT_SetText(printStopDlgText, buf);
				//BUTTON_SetFont(buttonOk,&FONT_TITLE);
				BUTTON_SetText(buttonOk, "OK");
			}
		
		}
	}
	#if 1
	else if(DialogType == DIALOG_TYPE_MAINBOAR_ERROR)
	{
		hStopDlgWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbDlgWin, 0);
		printStopDlgText = TEXT_CreateEx(0,0, LCD_WIDTH, 90, hStopDlgWnd, WM_CF_SHOW, TEXT_CF_LEFT,	alloc_win_id(), " ");
		TEXT_SetTextColor(printStopDlgText, gCfgItems.title_color);
		TEXT_SetTextAlign(printStopDlgText, GUI_TA_VCENTER | GUI_TA_HCENTER);		
		
		buttonOk = BUTTON_CreateEx(120, 90, 80, 40,hStopDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		if(strlen(cmd_code)>40)
		{
			memset(codebuff,'\0',sizeof(codebuff));
			strncpy(codebuff,cmd_code,40);
			codebuff[40]='\n';
			strcat(codebuff,&cmd_code[40]);
			TEXT_SetText(printStopDlgText, codebuff);	
		}
		else
		{
			TEXT_SetText(printStopDlgText, cmd_code);
		}		
		//TEXT_SetText(printStopDlgText, cmd_code);
		BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);
	}
	
	#endif
	else
	{
		hStopDlgWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbDlgWin, 0);
		printStopDlgText = TEXT_CreateEx(0,0, LCD_WIDTH, 90, hStopDlgWnd, WM_CF_SHOW, TEXT_CF_LEFT,	alloc_win_id(), " ");
		TEXT_SetBkColor(printStopDlgText, gCfgItems.background_color);

		TEXT_SetTextColor(printStopDlgText, gCfgItems.title_color);
		TEXT_SetTextAlign(printStopDlgText, GUI_TA_VCENTER | GUI_TA_HCENTER);

		if((DialogType == DIALOG_TYPE_M80_FAIL)
			||(DialogType == DIALOG_TYPE_FILAMENT_LOAD_COMPLETED)
			||(DialogType == DIALOG_TYPE_FILAMENT_UNLOAD_COMPLETED))
		{
			buttonOk = BUTTON_CreateEx(120,	90, 80, 40,hStopDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		}
		else	if(DialogType == DIALOG_TYPE_FILAMENT_LOAD_HEAT)
		{
			buttonCancle = BUTTON_CreateEx(120,  90,80, 40,hStopDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
			filament_temper = TEXT_CreateEx(135,60, LCD_WIDTH, 20, hStopDlgWnd, WM_CF_SHOW, TEXT_CF_LEFT,  alloc_win_id(), " ");
			TEXT_SetTextColor(filament_temper, gCfgItems.title_color);
			TEXT_SetBkColor(filament_temper, gCfgItems.background_color);
		}
		else if(DialogType == DIALOG_TYPE_FILAMENT_UNLOAD_HEAT)
		{
			buttonCancle = BUTTON_CreateEx(120,  90,80, 40,hStopDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
			filament_temper = TEXT_CreateEx(135,65, LCD_WIDTH, 20, hStopDlgWnd, WM_CF_SHOW, TEXT_CF_LEFT,  alloc_win_id(), " ");
			TEXT_SetTextColor(filament_temper, gCfgItems.title_color);
			TEXT_SetBkColor(filament_temper, gCfgItems.background_color);
		}
		else if((DialogType == DIALOG_TYPE_FILAMENT_LOADING)||(DialogType == DIALOG_TYPE_FILAMENT_UNLOADING))
		{
			FilamentBar = PROGBAR_CreateEx(70, 60, 180, 20, hStopDlgWnd, WM_CF_SHOW, 0, 0);
			PROGBAR_SetBarColor(FilamentBar, 0, GUI_GREEN);
			PROGBAR_SetValue(FilamentBar,filament_rate);
			PROGBAR_SetText(FilamentBar," ");
		}	
		else if(DialogType == DIALOG_TYPE_PRINT_FILE)
		{
			buttonOk = BUTTON_CreateEx(70,  90, 80, 40,hStopDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
			buttonCancle = BUTTON_CreateEx(180,  90,80, 40,hStopDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());		
			printStopDlgText = TEXT_CreateEx(0,0, LCD_WIDTH, 30, hStopDlgWnd, WM_CF_SHOW, TEXT_CF_HCENTER|TEXT_CF_BOTTOM, alloc_win_id(), " ");
			printfilename = TEXT_CreateEx(0,45, LCD_WIDTH, 40, hStopDlgWnd, WM_CF_SHOW, TEXT_CF_HCENTER|TEXT_CF_TOP,  alloc_win_id(), " ");
			TEXT_SetBkColor(printfilename, gCfgItems.background_color);
			TEXT_SetTextColor(printfilename, gCfgItems.title_color);
			TEXT_SetBkColor(printStopDlgText, gCfgItems.background_color);
			TEXT_SetTextColor(printStopDlgText, gCfgItems.title_color);			
		}
		//chen 9.10
		else if(DialogType == DIALOG_EMPTY_FILAMENT_TIPS)
		{
			
			TEXT_SetTextColor(printStopDlgText, gCfgItems.title_color);
			TEXT_SetTextAlign(printStopDlgText, GUI_TA_VCENTER | GUI_TA_HCENTER);		
			
			buttonOk = BUTTON_CreateEx(120, 90, 80, 40,hStopDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		}
		else		
		{
			buttonOk = BUTTON_CreateEx(70,  90, 80, 40,hStopDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
			buttonCancle = BUTTON_CreateEx(180,  90,80, 40,hStopDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		}
		//TEXT_SetBkColor(printStopDlgText, gCfgItems.background_color);

		//TEXT_SetTextColor(printStopDlgText, gCfgItems.title_color);
		//TEXT_SetTextAlign(printStopDlgText, GUI_TA_VCENTER | GUI_TA_HCENTER);

				if(DialogType == DIALOG_TYPE_STOP)
				{
					TEXT_SetText(printStopDlgText, print_file_dialog_menu.cancle_print);
					BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);
					BUTTON_SetText(buttonCancle, print_file_dialog_menu.cancle);
				}
				else if(DialogType == DIALOG_TYPE_PRINT_FILE)
				{
					if(gCfgItems.pwd_reprint_flg == 1)
					{
						TEXT_SetText(printStopDlgText, print_file_dialog_menu.print_from_breakpoint);
						BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);
						BUTTON_SetText(buttonCancle, print_file_dialog_menu.cancle);			
					}
					else
					{
						TEXT_SetText(printStopDlgText, print_file_dialog_menu.print_file);
						GUI_UC_SetEncodeNone();
						TEXT_SetFont(printfilename,&GUI_FontHZ16);
						if(strlen(curFileName)>32)
						{
							memset(codebuff,'\0',sizeof(codebuff));
							strncpy(codebuff,curFileName,32);
							codebuff[32]='\n';
							strcat(codebuff,&curFileName[32]);
							TEXT_SetText(printfilename, &codebuff[3]);	
						}
						else
						{
							TEXT_SetText(printfilename, &curFileName[3]);
						}
						GUI_UC_SetEncodeUTF8();
						BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);
						BUTTON_SetText(buttonCancle, print_file_dialog_menu.cancle);
					}
				}
				else if(DialogType == DIALOG_TYPE_REPRINT_NO_FILE)
				{
					TEXT_SetText(printStopDlgText, file_menu.no_file_and_check);
					BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);
					BUTTON_SetText(buttonCancle, print_file_dialog_menu.cancle);
				}
				else if(DialogType == DIALOG_TYPE_REPRINT_NO_TEMP)
				{
					TEXT_SetText(printStopDlgText, file_menu.no_desire_temp);
					BUTTON_SetText(buttonOk, print_file_dialog_menu.defaulty);
					BUTTON_SetText(buttonCancle, print_file_dialog_menu.setting);
				}
				else if(DialogType == DIALOG_TYPE_M80_FAIL)
				{
					TEXT_SetText(printStopDlgText, print_file_dialog_menu.close_machine_error);
					BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);
				}
				/*
				else if(DialogType == DIALOG_TYPE_MESSEGE_ERR1)
				{
					TEXT_SetText(printStopDlgText, print_file_dialog_menu.file_name_too_long_error);
					BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);
				}
				*/
				else if(DialogType == DIALOG_TYPE_UNBIND)
				{
					TEXT_SetText(printStopDlgText, common_menu.unbind_printer_tips);
					BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);
					BUTTON_SetText(buttonCancle, print_file_dialog_menu.cancle);
				}
				
				else if(DialogType == DIALOG_TYPE_FILAMENT_LOAD_HEAT)
				{
					TEXT_SetText(printStopDlgText, filament_menu.filament_dialog_load_heat);
					BUTTON_SetText(buttonCancle, print_file_dialog_menu.cancle);			
				}
				else if(DialogType == DIALOG_TYPE_FILAMENT_HEAT_LOAD_COMPLETED)
				{
					TEXT_SetText(printStopDlgText, filament_menu.filament_dialog_load_heat_confirm);
					BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);	
		      BUTTON_SetText(buttonCancle, print_file_dialog_menu.cancle);
				}
				else if(DialogType == DIALOG_TYPE_FILAMENT_LOADING)	
				{
					TEXT_SetText(printStopDlgText, filament_menu.filament_dialog_loading);
				}
				else if(DialogType == DIALOG_TYPE_FILAMENT_LOAD_COMPLETED)	
				{
					TEXT_SetText(printStopDlgText, filament_menu.filament_dialog_load_completed);
					BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);		
				}	
				else if(DialogType == DIALOG_TYPE_FILAMENT_UNLOAD_HEAT)
				{
					TEXT_SetText(printStopDlgText, filament_menu.filament_dialog_unload_heat);
					BUTTON_SetText(buttonCancle, print_file_dialog_menu.cancle);			
				}
				else if(DialogType == DIALOG_TYPE_FILAMENT_HEAT_UNLOAD_COMPLETED)
				{
					TEXT_SetText(printStopDlgText, filament_menu.filament_dialog_unload_heat_confirm);
					BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);	
		      BUTTON_SetText(buttonCancle, print_file_dialog_menu.cancle);
				}
				else if(DialogType == DIALOG_TYPE_FILAMENT_UNLOADING)	
				{
					TEXT_SetText(printStopDlgText, filament_menu.filament_dialog_unloading);
				}
				else if(DialogType == DIALOG_TYPE_FILAMENT_UNLOAD_COMPLETED)	
				{
					TEXT_SetText(printStopDlgText, filament_menu.filament_dialog_unload_completed);
					BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);		
				}
				//chen 9.10
				else if(DialogType == DIALOG_EMPTY_FILAMENT_TIPS)
				{
					TEXT_SetText(printStopDlgText, filament_menu.filamemt_dialog_empty);
					BUTTON_SetText(buttonOk, print_file_dialog_menu.confirm);	
				}

	}
	
	TEXT_SetTextColor(printStopDlgText, gCfgItems.title_color);
	TEXT_SetTextAlign(printStopDlgText, GUI_TA_VCENTER | GUI_TA_HCENTER);

	if(buttonOk)
	{
		BUTTON_SetBkColor(buttonOk, BUTTON_CI_UNPRESSED, GUI_BLUE);
		BUTTON_SetBkColor(buttonOk, BUTTON_CI_PRESSED, GUI_BLUE);
		BUTTON_SetTextColor(buttonOk, BUTTON_CI_UNPRESSED, GUI_WHITE);
		BUTTON_SetTextColor(buttonOk, BUTTON_CI_PRESSED, GUI_WHITE);
		BUTTON_SetTextAlign(buttonOk, GUI_TA_VCENTER | GUI_TA_HCENTER);

			//对按钮做擦除标志设置
		BUTTON_SetBmpFileName(buttonOk,NULL,1);
	}
	if(buttonCancle)
	{
		BUTTON_SetBkColor(buttonCancle, BUTTON_CI_UNPRESSED, GUI_BLUE);
		BUTTON_SetBkColor(buttonCancle, BUTTON_CI_PRESSED, GUI_BLUE);
		BUTTON_SetTextColor(buttonCancle, BUTTON_CI_UNPRESSED, GUI_WHITE);
		BUTTON_SetTextColor(buttonCancle, BUTTON_CI_PRESSED, GUI_WHITE);
		BUTTON_SetTextAlign(buttonCancle, GUI_TA_VCENTER | GUI_TA_HCENTER);

			//对按钮做擦除标志设置
		BUTTON_SetBmpFileName(buttonCancle,NULL,1);
	}


	
	

	//TEXT_SetDefaultFont(&GUI_FontHZ_fontHz14);

	//GUI_Exec();
}
#if 1
void filament_setbar()
{
	PROGBAR_SetBarColor(FilamentBar, 0, GUI_GREEN);
	PROGBAR_SetValue(FilamentBar,filament_rate);
}
#endif

void filament_sprayer_temp()
{
	int8_t buf[20] = {0};

	TEXT_SetTextColor(filament_temper, gCfgItems.state_text_color);
	TEXT_SetBkColor(filament_temper, gCfgItems.state_background_color);
	//不支持双进一出
	if(gCfgItems.single_nozzle==0)
	{
		sprintf(buf, "E%d:%d/%d", gCfgItems.curSprayerChoose+1,(int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose],(int)gCfgItems.desireSprayerTemp[gCfgItems.curSprayerChoose]);
	}
	//支持
	else
	{
		sprintf(buf, "Temp.:%d/%d",(int)gCfgItems.curSprayerTemp[0],(int)gCfgItems.desireSprayerTemp[0]);
	}
	TEXT_SetText(filament_temper, buf);
}

void filament_dialog_handle(void)
{			
		
			
			if((temp_update_flag == 1)
				&&((DialogType == DIALOG_TYPE_FILAMENT_LOAD_HEAT)
				||(DialogType == DIALOG_TYPE_FILAMENT_UNLOAD_HEAT)))
			{
				temp_update_flag = 0;
				filament_sprayer_temp();
			}
			//HC-chen
			//支持双进一出
			if(gCfgItems.single_nozzle==1)
			{
				if(((abs((int)gCfgItems.curSprayerTemp[0] - gCfgItems.filament_limit_temper)<=1)
				||((int)gCfgItems.curSprayerTemp[0] >= gCfgItems.filament_limit_temper))
				&&(filament_load_heat_flg==1))
				{
					filament_load_heat_flg = 0;
					Clear_dialog();
					draw_dialog(DIALOG_TYPE_FILAMENT_HEAT_LOAD_COMPLETED);				
				}
			}//不支持
			else
			{
				if(((abs((int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose] - gCfgItems.filament_limit_temper)<=1)
				||((int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose] >= gCfgItems.filament_limit_temper))
				&&(filament_load_heat_flg==1))
				{
					filament_load_heat_flg = 0;
					Clear_dialog();
					draw_dialog(DIALOG_TYPE_FILAMENT_HEAT_LOAD_COMPLETED);				
				}
			} 
			if(filament_loading_completed==1)
			{
				filament_rate = 0;
				filament_loading_completed = 0;

				Clear_dialog();
				draw_dialog(DIALOG_TYPE_FILAMENT_LOAD_COMPLETED);	

				if(printerStaus == pr_pause)
				{
					positionSaveFlag = 1;
				}
			}
			if(gCfgItems.single_nozzle==1)
			{
				if(((abs((int)gCfgItems.curSprayerTemp[0] - gCfgItems.filament_limit_temper)<=1)
				||((int)gCfgItems.curSprayerTemp[0] >= gCfgItems.filament_limit_temper))
				&&(filament_unload_heat_flg==1))
				{
					filament_unload_heat_flg = 0;
					Clear_dialog();
					draw_dialog(DIALOG_TYPE_FILAMENT_HEAT_UNLOAD_COMPLETED);				
				}
			}
			else
			{
				if(((abs((int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose] - gCfgItems.filament_limit_temper)<=1)
				||((int)gCfgItems.curSprayerTemp[gCfgItems.curSprayerChoose] >= gCfgItems.filament_limit_temper))
				&&(filament_unload_heat_flg==1))
				{
					filament_unload_heat_flg = 0;
					Clear_dialog();
					draw_dialog(DIALOG_TYPE_FILAMENT_HEAT_UNLOAD_COMPLETED);				
				}
			}
			

			if(filament_unloading_completed==1)
			{
				filament_rate = 0;
				filament_unloading_completed = 0;

				Clear_dialog();
				draw_dialog(DIALOG_TYPE_FILAMENT_UNLOAD_COMPLETED);

				if(printerStaus == pr_pause)
				{
					positionSaveFlag = 1;
				}

			}
			
			if((DialogType == DIALOG_TYPE_FILAMENT_LOADING)||(DialogType == DIALOG_TYPE_FILAMENT_UNLOADING))
			{
				#if defined(STM32F10X_CL)
				filament_setbar();
				#else
				PROGBAR_SetBarColor(FilamentBar, 0, GUI_GREEN);
				PROGBAR_SetValue(FilamentBar,filament_rate);
				#endif
			}				
	
}

void Clear_dialog()
{
	GUI_SetBkColor(gCfgItems.background_color);	
	if(WM_IsWindow(hStopDlgWnd))
	{
		WM_DeleteWindow(hStopDlgWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}
