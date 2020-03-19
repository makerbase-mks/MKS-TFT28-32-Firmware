#include "stm32f10x.h"
#include "gui.h"
#include "button.h"
#include "draw_bind.h"
#include "draw_ui.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "text.h"

#include "wifi_module.h"

typedef unsigned char bool;

#include "qr_encode.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif

static GUI_HWIN hBindWnd;


extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;


static BUTTON_Handle buttonRet, buttonReleaseBind;
static TEXT_Handle bind_or_not;

static uint8_t unbinding_flag = 0;

static void cbBindWin(WM_MESSAGE * pMsg) {

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
				if(pMsg->hWinSrc == buttonReleaseBind)
				{
					if(cloud_para.state == 0x12)
					{			
						Clear_Bind();
						draw_dialog(DIALOG_TYPE_UNBIND);
					}
					
				}
				else if(pMsg->hWinSrc == buttonRet)
				{
					Clear_Bind();
					draw_return_ui();
					
				}			
				
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}

void cloud_unbind()
{
	package_to_wifi(WIFI_CLOUD_UNBIND, (char *)0, 0);
	unbinding_flag = 1;
}


#define QRCODE_X 	20
#define QRCODE_Y 	35
#define QRCODE_WIDTH	160


static uint8_t id_mark = 0;


void DISPLAY_RENCODE_TO_TFT(u8 *qrcode_data)
{
	u8 i,j;
	u16 x,y,p;

	if(id_mark ==0)
	{
		EncodeData((char *)qrcode_data);

		id_mark = 1;
		
		
	}
	
	
	GUI_SetBkColor(GUI_WHITE);
	GUI_SetColor(GUI_WHITE);
	//TEXT_SetDefaultFont(&FONT_TITLE);
	GUI_FillRect(10, QRCODE_Y, 200, QRCODE_Y + 190);
	GUI_SetColor(GUI_BLACK);
	
	if(m_nSymbleSize*2>QRCODE_WIDTH)	
	{
		return;
	}
	
	for(i=0;i<40;i++)
	{
		if((m_nSymbleSize*i*2) > QRCODE_WIDTH)	break;
	}
	
	p=(i-1)*2;//???
	
	//x=(QRCODE_WIDTH - m_nSymbleSize*p)/2 + QRCODE_X;
	x= QRCODE_X + 20;
	y= QRCODE_Y + 20;
	
	for(i=0;i<m_nSymbleSize;i++)
	{
		for(j=0;j<m_nSymbleSize;j++)
		{
			//USART1_SendData(m_byModuleData[j][i]);
			if(m_byModuleData[i][j]==1)
				GUI_FillRect(x+p*i,y+p*j,x+p*(i+1)-1,y+p*(j+1)-1);

		}
			
	}
	
}


//every key:22*23
void draw_bind()
{
		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	int row;

	
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != BIND_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = BIND_UI;
	}
	disp_state = BIND_UI;
	GUI_Clear();	
	
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
	
	//GUI_SetFont(&FONT_TITLE);
	
	//disp_wifi_state();
	
	
	//GUI_SetFont(&FONT_TITLE);

	//GUI_SetColor(gCfgItems.state_text_color);

	//GUI_DrawRect(1, titleHeight + 1, LCD_WIDTH  - 3, LCD_HEIGHT - 3);

	//GUI_DispStringAt("????->??", 0, 0);
	
	hBindWnd = WM_CreateWindow(LCD_WIDTH * 3 / 4 , titleHeight, LCD_WIDTH / 4 - 1, imgHeight - 1, WM_CF_SHOW, cbBindWin, 0);
	
	//if(gCfgItems.cloud_enable)
	
	bind_or_not = TEXT_CreateEx(0, 15, 78, 25, hBindWnd, WM_CF_SHOW, TEXT_CF_LEFT,  GUI_ID_TEXT2, "");

	TEXT_SetTextAlign(bind_or_not, GUI_TA_VCENTER | GUI_TA_HCENTER);
	
	buttonRet = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hBindWnd, BUTTON_CF_SHOW, 0, alloc_win_id());



	if(gCfgItems.language == LANG_ENGLISH)
	{		
		buttonReleaseBind = BUTTON_CreateEx(0 , 50, LCD_WIDTH / 4, 40, hBindWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	
		//BUTTON_SetText(buttonReleaseBind, "unbind ");
	}
	else if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		buttonReleaseBind = BUTTON_CreateEx(0 , 50, LCD_WIDTH / 4, 40, hBindWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	
		//BUTTON_SetText(buttonReleaseBind, "  ½â½‰");
	}
	else
	{
		buttonReleaseBind = BUTTON_CreateEx(0 , 50, LCD_WIDTH / 4, 40, hBindWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	
		//BUTTON_SetText(buttonReleaseBind, "  ½â°ó  ");
	}
	BUTTON_SetBmpFileName(buttonReleaseBind,NULL,1);
	BUTTON_SetTextAlign(buttonReleaseBind, GUI_TA_VCENTER | GUI_TA_HCENTER);		
	BUTTON_SetBkColor(buttonReleaseBind, BUTTON_CI_UNPRESSED, GUI_BLUE);
	BUTTON_SetBkColor(buttonReleaseBind, BUTTON_CI_PRESSED, GUI_LIGHTBLUE);
	
	
	
	DISPLAY_RENCODE_TO_TFT((u8 *)cloud_para.id);
	GUI_DispStringAt((const char *)cloud_para.id, 15, 200);
	
#if VERSION_WITH_PIC	
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",1);
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);

#endif
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetText(buttonRet,common_menu.text_back);
	}

	BUTTON_SetText(buttonReleaseBind,cloud_menu.unbind);
	disp_bind_state();


	
}


void disp_bind_state()
{
	TEXT_SetTextColor(bind_or_not, gCfgItems.state_text_color);
	TEXT_SetBkColor(bind_or_not,  gCfgItems.state_background_color);

	
	if(cloud_para.state != 0x12)
	{
		unbinding_flag = 0;
	}
	if(unbinding_flag)
	{
		TEXT_SetText(bind_or_not, cloud_menu.unbinding);
		WM_DisableWindow(buttonReleaseBind);
	}
	else
	{
		if(cloud_para.state == 0x10)
		{
			TEXT_SetText(bind_or_not,cloud_menu.disconnected);
		}
		else if(cloud_para.state == 0x11)
		{
			TEXT_SetText(bind_or_not, cloud_menu.unbinded);
		}
		else if(cloud_para.state == 0x12)
		{
			TEXT_SetText(bind_or_not, cloud_menu.binded);
		}
		else
		{
			TEXT_SetText(bind_or_not, cloud_menu.disable);
		}
	}

	
	if((cloud_para.state == 0x12) && (unbinding_flag == 0))
	{
		WM_EnableWindow(buttonReleaseBind);
		
	}
	else// if(WM_IsWindow(buttonReleaseBind))
	{
		//BUTTON_Delete(buttonReleaseBind);
		WM_DisableWindow(buttonReleaseBind);
	}
	
	

	if(cloud_para.id[0] != 0)
	{		
		
		if(id_mark == 0)
		{
			DISPLAY_RENCODE_TO_TFT((u8 *)cloud_para.id);
			GUI_DispStringAt((const char *)cloud_para.id, 10, 200);
		}

		
	}
	else
		id_mark = 0;
	
	
}

static char last_cloud_state = 0;
void refresh_bind_ui()
{
	if((last_cloud_state != cloud_para.state) || unbinding_flag)
	{
		disp_bind_state();
		last_cloud_state = cloud_para.state;
	}
	if(cloud_para.id[0] != 0)
	{		
		
		if(id_mark == 0)
		{
			DISPLAY_RENCODE_TO_TFT((u8 *)cloud_para.id);
			GUI_DispStringAt((const char *)cloud_para.id, 10, 200);
		}

		
	}
	else
		id_mark = 0;
}


void Clear_Bind()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hBindWnd))
	{
		WM_DeleteWindow(hBindWnd);
	//	GUI_Exec();
	}
	
	//GUI_Clear();
}



