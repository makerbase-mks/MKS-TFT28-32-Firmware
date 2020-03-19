#include "gui.h"
#include "button.h"
#include "text.h"
#include "draw_ui.h"
#include "draw_leveling.h"
#include "mks_tft_fifo.h"
#include "MKS_USART2_IT.h"
#include "sd_usr.h"
static GUI_HWIN hLevelingWnd;

extern TFT_FIFO gcodeCmdTxFIFO;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static BUTTON_Handle buttonAutoleveling,buttonleveling1,buttonleveling2,buttonleveling3,buttonleveling4,buttonleveling5,buttonRet;

uint8_t leveling_first_time=0;
extern volatile unsigned char *codebufpoint;
extern char cmd_code[201];

#if 0
static BUTTON_Handle buttonNext,buttonRet;
static TEXT_Handle textLevelingMsg;
uint8_t next_cnt=0;
uint8_t leveling_finish_flag = 0;
#endif

static void cbLevelingWin(WM_MESSAGE * pMsg) {

	uint8_t level_buf[80]={0};
	
	switch (pMsg->MsgId)
	{
		case WM_PAINT:
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
					leveling_first_time=0;
					Clear_Leveling();
					draw_return_ui();
				}
				else if(pMsg->hWinSrc == buttonleveling1)
				{
						if(leveling_first_time == 1)
						{
							leveling_first_time = 0;
							initFIFO(&gcodeCmdTxFIFO);
							memset(level_buf,0,sizeof(level_buf));
							sprintf(level_buf,"G28\n");
							pushFIFO(&gcodeCmdTxFIFO, level_buf);
						}
						//memset(level_buf,0,sizeof(level_buf));
						//sprintf(level_buf,"G91\n");
						//pushFIFO(&gcodeCmdTxFIFO, level_buf);					
						memset(level_buf,0,sizeof(level_buf));
						//HC-chen
						sprintf(level_buf,"G1 Z10 F%d\n",gCfgItems.leveling_z_speed);
						//sprintf(level_buf,"G1 Z%d F%d\n",gCfgItems.leveling_z_high,gCfgItems.leveling_z_speed);
						pushFIFO(&gcodeCmdTxFIFO, level_buf);						
						//memset(level_buf,0,sizeof(level_buf));
						//sprintf(level_buf,"G90\n");
						//pushFIFO(&gcodeCmdTxFIFO, level_buf);		
						memset(level_buf,0,sizeof(level_buf));
						sprintf(level_buf,"G1 X%d Y%d F%d\n",gCfgItems.leveling_point1_x,gCfgItems.leveling_point1_y,gCfgItems.leveling_xy_speed);
						pushFIFO(&gcodeCmdTxFIFO, level_buf);
						memset(level_buf,0,sizeof(level_buf));
						sprintf(level_buf,"G1 Z0\n");
						pushFIFO(&gcodeCmdTxFIFO, level_buf);						
				}
				else if(pMsg->hWinSrc == buttonleveling2)
				{
						if(leveling_first_time == 1)
						{
							leveling_first_time = 0;
							initFIFO(&gcodeCmdTxFIFO);
							memset(level_buf,0,sizeof(level_buf));
							sprintf(level_buf,"G28\n");
							pushFIFO(&gcodeCmdTxFIFO, level_buf);
						}
						//memset(level_buf,0,sizeof(level_buf));
						//sprintf(level_buf,"G91\n");
						//pushFIFO(&gcodeCmdTxFIFO, level_buf);					
						memset(level_buf,0,sizeof(level_buf));
						//HC-chen
						sprintf(level_buf,"G1 Z10 F%d\n",gCfgItems.leveling_z_speed);
						//sprintf(level_buf,"G1 Z%d F%d\n",gCfgItems.leveling_z_high,gCfgItems.leveling_z_speed);
						pushFIFO(&gcodeCmdTxFIFO, level_buf);						
						//memset(level_buf,0,sizeof(level_buf));
						//sprintf(level_buf,"G90\n");
						//pushFIFO(&gcodeCmdTxFIFO, level_buf);		
						memset(level_buf,0,sizeof(level_buf));
						sprintf(level_buf,"G1 X%d Y%d F%d\n",gCfgItems.leveling_point2_x,gCfgItems.leveling_point2_y,gCfgItems.leveling_xy_speed);
						pushFIFO(&gcodeCmdTxFIFO, level_buf);
						memset(level_buf,0,sizeof(level_buf));
						sprintf(level_buf,"G1 Z0\n");
						pushFIFO(&gcodeCmdTxFIFO, level_buf);							
				}
				else if(pMsg->hWinSrc == buttonleveling3)
				{
						if(leveling_first_time == 1)
						{
							leveling_first_time = 0;
							initFIFO(&gcodeCmdTxFIFO);
							memset(level_buf,0,sizeof(level_buf));
							sprintf(level_buf,"G28\n");
							pushFIFO(&gcodeCmdTxFIFO, level_buf);
						}
						//memset(level_buf,0,sizeof(level_buf));
						//sprintf(level_buf,"G91\n");
						//pushFIFO(&gcodeCmdTxFIFO, level_buf);					
						memset(level_buf,0,sizeof(level_buf));
						//HC-chen
						sprintf(level_buf,"G1 Z10 F%d\n",gCfgItems.leveling_z_speed);
						//sprintf(level_buf,"G1 Z%d F%d\n",gCfgItems.leveling_z_high,gCfgItems.leveling_z_speed);
						pushFIFO(&gcodeCmdTxFIFO, level_buf);						
						//memset(level_buf,0,sizeof(level_buf));
						//sprintf(level_buf,"G90\n");
						//pushFIFO(&gcodeCmdTxFIFO, level_buf);		
						memset(level_buf,0,sizeof(level_buf));
						sprintf(level_buf,"G1 X%d Y%d F%d\n",gCfgItems.leveling_point3_x,gCfgItems.leveling_point3_y,gCfgItems.leveling_xy_speed);
						pushFIFO(&gcodeCmdTxFIFO, level_buf);
						memset(level_buf,0,sizeof(level_buf));
						sprintf(level_buf,"G1 Z0\n");
						pushFIFO(&gcodeCmdTxFIFO, level_buf);						
				}		
				else if(pMsg->hWinSrc == buttonleveling4)
				{
						if(leveling_first_time == 1)
						{
							leveling_first_time = 0;
							initFIFO(&gcodeCmdTxFIFO);
							memset(level_buf,0,sizeof(level_buf));
							sprintf(level_buf,"G28\n");
							pushFIFO(&gcodeCmdTxFIFO, level_buf);
						}
						//memset(level_buf,0,sizeof(level_buf));
						//sprintf(level_buf,"G91\n");
						//pushFIFO(&gcodeCmdTxFIFO, level_buf);					
						memset(level_buf,0,sizeof(level_buf));
						//HC-chen
						sprintf(level_buf,"G1 Z10 F%d\n",gCfgItems.leveling_z_speed);
						//sprintf(level_buf,"G1 Z%d F%d\n",gCfgItems.leveling_z_high,gCfgItems.leveling_z_speed);
						pushFIFO(&gcodeCmdTxFIFO, level_buf);						
						//memset(level_buf,0,sizeof(level_buf));
						//sprintf(level_buf,"G90\n");
						//pushFIFO(&gcodeCmdTxFIFO, level_buf);		
						memset(level_buf,0,sizeof(level_buf));
						sprintf(level_buf,"G1 X%d Y%d F%d\n",gCfgItems.leveling_point4_x,gCfgItems.leveling_point4_y,gCfgItems.leveling_xy_speed);
						pushFIFO(&gcodeCmdTxFIFO, level_buf);
						memset(level_buf,0,sizeof(level_buf));
						sprintf(level_buf,"G1 Z0\n");
						pushFIFO(&gcodeCmdTxFIFO, level_buf);						
				}	
				else if(pMsg->hWinSrc == buttonleveling5)
				{
						if(leveling_first_time == 1)
						{
							leveling_first_time = 0;
							initFIFO(&gcodeCmdTxFIFO);
							memset(level_buf,0,sizeof(level_buf));
							sprintf(level_buf,"G28\n");
							pushFIFO(&gcodeCmdTxFIFO, level_buf);
						}
						//memset(level_buf,0,sizeof(level_buf));
						//sprintf(level_buf,"G91\n");
						//pushFIFO(&gcodeCmdTxFIFO, level_buf);					
						memset(level_buf,0,sizeof(level_buf));
						//HC-chen
						sprintf(level_buf,"G1 Z10 F%d\n",gCfgItems.leveling_z_speed);
						//sprintf(level_buf,"G1 Z%d F%d\n",gCfgItems.leveling_z_high,gCfgItems.leveling_z_speed);
						pushFIFO(&gcodeCmdTxFIFO, level_buf);						
						//memset(level_buf,0,sizeof(level_buf));
						//sprintf(level_buf,"G90\n");
						//pushFIFO(&gcodeCmdTxFIFO, level_buf);		
						memset(level_buf,0,sizeof(level_buf));
						sprintf(level_buf,"G1 X%d Y%d F%d\n",gCfgItems.leveling_point5_x,gCfgItems.leveling_point5_y,gCfgItems.leveling_xy_speed);
						pushFIFO(&gcodeCmdTxFIFO, level_buf);
						memset(level_buf,0,sizeof(level_buf));
						sprintf(level_buf,"G1 Z0\n");
						pushFIFO(&gcodeCmdTxFIFO, level_buf);						
				}					
			}
			break;
			
		default:break;
	}
}
void draw_leveling()
{
	uint8_t bf[80] = {0};
	
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != LEVELING_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = LEVELING_UI;
	}
	
	disp_state = LEVELING_UI;	

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
	
	hLevelingWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbLevelingWin, 0);

	buttonleveling1 = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLevelingWnd, BUTTON_CF_SHOW, 0, 301);
	buttonleveling2 = BUTTON_CreateEx(LCD_WIDTH / 4,	0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLevelingWnd, BUTTON_CF_SHOW, 0, 302);
	buttonleveling3 = BUTTON_CreateEx(LCD_WIDTH /2 , 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLevelingWnd, BUTTON_CF_SHOW, 0, 303);
	if(gCfgItems.leveling_point_number>3)
	{
		buttonleveling4 = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLevelingWnd, BUTTON_CF_SHOW, 0, 304);
	}
	if(gCfgItems.leveling_point_number>4)
	{
		buttonleveling5 = BUTTON_CreateEx(0 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLevelingWnd, BUTTON_CF_SHOW, 0, 305);
	}
	
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , imgHeight / 2, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLevelingWnd, BUTTON_CF_SHOW, 0, 309);

	BUTTON_SetBmpFileName(buttonleveling1, "bmp_leveling1.bin",1);
	BUTTON_SetBmpFileName(buttonleveling2, "bmp_leveling2.bin",1);
	BUTTON_SetBmpFileName(buttonleveling3, "bmp_leveling3.bin",1);
	if(gCfgItems.leveling_point_number>3)
	{
		BUTTON_SetBmpFileName(buttonleveling4, "bmp_leveling4.bin",1);
	}
	if(gCfgItems.leveling_point_number>4)
	{
		BUTTON_SetBmpFileName(buttonleveling5, "bmp_leveling5.bin",1);
	}
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);

	BUTTON_SetBitmapEx(buttonleveling1, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonleveling2, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	BUTTON_SetBitmapEx(buttonleveling3, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	if(gCfgItems.leveling_point_number>3)
	{
		BUTTON_SetBitmapEx(buttonleveling4, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	}
	if(gCfgItems.leveling_point_number>4)
	{
		BUTTON_SetBitmapEx(buttonleveling5, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	}
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct,BMP_PIC_X, BMP_PIC_Y);
	
	if(gCfgItems.multiple_language != 0)
	{
			BUTTON_SetText(buttonleveling1,leveling_menu.position1);
			BUTTON_SetText(buttonleveling2,leveling_menu.position2);
			BUTTON_SetText(buttonleveling3,leveling_menu.position3);
			if(gCfgItems.leveling_point_number>3)
				BUTTON_SetText(buttonleveling4,leveling_menu.position4);
			if(gCfgItems.leveling_point_number>4)
				BUTTON_SetText(buttonleveling5,leveling_menu.position5);
			BUTTON_SetText(buttonRet,common_menu.text_back);			
	}
}

void Clear_Leveling()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hLevelingWnd))
	{
		WM_DeleteWindow(hLevelingWnd);
		GUI_Exec();
	}

}
