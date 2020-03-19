#include "stdint.h"
#include "printer.h"
#include "mks_tft_reprint.h"
#include "bak_cfg_inf.h"
#include "sd_usr.h"

#include "wifi_module.h"

extern CFG_ITMES gCfgItems;
extern unsigned char bmp_public_buf[16 * 1024];

void bakup_cfg_inf(BAK_INF_ITEM_ID  cfgItemId, uint32_t  value)
{
	switch(cfgItemId)
	{
		case BAK_BAUD_ID:
			I2C_EE_BufferWrite((uint8_t *)&value, BAK_BAUDRATE_ADDR,  1);
			break;

		case BAK_LANG_ID:
			I2C_EE_BufferWrite((uint8_t *)&value, BAK_LANGUARY_ADDR,  1);
			break;

		case BAK_MOV_SPEED_ID:
			I2C_EE_BufferWrite((uint8_t *)&value, BAK_MOV_SPEED_ADDR,  1);
			break;
			
		case BAK_MOV_DIST_ID:
			I2C_EE_BufferWrite((uint8_t *)&value, BAK_MOV_DIST_ADDR,  1);
			break;
				
		case BAK_EXTRU_SPEED_ID:
			I2C_EE_BufferWrite((uint8_t *)&value, BAK_EXTRU_SPEED_ADDR,  1);
			break;

		case BAK_MACH_TYPE_ID:
			I2C_EE_BufferWrite((uint8_t *)&value, BAK_MACHINE_TYPE_ADDR,  1);
			break;
			
		case BAK_SPRAY_NUM_ID:
			I2C_EE_BufferWrite((uint8_t *)&value, BAK_SPRAYER_NUM_ADDR,  1);
			break;
				
		case BAK_FAN_SPEED_ID:
			I2C_EE_BufferWrite((uint8_t *)&value, BAK_FAN_SPEED_ADDR,  1);
			break;
					
		case BAK_FILE_SYS_ID:
			I2C_EE_BufferWrite((uint8_t *)&value, BAK_FILE_SYS_ADDR,  1);
			break;

		case BAK_TOUCH_ADJ_FLAG_ID:
			//I2C_EE_BufferWrite((uint8_t *)&value, BAK_TOUCH_ADJ_FLAG_ADDR,  1);
			break;

		case BAK_TOUCH_ADJ_XMAX_ID:
			//I2C_EE_BufferWrite((uint8_t *)&value, BAK_TOUCH_ADJ_XMAX_ADDR,  4);
			break;

		case BAK_TOUCH_ADJ_XMIN_ID:
			//I2C_EE_BufferWrite((uint8_t *)&value, BAK_TOUCH_ADJ_XMIN_ADDR,  4);
			break;

		case BAK_TOUCH_ADJ_YMAX_ID:
			//I2C_EE_BufferWrite((uint8_t *)&value, BAK_TOUCH_ADJ_YMAX_ADDR,  4);
			break;

		case BAK_TOUCH_ADJ_YMIN_ID:
			//I2C_EE_BufferWrite((uint8_t *)&value, BAK_TOUCH_ADJ_YMIN_ADDR,  4);
			break;

		case BAK_CUSTOM_PIC_ID:
			//I2C_EE_BufferWrite((uint8_t *)&value, BAK_CUSTOM_PIC_ADDR,  1);

		default:
			break;
	}
}

void bakup_file_path(uint8_t *path, uint32_t  len)
{
	if((path == 0) || (len == 0))
	{
		return;
	}
	I2C_EE_BufferWrite((uint8_t *)&len, BAK_FILE_PATH_LEN_ADDR,  1);
	I2C_EE_BufferWrite(path, BAK_FILE_PATH_ADDR,  len);
}


void recover_cfg_inf()
{
	uint32_t  valid_flag;
	uint32_t  tmp_reprint_flag;
	uint32_t  tmp_reprint_offset;

	I2C_EE_BufferRead((uint8_t *)&valid_flag, BAK_VALID_FLAG_ADDR,  4); 
	if(valid_flag == BAK_INF_VALID_FLAG)
	{
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.baud, BAK_BAUDRATE_ADDR,  1); 
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.language, BAK_LANGUARY_ADDR,  1); 
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.moveSpeed, BAK_MOV_SPEED_ADDR,  1); 
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.move_dist, BAK_MOV_DIST_ADDR,  1); 
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.extruSpeed, BAK_EXTRU_SPEED_ADDR,  1); 
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.machineType, BAK_MACHINE_TYPE_ADDR,  1); 
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.sprayerNum, BAK_SPRAYER_NUM_ADDR,  1); 
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.fanSpeed, BAK_FAN_SPEED_ADDR,  1); 
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.fileSysType, BAK_FILE_SYS_ADDR,  1); 
		//从V1.2.0版本以后，不再需要校正屏幕
		/*
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.touch_adj_flag, BAK_TOUCH_ADJ_FLAG_ADDR,  1); 
		if(gCfgItems.touch_adj_flag == 0xaa)
		{
			I2C_EE_BufferRead((uint8_t *)&gCfgItems.touch_adj_xMin, BAK_TOUCH_ADJ_XMIN_ADDR,  4); 
			I2C_EE_BufferRead((uint8_t *)&gCfgItems.touch_adj_xMax, BAK_TOUCH_ADJ_XMAX_ADDR,  4); 
			I2C_EE_BufferRead((uint8_t *)&gCfgItems.touch_adj_yMin, BAK_TOUCH_ADJ_YMIN_ADDR,  4); 
			I2C_EE_BufferRead((uint8_t *)&gCfgItems.touch_adj_yMax, BAK_TOUCH_ADJ_YMAX_ADDR,  4); 
		}
		*/
		I2C_EE_BufferRead((uint8_t *)&tmp_reprint_flag, BAK_REPRINT_INFO,  4); 
		gCfgItems.rePrintFlag = tmp_reprint_flag >> 24;
		//断电偏移量出错处理
		I2C_EE_BufferRead((uint8_t *)&tmp_reprint_offset, BAK_REPRINT_OFFSET,  4);
		if(tmp_reprint_offset == 0xffffffff)
		{
			gCfgItems.rePrintFlag = printer_normal;
		}		

		//I2C_EE_BufferRead((uint8_t *)&gCfgItems.custom_pic_flag, BAK_CUSTOM_PIC_ADDR,  1); 

		I2C_EE_BufferRead((uint8_t *)&gCfgItems.background_color, BAK_BACKGROUND_COLOR_ADD,4);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.title_color, BAK_TITIL_COLOR_ADD,4);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.state_background_color, BAK_STATE_BACKGROUND_COLOR_ADD,4);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.state_text_color, BAK_STATE_TEXT_COLOR_ADD,4);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.filename_color, BAK_FILENAME_COLOR_ADD,4);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.filename_background_color, BAK_FILENAME_BACKGROUND_COLOR_ADD,4);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.printingstate_word_background_color, BAK_PRINTINGSTATE_WORD_BACKGROUND_COLOR_ADD,4);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.printingstate_word_color, BAK_PRINTINGSTATE_WORD_COLOR_ADD,4);
		
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.MoreItem_pic_cnt, BAK_MOREITEM_PIC_CNT,1);	

		I2C_EE_BufferRead((uint8_t *)&gCfgItems.custom_bed_flag, BAK_CUSTOM_BED_FLAG_ADDR,1);
		
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.button_3d_effect_flag, BAK_BUTTON_3D_EFFECT_FLAG,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.firmware_type,BAK_FIRMWARETYPE_ADDR,1);

		I2C_EE_BufferRead((uint8_t *)&gCfgItems.Pause_XPOS,BAK_PAUSE_XPOS_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.Pause_YPOS,BAK_PAUSE_YPOS_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.Pause_ZADD,BAK_PAUSE_ZADD_ADDR,2);

		
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.func_btn1_display_flag,BAK_FUNC1_DISPLAY_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.func_btn2_display_flag,BAK_FUNC2_DISPLAY_ADDR,1);

		I2C_EE_BufferRead((uint8_t *)&gCfgItems.preheat_max_desireSprayerTemp,BAK_SPRAYER1_MAXTEMP_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.preheat_max_desireBedTemp,BAK_BED_MAXTEMP_ADDR,2);

		I2C_EE_BufferRead((uint8_t *)&gCfgItems.filament_det1_level_flg,BAK_FILAMENT_DET1_FLAG_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.print_finish_close_machine_flg,BAK_PRINT_FINISH_CLOASE_MACHINE_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.morefunc_cnt,BAK_PRINTING_MOREFUNC_CNT_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.filament_load_speed,BAK_FILAMENT_LOAD_SPEED_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.filament_unload_speed,BAK_FILAMENT_UNLOAD_SPEED_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.filament_load_length,BAK_FILAMENT_LOAD_LENGTH_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.filament_unload_length,BAK_FILAMENT_UNLOAD_LENGTH_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.filament_limit_temper,BAK_FILAMENT_LIMIT_TERMPER_ADDR,2);
		//I2C_EE_BufferRead((uint8_t *)&gCfgItems.filament_unload_limit_temper,BAK_FILAMENT_UNLOAD_LIMIT_TERMPER_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_point_number,BAK_LEVELING_POINT_NUMBER_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_point1_x,BAK_LEVELING_POINT1_X_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_point1_y,BAK_LEVELING_POINT1_Y_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_point2_x,BAK_LEVELING_POINT2_X_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_point2_y,BAK_LEVELING_POINT2_Y_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_point3_x,BAK_LEVELING_POINT3_X_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_point3_y,BAK_LEVELING_POINT3_Y_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_point4_x,BAK_LEVELING_POINT4_X_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_point4_y,BAK_LEVELING_POINT4_Y_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_point5_x,BAK_LEVELING_POINT5_X_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_point5_y,BAK_LEVELING_POINT5_Y_ADDR,2);		
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_z_speed,BAK_LEVELING_Z_SPEED_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_xy_speed,BAK_LEVELING_XY_SPEED_ADDR,2);
		#ifdef SAVE_FROM_SD
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.pwroff_save_mode,BAK_PWROFF_SAVE_MODE_ADDR,1);	
		#endif
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.breakpoint_z_error,BAK_BREAKPOINT_Z_ERROR_ADDR,4);

		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_mode,BAK_LEVELING_MODE_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.filament_det2_level_flg,BAK_FILAMENT_DET2_FLAG_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.have_ups,BAK_HAVE_UPS_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.insert_det_module,BAK_INSERT_MODULE_ADDR,1);		
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.multiple_language,BAK_MULTIPLE_LANGUAGE_ADDR,1);
		//I2C_EE_BufferRead((uint8_t *)&gCfgItems.disable_pwr_det,BAK_DISABLE_PWR_DET_ADDR,1);
		//I2C_EE_BufferRead((uint8_t *)&gCfgItems.disable_mt_det,BAK_DISABLE_MT_DET_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.OVERTURN_180,BAK_OVERTURN_180_ADDR,1);
		//HC-chen
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.leveling_z_high,BAK_LEVELING_Z_HIGH,2);
		//HC 2.5
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.disable_pwr_det,BAK_DISABLE_PWR_DET_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.disable_mt_det,BAK_DISABLE_MT_DET_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.single_nozzle,BAK_SINGLE_NOZZLE_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.wifi_state, BAK_WIFI_STATE_ADDR,  1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.cloud_enable,BAK_CLOUD_FLAG_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.add_checkSum,BAK_M110_ADD_CHECKSUM_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.display_error,BAK_DISP_EEROR_ADDR,1);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.unload_retract_length,BAK_UNLOAD_RETRACT_LENGTH_ADDR,2);
		I2C_EE_BufferRead((uint8_t *)&gCfgItems.unload_retract_speed,BAK_UNLOAD_RETRACT_SPEED_ADDR,2);

		I2C_EE_BufferRead((uint8_t *)&gCfgItems.babystep,BAK_BABYSTEP_ADDR,1);
		
	}
	else
	{
		//for(i=0;i<256;i++)
		//{
		//	I2C_EE_BufferRead((uint8_t *)&bmp_public_buf[0],0,256);
		//}
		//while(1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.baud, BAK_BAUDRATE_ADDR,  1); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.language, BAK_LANGUARY_ADDR,  1); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.moveSpeed, BAK_MOV_SPEED_ADDR,  1); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.move_dist, BAK_MOV_DIST_ADDR,  1); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.extruSpeed, BAK_EXTRU_SPEED_ADDR,  1); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.machineType, BAK_MACHINE_TYPE_ADDR,  1); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.sprayerNum, BAK_SPRAYER_NUM_ADDR,  1); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.fanSpeed, BAK_FAN_SPEED_ADDR,  1); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.fileSysType, BAK_FILE_SYS_ADDR,  1);
		/*
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.touch_adj_flag, BAK_TOUCH_ADJ_FLAG_ADDR,  1); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.touch_adj_xMin, BAK_TOUCH_ADJ_XMIN_ADDR,  4); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.touch_adj_xMax, BAK_TOUCH_ADJ_XMAX_ADDR,  4); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.touch_adj_yMin, BAK_TOUCH_ADJ_YMIN_ADDR,  4); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.touch_adj_yMax, BAK_TOUCH_ADJ_YMAX_ADDR,  4); 
		*/
		//I2C_EE_BufferWrite((uint8_t *)&gCfgItems.custom_pic_flag, BAK_CUSTOM_PIC_ADDR,  1); 
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.background_color, BAK_BACKGROUND_COLOR_ADD,4);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.title_color, BAK_TITIL_COLOR_ADD,4);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.state_background_color, BAK_STATE_BACKGROUND_COLOR_ADD,4);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.state_text_color, BAK_STATE_TEXT_COLOR_ADD,4);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filename_color, BAK_FILENAME_COLOR_ADD,4);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filename_background_color, BAK_FILENAME_BACKGROUND_COLOR_ADD,4);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.printingstate_word_background_color, BAK_PRINTINGSTATE_WORD_BACKGROUND_COLOR_ADD,4);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.printingstate_word_color, BAK_PRINTINGSTATE_WORD_COLOR_ADD,4);
		
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.MoreItem_pic_cnt, BAK_MOREITEM_PIC_CNT,1);	

		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.custom_bed_flag,BAK_CUSTOM_BED_FLAG_ADDR,1);

		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.button_3d_effect_flag,BAK_BUTTON_3D_EFFECT_FLAG,1);

		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.firmware_type,BAK_FIRMWARETYPE_ADDR,1);

		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.Pause_XPOS,BAK_PAUSE_XPOS_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.Pause_YPOS,BAK_PAUSE_YPOS_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.Pause_ZADD,BAK_PAUSE_ZADD_ADDR,2);

		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.func_btn1_display_flag,BAK_FUNC1_DISPLAY_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.func_btn2_display_flag,BAK_FUNC2_DISPLAY_ADDR,1);

		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.preheat_max_desireSprayerTemp,BAK_SPRAYER1_MAXTEMP_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.preheat_max_desireBedTemp,BAK_BED_MAXTEMP_ADDR,2);

		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_det1_level_flg,BAK_FILAMENT_DET1_FLAG_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.print_finish_close_machine_flg,BAK_PRINT_FINISH_CLOASE_MACHINE_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.morefunc_cnt,BAK_PRINTING_MOREFUNC_CNT_ADDR,1);
		
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_load_speed,BAK_FILAMENT_LOAD_SPEED_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_unload_speed,BAK_FILAMENT_UNLOAD_SPEED_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_load_length,BAK_FILAMENT_LOAD_LENGTH_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_unload_length,BAK_FILAMENT_UNLOAD_LENGTH_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_limit_temper,BAK_FILAMENT_LIMIT_TERMPER_ADDR,2);
		//I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_unload_limit_temper,BAK_FILAMENT_UNLOAD_LIMIT_TERMPER_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point_number,BAK_LEVELING_POINT_NUMBER_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point1_x,BAK_LEVELING_POINT1_X_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point1_y,BAK_LEVELING_POINT1_Y_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point2_x,BAK_LEVELING_POINT2_X_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point2_y,BAK_LEVELING_POINT2_Y_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point3_x,BAK_LEVELING_POINT3_X_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point3_y,BAK_LEVELING_POINT3_Y_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point4_x,BAK_LEVELING_POINT4_X_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point4_y,BAK_LEVELING_POINT4_Y_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point5_x,BAK_LEVELING_POINT5_X_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point5_y,BAK_LEVELING_POINT5_Y_ADDR,2);		
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_z_speed,BAK_LEVELING_Z_SPEED_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_xy_speed,BAK_LEVELING_XY_SPEED_ADDR,2);
		#ifdef SAVE_FROM_SD
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.pwroff_save_mode,BAK_PWROFF_SAVE_MODE_ADDR,1);
		#endif
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.breakpoint_z_error,BAK_BREAKPOINT_Z_ERROR_ADDR,4);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_mode,BAK_LEVELING_MODE_ADDR,1);		
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_det2_level_flg,BAK_FILAMENT_DET2_FLAG_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.have_ups,BAK_HAVE_UPS_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.insert_det_module,BAK_INSERT_MODULE_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.multiple_language,BAK_MULTIPLE_LANGUAGE_ADDR,1);
		//I2C_EE_BufferWrite((uint8_t *)&gCfgItems.disable_pwr_det,BAK_DISABLE_PWR_DET_ADDR,1);
		//I2C_EE_BufferWrite((uint8_t *)&gCfgItems.disable_mt_det,BAK_DISABLE_MT_DET_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.OVERTURN_180,BAK_OVERTURN_180_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_z_high,BAK_LEVELING_Z_HIGH,2);
		//HC 2.5
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.disable_pwr_det,BAK_DISABLE_PWR_DET_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.disable_mt_det,BAK_DISABLE_MT_DET_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.single_nozzle,BAK_SINGLE_NOZZLE_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.wifi_state, BAK_WIFI_STATE_ADDR,  1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.cloud_enable,BAK_CLOUD_FLAG_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.add_checkSum,BAK_M110_ADD_CHECKSUM_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.display_error,BAK_DISP_EEROR_ADDR,1);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.unload_retract_length,BAK_UNLOAD_RETRACT_LENGTH_ADDR,2);
		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.unload_retract_speed,BAK_UNLOAD_RETRACT_SPEED_ADDR,2);

		I2C_EE_BufferWrite((uint8_t *)&gCfgItems.babystep,BAK_BABYSTEP_ADDR,1);
		
		valid_flag = BAK_INF_VALID_FLAG;
		I2C_EE_BufferWrite((uint8_t *)&valid_flag, BAK_VALID_FLAG_ADDR,  4); 		
	}

}


