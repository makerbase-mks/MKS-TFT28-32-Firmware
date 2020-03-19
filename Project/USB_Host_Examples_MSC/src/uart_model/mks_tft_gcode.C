/********************   (C) COPYRIGHT 2014 www.makerbase.com.cn   ********************
 * 文件名  ：mks_tft_gcode.c
 * 描述    ：1.从u盘读取源文件，每次读取1k字节，轮流写入udiskBuffer.buffer[0]和udiskBuffer.buffer[1]；
 						2. 当udiskBuffer.buffer[n]非空时，提取出有效gcode指令，加上前/后缀后,Push到gcodeTxFIFO队列。
 * 作者    ：skyblue
**********************************************************************************/


#include <stdio.h>
#include <string.h>
#include "ff.h"
#include "mks_tft_fifo.h"
#include "mks_tft_gcode.h"
//#include "main.h"
#include "mks_tft_com.h"
#include "printer.h"
#include "draw_ui.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#include "xprintf.h"

extern FIL fp_reprint_rw;

extern USBH_HOST                     USB_Host;

extern unsigned char path_bak[15];
extern unsigned char *path_reprint;
extern uint32_t rePrintOffset;

extern PRINT_TIME print_time;
extern void Close_machine_display();

extern void Btn_putdown_close_machine();
extern uint8_t IsChooseAutoShutdown;
extern uint8_t close_fail_flg;
extern uint16_t close_fail_cnt;

struct position Gcode_current_position[30];

uint8_t Chk_close_machine_flg = 0;

UDISK_DATA_BUFFER udiskBuffer;

unsigned char note_flag=1;	//注释标志 init : 1
unsigned long gcodeLineCnt=0;	//指令行号编号 Nxxxxx
unsigned long gcodeLineCntBak1=0;
unsigned long gcodeLineCntBak2=0;

UDISK_FILE_STAUS udiskFileStaus;			//文件状态

TARGER_TEMP targetTemp;
TEMP_STATUS	tempStatus;
void getFanStatus(unsigned char *gcode,unsigned char *end)
{
	unsigned char tempBuf[30];
	unsigned char i;
	unsigned char *p;
	
		if(*gcode == 'M' && *(gcode+1) == '1' && *(gcode+2) == '0'&& (*(gcode+3) == '6' || *(gcode+3) == '7' ))	//M106 M107
		{
			p = gcode;
			i=0;
			while(p<end)
			{
				tempBuf[i++]=*p++;
			}
			tempBuf[i] = '\n';
			
			pushFIFO(&gcodeCmdRxFIFO,&tempBuf[0]);
		}
		
}

void getTargetTemp(unsigned char *gcode,unsigned char *end)
{
	int8_t *tmpStr_1 = 0;
	
	unsigned char tempBuf[80]="ok T:0 /210 B:0 /45 @:0 B@:0";
	unsigned char count;
	unsigned char *p;
	if(tempStatus == temp_ok )		return;
	
	p = &tempBuf[0];
	//获取单位 mm or inch ,默认mm

	if(*gcode == 'G' && *(gcode+1) == '2' && *(gcode+2) == '0' )
		RePrintData.unit = 1;	//0 mm,1 inch
/*	
//20151019
	if(*gcode == 'M' && *(gcode+1) == '1' && (*(gcode+2) == '9' ||*(gcode+2) == '4' )&& *(gcode+3) == '0')	//M190 or M140
	{
		gcode += 4;
		count = 0;
		while(*gcode++ != 'S')
			if(count++ > 10) break;

		while(gcode < end)
		{
			if(*gcode == '.')break;
			*p++ = *gcode++;
			if(p >=&tempBuf[0]+10) break;
		}
		*p = '\0';
		targetTemp.bedTemp = atoi(&tempBuf[0]);
	}

	if(*gcode == 'M' && *(gcode+1) == '1' && *(gcode+2) == '0'&& (*(gcode+3) == '9' || *(gcode+3) == '4'))	//M109 or M104
	{
		gcode += 4;
		count = 0;
		while(*gcode++ != 'S')
			if(count++ > 10) break;

		while(gcode < end)
		{
			if(*gcode == '.') break;
			*p++ = *gcode++;
			if(p >=&tempBuf[0]+10) break;
		}
		*p = '\0';
		targetTemp.t0Temp = atoi(&tempBuf[0]);
	}
*/
	if(*gcode == 'M' && *(gcode+1) == '1' && (*(gcode+2) == '9' ||*(gcode+2) == '4' )&& *(gcode+3) == '0')	//M190 or M140
	{
		gcode += 4;
		count = 0;
		while(*gcode++ != 'S')
			if(count++ > 10) break;

		while(gcode < end)
		{
			if(*gcode == '.')break;
			*p++ = *gcode++;
			if(p >=&tempBuf[0]+10) break;
		}
		*p = '\0';
		targetTemp.bedTemp = atoi(&tempBuf[0]);
	}

	if(gCfgItems.sprayerNum == 1)
	{
		if(*gcode == 'M' && *(gcode+1) == '1' && *(gcode+2) == '0'&& (*(gcode+3) == '9' || *(gcode+3) == '4'))	//M109 or M104
		{
			gcode += 4;
			count = 0;
			while(*gcode++ != 'S')
				if(count++ > 10) break;

			while(gcode < end)
			{
				if(*gcode == '.') break;
				*p++ = *gcode++;
				if(p >=&tempBuf[0]+10) break;
			}
			*p = '\0';
			targetTemp.t0Temp = atoi(&tempBuf[0]);
		}
	}
	else
	{
		if(*gcode == 'M' && *(gcode+1) == '1' && *(gcode+2) == '0'&& (*(gcode+3) == '9' || *(gcode+3) == '4'))	//M109 or M104
		{
			if((int8_t *)strstr(gcode, "T0"))
			{
					tmpStr_1 = (int8_t *)strstr(gcode, "S");	
					if(tmpStr_1)
					{
						gcode = tmpStr_1+1;
						while(gcode < end)
						{
							if(*gcode == '.') break;
							*p++ = *gcode++;
							if(p >=&tempBuf[0]+10) break;
						}
						*p = '\0';
						targetTemp.t0Temp = atoi(&tempBuf[0]);						
					}
			}
			else if((int8_t *)strstr(gcode, "T1"))
			{
					tmpStr_1 = (int8_t *)strstr(gcode, "S");	
					if(tmpStr_1)
					{
						gcode = tmpStr_1+1;
						while(gcode < end)
						{
							if(*gcode == '.') break;
							*p++ = *gcode++;
							if(p >=&tempBuf[0]+10) break;
						}
						*p = '\0';
						targetTemp.t1Temp = atoi(&tempBuf[0]);						
					}			
			}
			else
			{
					tmpStr_1 = (int8_t *)strstr(gcode, "S");	
					if(tmpStr_1)
					{
						gcode = tmpStr_1+1;
						while(gcode < end)
						{
							if(*gcode == '.') break;
							*p++ = *gcode++;
							if(p >=&tempBuf[0]+10) break;
						}
						*p = '\0';
						if(RePrintData.spayerchoose == 1)
						{
							targetTemp.t1Temp = atoi(&tempBuf[0]);
						}
						else
						{
							targetTemp.t0Temp = atoi(&tempBuf[0]);						
						}		
					}
			}		

		}

	}
/*
	if((targetTemp.bedTemp > 0 && targetTemp.t0Temp >0) ||( gcodeLineCnt> 50))
	{
		//tempBuf[40]="ok T:0 /210 B:0 /45 @:0 B@:0";
		p = &tempBuf[0];	
		*p++ = 'o';*p++ = 'k';*p++ = ' ';*p++ = 'T';*p++ = ':';*p++ = '0';*p++ = ' ';*p++ = '/';
		*p++ = targetTemp.t0Temp/100+48;
		*p++ = (targetTemp.t0Temp/10)%10 + 48;
		*p++ = targetTemp.t0Temp%10 + 48;
		
		*p++ = ' ';	*p++ = 'B';*p++ = ':';*p++ = '0';*p++ = ' ';*p++ = '/';
		*p++ = targetTemp.bedTemp/10+48;
		*p++ = targetTemp.bedTemp%10 + 48;
		*p++ = ' ';*p++ = '@';*p++ = ':';*p++ = '0';*p++ = ' ';*p++ = 'B';*p++ = '@';*p++ = ':';*p++ = '0';*p++ = '\n';
		
		pushFIFO(&gcodeCmdRxFIFO,&tempBuf[0]);
		tempStatus = temp_ok;
	}
*/
	if((targetTemp.bedTemp > 0)||(targetTemp.t0Temp >0)||(targetTemp.t1Temp >0))
	{
		if(gCfgItems.sprayerNum == 1)
		{
			//tempBuf[40]="ok T:0 /210 B:0 /45 @:0 B@:0";
			p = &tempBuf[0];	
			*p++ = 'o';*p++ = 'k';*p++ = ' ';*p++ = 'T';*p++ = ':';
			// *p++ = '0';
			*p++ = ((uint32_t)(gCfgItems.curSprayerTemp[0]))/100+48;
			*p++ = (((uint32_t)(gCfgItems.curSprayerTemp[0]))/10)%10 + 48;
			*p++ = ((uint32_t)(gCfgItems.curSprayerTemp[0]))%10 + 48;
			*p++ = ' ';*p++ = '/';
			*p++ = targetTemp.t0Temp/100+48;
			*p++ = (targetTemp.t0Temp/10)%10 + 48;
			*p++ = targetTemp.t0Temp%10 + 48;
			
			*p++ = ' ';	*p++ = 'B';*p++ = ':';
			// *p++ = '0';
			*p++ = ((uint32_t)(gCfgItems.curBedTemp))/100+48;
			*p++ = (((uint32_t)(gCfgItems.curBedTemp))/10)%10+48;
			*p++ = ((uint32_t)(gCfgItems.curBedTemp))%10 + 48;
			*p++ = ' ';*p++ = '/';
			*p++ = targetTemp.bedTemp/100+48;
			*p++ = (targetTemp.bedTemp/10)%10+48;
			*p++ = targetTemp.bedTemp%10 + 48;
			*p++ = ' ';*p++ = '@';*p++ = ':';*p++ = '0';*p++ = ' ';*p++ = 'B';*p++ = '@';*p++ = ':';*p++ = '0';*p++ = '\n';
			
			pushFIFO(&gcodeCmdRxFIFO,&tempBuf[0]);
		}
		else
		{
			//ok T:0 /210 B:0 /45 T0:0/210 T1:0 /210 @:0 B@:0
			p = &tempBuf[0];	
			*p++ = 'o';*p++ = 'k';*p++ = ' ';*p++ = 'T';*p++ = ':';
			// *p++ = '0';
			*p++ = ((uint32_t)(gCfgItems.curSprayerTemp[0]))/100+48;
			*p++ = (((uint32_t)(gCfgItems.curSprayerTemp[0]))/10)%10 + 48;
			*p++ = ((uint32_t)(gCfgItems.curSprayerTemp[0]))%10 + 48;
			*p++ = ' ';*p++ = '/';
			*p++ = targetTemp.t0Temp/100+48;
			*p++ = (targetTemp.t0Temp/10)%10 + 48;
			*p++ = targetTemp.t0Temp%10 + 48;
			//B
			*p++ = ' ';	*p++ = 'B';*p++ = ':';
			// *p++ = '0';
			*p++ = ((uint32_t)(gCfgItems.curBedTemp))/100+48;
			*p++ = (((uint32_t)(gCfgItems.curBedTemp))/10)%10+48;
			*p++ = ((uint32_t)(gCfgItems.curBedTemp))%10 + 48;
			*p++ = ' ';*p++ = '/';
			*p++ = targetTemp.bedTemp/100+48;
			*p++ = (targetTemp.bedTemp/10)%10+48;
			*p++ = targetTemp.bedTemp%10 + 48;
			//T0
			*p++ = ' ';*p++ = 'T';*p++ = '0';*p++ = ':';
			// *p++ = '0';
			*p++ = ((uint32_t)(gCfgItems.curSprayerTemp[0]))/100+48;
			*p++ = (((uint32_t)(gCfgItems.curSprayerTemp[0]))/10)%10 + 48;
			*p++ = ((uint32_t)(gCfgItems.curSprayerTemp[0]))%10 + 48;			
			*p++ = ' ';*p++ = '/';
			*p++ = targetTemp.t0Temp/100+48;
			*p++ = (targetTemp.t0Temp/10)%10 + 48;
			*p++ = targetTemp.t0Temp%10 + 48;
			//T1
			*p++ = ' ';*p++ = 'T';*p++ = '1';*p++ = ':';
			// *p++ = '0';
			*p++ = ((uint32_t)(gCfgItems.curSprayerTemp[1]))/100+48;
			*p++ = (((uint32_t)(gCfgItems.curSprayerTemp[1]))/10)%10 + 48;
			*p++ = ((uint32_t)(gCfgItems.curSprayerTemp[1]))%10 + 48;			
			*p++ = ' ';*p++ = '/';
			*p++ = targetTemp.t1Temp/100+48;
			*p++ = (targetTemp.t1Temp/10)%10 + 48;
			*p++ = targetTemp.t1Temp%10 + 48;

			
			*p++ = ' ';*p++ = '@';*p++ = ':';*p++ = '0';*p++ = ' ';*p++ = 'B';*p++ = '@';*p++ = ':';*p++ = '0';*p++ = '\n';

			pushFIFO(&gcodeCmdRxFIFO,&tempBuf[0]);			
		}
		
	}

	if(gcodeLineCnt> 50)
	{
		tempStatus = temp_ok;
	}

}

volatile RECOVER_SD rec_sd;
void udiskBufferInit(void)
{
	memset(udiskBuffer.buffer[0],'\n',sizeof(udiskBuffer.buffer[0]));
	memset(udiskBuffer.buffer[1],'\n',sizeof(udiskBuffer.buffer[1]));
	udiskBuffer.current = 0;
	udiskBuffer.p = udiskBuffer.buffer[udiskBuffer.current];
	udiskBuffer.state[udiskBuffer.current] = udisk_buf_full;
	udiskBuffer.state[(udiskBuffer.current+1)%2] = udisk_buf_empty;

	note_flag = 1;
	gcodeLineCnt = 0;
	RePrintData.record_line = 0;
	
	udiskFileStaus = udisk_file_ok;
	/*----------------*/
	targetTemp.bedTemp = 0;
	targetTemp.t0Temp = 0;
	targetTemp.t1Temp = 0;
	targetTemp.t2Temp = 0;
	tempStatus = temp_fail;
	/*----------------*/

	RePrintData.saveEnable = 0;
	
	initFIFO(&gcodeTxFIFO);
//	initFIFO(&gcodeRxFIFO);
	initFIFO(&gcodeCmdTxFIFO);
	initFIFO(&gcodeCmdRxFIFO);

	//chen 10.8
	rec_sd.read_disk_err = 0;
	rec_sd.just_delay_one = 0;
}

volatile long total;
volatile FRESULT res_ok;
volatile int usb_cnt;
volatile int usb_cnt_bak;
//volatile uint8_t sd_cd=0;
/* 读卡分频32M,读1K文件时间为7ms,读卡分频8M读1K文件时间为5ms*/
void udiskFileR(FIL *srcfp)		//读取u盘文件，写入udiskBuffer
{		
		unsigned int readByteCnt=0;
		if((udiskBuffer.state[(udiskBuffer.current+1)%2] == udisk_buf_full) && (udiskFileStaus == udisk_file_ok))
			return;
		//if(sd_cd==1)
			//return;
/*--------------------------------*/
//FRESULT f_read (
//	FIL *fp, 		/* Pointer to the file object */
//	void *buff,		/* Pointer to data buffer */
//	UINT btr,		/* Number of bytes to read */
//	UINT *br		/* Pointer to number of bytes read */
//)

/*--------------------------------*/	
		SPI1_SetSpeed(SPI_BaudRatePrescaler_32);//设置到 2.5MHz
		switch(udiskFileStaus)
		{
			case udisk_file_ok:
				res_ok = f_read(srcfp,udiskBuffer.buffer[(udiskBuffer.current+1)%2],UDISKBUFLEN,&readByteCnt);

				//if(res_ok == FR_OK)
				//chen 10.8
				if(res_ok == FR_OK && rec_sd.read_disk_err ==0)
				{
					udiskBuffer.state[(udiskBuffer.current+1)%2] = udisk_buf_full;
					total += readByteCnt;
					if((readByteCnt < UDISKBUFLEN)&&(srcfp->fsize <= total))
					//if(readByteCnt < UDISKBUFLEN)
					{
						udiskFileStaus = udisk_file_end;
						fileEndCnt = 30000; 
						total = 0;
					}
				}
				else
				{
					memset(udiskBuffer.buffer[(udiskBuffer.current+1)%2],'\n',sizeof(udiskBuffer.buffer[(udiskBuffer.current+1)%2]));				
					//if((readByteCnt < UDISKBUFLEN)&&(srcfp->fsize > total))
					if(srcfp->fsize > total)
					{
						//chen 9.29
						//接收到ok后不再发送gcode数据
						usart2Data.prWaitStatus = pr_wait_idle;
						
						if(rec_sd.just_delay_one == 0)
						tftDelay(2000);  //阻止重复延时
						
						rec_sd.just_delay_one = 1;
						
						Restart_data_init();
					} 					
				}
					//RePrintData.offset1 =  f_tell(srcfp);	//获取文件偏移位置
				break;
			case udisk_file_end:
					//if((fileEndCnt == 0)||(udiskBuffer.state[0] == udisk_buf_empty && udiskBuffer.state[1] == udisk_buf_empty && checkFIFO(&gcodeTxFIFO)== fifo_empty)) //打印结束
					if((udiskBuffer.state[0] == udisk_buf_empty && udiskBuffer.state[1] == udisk_buf_empty && checkFIFO(&gcodeTxFIFO)== fifo_empty)) //打印结束
					{
						tftDelay(3);
						printerInit();
						tftDelay(3);

						I2C_EE_Init(100000);

						I2C_EE_BufferRead(&dataToEeprom, BAK_REPRINT_INFO,  4); 
						dataToEeprom &= 0x00ffffff;
						dataToEeprom |= (uint32_t)(printer_normal << 24 ) & 0xff000000;
						I2C_EE_BufferWrite(&dataToEeprom, BAK_REPRINT_INFO,  4); 		// 续打标志(uint8_t) | 单位unit (uint8_t) | saveFlag(uint8_t)| null(uint8_t)
						
						printerStaus = pr_idle;		//打印结束
						usart2Data.printer = printer_idle;
						usart2Data.prWaitStatus = pr_wait_idle;
						usart2Data.timer = timer_stop;						//清除定时器

						#ifdef SAVE_FROM_SD
						//删除续打数据文件。
						if(gCfgItems.pwroff_save_mode == 0)
						{
							if(gCfgItems.fileSysType == FILE_SYS_SD)
							{
								strcpy(path_bak, "1:");
								strcat(path_bak,path_reprint);						
								f_unlink(path_bak);
							}
							else
							{
								//strcpy(path_bak, "0:");
							}
						}
						#endif
						
						if((gCfgItems.print_finish_close_machine_flg == 1)&&(IsChooseAutoShutdown == 1))
						{
							Close_machine_display();
							IsChooseAutoShutdown = 0;
							#if 0
							//Print_finish_close_machine();
							Btn_putdown_close_machine();
							IsChooseAutoShutdown = 0;
							clear_cur_ui();
							//GUI_SetFont(&FONT_TITLE);
							if(gCfgItems.language == LANG_COMPLEX_CHINESE)
							{
								GUI_SetFont(&GUI_FontHZ16);
							}
							else
							{
								GUI_SetFont(&FONT_TITLE);
							}

							if(gCfgItems.language == LANG_ENGLISH)
							{
								GUI_DispStringAt("Print end! Closing Machine...", 50, 120);
							}
							else 	if(gCfgItems.language == LANG_COMPLEX_CHINESE)
							{
								GUI_DispStringAt("打印完成!正在關機...", 50, 120);
							}
							else
							{
								GUI_DispStringAt("打印完成! 正在关机...", 50, 120);
							}
							close_fail_flg = 1;
							close_fail_cnt = 0;
							while(close_fail_flg);
							clear_cur_ui();
							draw_dialog(DIALOG_TYPE_M80_FAIL);
							#endif
						}
					}

					if(udiskBuffer.state[udiskBuffer.current] == udisk_buf_empty)
					{
							udiskBuffer.current = (udiskBuffer.current+1)%2;
							udiskBuffer.p = udiskBuffer.buffer[udiskBuffer.current];
					}

				break;
				default : break;
		}
		SPI1_SetSpeed(SPI_BaudRatePrescaler_8);//设置到 10MHz
}

extern uint8_t gCurDir[100];


void Restart_data_init()
{
	volatile uint8_t record_cnt;
	volatile uint16_t last_tick;
	
	memset(gCurDir, 0, sizeof(gCurDir));
		
	f_close(srcfp);
	strcpy(gCurDir, "1:");
	SD_Initialize();
	ShowSDFiles();

	res_ok=f_open(srcfp,curFileName,FA_READ);
	if(res_ok == FR_OK)
	{	
		rec_sd.just_delay_one = 0;
		//找到对应行的文件偏移
		do
		{
			for(record_cnt=0;record_cnt<30;record_cnt++)
			{
				if(Gcode_current_position[record_cnt].Gcode_LineNumb == (RePrintData.record_line))
				{
						RePrintData.offset = Gcode_current_position[record_cnt].Gcode_fileOffset;
						break;
				}
			}
		}while(record_cnt == 30 && RePrintData.record_line--);
		
		f_lseek(srcfp, RePrintData.offset);

		initFIFO(&gcodeTxFIFO);
		initFIFO(&gcodeCmdTxFIFO);
		initFIFO(&gcodeCmdRxFIFO);
		
		memset(udiskBuffer.buffer[0],'\n',sizeof(udiskBuffer.buffer[0]));
		memset(udiskBuffer.buffer[1],'\n',sizeof(udiskBuffer.buffer[1]));
		udiskBuffer.current = 0;
		udiskBuffer.p = udiskBuffer.buffer[udiskBuffer.current];
		udiskBuffer.state[udiskBuffer.current] = udisk_buf_full;
		udiskBuffer.state[(udiskBuffer.current+1)%2] = udisk_buf_empty;

		note_flag = 1;
		gcodeLineCnt = 0;
		RePrintData.record_line = 0;
		
		//填指令
		while(checkFIFO(&gcodeTxFIFO)!= fifo_full)
		{
			udiskFileR(srcfp);												
			pushTxGcode();	
			/*last_tick=getTick();
			if(getTickDiff(last_tick, getTick()) > 4000)
				break;	*/
		}	
		//发N-1 M110*15
		printerInit();
	}	
}

void pushTxGcode(void)		//从udiskBuffer数据中提取出有效的gcode指令，并加上行号，push到gcodeTxFIFO
{
	//tan 20170111
	int8_t *Ztemp;
	unsigned char i=0;

	static unsigned char position_cnt = 0;
	unsigned char numb_cnt = 0;
	
	unsigned char gcode[FIFO_SIZE];		//存储从udiskBuffer提取的一条gcode
	unsigned char *p=gcode;				//指向gcode的指针
	unsigned char gcode_tx[FIFO_SIZE];	//可发送的gcode指令，加了行号和校验码
	unsigned char *p_tx=gcode_tx;		//指向gcode_tx的指针
	unsigned long gcodeLineCnt_b;		//暂存gcodeLineCnt
	unsigned char lineCntBuf[20];		//存储行号字符串
	unsigned char *p_cnt=lineCntBuf;	
	unsigned char checkSum=0;			//校验和
	unsigned char ulockCnt=0;			//长注释 计数，防止 搜索长数据，导致不能从udisk读取文件，造成死锁

	if(checkFIFO(&gcodeTxFIFO)== fifo_full)			//队列满
		return;

	if(udiskBuffer.state[udiskBuffer.current] == udisk_buf_empty)	//buffer空
		return;

			while(*udiskBuffer.p != '\n'  && *udiskBuffer.p != '\r')	//行结束
			{
				if(p-gcode > (FIFO_SIZE-10))	//一行指令太长，结束，并注释掉后续字符
				{
					*(udiskBuffer.p +1)= ';';
					break;
				}

				//if(ulockCnt++ > FIFO_SIZE && p == gcode)		//防止长注释 造成死锁
				//{
				//	return;
				//}

				

				if(*udiskBuffer.p == ';')	//去掉 ';' 后面的注释
					note_flag =  0;

				if(note_flag)
					*p++ = *udiskBuffer.p++;	//提取有效gcode指令
				else
					udiskBuffer.p++;

				if(udiskBuffer.p == udiskBuffer.buffer[udiskBuffer.current]+ UDISKBUFLEN)	//当前buffer 读取结束,转换到另一buffer
				{
					memset(udiskBuffer.buffer[udiskBuffer.current],'\n',sizeof(udiskBuffer.buffer[0]));		//buffer 数据置'\n'
					udiskBuffer.state[udiskBuffer.current] = udisk_buf_empty;								//buffer 状态置empty
					udiskBuffer.current = (udiskBuffer.current+1)%2;										//转下一个buffer
					udiskBuffer.p = udiskBuffer.buffer[udiskBuffer.current];								//地址指向下一个buffer
				}

				if(ulockCnt++ > FIFO_SIZE && p == gcode)		//防止长注释 造成死锁
				{
					return;
				}


			}
			udiskBuffer.p++;	//跳过'\n'字符
			if(udiskBuffer.p == udiskBuffer.buffer[udiskBuffer.current]+ UDISKBUFLEN)	//当前buffer 读取结束,转换到另一buffer
				{
					memset(udiskBuffer.buffer[udiskBuffer.current],'\n',sizeof(udiskBuffer.buffer[0]));		//buffer 数据置'\n'
					udiskBuffer.state[udiskBuffer.current] = udisk_buf_empty;								//buffer 状态置empty
					udiskBuffer.current = (udiskBuffer.current+1)%2;										//转下一个buffer
					udiskBuffer.p = udiskBuffer.buffer[udiskBuffer.current];								//地址指向下一个buffer
				}

			note_flag = 1;		

			if(p > gcode)		//读取到了gcode指令
			{
				while(*(--p) == 32);	//去掉gcode指令后多余的空格
					p++;
				
				*p_tx++ = 'N';					//加'N'	
				
				gcodeLineCnt_b = gcodeLineCnt;			//加行号
				
			
				*p_cnt++=gcodeLineCnt_b%10 + 48;
				gcodeLineCnt_b /= 10;
				while(gcodeLineCnt_b!=0)
				{
					*p_cnt++=gcodeLineCnt_b%10 + 48;
					gcodeLineCnt_b /= 10;
				}


				while(p_cnt>lineCntBuf)
					*p_tx++ = *--p_cnt;
				
				*p_tx++ = 32;							//加空格

				gcodeLineCnt++;
				//单双喷头挤出判断
				if((gcode[0]=='T')&&(gcode[1]=='0'))
				{
					RePrintData.spayerchoose = 0;
				}
				if((gcode[0]=='T')&&(gcode[1]=='1'))
				{
					RePrintData.spayerchoose = 1;
				}
				//
				getTargetTemp(&gcode[0],p);			//获取目标温度
				getFanStatus(&gcode[0],p);				//获取风扇状态
				
				p_cnt=gcode;								//加gcode指令,临时使用p_cnt
				while(p_cnt<p)								
				{
				*p_tx++ = *p_cnt++;
				}
				*p_tx++ = '*';										//加'*'

															//加校验
				p_cnt= gcode_tx;
				while(*p_cnt != '*')
					checkSum ^= *p_cnt++;
				
				if(checkSum/100 != 0)				
				{
					*p_tx++ = checkSum/100 + 48;
					*p_tx++ = (checkSum/10)%10 + 48;
					*p_tx++ = checkSum%10 + 48;
				}
				else if(checkSum/10 != 0)
				{
					*p_tx++ = checkSum/10 + 48;
					*p_tx++ = checkSum%10 + 48;
				}
				else
					*p_tx++ = checkSum%10 + 48;
				
				*p_tx++ = '\n';								//加'\n'

				//USART2_CR1 &= 0xff9f;
				pushFIFO(&gcodeTxFIFO,&gcode_tx[0]);			//入队列
				//tan 20170111
				//if(gCfgItems.pwroff_save_mode == 0)
				//只在SD卡中做文件断电续打的方式
				//if(gCfgItems.fileSysType == FILE_SYS_SD)
				{
					Ztemp = (int8_t *)strstr(&gcode_tx[0],"Z");
					if(Ztemp)
					{
						i=0;
						strcpy(gCfgItems.z_display_pos_bak,gCfgItems.z_display_pos);
						memset(gCfgItems.z_display_pos,0,sizeof(gCfgItems.z_display_pos));
						while((*(Ztemp+1+i)!=' ')&&(*(Ztemp+1+i)!='*')&&(*(Ztemp+1+i)!='\r')&&(*(Ztemp+1+i)!='\n'))
						{
						#ifdef SAVE_FROM_SD
							gCfgItems.sd_saving = 1;
							
							gCfgItems.sd_save_flg = 1;
							gcodeLineCntBak1 = gcodeLineCnt;
						#endif
							if((*(Ztemp+1+i) =='+')||(*(Ztemp+1+i) =='-'))
							{
								strcpy(gCfgItems.z_display_pos,gCfgItems.z_display_pos_bak);
								break;
							}
							gCfgItems.z_display_pos[i] = *(Ztemp+1+i);
							i++;
							if(i>20)break;
						}
					}
					/*
					if(gCfgItems.sd_save_flg == 1)
					{
						gcodeLineCntBak2 = gcodeLineCnt;
					}
					if(gcodeLineCntBak2-gcodeLineCntBak1 >= 10)
					{
						gcodeLineCntBak2 = 0;
						gcodeLineCntBak1 = 0;
						gCfgItems.sd_save_flg = 0;
						gCfgItems.sd_saving = 1;
					}
					*/
				}
				RePrintData.offset =  f_tell(srcfp)-UDISKBUFLEN;
				if(udiskBuffer.state[(udiskBuffer.current+1)%2] == udisk_buf_full)
					RePrintData.offset -= UDISKBUFLEN;
				RePrintData.offset += udiskBuffer.p - udiskBuffer.buffer[udiskBuffer.current];
				//USART2_CR1 |= 0x0060;

				//20151012
				Gcode_current_position[position_cnt].Gcode_LineNumb= gcodeLineCnt;
				Gcode_current_position[position_cnt++].Gcode_fileOffset= RePrintData.offset;
				if(position_cnt >= 30)
				{
					position_cnt = 0;
				}
				

			}
}
#ifdef SAVE_FROM_SD
unsigned char path_bak[15]= {0};
unsigned char *path_reprint = "/mks_pft.sys";
FIL fp_reprint_rw;

extern volatile uint8_t per_second_save_sd;
void sd_saved_data()
{
	unsigned char sd_buf_w[100];
	//FIL fp_reprint_w;
	unsigned int bw_repint;
	unsigned char i=0;
	if((gCfgItems.sd_saving)&&(printerStaus == pr_working)&&(per_second_save_sd==1))
	{
		per_second_save_sd=0;
		memset(sd_buf_w,0,sizeof(sd_buf_w));
		sprintf(sd_buf_w,"P:%d|T0:%.2f|T1:%.2f|B:%.2f|FanOn:%d|FanSp:%d|h:%d|m:%d|Z:%s|C:%d|",\
			RePrintData.offset,gCfgItems.desireSprayerTemp[0],gCfgItems.desireSprayerTemp[1],\
			gCfgItems.desireBedTemp,gCfgItems.fanOnoff,gCfgItems.fanSpeed,\
			print_time.hours,print_time.minutes,gCfgItems.z_display_pos,gCfgItems.curSprayerChoose);
		//strcat(sd_buf_w,gCfgItems.z_display_pos);
		//sd_buf_w的每个值都与0x4D异或(0x4D对应于ASCII的M)
		
		for(i=0;i<strlen(sd_buf_w);i++)
		{
			sd_buf_w[i] = sd_buf_w[i]^0x4d; 
		}
		
		if(gCfgItems.fileSysType == FILE_SYS_SD)
		{
			strcpy(path_bak, "1:");

			strcat(path_bak,path_reprint);
			if(f_open(&fp_reprint_rw, path_bak, FA_WRITE|FA_OPEN_ALWAYS)== FR_OK)
			{
				f_write(&fp_reprint_rw, sd_buf_w, 100,&bw_repint);
				f_close(&fp_reprint_rw);	
				gCfgItems.sd_saving = 0;
			}
		}
		else
		{
			//strcpy(path_bak, "0:");
		}
	}
}

extern FATFS fs; 

void sd_data_recover()
{
	char *sdstr_temp;
	unsigned char i=0;
	char sdread_temp[20]={0};
	//FIL fp_reprint_r;
	unsigned char sd_buf_r[100];
	unsigned int br_repint;
	
	if(gCfgItems.fileSysType == FILE_SYS_SD)
	{
		f_mount(1, &fs);
		strcpy(path_bak, "1:");

	strcat(path_bak,path_reprint);	
	if(f_open(&fp_reprint_rw, path_bak, FA_READ)== FR_OK)
	{
		memset(sd_buf_r,0,sizeof(sd_buf_r));
		f_read(&fp_reprint_rw, sd_buf_r, 100, &br_repint);
	//sd_buf_r的每个值都与0x4D异或(0x4D对应于ASCII的M)
		
		for(i=0;i<strlen(sd_buf_r);i++)
		{
			sd_buf_r[i] = sd_buf_r[i]^0x4d; 
		}
		
		sdstr_temp = strstr(sd_buf_r,"P:");
		if(sdstr_temp)
		{
			i=0;
			while(*(sdstr_temp+2+i)!='|')
			{
				sdread_temp[i] = *(sdstr_temp+2+i);
				i++;
				if(i>20)break;
			}
			sdread_temp[i] = 0;
			RePrintData.offset = atoi(sdread_temp);
			rePrintOffset = RePrintData.offset;
			total = rePrintOffset;
		}
		
		sdstr_temp = strstr(sd_buf_r,"T0:");
		if(sdstr_temp)
		{
			i=0;
			while(*(sdstr_temp+3+i)!='|')
			{
				sdread_temp[i] = *(sdstr_temp+3+i);
				i++;
				if(i>20)break;
			}
			sdread_temp[i] = 0;
			gCfgItems.desireSprayerTemp[0] = atoi(sdread_temp);
		}	
		sdstr_temp = strstr(sd_buf_r,"T1:");
		if(sdstr_temp)
		{
			i=0;
			while(*(sdstr_temp+3+i)!='|')
			{
				sdread_temp[i] = *(sdstr_temp+3+i);
				i++;
				if(i>20)break;
			}
			sdread_temp[i] = 0;
			gCfgItems.desireSprayerTemp[1] = atoi(sdread_temp);
		}	
		sdstr_temp = strstr(sd_buf_r,"B:");
		if(sdstr_temp)
		{
			i=0;
			while(*(sdstr_temp+2+i)!='|')
			{
				sdread_temp[i] = *(sdstr_temp+2+i);
				i++;
				if(i>20)break;
			}
			sdread_temp[i] = 0;
			gCfgItems.desireBedTemp = atoi(sdread_temp);
		}	
		sdstr_temp = strstr(sd_buf_r,"FanSp:");
		if(sdstr_temp)
		{
			i=0;
			while(*(sdstr_temp+6+i)!='|')
			{
				sdread_temp[i] = *(sdstr_temp+6+i);
				i++;
				if(i>20)break;
			}
			sdread_temp[i] = 0;
			gCfgItems.fanSpeed = atoi(sdread_temp);
		}
		sdstr_temp = strstr(sd_buf_r,"FanOn:");
		if(sdstr_temp)
		{
			i=0;
			while(*(sdstr_temp+6+i)!='|')
			{
				sdread_temp[i] = *(sdstr_temp+6+i);
				i++;
				if(i>20)break;
			}
			sdread_temp[i] = 0;
			gCfgItems.fanOnoff = atoi(sdread_temp);
		}		
		sdstr_temp = strstr(sd_buf_r,"h:");
		if(sdstr_temp)
		{
			i=0;
			while(*(sdstr_temp+2+i)!='|')
			{
				sdread_temp[i] = *(sdstr_temp+2+i);
				i++;
				if(i>20)break;
			}
			sdread_temp[i] = 0;
			print_time.hours = atoi(sdread_temp);
		}
		sdstr_temp = strstr(sd_buf_r,"m:");
		if(sdstr_temp)
		{
			i=0;
			while(*(sdstr_temp+2+i)!='|')
			{
				sdread_temp[i] = *(sdstr_temp+2+i);
				i++;
				if(i>20)break;
			}
			sdread_temp[i] = 0;
			print_time.minutes= atoi(sdread_temp);
		}
		sdstr_temp = strstr(sd_buf_r,"Z:");
		if(sdstr_temp)
		{
			i=0;
			while(*(sdstr_temp+2+i)!='|')
			{
				gCfgItems.z_display_pos[i] = *(sdstr_temp+2+i);
				i++;
				if(i>20)break;
			}
			gCfgItems.z_display_pos[i] = 0;
		}
		sdstr_temp = strstr(sd_buf_r,"C:");
		if(sdstr_temp)
		{
			i=0;
			while(*(sdstr_temp+2+i)!='|')
			{
				sdread_temp[i] = *(sdstr_temp+2+i);
				i++;
				if(i>20)break;
			}
			sdread_temp[i] = 0;
			gCfgItems.curSprayerChoose = atoi(sdread_temp);
		}		
/*
		sdstr_temp = strstr(sd_buf,"File:");
		if(sdstr_temp)
		{
			i=0;
			while(*(sdstr_temp+2+i)!='\n')
			{
				sdread_temp[i] = *(sdstr_temp+2+i);
				i++;
				if(i>100)break;
			}
			sdread_temp[i] = 0;
			strcpy(sdread_temp,curFileName);
		}			
*/
		f_close(&fp_reprint_rw);

		gCfgItems.rePrintFlag = printer_pwdwn_reprint;
		RePrintData.printerStatus = printer_pwdwn_reprint;
	}
	}
	else
	{
		//f_mount(0, &fs);
		//strcpy(path_bak, "0:");
	}
}
#endif
