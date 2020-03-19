#ifndef _GCODE_H_
#define _GCODE_H_

/*Gcode command define*/
#define	MOVE_X_COMMAN(buf, x, v)		sprintf(buf, "G1 X%3.1f F%d\n", x, v)
#define	MOVE_Y_COMMAN(buf, y, v)		sprintf(buf, "G1 Y%3.1f F%d\n", y, v)
#define	MOVE_Z_COMMAN(buf, z, v)		sprintf(buf, "G1 Z%3.1f F%d\n", z, v)
#define	MOVE_E_COMMAN(buf, e, v)		sprintf(buf, "G1 E%d F%d\n", e, v)

#define	BABY_STEP_X_COMMAN(buf, x)		sprintf(buf, "M290 X%.2f\n", x)
#define	BABY_STEP_Y_COMMAN(buf, y)		sprintf(buf, "M290 Y%.2f\n", y)
#define	BABY_STEP_Z_COMMAN(buf, z)		sprintf(buf, "M290 Z%.2f\n", z)

#define	SET_EXTRUD_TEM_COMMAND(buf, t)	sprintf(buf, "M104 S%.1f\n", t)
#define	SET_BED_TEM_COMMAND(buf, t)	sprintf(buf, "M140 S%.1f\n", t)
#define	SET_FAN_SPEED_COMMAND(buf, sp)	sprintf(buf, "M106 S%d\n", sp)
#define	SET_MOVE_SPEED_COMMAND(buf, rate)	sprintf(buf, "M220 S%d\n", rate)
#define	SET_EXTRU_SPEED_COMMAND(buf, rate)	sprintf(buf, "M221 S%d\n", rate)

#define	HOME_X_COMMAND				"G28 X0\n"
#define	HOME_Y_COMMAND				"G28 Y0\n"
#define	HOME_Z_COMMAND				"G28 Z0\n"
#define	HOME_A_COMMAND				"G28\n"
#define	ABSOLUTE_COORD_COMMAN		"G90\n"
#define	RELATIVE_COORD_COMMAN		"G91\n"

#define	GET_CUR_TEM_COMMAND			"M105\n"
#define	SET_FAN_OFF_COMMAND			"M107\n"
#define	WAIT_EXTRUD_TEM_COMMAND	"M109\n"
#define	DISP_CUR_POS_COMMAND		"M114\n"
//HC-chen
#define   M_OFF_COMMAND                           "M84\n"
#define	SAVE_EEPROM_COMMAND			"M500\n"
#define	GET_BABY_ZOFFSET_COMMAND  "M851\n"
#define	GET_BABY_STEP_VALUE_COMMAND  "M290\n"




extern void get_cmd_ack();


#endif

