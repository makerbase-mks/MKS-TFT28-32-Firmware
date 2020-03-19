#include "SZ_STM32F107VC_LIB.h"
#include "gcode.h"
#include "draw_ui.h"
#include "bak_cfg_inf.h"
#include "Printer.h"
#include "sd_usr.h"
#include "wifi_module.h"

#define PRINT_COM_ID	COM2

#define MAX_CMD_SIZE	1

extern unsigned char *codebufpoint;

volatile unsigned int USR_CTRL = 0;

int8_t curSprayerTemp[10];
int8_t curBedTemp[10];

int32_t printer_state = PRINTER_NOT_CONNECT;

uint8_t gcode_cache = 0;

CFG_ITMES gCfgItems;

extern GUI_BITMAP bmp_struct;

extern DISP_STATE disp_state;

extern uint16_t readLcdId;
#if 0
void code_page_sel()
{
	switch(gCfgItems.language)
	{
		case LANG_SIMPLE_CHINESE:
		case LANG_COMPLEX_CHINESE:
		case LANG_ENGLISH:
		case LANG_JAPAN:
			gCfgItems.code_page = 936;
			break;
		case LANG_GERMAN:
			break;
		case LANG_FRENCH:
			break;
		case LANG_RUSSIAN:
			gCfgItems.code_page = 866;
			break;
		case LANG_SPANISH:
			break;
		case LANG_TURKISH:
			gCfgItems.code_page = 936;
			break;
		case LANG_GREEK:
			gCfgItems.code_page = 1253;
			break;
		#if DEF_KOREAN_USE
		case LANG_KOREAN:
			gCfgItems.code_page = 949;
			break;
		#endif
		case LANG_ITALY:
			break;
	}

}

union	LCD_identify
{
	uint32_t id;
	uint8_t   id_buf[4];
}LCD_ID;

//////extern PRINT_TASK_FLAG print_task_flag;
#endif

void reset_tx_fifo()
{
	/*UartTxFifo.uart_read_point = UartTxFifo.uart_write_point = 0;
	memset(UartTxFifo.uartTxBuffer, 0, sizeof(UartTxFifo.uartTxBuffer));*/
}

extern uint16_t readLcdId;
void printer_init()
{
	uint32_t flash_inf_valid_flag = 0;

/*	memset(gCfgItems.curBedTemp, 0, sizeof(gCfgItems.curBedTemp));
	memset(gCfgItems.curSprayerTemp[0], 0, sizeof(gCfgItems.curSprayerTemp[0]));
	memset(gCfgItems.curSprayerTemp[0], 0, sizeof(gCfgItems.curSprayerTemp[1]));
	memset(gCfgItems.curPos[0], 0, sizeof(gCfgItems.curPos[0]));
	memset(gCfgItems.curPos[1], 0, sizeof(gCfgItems.curPos[1]));
	memset(gCfgItems.curPos[2], 0, sizeof(gCfgItems.curPos[2]));*/

	//HC-chen
	//gCfgItems.preheat_desireBedTemp = 50;
	//gCfgItems.preheat_desireSprayerTemp[0] = gCfgItems.preheat_desireSprayerTemp[1] = 180;
	gCfgItems.preheat_desireBedTemp = 0;
	gCfgItems.preheat_desireSprayerTemp[0] = gCfgItems.preheat_desireSprayerTemp[1] = 0; 
	
	gCfgItems.desireBedTemp = 0;
	gCfgItems.desireSprayerTemp[0] = gCfgItems.desireSprayerTemp[1] = 0;
	gCfgItems.curSprayerChoose = 0;
	gCfgItems.extruSpeed = 1;
	gCfgItems.extruStep = 5;
	
	gCfgItems.stepHeat = 10;
	gCfgItems.moveSpeed = 3000;
	gCfgItems.machineType = 1;
	gCfgItems.fanOnoff = 0;
	gCfgItems.fanSpeed = 0;
	gCfgItems.pre_fanSpeed = 127;
	//gCfgItems.language = 1;//简体中文
	gCfgItems.language = LANG_ENGLISH;//英文
	gCfgItems.sprayerNum = 1;
	gCfgItems.printSpeed = 100;
	gCfgItems.printExtSpeed0= 100;
	gCfgItems.printExtSpeed1 = 100;
	gCfgItems.stepPrintSpeed = 1;
	gCfgItems.move_dist = 1;
	gCfgItems.baud = 4;
/*
	gCfgItems.touch_adj_flag = 0;
	gCfgItems.touch_adj_xMin = 336;
	gCfgItems.touch_adj_xMax = 4059;
	gCfgItems.touch_adj_yMin = 3814;
	gCfgItems.touch_adj_yMax = 106;
*/
/*#if OVERTURN_180
		gCfgItems.touch_adj_flag = 0;
		gCfgItems.touch_adj_xMin = 389;
		gCfgItems.touch_adj_xMax = 3518;
		gCfgItems.touch_adj_yMin = 3770;
		gCfgItems.touch_adj_yMax = 284;
	
#else
	
#endif
*/
	gCfgItems.fileSysType = 1;

	gCfgItems.rePrintFlag = printer_normal;

	gCfgItems.custom_bed_flag = 1;
	//HC-chen 
	gCfgItems.leveling_z_high=10;
	gCfgItems.wifi_state=1;
	gCfgItems.cloud_enable=1;
	gCfgItems.add_checkSum=0;
	gCfgItems.display_error=0;
	gCfgItems.reprint_no_temp_flag=0; 
	gCfgItems.unload_retract_length = 0;
	gCfgItems.unload_retract_speed = 1000;
	
	
	#if 0
	if(readLcdId == 0x9341)
	{
		gCfgItems.touch_adj_flag = 0;
		gCfgItems.touch_adj_xMin = 400;
		gCfgItems.touch_adj_xMax = 3836;
		gCfgItems.touch_adj_yMin = 3805;
		gCfgItems.touch_adj_yMax = 212;


		/*
		int tmpCoord;
		
		if(gCfgItems.touch_adj_xMin <  gCfgItems.touch_adj_xMax)
		{
			tmpCoord = gCfgItems.touch_adj_xMin;
			gCfgItems.touch_adj_xMin = gCfgItems.touch_adj_xMax;
			gCfgItems.touch_adj_xMax = tmpCoord;
		}
		if(gCfgItems.touch_adj_yMin >  gCfgItems.touch_adj_yMax)
		{
			tmpCoord = gCfgItems.touch_adj_yMin;
			gCfgItems.touch_adj_yMin = gCfgItems.touch_adj_yMax;
			gCfgItems.touch_adj_yMax = tmpCoord;
		}
		*/
		
	}
	#endif 
	#if 0
	if(readLcdId == 0x8989)
	{
		gCfgItems.touch_adj_flag = 0;
		gCfgItems.touch_adj_xMin = 389;
		gCfgItems.touch_adj_xMax = 3518;
		gCfgItems.touch_adj_yMin = 284;
		gCfgItems.touch_adj_yMax = 3770;	
	}
	#endif
	
	gCfgItems.background_color = 0x000000;//蓝色
	gCfgItems.title_color = 0xFFFFFF;//白色
	gCfgItems.state_background_color = 0x000000;//深蓝色
	gCfgItems.state_text_color = 0xFFFFFF;//白色
	gCfgItems.filename_background_color = 0x000000;//蓝色
	gCfgItems.filename_color = 0xFFFFFF;//白色
	gCfgItems.printingstate_word_background_color = 0x000000;//蓝色
	gCfgItems.printingstate_word_color = 0xFFFFFF;//白色

	gCfgItems.button_3d_effect_flag = 0;
	
	gCfgItems.MoreItem_pic_cnt = 0;

	/*wifi_cfg*/
	gCfgItems.wifi_mode_sel = 1;//1为AP模式，其他为client模式，默认为AP模式。
	
	memset(wifiPara, 0, sizeof(wifiPara));
	memset(ipPara, 0, sizeof(ipPara));
	
	strcpy(wifiPara.ap_name,WIFI_AP_NAME);
	strcpy(wifiPara.keyCode,WIFI_KEY_CODE);
	//client模式初始值
	strcpy(ipPara.ip_addr,IP_ADDR);
	strcpy(ipPara.mask,IP_MASK);
	strcpy(ipPara.gate,IP_GATE);
	strcpy(ipPara.dns,IP_DNS);
	
	ipPara.dhcp_flag = IP_DHCP_FLAG;

	//AP模式初始值
	strcpy(ipPara.dhcpd_ip,AP_IP_ADDR);
	strcpy(ipPara.dhcpd_mask,AP_IP_MASK);
	strcpy(ipPara.dhcpd_gate,AP_IP_GATE);
	strcpy(ipPara.dhcpd_dns,AP_IP_DNS);
	strcpy(ipPara.start_ip_addr,IP_START_IP);
	strcpy(ipPara.end_ip_addr,IP_END_IP);
	
	ipPara.dhcpd_flag = AP_IP_DHCP_FLAG;

	gCfgItems.firmware_type = 1;

	gCfgItems.Pause_XPOS = -1;
	gCfgItems.Pause_YPOS = -1;
	gCfgItems.Pause_ZADD = 10;

	gCfgItems.func_btn1_display_flag = 1;
	gCfgItems.func_btn2_display_flag = 1;
	gCfgItems.func_btn3_display_flag = 0;
	
	gCfgItems.preheat_max_desireSprayerTemp = MAX_SPRAYER_TEMP;
	gCfgItems.preheat_max_desireBedTemp = MAX_BED_TEMP;	

	gCfgItems.pwrdn_mtrdn_level_flg = 1;
	
	gCfgItems.print_finish_close_machine_flg = 0;

	gCfgItems.morefunc_cnt = 0;

	gCfgItems.filament_load_speed = 1200;
	gCfgItems.filament_unload_speed = 1200;
	gCfgItems.filament_load_length= 200;
	gCfgItems.filament_unload_length= 200;
	gCfgItems.filament_limit_temper = 200;
	//gCfgItems.filament_unload_limit_temper = 200;
	
	gCfgItems.leveling_point_number = 4;
	//(50,50)
	gCfgItems.leveling_point1_x = 50;
	gCfgItems.leveling_point1_y = 50;
	//(180,50)
	gCfgItems.leveling_point2_x = 180;
	gCfgItems.leveling_point2_y = 50;
	//(180,180)
	gCfgItems.leveling_point3_x = 180;
	gCfgItems.leveling_point3_y = 180;
	//(50,180)
	gCfgItems.leveling_point4_x = 50;
	gCfgItems.leveling_point4_y = 180;
	//(150,150)
	gCfgItems.leveling_point5_x = 150;
	gCfgItems.leveling_point5_y = 150;

	gCfgItems.leveling_xy_speed = 3000;
	gCfgItems.leveling_z_speed = 1500;

	gCfgItems.zpos = 0;
	memset(gCfgItems.move_z_coordinate,0,sizeof(gCfgItems.move_z_coordinate));	
	gCfgItems.getzpos_enable= 0;
#ifdef SAVE_FROM_SD
	gCfgItems.pwroff_save_mode = 0;
#endif	
	gCfgItems.sd_saving = 0;
	memset(gCfgItems.z_display_pos,0,sizeof(gCfgItems.z_display_pos));	

	gCfgItems.breakpoint_z_error = 0.2;

	gCfgItems.wifi_type = 0x02;
	strcpy(gCfgItems.cloud_hostUrl, "baizhongyun.cn");
	gCfgItems.cloud_port = 10086;
	
	gCfgItems.leveling_mode = 0;
	
	gCfgItems.filament_det1_level_flg = 0;
	gCfgItems.filament_det2_level_flg = 0;
	gCfgItems.have_ups = 0;
	gCfgItems.insert_det_module = 0;
	gCfgItems.multiple_language = 1;
	gCfgItems.disable_pwr_det = 0;
	gCfgItems.disable_mt_det = 0;
	gCfgItems.OVERTURN_180 = 0;
	gCfgItems.single_nozzle=0;
	gCfgItems.babystep=0;
	
	gCfgItems.zoffset_disp_flag = 0;
	memset((void *)gCfgItems.disp_zoffset_buf,0,sizeof(gCfgItems.disp_zoffset_buf));
	
	recover_cfg_inf();

	gCfgItems.custom_pic_flag = 1;

	get_sd_cfg();

	if(gCfgItems.OVERTURN_180 != 0xee) //不翻转
	{
		gCfgItems.OVERTURN_180 = 0;
	}
	
	gCfgItems.filament_loading_time = (uint16_t)((gCfgItems.filament_load_length*60.0/gCfgItems.filament_load_speed));
	gCfgItems.filament_unloading_time = (uint16_t)((gCfgItems.filament_unload_length*60.0/gCfgItems.filament_unload_speed));

	//code_page_sel();
	if(gCfgItems.multiple_language == 0)
	{
		if(gCfgItems.language_bak != 0)
		{
			gCfgItems.language = gCfgItems.language_bak;
			I2C_EE_BufferWrite((uint8_t *)&gCfgItems.language,BAK_LANGUARY_ADDR,1);	
		}
	}

	SPI_FLASH_BufferRead(&flash_inf_valid_flag,FlASH_INF_VALID_ADDR,4);
	if(flash_inf_valid_flag == FLASH_INF_VALID_FLAG)
	{
		SPI_FLASH_BufferRead(wifiPara.ap_name,WIFI_NAME_ADDR,32);
		SPI_FLASH_BufferRead(wifiPara.keyCode,WIFI_KEYCODE_ADDR,64);
		SPI_FLASH_BufferRead(ipPara.ip_addr,WIFI_IP_ADDR,15);
		SPI_FLASH_BufferRead(ipPara.mask,WIFI_MASK_ADDR,15);
		SPI_FLASH_BufferRead(ipPara.gate,WIFI_GATE_ADDR,15);
		SPI_FLASH_BufferRead((uint8_t*)&ipPara.dhcp_flag,WIFI_DHCP_FLAG_ADDR,1);
		SPI_FLASH_BufferRead((uint8_t*)&gCfgItems.wifi_mode_sel,WIFI_MODE_SEL_ADDR,1);
		SPI_FLASH_BufferRead((uint8_t*)&gCfgItems.wifi_type,WIFI_MODE_TYPE_ADDR,1);
		if((gCfgItems.wifi_type != 0x1) && (gCfgItems.wifi_type != 0x2))
		{
			gCfgItems.wifi_type = 0x2;
		}
		if(gCfgItems.wifi_type == HLK_WIFI)
		{
			wifiPara.mode = gCfgItems.wifi_mode_sel;
		}
	}
	
	//I2C_EE_BufferWrite((uint8_t *)&gCfgItems.custom_pic_flag, BAK_CUSTOM_PIC_ADDR,  1);

	//if(gCfgItems.custom_pic_flag == 1)
	//{
		bmp_struct.XSize = 78;
		bmp_struct.YSize = 104;
		BMP_PIC_X = 0 ;
		BMP_PIC_Y = 0;
	//}
	/*
	else
	{
		bmp_struct.XSize = 80;
		bmp_struct.YSize = 50;
		BMP_PIC_X = 0 ;
		BMP_PIC_Y = 17;
	}
	*/

}



int send2Printer()
{
#if 0	
	int i = 0;
	int len, total_len = 0;
	int8_t tmpP;
	int8_t buf[128]= {0};
	

///////	if(print_task_flag != TASK_GOING)
	{
		return -1;
	}
	
//	if(printer_state >= PRINTER_OPERATE_OK)
	{		
		while(gcode_cache < MAX_CMD_SIZE)
		{
			memset(buf, 0, sizeof(buf));
			len = readUsartFifo(&UartTxFifo, buf, sizeof(buf) - 1);
			if(len > 0)
			{
					
					//printf("%s", buf);
					
					if(((strstr(buf, "G") != 0) || (strstr(buf, "M") != 0) || (strstr(buf, "T") != 0)) && ((strstr(buf, "\n") != 0) || (strstr(buf, "\r") != 0)))
					{
					UART2_Printf(buf, len);
					//	printer_state--;
						gcode_cache++;
					}
					total_len += len;
					
				
			}
			else
			{
				break;
			}
		}

		if(total_len)
		{
			setProBarValue(total_len);
		}
	}
	#endif

}

int32_t send_to_printer_ready(int8_t *buf, int32_t size)
{
#if 0
	if(buf == 0 || size <= 0)
	{
			return 0;
	}
	return writeUsartFifo(&UartTxFifo, buf, size);
	#endif
}

void get_cur_temp()
{
	pushFIFO(&gcodeCmdTxFIFO, GET_CUR_TEM_COMMAND);
}
