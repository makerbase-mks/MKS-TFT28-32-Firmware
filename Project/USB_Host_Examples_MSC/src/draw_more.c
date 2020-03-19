#include "draw_more.h"
#include "gui.h"
#include "button.h"
#include "draw_ui.h"
#include "printer.h"
#include "fontLib.h"
#include "gcode.h"
#include "stdint.h"
#include "sd_usr.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif

static BUTTON_Handle buttonCustom1, buttonCustom2, buttonCustom3, buttonCustom4, buttonCustom5, buttonCustom6, buttonCustom7,  buttonRet;
GUI_HWIN hMoreWnd;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

extern char cmd_code[201];


unsigned char codebuff[100];
unsigned char x[1]={0};
volatile unsigned char *codebufpoint = x;


//void pushButtonCodeToFifo(uint8_t *buttoncode)
void pushButtonCodeToFifo(void)
{
	uint8_t i = 0,j;
	
	while(*(codebufpoint) != '\0')
	{
		memset(codebuff,0,sizeof(codebuff));
		
		while((*(codebufpoint) != 0x3b) && (i<96))//命令行以分号为结束符。
		{
			codebuff[i] = *codebufpoint;
			i++;
			codebufpoint++;
		}
		
		codebuff[i] = '\n';
		
		codebufpoint += 1;		
		if(checkFIFO(&gcodeCmdTxFIFO) != fifo_full)
		{
			pushFIFO(&gcodeCmdTxFIFO, codebuff);
		}
		else
		{
			codebufpoint = codebufpoint - i - 1;
			break;
		}
		i = 0;
	}
	
}
static void cbMoreWin(WM_MESSAGE * pMsg) {

	unsigned char codebuff[100];
	unsigned char codebuffpointer;
	unsigned char i = 0,j = 0;
	
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

			
			if(pMsg->hWinSrc == buttonCustom1)
			{
				SPI_FLASH_BufferRead(cmd_code,BUTTON_CMD1_ADDR,201);
				codebufpoint = cmd_code;
			}
			else if(pMsg->hWinSrc == buttonCustom2)
			{
				SPI_FLASH_BufferRead(cmd_code,BUTTON_CMD2_ADDR,201);
				codebufpoint = cmd_code;

			}
			else if(pMsg->hWinSrc == buttonCustom3)
			{
				SPI_FLASH_BufferRead(cmd_code,BUTTON_CMD3_ADDR,201);
				codebufpoint = cmd_code;				
			}
			else if(pMsg->hWinSrc == buttonCustom4)
			{
				SPI_FLASH_BufferRead(cmd_code,BUTTON_CMD4_ADDR,201);
				codebufpoint = cmd_code;

			}
			else if(pMsg->hWinSrc == buttonCustom5)
			{
				SPI_FLASH_BufferRead(cmd_code,BUTTON_CMD5_ADDR,201);
				codebufpoint = cmd_code;

			}
			else if(pMsg->hWinSrc == buttonCustom6)
			{
				SPI_FLASH_BufferRead(cmd_code,BUTTON_CMD6_ADDR,201);
				codebufpoint = cmd_code;

			}
			else if(pMsg->hWinSrc == buttonCustom7)
			{
				SPI_FLASH_BufferRead(cmd_code,BUTTON_CMD7_ADDR,201);
				codebufpoint = cmd_code;
			}			
			else if(pMsg->hWinSrc == buttonRet)
			{
				last_disp_state = MORE_UI;
				Clear_more();
				draw_return_ui();		
			}

		}
		break;
	default:
	WM_DefaultProc(pMsg);
	}
}


void draw_More()
{
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != MORE_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = MORE_UI;
	}
	disp_state = MORE_UI;
	
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

	GUI_DispStringAt(creat_title_text(), 0, 0);
	

	hMoreWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbMoreWin, 0);
	switch(gCfgItems.MoreItem_pic_cnt)
	{
		case 7:
		   buttonCustom7= BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoreWnd, BUTTON_CF_SHOW, 0, 107);
		case 6:
		   buttonCustom6= BUTTON_CreateEx(LCD_WIDTH / 4,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoreWnd, BUTTON_CF_SHOW, 0, 106);
		case 5:
		   buttonCustom5= BUTTON_CreateEx(0,  imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoreWnd, BUTTON_CF_SHOW, 0, 105);
		case 4:
		   buttonCustom4= BUTTON_CreateEx(LCD_WIDTH * 3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoreWnd, BUTTON_CF_SHOW, 0, 108);
		case 3:
		   buttonCustom3= BUTTON_CreateEx(LCD_WIDTH / 2,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoreWnd, BUTTON_CF_SHOW, 0, 103);
		case 2:
		   buttonCustom2= BUTTON_CreateEx(LCD_WIDTH / 4,  0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoreWnd, BUTTON_CF_SHOW, 0, 102); 	   
		case 1:
		   buttonCustom1= BUTTON_CreateEx(0, 0,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoreWnd, BUTTON_CF_SHOW, 0, 101);
		break;
		default:break;
	 
	}

	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4,	imgHeight / 2,LCD_WIDTH / 4 - 2, imgHeight / 2 - 1,hMoreWnd, BUTTON_CF_SHOW, 0, 109);
		

	#if VERSION_WITH_PIC	
	switch(gCfgItems.MoreItem_pic_cnt)
	{
		case 7:
			BUTTON_SetBmpFileName(buttonCustom7, "bmp_custom7.bin",1);
		case 6:
			BUTTON_SetBmpFileName(buttonCustom6, "bmp_custom6.bin",1);
		case 5:
			BUTTON_SetBmpFileName(buttonCustom5, "bmp_custom5.bin",1);
		case 4:
			BUTTON_SetBmpFileName(buttonCustom4, "bmp_custom4.bin",1);
		case 3:
			BUTTON_SetBmpFileName(buttonCustom3, "bmp_custom3.bin",1);
		case 2:
			BUTTON_SetBmpFileName(buttonCustom2, "bmp_custom2.bin",1);
		case 1:
			BUTTON_SetBmpFileName(buttonCustom1, "bmp_custom1.bin",1);
			break;
		default:break;
	}	
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);
	
	switch(gCfgItems.MoreItem_pic_cnt)
	{
		case 7:
			BUTTON_SetBitmapEx(buttonCustom7, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		case 6:
			BUTTON_SetBitmapEx(buttonCustom6, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		case 5:
			BUTTON_SetBitmapEx(buttonCustom5, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		case 4:
			BUTTON_SetBitmapEx(buttonCustom4, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		case 3:
			BUTTON_SetBitmapEx(buttonCustom3, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		case 2:
			BUTTON_SetBitmapEx(buttonCustom2, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		case 1:
			BUTTON_SetBitmapEx(buttonCustom1, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
			break;
		default:break;
	}	
	
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);

	#endif
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetText(buttonRet,common_menu.text_back); 
	}
	
}

void Clear_more()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hMoreWnd))
	{
		WM_DeleteWindow(hMoreWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}


