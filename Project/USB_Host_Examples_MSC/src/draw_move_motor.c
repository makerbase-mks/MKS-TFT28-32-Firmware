#include "draw_move_motor.h"
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

BUTTON_Handle buttonXI, buttonXD, buttonYI, buttonYD, buttonZI, buttonZD, buttonV,  buttonRet;
GUI_HWIN hMoveMotorWnd;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

//int16_t curXSpeed = 3000;
//int16_t curYSpeed = 3000;
//int16_t curZSpeed = 3000;

static void cbMoveMotorWin(WM_MESSAGE * pMsg) {
	
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

			
			if(pMsg->hWinSrc == buttonXI)
			{
				//if(printer_state >= PRINTER_CONNECT_OK)
				if(gcodeCmdTxFIFO.count <= (FIFO_NODE-3))
				{
					//////printf(RELATIVE_COORD_COMMAN);
					pushFIFO(&gcodeCmdTxFIFO, RELATIVE_COORD_COMMAN);
					MOVE_X_COMMAN(buf, gCfgItems.move_dist, gCfgItems.moveSpeed);
					//////printf(buf);
					pushFIFO(&gcodeCmdTxFIFO, buf);
					////////printf(ABSOLUTE_COORD_COMMAN);
					pushFIFO(&gcodeCmdTxFIFO, ABSOLUTE_COORD_COMMAN);
				}
				//2015/09/22
				//移动后马上保存数据
					if(printerStaus== pr_pause)
						positionSaveFlag = 1;
			}
			else if(pMsg->hWinSrc == buttonXD)
			{
				//if(printer_state >= PRINTER_CONNECT_OK)
				if(gcodeCmdTxFIFO.count <= (FIFO_NODE-3))
				{
					//////printf(RELATIVE_COORD_COMMAN);
					pushFIFO(&gcodeCmdTxFIFO, RELATIVE_COORD_COMMAN);
					MOVE_X_COMMAN(buf, (float)0 - gCfgItems.move_dist,  gCfgItems.moveSpeed );
					//////printf(buf);
					pushFIFO(&gcodeCmdTxFIFO, buf);
					////////printf(ABSOLUTE_COORD_COMMAN);
					pushFIFO(&gcodeCmdTxFIFO, ABSOLUTE_COORD_COMMAN);
				}
				//2015/09/22
				//移动后马上保存数据
				if(printerStaus== pr_pause)
					positionSaveFlag = 1;
			}
			else if(pMsg->hWinSrc == buttonYI)
			{
				//if(printer_state >= PRINTER_CONNECT_OK)
				if(gcodeCmdTxFIFO.count <= (FIFO_NODE-3))
				{
					//////printf(RELATIVE_COORD_COMMAN);
					pushFIFO(&gcodeCmdTxFIFO, RELATIVE_COORD_COMMAN);
					MOVE_Y_COMMAN(buf, gCfgItems.move_dist,  gCfgItems.moveSpeed);
					//////printf(buf);
					pushFIFO(&gcodeCmdTxFIFO, buf);
					////////printf(ABSOLUTE_COORD_COMMAN);
					pushFIFO(&gcodeCmdTxFIFO, ABSOLUTE_COORD_COMMAN);
				}
				//2015/09/22
				//移动后马上保存数据
				if(printerStaus== pr_pause)
						positionSaveFlag = 1;				
			}
			else if(pMsg->hWinSrc == buttonYD)
			{
				//if(printer_state >= PRINTER_CONNECT_OK)
				if(gcodeCmdTxFIFO.count <= (FIFO_NODE-3))
				{
					//////printf(RELATIVE_COORD_COMMAN);
					pushFIFO(&gcodeCmdTxFIFO, RELATIVE_COORD_COMMAN);
					MOVE_Y_COMMAN(buf, (float)0 - gCfgItems.move_dist,  gCfgItems.moveSpeed);
					//////printf(buf);
					pushFIFO(&gcodeCmdTxFIFO, buf);
					////////printf(ABSOLUTE_COORD_COMMAN);
					pushFIFO(&gcodeCmdTxFIFO, ABSOLUTE_COORD_COMMAN);
				}
				//2015/09/22
				//移动后马上保存数据
				if(printerStaus== pr_pause)
						positionSaveFlag = 1;				
			}
			else if(pMsg->hWinSrc == buttonZI)
			{
				//if(printer_state >= PRINTER_CONNECT_OK)
				{
					//////printf(RELATIVE_COORD_COMMAN);
					if(gcodeCmdTxFIFO.count <(FIFO_NODE-3))
					{
						pushFIFO(&gcodeCmdTxFIFO, RELATIVE_COORD_COMMAN);
						MOVE_Z_COMMAN(buf, gCfgItems.move_dist,  gCfgItems.moveSpeed);
						//////printf(buf);
						pushFIFO(&gcodeCmdTxFIFO, buf);
						////////printf(ABSOLUTE_COORD_COMMAN);
						pushFIFO(&gcodeCmdTxFIFO, ABSOLUTE_COORD_COMMAN);
						//pushFIFO(&gcodeCmdTxFIFO, DISP_CUR_POS_COMMAND);
						if(printerStaus != pr_pause)
						{
							gCfgItems.getzpos_flg = 1;
						}
					}
				}
				//2015/09/22
				//移动后马上保存数据
				if(printerStaus== pr_pause)
						positionSaveFlag = 1;				
			}
			else if(pMsg->hWinSrc == buttonZD)
			{
				//if(printer_state >= PRINTER_CONNECT_OK)
				{
					//////printf(RELATIVE_COORD_COMMAN);
					if(gcodeCmdTxFIFO.count <(FIFO_NODE-3))
					{
						pushFIFO(&gcodeCmdTxFIFO, RELATIVE_COORD_COMMAN);
						MOVE_Z_COMMAN(buf, (float)0 - gCfgItems.move_dist,  gCfgItems.moveSpeed);
						//////printf(buf);
						pushFIFO(&gcodeCmdTxFIFO, buf);
						////////printf(ABSOLUTE_COORD_COMMAN);
						pushFIFO(&gcodeCmdTxFIFO, ABSOLUTE_COORD_COMMAN);
						//pushFIFO(&gcodeCmdTxFIFO, DISP_CUR_POS_COMMAND);
						if(printerStaus != pr_pause)
						{
							gCfgItems.getzpos_flg = 1;
						}
					}
				}
				//2015/09/22
				//移动后马上保存数据
				if(printerStaus== pr_pause)
						positionSaveFlag = 1;				
			}
			else if(pMsg->hWinSrc == buttonRet)
			{
				
				last_disp_state = MOVE_MOTOR_UI;
				Clear_move_motor();
				draw_return_ui();
								
			}
			else if(pMsg->hWinSrc == buttonV)
			{
				if(gCfgItems.move_dist == (float)10)
				{
					gCfgItems.move_dist = 0.1;
				}
				else
				{
					gCfgItems.move_dist *= (float)10;
				}
				
				disp_move_dist();
				
			}
			
			
		}
		break;
	default:
	WM_DefaultProc(pMsg);
	}
}


void draw_move_motor()
{
	uint8_t buffer_z[15]={0};
  	
//	int titleHeight = 30;
//	int imgHeight = LCD_HEIGHT - titleHeight;
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != MOVE_MOTOR_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = MOVE_MOTOR_UI;
	}
	disp_state = MOVE_MOTOR_UI;
	
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
	if(printerStaus != pr_pause)
	{
		sprintf(buffer_z,"Z:%0.2f",gCfgItems.zpos);
		GUI_DispStringAt(buffer_z,220, 0);
	}

	hMoveMotorWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbMoveMotorWin, 0);
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
	buttonXI = BUTTON_CreateEx(0, 0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 101);
	buttonYI = BUTTON_CreateEx(LCD_WIDTH / 4,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 102);
	buttonZI = BUTTON_CreateEx(LCD_WIDTH / 2,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 103);


	buttonXD = BUTTON_CreateEx(0,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 105);
	buttonYD = BUTTON_CreateEx(LCD_WIDTH / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 106);
	buttonZD = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 107);

	buttonV = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 108);
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoveMotorWnd, BUTTON_CF_SHOW, 0, 109);

	#if VERSION_WITH_PIC	

		BUTTON_SetBmpFileName(buttonXI, "bmp_xAdd.bin",1);
		BUTTON_SetBmpFileName(buttonXD, "bmp_xDec.bin",1);
		BUTTON_SetBmpFileName(buttonYI, "bmp_yAdd.bin",1);
		BUTTON_SetBmpFileName(buttonYD, "bmp_yDec.bin",1);
		BUTTON_SetBmpFileName(buttonZI, "bmp_zAdd.bin",1);
		BUTTON_SetBmpFileName(buttonZD, "bmp_zDec.bin",1);

	//
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);
	
	BUTTON_SetBitmapEx(buttonXI, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonXD, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonYI, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonYD, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonZI, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonZD, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonV, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);

	#endif

	
	disp_move_dist();

	//GUI_Exec();
	if(gCfgItems.multiple_language != 0)
	{
			BUTTON_SetText(buttonXI,move_menu.x_add);
			BUTTON_SetText(buttonXD,move_menu.x_dec);
			BUTTON_SetText(buttonYI,move_menu.y_add);
			BUTTON_SetText(buttonYD,move_menu.y_dec);
			BUTTON_SetText(buttonZI,move_menu.z_add);
			BUTTON_SetText(buttonZD,move_menu.z_dec);	
			BUTTON_SetText(buttonRet,common_menu.text_back);	
	}	
}

void disp_move_dist()
{
	//int8_t buf[30] = {0};


	//if(gCfgItems.custom_pic_flag != 1)
	//{
	//	if((int)gCfgItems.move_dist == 0)
	//	{
	//		sprintf(buf, "%.1f mm", gCfgItems.move_dist);
	//	}
	//	else
	//	{
	//		sprintf(buf, "%d mm", (int)gCfgItems.move_dist);
	//	}
	//	BUTTON_SetText(buttonV, buf);
	//}
	//else
	{		
		
		if((int)(10 * gCfgItems.move_dist) == 1)
		{
			BUTTON_SetBmpFileName(buttonV, "bmp_step_move0_1.bin",0);
		}
		else if((int)(10 * gCfgItems.move_dist) == 10)
		{
			BUTTON_SetBmpFileName(buttonV, "bmp_step_move1.bin",0);
		}
		else if((int)(10 * gCfgItems.move_dist) == 100)
		{
			BUTTON_SetBmpFileName(buttonV, "bmp_step_move10.bin",0);
		}
	if(gCfgItems.multiple_language != 0)
	{
			if((int)(10 * gCfgItems.move_dist) == 1)
				BUTTON_SetText(buttonV,move_menu.step_01mm);
			else if((int)(10 * gCfgItems.move_dist) == 10)
				BUTTON_SetText(buttonV,move_menu.step_1mm);
			else if((int)(10 * gCfgItems.move_dist) == 100)
				BUTTON_SetText(buttonV,move_menu.step_10mm);
	}		
		
	}
}

void Clear_move_motor()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hMoveMotorWnd))
	{
		WM_DeleteWindow(hMoveMotorWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}

