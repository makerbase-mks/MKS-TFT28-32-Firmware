#ifndef _DRAW_PRINT_FILE_H_
#define _DRAW_PRINT_FILE_H_

typedef struct 
{
	int cur_page_first_offset;
	int cur_page_last_offset;
	int curPage;
} DIR_OFFSET;

extern void draw_print_file();
extern void search_files();
extern void disp_udisk_files(int seq);

#endif

