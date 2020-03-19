/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_usr.h"
#include "lcd_log.h"
#include "ff.h"       /* FATFS */
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "printer.h"
#include "printing_thread.h"

extern USBH_HOST                     USB_Host;


/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_MSC_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the usb host stack user callbacks
* @{
*/ 

/** @defgroup USBH_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Defines
* @{
*/ 
#define IMAGE_BUFFER_SIZE    512

/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Macros
* @{
*/ 
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;

extern uint8_t trig_browser_flag;

extern FILE_PRINT_STATE gCurFileState ;
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Variables
* @{
*/ 
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
uint8_t filenameString[15]  = {0};
uint8_t gCurDir[100] = {0};

extern FATFS fs;
FIL file;
//uint8_t Image_Buf[IMAGE_BUFFER_SIZE];
uint8_t line_idx = 0;   




fileNameList gcodeFileList;

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
    
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_HOST_INIT[]        = "> Host Library Initialized\n";
const uint8_t MSG_DEV_ATTACHED[]     = "> Device Attached \n";
const uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected\n";
const uint8_t MSG_DEV_ENUMERATED[]   = "> Enumeration completed \n";
const uint8_t MSG_DEV_HIGHSPEED[]    = "> High speed device detected\n";
const uint8_t MSG_DEV_FULLSPEED[]    = "> Full speed device detected\n";
const uint8_t MSG_DEV_LOWSPEED[]     = "> Low speed device detected\n";
const uint8_t MSG_DEV_ERROR[]        = "> Device fault \n";

const uint8_t MSG_MSC_CLASS[]        = "> Mass storage device connected\n";
const uint8_t MSG_HID_CLASS[]        = "> HID device connected\n";
const uint8_t MSG_DISK_SIZE[]        = "> Size of the disk in MBytes: \n";
const uint8_t MSG_LUN[]              = "> LUN Available in the device:\n";
const uint8_t MSG_ROOT_CONT[]        = "> Exploring disk flash ...\n";
const uint8_t MSG_WR_PROTECT[]       = "> The disk is write protected\n";
const uint8_t MSG_UNREC_ERROR[]      = "> UNRECOVERED ERROR STATE\n";

/**
* @}
*/


/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/
int8_t gFile_Browser (char* path);

/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Functions
* @{
*/ 

 void usb_delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
}
/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
  static uint8_t startup = 0;  
  
  if(startup == 0 )
  {
    startup = 1;
		#if 0
    /* Configure the LEDs */
    STM_EVAL_LEDInit(LED1);
    STM_EVAL_LEDInit(LED2);
    STM_EVAL_LEDInit(LED3); 
    STM_EVAL_LEDInit(LED4); 
    
    STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);
		#endif
		
 #ifdef USB_LCD_LOG  
#if defined (USE_STM322xG_EVAL)
  STM322xG_LCD_Init();
#elif defined(USE_STM324xG_EVAL)
  STM324xG_LCD_Init();
#elif defined (USE_STM3210C_EVAL)
  SZ_STM32_LCDInit();
#else
 #error "Missing define: Evaluation board (ie. USE_STM322xG_EVAL)"
#endif

    
    LCD_LOG_Init();
   
#ifdef USB_LCD_LOG    
#ifdef USE_USB_OTG_HS 
    LCD_LOG_SetHeader(" USB OTG HS MSC Host");
#else
    LCD_LOG_SetHeader(" USB OTG FS MSC Host");
#endif
#endif
    LCD_UsrLog("> USB Host library started.\n"); 
    LCD_LOG_SetFooter (" ARMJISHU.COM USB Host Library v2.1.0" );
    
 #endif
//tan 20170112
//		xprintf("\n ==> ARMJISHU神舟STM32开发板，USB HOST实验之U盘的访问 <==");  
//    xprintf("\n ==> USB读取U盘目录，LCD显示/Media/文件夹中的BMP图片. <==\n");
  }
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
 #ifdef USB_LCD_LOG  
  LCD_UsrLog((void *)MSG_DEV_ATTACHED);
 #endif
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
   #ifdef USB_LCD_LOG  
  /* Set default screen color*/ 
  LCD_ErrLog((void *)MSG_UNREC_ERROR); 
   #endif
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected (void)
{
  
#ifdef USB_LCD_LOG
  LCD_LOG_ClearTextZone();
  
  LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 42, "                                      ");
  LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 30, "                                      ");  
  
  /* Set default screen color*/
  LCD_ErrLog((void *)MSG_DEV_DISCONNECTED);
  #endif
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
  /* callback for USB-Reset */
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {
   #ifdef USB_LCD_LOG  
    LCD_UsrLog((void *)MSG_DEV_HIGHSPEED);
   #endif
  }  
  else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
   #ifdef USB_LCD_LOG  
    LCD_UsrLog((void *)MSG_DEV_FULLSPEED);
   #endif
  }
  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
   #ifdef USB_LCD_LOG  
    LCD_UsrLog((void *)MSG_DEV_LOWSPEED);
   #endif
  }
  else
  {
   #ifdef USB_LCD_LOG  
    LCD_UsrLog((void *)MSG_DEV_ERROR);
   #endif
  }
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
  USBH_DevDesc_TypeDef *hs;
  hs = DeviceDesc;  
  
   #ifdef USB_LCD_LOG  
  LCD_UsrLog("VID : %04Xh\n" , (uint32_t)(*hs).idVendor); 
  LCD_UsrLog("PID : %04Xh\n" , (uint32_t)(*hs).idProduct); 
  #endif
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
  
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
  USBH_InterfaceDesc_TypeDef *id;
  
  id = itfDesc;  
  
  if((*id).bInterfaceClass  == 0x08)
  {
   #ifdef USB_LCD_LOG  
    LCD_UsrLog((void *)MSG_MSC_CLASS);
   #endif
  }
  else if((*id).bInterfaceClass  == 0x03)
  {
   #ifdef USB_LCD_LOG  
    LCD_UsrLog((void *)MSG_HID_CLASS);
   #endif
  }    
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
 #ifdef USB_LCD_LOG  
  LCD_UsrLog("Manufacturer : %s\n", (char *)ManufacturerString);
 #endif
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
 #ifdef USB_LCD_LOG  
  LCD_UsrLog("Product : %s\n", (char *)ProductString);  
 #endif
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
 #ifdef USB_LCD_LOG  
  LCD_UsrLog( "Serial Number : %s\n", (char *)SerialNumString);    
 #endif
} 



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
  
  /* Enumeration complete */
   #ifdef USB_LCD_LOG  
  LCD_UsrLog((void *)MSG_DEV_ENUMERATED);
   
  
  LCD_SetTextColor(Green);
  LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 42, "To see the root content of the disk : " );
  LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 30, "Press Key2...                       ");
   LCD_SetTextColor(LCD_LOG_DEFAULT_COLOR); 
  #endif
	//tan 20170112
  //xprintf("\n To see the root content of the disk : Press Key2..." );
 
  
} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
 #ifdef USB_LCD_LOG
  LCD_ErrLog ("> Device not supported."); 
 #endif
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
  USBH_USR_Status usbh_usr_status;
  
  usbh_usr_status = USBH_USR_NO_RESP;  
  
	#if 0
  /*Key B3 is in polling mode to detect user action */
  if(STM_EVAL_PBGetState(Button_KEY) == RESET) 
  {
    
   
    
  } 
	#endif
	 usbh_usr_status = USBH_USR_RESP_OK;

  return usbh_usr_status;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void)
{
 #ifdef USB_LCD_LOG
  LCD_ErrLog ("Overcurrent detected.");
 #endif
}


/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void)
{
  FRESULT res;
  uint8_t writeTextBuff[] = " WWW.ARMJISHU.COM  SZ-IV-STM32F107VC \r\n STM32 Connectivity line Host Demo application using FAT_FS   ";
  uint16_t bytesWritten, bytesToWrite;
  
  switch(USBH_USR_ApplicationState)
  {
  case USH_USR_FS_INIT: 
    
    /* Initialises the File System*/
    if ( f_mount( 0, &fs ) != FR_OK ) 
    {
      /* efs initialisation fails*/
#ifdef USB_LCD_LOG
		LCD_ErrLog("> Cannot initialize File System.\n");
#endif
      return(-1);
    }
#ifdef USB_LCD_LOG
    LCD_UsrLog("> File System initialized.\n");
    LCD_UsrLog("> Disk capacity : %d M Bytes\n", USBH_MSC_Param.MSCapacity * \
      USBH_MSC_Param.MSPageLength/1024/1024); 
#endif
    
    if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
    {
#ifdef USB_LCD_LOG
      LCD_ErrLog((void *)MSG_WR_PROTECT);
#endif
    }
    
    USBH_USR_ApplicationState = USH_USR_FS_READLIST;
    break;
    
  case USH_USR_FS_READLIST:
#ifdef USB_LCD_LOG
    LCD_UsrLog((void *)MSG_ROOT_CONT);
#endif
   // Explore_Disk("0:/", 1);
 //  if(trig_browser_flag == 1)
 //  	gFile_Browser(gCurDir);

 //  if(trig_browser_flag == 2)
  // 	f_read(srcfp,udiskBuffer.buffer[(udiskBuffer.current+1)%2],UDISKBUFLEN,&readByteCnt);
					
    line_idx = 0;   
   // USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;
    
    break;
    
  default: break;
  }
  return(0);
}

extern uint32_t Sd_file_cnt;
extern uint32_t Sd_file_offset;

int8_t gFile_Browser (char* path)
{
	FRESULT res;
	uint8_t ret = 1;
	FILINFO fno;
	DIR dir;
	char *fn;
	char tmp[50] = {0};

#if _USE_LFN
	static char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
#endif

	gcodeFileList.listVaild= 2;
	if(USBH_USR_ApplicationState != USH_USR_FS_READLIST)
	{
	
		return -1;
	}
	
	res = f_opendir(&dir, path);
	if (res == FR_OK)
	{
	  	Sd_file_cnt = 0;
		

		for (;;) 
		{
			res = f_readdir(&dir, &fno);
			
			if (res != FR_OK || fno.fname[0] == 0) 
			{
				gcodeFileList.listVaild= 2;
				break;
			}
			if ( fno.fname[0] == '.') 
				continue;			
			if ( fno.lfname[0] == '.') 
				continue;

			if ((fno.lfname[0] == 0) || (fno.lfname == 0))
				fn = fno.fname;
			else
				fn = fno.lfname;

		/*   if (fno.fattrib & AM_DIR) 
		  {
		    continue;
		  } 
		  else */
		  if(Sd_file_cnt == Sd_file_offset)
		  {
		    if((strstr(fn, ".gco")) || (strstr(fn, ".GCO")) || (fno.fattrib & AM_DIR))
		    {
				tmp[0] = '\0';

				if(strlen(path) >= sizeof(tmp))
				{
					break;
				}
				strcpy(tmp, path);
				strcat(tmp, "/");
				strcat(tmp, fn);

				if(fno.fattrib & AM_DIR)
				{
					gcodeFileList.fileAttr[gcodeFileList.index] = 1;
				}
				else
				{
					gcodeFileList.fileAttr[gcodeFileList.index] = 0;
				}

				if(strlen(tmp) >= sizeof(gcodeFileList.fileName[gcodeFileList.index]))
				{
					break;
				}
				
				strcpy(gcodeFileList.fileName[gcodeFileList.index], tmp);
				gcodeFileList.listVaild= 1;
				gcodeFileList.index++;
				/*
				if(gcodeFileList.index >= MAX_FILES_ONE_LEVER)
				{
					gcodeFileList.listVaild= 1;
					break;
				}
				*/
				// GUI_DispStringAt(tmp, 10, 100);	  

				//USB_OTG_BSP_mDelay(100);
			}    
			else
			{
				gcodeFileList.listVaild= 0;
			}
			
		    break;		      
		  }
			Sd_file_cnt++;
		}
	}  
	else
	{
		gcodeFileList.listVaild= 2;
	}
	
//	USBH_USR_ApplicationState = USH_USR_FS_READLIST;
	return ret;
}


/**
* @brief  Toggle_Leds
*         Toggle leds to shows user input state
* @param  None
* @retval None
*/

/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}

void reset_usb_state()
{
	USB_Host.gState = HOST_DEV_DISCONNECTED;	
}

int8_t get_printing_rate(FIL *fileHandle)
{
	int8_t rate;
	
	if(fileHandle == 0)
		return -1;

	if(fileHandle->fptr == 0)
		return -1;

	if(fileHandle->fsize == 0)
		return -1;

	if(fileHandle->fptr > fileHandle->fsize)
		return -1;

	rate = (unsigned char)((float)f_tell(fileHandle)  * 100 / f_size(fileHandle));

	if((rate == 99) && (printerStaus == pr_idle))	
	{
		rate = 100;
	}

	gCurFileState.totalSend = rate;

	return  rate;

}


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

