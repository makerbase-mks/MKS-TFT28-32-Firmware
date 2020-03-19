#ifndef _DRAW_UI_
#define _DRAW_UI_
#include "stdint.h"

#include "gui.h"
#include "id_manage.h"
#include "mks_tft_com.h"
#include "bak_cfg_inf.h"
#include "pic.h"
#include "draw_dialog.h"
#include "id_manage.h"
#include "Multi_language.h"



#define SIMPLE_DISPLAY	 0// 2:Ô×´Ì¬Ğ”Ê¾Ñ¦Ã¬1:İ²Ó—Ğ”Ê¾Ñ¦Ã¬0:Ş­Ö¤Ğ”Ê¾Ñ¦

#define TICK_CYCLE		1 //ms

#define LCD_WIDTH	320
#define LCD_HEIGHT	240

#define titleHeight	30

#define imgHeight		(LCD_HEIGHT - titleHeight)	

#define GUI_PURPLE			0x300018

//#define GUI_STATE_COLOR         0x646400 
//#define GUI_STATE_COLOR         0x806480 
#define GUI_RET_BUTTON_COLOR	GUI_DARKGREEN//0x00a800
#define GUI_STATE_COLOR         GUI_DARKBLUE
#define GUI_BK_CLOLOR			GUI_BLACK
#define GUI_BUTTON_COLOR		GUI_BLUE
#define GUI_FOCUS_CLOLOR		GUI_RET_BUTTON_COLOR


#define GUI_DARKGREEN2         GUI_DARKGREEN

#define	MAX_FILES_ONE_LEVER		5//30

extern char BMP_PIC_X ;
extern char BMP_PIC_Y;  // 0// 17

#define BMP_PIC_X_H		0
#define BMP_PIC_Y_H    0
#if 0

//Ô¯Ò”
#define LANG_SIMPLE_CHINESE		1
#define LANG_COMPLEX_CHINESE	2
#define LANG_ENGLISH						3
#define LANG_JAPAN							4
#define LANG_GERMAN						5		
#define LANG_FRENCH						6
#define LANG_RUSSIAN						7
#define LANG_KOREAN						8
#define LANG_TURKISH						9
#define LANG_SPANISH						10
#define LANG_GREEK							11
#define LANG_ITALY							12
#define LANG_PORTUGUESE				13

#define FONT_BUTTON	GUI_FontHZ_fontHz14
#define FONT_STATE_INF	GUI_FontHZ_fontHz14
#define FONT_TITLE		GUI_FontHZ_fontHz14

//********************************************//
//æ³¨æ„ï¼šç”±äºæ˜¾ç¤ºå­—ç¬¦çš„ç¼–ç é—®é¢˜ï¼Œæ‰€ä»¥ä»¥ä¸‹ä¸­æ–‡å­—ä½“å¿…é¡»é‡‡ç”¨ANSIæ‰“å¼€å’Œä¿å­˜ã€‚
//ÓÉÓÚÏÖÊµ×Ö·û±àÂëÎÊÌâ£¬ËùÒÔÒÔÏÂÖĞÎÄ×Ö±ØĞëÒÔANSI±ê×¼¸ñÊ½ÏÔÊ¾
//"ÕıÔÚ´òÓ¡""²Ù×÷""ÔİÍ£"Èı¸ö½çÃæµÄ±êÌâÀ¸Ö»×ö"ÖĞÓ¢ÇĞ»»"¡£
//"æ­£åœ¨æ‰“å°""æ“ä½œ""æš‚åœ"ä¸‰ä¸ªç•Œé¢çš„æ ‡é¢˜æ åªåšâ€œä¸­è‹±æ–‡åˆ‡é¢â€
#define PRINTING_GBK													"ÕıÔÚ´òÓ¡"
#define PRINTING_OPERATION_GBK							"²Ù×÷"
#define PRINTING_PAUSE_GBK										"ÔİÍ£"
#define PRINTING_OTHER_LANGUGE							"Printing"
#define PRINTING_OPERATION_OTHER_LANGUGE		"Operation"
#define PRINTING_PAUSE_OTHER_LANGUGE				"Pause"
//*********************************************//
#endif


#define FILE_SYS_USB	0
#define FILE_SYS_SD		1

struct PressEvt
{
 int x;
 int y;
 unsigned char pressed;
};

typedef struct
{
	
	//int16_t  days;
	uint16_t    hours;
	uint8_t    minutes;
	volatile int8_t    seconds;
	int8_t    ms_10;
	int8_t	start;
	
} PRINT_TIME;

typedef enum
{
	MAIN_UI,
	PRINT_READY_UI,
	PRINT_FILE_UI,
	PRINTING_UI,
	MOVE_MOTOR_UI,
	OPERATE_UI,
	PAUSE_UI,
	EXTRUSION_UI,
	FAN_UI,
	PRE_HEAT_UI,
	CHANGE_SPEED_UI,
	TEMP_UI,
	SET_UI,
	ZERO_UI,
	SPRAYER_UI,
	MACHINE_UI,
	LANGUAGE_UI,
	ABOUT_UI,
	LOG_UI,
	DISK_UI,
	CALIBRATE_UI,
	DIALOG_UI,
	WIFI_UI,
	MORE_UI,
	FILETRANSFER_UI,
	FILETRANSFERSTATE_UI,
	PRINT_MORE_UI,
	FILAMENTCHANGE_UI,
	LEVELING_UI,
	TOOL_UI,
	BIND_UI,
	BABY_STEP_UI
} DISP_STATE;

typedef struct
{
	DISP_STATE _disp_state[50];
	char       _disp_index;
	
} DISP_STATE_STACK;

typedef enum
{
	SD_NOT_INIT = 1,
	SD_INIT_OK,
	SD_FILE_RET_BEGIN,
	SD_FILE_RET_END,
	SD_SEL_FILE
	
} SD_STATE;

typedef struct
{
	uint8_t fileName[MAX_FILES_ONE_LEVER][200];//fileName[MAX_FILES_ONE_LEVER][50];
	uint8_t fileAttr[MAX_FILES_ONE_LEVER];	//0?agcode???tÂª?1?a????
	uint8_t index;
	uint8_t listVaild;	//flag of valid
	
} fileNameList;

extern fileNameList gcodeFileList;

extern GUI_FLASH const GUI_FONT GUI_FontHZ_fontHz14;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontJapan;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontRussia;

extern DISP_STATE disp_state;
extern DISP_STATE last_disp_state;
extern DISP_STATE_STACK disp_state_stack;


extern uint8_t curFileName[100];

extern void draw_main_ui(void);
extern void disp_sel_lang(void);
extern void clear_main_ui(void);
extern void clear_cur_ui(void);
extern char *creat_title_text(void);
extern void gui_view_init(void);
#endif
