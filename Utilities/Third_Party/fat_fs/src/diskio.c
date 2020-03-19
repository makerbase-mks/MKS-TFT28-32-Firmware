/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "usb_core.h"
#include "usbh_msc_scsi.h"
#include "diskio.h"

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */

#define USB		0
#define MMC		1
#define ATA		2

#define SECTOR_SIZE 512U

static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

u32 buff2[1024/4];

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                     USB_Host;


extern void T_function(uint32_t x_cnt,uint8_t sel_type);
/*-----------------------------------------------------------------------*/
/* Inicializes a Drive                                                    */

DSTATUS disk_initialize (BYTE drv)    /* Physical drive nmuber (0..) */
{
 

  switch (drv) {
	case ATA :
	case MMC:
		return 0;

	case USB:
		if(HCD_IsDeviceConnected(&USB_OTG_Core))
		{  
			Stat &= ~STA_NOINIT;
		}

		break;
	}
  
  return Stat;
  
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat;
	int result;

	switch (drv) {
	case ATA :
	case MMC :		
		return 0;

	case USB :
		if (drv) 
			return STA_NOINIT;		/* Supports only single drive */
		else
			return Stat;
 			
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	//2015.11.11
	unsigned long usb_check_cnt=0;
	
	DRESULT res;
	int result;
	BYTE status = USBH_MSC_OK;

	switch (drv) {
	case ATA :
		break;

	case MMC :

		//chen 9.29
	       memset(buff2,'\n',sizeof(buff2));
		
		if(count==1)
	        {
	          //SD_ReadBlock((uint8_t*)buff2, (uint32_t)(sector << 9) ,SECTOR_SIZE);
	          SD_ReadDisk((uint8_t*)buff2, (uint32_t)sector ,1);
	          memcpy(buff,buff2,SECTOR_SIZE);
		}
		else
	        {
	          SD_ReadDisk((uint8_t*)buff2, (uint32_t)sector ,count);
	          memcpy(buff,buff2,SECTOR_SIZE * count);
		}


		return RES_OK;

	case USB :
		 if (drv || !count) 
		 	return RES_PARERR;
		 
		  if (Stat & STA_NOINIT) 
		  	return RES_NOTRDY;
		  
		  
		  if(HCD_IsDeviceConnected(&USB_OTG_Core))
		  {
				//2015.11.11
				/*
			    do
			    {
				      status = USBH_MSC_Read10(&USB_OTG_Core, buff,sector,512 * count);
				      USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);
				      
				      if(!HCD_IsDeviceConnected(&USB_OTG_Core))
				      { 
				        return RES_ERROR;
				      }  
			    }
			    while(status == USBH_MSC_BUSY );
				*/
					do
			    {
				      status = USBH_MSC_Read10(&USB_OTG_Core, buff,sector,512 * count);
				      USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);
				      
				      if(!HCD_IsDeviceConnected(&USB_OTG_Core))
				      { 
				        return RES_ERROR;
				      }  
							
							usb_check_cnt++;

							#if debug_flg == 1
							T_function(50,1);
							#endif
			    }
			    while((status == USBH_MSC_BUSY)&&(usb_check_cnt < 10000) );
		  }
			
		  if(status == USBH_MSC_OK)
		  	return RES_OK;
		  break;
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	DRESULT res;
	int result;
	BYTE status = USBH_MSC_OK;

	switch (drv) {
	case ATA :
	
		return RES_PARERR;

	case MMC :
		if(count==1)
		{
		  memcpy(buff2,buff,SECTOR_SIZE);
		  SD_WriteBlock((uint8_t*)buff2, (uint32_t)(sector << 9) ,SECTOR_SIZE);
		}
		else
		{
		  memcpy(buff2,buff,SECTOR_SIZE * count);
		  SD_WriteMultiBlocks((uint8_t*)buff2,sector << 9 ,SECTOR_SIZE,count);
		}

		return RES_OK;

	case USB :
		 if (drv || !count) return RES_PARERR;
		if (Stat & STA_NOINIT) return RES_NOTRDY;
		if (Stat & STA_PROTECT) return RES_WRPRT;


		if(HCD_IsDeviceConnected(&USB_OTG_Core))
		{  
			do
			{
				  status = USBH_MSC_Write10(&USB_OTG_Core,(BYTE*)buff,sector,512 * count);
				  USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);
				  
				  if(!HCD_IsDeviceConnected(&USB_OTG_Core))
				  { 
				    return RES_ERROR;
				  }
			}while(status == USBH_MSC_BUSY );

		}

		if(status == USBH_MSC_OK)
			return RES_OK;

		break;
	}
	return RES_PARERR;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;
	int result;

	switch (drv) {
	case ATA :

	case MMC :
	
		return 0;

	case USB :

		if (drv) 
			return RES_PARERR;
  
		  res = RES_ERROR;
		  
		  if (Stat & STA_NOINIT) return RES_NOTRDY;
		  
		  switch (ctrl) {
			  case CTRL_SYNC :		/* Make sure that no pending write process */
			    
			    res = RES_OK;
			    break;
			    
			  case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
			    
			    *(DWORD*)buff = (DWORD) USBH_MSC_Param.MSCapacity;
			    res = RES_OK;
			    break;
			    
			  case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
			    *(WORD*)buff = 512;
			    res = RES_OK;
			    break;
			    
			  case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			    
			    *(DWORD*)buff = 512;
			    
			    break;
			    
			    
			  default:
			    res = RES_PARERR;
		  }

		  break;
		  
	}
	return RES_PARERR;
}

