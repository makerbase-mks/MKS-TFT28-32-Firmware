#include "stm32f10x.h"
#include "ff.h"
#include "diskio.h"
#include "printer.h"
#include "SZ_STM32F107VC_LIB.h"
#include "sd_usr.h"
#include "SPI_FLASH.h"

#include "draw_ui.h"

#include "wifi_module.h"

#ifndef __cplusplus
typedef enum {FALSE = 0, TRUE = !FALSE} bool;
#endif

/* ------------------file------------------------ */
extern FATFS fs;            // Work area (file system object) for logical drive
FIL fsrc, fdst;      // file objects
BYTE buffer[51];     // file copy buffer
FRESULT res;         // FatFs function common result code
UINT br, bw;         // File R/W count

extern int8_t  gCurDir[100];
extern fileNameList gcodeFileList;

/* Private typedef -----------------------------------------------------------*/
//typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
//TestStatus Buffercmp(u8* pBuffer1, u8* pBuffer2, u16 BufferLength);
void ShowSDFiles(void);
bool  File_type_Check( u8 * FileName, u8 * filter );
void ListSDFiles(char *path);
void ShowHelp(void);
void BufferSet(u8* pBuffer, u8 data, u16 BufferLength);

uint32_t Sd_file_offset = 0;
uint32_t Sd_file_cnt = 0;
//uint32_t Sd_display_file_cnt = 0;

#define MAX_FILES_PER_LEVER	5


uint8_t Explore_Disk (char* path , uint8_t recu_level)
{

  FILINFO fno;
  DIR dir;
  SD_CardInfo cardinfo;
  char *fn;
  char tmp[200];
  char Fstream[200];
	int local_offset;
	int file_offset = 0;
	
  #if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

	if(path == 0)
		return 0;

	if(path[0] == '0')
		f_mount(0, &fs);
	else if(path[0] == '1')
		f_mount(1, &fs);
	else
		return;
	
	for(;;)
	{
		local_offset = 0;
		
		if (f_opendir(&dir, path) == FR_OK) 
	  	{

		    while(1)
		    {
				res = f_readdir(&dir, &fno);
				if (res != FR_OK || fno.fname[0] == 0) 
				{
					return;
				}
				if (fno.fname[0] == '.')
				{
					continue;
				}

				


				if(local_offset >= file_offset)
				{
					file_offset++;
					break;
				}

				local_offset++;
		    }
			
		    if ((fno.lfname[0] == 0) || (fno.lfname == 0))
				fn = fno.fname;
			else
				fn = fno.lfname;
				
		      	if((strstr(fn, ".g")) || (strstr(fn, ".G")) || (fno.fattrib & AM_DIR))
			{
					  
					tmp[0] = '\0';
					if(gCfgItems.wifi_type == HLK_WIFI)
					{
						strcpy(tmp, path);
						strcat(tmp, "/");
						strcat(tmp, fn);
					}
					else
						strcpy(tmp, fn);
					
					memset(Fstream, 0, sizeof(Fstream));
					strcpy(Fstream, tmp);
					if((fno.fattrib & AM_DIR)&&(recu_level <= 10))
				      {
				      //  Explore_Disk(tmp, recu_level + 1);
						
						strcat(Fstream, ".DIR\n");

					  	send_to_wifi(Fstream, strlen(Fstream));
					
				      }
					else
					{					
						strcat(Fstream, "\n");
						send_to_wifi(Fstream, strlen(Fstream));
						
					}
				}
		  
		      

		     
		    }
				else
					break;
	
  	
	}
 return res;
}



void ShowSDFiles(void)
{
  FILINFO fno;
  DIR dir;
  SD_CardInfo cardinfo;
  char *fn;
  char tmp[200];

  #if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif
 

 f_mount(1, &fs);
 if (f_opendir(&dir, gCurDir) == FR_OK) 
  {
  	Sd_file_cnt = 0;
	gcodeFileList.listVaild= 2;
	for (;;) 
	{
		res = f_readdir(&dir, &fno);
		if (res != FR_OK || fno.fname[0] == 0) 
		{
			gcodeFileList.listVaild= 2;
			break;
		}
		if ( fno.fname[0] == '.') 
			continue;		
		if ( fno.lfname[0] == '.') 
			continue;

		if ((fno.lfname[0] == 0) || (fno.lfname == 0))
			fn = fno.fname;
		else
			fn = fno.lfname;

		/*   if (fno.fattrib & AM_DIR) 
		{
		continue;
		} 
		else */
		if(Sd_file_cnt == Sd_file_offset)
		{
			//Sd_file_offset++;
			if((strstr(fn, ".g")) || (strstr(fn, ".G")) || (fno.fattrib & AM_DIR))
			{
				  //Sd_display_file_cnt++;
				  
				  tmp[0] = '\0';
				  strcpy(tmp, gCurDir);
				  strcat(tmp, "/");
				  strcat(tmp, fn);

				  gcodeFileList.listVaild= 1;

				if(fno.fattrib & AM_DIR)
				{
					gcodeFileList.fileAttr[gcodeFileList.index] = 1;
				}
				else
				{
					gcodeFileList.fileAttr[gcodeFileList.index] = 0;
				}

				strcpy(gcodeFileList.fileName[gcodeFileList.index], tmp);
				gcodeFileList.index++;
				
				
			}
			else
			{
				gcodeFileList.listVaild= 0;
			}
			break;
		}
		
		Sd_file_cnt++;
	}

#if 0
    while (f_readdir(&dirs, &finfo) == FR_OK)  
    {
      if (finfo.fattrib & AM_ARC) 
      {
        if(!finfo.fname[0])	
          break;         
          printf("\n\r file name is: %s\n",finfo.fname);
          printf("\n\r file size is: %d ", finfo.fsize); 

          if(File_type_Check( (u8 *)finfo.fname, "txt"))
          { 
            BufferSet(buffer, 0, 100);
            res = f_open(&fsrc, finfo.fname, FA_OPEN_EXISTING | FA_READ);
            res = f_read(&fsrc, buffer, 100, &br);
            printf("\n\r file contex is: \n\r%s\n\r", buffer); 
            f_close(&fsrc);			                      
          }
      }
      else
      {
        printf("\n\r Path name is: %s", finfo.fname); 
        continue;//break;
      }
    } 
      res = f_open(&fsrc, "armjishu.txt", FA_CREATE_ALWAYS | FA_WRITE);
      res = f_write(&fsrc, &armjishu, sizeof(armjishu), &bw);
      f_close(&fsrc);
	#endif
   
 }
 else
	 gcodeFileList.listVaild= 2;
 
}
#if 0
static FIL curFile;
char tmpFileBuf[10];
void write_pic_data()
{

	FILINFO fno;
	DIR dir;
	//char path[100]={"1:"};  
	SD_CardInfo cardinfo;
	char *fn;
	char tmp[30];
	unsigned int readByteCnt;
	
	FRESULT res;

		#if _USE_LFN
	static char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
#endif

	strcpy(gCurDir, "1:/mks_pic");
	f_mount(1, &fs);
	if (f_opendir(&dir, gCurDir) == FR_OK) 
	{

		for (;;) 
		{
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) 
			{
			//gcodeFileList.listVaild= 1;
			break;
			}
			if ( fno.fname[0] == '.') 
				continue;

			if ((fno.lfname[0] == 0) || (fno.lfname == 0))
				fn = fno.fname;
			else
				fn = fno.lfname;

			if (fno.fattrib & AM_DIR) 
			{
				continue;
			} 
			else 
			{
				if((strstr(fn, "bmp_logo.bin")) )
				{
					  tmp[0] = '\0';
					strcpy(tmp, gCurDir);
					strcat(tmp, "/");
					strcat(tmp, fn);

					gcodeFileList.listVaild= 1;

					res = f_open(&curFile, tmp, FA_OPEN_EXISTING | FA_READ);
					if(res == FR_OK)
					{
						int frame;
						for(frame = 0; frame < 150 * 1024/ sizeof(tmpFileBuf); frame++)
						{
							f_read(&curFile,tmpFileBuf,sizeof(tmpFileBuf),&readByteCnt);
							Pic_Logo_Write(fn, tmpFileBuf, readByteCnt);	
						}
					}
					f_close(&curFile);

										
				  
				}
			}
		}
	}
}
#endif

int ascii2dec(char *ascii, char width)
{
	int i = 0;
	int result = 0;

	if(ascii == 0)
		return 0;
	
	while(i < width)
	{		
		result = result << 4;
		
		if(*(ascii + i) >= '0' && *(ascii + i) <= '9')
			result += *(ascii + i) - '0';
		else if(*(ascii + i) >= 'a' && *(ascii + i) <= 'f')
			result += *(ascii + i) - 'a' + 0x0a;
		else if(*(ascii + i) >= 'A' && *(ascii + i) <= 'F')
			result += *(ascii + i) - 'A' + 0x0a;
		else
			return 0;
		
		i++;
	}
	return result;
}

char *cfg_file = "1:/mks_config.txt";
char *cfg_file_cur = "1:/CONFIG.CUR";

char cmd_code[201];

extern uint8_t  DecStr2Float(int8_t * buf,  float  *result);

void get_sd_cfg()
{
	uint32_t i;
	uint8_t temp;

	__IO long file_pos=0; 
	__IO uint16_t sd_read_cnt=0;
	
	unsigned char code_cnt = 0;
	
	FRESULT r;
	FIL cfgFile;
	FIL saveFile;
	
	//char file_str[5120];//读取文件最大为4k。
	char file_str[512];//读取文件最大为1k。
	int byte_have_read;
	
	volatile char *tmp_index;
	uint32_t flash_inf_valid_flg;
	
	uint32_t file_read_length = 0;
	uint32_t flg_length = 0;
	//uint32_t valid_length = 0;
	char *tmp1,*tmp2;
	//char invalid_char1[1024]={0};
	//char invalid_char2[1024]={0};
	
	f_mount(1, &fs);
	
	if (f_open(&cfgFile, cfg_file, FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		
		
		//擦除4k
		SPI_FLASH_SectorErase(SD_INF_ADDR);
		//while(1)
		while(cfgFile.fsize > file_pos)
		{
		/*
			memset(file_str, 0, sizeof(file_str));
			memset(invalid_char1, 0, sizeof(invalid_char1));
			memset(invalid_char2, 0, sizeof(invalid_char2));
			f_lseek(&cfgFile, file_read_length);
			//r = f_read(&cfgFile, file_str, sizeof(file_str), &byte_have_read);
			r = f_read(&cfgFile, file_str, 1024, &byte_have_read);
			if(file_str[byte_have_read-1] == '\n')
			{
				file_read_length += byte_have_read;
			}
			else
			{
				tmp1 = (char *)strrchr(&file_str[0],'>');
				if(tmp1)
				{
					strcpy(invalid_char1,tmp1);
					tmp2 = (char *)strrchr(invalid_char1,'\n');
					if(tmp2)
					{
						strcpy(invalid_char2,tmp2);
						flg_length = strlen(invalid_char2);
					}
					else
					{
						flg_length = strlen(invalid_char1);
					}
				}
				file_read_length += (byte_have_read - flg_length);
			}		
			*/
			memset(file_str, 0, sizeof(file_str));
			sd_read_cnt = 0;
			do
			{
				r = f_read(&cfgFile, &file_str[sd_read_cnt], 1, &byte_have_read);
				sd_read_cnt++;
				file_pos++;
				if(cfgFile.fsize <= file_pos)
				{
					break;
				}
			}
			while((file_str[sd_read_cnt-1] != '\r')&&(file_str[sd_read_cnt-1] != '\n'));
			file_str[sd_read_cnt] = '\n';
			//if(r == FR_OK)
			{
				
					/*picture custom flag*/
					/*tmp_index = (char *)strstr(file_str, "picture_custom_flag:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen("picture_custom_flag:")] == '0')
							gCfgItems.custom_pic_flag = 0;
						else
							gCfgItems.custom_pic_flag = 1;
					}*/

					/*background_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_background_color:0");
					if(tmp_index != 0)
					{
						tmp_index += strlen(">cfg_background_color:0x");
						
						gCfgItems.background_color = ascii2dec(tmp_index, 6);

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.background_color, BAK_BACKGROUND_COLOR_ADD,4);
					}

					/*title_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_title_color:0");
					if(tmp_index != 0)
					{
						tmp_index += strlen(">cfg_title_color:0x");
						
						gCfgItems.title_color = ascii2dec(tmp_index, 6);

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.title_color, BAK_TITIL_COLOR_ADD,4);
					}

					/*state_background_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_state_background_color:0");
					if(tmp_index != 0)
					{
						tmp_index += strlen(">cfg_state_background_color:0x");
						
						gCfgItems.state_background_color = ascii2dec(tmp_index, 6);

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.state_background_color, BAK_STATE_BACKGROUND_COLOR_ADD,4);
					}

					/*state_text_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_state_text_color:0");
					if(tmp_index != 0)
					{
						tmp_index += strlen(">cfg_state_text_color:0x");
						
						gCfgItems.state_text_color = ascii2dec(tmp_index, 6);

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.state_text_color, BAK_STATE_TEXT_COLOR_ADD,4);
					}

					/*filename_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_filename_color:0");
					if(tmp_index != 0)
					{
						tmp_index += strlen(">cfg_filename_color:0x");
						
						gCfgItems.filename_color = ascii2dec(tmp_index, 6);

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filename_color, BAK_FILENAME_COLOR_ADD,4);
					}

					/*filename_background_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_filename_background_color:0");
					if(tmp_index != 0)
					{
						tmp_index += strlen(">cfg_filename_background_color:0x");
						
						gCfgItems.filename_background_color = ascii2dec(tmp_index, 6);
						
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filename_background_color, BAK_FILENAME_BACKGROUND_COLOR_ADD,4);
					}
					/*printingstate_words_background_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_printingstat_word_background_color:0");
					if(tmp_index != 0)
					{
						tmp_index += strlen(">cfg_printingstat_word_background_color:0x");
						
						gCfgItems.printingstate_word_background_color = ascii2dec(tmp_index, 6);

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.printingstate_word_background_color, BAK_PRINTINGSTATE_WORD_BACKGROUND_COLOR_ADD,4);
					}
					/*printingstate_background_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_printingstat_word_color:0");
					if(tmp_index != 0)
					{
						tmp_index += strlen(">cfg_printingstat_word_color:0x");
						
						gCfgItems.printingstate_word_color = ascii2dec(tmp_index, 6);

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.printingstate_word_color, BAK_PRINTINGSTATE_WORD_COLOR_ADD,4);
					}
					/*定制按钮3d显示效果*/
					tmp_index = (char *)strstr(file_str,">cfg_BUTTON_3D_effects:");
					if(tmp_index != 0)
					{
						if((tmp_index[strlen(">cfg_BUTTON_3D_effects:")]) == '1')
						{
							gCfgItems.button_3d_effect_flag= 1;
						}
						else
						{
							gCfgItems.button_3d_effect_flag = 0;
						}

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.button_3d_effect_flag, BAK_BUTTON_3D_EFFECT_FLAG,1);
					}
					

					
					/*picture counters*/
					tmp_index = (char *)strstr(file_str, ">moreitem_pic_cnt:");
					if(tmp_index != 0)
					{
						switch(tmp_index[strlen(">moreitem_pic_cnt:")])
						{
						case '1':
							gCfgItems.MoreItem_pic_cnt = 1;
							break;
						case '2':
							gCfgItems.MoreItem_pic_cnt = 2;
							break;
						case '3':
							gCfgItems.MoreItem_pic_cnt = 3;
							break;
						case '4':
							gCfgItems.MoreItem_pic_cnt = 4;
							break;
						case '5':
							gCfgItems.MoreItem_pic_cnt = 5;
							break;
						case '6':
							gCfgItems.MoreItem_pic_cnt = 6;
							break;	
						case '7':
							gCfgItems.MoreItem_pic_cnt = 7;
							break;		
						default:
							gCfgItems.MoreItem_pic_cnt = 0;
							break;
						}
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.MoreItem_pic_cnt, BAK_MOREITEM_PIC_CNT,1);
					}
					
					//读button1命令
					tmp_index = (char *)strstr(file_str, ">moreitem_button1_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">moreitem_button1_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}
						}
						code_cnt = 0;
						//擦除4k
						//SPI_FLASH_SectorErase(SD_INF_ADDR);
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_CMD1_ADDR,(strlen(cmd_code)+1));
						
					}
					//读button2命令
					tmp_index = (char *)strstr(file_str, ">moreitem_button2_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">moreitem_button2_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_CMD2_ADDR,(strlen(cmd_code)+1));					
					}
					//读button3命令
					tmp_index = (char *)strstr(file_str, ">moreitem_button3_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">moreitem_button3_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_CMD3_ADDR,(strlen(cmd_code)+1));					
					}
					//读button4命令
					tmp_index = (char *)strstr(file_str, ">moreitem_button4_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">moreitem_button4_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_CMD4_ADDR,(strlen(cmd_code)+1));					
					}	
					//读button5命令
					tmp_index = (char *)strstr(file_str, ">moreitem_button5_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">moreitem_button5_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_CMD5_ADDR,(strlen(cmd_code)+1));					
					}	
					//读button6命令
					tmp_index = (char *)strstr(file_str, ">moreitem_button6_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">moreitem_button6_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_CMD6_ADDR,(strlen(cmd_code)+1));					
					}	
					//读button7命令
					tmp_index = (char *)strstr(file_str, ">moreitem_button7_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">moreitem_button7_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_CMD7_ADDR,(strlen(cmd_code)+1));					
					}
					//wifi模块: 0:ESP, 1:HLK
					tmp_index = (char *)strstr(file_str,">cfg_wifi_type:");
					if(tmp_index != 0)
					{
						if( tmp_index[strlen(">cfg_wifi_type:")] == '1')
						{
							gCfgItems.wifi_type = 0x1;	
						}
						else
						{
							gCfgItems.wifi_type = 0x2;
						}		

						SPI_FLASH_BufferWrite((uint8_t *)&gCfgItems.wifi_type,WIFI_MODE_TYPE_ADDR,1);
					}
					//wifi模式选择
					tmp_index = (char *)strstr(file_str,">cfg_wifi_mode:");
					if(tmp_index != 0)
					{
						if( tmp_index[strlen(">cfg_wifi_mode:")] == '1')
						{
							gCfgItems.wifi_mode_sel = 1;	
						}
						else
						{
							gCfgItems.wifi_mode_sel = 2;
						}		
						cfg_wifi_flag = 1;

						SPI_FLASH_BufferWrite((uint8_t *)&gCfgItems.wifi_mode_sel,WIFI_MODE_SEL_ADDR,1);
					}
					//读WIFI名称
					tmp_index = (char *)strstr(file_str, ">cfg_wifi_ap_name:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_wifi_ap_name:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 32)
							{
								break;
							}						
						}
						code_cnt = 0;

						strncpy((char *)gCfgItems.wifi_ap, cmd_code, 32);

						cfg_wifi_flag = 1;
						
						SPI_FLASH_BufferWrite(cmd_code,WIFI_NAME_ADDR,(strlen(cmd_code)+1));
						
					}
					//WIFI密码
					tmp_index = (char *)strstr(file_str, ">cfg_wifi_key_code:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_wifi_key_code:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 64)
							{
								break;
							}						
						}
						code_cnt = 0;

						strncpy((char *)gCfgItems.wifi_key, cmd_code, 64);

						cfg_wifi_flag = 1;
						
						SPI_FLASH_BufferWrite(cmd_code,WIFI_KEYCODE_ADDR,(strlen(cmd_code)+1));	
					}
					/*cloud service*/
					tmp_index = (char *)strstr(file_str, ">cfg_cloud_enable:");
					if(tmp_index != 0)
					{
						if( tmp_index[strlen(">cfg_cloud_enable:")] == '1')
						{
							gCfgItems.cloud_enable = 1;	
						}
						else
						{
							gCfgItems.cloud_enable = 0;
						}	
						cfg_cloud_flag = 1;
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.cloud_enable,BAK_CLOUD_FLAG_ADDR,1);
					}
					tmp_index = (char *)strstr(file_str, ">cfg_wifi_cloud_host:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_wifi_cloud_host:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 96)
							{
								break;
							}						
						}
						code_cnt = 0;
						strncpy((char *)gCfgItems.cloud_hostUrl, cmd_code, 96);
						cfg_cloud_flag = 1;
					}		
					tmp_index = (char *)strstr(file_str, ">cfg_cloud_port:");
					if(tmp_index != 0)
					{
						if( (strcmp((const char *)(&tmp_index[strlen(">cfg_cloud_port:")]), "0") > 0) &&(strcmp((const char *)(&tmp_index[strlen(">cfg_cloud_port:")]), "99999") < 0))
						{
							gCfgItems.cloud_port = atoi((char *)(&tmp_index[strlen(">cfg_cloud_port:")]));
						}
						else					
							gCfgItems.cloud_port = 10086;
						cfg_cloud_flag = 1;
					}		
					//WIFI IP地址
					tmp_index = (char *)strstr(file_str, ">cfg_ip_address:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_ip_address:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 15)
							{
								break;
							}						
						}
						code_cnt = 0;
						
						SPI_FLASH_BufferWrite(cmd_code,WIFI_IP_ADDR,(strlen(cmd_code)+1));	
					}	
					//WIFI IP掩码
					tmp_index = (char *)strstr(file_str, ">cfg_ip_mask:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_ip_mask:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 15)
							{
								break;
							}						
						}
						code_cnt = 0;
						
						SPI_FLASH_BufferWrite(cmd_code,WIFI_MASK_ADDR,(strlen(cmd_code)+1));
					}	
					//WIFI IP网关
					tmp_index = (char *)strstr(file_str, ">cfg_ip_gate:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_ip_gate:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 15)
							{
								break;
							}						
						}
						code_cnt = 0;
						
						SPI_FLASH_BufferWrite(cmd_code,WIFI_GATE_ADDR,(strlen(cmd_code)+1));		
					}

					//IP_DHCP_FLAG 
					tmp_index = (char *)strstr(file_str, ">cfg_ip_dhcp_flag:");
					if(tmp_index != 0)
					{
						
						if( tmp_index[strlen(">cfg_ip_dhcp_flag:")] == '1')
						{
							ipPara.dhcp_flag = 1;	
						}
						else
						{
							ipPara.dhcp_flag = 0;
						}
						SPI_FLASH_BufferWrite((uint8_t *)&ipPara.dhcp_flag,WIFI_DHCP_FLAG_ADDR,1);		
						//flash存储标志位。
						flash_inf_valid_flg = FLASH_INF_VALID_FLAG;
						SPI_FLASH_BufferWrite((uint8_t*)&flash_inf_valid_flg,FlASH_INF_VALID_ADDR,4);	
						
					}	

					//喷头数量
					tmp_index = (char *)strstr(file_str, ">cfg_sprayer_counter:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen(">cfg_sprayer_counter:")] == '2')
						{
							gCfgItems.sprayerNum = 2;
						}
						else
						{
							gCfgItems.sprayerNum = 1;
						}

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.sprayerNum,BAK_SPRAYER_NUM_ADDR,1);
						
					}
					//定制热床
					tmp_index = (char *)strstr(file_str, ">cfg_custom_bed_flag:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen(">cfg_custom_bed_flag:")] == '1')
						{
							gCfgItems.custom_bed_flag = 1;
						}
						else
						{
							gCfgItems.custom_bed_flag = 0;
						}

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.custom_bed_flag,BAK_CUSTOM_BED_FLAG_ADDR,1);
						
					}
					//主板固件类型配置
					tmp_index = (char *)strstr(file_str, ">cfg_firmware_type:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen(">cfg_firmware_type:")] == '1')
						{
							gCfgItems.firmware_type = 1;
						}
						else if(tmp_index[strlen(">cfg_firmware_type:")] == '2')
						{
							gCfgItems.firmware_type = 2;
						}
						else if(tmp_index[strlen(">cfg_firmware_type:")] == '3')
						{
							gCfgItems.firmware_type = 3;
						}
						else
						{
							gCfgItems.firmware_type = 1;
						}

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.firmware_type,BAK_FIRMWARETYPE_ADDR,1);
						
					}		
					//通信波特率
					tmp_index = (char *)strstr(file_str, ">cfg_baud_rate:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen(">cfg_baud_rate:")] == '1')//9600
						{
							gCfgItems.baud = 1;
						}
						else if(tmp_index[strlen(">cfg_baud_rate:")] == '2')//57600
						{
							gCfgItems.baud = 2;
						}
						else if(tmp_index[strlen(">cfg_baud_rate:")] == '3')//115200
						{
							gCfgItems.baud = 3;
						}
						else if(tmp_index[strlen(">cfg_baud_rate:")] == '4')//250000
						{
							gCfgItems.baud = 4;
						}
						else 
						{
							gCfgItems.baud = 3;
						}

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.baud,BAK_BAUDRATE_ADDR,1);
						
					}		

					//X/Y/Z轴位置值。
					tmp_index = (char *)strstr(file_str, ">cfg_XPOS:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_XPOS:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt > 10)
							{
								break;
							}						
						}
						code_cnt = 0;
						gCfgItems.Pause_XPOS = atoi(cmd_code);

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.Pause_XPOS,BAK_PAUSE_XPOS_ADDR,2);
						
					}						

					tmp_index = (char *)strstr(file_str, ">cfg_YPOS:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_YPOS:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt > 10)
							{
								break;
							}						
						}
						code_cnt = 0;
						gCfgItems.Pause_YPOS = atoi(cmd_code);

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.Pause_YPOS,BAK_PAUSE_YPOS_ADDR,2);
						
					}

					tmp_index = (char *)strstr(file_str, ">cfg_ZADD:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_ZADD:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt > 10)
							{
								break;
							}						
						}
						code_cnt = 0;
						gCfgItems.Pause_ZADD= atoi(cmd_code);

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.Pause_ZADD,BAK_PAUSE_ZADD_ADDR,2);
						
					}

					//机型
					tmp_index = (char *)strstr(file_str, ">cfg_machine_type:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen(">cfg_machine_type:")] == '1')
						{
							gCfgItems.machineType= 1;
						}
						else if(tmp_index[strlen(">cfg_machine_type:")] == '2')
						{
							gCfgItems.machineType = 2;
						}
						else
						{
							gCfgItems.machineType = 1;
						}

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.machineType,BAK_MACHINE_TYPE_ADDR,1);		
					}	

					//语言
					tmp_index = (char *)strstr(file_str, ">cfg_language_type:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen(">cfg_language_type:")] == '1')
						{
							gCfgItems.language_bak= LANG_SIMPLE_CHINESE;
						}
						else if(tmp_index[strlen(">cfg_language_type:")] == '2')
						{
							gCfgItems.language_bak = LANG_COMPLEX_CHINESE;
						}
						else if(tmp_index[strlen(">cfg_language_type:")] == '3')
						{
							gCfgItems.language_bak = LANG_ENGLISH;
						}
						/*
						else if(tmp_index[strlen(">cfg_language_type:")] == '4')
						{
							gCfgItems.language_bak = LANG_JAPAN;
						}
						*/
						else if(tmp_index[strlen(">cfg_language_type:")] == '4')
						{
							gCfgItems.language_bak = LANG_RUSSIAN;
						}
						else if(tmp_index[strlen(">cfg_language_type:")] == '5')
						{
							gCfgItems.language_bak = LANG_SPANISH;
						}						
						else
						{
							gCfgItems.language_bak = LANG_ENGLISH;
						}

						//I2C_EE_BufferWrite((uint8_t *)&gCfgItems.language_bak,BAK_LANGUARY_ADDR,1);		
					}

					//功能键显示标志
					tmp_index = (char *)strstr(file_str, ">cfg_function_btn1_display:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen(">cfg_function_btn1_display:")] == '0')
						{
							gCfgItems.func_btn1_display_flag = 0;
						}
						else if(tmp_index[strlen(">cfg_function_btn1_display:")] == '1')
						{
							gCfgItems.func_btn1_display_flag = 1;
						}
						else
						{
							gCfgItems.func_btn1_display_flag = 1;
						}

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.func_btn1_display_flag,BAK_FUNC1_DISPLAY_ADDR,1);		
					}

					//读功能键命令
					tmp_index = (char *)strstr(file_str, ">function_btn1_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">function_btn1_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_FUNCTION1_ADDR,(strlen(cmd_code)+1));					
					}
					#if 0
					//功能键显示标志
					tmp_index = (char *)strstr(file_str, ">cfg_function_btn2_display:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen(">cfg_function_btn2_display:")] == '0')
						{
							gCfgItems.func_btn2_display_flag = 0;
						}
						else if(tmp_index[strlen(">cfg_function_btn2_display:")] == '1')
						{
							gCfgItems.func_btn2_display_flag = 1;
						}
						else
						{
							gCfgItems.func_btn2_display_flag = 1;
						}

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.func_btn2_display_flag,BAK_FUNC2_DISPLAY_ADDR,1); 	
					}
					
					//读功能键命令
					tmp_index = (char *)strstr(file_str, ">function_btn2_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">function_btn2_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							} 					
						}
						code_cnt = 0;
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_FUNCTION2_ADDR,(strlen(cmd_code)+1));					
					}
					#endif
					//喷头最大设置温度
					tmp_index = (char *)strstr(file_str, ">cfg_max_sprayer_temperature:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_max_sprayer_temperature:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.preheat_max_desireSprayerTemp = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.preheat_max_desireSprayerTemp,BAK_SPRAYER1_MAXTEMP_ADDR,2);	
					}

					//热床最大设置温度
					tmp_index = (char *)strstr(file_str, ">cfg_max_hotbed_temperature:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_max_hotbed_temperature:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.preheat_max_desireBedTemp = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.preheat_max_desireBedTemp,BAK_BED_MAXTEMP_ADDR,2);	
					}

					//断电断料端口电平配置。
					tmp_index = (char *)strstr(file_str, ">cfg_PB1_trigger_Level:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen(">cfg_PB1_trigger_Level:")] == '0')
						{
							gCfgItems.filament_det1_level_flg= 0;
						}
						else if(tmp_index[strlen(">cfg_PB1_trigger_Level:")] == '1')
						{
							gCfgItems.filament_det1_level_flg = 1;
						}
						else
						{
							gCfgItems.filament_det1_level_flg = 0;
						}

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_det1_level_flg,BAK_FILAMENT_DET1_FLAG_ADDR,1); 	
					}
					//当PB0作为断料检测时，断电断料端口电平配置。
					tmp_index = (char *)strstr(file_str, ">cfg_PB0_trigger_Level:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen(">cfg_PB0_trigger_Level:")] == '0')
						{
							gCfgItems.filament_det2_level_flg= 0;
						}
						else if(tmp_index[strlen(">cfg_PB0_trigger_Level:")] == '1')
						{
							gCfgItems.filament_det2_level_flg = 1;
						}
						else
						{
							gCfgItems.filament_det2_level_flg = 0;
						}

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_det2_level_flg,BAK_FILAMENT_DET2_FLAG_ADDR,1); 	
					}					
					//打完关机配置
					tmp_index = (char *)strstr(file_str, ">cfg_print_finish_close_Machine:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen(">cfg_print_finish_close_Machine:")] == '1')
						{
							gCfgItems.print_finish_close_machine_flg = 1;
						}
						else
						{
							gCfgItems.print_finish_close_machine_flg = 0;
						}

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.print_finish_close_machine_flg,BAK_PRINT_FINISH_CLOASE_MACHINE_ADDR,1);
						
					} 
					//打印界面更多按钮数
					tmp_index = (char *)strstr(file_str, ">morefunc_cnt:");
					if(tmp_index != 0)
					{
						switch(tmp_index[strlen(">morefunc_cnt:")])
						{
						case '1':
							gCfgItems.morefunc_cnt = 1;
							break;
						case '2':
							gCfgItems.morefunc_cnt = 2;
							break;
						case '3':
							gCfgItems.morefunc_cnt = 3;
							break;
						case '4':
							gCfgItems.morefunc_cnt = 4;
							break;
						case '5':
							gCfgItems.morefunc_cnt = 5;
							break;
						//case '6':
						//	gCfgItems.morefunc_cnt = 6;
						//	break;	
						default:
							gCfgItems.morefunc_cnt = 0;
							break;
						}
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.morefunc_cnt, BAK_PRINTING_MOREFUNC_CNT_ADDR,1);
					}
					
					//读morefunc1命令
					tmp_index = (char *)strstr(file_str, ">morefunc1_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">morefunc1_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}
						}
						code_cnt = 0;
						//擦除4k
						//SPI_FLASH_SectorErase(SD_INF_ADDR);
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_MOREFUNC1_ADDR,(strlen(cmd_code)+1));
						
					}
					//读morefunc2命令
					tmp_index = (char *)strstr(file_str, ">morefunc2_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">morefunc2_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}
						}
						code_cnt = 0;
						//擦除4k
						//SPI_FLASH_SectorErase(SD_INF_ADDR);
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_MOREFUNC2_ADDR,(strlen(cmd_code)+1));
						
					}
					//读morefunc3命令
					tmp_index = (char *)strstr(file_str, ">morefunc3_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">morefunc3_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}
						}
						code_cnt = 0;
						//擦除4k
						//SPI_FLASH_SectorErase(SD_INF_ADDR);
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_MOREFUNC3_ADDR,(strlen(cmd_code)+1));
						
					}
					//读morefunc4命令
					tmp_index = (char *)strstr(file_str, ">morefunc4_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">morefunc4_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}
						}
						code_cnt = 0;
						//擦除4k
						//SPI_FLASH_SectorErase(SD_INF_ADDR);
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_MOREFUNC4_ADDR,(strlen(cmd_code)+1));
						
					}
					//读morefunc5命令
					tmp_index = (char *)strstr(file_str, ">morefunc5_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">morefunc5_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}
						}
						code_cnt = 0;
						//擦除4k
						//SPI_FLASH_SectorErase(SD_INF_ADDR);
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_MOREFUNC5_ADDR,(strlen(cmd_code)+1));
						
					}
					#if 0
					//读morefunc6命令
					tmp_index = (char *)strstr(file_str, ">morefunc6_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">morefunc6_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							}
						}
						code_cnt = 0;
						//擦除4k
						//SPI_FLASH_SectorErase(SD_INF_ADDR);
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_MOREFUNC6_ADDR,(strlen(cmd_code)+1));
						
					}
					#endif
					
					tmp_index = (char *)strstr(file_str, ">cfg_filament_load_speed:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_filament_load_speed:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.filament_load_speed = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_load_speed,BAK_FILAMENT_LOAD_SPEED_ADDR,2);	
					}
					tmp_index = (char *)strstr(file_str, ">cfg_filament_unload_speed:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_filament_unload_speed:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.filament_unload_speed = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_unload_speed,BAK_FILAMENT_UNLOAD_SPEED_ADDR,2);	
					}					
					tmp_index = (char *)strstr(file_str, ">cfg_filament_load_length:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_filament_load_length:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.filament_load_length= atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_load_length,BAK_FILAMENT_LOAD_LENGTH_ADDR,2); 
					}
					tmp_index = (char *)strstr(file_str, ">cfg_filament_unload_length:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_filament_unload_length:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.filament_unload_length= atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_unload_length,BAK_FILAMENT_UNLOAD_LENGTH_ADDR,2); 
					}					
					tmp_index = (char *)strstr(file_str, ">cfg_filament_load_limit_temperature:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_filament_load_limit_temperature:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.filament_limit_temper = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_limit_temper,BAK_FILAMENT_LIMIT_TERMPER_ADDR,2); 
					}
					/*
					tmp_index = (char *)strstr(file_str, ">cfg_filament_unload_limit_temperature:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_filament_unload_limit_temperature:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.filament_unload_limit_temper = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.filament_unload_limit_temper,BAK_FILAMENT_UNLOAD_LIMIT_TERMPER_ADDR,2); 
					}		
					*/
					//自动调平指令
					tmp_index = (char *)strstr(file_str, ">cfg_auto_leveling_cmd:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_auto_leveling_cmd:");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 200)
							{
								break;
							} 					
						}
						code_cnt = 0;
						//分别写入button的命令代码
						SPI_FLASH_BufferWrite(cmd_code,BUTTON_AUTOLEVELING_ADDR,(strlen(cmd_code)+1)); 				
					}

					tmp_index = (char *)strstr(file_str, ">cfg_point_number:");
					if(tmp_index != 0)
					{
						if(tmp_index[strlen(">cfg_point_number:")] == '3')
						{
							gCfgItems.leveling_point_number = 3;
						}
						else if(tmp_index[strlen(">cfg_point_number:")] == '4')
						{
							gCfgItems.leveling_point_number = 4;
						}
						else if(tmp_index[strlen(">cfg_point_number:")] == '5')
						{
							gCfgItems.leveling_point_number = 5;
						}
						else
						{
							gCfgItems.leveling_point_number = 4;
						}

						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point_number,BAK_LEVELING_POINT_NUMBER_ADDR,1);
					}		
					
					tmp_index = (char *)strstr(file_str, ">cfg_point1:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_point1:");
						while((*tmp_index) != ',')
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 
						}
						code_cnt = 0;
						gCfgItems.leveling_point1_x = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point1_x,BAK_LEVELING_POINT1_X_ADDR,2);	
						
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index++;
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.leveling_point1_y = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point1_y,BAK_LEVELING_POINT1_Y_ADDR,2);	
					}
					
					tmp_index = (char *)strstr(file_str, ">cfg_point2:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_point2:");
						while((*tmp_index) != ',')
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 
						}
						code_cnt = 0;
						gCfgItems.leveling_point2_x = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point2_x,BAK_LEVELING_POINT2_X_ADDR,2);	
						
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index++;
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.leveling_point2_y = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point2_y,BAK_LEVELING_POINT2_Y_ADDR,2);	
					}

					tmp_index = (char *)strstr(file_str, ">cfg_point3:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_point3:");
						while((*tmp_index) != ',')
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 
						}
						code_cnt = 0;
						gCfgItems.leveling_point3_x = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point3_x,BAK_LEVELING_POINT3_X_ADDR,2);
						
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index++;
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.leveling_point3_y = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point3_y,BAK_LEVELING_POINT3_Y_ADDR,2); 
					}
					
					tmp_index = (char *)strstr(file_str, ">cfg_point4:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_point4:");
						while((*tmp_index) != ',')
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 
						}
						code_cnt = 0;
						gCfgItems.leveling_point4_x = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point4_x,BAK_LEVELING_POINT4_X_ADDR,2);	
						
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index++;
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.leveling_point4_y = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point4_y,BAK_LEVELING_POINT4_Y_ADDR,2);	
					}

					tmp_index = (char *)strstr(file_str, ">cfg_point5:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_point5:");
						while((*tmp_index) != ',')
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 
						}
						code_cnt = 0;
						gCfgItems.leveling_point5_x = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point5_x,BAK_LEVELING_POINT5_X_ADDR,2); 
						
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index++;
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 					
						}
						code_cnt = 0;
						gCfgItems.leveling_point5_y = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_point5_y,BAK_LEVELING_POINT5_Y_ADDR,2); 
					}

					tmp_index = (char *)strstr(file_str, ">cfg_leveling_z_high:");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_leveling_z_high:");
						//HC-chen 2.8
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt ++;
							tmp_index++;
							if(code_cnt >= 20)
							{
								break;
							} 
						}
						code_cnt = 0;
						gCfgItems.leveling_z_high = atoi(cmd_code);
						I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_z_high,BAK_LEVELING_Z_HIGH,2);
 				      }

			}
			
			tmp_index = (char *)strstr(file_str, ">cfg_leveling_z_speed:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_leveling_z_speed:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.leveling_z_speed = atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_z_speed,BAK_LEVELING_Z_SPEED_ADDR,2);	
			}
			
			tmp_index = (char *)strstr(file_str, ">cfg_leveling_xy_speed:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_leveling_xy_speed:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.leveling_xy_speed = atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_xy_speed,BAK_LEVELING_XY_SPEED_ADDR,2);	
			}
			#ifdef SAVE_FROM_SD
			//选择断电保存方式
			// 1:电容保存方式；0:无电容保存。
			tmp_index = (char *)strstr(file_str, ">cfg_pwroff_save_mode:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_pwroff_save_mode:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.pwroff_save_mode = atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.pwroff_save_mode,BAK_PWROFF_SAVE_MODE_ADDR,1);	
			}		
			#endif
			//断点续打z误差值
			tmp_index = (char *)strstr(file_str, ">cfg_breakpoint_z_error:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_breakpoint_z_error:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				DecStr2Float(cmd_code,  &gCfgItems.breakpoint_z_error);
				//gCfgItems.breakpoint_z_error= atof(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.breakpoint_z_error,BAK_BREAKPOINT_Z_ERROR_ADDR,4);	
			}
			tmp_index = (char *)strstr(file_str, ">cfg_leveling_mode:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_leveling_mode:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.leveling_mode= atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.leveling_mode,BAK_LEVELING_MODE_ADDR,1);	
			}

			tmp_index = (char *)strstr(file_str, ">cfg_have_ups_device:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_have_ups_device:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.have_ups= atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.have_ups,BAK_HAVE_UPS_ADDR,1);	
			}
			tmp_index = (char *)strstr(file_str, ">cfg_insert_det_module:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_insert_det_module:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.insert_det_module= atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.insert_det_module,BAK_INSERT_MODULE_ADDR,1);	
			}	
			tmp_index = (char *)strstr(file_str, ">cfg_multiple_language:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_multiple_language:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.multiple_language = atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.multiple_language,BAK_MULTIPLE_LANGUAGE_ADDR,1);	
			}
			
			//HC-chen 2017.10.10
			#if 0
			tmp_index = (char *)strstr(file_str, ">cfg_OVERTURN_180:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_OVERTURN_180:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.OVERTURN_180 = atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.OVERTURN_180,BAK_OVERTURN_180_ADDR,1);	
			}
			#else
			tmp_index = (char *)strstr(file_str, ">cfg_OVERTURN_180:0");
			if(tmp_index != 0)
			{
				tmp_index += strlen(">cfg_OVERTURN_180:0x");
				
				gCfgItems.OVERTURN_180 = ascii2dec(tmp_index, 2);

				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.OVERTURN_180, BAK_OVERTURN_180_ADDR,1);
			}
			#endif
			
			tmp_index = (char *)strstr(file_str, ">cfg_disable_PWR_det:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_disable_PWR_det:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.disable_pwr_det= atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.disable_pwr_det,BAK_DISABLE_PWR_DET_ADDR,1);
 			}	
			tmp_index = (char *)strstr(file_str, ">cfg_disable_MT_det:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_disable_MT_det:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.disable_mt_det= atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.disable_mt_det,BAK_DISABLE_MT_DET_ADDR,1);	
 			}

			//HC-chen 3.31
			tmp_index = (char *)strstr(file_str, ">cfg_single_nozzle:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_single_nozzle:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.single_nozzle= atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.single_nozzle,BAK_SINGLE_NOZZLE_ADDR,1);
 			}
			//HC-chen 7.25
			tmp_index = (char *)strstr(file_str, ">cfg_M110_add_checkSum:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_M110_add_checkSum:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.add_checkSum= atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.add_checkSum,BAK_M110_ADD_CHECKSUM_ADDR,1);
 			}
			tmp_index = (char *)strstr(file_str, ">cfg_display_eeror_message:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_display_eeror_message:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.display_error= atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.display_error,BAK_DISP_EEROR_ADDR,1);
 			}
			
			tmp_index = (char *)strstr(file_str, ">cfg_test_fala_flag:0x");
			if(tmp_index != 0)
			{
				tmp_index += strlen(">cfg_test_fala_flag:0x");	
				gCfgItems.TestFalaFlag = ascii2dec(tmp_index, 2);
				
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.TestFalaFlag, BAK_TEST_FALA_FLAG_ADDR, 1); 
 			}
			tmp_index = (char *)strstr(file_str, ">cfg_unload_retract_length:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_unload_retract_length:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.unload_retract_length= atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.unload_retract_length,BAK_UNLOAD_RETRACT_LENGTH_ADDR,2); 
			}	
			tmp_index = (char *)strstr(file_str, ">cfg_unload_retract_speed:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_unload_retract_speed:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.unload_retract_speed= atoi(cmd_code);
				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.unload_retract_speed,BAK_UNLOAD_RETRACT_SPEED_ADDR,2); 
			}	
			tmp_index = (char *)strstr(file_str, ">cfg_babystep_btn_display:");
			if(tmp_index != 0)
			{
				if(tmp_index[strlen(">cfg_babystep_btn_display:")] == '1')
				{
					gCfgItems.babystep = 1;
				}
				else
				{
					gCfgItems.babystep = 0;
				}

				I2C_EE_BufferWrite((uint8_t *)&gCfgItems.babystep,BAK_BABYSTEP_ADDR,1);
				
			} 
			
			//if(byte_have_read < 1024)break;
		}
		
		f_close(&cfgFile);


		if (f_open(&saveFile, cfg_file_cur, FA_OPEN_EXISTING | FA_READ) == FR_OK)
		{
			f_close(&saveFile);
			r = f_unlink(cfg_file_cur);
		}
				
		r = f_rename(cfg_file, "CONFIG.CUR");
		
		
		
	}
		
}


//! This function checks the file extension
//! @param     FileName   ASCII string of a File Name (ex: "abc.wav" )
//!
//! @param     filter      extension filter is a ASCII string (ex: "wav" )
//!
//! @return    TRUE, the file name have a good extension
//! @return    FALSE, otherwise
bool  File_type_Check( u8 * FileName, u8 * filter )
{
  u8 * FileNameLocal;
  u32 DotPos = 0;
  bool RetVal = FALSE;
  

  while(1)
  {
    if((*(FileName + DotPos) == '.') || (DotPos >= 8+3))
    {
      break;
    }
    DotPos++;
  }
  
  if(DotPos >= 8+3)
  {
    RetVal = FALSE;
  }
  else
  {
    FileNameLocal = (FileName + DotPos + 1);
    DotPos = 0;
    for(DotPos=0; DotPos < 3; DotPos++)
    {
      if((*FileNameLocal == *filter) || ((*FileNameLocal+('a'-'A')) == *filter))
      {
        RetVal = TRUE;
        if(*(filter + 1) == '\0')
          break;
      }
      else
      {
        RetVal = FALSE;
        break;
      }
    }
  }

  return RetVal;
}


/*******************************************************************************
* Function Name  : Buffercmp
* Description    : Compares two buffers.
* Input          : - pBuffer1, pBuffer2: buffers to be compared.
*                : - BufferLength: buffer's length
* Output         : None
* Return         : PASSED: pBuffer1 identical to pBuffer2
*                  FAILED: pBuffer1 differs from pBuffer2
*******************************************************************************/
/*TestStatus Buffercmp(u8* pBuffer1, u8* pBuffer2, u16 BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}

void BufferSet(u8* pBuffer, u8 data, u16 BufferLength)
{
  u16 i;
  for(i = 0; i<BufferLength; i++)
  {
    pBuffer[i] = data;
  }
}*/
