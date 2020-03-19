/********************   (C) COPYRIGHT 2014 www.makerbase.com.cn   ********************
 * 文件名  ：mks_tft_gcode.c
 * 描述    ：1.从u盘读取源文件，每次读取1k字节，轮流写入udiskBuffer.buffer[0]和udiskBuffer.buffer[1]；
 						2. 当udiskBuffer.buffer[n]非空时，提取出有效gcode指令，加上前/后缀后,Push到gcodeTxFIFO队列。
 * 作者    ：skyblue
**********************************************************************************/

#ifndef MKS_TFT_GCODE_H
#define MKS_TFT_GCODE_H
#include "ff.h"

#ifdef __cplusplus
 extern "C" {
#endif

struct position
{
	unsigned long Gcode_LineNumb;
	unsigned long Gcode_fileOffset;
};


#define UDISKBUFLEN 1024		//存储读取U盘文件的数据长度

typedef enum
{
	udisk_file_ok = 0,
	udisk_file_end,
}UDISK_FILE_STAUS;

typedef enum
{
	udisk_buf_empty = 0,
	udisk_buf_full,	
} UDISK_DATA_BUFFER_STATE;


typedef struct			
{
	unsigned char buffer[2][UDISKBUFLEN];		//两个BUFFER轮流从U盘读取文件
	unsigned char *p;
	unsigned char current;					//指示当前非读取文件的BUFFER
	UDISK_DATA_BUFFER_STATE state[2];		//指示buffer状态
}UDISK_DATA_BUFFER;			//

typedef enum
{
	temp_fail = 0,
	temp_ok,
} TEMP_STATUS;

typedef struct{
	char just_delay_one;
	char read_disk_err;
}RECOVER_SD;
extern volatile RECOVER_SD rec_sd;

extern unsigned char note_flag;

extern UDISK_DATA_BUFFER udiskBuffer;
extern unsigned long gcodeLineCnt;
extern UDISK_FILE_STAUS udiskFileStaus;	

extern void udiskBufferInit(void);
extern void udiskFileR(FIL *srcfp);
extern void pushTxGcode(void);
extern void sd_saved_data();
extern void sd_data_recover();
extern void Restart_data_init();

#ifdef __cplusplus
}
#endif
#endif
