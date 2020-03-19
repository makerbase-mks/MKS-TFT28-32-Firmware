#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_printing.h"
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

int8_t curFilePath[30];

PRINT_TIME print_time;

FIL *srcfp;

static GUI_HWIN hPrintingWnd, hRetDlgWnd;
static PROGBAR_Handle printingBar;
static TEXT_Handle printTimeLeft, printSpeed, sprayTem1, sprayTem2, BedTem, fanSpeed;
static BUTTON_Handle button1, buttonE1, buttonE2, buttonFan, buttonBed, buttonOk, buttonCancle, btnRetMainPage;
static TEXT_Handle printRetDlgText;

static FIL curFile;


extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;
//extern volatile RECOVER_SD rec_sd;

static print_delay(int nCount)
{
	int i, j;
	for(i = nCount; i > 0; i--)
		for(j = 0; j < 120; j++);
}


static void cbRetDlgWin(WM_MESSAGE * pMsg)
{
	int8_t sel_item;
	int8_t i;
	//int8_t sel_file[30];

	//struct PressEvt *press_event;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_SetBkColor(gCfgItems.background_color);
		GUI_Clear();
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
				//hAlertWin = GUI_CreateDialogBox(aDialogAlert, GUI_COUNTOF(aDialogAlert), _cbAlert, 0, 0, 0);
				last_disp_state = PRINTING_UI;
				BUTTON_Delete(buttonOk);
				BUTTON_Delete(buttonCancle);
				if(WM_IsWindow(hRetDlgWnd))
				{
					WM_DeleteWindow(hRetDlgWnd);
				}
				Clear_printing();
				
				draw_ready_print();
				
			}
			else if(pMsg->hWinSrc == buttonCancle)
			{			
				if(WM_IsWindow(hRetDlgWnd))
				{
					BUTTON_Delete(buttonOk);
					BUTTON_Delete(buttonCancle);
					WM_DeleteWindow(hRetDlgWnd);
					//WM_EnableWindow(hPrintFileWnd);
					GUI_Exec();
				}
				
			}
		}
	}
}

static void cbPrintingWin(WM_MESSAGE * pMsg) {
uint8_t buf[10];

struct PressEvt *press_event;
switch (pMsg->MsgId)
{
	case WM_PAINT:
		/*
		GUI_SetColor(gCfgItems.state_background_color);
		GUI_FillRect(1, 1, LCD_WIDTH *3 / 4 - 3, imgHeight /2 - 3);
		GUI_FillRect(1, imgHeight/2 , LCD_WIDTH , imgHeight );
		GUI_SetColor(gCfgItems.state_text_color);
		GUI_DrawRect(0, 1, LCD_WIDTH *3 / 4 - 2, imgHeight /2 - 3);
		GUI_DrawRect(0, imgHeight/2 + 1, LCD_WIDTH  -4, imgHeight - 2);
	*/
		GUI_SetColor(gCfgItems.state_background_color);
		GUI_DrawRect(0, 0, LCD_WIDTH *3 / 4 -2, imgHeight /2 - 1);
		GUI_SetColor(gCfgItems.state_background_color);
		GUI_FillRect(0, 0, LCD_WIDTH *3 / 4 -2, imgHeight /2 - 1);
				
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
			if(pMsg->hWinSrc == button1)
			{
				last_disp_state = PRINTING_UI;
				Clear_printing();
				if((printerStaus == pr_idle)  &&  (gCurFileState.totalSend == 100) )
				{
					f_close(srcfp);

					reset_file_info();
					reset_tx_fifo();
					Get_Temperature_Flg = 0;
					draw_ready_print();
				}
				else
				{
					draw_operate();
				}
			}
			/*else if(pMsg->hWinSrc == btnRetMainPage) 
			{
				 f_close(srcfp);

				reset_file_info();
				reset_tx_fifo();
				
				Clear_printing();
				draw_ready_print();
			}*/
			
			
		}
		break;
		
	default:
		WM_DefaultProc(pMsg);
	}
}
//int once_flag = 0;


void draw_printing()
{
#if 1
//	int titleHeight = 30;
//	int imgHeight = LCD_HEIGHT - titleHeight;	
	//int8_t buf[50] = {0};
	FRESULT res;
	int i;

	disp_state_stack._disp_index = 0;
	memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
	disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINTING_UI;

	disp_state = PRINTING_UI;

	//GUI_SetFont(&GUI_Font8x18);
	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
	//GUI_SetFont(&FONT_TITLE);
	#if 0
	if(gCfgItems.language == LANG_ENGLISH)
	{
		GUI_SetFont(&FONT_TITLE);
	}
	else
	{
		GUI_SetFont(&GUI_FontHZ16);
	}
	#endif

	//sprintf(buf, "正在打印:%s", curFileName);
	//GUI_DispStringAt(buf, 0, 0);
	GUI_UC_SetEncodeNone();
	GUI_SetFont(&GUI_FontHZ16);
	GUI_DispStringAt(creat_title_text(), 0, 0);
	GUI_SetFont(&FONT_TITLE);
	GUI_UC_SetEncodeUTF8();

	hPrintingWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight / 2, WM_CF_SHOW, cbPrintingWin, 0);

	printingBar = PROGBAR_CreateEx(10, 20, LCD_WIDTH * 3 / 4 - 20, 30, hPrintingWnd, WM_CF_SHOW, 0, 0);

	
	//PROGBAR_SetTextAlign(printingBar, int Align)
	
	button1 = BUTTON_CreateEx(LCD_WIDTH * 3 / 4 , 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hPrintingWnd, BUTTON_CF_SHOW, 0, 301);
	//button2 = BUTTON_CreateEx(LCD_WIDTH  / 4 + buttonWidth - 2,  imgHeight - buttonHeight,buttonWidth - 1, buttonHeight - 1, hPrintFileWnd, BUTTON_CF_SHOW, 0, 202);
	//button3 = BUTTON_CreateEx(LCD_WIDTH  / 4 + buttonWidth * 2 ,   imgHeight - buttonHeight, buttonWidth - 1, buttonHeight - 1, hPrintFileWnd, BUTTON_CF_SHOW, 0, 203);

	//TEXT_SetDefaultFont(&GUI_Font8x18);
	
	printTimeLeft = TEXT_CreateEx(10, imgHeight * 3/ 8 - 5, LCD_WIDTH -90, imgHeight / 10, hPrintingWnd, WM_CF_SHOW, TEXT_CF_LEFT,  GUI_ID_TEXT0, "");

	//printSpeed = TEXT_CreateEx(LCD_WIDTH * 3 / 8  + 30, imgHeight * 3/ 8 - 5, LCD_WIDTH * 3 / 8 - 35, imgHeight / 10, hPrintingWnd, WM_CF_SHOW, TEXT_CF_LEFT,  GUI_ID_TEXT2, "");

	sprayTem1 = TEXT_CreateEx(0, imgHeight - 30, 160, 25, hPrintingWnd, WM_CF_SHOW, TEXT_CF_HCENTER,  GUI_ID_TEXT2, "");

	sprayTem2 = TEXT_CreateEx(80, imgHeight - 30, 160, 25, hPrintingWnd, WM_CF_SHOW, TEXT_CF_HCENTER,  GUI_ID_TEXT2, "");

	BedTem = TEXT_CreateEx(160, imgHeight - 30, 80, 25, hPrintingWnd, WM_CF_SHOW, TEXT_CF_HCENTER,  GUI_ID_TEXT2, "");

	fanSpeed = TEXT_CreateEx(240, imgHeight - 30, 60, 25, hPrintingWnd, WM_CF_SHOW, TEXT_CF_HCENTER,  GUI_ID_TEXT2, "");

	PROGBAR_SetBarColor(printingBar, 0, GUI_GREEN);
	
	PROGBAR_SetTextColor(printingBar, 0, GUI_BLACK);
	PROGBAR_SetFont(printingBar, &GUI_FontHZ_fontHz14);
	
	//TEXT_SetBkColor(printTimeLeft, GUI_INVALID_COLOR);

	#if VERSION_WITH_PIC	
	BUTTON_SetBmpFileName(button1, "bmp_menu.bin",0);
	BUTTON_SetBitmapEx(button1, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);	
	if(gCfgItems.multiple_language != 0)
	{
		BUTTON_SetText(button1,printing_menu.option);
	}

	#endif
	

	start_print_time();
	
	disp_print_time();
	//disp_printing_speed();


	#if VERSION_WITH_PIC	
	//2015.05.16
	//GUI_SetColor(gCfgItems.state_background_color);
	//GUI_SetColor(gCfgItems.printingstate_word_background_color);
	//GUI_FillRect(1, 1, LCD_WIDTH *3 / 4 - 3, imgHeight /2 - 3);
	//GUI_FillRect(1, imgHeight/2 , LCD_WIDTH , titleHeight + imgHeight );	
	//2015.05.16
	//GUI_SetColor(gCfgItems.state_text_color);
	//GUI_SetColor(gCfgItems.printingstate_word_color);
	//GUI_DrawRect(0, 1, LCD_WIDTH *3 / 4 - 2, imgHeight /2 - 3);
	//GUI_DrawRect(0, titleHeight + imgHeight/2 + 1, LCD_WIDTH-2, titleHeight + imgHeight);
	

	Pic_Read("bmp_extru1_no_words.bin",bmp_public_buf);
	GUI_DrawBitmap(&bmp_struct,  5, titleHeight + imgHeight/2 + 2);
	//HC-chen
	if(gCfgItems.sprayerNum == 2 && gCfgItems.single_nozzle==0)
	{
		Pic_Read("bmp_extru2_no_words.bin",bmp_public_buf);
		GUI_DrawBitmap(&bmp_struct,  83, titleHeight + imgHeight/2 + 2);
	}
	if(gCfgItems.custom_bed_flag == 1)
	{
		Pic_Read("bmp_bed_no_words.bin",bmp_public_buf);
		GUI_DrawBitmap(&bmp_struct,  161, titleHeight + imgHeight/2 + 2);
	}
	Pic_Read("bmp_fan_no_words.bin",bmp_public_buf);
	GUI_DrawBitmap(&bmp_struct,  239, titleHeight + imgHeight/2 + 2);
	
	disp_sprayer_tem_printing();
	disp_bed_temp_printing();
	disp_fan_speed_printing();	
	#endif

	if(gCurFileState.file_open_flag != 0xaa)
	{
		int ret;
		
		reset_file_info();
		
		res = f_open(&curFile, curFileName, FA_OPEN_EXISTING | FA_READ);
		if(res == FR_OK)
		{
			
			gCurFileState.file_open_flag = 0xaa;

			bakup_file_path(curFileName, strlen(curFileName));
				
			/*gCurFileState.fileSize = f_size(&gCurFileState.curFile);

			printer_state = PRINTER_OPERATE_OK;

			print_task_flag = TASK_GOING;*/

			///srcfp = &gCurFileState.curFile;

			srcfp = &curFile;

			printerStaus = pr_working;

			//once_flag = 0;
			
			//chen 9.28
			//rec_sd.dir_ptr_bak=(unsigned char *)curFile.dir_ptr;
		

		}
		
	}
	else
	{
		//setProBarRate(get_printing_rate(srcfp));
		setProBarRate(gCurFileState.totalSend);
		//	PROGBAR_SetValue(printingBar,  );
	}

	#endif
	
}

void reset_print_time()
{
	//print_time.days = 0;
	print_time.hours = 0;
	print_time.minutes = 0;
	print_time.seconds = 0;
	print_time.ms_10 = 0;
	//print_time.start = 1;
}

void start_print_time()
{
	print_time.start = 1;
}

void stop_print_time()
{
	print_time.start = 0;
}

extern uint32_t rcv_ok_nums;
void disp_print_time()
{
	int8_t buf[30] = {0};
	float  z_pos_temp;
	
	TEXT_SetTextColor(printTimeLeft, gCfgItems.state_text_color);
	TEXT_SetBkColor(printTimeLeft,  gCfgItems.state_background_color);
	memset(buf, 0, sizeof(buf));
	//z_pos_temp = atof(gCfgItems.z_display_pos);
	sprintf(buf, "T: %d%d:%d%d:%d%d  Z:", print_time.hours/10, print_time.hours%10, print_time.minutes/10, print_time.minutes%10,  print_time.seconds/10, print_time.seconds%10);
	//sprintf(buf, "ok nums:  %d%d%d:%d%d", wan,qian, bai,shi,ge);
	strcat(buf,gCfgItems.z_display_pos);
	TEXT_SetText(printTimeLeft, buf);
}

void disp_sprayer_tem_printing()
{
	int8_t buf[30] = {0};
	volatile uint8_t i0 = 0;
	
	TEXT_SetTextColor(sprayTem1, gCfgItems.printingstate_word_background_color);
	TEXT_SetBkColor(sprayTem1,  gCfgItems.printingstate_word_color);
	TEXT_SetTextColor(sprayTem2, gCfgItems.printingstate_word_background_color);
	TEXT_SetBkColor(sprayTem2,  gCfgItems.printingstate_word_color);

	//#ifdef SPRAYER_NUM_LIMIT	
	//HC-chen
	if(gCfgItems.sprayerNum == 2 && gCfgItems.single_nozzle==0)
	//#endif
	{
		//2015.05.16
		//GUI_SetBkColor(gCfgItems.state_background_color);
		//sprintf(buf, "%3d/%d %3d/%d ", (uint16_t)gCfgItems.curSprayerTemp[0], (uint16_t)gCfgItems.desireSprayerTemp[0], (uint16_t)gCfgItems.curSprayerTemp[1], (uint16_t)gCfgItems.desireSprayerTemp[1]);
		//GUI_DispStringAt(buf, 10, titleHeight + imgHeight - 20);
		GUI_SetBkColor(gCfgItems.printingstate_word_background_color);
		#if 0
		for(i0 = 0;i0<7;i0++)
		{
			buf[i0] = ' '; 
		}
		
		GUI_DispStringAt(buf, 10, titleHeight + imgHeight - 20);
		#endif
		memset(buf, ' ', sizeof(buf));
		GUI_DispStringAt(buf, 10, titleHeight + imgHeight - 20);
		sprintf(buf, "%3d/%d", (uint16_t)gCfgItems.curSprayerTemp[0], (uint16_t)gCfgItems.desireSprayerTemp[0]);
		GUI_DispStringAt(buf, 10, titleHeight + imgHeight - 20);	
		#if 0
		for(i0 = 0;i0<7;i0++)
		{
			buf[i0] = ' '; 
		}
		GUI_DispStringAt(buf, 10+78, titleHeight + imgHeight - 20);
		#endif
		memset(buf,' ',sizeof(buf));
		GUI_DispStringAt(buf, 10+78, titleHeight + imgHeight - 20);		
		sprintf(buf, "%3d/%d", (uint16_t)gCfgItems.curSprayerTemp[1], (uint16_t)gCfgItems.desireSprayerTemp[1]);
		GUI_DispStringAt(buf, 10+78, titleHeight + imgHeight - 20);				
	}
	//#ifdef SPRAYER_NUM_LIMIT	
	else
	{
	//	sprintf(buf, "%d/%d", (uint16_t)gCfgItems.curSprayerTemp[0], (uint16_t)gCfgItems.desireSprayerTemp[0]);
		GUI_SetBkColor(gCfgItems.printingstate_word_background_color);
	#if 0
		for(i0 = 0;i0<7;i0++)
		{
			buf[i0] = ' '; 
		}
		GUI_DispStringAt(buf, 10, titleHeight + imgHeight - 20);	
		#endif
		memset(buf, ' ', sizeof(buf));
		GUI_DispStringAt(buf, 10, titleHeight + imgHeight - 20);
		sprintf(buf, "%3d/%d", (uint16_t)gCfgItems.curSprayerTemp[0], (uint16_t)gCfgItems.desireSprayerTemp[0]);
		GUI_DispStringAt(buf, 10, titleHeight + imgHeight - 20);
	}
	//#endif
	
	//TEXT_SetText(sprayTem, buf);
}

static int8_t fan_dir = 0;

#if VERSION_WITH_PIC

void disp_fan_move_printing()
{
	if(gCfgItems.fanOnoff == 1)
	{
		fan_dir++;
		if(fan_dir%2)
		{
			Pic_Read("bmp_fan_move.bin",bmp_public_buf);
			GUI_DrawBitmap(&bmp_struct,  239, titleHeight + imgHeight/2 + 2);
		}
		else
		{
			Pic_Read("bmp_fan_no_words.bin",bmp_public_buf);
			GUI_DrawBitmap(&bmp_struct,  239, titleHeight + imgHeight/2 + 2);
		}
	}
	disp_fan_speed_printing();
}
#endif

void disp_bed_temp_printing()
{
	int8_t buf[30] = {0};
	volatile uint8_t i1 = 0;	
	
	TEXT_SetTextColor(BedTem, gCfgItems.printingstate_word_background_color);
	TEXT_SetBkColor(BedTem,  gCfgItems.printingstate_word_color);
	//GUI_SetBkColor(gCfgItems.state_background_color);
	
	if(gCfgItems.custom_bed_flag == 1)
	{
		#if 0
		for(i1 = 0;i1<7;i1++)
		{
			buf[i1] = ' '; 
		}
		GUI_DispStringAt(buf, 165, titleHeight + imgHeight - 20);	
		#endif
		memset(buf, ' ', sizeof(buf));
		GUI_DispStringAt(buf, 165, titleHeight + imgHeight - 20);
		sprintf(buf, "%3d/%d", (uint16_t)gCfgItems.curBedTemp, (uint16_t)gCfgItems.desireBedTemp);
		GUI_DispStringAt(buf, 165, titleHeight + imgHeight - 20);
	}
	//TEXT_SetText(BedTem, buf);
}

void disp_fan_speed_printing()
{
	int8_t buf[10] = {0};
	volatile uint8_t i2 = 0;
	
	TEXT_SetTextColor(fanSpeed, gCfgItems.printingstate_word_background_color);
	TEXT_SetBkColor(fanSpeed,  gCfgItems.printingstate_word_color);
//	GUI_SetBkColor(gCfgItems.state_background_color);
#if 0
	for(i2 = 0;i2<5;i2++)
	{
		buf[i2] = ' '; 
	}
	GUI_DispStringAt(buf, 255, titleHeight + imgHeight - 20);
	#endif
	memset(buf, ' ', sizeof(buf));
	GUI_DispStringAt(buf, 255, titleHeight + imgHeight - 20);
	sprintf(buf, "%3d", gCfgItems.fanSpeed);
	if(gCfgItems.fanSpeed < 10)
		GUI_DispStringAt(buf, 255, titleHeight + imgHeight - 20);
	else if(gCfgItems.fanSpeed < 100)
		GUI_DispStringAt(buf, 260, titleHeight + imgHeight - 20);
	else if(gCfgItems.fanSpeed < 256)
		GUI_DispStringAt(buf, 265, titleHeight + imgHeight - 20);	
	//TEXT_SetText(fanSpeed, buf);
}

void setProBarValue(int added)
{
	if(added <= 0)
		return;
	
	gCurFileState.totalSend += added;
	if(disp_state == PRINTING_UI)
	{
		PROGBAR_SetValue(printingBar, gCurFileState.totalSend * 100 / gCurFileState.fileSize );
	}
	
}



void setProBarRate(int rate)
{
	if(rate <= 0)
		return;

	if((printerStaus != pr_idle)  &&  (rate == 100))
	{	
		rate = 99;
	}

	gCurFileState.totalSend = rate;

	
	if(disp_state == PRINTING_UI)
	{
		PROGBAR_SetValue(printingBar, rate );

	
		if((printerStaus == pr_idle)  &&  (rate == 100) )
		{
			if(print_time.start != 0)
			{
				stop_print_time();
				//btnRetMainPage = BUTTON_CreateEx(LCD_WIDTH / 2,  imgHeight * 3/ 8 - 15, 70 , 30,hPrintingWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
				
				#if VERSION_WITH_PIC	
				BUTTON_SetBmpFileName(button1, "bmp_return.bin",0);

				BUTTON_SetBitmapEx(button1, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
				#endif
				if(gCfgItems.multiple_language != 0)
				{
					BUTTON_SetText(button1,common_menu.text_back);
				}
				#if 0
				if(gCfgItems.multiple_language != 0)
				{
						switch(gCfgItems.language)
						{
							case LANG_SIMPLE_CHINESE:
								BUTTON_SetText(button1,BACK_TEXT_CN);					
								break;
							case LANG_COMPLEX_CHINESE:
								BUTTON_SetText(button1,BACK_TEXT_CN);								
								break;
							case LANG_ENGLISH:
								BUTTON_SetText(button1,BACK_TEXT_CN);				
								break;

							case LANG_RUSSIAN:
								BUTTON_SetText(button1,BACK_TEXT_RU);					
								break;
							case LANG_SPANISH:
								BUTTON_SetText(button1,BACK_TEXT_SP);								
								break;								
							#if defined (STM32F10X_HD)
							case LANG_JAPAN:
								BUTTON_SetText(button1,BACK_TEXT_CN);					
								break;							
							case LANG_GERMAN:
								BUTTON_SetText(button1,BACK_TEXT_CN);					
								break;
							case LANG_FRENCH:
								BUTTON_SetText(button1,BACK_TEXT_CN);					
								break;

							case LANG_GREEK:
								BUTTON_SetText(button1,BACK_TEXT_GR);				
								break;
							case LANG_TURKISH:
								BUTTON_SetText(button1,BACK_TEXT_TU);					
								break;
							case LANG_KOREAN:
								BUTTON_SetText(button1,BACK_TEXT_KR);					
								break;
							case LANG_ITALY:
								BUTTON_SetText(button1,BACK_TEXT_IT);					
								break;
							#endif
						}
				}
#endif
			//	once_flag = 1;
			//BUTTON_SetTextAlign(btnRetMainPage, GUI_TA_VCENTER | GUI_TA_HCENTER);
			}

		/*	if((printerStaus == pr_idle)  &&  (rate == 100) && (!WM_IsWindow(hRetDlgWnd)))
			{
				hRetDlgWnd = WM_CreateWindowAsChild(0, 0, LCD_WIDTH, imgHeight, hPrintingWnd, WM_CF_SHOW, cbRetDlgWin, 0);
				printRetDlgText = TEXT_CreateEx(0,50, LCD_WIDTH, LCD_HEIGHT - imgHeight, hRetDlgWnd, WM_CF_SHOW, TEXT_CF_LEFT,  alloc_win_id(), "Print this file?");
				buttonOk = BUTTON_CreateEx(70,  90, 80, 40,hRetDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
				buttonCancle = BUTTON_CreateEx(180,  90,80, 40,hRetDlgWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
				TEXT_SetBkColor(printRetDlgText, GUI_STATE_COLOR);
				if(gCfgItems.language == LANG_ENGLISH)
				{
					TEXT_SetText(printRetDlgText, "Return to main page?");
					BUTTON_SetText(buttonOk, "OK");
					BUTTON_SetText(buttonCancle, "CANCLE");
				}
				else
				{
					TEXT_SetText(printRetDlgText, "返回主页?");
					BUTTON_SetText(buttonOk, "确定");
					BUTTON_SetText(buttonCancle, "取消");
				}
				TEXT_SetTextColor(printRetDlgText, GUI_WHITE);
				TEXT_SetTextAlign(printRetDlgText, GUI_TA_VCENTER | GUI_TA_HCENTER);
				BUTTON_SetTextAlign(buttonOk, GUI_TA_VCENTER | GUI_TA_HCENTER);
				BUTTON_SetTextAlign(buttonCancle, GUI_TA_VCENTER | GUI_TA_HCENTER);
			}*/

			
								
		}
		
	}
	
}

void Clear_printing()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hPrintingWnd))
	{
		WM_DeleteWindow(hPrintingWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}
