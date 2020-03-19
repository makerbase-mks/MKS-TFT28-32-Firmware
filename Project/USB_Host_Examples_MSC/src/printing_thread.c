#include "usbh_usr.h"
#include "draw_ui.h"
#include "printer.h"
#include "printing_thread.h"



PRINT_TASK_FLAG print_task_flag = TASK_RESET;

FILE_PRINT_STATE gCurFileState ;

void reset_file_info()
{
	gCurFileState.fileSize = 0;
	gCurFileState.totalRead = 0;
	gCurFileState.totalSend = 0;
	gCurFileState.bytesHaveRead = 0;
	gCurFileState.bufPoint = 0;

	gCurFileState.file_open_flag = 0;

//	memset(gCurFileState.Gcode_Buf, 0, sizeof(gCurFileState.Gcode_Buf));
}

int8_t  noteFlag = 0;
uint32_t ValidBytesToSend = 0;



PRINT_TASK_FLAG getPrintTaskState()
{
	return print_task_flag;
}


