#include "fs_usr.h"
#include "ff.h"

FATFS fs; 
void mount_file_sys(uint8_t disk_type)
{
	if(disk_type == DISK_SD)
	{
		f_mount(1, &fs);
	}
	else if(disk_type == DISK_USB)
	{
		f_mount(0, &fs);
	}
}

