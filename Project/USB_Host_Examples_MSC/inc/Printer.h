#ifndef _PRINTER_H_
#define _PRINTER_H_
#include "stdint.h"

#define MIN_SPRAYER_TEMP			0
#define MAX_SPRAYER_TEMP			270//300
#define MIN_BED_TEMP				0
#define MAX_BED_TEMP				150
#define MIN_EXT_SPEED_PERCENT		10
#define MAX_EXT_SPEED_PERCENT		999



typedef enum
{
	PRINTER_NOT_CONNECT = 0,
	PRINTER_CONNECT_OK,	
	PRINTER_OPERATING,
	PRINTER_OPERATE_OK,
} PRINTER_STATE;

typedef enum
{
	TASK_RESET,
	TASK_GOING,
	TASK_COMPLETE,
	TASK_PAUSE,
	TASK_STOP
	
} PRINT_TASK_FLAG;


typedef struct
{
	float curPos[3];
	float curSprayerTemp[2];	// 2个喷头温度
	float curSprayerTempBak[2];
	float curBedTemp;	//热床温度
	float curBedTempBak;
	float desireSprayerTemp[2];// 2个喷头目标温度
	float desireSprayerTempBak[2];
	float desireSprayerTempBak_1[2];
	float desireBedTemp;// 热床目标温度
	float desireBedTempBak;

	float preheat_desireSprayerTemp[2];// 2个喷头目标温度
	float preheat_desireBedTemp;// 热床目标温度

	uint16_t preheat_max_desireSprayerTemp;//喷头最大目标温度
	uint16_t preheat_max_desireBedTemp;//热床最大目标温度

	int8_t curSprayerChoose;	//当前选择的喷头
	int8_t curTempType;		//喷头1、喷头2、热床
	int8_t stepHeat;	//?よ????
	int8_t fanOnoff; //风机开关
	uint8_t fanSpeed;	//风机速度
	uint8_t pre_fanSpeed;//预设风机速度
	int8_t sprayerNum; //喷头数
	int8_t machineType; //机型
	uint16_t printSpeed;		//打印速度
	uint16_t printExtSpeed0;		//打印期间挤出速度
	uint16_t printExtSpeed1;		//打印期间挤出速度
	uint8_t stepPrintSpeed;	//打印速度调整步进
	int8_t extruStep;	//挤出步进
	int8_t extruSpeed;	//挤出速度
	int16_t moveSpeed;	//移动速度
	float move_dist;		//移动距离
	int8_t language;	//语言
	int8_t language_bak;	//语言
	int8_t baud;		//波特率

	int8_t fileSysType; //文件系统类型
	uint8_t touch_adj_flag; //触摸屏校正标志位
	int32_t touch_adj_xMin;
	int32_t touch_adj_xMax;
	int32_t touch_adj_yMin;
	int32_t touch_adj_yMax;

	int8_t rePrintFlag;

	int8_t custom_pic_flag;	//定制图片
	
	int32_t background_color; //背景颜色
	int32_t title_color; //标题颜色
	int32_t state_background_color; //状态显示背景颜色
	int32_t state_text_color; //状态显示文字颜色
	int32_t filename_color; //文件名文字颜色
	int32_t filename_background_color; //打印界面状态信息背景颜色
	int32_t printingstate_word_background_color;//打印界面状态信息文字颜色
	int32_t printingstate_word_color;
	uint8_t MoreItem_pic_cnt;//"更多"选项的图片数。

	uint8_t custom_bed_flag;//热床定制

	uint8_t button_3d_effect_flag;//按钮3d效果定制标志

	uint8_t firmware_type;//主板固件类型。1:marlin;2:repetier;3:smoothie

	int16_t Pause_XPOS;
	int16_t Pause_YPOS;
	int16_t Pause_ZADD;

	uint8_t func_btn1_display_flag;//功能按钮1显示标志:0不显示，1显示
	uint8_t func_btn2_display_flag;//功能按钮2显示标志:0不显示，1显示
	uint8_t func_btn3_display_flag;//功能按钮2显示标志:0不显示，1显示

	uint8_t pwrdn_mtrdn_level_flg;//断电电料逻辑电平标志，1:端口默认为高电平，0:端口默认为低电平

	uint8_t print_finish_close_machine_flg;//打完关机

	uint8_t morefunc_cnt;

	uint16_t filament_load_speed;//换料速度
	uint16_t filament_load_length;//换料步进
	//uint16_t filament_load_limit_temper;//换料的最低温度值
	uint16_t filament_loading_time;
	uint16_t filament_unload_speed;
	uint16_t  filament_unload_length;
	uint16_t filament_limit_temper;
	uint16_t filament_unloading_time;

	uint16_t unload_retract_length;  //退料时先进料的长度
	uint16_t unload_retract_speed;  //退料时先进料的速度
	
	uint16_t leveling_z_speed;
	uint16_t leveling_xy_speed;
	//HC-chen
	int16_t leveling_z_high;

	uint8_t leveling_point_number;//调平方式
	int16_t leveling_point1_x;
	int16_t leveling_point1_y;
	int16_t leveling_point2_x;
	int16_t leveling_point2_y;
	int16_t leveling_point3_x;
	int16_t leveling_point3_y;
	int16_t leveling_point4_x;
	int16_t leveling_point4_y;
	int16_t leveling_point5_x;
	int16_t leveling_point5_y;
	
	uint8_t getzpos_flg;
	uint8_t getzpos_enable;
	float zpos;//z轴坐标值
	unsigned char move_z_coordinate[20];

	uint8_t pwd_reprint_flg;
  float	z_current_position;
	float	find_z_current_position;
	uint8_t findpoint_start;
	float breakpoint_z_error;

	//tan 20170111
	unsigned char sd_save_flg;
	unsigned char sd_saving;
	unsigned char z_display_pos[21];
	unsigned char z_display_pos_bak[21];
	

	//tan 20170113
	unsigned char pwroff_save_mode;

	uint8_t leveling_mode;//调平方式，1:自动调平，0:手动调平
	
	uint8_t have_ups;
	uint8_t insert_det_module;// 1:接入220det模块，其他值:接PWC关机模块。PWC可以替代220det模块。

	uint8_t filament_det1_level_flg;// 断料检测1；1:高电平触发，0低电平触发。
	uint8_t filament_det2_level_flg;// 断料检测2；1:高电平触发，0低电平触发。

	//wifi
	//volatile uint8_t wifi_type;
	volatile char wifi_ap[32];	//wifi网络名称字符串
	volatile char wifi_key[64]; //wifi密码字符串
	volatile uint8_t wifi_mode_sel;
	uint8_t wifi_type; // HLK: 0x01 , ESP: 0X02

	volatile char cloud_enable;	
	volatile char cloud_hostUrl[96];	//云连接地址
	volatile int cloud_port;		//云连接端口

	volatile uint8_t multiple_language;//多国语言
	volatile uint8_t OVERTURN_180;   //屏幕翻转180°
	
	volatile int code_page;

	volatile uint8_t disable_pwr_det;
	volatile uint8_t disable_mt_det;
	volatile uint8_t single_nozzle;
	volatile uint8_t wifi_state;

	volatile uint8_t add_checkSum;   //M110是否加校验标志位
	volatile uint8_t display_error;   //显示报错信息标志位 

	volatile uint8_t TestFalaFlag;//测试法拉电容标志位
	volatile uint8_t reprint_no_temp_flag;   //断电续打没有温度手动添加温度标志位
	volatile uint8_t babystep;
	volatile uint8_t zoffset_disp_flag;
	volatile uint8_t disp_zoffset_buf[30];
} CFG_ITMES;

extern int32_t printer_state;

extern CFG_ITMES gCfgItems;

extern void printer_init();
extern void get_cur_temp();
//extern void code_page_sel();

#endif
