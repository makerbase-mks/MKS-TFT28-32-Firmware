#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_change_speed.h"
#include "draw_ui.h"
#include "pic.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "usbh_usr.h"
#include "printer.h"
#include "printing_thread.h"
#include "id_manage.h"
#include "text.h"
#include "gcode.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif

GUI_HWIN hChangeSpeedWnd;


extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static TEXT_Handle textPrintSpeed,textPrintSpeedvalue;

static uint8_t speedType = 0;

static BUTTON_Handle buttonInc, buttonDec, buttonStep, buttonMove, buttonExtruct, buttonRet;

static void cbChangeSpeedWin(WM_MESSAGE * pMsg) {

	uint8_t buf[30] = {0};

	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetColor(gCfgItems.state_text_color);
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
					Clear_changeSpeed();
					draw_return_ui();
				}
				else if(pMsg->hWinSrc == buttonInc)
				{
					
					if(speedType == 0)
					{
						
						if(gCfgItems.printSpeed < MAX_EXT_SPEED_PERCENT - gCfgItems.stepPrintSpeed)
						{
							gCfgItems.printSpeed += gCfgItems.stepPrintSpeed;
							
						}
						else
						{
							gCfgItems.printSpeed = MAX_EXT_SPEED_PERCENT;
						}
						SET_MOVE_SPEED_COMMAND(buf, gCfgItems.printSpeed);
						pushFIFO(&gcodeCmdTxFIFO, buf);
					}
					else if(speedType == 1)
					{
						if(gCfgItems.printExtSpeed0< MAX_EXT_SPEED_PERCENT - gCfgItems.stepPrintSpeed)
						{
							gCfgItems.printExtSpeed0 += gCfgItems.stepPrintSpeed;
							
						}
						else
						{
							gCfgItems.printExtSpeed0 = MAX_EXT_SPEED_PERCENT;
						}
						gCfgItems.printExtSpeed1 = gCfgItems.printExtSpeed0;
						SET_EXTRU_SPEED_COMMAND(buf, gCfgItems.printExtSpeed0);
						pushFIFO(&gcodeCmdTxFIFO, buf);
					}
					/*else if(speedType == 2)
					{
						if(gCfgItems.printExtSpeed1< MAX_EXT_SPEED_PERCENT - gCfgItems.stepPrintSpeed)
						{
							gCfgItems.printExtSpeed1 += gCfgItems.stepPrintSpeed;
							
						}
						else
						{
							gCfgItems.printExtSpeed1 = MAX_EXT_SPEED_PERCENT;
						}
						SET_EXTRU_SPEED_COMMAND(buf, gCfgItems.printExtSpeed1);
						pushFIFO(&gcodeCmdTxFIFO, buf);
					}*/
						
					disp_print_speed();
					
				}
				else if(pMsg->hWinSrc == buttonDec)
				{
					if(speedType == 0)
					{						
						if(gCfgItems.printSpeed > MIN_EXT_SPEED_PERCENT + gCfgItems.stepPrintSpeed)
						{
							gCfgItems.printSpeed -= gCfgItems.stepPrintSpeed;							
						}
						else
						{
							gCfgItems.printSpeed = MIN_EXT_SPEED_PERCENT;
						}
						SET_MOVE_SPEED_COMMAND(buf, gCfgItems.printSpeed);
						pushFIFO(&gcodeCmdTxFIFO, buf);
					}
					else if(speedType == 1)
					{
						if(gCfgItems.printExtSpeed0 > MIN_EXT_SPEED_PERCENT + gCfgItems.stepPrintSpeed)
						{
							gCfgItems.printExtSpeed0 -= gCfgItems.stepPrintSpeed;							
						}
						else
						{
							gCfgItems.printExtSpeed0 = MIN_EXT_SPEED_PERCENT;
						}
						gCfgItems.printExtSpeed1 = gCfgItems.printExtSpeed0;
						SET_EXTRU_SPEED_COMMAND(buf, gCfgItems.printExtSpeed0);
						pushFIFO(&gcodeCmdTxFIFO, buf);
					}
					/*else if(speedType == 2)
					{						

						if(gCfgItems.printExtSpeed1 > MIN_EXT_SPEED_PERCENT + gCfgItems.stepPrintSpeed)
						{
							gCfgItems.printExtSpeed1 -= gCfgItems.stepPrintSpeed;							
						}
						else
						{
							gCfgItems.printExtSpeed1 = MIN_EXT_SPEED_PERCENT;
						}
						SET_EXTRU_SPEED_COMMAND(buf, gCfgItems.printExtSpeed1);
						pushFIFO(&gcodeCmdTxFIFO, buf);
					}*/
						
					disp_print_speed();
				}
				else if(pMsg->hWinSrc == buttonStep)
				{					
					if(gCfgItems.stepPrintSpeed == 1)
					{
						gCfgItems.stepPrintSpeed = 5;
					}
					else if(gCfgItems.stepPrintSpeed == 5)
					{
						gCfgItems.stepPrintSpeed = 10;						
					}
					else
					{
						gCfgItems.stepPrintSpeed = 1;	
					}
					disp_step_speed();
				}
			/*	else if(pMsg->hWinSrc == buttonFivePercent)
				{					
					gCfgItems.stepPrintSpeed = 5;
					disp_step_speed();
				}*/
				else if(pMsg->hWinSrc == buttonMove)
				{
					speedType = 0;
					//disp_speed_type();
					disp_print_speed();
				}
				else if(pMsg->hWinSrc == buttonExtruct)
				{
					speedType = 1;
					//disp_speed_type();
					disp_print_speed();
				}
				/*else if(pMsg->hWinSrc == buttonExtruct2)
				{
					speedType = 2;
					disp_speed_type();
					disp_print_speed();
				}*/
				
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}


void draw_changeSpeed()
{
		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != CHANGE_SPEED_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = CHANGE_SPEED_UI;
	}
	
	disp_state = CHANGE_SPEED_UI;
		
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

	//GUI_DispStringAt("正在打印->操作->变速", 0, 0);
	GUI_DispStringAt(creat_title_text(), 0, 0);
	
	hChangeSpeedWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbChangeSpeedWin, 0);

	buttonInc = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hChangeSpeedWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonDec = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hChangeSpeedWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	
	buttonStep = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hChangeSpeedWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//	buttonFivePercent = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 4, LCD_WIDTH / 4 - 1, imgHeight / 4 - 1, hChangeSpeedWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonMove = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hChangeSpeedWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonExtruct = BUTTON_CreateEx(LCD_WIDTH /4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hChangeSpeedWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	//buttonExtruct2 = BUTTON_CreateEx(LCD_WIDTH /2 , imgHeight / 2, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hChangeSpeedWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hChangeSpeedWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	//TEXT_SetDefaultFont(&FONT_STATE_INF);

	textPrintSpeed = TEXT_CreateEx(LCD_WIDTH / 4, 35, LCD_WIDTH / 2-2, (imgHeight / 2)/3-15, hChangeSpeedWnd, WM_CF_SHOW, GUI_TA_HCENTER|GUI_TA_VCENTER, alloc_win_id(),  " ");
	textPrintSpeedvalue = TEXT_CreateEx(LCD_WIDTH / 4, 55, LCD_WIDTH / 2-2, (imgHeight / 2)/3-15, hChangeSpeedWnd, WM_CF_SHOW, TEXT_CF_LEFT, alloc_win_id(),  " ");
	

	TEXT_SetTextAlign(textPrintSpeed,  GUI_TA_VCENTER | GUI_TA_HCENTER);
	TEXT_SetTextAlign(textPrintSpeedvalue,  GUI_TA_VCENTER | GUI_TA_HCENTER);
	//TEXT_SetTextAlign(textPrintSpeed,  GUI_TA_VCENTER | GUI_TA_HCENTER);	

	#if VERSION_WITH_PIC	

	BUTTON_SetBmpFileName(buttonInc, "bmp_Add.bin",1);	
	BUTTON_SetBmpFileName(buttonDec, "bmp_Dec.bin",1);
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);	
	
	BUTTON_SetBitmapEx(buttonInc, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonDec, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonMove, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonExtruct, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonStep, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	if(gCfgItems.multiple_language != 0)
	{
			BUTTON_SetText(buttonInc,speed_menu.add);
			BUTTON_SetText(buttonDec,speed_menu.dec);
			BUTTON_SetText(buttonRet,common_menu.text_back);
	}
	#endif
	
	disp_print_speed();
	disp_speed_type();
	disp_step_speed();
	
	//GUI_Exec();


	
}

void Clear_changeSpeed()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hChangeSpeedWnd))
	{
		WM_DeleteWindow(hChangeSpeedWnd);
		GUI_Exec();
	}
	//GUI_Clear();
}

void disp_print_speed()
{
	int8_t buf[30] = {0};
	char buf1[10] = {0};
	
	TEXT_SetTextColor(textPrintSpeed, gCfgItems.state_text_color);
	TEXT_SetBkColor(textPrintSpeed, gCfgItems.state_background_color);
	TEXT_SetTextColor(textPrintSpeedvalue, gCfgItems.state_text_color);
	TEXT_SetBkColor(textPrintSpeedvalue, gCfgItems.state_background_color);	


	if(speedType == 0)
		{
			sprintf(buf1, "%d%%",gCfgItems.printSpeed );
			sprintf(buf, speed_menu.move_speed);
		}
		else if(speedType == 1)
		{
			sprintf(buf1, "%d%%", gCfgItems.printExtSpeed0); 
			sprintf(buf, speed_menu.extrude_speed);
		}
			
		TEXT_SetText(textPrintSpeed, buf);
		TEXT_SetText(textPrintSpeedvalue, buf1);
	
	
		switch(speedType)
		{
			case 1:
				//BUTTON_SetBmpFileName(buttonExtruct.btnHandle, "bmp_extruct_sel.bin",0);	
				//BUTTON_SetBmpFileName(buttonMove.btnHandle, "bmp_mov.bin",0); 
	
				BUTTON_SetBmpFileName(buttonExtruct, "bmp_extruct_sel.bin",0);	
				BUTTON_SetBmpFileName(buttonMove, "bmp_mov.bin",0); 
				
				BUTTON_SetBitmapEx(buttonExtruct, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
				BUTTON_SetBitmapEx(buttonMove, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		
				break;
	
			default:	
				BUTTON_SetBmpFileName(buttonMove, "bmp_mov_sel.bin",0);
				BUTTON_SetBmpFileName(buttonExtruct, "bmp_extruct.bin",0);
				BUTTON_SetBitmapEx(buttonMove, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
				BUTTON_SetBitmapEx(buttonExtruct, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	
				break;
		}
		
		if(gCfgItems.multiple_language != 0)
		{
			BUTTON_SetText(buttonExtruct,speed_menu.extrude);
			BUTTON_SetText(buttonMove,speed_menu.move);
		}

	//TEXT_SetText(textPrintSpeed, buf);

}

void disp_speed_type()
{
	switch(speedType)
	{
		case 1:
			BUTTON_SetBmpFileName(buttonExtruct, "bmp_extruct_sel.bin",0);	
			BUTTON_SetBmpFileName(buttonMove, "bmp_mov.bin",0);	
			BUTTON_SetBitmapEx(buttonExtruct, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			BUTTON_SetBitmapEx(buttonMove, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			#if VERSION_WITH_PIC
			BUTTON_SetBmpFileName(buttonExtruct, "bmp_extruct_sel.bin",0);	
			BUTTON_SetBmpFileName(buttonMove, "bmp_mov.bin",0);	
			#endif
			break;

		default:
			#if VERSION_WITH_PIC
		
			BUTTON_SetBmpFileName(buttonMove, "bmp_mov_sel.bin",0);
			BUTTON_SetBmpFileName(buttonExtruct, "bmp_extruct.bin",0);
			BUTTON_SetBitmapEx(buttonMove, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			BUTTON_SetBitmapEx(buttonExtruct, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			#endif
			break;
	}
	
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetText(buttonExtruct,speed_menu.extrude);
		BUTTON_SetText(buttonMove,speed_menu.move);
	}	

}

void disp_step_speed()
{
	if(gCfgItems.stepPrintSpeed == 1)
		BUTTON_SetBmpFileName(buttonStep, "bmp_step1_mm.bin",0);
	else if(gCfgItems.stepPrintSpeed == 5)
		BUTTON_SetBmpFileName(buttonStep, "bmp_step5_mm.bin",0);
	else if(gCfgItems.stepPrintSpeed == 10)
		BUTTON_SetBmpFileName(buttonStep, "bmp_step10_mm.bin",0);

	if(gCfgItems.multiple_language != 0)
	{
		if(gCfgItems.stepPrintSpeed == 1)
			BUTTON_SetText(buttonStep,speed_menu.step_1percent);
		else if(gCfgItems.stepPrintSpeed == 5)
			BUTTON_SetText(buttonStep,speed_menu.step_5percent);
		else if(gCfgItems.stepPrintSpeed == 10)
			BUTTON_SetText(buttonStep,speed_menu.step_10percent);
	
	}

}


