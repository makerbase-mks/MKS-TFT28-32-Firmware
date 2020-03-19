#include "gui.h"
#include "button.h"
#include "draw_ui.h"
#include "fontLib.h"
#include "printer.h"
#include "sd_usr.h"
#include "draw_printing_moremenu.h"
#include "draw_filamentchange.h"
#include "draw_babystep.h"
#include "gcode.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif
extern char cmd_code[201];
extern volatile unsigned char *codebufpoint;

static GUI_HWIN hPrintmoreWnd;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static BUTTON_Handle buttonPreHeat,buttonFan_P,buttonoff_P,morefunc1,morefunc2,morefunc3,morefunc4,morefunc5,morefunc6,buttonRet;
static BUTTON_Handle buttonSpeed,babystep;

uint8_t IsChooseAutoShutdown = 0;

static void cbPrintmoreWin(WM_MESSAGE * pMsg) {

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
					
					Clear_Printmore();
					draw_return_ui();
					
					//clear_cur_ui();
					//draw_return_ui();
					
					
				}
				else if(pMsg->hWinSrc == buttonSpeed)
				{
					last_disp_state = PRINT_MORE_UI;
					Clear_Printmore();
					draw_changeSpeed();
				}
				else if(pMsg->hWinSrc == buttonFan_P)
				{
					last_disp_state = PRINT_MORE_UI;
					Clear_Printmore();
					draw_fan();
				}
				else if(pMsg->hWinSrc == buttonPreHeat)
				{
					last_disp_state = PRINT_MORE_UI;
					Clear_pause();
					draw_preHeat();
				}				
				else if(pMsg->hWinSrc == buttonoff_P)
				{
					if(IsChooseAutoShutdown == 1)
					{
							IsChooseAutoShutdown = 0;		
							BUTTON_SetBmpFileName(buttonoff_P, "bmp_manual_off.bin",0);
							BUTTON_SetBitmapEx(buttonoff_P, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
							if(gCfgItems.multiple_language != 0)
								BUTTON_SetText(buttonoff_P,printing_more_menu.manual);
							/*
							if(gCfgItems.multiple_language != 0)
							{
									switch(gCfgItems.language)
									{
										case LANG_SIMPLE_CHINESE:
											BUTTON_SetText(buttonoff_P,MANUAL_SHUTDOWN_CN);			
											break;
										case LANG_COMPLEX_CHINESE:
											BUTTON_SetText(buttonoff_P,MANUAL_SHUTDOWN_T_CN);	 							
											break;
										case LANG_ENGLISH:
											BUTTON_SetText(buttonoff_P,MANUAL_SHUTDOWN_EN);			

										case LANG_RUSSIAN:
											BUTTON_SetText(buttonoff_P,MANUAL_SHUTDOWN_RU);			
											break;
										case LANG_SPANISH:
											BUTTON_SetText(buttonoff_P,MANUAL_SHUTDOWN_SP);	 							
											break;
										#if defined (STM32F10X_HD)
											break;
										case LANG_JAPAN:
											BUTTON_SetText(buttonoff_P,MANUAL_SHUTDOWN_JP);						
											break;										
										case LANG_GERMAN:
											BUTTON_SetText(buttonoff_P,MANUAL_SHUTDOWN_GN);		 				
											break;
										case LANG_FRENCH:
											BUTTON_SetText(buttonoff_P,MANUAL_SHUTDOWN_FR);		
											break;

										case LANG_GREEK:
											BUTTON_SetText(buttonoff_P,MANUAL_SHUTDOWN_GR);			
											break;
										case LANG_TURKISH:
											BUTTON_SetText(buttonoff_P,MANUAL_SHUTDOWN_TU);						
											break;

										case LANG_KOREAN:
											BUTTON_SetText(buttonoff_P,MANUAL_SHUTDOWN_KR);		 				
											break;
										
										case LANG_ITALY:
											BUTTON_SetText(buttonoff_P,MANUAL_SHUTDOWN_IT);		
											break;
											#endif
									}
							}
							*/
					}
					else
					{
							IsChooseAutoShutdown = 1;
							BUTTON_SetBmpFileName(buttonoff_P, "bmp_auto_off.bin",0);
							BUTTON_SetBitmapEx(buttonoff_P, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
							if(gCfgItems.multiple_language != 0)
								BUTTON_SetText(buttonoff_P,printing_more_menu.auto_close);
							/*
							if(gCfgItems.multiple_language != 0)
							{
									switch(gCfgItems.language)
									{
										case LANG_SIMPLE_CHINESE:
											BUTTON_SetText(buttonoff_P,AUTO_SHUTDOWN_CN);			
											break;
										case LANG_COMPLEX_CHINESE:
											BUTTON_SetText(buttonoff_P,AUTO_SHUTDOWN_CN);	 							
											break;
										case LANG_ENGLISH:
											BUTTON_SetText(buttonoff_P,AUTO_SHUTDOWN_CN);			
											break;

										case LANG_RUSSIAN:
											BUTTON_SetText(buttonoff_P,AUTO_SHUTDOWN_RU);			
											break;
										case LANG_SPANISH:
											BUTTON_SetText(buttonoff_P,AUTO_SHUTDOWN_SP);	 							
											break;
										#if defined (STM32F10X_HD)
										case LANG_JAPAN:
											BUTTON_SetText(buttonoff_P,AUTO_SHUTDOWN_CN);						
											break;										
										case LANG_GERMAN:
											BUTTON_SetText(buttonoff_P,AUTO_SHUTDOWN_CN);		 				
											break;
										case LANG_FRENCH:
											BUTTON_SetText(buttonoff_P,AUTO_SHUTDOWN_CN);		
											break;

										case LANG_GREEK:
											BUTTON_SetText(buttonoff_P,AUTO_SHUTDOWN_GR);			
											break;
										case LANG_TURKISH:
											BUTTON_SetText(buttonoff_P,AUTO_SHUTDOWN_TU);						
											break;
										case LANG_KOREAN:
											BUTTON_SetText(buttonoff_P,AUTO_SHUTDOWN_KR);		 				
											break;
										
										case LANG_ITALY:
											BUTTON_SetText(buttonoff_P,AUTO_SHUTDOWN_IT);		
											break;
											#endif
									}
							}		
							*/
					}
				}
				else if(pMsg->hWinSrc == morefunc1)
				{
							SPI_FLASH_BufferRead(cmd_code,BUTTON_MOREFUNC1_ADDR,201);
							codebufpoint = cmd_code;	
				}
				else if(pMsg->hWinSrc == morefunc2)
				{
							SPI_FLASH_BufferRead(cmd_code,BUTTON_MOREFUNC2_ADDR,201);
							codebufpoint = cmd_code;	
				}					
				else if(pMsg->hWinSrc == morefunc3)
				{
							SPI_FLASH_BufferRead(cmd_code,BUTTON_MOREFUNC3_ADDR,201);
							codebufpoint = cmd_code;	
				}				
				else if(pMsg->hWinSrc == morefunc4)
				{
							SPI_FLASH_BufferRead(cmd_code,BUTTON_MOREFUNC4_ADDR,201);
							codebufpoint = cmd_code;	
				}				
				else if(pMsg->hWinSrc == morefunc5)
				{
							SPI_FLASH_BufferRead(cmd_code,BUTTON_MOREFUNC5_ADDR,201);
							codebufpoint = cmd_code;	
				}
				else if(pMsg->hWinSrc == babystep)
				{
					pushFIFO(&gcodeCmdTxFIFO,(unsigned char *) GET_BABY_STEP_VALUE_COMMAND);
					pushFIFO(&gcodeCmdTxFIFO,(unsigned char *) GET_BABY_ZOFFSET_COMMAND);

					last_disp_state = PRINT_MORE_UI;
					Clear_Printmore();
					draw_babyStep();	
				}
				//else if(pMsg->hWinSrc == morefunc6)
				//{
				//			SPI_FLASH_BufferRead(cmd_code,BUTTON_MOREFUNC6_ADDR,201);
				//			codebufpoint = cmd_code;	
				//}				
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}

void draw_printmore()
{
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != PRINT_MORE_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINT_MORE_UI;
	}
	
	disp_state = PRINT_MORE_UI;
	
	

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

	hPrintmoreWnd=0;
	buttonFan_P=0;
	buttonSpeed=0;
	buttonPreHeat=0;
	buttonoff_P=0;
	buttonRet=0;
	
	hPrintmoreWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbPrintmoreWin, 0);

	buttonFan_P = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPrintmoreWnd, BUTTON_CF_SHOW, 0, 301);
	if(printerStaus == pr_pause)
	{
		buttonSpeed = 0;
		buttonPreHeat = BUTTON_CreateEx(LCD_WIDTH / 4,	0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPrintmoreWnd, BUTTON_CF_SHOW, 0, 302);		
	}
	else
	{
		buttonPreHeat = 0;
		buttonSpeed = BUTTON_CreateEx(LCD_WIDTH / 4,	0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPrintmoreWnd, BUTTON_CF_SHOW, 0, 302);		
	}

	{
			if(gCfgItems.print_finish_close_machine_flg == 1)
			{
				
				buttonoff_P = BUTTON_CreateEx(LCD_WIDTH / 2,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 302);

				if(gCfgItems.babystep == 1)
				{
					if(gCfgItems.morefunc_cnt >= 4)
					{
						gCfgItems.morefunc_cnt = 3;
					}
					switch(gCfgItems.morefunc_cnt)
					{
						case 3:
					   		morefunc3 = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 107);
						case 2:
					   		morefunc2 = BUTTON_CreateEx(LCD_WIDTH / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 106);
						case 1:
					   		morefunc1 = BUTTON_CreateEx(0,imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 105);
						break;
						default:break;			
					}
					babystep = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 108);
				}
				else
				{
					if(gCfgItems.morefunc_cnt >= 5)
					{
						gCfgItems.morefunc_cnt = 4;
					}
					switch(gCfgItems.morefunc_cnt)
					{
						case 4:
						   morefunc4 = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 107);
						case 3:
						   morefunc3 = BUTTON_CreateEx(LCD_WIDTH / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 106);
						case 2:
						   morefunc2 = BUTTON_CreateEx(0,imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 105);
						case 1:
						   morefunc1 = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 108);
						break;
						default:break;			
					}
				}

				
			}
			else
			{
				if(gCfgItems.babystep == 1)
				{
					if(gCfgItems.morefunc_cnt >= 5)
					{
						gCfgItems.morefunc_cnt = 4;
					}
					switch(gCfgItems.morefunc_cnt)
					{
						case 4:
						   morefunc4 = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 107);
						case 3:
						   morefunc3 = BUTTON_CreateEx(LCD_WIDTH / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 106);
						case 2:
						   morefunc2 = BUTTON_CreateEx(0,imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 105);
						case 1:
						   morefunc1 = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 108);
						   
						break;
						default:break;			
					}
					babystep = BUTTON_CreateEx(LCD_WIDTH / 2,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 103);
				}
				else
				{
					if(gCfgItems.morefunc_cnt >= 6)
					{
						gCfgItems.morefunc_cnt = 5;
					}
					switch(gCfgItems.morefunc_cnt)
					{
						case 5:
						   morefunc5 = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 107);
						case 4:
						   morefunc4 = BUTTON_CreateEx(LCD_WIDTH / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 106);
						case 3:
						   morefunc3 = BUTTON_CreateEx(0,imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 105);
						case 2:
						   morefunc2 = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 108);
						case 1:
						   morefunc1 = BUTTON_CreateEx(LCD_WIDTH / 2,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hPrintmoreWnd, BUTTON_CF_SHOW, 0, 103);
						break;
						default:break;			
					}
				}
				
			}		
	}
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPrintmoreWnd, BUTTON_CF_SHOW, 0, 308);
	
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);
	BUTTON_SetBmpFileName(buttonFan_P, "bmp_fan.bin",0);
	//HC-chen 2.8
	//if(printerStaus != pr_working)
	if(printerStaus == pr_pause)
	{
		BUTTON_SetBmpFileName(buttonPreHeat, "bmp_temp.bin",0);
		BUTTON_SetBitmapEx(buttonPreHeat, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);	
	}
	else
	{
		BUTTON_SetBmpFileName(buttonSpeed, "bmp_speed.bin",0);
		BUTTON_SetBitmapEx(buttonSpeed, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	}
	if(gCfgItems.print_finish_close_machine_flg == 1)
	{
		if(IsChooseAutoShutdown == 1)
		{
			BUTTON_SetBmpFileName(buttonoff_P, "bmp_auto_off.bin",0);
		}
		else
		{
			BUTTON_SetBmpFileName(buttonoff_P, "bmp_manual_off.bin",0);
		}

		switch(gCfgItems.morefunc_cnt)
		{
			case 4:
			   BUTTON_SetBmpFileName(morefunc4, "bmp_morefunc4.bin",1);
			   BUTTON_SetBitmapEx(morefunc4, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			case 3:
			   BUTTON_SetBmpFileName(morefunc3, "bmp_morefunc3.bin",1);
			   BUTTON_SetBitmapEx(morefunc3, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			case 2:
			   BUTTON_SetBmpFileName(morefunc2, "bmp_morefunc2.bin",1);
			   BUTTON_SetBitmapEx(morefunc2, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			case 1:
			   BUTTON_SetBmpFileName(morefunc1, "bmp_morefunc1.bin",1);
			   BUTTON_SetBitmapEx(morefunc1, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
				
			break;
			default:break;			
		}
		if(gCfgItems.babystep == 1)
		{
			BUTTON_SetBmpFileName(babystep, "bmp_babysteps.bin",1);
		       BUTTON_SetBitmapEx(babystep, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		}
		
		BUTTON_SetBitmapEx(buttonoff_P, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		//BUTTON_SetBmpFileName(morefunc5, "bmp_morefunc5.bin",1);
		//BUTTON_SetBitmapEx(morefunc5, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	}
	else
	{

		switch(gCfgItems.morefunc_cnt)
		{
			case 5:
			   BUTTON_SetBmpFileName(morefunc5, "bmp_morefunc5.bin",1);
			   BUTTON_SetBitmapEx(morefunc5, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			case 4:
			   BUTTON_SetBmpFileName(morefunc4, "bmp_morefunc4.bin",1);
			   BUTTON_SetBitmapEx(morefunc4, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			case 3:
			   BUTTON_SetBmpFileName(morefunc3, "bmp_morefunc3.bin",1);
			   BUTTON_SetBitmapEx(morefunc3, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			case 2:
			   BUTTON_SetBmpFileName(morefunc2, "bmp_morefunc2.bin",1);
			   BUTTON_SetBitmapEx(morefunc2, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			case 1:
			   BUTTON_SetBmpFileName(morefunc1, "bmp_morefunc1.bin",1);
			   BUTTON_SetBitmapEx(morefunc1, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			   
			   	
			break;
			default:break;			
		}
		if(gCfgItems.babystep == 1)
		{
			BUTTON_SetBmpFileName(babystep, "bmp_babysteps.bin",1);
		       BUTTON_SetBitmapEx(babystep, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		}
		
	}
	BUTTON_SetBitmapEx(buttonFan_P, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);

	if(gCfgItems.multiple_language != 0)
	{
		//HC-chen 2.8
		//if(printerStaus != pr_working)
		if(printerStaus == pr_pause)
			BUTTON_SetText(buttonPreHeat,printing_more_menu.temp);
		else
			BUTTON_SetText(buttonSpeed,printing_more_menu.speed);
		
		if(gCfgItems.print_finish_close_machine_flg == 1)
		{
			if(IsChooseAutoShutdown == 1)
			{
				BUTTON_SetText(buttonoff_P, printing_more_menu.auto_close);
			}
			else
			{
				BUTTON_SetText(buttonoff_P, printing_more_menu.manual);
			} 					
		}
		BUTTON_SetText(buttonFan_P,printing_more_menu.fan);
		BUTTON_SetText(buttonRet,common_menu.text_back);
		if(gCfgItems.babystep == 1)
			BUTTON_SetText(babystep,printing_more_menu.babystep);

	}


}

void Autoshutdown_display()
{
	if(IsChooseAutoShutdown == 1)
	{
		BUTTON_SetBmpFileName(buttonoff_P, "bmp_auto_off.bin",0);
		BUTTON_SetBitmapEx(buttonoff_P, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	}
	else
	{
		BUTTON_SetBmpFileName(buttonoff_P, "bmp_manual_off.bin",0);
		BUTTON_SetBitmapEx(buttonoff_P, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	}
	if(gCfgItems.multiple_language != 0)
	{
		if(IsChooseAutoShutdown == 1)
			BUTTON_SetText(buttonoff_P,printing_more_menu.auto_close);
		else
			BUTTON_SetText(buttonoff_P,printing_more_menu.manual);		
	}


}

void Clear_Printmore()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hPrintmoreWnd))
	{
		WM_DeleteWindow(hPrintmoreWnd);
		GUI_Exec();
	}
	//GUI_Clear();
}

