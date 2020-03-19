/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   USB host MSC class demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#include "xprintf.h"
#include "sys.h"
#include "i2c_ee.h"
#include "gui.h"
#include "button.h"
#include "PROGBAR.h"
#include "draw_ui.h"
#include "printer.h"
#include "touch_calibrate.h"
#include "mks_tft_com.h"
#include "sd_usr.h"
#include "printing_thread.h"
#include "fs_usr.h"
#include "wifi_module.h"

#include "SPI_FLASH.h"

/** @defgroup USBH_USR_MAIN_Private_Variables
* @{
*/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;

extern uint8_t filament_loading_complete;
extern uint8_t filament_heating_flg;
extern uint8_t filament_loading_flg;
extern uint8_t filament_load_cmd_flg;
extern uint8_t filament_load_timing_flg;
extern uint16_t filament_load_timing_cnt;


//extern uint8_t btn_putdown_time_flg;
//extern uint32_t btn_putdown_cnt;

#define PW_PORT_READ    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)
#define MT_PORT_READ    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)
#define DT_PORT_READ    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)

uint8_t poweroff_det_flg;
uint32_t poweroff_det_cnt;
uint8_t poweroff_det_low_flg;
uint32_t poweroff_det_low_cnt;
uint8_t poweroff_det_high_flg;
uint32_t poweroff_det_high_cnt;

uint8_t filament_det1_flg;
uint32_t filament_det1_cnt;
uint8_t filament_det1_low_flg;
uint32_t filament_det1_low_cnt;
uint8_t filament_det1_high_flg;
uint32_t filament_det1_high_cnt;
uint8_t filament_det1_check;

uint8_t filament_det2_flg;
uint32_t filament_det2_cnt;
uint8_t filament_det2_low_flg;
uint32_t filament_det2_low_cnt;
uint8_t filament_det2_high_flg;
uint32_t filament_det2_high_cnt;
uint8_t filament_det2_check;

uint8_t repetier_repint_flag = 0;

uint8_t Get_Temperature_Flg = 0;

//uint8_t PWRDN_MTRDN_flg = 0;
//uint16_t PWRDN_MTRDN_Tcnt_flg = 0;

uint8_t close_fail_flg = 1;
uint16_t close_fail_cnt = 0;

uint8_t pause_from_mt_high_level=0;
uint8_t pause_from_pw_high_level=0;


DISP_STATE disp_state = MAIN_UI;
DISP_STATE last_disp_state;

uint8_t volatile printing_rate_update_flag = 0;

PR_STATUS printerStaus = pr_idle;

extern uint8_t temp_update_flag;
extern uint8_t trig_browser_flag;

extern PRINT_TIME  print_time;

volatile uint8_t get_temp_flag = 0;
static volatile uint8_t fan_move_flag = 0;
static volatile uint8_t fan_disp_speed_flag = 0;

uint8_t link_mutex_detect_time;

static FIL curFile;


extern uint8_t curFileName[100];
extern uint8_t gCurDir[100];

uint32_t rePrintOffset;

uint32_t wifi_loop_cycle = 500;

uint8_t fan_change_flag = 0;

static volatile uint8_t get_ack_flag = 0;

volatile int16_t logo_time = 0;
static volatile uint32_t TimeIncrease = 0;

#if 0
void PB4_Close_machine();
void PB0_PB1_pwrdn_mtrdn();
#else
void PowerOff_Filament_Check();
#endif

void clock_run();

extern uint16_t readLcdId;
extern char wifi_check_time;

//unsigned char ret;
extern void draw_logo();

//extern void DBCS_CODE_INIT();

extern uint8_t empty_filament_tips_flag;


uint8_t wifi_init_flg = 0;
uint8_t wifi_init_state = 0;
FATFS m_fs; 
static FIL save_File;

__IO uint32_t delaycnt1 = 0;


uint8_t wifi_refresh_flg = 0, cloud_refresh_flg = 0;

//uint32_t DesireSprayerTemp[2];
//uint32_t DesireBedTemp;
//uint8_t  Fanonoff = 0xff;
//uint8_t  Fanspeed_temp = 0xff;
//void Para_save(void);
#if debug_flg
uint8_t T_FLG = 1;
uint32_t T_CNT = 0;
void T_function(uint32_t x_cnt,uint8_t sel_type);
void T_GPIO_init();
#endif
void Btn_putdown_close_machine();
void Print_finish_close_machine();

//static uint8_t PWRDN_CNT = 0;

//uint8_t PWRDN_LOWLEVLE_Tcnt_flg=0;
//uint8_t PWRDN_LOWPOWER_FLG = 0;
//uint8_t PWRDN_HIGHLEVEL_Tcnt_flg=0;
//uint8_t PWRDN_HIGHLEVEL_FLG = 0;	


//uint8_t MTRDN_LOWLEVLE_Tcnt_flg=0;
//uint8_t MTRDN_LOWPOWER_FLG = 0;
//uint8_t MTRDN_HIGHLEVEL_Tcnt_flg=0;
//uint8_t MTRDN_HIGHLEVEL_FLG = 0;		

static uint8_t sd_level = 0;
static uint8_t last_sd_level = 0;
static uint8_t sd_det = 0;
static uint32_t sd_last_tick = 0;

//extern volatile uint8_t sd_cd;

uint32_t   getTick()
{
	return TimeIncrease;
}

uint32_t  getTickDiff(int32_t lastTick, int32_t  curTick)
{
	if(lastTick <= curTick)
	{
		return (curTick - lastTick) * TICK_CYCLE;
	}
	else
	{
		return (0xffffffff - lastTick + curTick) * TICK_CYCLE;
	}
}

extern void LCD_TEST();

int main(void)
{
	SD_CardInfo cardinfo;
	//uint8_t buffer_display_z[15];
	
	__IO uint32_t i = 0,j_t=0;
	
	uint8_t filePathLen;
 
	//xPrintf_Init();
	BACKLIGHT = BACKLIGHT_OFF;	
	
//	TIM3_Int_Init(10,7199);//10Khzμ???êy?μ?ê￡???êyμ?5000?a500ms  
   	
	//delay_init();
	
	//AT24CXX_Init();			//IIC初始化

	//NVIC_Configuration();	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	//中断向量设置
	//有bootloader时，设置为0x7000, Options ->Linker ->Edit...-> 0x08007000
	//无bootloader时，设置为0x0000, Optio  ns ->Linker ->Edit...-> 0x08000000 	
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x7000);	

	I2C_EE_Init(100000);
	
	init_win_id();
	
	SD_Initialize();
	SD_GetCardInfo(&cardinfo);
	
	//GUI_Init();
	dma_init();
	SPI_FLASH_Init();	
	
	
	printer_init();	

	GUI_Init();
	//SPI3_Init();
	gui_view_init();

	//DBCS_CODE_INIT();
	//if(gCfgItems.touch_adj_flag != 0xaa)
	//{
	///	Touch_Adjust();
	//}
	disp_language_init();
	
	GUI_UC_SetEncodeUTF8();


	setTouchBound(gCfgItems.touch_adj_xMin, gCfgItems.touch_adj_xMax, gCfgItems.touch_adj_yMax, gCfgItems.touch_adj_yMin);
	#if defined(STM32F10X_CL)
	/* Init Host Library */
	for(j_t = 0;j_t < 3;j_t++)
	{
		USBH_Init(&USB_OTG_Core, 
		#ifdef USE_USB_OTG_FS  
		        USB_OTG_FS_CORE_ID,
		#else 
		        USB_OTG_HS_CORE_ID,
		#endif 
		        &USB_Host,
		        &USBH_MSC_cb, 
		        &USR_cb);
	/*

		i = 0;
		while(i < 1000)
		{
			USBH_Process(&USB_OTG_Core, &USB_Host);
			
			if(USBH_USR_ApplicationState == USH_USR_FS_READLIST)
			{
				break;
			}
			i++;
		}
	*/

		delaycnt1 = 0;
		while(delaycnt1 < 50000)
		{
			USBH_Process(&USB_OTG_Core, &USB_Host);
			
			if(USBH_USR_ApplicationState == USH_USR_FS_READLIST)
			{
				break;
			}
			delaycnt1++;
		}	
	}	
	#endif
	SZ_STM32_COM_Init(gCfgItems.baud);  
	delaycnt1 = 0;
	while(delaycnt1 < 10000)
	{
		delaycnt1++;
	}
	//wifi_init();
	mksUsart2Init();

	#if debug_flg == 1
	T_GPIO_init();
	#endif
	
	BACKLIGHT = BACKLIGHT_ON;	

	SZ_STM32_SysTickInit(1000 / TICK_CYCLE);

	//HC-chen 7.26 Test Fala
	Test_fala();
	
	#ifdef SAVE_FROM_SD
	if((gCfgItems.pwroff_save_mode == 0)&&((RE_PRINT_STATUS)gCfgItems.rePrintFlag != printer_pause_reprint))
	{
		sd_data_recover();
	}
	#endif

	
	
	if(((RE_PRINT_STATUS)gCfgItems.rePrintFlag == printer_pause_reprint) || ((RE_PRINT_STATUS)gCfgItems.rePrintFlag == printer_pwdwn_reprint))
	{
		repetier_repint_flag = 1;
		I2C_EE_Init(100000);
		SPEAKER = 1; 
		delaycnt1 = 0; while(delaycnt1 < 10000)delaycnt1++;
		SPEAKER = 0; 
		delaycnt1 = 0; while(delaycnt1 < 10000)delaycnt1++;
		SPEAKER = 1; 
		delaycnt1 = 0; while(delaycnt1 < 10000)delaycnt1++;	
		I2C_EE_BufferRead((uint8_t *)&filePathLen, BAK_FILE_PATH_LEN_ADDR,  1);
		if(filePathLen > 0)
		{
			int8_t res;
			//uint32_t time;
			
			I2C_EE_BufferRead(curFileName, BAK_FILE_PATH_ADDR,  filePathLen);

			#ifdef SAVE_FROM_SD
			if((RE_PRINT_STATUS)gCfgItems.rePrintFlag == printer_pause_reprint||(gCfgItems.pwroff_save_mode != 0))
			#endif
			{
				I2C_EE_BufferRead((uint8_t *)&rePrintOffset, BAK_REPRINT_OFFSET,  4);
				rePrintdata_show();
			}
			/*
			I2C_EE_BufferRead((uint8_t *)&time, BAK_REPRINT_TIME,  4);
			print_time.hours = (time >> 16)  & 0xffff;
			print_time.minutes = (time >> 8)  & 0xff;

			if((print_time.hours == 0xffff)&&(print_time.minutes == 0xff))
			{
				print_time.hours = 0;
				print_time.minutes = 0;
			}
			dataToEeprom = 0xffffffff;
			I2C_EE_BufferWrite(&dataToEeprom, BAK_REPRINT_TIME,  4);
			*/

			// f_mount(1, &fs);
			memset(gCurDir, 0, sizeof(gCurDir));
			
			if(gCfgItems.fileSysType == FILE_SYS_SD)
			{
				strcpy(gCurDir, "1:");
				SD_Initialize();
				ShowSDFiles();
			}
			else
			{
				strcpy(gCurDir, "0:");				
			}
				res = f_open(&curFile, curFileName, FA_OPEN_EXISTING | FA_READ);
				if(res == FR_OK)
				{
					srcfp = &curFile;
					gCurFileState.file_open_flag = 0xaa;
					gCurFileState.totalSend = (unsigned char)((float)rePrintOffset  * 100 / f_size(&curFile));
					

					//if((RE_PRINT_STATUS)gCfgItems.rePrintFlag == printer_pause_reprint)
					{		
						//HC-chen 2.28
						#if 0
						printerStaus = pr_reprint;
						disp_state_stack._disp_index = 0;
						memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
						disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINTING_UI;
						draw_pause();

						#else
						//断电续打喷头保存不到温度,温度为0时手动加温度
						if( ((gCfgItems.desireSprayerTemp[0] ==0)  || ((gCfgItems.sprayerNum == 2)&&(gCfgItems.desireSprayerTemp[1] ==0)))\
							&&((RE_PRINT_STATUS)gCfgItems.rePrintFlag == printer_pwdwn_reprint))
						{
							draw_dialog(DIALOG_TYPE_REPRINT_NO_TEMP);
						}
						else
						{
							printerStaus = pr_reprint;
							disp_state_stack._disp_index = 0;
							memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
							disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINTING_UI;
							draw_pause();
						}

						#endif
						
					}
				/*	else if((RE_PRINT_STATUS)gCfgItems.rePrintFlag == printer_pwdwn_reprint)
					{
						printerStaus = pr_working;
						start_print_time();
						draw_printing();
					}*/
					
					logo_time = 3000 / TICK_CYCLE;
				}
				else
				{
				#if defined(STM32F10X_CL)
					USBH_Init(&USB_OTG_Core, 
										#ifdef USE_USB_OTG_FS  
														USB_OTG_FS_CORE_ID,
										#else 
														USB_OTG_HS_CORE_ID,
										#endif 
										&USB_Host,
										&USBH_MSC_cb, 
										&USR_cb); 		
					delaycnt1 = 0;
					while(delaycnt1 < 50000)
					{
							USBH_Process(&USB_OTG_Core, &USB_Host);

							if(USBH_USR_ApplicationState == USH_USR_FS_READLIST)
							{
								break;
							}
							delaycnt1++;
					}
					#endif

					memset(gCurDir, 0, sizeof(gCurDir));

					if(gCfgItems.fileSysType == FILE_SYS_SD)
					{
						strcpy(gCurDir, "1:");
						SD_Initialize();
						ShowSDFiles();
					}
					else
					{
						strcpy(gCurDir, "0:");
					}

					res = f_open(&curFile, curFileName, FA_OPEN_EXISTING | FA_READ);
					if(res == FR_OK)
					{
						srcfp = &curFile;
						gCurFileState.file_open_flag = 0xaa;
						gCurFileState.totalSend = (unsigned char)((float)rePrintOffset  * 100 / f_size(&curFile));

						
						printerStaus = pr_reprint;
						disp_state_stack._disp_index = 0;
						memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
						disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINTING_UI;
						draw_pause();
					}
					else
					{
						disp_state_stack._disp_index = 0;
						memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
						disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINT_READY_UI;
						draw_dialog(DIALOG_TYPE_REPRINT_NO_FILE);
					}
					
				

				/*
					disp_state_stack._disp_index = 0;
					memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
					disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINT_READY_UI;
					draw_dialog(DIALOG_TYPE_REPRINT_NO_FILE);
			*/
			
					
				}
		
			
		}
		else
		{
			draw_main_ui();	
			logo_time = 0;
		}
	}
	else
	{
		draw_main_ui();	
		logo_time = 0;
	}	
	if(gCfgItems.wifi_type == ESP_WIFI)
	{
		wifi_init();
	}
	
	SPEAKER = 0;
	  while (1)
	  {
	  //if(readLcdId != 0x1505)
	  	//LCD_TEST();
	  	//code_page_sel();
		if(gCfgItems.fileSysType == FILE_SYS_SD)
		{
		  	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == Bit_RESET)
		  	{
		  		//sd_cd=0;
		  		sd_level = 0;
				if(last_sd_level == 1) 
				{
					sd_det = 1;		//有时SD卡需要插两次才能进去	
					sd_last_tick = getTick();
				}
				#if defined(STM32F10X_CL)
				if((sd_det == 1) && getTickDiff(sd_last_tick, getTick()) > 3000)
				{
					SD_Initialize();
					sd_det = 0;
				}
				#endif
				#if defined(STM32F10X_HD)
				if((sd_det == 1) && getTickDiff(sd_last_tick, getTick()) > 10)
				{
					SD_Initialize();
					sd_det = 0;
				}
				#endif				
		  	}
			else
			{
				sd_level = 1;
				//sd_cd=1;
			}
			
			last_sd_level = sd_level;
		}
	  	if(wifi_link_state != WIFI_TRANS_FILE)
	  	{
			switch(disp_state)
			{
				case MAIN_UI:
					
					if(logo_time * TICK_CYCLE >= 3000 )
					{
						//logo_time = 3000 / TICK_CYCLE;
						clear_main_ui();
						last_disp_state = MAIN_UI;
						draw_ready_print();
						
					}
					break;

				case EXTRUSION_UI:
					if(temp_update_flag)
					{
						temp_update_flag = 0;
						disp_sprayer_temp();
					}
					break;

				case PRE_HEAT_UI:
					if(temp_update_flag)
					{
						temp_update_flag = 0;
						disp_desire_temp();
					}
					break;

				case PRINT_READY_UI:
				/*	if(fan_move_flag)
					{
						fan_move_flag = 0;
						disp_fan_move();
					}*/
					#if SIMPLE_DISPLAY == 2
					if(temp_update_flag)
					{
						temp_update_flag = 0;
						disp_sprayer_temp_main();
						disp_bed_temp_main();
					}
					if(fan_move_flag)
					{
						fan_move_flag = 0;
						disp_fan_move_main();
					}
					/*if(fan_disp_speed_flag)
					{
						fan_disp_speed_flag= 0;
						disp_fan_speed_main();
					}*/
					#endif
					break;

				case PRINT_FILE_UI:
					break;

				case PRINTING_UI:
			
					if(temp_update_flag)
					{
						temp_update_flag = 0;
						disp_sprayer_tem_printing();
						disp_bed_temp_printing();
					}
					if(printing_rate_update_flag)
					{
						printing_rate_update_flag = 0;
						setProBarRate(get_printing_rate(srcfp));
					}
					#if VERSION_WITH_PIC
					if(fan_move_flag)
					{
						fan_move_flag = 0;
						disp_fan_move_printing();
					}
					/*if(fan_disp_speed_flag)
					{
						fan_disp_speed_flag= 0;
						disp_fan_speed_printing();
					}*/
					#endif
					break;

				case OPERATE_UI:
					if(temp_update_flag)
					{
						temp_update_flag = 0;
						disp_temp_operate();
					}
					if(printing_rate_update_flag)
					{
						printing_rate_update_flag = 0;
						setProBarRateOpera(get_printing_rate(srcfp));
					}
					//HC-chen 2.8
					if(fan_change_flag)
					{
						fan_change_flag = 0;
						disp_temp_operate();
					}
					if((printerStaus == pr_idle)&&(gCurFileState.totalSend == 100))
					{
						clear_cur_ui();
						draw_printing();
					}

					
					break;

				case PAUSE_UI:
					if(printerStaus == pr_idle)
					{
						Clear_pause();
						stop_print_time();
						f_close(srcfp);

						reset_file_info();
						//reset_tx_fifo();
						draw_ready_print();
					}
					if(temp_update_flag)
					{
						temp_update_flag = 0;
						disp_temp_pause();
					}
					
					if(fan_change_flag)
					{
						fan_change_flag = 0;
						disp_temp_pause();
					}
					break;
					
				case FAN_UI:
					if(fan_change_flag)
					{
						fan_change_flag = 0;
						disp_fan_speed();
					}
					break;
					
				case MOVE_MOTOR_UI:
						//显示Z轴坐标
						if((gCfgItems.getzpos_enable ==0)&&(printerStaus != pr_pause))
						{
							i=0;
							while(RePrintData.mov_z_coordinate[i]!= '\n')
							{
								gCfgItems.move_z_coordinate[i] = RePrintData.mov_z_coordinate[i];
								i++;
								if(i>20)break;
							}
							gCfgItems.move_z_coordinate[i] = 0;
							GUI_DispStringAtCEOL(gCfgItems.move_z_coordinate,220, 0);
						}

					break;

			case WIFI_UI:
				if(wifi_refresh_flg == 1)
				{					
					disp_wifi_state();
					wifi_refresh_flg = 0;
				}
				break;
			case PRINT_MORE_UI:
				//Autoshutdown_display();
				break;
			case FILAMENTCHANGE_UI:
					if(temp_update_flag)
					{
						temp_update_flag = 0;
						disp_filament_sprayer_temp();
					}
					#if 0
					//换料处理
					//FilamentChange_handle();
					if(filament_heating_flg == 1)
					{
						if(gCfgItems.curSprayerTemp[0]>=gCfgItems.filament_unload_limit_temper)
						{
							filament_heating_flg = 0;
							filament_load_cmd_flg = 1;
							filament_loading_flg = 1;
						}
					}
					disp_filament_tips();
					#endif
					break;
			case BIND_UI:
					if(cloud_refresh_flg == 1)
					{
						//clear_cur_ui();
						//draw_bind();	
						refresh_bind_ui();
						cloud_refresh_flg = 0;
					}
					break;
			case DIALOG_UI:
				filament_dialog_handle();
				break;
			case BABY_STEP_UI:
				if(gCfgItems.zoffset_disp_flag == ENABLE)
				{
					gCfgItems.zoffset_disp_flag = DISABLE;
					//获得Zoffset数值
					
					//GUI_DispStringAt((const char *),290, TITLE_YPOS);
					GUI_SetColor(gCfgItems.background_color);
					GUI_FillRect(200, 0, LCD_WIDTH, titleHeight);
					GUI_SetColor(gCfgItems.title_color);
					GUI_DispStringAt((const char *)gCfgItems.disp_zoffset_buf,200, 0);
					
					//memset(gCfgItems.disp_zoffset_buf,0,sizeof(gCfgItems.disp_zoffset_buf));
				}
			break;
			default:
					break;
					
		    }

			
			
			mksUsart2Polling();

			
			
			clock_run();
		}
		#if defined(STM32F10X_CL)
		USBH_Process(&USB_OTG_Core, &USB_Host);
		#endif
		
		if(wifi_init_flg == 1)
		{
			if(gCfgItems.wifi_type == HLK_WIFI)
			{
				if(wifi_init_state == 0)
				{
					wifi_init();
					wifi_init_state = 1;
				}
				else
				{
					wifi_looping();
				}
			}
			else
				wifi_looping();
		}

		if(wifi_link_state != WIFI_TRANS_FILE)
		{
			if(get_temp_flag)
			{	
				get_temp_flag = 0;
				//if((printerStaus != pr_idle) || (printer_state == PRINTER_NOT_CONNECT))
				if(((link_mutex_detect_time >= 5) || (printer_state == PRINTER_NOT_CONNECT) || (printerStaus == pr_working))\
					&&(Get_Temperature_Flg == 1))
				{
					link_mutex_detect_time = 0;
					get_cur_temp();
				}

			}
			/*
			if(get_ack_flag)
			{
				get_ack_flag = 0;
				get_cmd_ack();
			}
			*/
			get_cmd_ack();
			
			pushButtonCodeToFifo();
			
		}
		#if 0
		PB0_PB1_pwrdn_mtrdn();
		PB4_Close_machine();
		#else
		PowerOff_Filament_Check();
		#endif
		#ifdef SAVE_FROM_SD
		if(gCfgItems.pwroff_save_mode == 0)
		{
			sd_saved_data();
		}
		#endif
		
		GUI_TOUCH_Exec();	
		GUI_Exec();

		
	
  
  }
}
extern int8_t gFile_Browser (char* path);
void display_udisk_files()
{
	//USBH_Process(&USB_OTG_Core, &USB_Host);
	gFile_Browser(gCurDir);
}

//uint8_t stop_cmd[7]="M81\n";

void Btn_putdown_close_machine()
{
	memset(usart2Data.usart2Txbuf,0,sizeof(usart2Data.usart2Txbuf));
	usart2Data.usart2Txbuf[0]='M';usart2Data.usart2Txbuf[1]='8';usart2Data.usart2Txbuf[2]='1';usart2Data.usart2Txbuf[3]='\n'; 	
	usart2TxStart();
}
#if 0
void Print_finish_close_machine()
{
	pushFIFO(&gcodeCmdTxFIFO,stop_cmd);
}
#endif
static uint8_t lastSec = 0;
void clock_run()
{
	
	if(print_time.seconds >= 60)
	{
		print_time.seconds = 0;
		print_time.minutes++;
		if(print_time.minutes >= 60)
		{
			print_time.minutes = 0;
			print_time.hours++;
			/*if(print_time.hours >= 24)
			{
				print_time.hours = 0;
				print_time.days++;
			}*/
		}
		
	}
	if(disp_state == PRINTING_UI)
	{
		if(lastSec != print_time.seconds)
		{
			disp_print_time();
		}
		lastSec =  print_time.seconds;
	}
}




/**-------------------------------------------------------
  * @函数名 SysTick_Handler_User
  * @功能   系统节拍定时器服务请求用户处理函数
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
uint8_t waiting_wifi_time = 0;

extern uint16_t resume_time;

uint8_t filament_loading_time_flg;
uint32_t filament_loading_time_cnt;
uint8_t filament_loading_completed;
uint8_t filament_unloading_time_flg;
uint32_t filament_unloading_time_cnt;
uint8_t filament_unloading_completed;
extern uint32_t filament_rate;
volatile uint8_t per_second_save_sd;

void SysTick_Handler_User()
//void Timer3_Handler_User(void)
{
	if(filament_loading_time_flg == 1)
	{
		filament_loading_time_cnt++;
		filament_rate = (((float)filament_loading_time_cnt/(gCfgItems.filament_loading_time*1000))*100);
		if(filament_loading_time_cnt >= (gCfgItems.filament_loading_time*1000))
		{
			filament_loading_time_cnt = 0;
			filament_loading_time_flg = 0;
			filament_loading_completed = 1;
		}
	}
	if(filament_unloading_time_flg == 1)
	{
		filament_unloading_time_cnt++;
		filament_rate = (((float)filament_unloading_time_cnt/(gCfgItems.filament_unloading_time*1000))*100);
		if(filament_unloading_time_cnt >= (gCfgItems.filament_unloading_time*1000))
		{
			filament_unloading_time_cnt = 0;
			filament_unloading_time_flg = 0;
			filament_unloading_completed = 1;
			filament_rate = 100;
		}
	}
	
/*
	if(btn_putdown_time_flg == 1)
	{
		btn_putdown_cnt++;//100
	}
	*/
	resume_time++;
	
	if(poweroff_det_flg==1)
	{
		poweroff_det_cnt++;
	}
	
	if(filament_det1_flg==1)
	{
		filament_det1_cnt++;
	}

	if(filament_det1_low_flg==1)
	{
		filament_det1_low_cnt++;
	}

	if(filament_det1_high_flg==1)
	{
		filament_det1_high_cnt++;
	}
	if(filament_det2_flg==1)
	{
		filament_det2_cnt++;
	}

	if(filament_det2_low_flg==1)
	{
		filament_det2_low_cnt++;
	}

	if(filament_det2_high_flg==1)
	{
		filament_det2_high_cnt++;
	}

	if(poweroff_det_low_flg==1)
	{
		poweroff_det_low_cnt++;
	}

	if(poweroff_det_high_flg==1)
	{
		poweroff_det_high_cnt++;
	}


#if 1
	if(!(TimeIncrease * TICK_CYCLE % 5000))	//5s
	{
		get_temp_flag = 1;
		wifi_check_time = 1;
		waiting_wifi_time ++;
		if((waiting_wifi_time == 8) || (gCfgItems.wifi_type == ESP_WIFI))//40s
		{
			if(wifi_init_flg == 0)
			{
				wifi_init_flg = 1;
			}
			
		}
		if(wifi_refresh_flg == 0)
			wifi_refresh_flg = 1;
		
		if(cloud_refresh_flg == 0)
			cloud_refresh_flg = 1;
		
	}
	
	if(!(TimeIncrease * TICK_CYCLE % 3000))	// 3s
	{		
		printing_rate_update_flag = 1;
	}
	
	if(!(TimeIncrease * TICK_CYCLE % 1000))	// 1s
	{		
		get_ack_flag = 1;
#if 0
		if(PWRDN_MTRDN_Tcnt_flg == 1)
		{
			PWRDN_MTRDN_flg = 1;
		}
		
		if(PWRDN_LOWLEVLE_Tcnt_flg==1)
		{
			PWRDN_LOWPOWER_FLG = 1;
		}
		
		if(PWRDN_HIGHLEVEL_Tcnt_flg==1)
		{
			PWRDN_HIGHLEVEL_FLG = 1;
		}
		
		if(MTRDN_LOWLEVLE_Tcnt_flg==1)
		{
			MTRDN_LOWPOWER_FLG = 1;
		}
		
		if(MTRDN_HIGHLEVEL_Tcnt_flg==1)
		{
			MTRDN_HIGHLEVEL_FLG = 1;
		}
#endif
		if(close_fail_flg == 1)
		{
			close_fail_cnt++;
			if(close_fail_cnt >= 5)
			{
				close_fail_cnt = 0;
				close_fail_flg = 0;
			}
		}
	}
	if(!(TimeIncrease * TICK_CYCLE % wifi_loop_cycle))	// 0.5s
	{
		wifi_loop_time = 1;	
	}
	//if(!(TimeIncrease * TICK_CYCLE % 100))	// 0.1s
	//HC-chen
	if(!(TimeIncrease * TICK_CYCLE % 1000))	// 1s
	{		
		fan_move_flag = 1;
	}
	/*if(!(TimeIncrease * TICK_CYCLE % 100))	// 0.1s
	{		
		fan_disp_speed_flag = 1;
	}*/
	//sean 19.1.12
	if(!(TimeIncrease * TICK_CYCLE % 1000))	// 1s
	{		
		per_second_save_sd=1;
	}
	

	//if(!(TimeIncrease * TICK_CYCLE % 10)) //10ms
	//	GUI_TOUCH_Exec();

	TimeIncrease++;

	mksUsart2RepeatTx();
 	
	
	////////send2Printer();
	//////////	rxPrinter();
	

	if(disp_state == MAIN_UI)
	{
		if(logo_time * TICK_CYCLE < 3000 )	// 3s
		{	
			logo_time++;
			//get_cur_temp();
		}
	}


	if(((TimeIncrease * TICK_CYCLE % 1000) == 0) )
	{		
		link_mutex_detect_time++;
		if(print_time.start == 1)
		{
			print_time.seconds++;
		}
	}

	//开始打印10s 后将标志位置0
	if(((TimeIncrease * TICK_CYCLE % 10000) == 0) )
	{		
		if(empty_filament_tips_flag ==1)
		{
			empty_filament_tips_flag=0;
		}
	}
	
	
    #endif
#if debug_flg	
	if(T_CNT != 0) 
	{
		T_CNT--;
	}
#endif	
}
#if debug_flg
//测试输出口PB0/PB1初始化。
void T_GPIO_init()
{	
	GPIO_InitTypeDef GPIO_InitStructure_T;
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	 //PORTB时钟使能 
  	GPIO_InitStructure_T.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;//PB0 PB1
	GPIO_InitStructure_T.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure_T.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure_T);

	/*FOR SD DET*/
	GPIO_InitStructure_T.GPIO_Pin = GPIO_Pin_15;//PB15
	GPIO_InitStructure_T.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure_T);
}
/*****************************************/
//x_cnt:时间。
//sel_type:表示两种类型的输出，1:表示用于while循环测试
//0:表示用于硬件复位中断的测试。
/*****************************************/
void T_function(uint32_t x_cnt,uint8_t sel_type)
{
	if(sel_type == 1)
	{
		if(T_CNT == 0)
		{
			T_CNT = x_cnt;
			if(T_FLG%2)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_0);
				//PBout(0) = 1;
				T_FLG = 0;
			}
			else
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_0);
				//PBout(0) = 0;
				T_FLG = 1;
			}
			
		}
	}
	else
	{
		while(x_cnt--)
		{
			if(T_FLG%2)
			{
				GPIO_SetBits(GPIOB,GPIO_Pin_1);
				T_FLG = 0;
			}
			else
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_1);
				T_FLG = 1;
			}
		}
	}
}

#endif


#if 1
void Close_machine_display()
{
	clear_cur_ui();
	disp_state = PRINT_READY_UI;
	GUI_SetBkColor(gCfgItems.background_color);
	GUI_SetColor(gCfgItems.title_color);
	GUI_Clear();
	GUI_DispStringAt(common_menu.close_machine_tips, 80, 120);
	Btn_putdown_close_machine();
	close_fail_flg = 1;
	close_fail_cnt = 0;
	while(close_fail_flg);
	clear_cur_ui();
	//draw_ready_print();
	draw_dialog(DIALOG_TYPE_M80_FAIL);

}


//断电断料检测
//断电:PB0断电检测，可以作为第二喷头的断料检测接口。
//断料:PB1断料检测，
//关机模块检测管脚:
//PB4，可以通过配置文件来选择接220DET模块或接PWC模块;
//默认接PWC模块。
void PowerOff_Filament_Check()
{
	volatile uint8_t i;

	//断电检测
	if(gCfgItems.insert_det_module == 1)//接220det模块，断电检测
	{
		if(printerStaus == pr_working)//打印中则进入暂停界面
		{
			if(DT_PORT_READ== Bit_RESET)
			{
				poweroff_det_flg = 1;
				if(poweroff_det_cnt >= 1000)
				{
					if((DT_PORT_READ==Bit_RESET)&&(gCfgItems.have_ups==1))
					{
						poweroff_det_flg = 0;
						poweroff_det_cnt= 0;
						
						I2C_EE_Init(100000);
						clear_cur_ui();
						printerStaus = pr_pause;
						draw_pause();
						mksBpAlrmEn = 1;

						return;				
					}

					poweroff_det_flg = 0;
					poweroff_det_cnt= 0;

					Close_machine_display();

					return;
				}
			}
			else
			{
				poweroff_det_flg = 0;
				poweroff_det_cnt= 0;
			}
		}		
	}
	else//接PWC关机模块
	{
		if(DT_PORT_READ == Bit_RESET)//
		{
			poweroff_det_low_flg = 1;//启动计时
		}
		
		if(poweroff_det_low_cnt >= 80)// 1s跳变为高电平
		{
			if(DT_PORT_READ == Bit_SET)
			{
				poweroff_det_low_flg = 0;
				poweroff_det_low_cnt = 0;
				poweroff_det_flg=1;
			}
			else
			{
				poweroff_det_low_flg = 0;
				poweroff_det_low_cnt = 0;
			}
		}
		
		if(poweroff_det_flg==1)
		{
			poweroff_det_high_flg = 1;
			if(poweroff_det_high_cnt >= 80)
			{
				if(DT_PORT_READ == Bit_SET)
				{
					if((printerStaus== pr_working)&&(gCfgItems.have_ups==1))//打印中且有UPS后备电源，则进入暂停界面
					{
						poweroff_det_high_flg = 0;
						poweroff_det_high_cnt = 0;
						poweroff_det_flg=0;

						I2C_EE_Init(100000);
						clear_cur_ui();
						printerStaus = pr_pause;
						draw_pause();
						mksBpAlrmEn = 1;

						return;
					}
					//没UPS电源或没在打印，直接关机。
					poweroff_det_high_flg = 0;
					poweroff_det_high_cnt = 0;
					poweroff_det_flg=0;
					Close_machine_display();
					return;					
				}
				else
				{
					poweroff_det_low_flg = 0;
					poweroff_det_low_cnt = 0;
					poweroff_det_high_flg = 0;
					poweroff_det_high_cnt = 0;
					poweroff_det_flg = 0;
				}
			}
		}
	}
	//断电检测
	if(printerStaus == pr_working)//打印中则进入暂停界面
	{
		if(gCfgItems.disable_pwr_det == 0)
		{
		if(gCfgItems.filament_det2_level_flg == 1)//断料接口接入电平为高电平触发时的处理
		{
			//高电平触发时，是检测2s低电平和2s高电平
      if(PW_PORT_READ == Bit_RESET)
			{
				filament_det2_low_flg = 1;//启动计时
			}
			
			if(filament_det2_low_cnt >= 2000)// 2s
			{
        if(PW_PORT_READ == Bit_SET)
				{
					filament_det2_high_flg = 1;
					filament_det2_check=1;

					filament_det2_low_flg = 0;
					filament_det2_low_cnt = 0;
				}
				else
				{
					filament_det2_low_flg = 0;
					filament_det2_low_cnt = 0;
				}
			}
				
			if(filament_det2_check ==1)
			{
				if(filament_det2_high_cnt >= 2000)
				{
					if(PW_PORT_READ == Bit_SET)
					{
						filament_det2_high_flg= 0;
						filament_det2_high_cnt= 0;
						filament_det2_check=0;
						
						I2C_EE_Init(100000);
						clear_cur_ui();
						printerStaus = pr_pause;
						//chen 9.10
						if(empty_filament_tips_flag ==1)
						{
							draw_dialog(DIALOG_EMPTY_FILAMENT_TIPS);
						}
						else
						{
							draw_pause();
						}
						pause_from_pw_high_level=1;
						mksBpAlrmEn = 1;

						return;		
					}
					else
					{
						filament_det2_low_flg = 0;
						filament_det2_low_cnt = 0;
						filament_det2_high_flg= 0;
						filament_det2_high_cnt= 0;
						filament_det2_check=0;
					}
				}
			}
		}
		else//断料接口接入电平为低电平触发时处理
		{
			if(PW_PORT_READ == Bit_RESET)
			{
				filament_det2_flg = 1;
				if(filament_det2_cnt >= 1000)
				{
					if(PW_PORT_READ == Bit_RESET)
					{
						filament_det2_flg = 0;
						filament_det2_cnt= 0;

						I2C_EE_Init(100000);
						clear_cur_ui();
						printerStaus = pr_pause;
						//chen 9.10
						if(empty_filament_tips_flag ==1)
						{
							draw_dialog(DIALOG_EMPTY_FILAMENT_TIPS);
						}
						else
						{
							draw_pause();
						}
						mksBpAlrmEn = 1;

						return;				
					}
				}
			}
			else
			{
				filament_det1_flg = 0;
				//HC-chen 7.30
				filament_det2_cnt= 0;
			}
		}
		}
	}	
	//断料检测
	if(printerStaus == pr_working)//打印中则进入暂停界面
	{
		if(gCfgItems.disable_mt_det ==0)
		{
		if(gCfgItems.filament_det1_level_flg == 1)//断料接口接入电平为高电平触发时的处理
		{
			//高电平触发时，是检测2s低电平和2s高电平
      if(MT_PORT_READ == Bit_RESET)
			{
				filament_det1_low_flg = 1;//启动计时
			}
			
			if(filament_det1_low_cnt >= 2000)// 2s
			{
        if(MT_PORT_READ == Bit_SET)
				{
					filament_det1_high_flg = 1;
					filament_det1_check=1;

					filament_det1_low_flg = 0;
					filament_det1_low_cnt = 0;
				}
				else
				{
					filament_det1_low_flg = 0;
					filament_det1_low_cnt = 0;
				}
			}
				
			if(filament_det1_check ==1)
			{
				if(filament_det1_high_cnt >= 2000)
				{
					if(MT_PORT_READ == Bit_SET)
					{
						filament_det1_high_flg= 0;
						filament_det1_high_cnt= 0;
						filament_det1_check=0;
						
						I2C_EE_Init(100000);
						clear_cur_ui();
						printerStaus = pr_pause;
						//chen 9.10
						if(empty_filament_tips_flag ==1)
						{
							draw_dialog(DIALOG_EMPTY_FILAMENT_TIPS);
						}
						else
						{
							draw_pause();
						}
						pause_from_mt_high_level=1;
						mksBpAlrmEn = 1;

						return;		
					}
					else
					{
						filament_det1_low_flg = 0;
						filament_det1_low_cnt = 0;
						filament_det1_high_flg= 0;
						filament_det1_high_cnt= 0;
						filament_det1_check=0;
					}
				}
			}
		}
		else//断料接口接入电平为低电平触发时处理
		{
			if(MT_PORT_READ == Bit_RESET)
			{
				filament_det1_flg = 1;
				if(filament_det1_cnt >= 1000)
				{
					if(MT_PORT_READ == Bit_RESET)
					{
						filament_det1_flg = 0;
						filament_det1_cnt= 0;

						I2C_EE_Init(100000);
						clear_cur_ui();
						printerStaus = pr_pause;
						//chen 9.10
						if(empty_filament_tips_flag ==1)
						{
							draw_dialog(DIALOG_EMPTY_FILAMENT_TIPS);
						}
						else
						{
							draw_pause();
						}
						mksBpAlrmEn = 1;

						return;				
					}
				}
			}
			else
			{
				filament_det1_flg = 0;
				//HC-chen 7.30
				filament_det1_cnt= 0;
			}
		}
		}
	}
}
#endif

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
