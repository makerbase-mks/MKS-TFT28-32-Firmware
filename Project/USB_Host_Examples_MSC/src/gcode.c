#include "gcode.h"
#include "printer.h"
#include "stdint.h"
#include "draw_ui.h"
#include "math.h"

uint8_t temp_update_flag = 0;

extern uint8_t fan_change_flag;
extern CFG_ITMES gCfgItems;
////extern PRINT_TASK_FLAG print_task_flag;
extern uint8_t gcode_cache;
extern uint8_t link_mutex_detect_time;
uint32_t rcv_ok_nums = 0;

SD_STATE  gSdState;

uint8_t  DecStr2Float(int8_t * buf,  float  *result)
{
	int  index = 0;
	
	float  retVal = 0;
	int8_t  dot_flag = 0;
	int8_t  negat_flag = 0;
		
	if(buf == 0  ||  result == 0)

	{
		return  0;
	}

	do
	{
		if((buf[index] <= '9')  &&  (buf[index]  >= '0'))
		{
			if(dot_flag)
			{
				retVal  += (float)((buf[index] - '0') * pow(10, (0 - dot_flag)));
				dot_flag++;
			}
			else
			{
				retVal  *=  10;
				retVal  += buf[index] - '0';
			}
			
		}
		else if(buf[index]  == '.')
		{
			dot_flag = 1;
		}
		else if(buf[index] == '-')
		{
				negat_flag = 1;
		}
		else
		{
				if(negat_flag)
				{
					*result = (float)0 - retVal;
				}
				else
				{
						*result = retVal;
				}
			
			return 1;
		}
		index++;
			
	} while(1);
	
}

uint8_t  DecStr2Int(int8_t * buf,  int  *result)
{
	int  index = 0;
	
	int  retVal = 0;
	int8_t  dot_flag = 0;
	int8_t  negat_flag = 0;
		
	if(buf == 0  ||  result == 0)
	{
		return  0;
	}

	do
	{
		if((buf[index] <= '9')  &&  (buf[index]  >= '0'))
		{
			
			retVal  *=  10;
			retVal  += buf[index] - '0';
			
		}
		else if(buf[index] == '-')
		{
				negat_flag = 1;
		}
		else
		{
				if(negat_flag)
				{
					*result = 0 - retVal;
				}
				else
				{
						*result = retVal;
				}
			return 1;
		}
		index++;
			
	} while(1);
	
}

void strDelChar(char *a, int32_t len, char ch)
{ 
	int i, j;
	
	for(i = 0, j = 0; i < len; i++)    
		if(a[i] != ch)        
			a[j++] = a[i];   
	
	a[j] = '\0';  
	
}

static uint8_t get_temper_flg = 0;
uint8_t FanSpeed_bak = 0;

extern char cmd_code[201];

void get_cmd_ack()
{
	
	int8_t *tmpStr = 0;
	int8_t *tmpStr_Line = 0;
	int8_t *tmpStr_upper = 0;
	int8_t *tmpStr_e = 0;
	
	float  tmpTemp = 0;
	int8_t rcv_ack_flag = 0;
	int32_t i, j, k;
	int8_t inc_flag = 0;
	int8_t num_valid = 0;
	int8_t cmdRxBuf[128] = {0};
	int8_t  tempBuf[100] = {0};
	
	memset(cmdRxBuf, 0, sizeof(cmdRxBuf));

	if(popFIFO(&gcodeCmdRxFIFO,  cmdRxBuf) == fifo_ok)
	{	
		link_mutex_detect_time = 0;
		get_temper_flg = 0;
		i = 0;
		
		#if 1
		if(gCfgItems.display_error != 0)
		{
			tmpStr_Line = (int8_t *)strstr(&cmdRxBuf[i], "Error:Line Number");
			tmpStr = (int8_t *)strstr(&cmdRxBuf[i], "Error:");
			if((tmpStr)&&(!tmpStr_Line))
			{
				memset(cmd_code,0,sizeof(cmd_code));
				strcpy(cmd_code,cmdRxBuf);
				if(disp_state != DIALOG_UI)
					clear_cur_ui();
				draw_dialog(DIALOG_TYPE_MAINBOAR_ERROR);
			}
		}
		/*
		tmpStr = (int8_t *)strstr(&cmdRxBuf[i], "error:");
		if(tmpStr)
		{
			memset(cmd_code,0,sizeof(cmd_code));
			strcpy(cmd_code,cmdRxBuf);
			if(disp_state != DIALOG_UI)
				clear_cur_ui();
			draw_dialog(DIALOG_TYPE_MAINBOAR_ERROR);
		}
		*/
		#endif
		tmpStr = (int8_t *)strstr(&cmdRxBuf[i], "T:");
		if( tmpStr)
		{
			if(printer_state == PRINTER_NOT_CONNECT)
			{
				printer_state = PRINTER_CONNECT_OK;
			}
			if((*(tmpStr+2) =='i')&&(*(tmpStr+3) =='n')&&(*(tmpStr+4) =='f'))
			{
				gCfgItems.curSprayerTemp[0] = -50;
			}
			else
			{
			memset(tempBuf, 0, sizeof(tempBuf));
			k = 0;
			num_valid = 0;
			for(j = 2; tmpStr[j] != ' '; j++)
			{
				
				if(tmpStr[j] == '\0')
				{
					break;
				}
				
				tempBuf[k] = tmpStr[j];
				num_valid = 1;
				k++;
			}
			if(num_valid)
			{
				if(DecStr2Float(tempBuf, &tmpTemp)	 !=   0)//当前温度
				{
					//if((int)tmpTemp != 0)
					{
						if(gCfgItems.sprayerNum == 1)
						{
							gCfgItems.curSprayerTemp[0] = tmpTemp;
							if(gCfgItems.curSprayerTemp[0] > 999)//限定液晶显示只有3位
							{
								gCfgItems.curSprayerTemp[0] = gCfgItems.curSprayerTempBak[0];
								
							}
							else
							{
								gCfgItems.curSprayerTempBak[0] = gCfgItems.curSprayerTemp[0];
							}
						}
						else
						{
								tmpStr_e = (int8_t *)strstr(&cmdRxBuf[i], "E:");
								if(tmpStr_e)
								{
									if(*(tmpStr_e+2) =='0')
									{
										gCfgItems.curSprayerTemp[0] = tmpTemp;
										if(gCfgItems.curSprayerTemp[0] > 999)//限定液晶显示只有3位
										{
											gCfgItems.curSprayerTemp[0] = gCfgItems.curSprayerTempBak[0];
											
										}
										else
										{
											gCfgItems.curSprayerTempBak[0] = gCfgItems.curSprayerTemp[0];
										}
									}
									else if(*(tmpStr_e+2) =='1')
									{
										gCfgItems.curSprayerTemp[1] = tmpTemp;
										if(gCfgItems.curSprayerTemp[1] > 999)//限定液晶显示只有3位
										{
											gCfgItems.curSprayerTemp[1] = gCfgItems.curSprayerTempBak[1];
											
										}
										else
										{
											gCfgItems.curSprayerTempBak[1] = gCfgItems.curSprayerTemp[1];
										}												
									}
								}
								else
								{
									gCfgItems.curSprayerTemp[0] = tmpTemp;
									if(gCfgItems.curSprayerTemp[0] > 999)//限定液晶显示只有3位
									{
										gCfgItems.curSprayerTemp[0] = gCfgItems.curSprayerTempBak[0];
									}
									else
									{
										gCfgItems.curSprayerTempBak[0] = gCfgItems.curSprayerTemp[0];
									}
								}
						}
						
						temp_update_flag = 1;
					}
				}
				if(tmpStr[j + 1] == '/')
				{
					j += 2;
					memset(tempBuf, 0, sizeof(tempBuf));
					k = 0;
					num_valid = 0;
					for(; tmpStr[j] != ' '; j++)
					{
						
						if(tmpStr[j] == '\0')
						{
							break;
						}
						tempBuf[k] = tmpStr[j];
						num_valid = 1;
						k++;
						
					}
					if(num_valid)
					{
						if(DecStr2Float(tempBuf, &tmpTemp)	 !=   0)//目标温度
						{
							//if((int)tmpTemp != 0)
							{
								if(gCfgItems.sprayerNum == 1)
								{
									gCfgItems.desireSprayerTemp[0] = tmpTemp;
									if(gCfgItems.desireSprayerTemp[0] > 999)//限定液晶显示只有3位
									{
										gCfgItems.desireSprayerTemp[0] = gCfgItems.desireSprayerTempBak[0];
									}
									else
									{
										gCfgItems.desireSprayerTempBak[0] = gCfgItems.desireSprayerTemp[0];
									}
									
									temp_update_flag = 1;		
								}
								else
								{
									tmpStr_e = (int8_t *)strstr(&cmdRxBuf[i], "E:");
									if(tmpStr_e)
									{
										if(*(tmpStr_e+2) =='0')
										{
											gCfgItems.desireSprayerTemp[0] = tmpTemp;
											if(gCfgItems.desireSprayerTemp[0] > 999)//限定液晶显示只有3位
											{
												gCfgItems.desireSprayerTemp[0] = gCfgItems.desireSprayerTempBak[0];
											}
											else
											{
												gCfgItems.desireSprayerTempBak[0] = gCfgItems.desireSprayerTemp[0];
											}
											temp_update_flag = 1;	
										}
										else	if(*(tmpStr_e+2) =='1')
										{
											gCfgItems.desireSprayerTemp[1] = tmpTemp;
											if(gCfgItems.desireSprayerTemp[1] > 999)//限定液晶显示只有3位
											{
												gCfgItems.desireSprayerTemp[1] = gCfgItems.desireSprayerTempBak[1];
											}
											else
											{
												gCfgItems.desireSprayerTempBak[1] = gCfgItems.desireSprayerTemp[1];
											}
											temp_update_flag = 1;	
										}
									}
									else
									{
											gCfgItems.desireSprayerTemp[0] = tmpTemp;
											if(gCfgItems.desireSprayerTemp[0] > 999)//限定液晶显示只有3位
											{
												gCfgItems.desireSprayerTemp[0] = gCfgItems.desireSprayerTempBak[0];
											}
											else
											{
												gCfgItems.desireSprayerTempBak[0] = gCfgItems.desireSprayerTemp[0];
											}
											temp_update_flag = 1;	
									}
								}
							}
						}
					}
				}
			}
			}
		}		


		tmpStr = (int8_t *)strstr(&cmdRxBuf[i], "B:");
		if(tmpStr)
		{
				if(printer_state == PRINTER_NOT_CONNECT)
				{
					printer_state = PRINTER_CONNECT_OK;
				}
				if((*(tmpStr+2) =='i')&&(*(tmpStr+3) =='n')&&(*(tmpStr+4) =='f'))
				{
					gCfgItems.curBedTemp = -50;
				}
				else
				{
				memset(tempBuf, 0, sizeof(tempBuf));
				k = 0;
				num_valid = 0;
				for(j = 2; tmpStr[j] != ' '; j++)
				{
					
					if(tmpStr[j] == '\0')
					{
						break;
					}
					tempBuf[k] = tmpStr[j];
					num_valid = 1;
					k++;
					
				}
				if(num_valid)
				{
					if(DecStr2Float(tempBuf, &tmpTemp)	 != 	0)
					{
						//if((int)tmpTemp != 0)
						{
							gCfgItems.curBedTemp = tmpTemp;
							if(gCfgItems.curBedTemp > 999)//限定液晶显示只有3位
							{
								gCfgItems.curBedTemp = gCfgItems.curBedTempBak;
							}
							else
							{
								gCfgItems.curBedTempBak = gCfgItems.curBedTemp;
							}
							temp_update_flag = 1; 						
						}
		
					}
					if(tmpStr[j + 1] == '/')
					{
						j += 2;
						memset(tempBuf, 0, sizeof(tempBuf));
						k = 0;
						num_valid = 0;
						for(; tmpStr[j] != ' '; j++)
						{
							
							if(tmpStr[j] == '\0')
							{ 							
								break;
							}
							tempBuf[k] = tmpStr[j];
							num_valid = 1;
							k++;
								
						}
						if(num_valid)
						{
							if(DecStr2Float(tempBuf, &tmpTemp)	!=	 0)
							{
								//if((int)tmpTemp != 0)
								{
									gCfgItems.desireBedTemp = tmpTemp;
									if(gCfgItems.desireBedTemp > 999)//限定液晶显示只有3位
									{
										gCfgItems.desireBedTemp = gCfgItems.desireBedTempBak;
									}
									else
									{
										gCfgItems.desireBedTempBak = gCfgItems.desireBedTemp;
									} 									
									temp_update_flag = 1; 									
								}
							}
						}
					}
				}
			}
		}
		
			tmpStr = (int8_t *)strstr(&cmdRxBuf[i], "T0:");
			if( tmpStr)
			{
				if(printer_state == PRINTER_NOT_CONNECT)
				{
					printer_state = PRINTER_CONNECT_OK;
				}
				if((*(tmpStr+2) =='i')&&(*(tmpStr+3) =='n')&&(*(tmpStr+4) =='f'))
				{
					gCfgItems.curSprayerTemp[0] = -50;
				}
				else
				{		
				memset(tempBuf, 0, sizeof(tempBuf));
				k = 0;
				num_valid = 0;
				for(j = 3; tmpStr[j] != ' '; j++)
				{
					
					if(tmpStr[j] == '\0')
					{
						break;
					}
					
					tempBuf[k] = tmpStr[j];
					num_valid = 1;
					k++;
					
				}
				if(num_valid)
				{
					if(DecStr2Float(tempBuf, &tmpTemp)	 != 	0)
					{
						//if((int)tmpTemp != 0)
						{
							gCfgItems.curSprayerTemp[0] = tmpTemp;
							if(gCfgItems.curSprayerTemp[0] > 999)//限定液晶显示只有3位
							{
								gCfgItems.curSprayerTemp[0] = gCfgItems.curSprayerTempBak[0];
							}
							else
							{
								gCfgItems.curSprayerTempBak[0] = gCfgItems.curSprayerTemp[0]; 
							}
							temp_update_flag = 1;
						}
					}
					if(tmpStr[j + 1] == '/')
					{
						j += 2;
						memset(tempBuf, 0, sizeof(tempBuf));
						k = 0;
						num_valid = 0;
						for(; tmpStr[j] != ' '; j++)
						{
							
							if(tmpStr[j] == '\0')
							{
								break;
							}
							tempBuf[k] = tmpStr[j];
							num_valid = 1;
							k++;
							
						}
						if(num_valid)
						{
							if(DecStr2Float(tempBuf, &tmpTemp)	 != 	0)
							{
								//if((int)tmpTemp != 0)
								{
									gCfgItems.desireSprayerTemp[0] = tmpTemp;
									if(gCfgItems.desireSprayerTemp[0] > 999)//限定液晶显示只有3位
									{
										gCfgItems.desireSprayerTemp[0] = gCfgItems.desireSprayerTempBak[0];
									}
									else
									{
										gCfgItems.desireSprayerTempBak[0] = gCfgItems.desireSprayerTemp[0]; 
									} 										
									temp_update_flag = 1;
								}
		
							}
						}
					}
				}
			}
		}
			
			tmpStr = (int8_t *)strstr(&cmdRxBuf[i], "T1:");
			if( tmpStr)
			{
				if(printer_state == PRINTER_NOT_CONNECT)
				{
					printer_state = PRINTER_CONNECT_OK;
				}
				if((*(tmpStr+2) =='i')&&(*(tmpStr+3) =='n')&&(*(tmpStr+4) =='f'))
				{
					gCfgItems.desireSprayerTemp[1] = -50;
				}
				else
				{	

				memset(tempBuf, 0, sizeof(tempBuf));
				k = 0;
				num_valid = 0;
				for(j = 3; tmpStr[j] != ' '; j++)
				{
					
					if(tmpStr[j] == '\0')
					{
						break;
					}
					
					tempBuf[k] = tmpStr[j];
					num_valid = 1;
					k++;
					
				}
				if(num_valid)
				{
					if(DecStr2Float(tempBuf, &tmpTemp)	 !=   0)
					{
						//if((int)tmpTemp != 0)
						{
							gCfgItems.curSprayerTemp[1] = tmpTemp;
							if(gCfgItems.curSprayerTemp[1] > 999)//限定液晶显示只有3位
							{
								gCfgItems.curSprayerTemp[1] = gCfgItems.curSprayerTempBak[1];
							}
							else
							{
								gCfgItems.curSprayerTempBak[1] = gCfgItems.curSprayerTemp[1];			
							}
							temp_update_flag = 1;
						}
					}
					if(tmpStr[j + 1] == '/')
					{
						j += 2;
						memset(tempBuf, 0, sizeof(tempBuf));
						k = 0;
						num_valid = 0;
						for(; tmpStr[j] != ' '; j++)
						{
							
							if(tmpStr[j] == '\0')
							{
								break;
							}
							tempBuf[k] = tmpStr[j];
							num_valid = 1;
							k++;
							
						}
						if(num_valid)
						{
							if(DecStr2Float(tempBuf, &tmpTemp)	 !=   0)
							{
								//if((int)tmpTemp != 0)
								{
									gCfgItems.desireSprayerTemp[1] = tmpTemp;
									if(gCfgItems.desireSprayerTemp[1] > 999)//限定液晶显示只有3位
									{
										gCfgItems.desireSprayerTemp[1] = gCfgItems.desireSprayerTempBak[1];
									}
									else
									{
										gCfgItems.desireSprayerTempBak[1] = gCfgItems.desireSprayerTemp[1];			
									}											
									temp_update_flag = 1;
								}

							}
						}
					}
				}
			}
		}
				
			tmpStr = (int8_t *)strstr(&cmdRxBuf[i], "M107");
			if( tmpStr)
			{
				//TAN
				FanSpeed_bak = gCfgItems.fanSpeed;
				gCfgItems.fanOnoff = 0;
				gCfgItems.fanSpeed = 0;
				fan_change_flag = 1;
			}

			tmpStr = (int8_t *)strstr(&cmdRxBuf[i], "M106 S");
			if( tmpStr)
			{
				j= strlen("M106 S");

				k = 0;
				memset(tempBuf, 0, sizeof(tempBuf));
				num_valid = 0;
				for(; tmpStr[j] != '\0'; j++)
				{
					
					tempBuf[k] = tmpStr[j];
					num_valid = 1;
					k++;
					
				}
				if(num_valid)
				{
					int tmpInt = 0;
					if(DecStr2Int(tempBuf, &tmpInt)	 !=   0)
					{
						//TAN
						FanSpeed_bak = gCfgItems.fanSpeed;
						
						if(tmpInt == 0)
						{
							gCfgItems.fanOnoff = 0;
							gCfgItems.fanSpeed = 0;
						}
						else
						{
							gCfgItems.fanOnoff  = 1;
							gCfgItems.fanSpeed = tmpInt;
						}
						fan_change_flag = 1;
					}
				}
			}
		}
}



