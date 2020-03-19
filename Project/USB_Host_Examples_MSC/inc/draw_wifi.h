#ifndef _DRAW_WIFI_H_
#define _DRAW_WIFI_H_
#include "stdint.h"

extern void draw_Wifi();
extern void Clear_Wifi();
extern void disp_wifi_state();
extern void disp_wifi_enable(uint8_t state);

#define wifiEnble    1
#define wifiDisable 0
#endif
