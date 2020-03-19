#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_language.h"
#include "draw_ui.h"
#include "pic.h"
#include "fontLib.h"
#include "LISTBOX.h"
#include "usbh_usr.h"
#include "printer.h"
#include "printing_thread.h"

#ifndef GUI_FLASH
#define GUI_FLASH
#endif

static GUI_HWIN hLanguageWnd;


//extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz18;

static BUTTON_Handle buttonUp,buttonNext,buttonRet;
static BUTTON_Handle buttonSimple_Ch,buttontraditional_Ch,buttonEng,buttonJapan,buttonGerman,buttonFrench,buttonRussian;
static BUTTON_Handle buttonTurkish,buttonSpanish,buttonPortuguese,buttonKorean,buttonItaly;

static uint8_t page_nub = 1;
//void disp_language_button_color(BUTTON_Handle button,int bkgcolor,int textcolor);



static void cbLanguageWin(WM_MESSAGE * pMsg) {

	struct PressEvt *press_event;
	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			//GUI_SetBkColor(GUI_BLUE);
			//GUI_Clear();
			//GUI_DispString("window");
			break;

		case WM_NOTIFY_PARENT:
			if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				if(pMsg->hWinSrc == buttonRet)
				{
					last_disp_state = LANGUAGE_UI;
					Clear_Language();
					draw_return_ui();
				}
				else if(pMsg->hWinSrc == buttonSimple_Ch)
				{
					
					gCfgItems.language = LANG_SIMPLE_CHINESE;
					bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);
					disp_language_init();
					disp_sel_item();
				}
				else if(pMsg->hWinSrc == buttontraditional_Ch)
				{

					gCfgItems.language = LANG_COMPLEX_CHINESE;
					bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);
					disp_language_init();
					disp_sel_item();
				}
				else if(pMsg->hWinSrc == buttonEng)
				{

					gCfgItems.language = LANG_ENGLISH;
					bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);
					disp_language_init();
					disp_sel_item();
				}
				#if 0
				else if(pMsg->hWinSrc == buttonJapan)
				{

					gCfgItems.language = LANG_JAPAN;
					bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);
					disp_language_init();
					disp_sel_item();
				}				
				else if(pMsg->hWinSrc == buttonGerman)
				{

					gCfgItems.language = LANG_GERMAN;
					bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);
					disp_language_init();
					disp_sel_item();
				}
				else if(pMsg->hWinSrc == buttonPortuguese)
				{

					gCfgItems.language = LANG_GREEK;
					bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);
					disp_language_init();
					disp_sel_item();
				}	
				else if(pMsg->hWinSrc == buttonTurkish)
				{

					gCfgItems.language = LANG_TURKISH;
					bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);
					disp_language_init();
					disp_sel_item();
				}		
				else if(pMsg->hWinSrc == buttonKorean)
				{

					gCfgItems.language = LANG_KOREAN;
					bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);
					disp_language_init();
					disp_sel_item();
				}					
				#endif
	
				else if(pMsg->hWinSrc == buttonRussian)
				{

					gCfgItems.language = LANG_RUSSIAN;
					bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);
					disp_language_init();
					disp_sel_item();
				}	
				else if(pMsg->hWinSrc == buttonSpanish)
				{
		
					gCfgItems.language = LANG_SPANISH;
					bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);
					disp_language_init();
					disp_sel_item();
				}	
				#if defined (STM32F10X_HD)

				else if(pMsg->hWinSrc == buttonFrench)
				{

					gCfgItems.language = LANG_FRENCH;
					bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);
					disp_language_init();
					disp_sel_item();
				}	

				else if(pMsg->hWinSrc == buttonItaly)
				{

					gCfgItems.language = LANG_ITALY;
					bakup_cfg_inf(BAK_LANG_ID,  gCfgItems.language);
					disp_language_init();
					disp_sel_item();
				}	
							
				else if(pMsg->hWinSrc == buttonNext)
				{
					buttonSimple_Ch = 0;
					buttontraditional_Ch = 0;
					buttonEng = 0;
					buttonRussian= 0;
					buttonSpanish= 0;
					buttonJapan = 0;
					
					buttonNext = 0;
					last_disp_state = LANGUAGE_UI;
					Clear_Language();
					page_nub=2;				
					draw_Language();
				}
				else if(pMsg->hWinSrc == buttonUp)
				{
					buttonFrench= 0;
					buttonGerman= 0;
					buttonPortuguese= 0;
					buttonTurkish= 0;
					buttonKorean= 0;
					buttonItaly= 0;
					buttonUp = 0;

					last_disp_state = LANGUAGE_UI;
					Clear_Language();
					page_nub=1;				
					draw_Language();
				}
				#endif	
			}
			break;
			
		default:
			WM_DefaultProc(pMsg);
		}
	}


void draw_Language()
{
		
//	int titleHeight = 30;

//	int imgHeight = LCD_HEIGHT - titleHeight;	
	
	int i;

	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != LANGUAGE_UI)
	{
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = LANGUAGE_UI;
	}
	disp_state = LANGUAGE_UI;
		
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

	//GUI_DispStringAt("准备打印->设置->语言", 0, 0);
	GUI_DispStringAt(creat_title_text(), 0, 0);
	
	hLanguageWnd = WM_CreateWindow(0, titleHeight, LCD_WIDTH, imgHeight, WM_CF_SHOW, cbLanguageWin, 0);


	//if(page_nub == 1)
	{
		buttonSimple_Ch = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		buttontraditional_Ch = BUTTON_CreateEx(LCD_WIDTH  / 4,  0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		buttonEng = BUTTON_CreateEx(LCD_WIDTH  / 2,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		buttonRussian = BUTTON_CreateEx(LCD_WIDTH*3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		buttonSpanish = BUTTON_CreateEx(0, imgHeight/2, LCD_WIDTH/4 - 2,imgHeight/2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

		#if defined (STM32F10X_HD)
		buttonFrench = BUTTON_CreateEx(LCD_WIDTH/4, imgHeight/2, LCD_WIDTH/4-2, imgHeight/2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());	
		buttonItaly = BUTTON_CreateEx(LCD_WIDTH/2,imgHeight/2, LCD_WIDTH/4-2, imgHeight/2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

		
		//buttonJapan = BUTTON_CreateEx(LCD_WIDTH/4, imgHeight/2, LCD_WIDTH/4-2, imgHeight/2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());	
		//buttonNext = BUTTON_CreateEx(LCD_WIDTH/2,imgHeight/2, LCD_WIDTH/4-2, imgHeight/2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		#endif		
		BUTTON_SetBitmapEx(buttonSimple_Ch, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttontraditional_Ch, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonEng, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonRussian, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonSpanish, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		#if defined (STM32F10X_HD)
		BUTTON_SetBitmapEx(buttonFrench, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonItaly, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		
		//BUTTON_SetBitmapEx(buttonJapan, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		//BUTTON_SetBitmapEx(buttonNext, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		#endif
	}
	#if 0
	else
	{
		buttonGerman = BUTTON_CreateEx(0, 0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		buttonItaly= BUTTON_CreateEx(LCD_WIDTH  / 4,  0, LCD_WIDTH / 4 - 1, imgHeight / 2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		buttonFrench = BUTTON_CreateEx(LCD_WIDTH  / 2,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		buttonTurkish = BUTTON_CreateEx(LCD_WIDTH*3 / 4,  0, LCD_WIDTH / 4 - 2, imgHeight / 2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		buttonPortuguese = BUTTON_CreateEx(0, imgHeight/2, LCD_WIDTH/4 - 2,imgHeight/2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		//buttonKorean = BUTTON_CreateEx(LCD_WIDTH/4, imgHeight/2, LCD_WIDTH/4-2, imgHeight/2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		buttonUp = BUTTON_CreateEx(LCD_WIDTH/2,imgHeight/2, LCD_WIDTH/4-2, imgHeight/2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	
		BUTTON_SetBitmapEx(buttonRussian, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonSpanish, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonPortuguese, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonTurkish, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		//BUTTON_SetBitmapEx(buttonKorean, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonItaly, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
		BUTTON_SetBitmapEx(buttonUp, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	
	}
	#endif
	buttonRet = BUTTON_CreateEx(LCD_WIDTH * 3/4 ,imgHeight/2, LCD_WIDTH/4 - 2, imgHeight/2 - 1, hLanguageWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	//TEXT_SetDefaultFont(&FONT_STATE_INF);	
	disp_sel_item();	
	//GUI_Exec();
	
	BUTTON_SetBmpFileName(buttonRet, "bmp_return.bin",0);
	BUTTON_SetBitmapEx(buttonRet, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
#if defined (STM32F10X_HD)
	//BUTTON_SetBmpFileName(buttonNext, "bmp_pageDown.bin",0);
	//BUTTON_SetBitmapEx(buttonNext, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
	//BUTTON_SetBmpFileName(buttonUp, "bmp_pageUp.bin",0);
	//BUTTON_SetBitmapEx(buttonUp, 0, &bmp_struct, BMP_PIC_X, BMP_PIC_Y);
#endif
if(gCfgItems.multiple_language != 0)
{
	BUTTON_SetText(buttonRet,common_menu.text_back);
}

	
}

void disp_sel_item()
{
	if(page_nub == 1)
	{
		switch(gCfgItems.language)
		{
		case LANG_SIMPLE_CHINESE:
		BUTTON_SetBmpFileName(buttonSimple_Ch, "bmp_simplified_cn_sel.bin",0);
		BUTTON_SetBmpFileName(buttontraditional_Ch, "bmp_traditional_cn.bin",0);
		BUTTON_SetBmpFileName(buttonEng, "bmp_english.bin",0);
		BUTTON_SetBmpFileName(buttonRussian, "bmp_russian.bin",0);
		BUTTON_SetBmpFileName(buttonSpanish, "bmp_spanish.bin",0);
		#if defined (STM32F10X_HD)
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);	
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);
		#endif
		break;
		case LANG_COMPLEX_CHINESE:
		BUTTON_SetBmpFileName(buttonSimple_Ch, "bmp_simplified_cn.bin",0);
		BUTTON_SetBmpFileName(buttontraditional_Ch, "bmp_traditional_cn_sel.bin",0);
		BUTTON_SetBmpFileName(buttonEng, "bmp_english.bin",0);
		BUTTON_SetBmpFileName(buttonRussian, "bmp_russian.bin",0);
		BUTTON_SetBmpFileName(buttonSpanish, "bmp_spanish.bin",0);
		#if defined (STM32F10X_HD)
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);
		#endif
		break;
		case LANG_ENGLISH:
		BUTTON_SetBmpFileName(buttonSimple_Ch, "bmp_simplified_cn.bin",0);
		BUTTON_SetBmpFileName(buttontraditional_Ch, "bmp_traditional_cn.bin",0);
		BUTTON_SetBmpFileName(buttonEng, "bmp_english_sel.bin",0);
		BUTTON_SetBmpFileName(buttonRussian, "bmp_russian.bin",0);
		BUTTON_SetBmpFileName(buttonSpanish, "bmp_spanish.bin",0);
		#if defined (STM32F10X_HD)
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);
		#endif
		break;

		case LANG_RUSSIAN:
		BUTTON_SetBmpFileName(buttonSimple_Ch, "bmp_simplified_cn.bin",0);
		BUTTON_SetBmpFileName(buttontraditional_Ch, "bmp_traditional_cn.bin",0);
		BUTTON_SetBmpFileName(buttonEng, "bmp_english.bin",0);
		BUTTON_SetBmpFileName(buttonRussian, "bmp_russian_sel.bin",0);
		BUTTON_SetBmpFileName(buttonSpanish, "bmp_spanish.bin",0);	
		#if defined (STM32F10X_HD)
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);	
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);
		#endif
		break;
		case LANG_SPANISH:
		BUTTON_SetBmpFileName(buttonSimple_Ch, "bmp_simplified_cn.bin",0);
		BUTTON_SetBmpFileName(buttontraditional_Ch, "bmp_traditional_cn.bin",0);
		BUTTON_SetBmpFileName(buttonEng, "bmp_english.bin",0);
		BUTTON_SetBmpFileName(buttonRussian, "bmp_russian.bin",0);
		BUTTON_SetBmpFileName(buttonSpanish, "bmp_spanish_sel.bin",0);	
		#if defined (STM32F10X_HD)
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);	
		#endif
		break;
		
		case LANG_ITALY:
		BUTTON_SetBmpFileName(buttonSimple_Ch, "bmp_simplified_cn.bin",0);
		BUTTON_SetBmpFileName(buttontraditional_Ch, "bmp_traditional_cn.bin",0);
		BUTTON_SetBmpFileName(buttonEng, "bmp_english.bin",0);
		BUTTON_SetBmpFileName(buttonRussian, "bmp_russian.bin",0);
		BUTTON_SetBmpFileName(buttonSpanish, "bmp_spanish.bin",0);	
		#if defined (STM32F10X_HD)
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy_sel.bin",0);
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);	
		#endif
		break;	
		case LANG_FRENCH:
		BUTTON_SetBmpFileName(buttonSimple_Ch, "bmp_simplified_cn.bin",0);
		BUTTON_SetBmpFileName(buttontraditional_Ch, "bmp_traditional_cn.bin",0);
		BUTTON_SetBmpFileName(buttonEng, "bmp_english.bin",0);
		BUTTON_SetBmpFileName(buttonRussian, "bmp_russian.bin",0);
		BUTTON_SetBmpFileName(buttonSpanish, "bmp_spanish.bin",0);	
		#if defined (STM32F10X_HD)
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french_sel.bin",0);	
		#endif
		break;			
		default:
		BUTTON_SetBmpFileName(buttonSimple_Ch, "bmp_simplified_cn.bin",0);
		BUTTON_SetBmpFileName(buttontraditional_Ch, "bmp_traditional_cn.bin",0);
		BUTTON_SetBmpFileName(buttonEng, "bmp_english.bin",0);
		BUTTON_SetBmpFileName(buttonRussian, "bmp_russian.bin",0);
		BUTTON_SetBmpFileName(buttonSpanish, "bmp_spanish.bin",0);
		#if defined (STM32F10X_HD)
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);	
		#endif
		break;
		}

		if(gCfgItems.multiple_language != 0)
		{
			BUTTON_SetText(buttonSimple_Ch,language_menu.chinese_s);
			BUTTON_SetText(buttontraditional_Ch,language_menu.chinese_t);
			BUTTON_SetText(buttonEng,language_menu.english);
			BUTTON_SetText(buttonItaly,language_menu.italy);
			BUTTON_SetText(buttonRussian,language_menu.russian);	
			BUTTON_SetText(buttonFrench,language_menu.french);	
			BUTTON_SetText(buttonSpanish,language_menu.spanish);
		}
	}
	#if 0
	
	else
	{
		switch(gCfgItems.language)
		{
		//第二页语言显示
		case LANG_GERMAN:
		BUTTON_SetBmpFileName(buttonGerman, "bmp_german_sel.bin",0);
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);
		BUTTON_SetBmpFileName(buttonPortuguese, "bmp_protuguese.bin",0);
		BUTTON_SetBmpFileName(buttonTurkish, "bmp_turkish.bin",0);
		//BUTTON_SetBmpFileName(buttonKorean, "bmp_korean.bin",0);
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);			
		break;
		case LANG_FRENCH:
		BUTTON_SetBmpFileName(buttonGerman, "bmp_german.bin",0);
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french_sel.bin",0);
		BUTTON_SetBmpFileName(buttonPortuguese, "bmp_protuguese.bin",0);
		BUTTON_SetBmpFileName(buttonTurkish, "bmp_turkish.bin",0);
		//BUTTON_SetBmpFileName(buttonKorean, "bmp_korean.bin",0);
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);			
		break;
		case LANG_PORTUGUESE:
		BUTTON_SetBmpFileName(buttonGerman, "bmp_german.bin",0);
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);
		BUTTON_SetBmpFileName(buttonPortuguese, "bmp_protuguese_sel.bin",0);
		BUTTON_SetBmpFileName(buttonTurkish, "bmp_turkish.bin",0);
		//BUTTON_SetBmpFileName(buttonKorean, "bmp_korean.bin",0);
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);			
		break;
		case LANG_TURKISH:
		BUTTON_SetBmpFileName(buttonGerman, "bmp_german.bin",0);
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);
		BUTTON_SetBmpFileName(buttonPortuguese, "bmp_protuguese.bin",0);
		BUTTON_SetBmpFileName(buttonTurkish, "bmp_turkish_sel.bin",0);
		//BUTTON_SetBmpFileName(buttonKorean, "bmp_korean.bin",0);
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);
		break;

		case LANG_KOREAN:
		BUTTON_SetBmpFileName(buttonGerman, "bmp_german.bin",0);
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);
		BUTTON_SetBmpFileName(buttonPortuguese, "bmp_protuguese.bin",0);
		BUTTON_SetBmpFileName(buttonTurkish, "bmp_turkish.bin",0);
		//BUTTON_SetBmpFileName(buttonKorean, "bmp_korean_sel.bin",0);
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);
		break;

		case LANG_ITALY:
		BUTTON_SetBmpFileName(buttonGerman, "bmp_german.bin",0);
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);
		BUTTON_SetBmpFileName(buttonPortuguese, "bmp_protuguese.bin",0);
		BUTTON_SetBmpFileName(buttonTurkish, "bmp_turkish.bin",0);
		//BUTTON_SetBmpFileName(buttonKorean, "bmp_korean.bin",0);
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy_sel.bin",0);
		break;
		default:
		BUTTON_SetBmpFileName(buttonGerman, "bmp_german.bin",0);
		BUTTON_SetBmpFileName(buttonFrench, "bmp_french.bin",0);
		BUTTON_SetBmpFileName(buttonPortuguese, "bmp_protuguese.bin",0);
		BUTTON_SetBmpFileName(buttonTurkish, "bmp_turkish.bin",0);
		//BUTTON_SetBmpFileName(buttonKorean, "bmp_korean.bin",0);
		BUTTON_SetBmpFileName(buttonItaly, "bmp_italy.bin",0);
		break;
		}
		if(gCfgItems.multiple_language != 0)
		{
			//BUTTON_SetText(buttonGerman.btnHandle,language_menu.german);
			//BUTTON_SetText(buttonFrench,language_menu.french);
			//BUTTON_SetText(buttonKorean.btnHandle,language_menu.korean);
			//BUTTON_SetText(buttonPortuguese.btnHandle,language_menu.portuguese);
			//BUTTON_SetText(buttonTurkish.btnHandle,language_menu.brazil);	
			//BUTTON_SetText(buttonItaly,language_menu.italy);	
		}		
	}
	#endif
}


void Clear_Language()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hLanguageWnd))
	{
		WM_DeleteWindow(hLanguageWnd);
		GUI_Exec();
	}
	
	//GUI_Clear();
}


