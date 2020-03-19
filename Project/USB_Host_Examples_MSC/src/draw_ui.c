#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_ui.h"
#include "draw_ready_print.h"
#include "pic.h"
#include "gcode.h"
#include "printer.h"
#include "draw_leveling.h"
#include "draw_tool.h"
#include "draw_bind.h"
#include "draw_babystep.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmpreheat;
extern GUI_CONST_STORAGE GUI_BITMAP bmadd;
extern GUI_CONST_STORAGE GUI_BITMAP bmdec;
extern GUI_CONST_STORAGE GUI_BITMAP bmdir;
extern GUI_CONST_STORAGE GUI_BITMAP bmextru;
extern GUI_CONST_STORAGE GUI_BITMAP bmfile;
extern GUI_CONST_STORAGE GUI_BITMAP bmmov;
extern GUI_CONST_STORAGE GUI_BITMAP bmpageDown;
extern GUI_CONST_STORAGE GUI_BITMAP bmpageUp;
extern GUI_CONST_STORAGE GUI_BITMAP bmprinting;
extern GUI_CONST_STORAGE GUI_BITMAP bmreturn;
extern GUI_CONST_STORAGE GUI_BITMAP bmset;
extern GUI_CONST_STORAGE GUI_BITMAP bmzero;

DISP_STATE_STACK disp_state_stack;

char BMP_PIC_X = 0 ;
char BMP_PIC_Y = 17;

static   GUI_HWIN hMainWnd;
static  BUTTON_Handle button4;//button1, button2, button3, button4;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;
extern uint8_t curFileName[100];

extern GUI_CONST_STORAGE GUI_BITMAP bmlogo;
extern GUI_CONST_STORAGE GUI_BITMAP bmpreheat;

#if 0
#if VERSION_WITH_PIC	
GUI_CONST_STORAGE GUI_BITMAP bmp_preHeat = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_preHeatIcon,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_return = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_return,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_file = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_file,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_dir = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_dir,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_print = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_print,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_add = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_add,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_dec = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_dec,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_pageDown = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_pageDown,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_pageUp = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_pageUp,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_set = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_set,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_extru = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_extru,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_zero = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_zero,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_move = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_move,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

GUI_CONST_STORAGE GUI_BITMAP bmp_wifi = {
	  42, /* XSize */
	  36, /* YSize */
	  8, /* BytesPerLine */
	  16, /* BitsPerPixel */
	  (unsigned char *)gImage_wifi,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_BMP565
};

#if 0
GUI_CONST_STORAGE GUI_BITMAP bmp_logo = {
	  320, /* XSize */
	  240, /* YSize */
	  40, /* BytesPerLine (((bits) + 31) / 32 * 4)*/
	  8, /* BitsPerPixel */
	  (unsigned char *)gImage_logo,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_RLE8
};




GUI_CONST_STORAGE GUI_BITMAP bmp_logo_a = {
	  320, /* XSize */
	  120, /* YSize */
	  320, /* BytesPerLine (((bits) + 31) / 32 * 4)*/
	  8, /* BitsPerPixel */
	  (unsigned char *)gImage_logo_a,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_RLE8
};

GUI_CONST_STORAGE GUI_BITMAP bmp_logo_b = {
	  240, /* XSize */
	  119, /* YSize */
	  240, /* BytesPerLine (((bits) + 31) / 32 * 4)*/
	  8, /* BitsPerPixel */
	  (unsigned char *)gImage_logo_b,  /* Pointer to picture data */
	  0  /* Pointer to palette */
	 ,GUI_DRAW_RLE8
};
#endif
#endif
#endif

static void cbMainWin(WM_MESSAGE * pMsg) {
	struct PressEvt *press_event;
switch (pMsg->MsgId) {
case WM_PAINT:
//GUI_SetBkColor(GUI_BLACK);
//GUI_Clear();
//GUI_DispString("window");
	break;
case WM_TOUCH:
 	press_event = (struct PressEvt *)pMsg->Data.p;
	
	break;
case WM_TOUCH_CHILD:
 	
	break;



case WM_NOTIFY_PARENT:
	if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
	{
		press_event = (struct PressEvt *)pMsg->Data.p;
		/*if(pMsg->hWinSrc == button4)
		{
			clear_main_ui();
			draw_Language();
		}
		else if(pMsg->hWinSrc == button1)
		{
			gCfgItems.language = 1;
			disp_sel_lang();
			
		}
		else if(pMsg->hWinSrc == button2)
		{
			gCfgItems.language = 2;
			disp_sel_lang();
				
		}
		else if(pMsg->hWinSrc == button3)
		{
			gCfgItems.language = 3;
			disp_sel_lang();
				
		}*/
		if(pMsg->hWinSrc == button4)
		{
			if(gCfgItems.language == LANG_ENGLISH)
				gCfgItems.language = LANG_SIMPLE_CHINESE;
			else if(gCfgItems.language == LANG_SIMPLE_CHINESE)
				gCfgItems.language = LANG_ENGLISH;
			
			bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);	
			disp_sel_lang();
		}
		

	}
	break;

default:
	WM_DefaultProc(pMsg);
}
}


void draw_main_ui()
{
	int index; 
	int x_off = 0, y_off = 0;
	int _x, _y;
	uint16_t *p_index;
	int i;
	
	disp_state_stack._disp_index = 0;
	memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
	disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINT_READY_UI;
	
	disp_state = MAIN_UI;
	
	//GUI_SetFont(&FONT_TITLE);
	/*if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		GUI_SetFont(&GUI_FontHZ16);
	}
	else
	{
		GUI_SetFont(&FONT_TITLE);
	}*/
	GUI_SetBkColor(gCfgItems.background_color);
	GUI_Clear();

	GUI_SetColor(gCfgItems.title_color);
	//GUI_FillRect(0, 0, LCD_WIDTH - 1, LCD_HEIGHT/2 - 1);	

	
	#if VERSION_WITH_PIC	
	for(index = 0; index < 10; index ++)
	{
		Pic_Logo_Read("bmp_logo.bin", bmp_public_buf, 15360);

		
		i = 0;
		//GUI_DrawBitmap(&bmp_struct, x_off, y_off);
		
		

		LCD_setWindowArea(0, y_off * 24, 320, 24);
		LCD_SetCursor(0, y_off * 24);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */	
		for(_y = y_off * 24; _y < (y_off + 1) * 24; _y++)
		{
			for (x_off = 0; x_off < 320; x_off++) 
			{
				p_index = (uint16_t *)(&bmp_public_buf[i]);						
				LCD_WriteRAM(*p_index);
				i += 2;
				
			}
			if(i >= 15360)
					break;
		}
		y_off++;		

		
	}
	LCD_setWindowArea(0, 0, 319, 239);
	
	#endif	
/*	
	if(gCfgItems.custom_pic_flag != 1)
	{
		GUI_SetBkColor(GUI_RET_BUTTON_COLOR);
		GUI_DispStringAt("MKS TFT28", LCD_WIDTH / 2 + 20, LCD_HEIGHT  / 4);
		GUI_DispStringAt("V1.0.2", LCD_WIDTH / 2 + 20, LCD_HEIGHT  / 4 + 25);
		GUI_DispStringAt("By makerbase", LCD_WIDTH / 2 + 20, LCD_HEIGHT  / 4 + 50);
		GUI_SetBkColor(gCfgItems.background_color);
	}

	BUTTON_SetDefaultTextAlign(GUI_TA_BOTTOM | GUI_TA_HCENTER);
	BUTTON_SetDefaultFont(&FONT_BUTTON);
*/	
/*//2015.5.13
	BUTTON_SetDefaultBkColor(GUI_BUTTON_COLOR, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultBkColor(GUI_BUTTON_COLOR, BUTTON_CI_PRESSED);
	BUTTON_SetDefaultTextColor(GUI_WHITE, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultTextColor(GUI_WHITE, BUTTON_CI_PRESSED);
*/
/*
	BUTTON_SetDefaultBkColor(gCfgItems.background_color, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultBkColor(gCfgItems.background_color, BUTTON_CI_PRESSED);
	BUTTON_SetDefaultTextColor(GUI_WHITE, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultTextColor(GUI_WHITE, BUTTON_CI_PRESSED);
	
	hMainWnd = WM_CreateWindow(0, LCD_HEIGHT / 2, LCD_WIDTH, LCD_HEIGHT / 2 , WM_CF_SHOW, cbMainWin, 0);
	//button1 = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 1, LCD_HEIGHT / 2 - 1,hMainWnd, BUTTON_CF_SHOW, 0, 101);
	//button2 = BUTTON_CreateEx(LCD_WIDTH / 4, 0, LCD_WIDTH / 4 - 1, LCD_HEIGHT / 2 - 1,hMainWnd, BUTTON_CF_SHOW, 0, 102);
	//button3 = BUTTON_CreateEx(LCD_WIDTH / 2, 0, LCD_WIDTH / 4 - 1, LCD_HEIGHT / 2 - 1,hMainWnd, BUTTON_CF_SHOW, 0, 103);
	button4 = BUTTON_CreateEx(270, 50,43, 43,hMainWnd, BUTTON_CF_SHOW, 0, 104);
	//BUTTON_SetText(button1, "简体");
	//BUTTON_SetText(button2, "繁体");
	///BUTTON_SetText(button3, "EN");
	//BUTTON_SetText(button4, "More");
	BUTTON_SetText(button4, "EN");

	BUTTON_SetTextAlign(button4, GUI_TA_VCENTER| GUI_TA_HCENTER);
	//BUTTON_SetBkColor(button1, BUTTON_CI_UNPRESSED, 0x404000);
	//BUTTON_SetBkColor(button1, BUTTON_CI_PRESSED, GUI_LIGHTBLUE);
	//BUTTON_SetBkColor(button2, BUTTON_CI_PRESSED, GUI_LIGHTBLUE);
	//BUTTON_SetBkColor(button3, BUTTON_CI_PRESSED, GUI_LIGHTBLUE);

	disp_sel_lang();
*/	
	//BUTTON_SetBkColor(button4, BUTTON_CI_UNPRESSED, GUI_DARKGREEN2);
	//BUTTON_SetBkColor(button4, BUTTON_CI_PRESSED, GUI_DARKGREEN2);
}

#define MAX_TITLE_LEN	28

void titleText_cat(char *str, int strSize, char *addPart)
{
	if(str == 0 || addPart == 0)
	{
		return;
	}

	if(strlen(str) + strlen(addPart) >= strSize)
	{
		return;
	}

	strcat(str, addPart);
}

static char TitleText[30];
char *getDispText(int index)
	{
		memset(TitleText, 0, sizeof(TitleText));
		
		switch(disp_state_stack._disp_state[index])
		{
			case PRINT_READY_UI:
				strcpy(TitleText, main_menu.title);
	
				break;
			case TOOL_UI:
				strcpy(TitleText, main_menu.tool);
	
				break;	
			case PRINT_FILE_UI:
				strcpy(TitleText, file_menu.title);
	
				break;
	
			case PRINTING_UI:
				if(disp_state_stack._disp_state[disp_state_stack._disp_index] == PRINTING_UI
				|| disp_state_stack._disp_state[disp_state_stack._disp_index] == OPERATE_UI
				|| disp_state_stack._disp_state[disp_state_stack._disp_index] == PAUSE_UI)
				{
					strcpy(TitleText, common_menu.print_special_title);
				}
				else
				{
					strcpy(TitleText, printing_menu.title);
				}

	
				break;
	
			case MOVE_MOTOR_UI:
				strcpy(TitleText, move_menu.title);
	
				break;
	
			case OPERATE_UI:
				if(disp_state_stack._disp_state[disp_state_stack._disp_index] == PRINTING_UI
				|| disp_state_stack._disp_state[disp_state_stack._disp_index] == OPERATE_UI
				|| disp_state_stack._disp_state[disp_state_stack._disp_index] == PAUSE_UI)
				{
					strcpy(TitleText, common_menu.operate_special_title);
				}
				else
				{
					strcpy(TitleText, operation_menu.title);
				}

	
				break;
	
			case PAUSE_UI:
				if(disp_state_stack._disp_state[disp_state_stack._disp_index] == PRINTING_UI
				|| disp_state_stack._disp_state[disp_state_stack._disp_index] == OPERATE_UI
				|| disp_state_stack._disp_state[disp_state_stack._disp_index] == PAUSE_UI)
				{
					strcpy(TitleText, common_menu.pause_special_title);
				}
				else
				{
					strcpy(TitleText, pause_menu.title);
				}

	
				break;
	
			case EXTRUSION_UI:
				strcpy(TitleText, extrude_menu.title);
	
				break;
	
			case CHANGE_SPEED_UI:
				strcpy(TitleText, speed_menu.title);
	
				break;
				
			case FAN_UI:
				strcpy(TitleText, fan_menu.title);
	
				break;
				
			case PRE_HEAT_UI:
				if((disp_state_stack._disp_state[disp_state_stack._disp_index - 1] == OPERATE_UI)
					||(disp_state_stack._disp_state[disp_state_stack._disp_index - 1] == PRINT_MORE_UI))
				{
					strcpy(TitleText,preheat_menu.adjust_title);
	
				}
				else
				{
					strcpy(TitleText, preheat_menu.title);
	
				} 		
				break;
	
			case SET_UI:
				strcpy(TitleText, set_menu.title);
	
				break;
	
			case ZERO_UI:
				strcpy(TitleText, home_menu.title);
	
				break;
	
			case SPRAYER_UI:
	
				break;
	
			case MACHINE_UI:
	
				break;
	
			case LANGUAGE_UI:
				strcpy(TitleText, language_menu.title);
	
				break;
	
			case ABOUT_UI:
				strcpy(TitleText, about_menu.title);
	
				break;
	
			case LOG_UI:
	
				break;
	
			case DISK_UI:
	
				strcpy(TitleText, filesys_menu.title);
				break;
	
			case DIALOG_UI:
				strcpy(TitleText, common_menu.dialog_confirm_title);
	
				break;	
	
			case WIFI_UI:
				strcpy(TitleText, wifi_menu.title);
	
				break;	
			case MORE_UI:
			case PRINT_MORE_UI:
				strcpy(TitleText, more_menu.title);
	
				break;	
			case FILAMENTCHANGE_UI:
				strcpy(TitleText, filament_menu.title); 		
				break;	
			case LEVELING_UI:
				strcpy(TitleText, leveling_menu.title); 		
				break;			
			case BIND_UI:
				strcpy(TitleText, cloud_menu.title);			
				break;	
			case BABY_STEP_UI:
				strcpy(TitleText, printing_more_menu.babystep);			
				break;
			default:
				break;
		}
	
		return TitleText;
	}

static char titleText[100] = {0};

char *creat_title_text()
{
	int index = 0,ix,cnt;
	
	char *tmpText = 0;
	
	unsigned char tmpCurFileStr[20];
	

	
	memset(tmpCurFileStr, 0, sizeof(tmpCurFileStr));
	cutFileName(curFileName, 16, 16, tmpCurFileStr);

	memset(titleText, 0, sizeof(titleText));
	
	while(index <= disp_state_stack._disp_index)
	{
		
		tmpText = getDispText(index);
		if((*tmpText == 0) || (tmpText == 0))
		{
			index++;
			continue;
		}
		
		titleText_cat(titleText, sizeof(titleText), tmpText);
		if(index < disp_state_stack._disp_index)
		{
			titleText_cat(titleText, sizeof(titleText), ">");
		}
		
		index++;
	}

		
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] == PRINTING_UI
		|| disp_state_stack._disp_state[disp_state_stack._disp_index] == OPERATE_UI
		|| disp_state_stack._disp_state[disp_state_stack._disp_index] == PAUSE_UI)
	{
		titleText_cat(titleText, sizeof(titleText), ":");
		titleText_cat(titleText, sizeof(titleText), tmpCurFileStr);	
	}

	if(strlen(titleText) > MAX_TITLE_LEN)
	{
		memset(titleText, 0, sizeof(titleText));
	
		tmpText = getDispText(0);
		if(*tmpText != 0)
		{
			titleText_cat(titleText, sizeof(titleText), tmpText);
			titleText_cat(titleText, sizeof(titleText), ">...>");
			tmpText = getDispText(disp_state_stack._disp_index);
			if(*tmpText != 0)
			{
				titleText_cat(titleText, sizeof(titleText), tmpText);
			}
		}
		
	}

	return titleText;
}

void disp_sel_lang()
{
	if(gCfgItems.language == LANG_ENGLISH)
	{
		BUTTON_SetBkColor(button4, BUTTON_CI_UNPRESSED, GUI_FOCUS_CLOLOR);	
		//BUTTON_SetBkColor(button1, BUTTON_CI_UNPRESSED, GUI_BLUE);	
		//BUTTON_SetBkColor(button3, BUTTON_CI_UNPRESSED, GUI_BLUE);	
	}
	/*else if(gCfgItems.language == 3)
	{
		BUTTON_SetBkColor(button3, BUTTON_CI_UNPRESSED, GUI_FOCUS_CLOLOR);	
		BUTTON_SetBkColor(button1, BUTTON_CI_UNPRESSED, GUI_BLUE);	
		BUTTON_SetBkColor(button2, BUTTON_CI_UNPRESSED, GUI_BLUE);	
	}*/
	else
	{
		//BUTTON_SetBkColor(button1, BUTTON_CI_UNPRESSED, GUI_FOCUS_CLOLOR);	
		BUTTON_SetBkColor(button4, BUTTON_CI_UNPRESSED, GUI_BUTTON_COLOR);	
		//BUTTON_SetBkColor(button3, BUTTON_CI_UNPRESSED, GUI_BLUE);	
	}
		
}

void clear_main_ui()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hMainWnd))
	{
		WM_DeleteWindow(hMainWnd);
		GUI_Exec();
	}
	GUI_Exec();
	
}

void clear_cur_ui()
{
	last_disp_state = disp_state_stack._disp_state[disp_state_stack._disp_index];
	
	switch(disp_state_stack._disp_state[disp_state_stack._disp_index])
	{
		case PRINT_READY_UI:			
			Clear_ready_print();
			break;

		case PRINT_FILE_UI:
			Clear_print_file();
			break;

		case PRINTING_UI:
			Clear_printing();
			break;

		case MOVE_MOTOR_UI:
			Clear_move_motor();
			break;

		case OPERATE_UI:
			Clear_operate();
			break;

		case PAUSE_UI:
			Clear_pause();
			break;

		case EXTRUSION_UI:
			Clear_extrusion();
			break;

		case PRE_HEAT_UI:
			Clear_preHeat();
			break;

		case CHANGE_SPEED_UI:
			Clear_changeSpeed();
			break;

		case FAN_UI:
			Clear_fan();
			break;

		case SET_UI:
			Clear_Set();
			break;

		case ZERO_UI:
			Clear_Zero();
			break;
/*
		case SPRAYER_UI:
			Clear_Sprayer();
			break;
*/
/*		case MACHINE_UI:
			Clear_Machine();
			break;
*/
		case LANGUAGE_UI:
			Clear_Language();
			break;

		case ABOUT_UI:
			Clear_About();
			break;

		/*case LOG_UI:
			Clear_Connect();
			break;*/

		case WIFI_UI:
			Clear_Wifi();
			break;
			
		case MORE_UI:
			Clear_more();
			break;
			
		case FILETRANSFER_UI:
			Clear_fileTransfer();
			break;
			
		case FILETRANSFERSTATE_UI:
			Clear_WifiFileTransferdialog();
			break;
			
		case DIALOG_UI:
			Clear_dialog();
			break;
		case PRINT_MORE_UI:
			Clear_Printmore();
			break;
		case LEVELING_UI:
			Clear_Leveling();
			break;
		case TOOL_UI:
			#if SIMPLE_DISPLAY != 0
			Clear_Tool();
			#endif
			break;
		case BIND_UI:
			Clear_Bind();
			break;
		case BABY_STEP_UI:
			Clear_babyStep();
			break;
		default:
			break;
	}
	GUI_Clear();
}

void draw_return_ui()
{
	if(disp_state_stack._disp_index > 0)
	{
		disp_state_stack._disp_index--;
		
		switch(disp_state_stack._disp_state[disp_state_stack._disp_index])
		{
			case PRINT_READY_UI:
				Get_Temperature_Flg = 0;
				draw_ready_print();
				break;

			case PRINT_FILE_UI:
				draw_print_file();
				break;

			case PRINTING_UI:
				draw_printing();
				break;

			case MOVE_MOTOR_UI:
				draw_move_motor();
				break;

			case OPERATE_UI:
				draw_operate();
				break;

			case PAUSE_UI:
				draw_pause();
				break;

			case EXTRUSION_UI:
				draw_extrusion();
				break;

			case PRE_HEAT_UI:
				draw_preHeat();
				break;

			case CHANGE_SPEED_UI:
				draw_changeSpeed();
				break;

			case FAN_UI:
				draw_fan();
				break;

			case SET_UI:
				draw_Set();
				break;

			case ZERO_UI:
				draw_Zero();
				break;
/*
			case SPRAYER_UI:
				draw_Sprayer();
				break;
*/
/*			case MACHINE_UI:
				draw_Machine();
				break;
*/
			case LANGUAGE_UI:
				draw_Language();
				break;

			case ABOUT_UI:
				draw_About();
				break;

			/*case LOG_UI:
				draw_Connect();
				break;*/
/*
			case CALIBRATE_UI:
				draw_calibrate();
				break;
*/
			case WIFI_UI:
				draw_Wifi();
				break;
			case MORE_UI:
				draw_More();
				break;
			case PRINT_MORE_UI:
				draw_printmore();
				break;
			case FILAMENTCHANGE_UI:
				draw_FilamentChange();
				break;
			case LEVELING_UI:
				draw_leveling();
				break;
			case TOOL_UI:
				#if SIMPLE_DISPLAY != 0
				draw_tool();
				#endif
				break;
			case BIND_UI:
				draw_bind();
				break;
			case BABY_STEP_UI:
				draw_babyStep();
				break;
			default:
				break;
		}
	}

	
}

extern uint16_t readLcdId;
void gui_view_init()
{
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
	GUI_SetBkColor(gCfgItems.background_color);
	
	BUTTON_SetDefaultTextAlign(GUI_TA_BOTTOM | GUI_TA_HCENTER);
	//BUTTON_SetDefaultFont(&FONT_BUTTON);	
		if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		BUTTON_SetDefaultFont(&GUI_FontHZ16);
	}
	else
	{
		BUTTON_SetDefaultFont(&FONT_TITLE);
	}
	if(gCfgItems.language == LANG_COMPLEX_CHINESE)
	{
		TEXT_SetDefaultFont(&GUI_FontHZ16);
	}
	else
	{
		TEXT_SetDefaultFont(&FONT_TITLE);
	}
/*	
	BUTTON_SetDefaultBkColor(GUI_BUTTON_COLOR, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultBkColor(GUI_BUTTON_COLOR, BUTTON_CI_PRESSED);
	BUTTON_SetDefaultTextColor(GUI_WHITE, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultTextColor(GUI_WHITE, BUTTON_CI_PRESSED);
*/
#endif
	//HC-chen 2017.10.10
	if(readLcdId == 0x9341)
	{
		if(gCfgItems.OVERTURN_180!=0xee) //不翻转
		{
			gCfgItems.touch_adj_flag = 0;
			gCfgItems.touch_adj_xMin = 349;
			gCfgItems.touch_adj_xMax = 3969;
			gCfgItems.touch_adj_yMin = 3872;
			gCfgItems.touch_adj_yMax = 272;
		}
		else
		{
			gCfgItems.touch_adj_flag = 0;
			gCfgItems.touch_adj_xMin = 3969;
			gCfgItems.touch_adj_xMax = 349;
			gCfgItems.touch_adj_yMin = 272;
			gCfgItems.touch_adj_yMax = 3872;
		}
	}
	else if(readLcdId==0x1505 )
	{
		if(gCfgItems.OVERTURN_180!=0xee) //不翻转
		{
			gCfgItems.touch_adj_flag = 0;
			gCfgItems.touch_adj_xMin =395;
			gCfgItems.touch_adj_xMax =3867;
			gCfgItems.touch_adj_yMin = 179;
			gCfgItems.touch_adj_yMax = 3855;
		}
		else  //翻转
		{
			gCfgItems.touch_adj_flag = 0;
			gCfgItems.touch_adj_xMin = 3518;
			gCfgItems.touch_adj_xMax = 389;
			gCfgItems.touch_adj_yMin = 3770;
			gCfgItems.touch_adj_yMax = 284;	
		}
		
	}
	else if(readLcdId==0x8552)
	{
		if(gCfgItems.OVERTURN_180!=0xee)
		{
			gCfgItems.touch_adj_flag = 0;
			gCfgItems.touch_adj_xMin = 352;
			gCfgItems.touch_adj_xMax = 3924;
			gCfgItems.touch_adj_yMin = 3840;
			gCfgItems.touch_adj_yMax = 201;
		}
		else
		{
			gCfgItems.touch_adj_flag = 0;
			gCfgItems.touch_adj_xMin = 3924;
			gCfgItems.touch_adj_xMax = 352;
			gCfgItems.touch_adj_yMin = 201;
			gCfgItems.touch_adj_yMax = 3840;
		}		
	}
	else
	{
		if(gCfgItems.OVERTURN_180!=0xee)
		{
			gCfgItems.touch_adj_flag = 0;
			gCfgItems.touch_adj_xMin = 3518;
			gCfgItems.touch_adj_xMax = 389;
			gCfgItems.touch_adj_yMin = 284;
			gCfgItems.touch_adj_yMax = 3770;
		}
		else
		{
			gCfgItems.touch_adj_flag = 0;
			gCfgItems.touch_adj_xMin = 389;
			gCfgItems.touch_adj_xMax = 3518;
			gCfgItems.touch_adj_yMin = 3770;
			gCfgItems.touch_adj_yMax = 284;
		}
	}
	
	GUI_SetFont(&FONT_TITLE);
	BUTTON_SetDefaultFont(&FONT_TITLE);
	TEXT_SetDefaultFont(&FONT_TITLE);
	GUI_SetBkColor(gCfgItems.background_color);
	BUTTON_SetDefaultTextAlign(GUI_TA_BOTTOM | GUI_TA_HCENTER);

	BUTTON_SetDefaultBkColor(gCfgItems.background_color, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultBkColor(gCfgItems.background_color, BUTTON_CI_PRESSED);
	BUTTON_SetDefaultTextColor(gCfgItems.title_color, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultTextColor(gCfgItems.title_color, BUTTON_CI_PRESSED);
}
