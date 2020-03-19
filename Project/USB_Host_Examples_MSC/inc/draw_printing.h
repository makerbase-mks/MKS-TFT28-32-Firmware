#ifndef _DRAW_PRINTING_H_
#define _DRAW_PRINTING_H_
#include "stdint.h"



extern void draw_printing();
extern void Clear_printing();
extern void setProBarValue(int added);
extern void disp_print_time();
extern void disp_printing_speed();
extern void disp_sprayer_tem_printing();
extern void disp_bed_temp_printing();
extern void disp_fan_speed_printing();
extern void reset_print_time();
extern void setProBarRate(int rate);
extern void disp_fan_move_printing();
extern void start_print_time();
extern void stop_print_time();
#endif

