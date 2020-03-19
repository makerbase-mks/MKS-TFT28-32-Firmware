#ifndef _PRINTING_THREAD_H_
#define _PRINTING_THREAD_H_


#define GCODE_BUFFER_SIZE	1024





typedef struct
{
	FIL curFile;
	uint32_t fileSize;
	uint16_t bytesHaveRead;
	uint32_t totalRead;
	uint32_t totalSend;
	//uint8_t Gcode_Buf[GCODE_BUFFER_SIZE];
	uint16_t bufPoint;
	uint8_t file_open_flag;
	
} FILE_PRINT_STATE;

extern PRINT_TASK_FLAG print_task_flag;
extern FILE_PRINT_STATE gCurFileState;




#endif

