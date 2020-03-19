#include "stdint.h"
#include "string.h"
#include "stm32f10x_gpio.h"
#include "SZ_STM32F107VC_LIB.h"
#include "mks_tft_com.h"
#include "Printer.h"
#include "draw_ui.h"
//#include "usb_core.h"
#include "wifi_module.h"
#include "sd_usr.h"
#include "Gcode.h"
#include "wifi_upload.h"

#define WAIT_ESP_TRANS_TIMEOUT_TICK	10500

int cfg_wifi_flag = 0;
int cfg_cloud_flag = 0;

extern PRINT_TIME print_time;
extern FIL *srcfp;

char wifi_firm_ver[20] = {0};

uint8_t wifi_connect_flg = 0;
extern volatile uint8_t get_temp_flag;


#define WIFI_MODE	2	//WIFI MODE
#define WIFI_AP_MODE	3//AP??

int upload_result = 0; //0:传输未启动；1:传输进行中；2:传输异常，中止；3:传输完成。

uint32_t upload_time = 0; //传输时间
uint32_t upload_size = 0; //传输大小

volatile WIFI_STATE wifi_link_state;
WIFI_PARA wifiPara;
IP_PARA ipPara;
CLOUD_PARA cloud_para;

char wifi_loop_time = 0;
char wifi_check_time = 0;

extern uint8_t gCurDir[100];

extern uint32_t wifi_loop_cycle;

volatile TRANSFER_STATE esp_state;

uint8_t left_to_send = 0;
uint8_t left_to_save[96] = {0};

#define UDISKBUFLEN 1024


#define TRANS_RCV_FIFO_BLOCK_NUM	16

#define USART1_DR_Addr ((u32)0x40013804)

typedef struct			
{
	unsigned char *bufferAddr[TRANS_RCV_FIFO_BLOCK_NUM];		//用于指向存储块的其实地址，目前共用16K的图片缓存buf
	unsigned char *p;	
	UDISK_DATA_BUFFER_STATE state[TRANS_RCV_FIFO_BLOCK_NUM];		
	unsigned char read_cur; //当前读块索引
	unsigned char write_cur;	//当前写块索引
}WIFI_DMA_RCV_FIFO;			//

volatile WIFI_DMA_RCV_FIFO wifiDmaRcvFifo;

typedef struct
{
	uint8_t flag; // 0x0: no error;  0x01: error
	uint32_t start_tick; //错误开始时刻
	uint32_t now_tick;
	
} WIFI_TRANS_ERROR;

volatile WIFI_TRANS_ERROR wifiTransError;

#ifdef ESP_MODEL
volatile ESP_SEND_STATE espSendState = ESP_SEND_IDLE;
#endif




#define WIFI_GCODE_BUFFER_SIZE	96
typedef struct
{
	uint8_t Buffer[WIFI_GCODE_BUFFER_SIZE];
	uint32_t r;
	uint32_t w;
} WIFI_GCODE_BUFFER;

extern volatile WIFI_STATE wifi_link_state;
extern WIFI_PARA wifiPara;
extern IP_PARA ipPara;
extern CLOUD_PARA cloud_para;

//extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
//extern USBH_HOST                     USB_Host;


extern void DMA1_Channel5_IRQHandler(void);



void wifi_delay(int n)
{	int i, j;

	uint32_t begin = getTick();
	uint32_t end = begin;

	while(getTickDiff(begin, end) < n)
	{
		end = getTick();
	}
}

void wifi_reset()
{
	uint32_t start, now;
	start = getTick();
	now = start;
	//GPIO_ResetBits(GPIOG, GPIO_Pin_1);
        WIFI_RESET();
	while(getTickDiff(start, now) < 500)
	{
		now = getTick();
	}
	//GPIO_SetBits(GPIOG, GPIO_Pin_1);
        WIFI_SET();
	
}

static void dma_init()
{	
	int i;

	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/*DMA 中断*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_DeInit(DMA1_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)USART1_DR_Addr;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)WifiRxFifo.uartTxBuffer;//pBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = UART_RX_BUFFER_SIZE;//NumByteToRead;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	

	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC | DMA_IT_HT , ENABLE);
	
	DMA_Cmd(DMA1_Channel5, ENABLE); //使能DMA通道5

	//USART_DMACmd (SZ_STM32_COM3, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd (USART1, USART_DMAReq_Rx, ENABLE);


	for(i = 0; i < TRANS_RCV_FIFO_BLOCK_NUM; i++)
	{
		wifiDmaRcvFifo.bufferAddr[i] = &bmp_public_buf[1024 * i];
		
		wifiDmaRcvFifo.state[i] = udisk_buf_empty;
	
	}

	memset(wifiDmaRcvFifo.bufferAddr[0], 0, 1024 * TRANS_RCV_FIFO_BLOCK_NUM);
	wifiDmaRcvFifo.read_cur = 0;
	wifiDmaRcvFifo.write_cur = 0;





}

static void wifi_deInit()
{
	DMA_DeInit(DMA1_Channel5);
	
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC | DMA_IT_HT , DISABLE);

	USART_DMACmd (USART1, USART_DMAReq_Rx, DISABLE);
}



void esp_port_begin(uint8_t interrupt)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	WifiRxFifo.uart_read_point = 0;
	WifiRxFifo.uart_write_point = 0;
	memset(WifiRxFifo.uartTxBuffer, 0, sizeof(WifiRxFifo.uartTxBuffer));
	
	
	if(interrupt)
	{
		wifi_deInit (); 

		SZ_STM32_COMInit(COM1, 115200);

		USART_ITConfig(SZ_STM32_COM1, USART_IT_RXNE, ENABLE);

		NVIC_InitStructure.NVIC_IRQChannel = SZ_STM32_COM1_IRQn;
	//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

		NVIC_Init(&NVIC_InitStructure);
	}
	else
	{
		NVIC_DisableIRQ(SZ_STM32_COM1_IRQn);
				
		USART_Cmd(SZ_STM32_COM1, DISABLE);

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		
		SZ_STM32_COMInit(COM1, 1958400);

		USART_Cmd(SZ_STM32_COM1, ENABLE);

		wifi_delay(10);

		dma_init();
	}
}

int raw_send_to_wifi(char *buf, int len)
{
	uint32_t i;
	volatile uint32_t delayIndex;
	#ifdef ESP_MODEL
	uint8_t esp_send_head[20] = {0}; ;
	#endif

	if(buf == 0  ||  len <= 0)
	{
		return 0;
	}
	for(i = 0; i < len; i++)
	{
		if(gCfgItems.wifi_type == HLK_WIFI) 
		{
			while (USART_GetFlagStatus(SZ_STM32_COM3, USART_FLAG_TC) == RESET);/*??????*/
		
			USART_SendData(SZ_STM32_COM3, *(buf + i)); 
		}	
		else
		{
			while (USART_GetFlagStatus(SZ_STM32_COM1, USART_FLAG_TC) == RESET);/*??????*/
		
			USART_SendData(SZ_STM32_COM1, *(buf + i)); 
		}

	
	}

	#ifdef ESP_MODEL
	if(espSendState == ESP_WAIT_SEND)
		espSendState = ESP_WAIT_COMPL;
	#endif

	return len;

}



void wifi_ret_ack()
{
	if(gCfgItems.wifi_type == HLK_WIFI) 
	{
		raw_send_to_wifi("ok\r\n", strlen("ok\r\n"));
	}
}


char buf_to_wifi[256];
int index_to_wifi = 0;
int package_to_wifi(WIFI_RET_TYPE type, char *buf, int len)
{
	char wifi_ret_head = 0xa5;
	char wifi_ret_tail = 0xfc;

	

	if(type == WIFI_PARA_SET)
 	{
 		int data_offset = 4;
		int apLen = strlen((const char *)gCfgItems.wifi_ap);
		int keyLen = strlen((const char *)gCfgItems.wifi_key);
		
 		memset(buf_to_wifi, 0, sizeof(buf_to_wifi));
		index_to_wifi = 0;

		buf_to_wifi[data_offset] = gCfgItems.wifi_mode_sel;
		buf_to_wifi[data_offset + 1]  = apLen;
		strncpy(&buf_to_wifi[data_offset + 2], (const char *)gCfgItems.wifi_ap, apLen);
		buf_to_wifi[data_offset + apLen + 2]  = keyLen;
		strncpy(&buf_to_wifi[data_offset + apLen + 3], (const char *)gCfgItems.wifi_key, keyLen);
		buf_to_wifi[data_offset + apLen + keyLen + 3] = wifi_ret_tail;

		index_to_wifi = apLen + keyLen + 3;

		buf_to_wifi[0] = wifi_ret_head;
		buf_to_wifi[1] = type;
		buf_to_wifi[2] = index_to_wifi & 0xff;
		buf_to_wifi[3] = (index_to_wifi >> 8) & 0xff;

		raw_send_to_wifi(buf_to_wifi, 5 + index_to_wifi);

		memset(buf_to_wifi, 0, sizeof(buf_to_wifi));
		index_to_wifi = 0;
		
 	}	
		
	else if(type == WIFI_TRANS_INF)
 	{

		if(len > sizeof(buf_to_wifi) - index_to_wifi - 5)
		{
			memset(buf_to_wifi, 0, sizeof(buf_to_wifi));
			index_to_wifi = 0;
			return;

		}
		

		 if(len > 0)
		 {		
		 	
		 	memcpy(&buf_to_wifi[4 + index_to_wifi], buf, len);
			index_to_wifi += len;
		 
			if(index_to_wifi < 1)
				return;

			 if(buf_to_wifi[index_to_wifi + 3] == '\n')
			 {	
			 	//mask "wait" "busy" "X:"
			 	if(((buf_to_wifi[4] == 'w') && (buf_to_wifi[5] == 'a') && (buf_to_wifi[6] == 'i')  && (buf_to_wifi[7] == 't') )
					|| ((buf_to_wifi[4] == 'b') && (buf_to_wifi[5] == 'u') && (buf_to_wifi[6] == 's')  && (buf_to_wifi[7] == 'y') )
					|| ((buf_to_wifi[4] == 'X') && (buf_to_wifi[5] == ':') )
					)
			 	{
			 		memset(buf_to_wifi, 0, sizeof(buf_to_wifi));
				 	index_to_wifi = 0;
					return;
			 	}

				buf_to_wifi[0] = wifi_ret_head;
				buf_to_wifi[1] = type;
				buf_to_wifi[2] = index_to_wifi & 0xff;
				buf_to_wifi[3] = (index_to_wifi >> 8) & 0xff;	
				buf_to_wifi[4 + index_to_wifi] = wifi_ret_tail;

				raw_send_to_wifi(buf_to_wifi, 5 + index_to_wifi);

				memset(buf_to_wifi, 0, sizeof(buf_to_wifi));
				 index_to_wifi = 0;
			 }
		}
	}
	else if(type == WIFI_EXCEP_INF)
	{
		memset(buf_to_wifi, 0, sizeof(buf_to_wifi));		

		buf_to_wifi[0] = wifi_ret_head;
		buf_to_wifi[1] = type;
		buf_to_wifi[2] = 1;
		buf_to_wifi[3] = 0;
		buf_to_wifi[4] = *buf;
		buf_to_wifi[5] = wifi_ret_tail;

		raw_send_to_wifi(buf_to_wifi, 6);

		memset(buf_to_wifi, 0, sizeof(buf_to_wifi));
		index_to_wifi = 0;
	}
	else if(type == WIFI_CLOUD_CFG)
	{
		int data_offset = 4;
		int urlLen = strlen((const char *)gCfgItems.cloud_hostUrl);
		
 		memset(buf_to_wifi, 0, sizeof(buf_to_wifi));
		index_to_wifi = 0;

		if(gCfgItems.cloud_enable == 0x1)
			buf_to_wifi[data_offset] = 0x0a;
		else
			buf_to_wifi[data_offset] = 0x05;
		
		buf_to_wifi[data_offset + 1]  = urlLen;
		strncpy(&buf_to_wifi[data_offset + 2], (const char *)gCfgItems.cloud_hostUrl, urlLen);
		buf_to_wifi[data_offset + urlLen + 2]  = gCfgItems.cloud_port & 0xff;
		buf_to_wifi[data_offset + urlLen + 3]  = (gCfgItems.cloud_port >> 8) & 0xff;
		buf_to_wifi[data_offset + urlLen + 4] = wifi_ret_tail;

		index_to_wifi = urlLen + 4;

		buf_to_wifi[0] = wifi_ret_head;
		buf_to_wifi[1] = type;
		buf_to_wifi[2] = index_to_wifi & 0xff;
		buf_to_wifi[3] = (index_to_wifi >> 8) & 0xff;

		raw_send_to_wifi(buf_to_wifi, 5 + index_to_wifi);

		memset(buf_to_wifi, 0, sizeof(buf_to_wifi));
		index_to_wifi = 0;
	}
	else if(type == WIFI_CLOUD_UNBIND)
	{
		memset(buf_to_wifi, 0, sizeof(buf_to_wifi));		

		buf_to_wifi[0] = wifi_ret_head;
		buf_to_wifi[1] = type;
		buf_to_wifi[2] = 0;
		buf_to_wifi[3] = 0;
		buf_to_wifi[4] = wifi_ret_tail;

		raw_send_to_wifi(buf_to_wifi, 5);

		memset(buf_to_wifi, 0, sizeof(buf_to_wifi));
		index_to_wifi = 0;
	}
}


int send_to_wifi(char *buf, int len)
{
	if(gCfgItems.wifi_type == HLK_WIFI)
	{
		return raw_send_to_wifi(buf, len);
	}
	else
	{
		return package_to_wifi(WIFI_TRANS_INF, buf, len);
	}
}

void set_cur_file_sys(int fileType)
{
	gCfgItems.fileSysType = fileType;
}

void get_file_list(char *path)
{
	if( path == 0)
	{
		return;
	}
	
	if(gCfgItems.fileSysType == FILE_SYS_SD)
	{
		SD_Initialize();
		
	}
	else if(gCfgItems.fileSysType == FILE_SYS_USB)
	{
		reset_usb_state();		
	}
	

	//if(gCfgItems.fileSysType == FILE_SYS_SD)
	{			
		Explore_Disk(path, 0);
	}
	
	
}

char wait_ip_back_flag = 0;

void init_queue(QUEUE *h_queue)
{
	if(h_queue == 0)
		return;
	
	h_queue->rd_index = 0;
	h_queue->wt_index = 0;
	memset(h_queue->buf, 0, sizeof(h_queue->buf));
}

int push_queue(QUEUE *h_queue, char *data_to_push, int data_len)
{
	if(h_queue == 0)
		return -1;

	if(data_len > sizeof(h_queue->buf[h_queue->wt_index]))
		return -1;

	if((h_queue->wt_index + 1) % 20 == h_queue->rd_index)
		return -1;

	memset(h_queue->buf[h_queue->wt_index], 0, sizeof(h_queue->buf[h_queue->wt_index]));
	memcpy(h_queue->buf[h_queue->wt_index], data_to_push, data_len);

	h_queue->wt_index = (h_queue->wt_index + 1) % 20;
	
	return 0;
}

int pop_queue(QUEUE *h_queue, char *data_for_pop, int data_len)
{
	if(h_queue == 0)
		return -1;

	if(data_len < strlen(h_queue->buf[h_queue->rd_index]))
		return -1;

	if(h_queue->rd_index == h_queue->wt_index)
		return -1;

	memset(data_for_pop, 0, data_len);
	memcpy(data_for_pop, h_queue->buf[h_queue->rd_index], strlen(h_queue->buf[h_queue->rd_index]));

	h_queue->rd_index = (h_queue->rd_index + 1) % 20;
	
	return 0;
}

unsigned char computeBinarySize(char *ptr)  // unsigned int bitfield) {
{
    unsigned char s = 2; // not include checksum
    unsigned short bitfield = *(unsigned short*)ptr;
    if(bitfield & 1) s+=2;
    if(bitfield & 8) s+=4;
    if(bitfield & 16) s+=4;
    if(bitfield & 32) s+=4;
    if(bitfield & 64) s+=4;
    if(bitfield & 256) s+=4;
    if(bitfield & 512) s+=1;
    if(bitfield & 1024) s+=4;
    if(bitfield & 2048) s+=4;
 
    if(bitfield & 2) s+=1;
    if(bitfield & 4) s+=1;
    if(bitfield & 32768) s+=16;

    return s;
}

typedef struct
{
	FIL  *file;
	char write_buf[1024];
	int write_index;	
	uint8_t saveFileName[30];
	uint32_t fileLen;
	uint32_t tick_begin;
	uint32_t tick_end;
} FILE_WRITER;

FILE_WRITER file_writer;
FIL save_File;

int32_t lastFragment = 0;

char lastBinaryCmd[50] = {0};

int total_write = 0;	
char binary_head[2] = {0, 0};
unsigned char binary_data_len = 0;

int write_to_file(char *buf, int len)
{
	int num_write;
	int i;
	FRESULT res;
	

	for(i = 0; i < len; i++)
	{
		file_writer.write_buf[file_writer.write_index++] = buf[i];
		if(file_writer.write_index >= 1024)
		{
			
			res =  f_write (file_writer.file, file_writer.write_buf, (uint32_t)file_writer.write_index,  (uint32_t *)&num_write);
			if((res != FR_OK) || (num_write != file_writer.write_index))
			{
				return  -1;
			}
			memset(file_writer.write_buf, 0, sizeof(file_writer.write_buf));
			file_writer.write_index = 0;		
		}
	}
	return 0;
}






#define ESP_PROTOC_HEAD	(uint8_t)0xa5
#define ESP_PROTOC_TAIL		(uint8_t)0xfc

#define ESP_TYPE_NET				(uint8_t)0x0
#define ESP_TYPE_GCODE				(uint8_t)0x1
#define ESP_TYPE_FILE_FIRST			(uint8_t)0x2
#define ESP_TYPE_FILE_FRAGMENT		(uint8_t)0x3

uint8_t esp_msg_buf[UART_RX_BUFFER_SIZE] = {0}; //????????
uint16_t esp_msg_index = 0; //???

typedef struct
{
	uint8_t head; //0xa5
	uint8_t type; //0x0:????,0x1:gcode??,0x2:??????,0x3:??????(?????),0x4:????????
	uint16_t dataLen; //????
	uint8_t *data; //????
	uint8_t tail; // 0xfc
} ESP_PROTOC_FRAME;


static int cut_msg_head(uint8_t *msg, uint16_t msgLen, uint16_t cutLen)
{
	int i;
	
	if(msgLen < cutLen)
	{
		return 0;
	}
	else if(msgLen == cutLen)
	{
		memset(msg, 0, msgLen);
		return 0;
	}
	for(i = 0; i < (msgLen - cutLen); i++)
	{
		msg[i] = msg[cutLen + i];
	}
	memset(&msg[msgLen - cutLen], 0, cutLen);
	
	return msgLen - cutLen;
	
}

static void wifi_gcode_exec(uint8_t *cmd_line)
{
	int8_t  tempBuf[100] = {0};
	uint8_t *tmpStr = 0;
	int  cmd_value;
	
	if((strstr((char *)&cmd_line[0], "\n") != 0) && ((strstr((char *)&cmd_line[0], "G") != 0) || (strstr((char *)&cmd_line[0], "M") != 0) || (strstr((char *)&cmd_line[0], "T") != 0) ))
	{
		
		tmpStr = (int8_t *)strstr(&cmd_line[0], "\n");
		if(tmpStr)
		{
			*tmpStr = '\0';
		}
		tmpStr = (int8_t *)strstr(&cmd_line[0], "\r");
		if(tmpStr)
		{
			*tmpStr = '\0';
		}
		tmpStr = (int8_t *)strstr(&cmd_line[0], "*");
		if(tmpStr)
		{
			*tmpStr = '\0';
		}
		tmpStr = (int8_t *)strstr(&cmd_line[0], "M");
		if( tmpStr)
		{
			cmd_value = atoi(tmpStr + 1);
			tmpStr = (int8_t *)strstr(tmpStr, " ");

			switch(cmd_value)
			{
				
				case 20: //print sd / udisk file
					{								
							
						int index = 0;
						char *det_pos;

						if(tmpStr == 0)
						{
							gCfgItems.fileSysType = FILE_SYS_SD;	
							send_to_wifi("Begin file list\r\n", strlen("Begin file list\r\n"));
							get_file_list("1:/");
							send_to_wifi("End file list\r\n", strlen("End file list\r\n"));

							send_to_wifi("ok\r\n", strlen("ok\r\n"));
							break;
						}

						while(tmpStr[index] == ' ')
							index++;

						if(gCfgItems.wifi_type == ESP_WIFI)
						{
							char *path = tempBuf;

							
							
							//det_pos = (char *)strstr(&tmpStr[index], "1:/sdcard");

							if(strlen((const char *)&tmpStr[index]) < 80)
							{
								send_to_wifi("Begin file list\r\n", strlen("Begin file list\r\n"));
								
								if(strncmp(&tmpStr[index], "1:", 2) == 0)
								{
									gCfgItems.fileSysType = FILE_SYS_SD;									
									
								}
	 							else if(strncmp(&tmpStr[index], "0:", 2) == 0)
	 							{
	 								gCfgItems.fileSysType = FILE_SYS_USB;
																		
								}
								strcpy((char *)path, &tmpStr[index]);	
								get_file_list(path);
								send_to_wifi("End file list\r\n", strlen("End file list\r\n"));
							}
							send_to_wifi("ok\r\n", strlen("ok\r\n"));
						}
						else
						{
							wifi_ret_ack();
							det_pos = (char *)strstr(&tmpStr[index], ".DIR");

							if(det_pos)
							{
								while(tmpStr[index] == ' ')
									index++;
								
								*det_pos = '\0';
								send_to_wifi("Begin file list\r\n", strlen("Begin file list\r\n"));
								get_file_list(&tmpStr[index]);																	
								send_to_wifi("End file list\r\n", strlen("End file list\r\n"));
							}
							else
							{
								send_to_wifi("Begin file list\r\n", strlen("Begin file list\r\n"));
								if(gCfgItems.fileSysType == FILE_SYS_SD)
								{
									get_file_list("1:");
									
								}
								else if(gCfgItems.fileSysType == FILE_SYS_USB)
								{
									get_file_list("0:");		
								}
																
								send_to_wifi("End file list\r\n", strlen("End file list\r\n"));
							}
						}
						
						
					}
					break;

				case 21:
					/*init sd card*/
					send_to_wifi("ok\r\n", strlen("ok\r\n"));
					break;

				case 23:					
					/*select the file*/
					if(printerStaus == pr_idle)
					{
						char *det_pos;
						int index = 0;
						FIL temp_file;
						FRESULT res ;
						while(tmpStr[index] == ' ')
							index++;

						if(strstr(&tmpStr[index], ".g") || strstr(&tmpStr[index], ".G"))
						{
							if(strlen((const char *)&tmpStr[index]) < 80)
							{
								memset(curFileName, 0, sizeof(curFileName));

								if(gCfgItems.wifi_type == ESP_WIFI)
								{
									if(strncmp(&tmpStr[index], "1:", 2) == 0)
									{
										gCfgItems.fileSysType = FILE_SYS_SD;									
										
									}
		 							else if(strncmp(&tmpStr[index], "0:", 2) == 0)
		 							{
		 								gCfgItems.fileSysType = FILE_SYS_USB;
																			
									}
									else
									{
										if(gCfgItems.fileSysType == FILE_SYS_SD)
										{
											strcat((char *)curFileName, "1:");
										}
										else
										{
											strcat((char *)curFileName, "0:");
										}
										if(tmpStr[index] != '/')
											strcat((char *)curFileName, "/");
									}
									strcat((char *)curFileName, &tmpStr[index]);
									
									
								}
								else
								{
									strcpy(curFileName, &tmpStr[index]);
								}
								res = f_open(&temp_file, curFileName, FA_OPEN_EXISTING | FA_READ);
								if(res == FR_OK)
								{
									send_to_wifi("File selected\r\n", strlen("File selected\r\n"));
									
								}
								else
								{
									send_to_wifi("file.open failed\r\n", strlen("file.open failed\r\n"));
									strcpy(curFileName, "notValid");
								}
								send_to_wifi("ok\r\n", strlen("ok\r\n"));
								
							}
							
						
						}
							
						
					}
					break;

				case 24:
					if(strcmp(curFileName, "notValid") != 0)
            				{
	            				/*start or resume print file*/	
						Get_Temperature_Flg = 1;
						get_temp_flag = 1;

						
	            				
						if(printerStaus == pr_idle)
						{
							clear_cur_ui();
							reset_file_info();
							reset_print_time();
							start_print_time();
							draw_printing();

							
						}
						else if(printerStaus == pr_pause)
						{
							clear_cur_ui();
							start_print_time();
							draw_printing();
							
							printerStaus = pr_working;
							I2C_EE_Init(400000);
						}
						else if(printerStaus == pr_reprint)
						{
							clear_cur_ui();
							start_print_time();
							draw_printing();
							
							
							printerStaus = pr_working;
							I2C_EE_Init(400000);
						}		
					}
					send_to_wifi("ok\r\n", strlen("ok\r\n"));
					break;

				case 25:
					/*pause print file*/						
					if(printerStaus == pr_working)
					{
						stop_print_time();							

						clear_cur_ui();
						
						printerStaus = pr_pause;
						
						draw_pause();

						send_to_wifi("ok\r\n", strlen("ok\r\n"));

						I2C_EE_Init(100000);
					}					
					
					break;
					
				case 26:
					/*stop print file*/						
					//if((printerStaus == pr_working) || (printerStaus == pr_pause))
					if((printerStaus == pr_working) || (printerStaus == pr_pause) || (printerStaus == pr_reprint))
					{
						stop_print_time();							

						clear_cur_ui();
						
						printerStaus = pr_stop;
						reset_file_info();
						Get_Temperature_Flg = 0;
						draw_ready_print();

						send_to_wifi("ok\r\n", strlen("ok\r\n"));

						I2C_EE_Init(100000);
					}					
					
					break;

				case 27:
					/*report print rate*/
					//if((printerStaus == pr_working) || (printerStaus == pr_pause))
					if((printerStaus == pr_working) || (printerStaus == pr_pause)|| (printerStaus == pr_reprint))
					{
						int rate = get_printing_rate(srcfp);
						if((printerStaus != pr_idle)  &&  (rate == 100))
						{	
							rate = 99;
						}
						
						memset(tempBuf, 0, sizeof(tempBuf));

						sprintf(tempBuf, "M27 %d\r\n", rate);

						wifi_ret_ack();

						send_to_wifi(tempBuf, strlen(tempBuf));
						
					}
					
					break;

				case 28:
					/*begin to transfer file to filesys*/
					if(printerStaus == pr_idle)
					{
						
						int index = 0;
						while(tmpStr[index] == ' ')
							index++;

						if(strstr(&tmpStr[index], ".g") || strstr(&tmpStr[index], ".G"))
						{
							FRESULT res;
							
							strcpy(file_writer.saveFileName, &tmpStr[index]);
							
							if(gCfgItems.fileSysType == FILE_SYS_SD)
							{
								memset(tempBuf, 0, sizeof(tempBuf));
								sprintf(tempBuf, "1:/%s", file_writer.saveFileName);
								SD_Initialize();
							}
							else if(gCfgItems.fileSysType == FILE_SYS_USB)
							{
								memset(tempBuf, 0, sizeof(tempBuf));
								sprintf(tempBuf, "0:/%s", file_writer.saveFileName);
							}
							mount_file_sys(gCfgItems.fileSysType);
							
							res = f_open(&save_File, tempBuf, FA_CREATE_ALWAYS | FA_WRITE);
						//	strcat(tempBuf, ".cpr");
						//	res = f_open(&file_brp, tempBuf, FA_CREATE_ALWAYS | FA_WRITE);
							
							if(res == FR_OK)
							{
								memset(file_writer.saveFileName, 0, sizeof(file_writer.saveFileName));
								strcpy(file_writer.saveFileName, &tmpStr[index]);
								memset(tempBuf, 0, sizeof(tempBuf));
								sprintf(tempBuf, "Writing to file: %s\r\n", file_writer.saveFileName);
								wifi_ret_ack();
								send_to_wifi(tempBuf, strlen(tempBuf));

								total_write = 0;	
								wifi_link_state = WIFI_WAIT_TRANS_START;
								
							}
							else
							{
								wifi_link_state = WIFI_CONNECTED;
								clear_cur_ui();
								draw_dialog_filetransfer(2);
							}
						
						}
							
						
						//wifi_ret_ack();
					}
					break;

				
				case 105:
				case 991:
					memset(tempBuf, 0, sizeof(tempBuf));
					if(cmd_value == 105)
					{
						send_to_wifi("ok\r\n", strlen("ok\r\n"));
						sprintf(tempBuf,"T:%.1f /%.1f B:%.1f /%.1f T0:%.1f /%.1f T1:%.1f /%.1f @:0 B@:0\r\n", 
						gCfgItems.curSprayerTemp[0], gCfgItems.desireSprayerTemp[0], gCfgItems.curBedTemp, gCfgItems.desireBedTemp,
						gCfgItems.curSprayerTemp[0], gCfgItems.desireSprayerTemp[0], gCfgItems.curSprayerTemp[1], gCfgItems.desireSprayerTemp[1]);
					}
					else
					{
						sprintf(tempBuf,"T:%d /%d B:%d /%d T0:%d /%d T1:%d /%d @:0 B@:0\r\n", 
						(int)gCfgItems.curSprayerTemp[0], (int)gCfgItems.desireSprayerTemp[0], (int)gCfgItems.curBedTemp, (int)gCfgItems.desireBedTemp,
						(int)gCfgItems.curSprayerTemp[0], (int)gCfgItems.desireSprayerTemp[0], (int)gCfgItems.curSprayerTemp[1], (int)gCfgItems.desireSprayerTemp[1]);
					}

					send_to_wifi(tempBuf, strlen(tempBuf));
					
					//???????????
					pushFIFO(&gcodeCmdTxFIFO, GET_CUR_TEM_COMMAND);
					//usart2Data.prWaitStatus = pr_wait_idle;
					
					break;

				case 110:
					if((gCfgItems.wifi_type == HLK_WIFI) && (wifi_link_state == WIFI_WAIT_TRANS_START))
					{
						
						wifi_link_state = WIFI_TRANS_FILE;
						file_writer.write_index = 0;
						file_writer.file = &save_File;
						send_to_wifi("M110 OK\r\n", strlen("M110 OK\r\n"));
						wifi_ret_ack();
						//wifi_loop_cycle = 2;
						wifi_loop_cycle = 50;
						
						clear_cur_ui();
						draw_filetansfer(0);
						
					}
					else
					{
						strcat(cmd_line, "\n");
						pushFIFO(&gcodeCmdTxFIFO, cmd_line);
						
						send_to_wifi("ok\r\n", strlen("ok\r\n"));
					}
					break;
				case 992:
					if((printerStaus == pr_working) || (printerStaus == pr_pause))
					{
						memset(tempBuf,0,sizeof(tempBuf));
						sprintf(tempBuf, "M992 %d%d:%d%d:%d%d\r\n", print_time.hours/10, print_time.hours%10, print_time.minutes/10, print_time.minutes%10,	print_time.seconds/10, print_time.seconds%10);
						wifi_ret_ack();
						send_to_wifi(tempBuf, strlen(tempBuf));	
					}
											
					break;
				case 994:
					if((printerStaus == pr_working) || (printerStaus == pr_pause))
					{
						memset(tempBuf,0,sizeof(tempBuf));
						if(strlen((const char *)curFileName) > 100)
						{
							return;
						}
						sprintf((char *)tempBuf, "M994 %s;%d\n", curFileName, f_size(srcfp));
						wifi_ret_ack();
						send_to_wifi(tempBuf, strlen(tempBuf));	
					}
					break;
				case 997:
					if(printerStaus == pr_idle)
					{
						wifi_ret_ack();
						send_to_wifi("M997 IDLE\r\n", strlen("M997 IDLE\r\n"));
					}
					else if(printerStaus == pr_working)
					{
						wifi_ret_ack();
						send_to_wifi("M997 PRINTING\r\n", strlen("M997 PRINTING\r\n"));
					}
					else if(printerStaus == pr_pause)
					{
						wifi_ret_ack();
						send_to_wifi("M997 PAUSE\r\n", strlen("M997 PAUSE\r\n"));
					}
					else if(printerStaus == pr_reprint)
					{
						wifi_ret_ack();
						send_to_wifi("M997 PAUSE\r\n", strlen("M997 PAUSE\r\n"));
					}								
					break;

				case 998:
					if(printerStaus == pr_idle)
					{
						if(atoi(tmpStr) == 0)
						{
							set_cur_file_sys(0);
						}
						else if(atoi(tmpStr) == 1)
						{
							set_cur_file_sys(1);
						}
						wifi_ret_ack();
					}
					break;

				case 115:
					memset(tempBuf,0,sizeof(tempBuf));
					send_to_wifi("ok\r\n", strlen("ok\r\n"));
					#if defined(STM32F10X_CL)
					send_to_wifi("FIRMWARE_NAME:TFT28/32\r\n", strlen("FIRMWARE_NAME:TFT28/32\r\n"));	
					#else
					send_to_wifi("FIRMWARE_NAME:TFT24\r\n", strlen("FIRMWARE_NAME:TFT24\r\n"));	
					#endif
				
					break;

				default:
					strcat(cmd_line, "\n");
					
					if(checkFIFO(&gcodeCmdTxFIFO) != fifo_full)
					{
						left_to_send = 0;
						pushFIFO(&gcodeCmdTxFIFO, cmd_line);
						send_to_wifi("ok\r\n", strlen("ok\r\n"));
					}
					else
					{
						if(strlen(cmd_line) < sizeof(left_to_save))
						{
							memset(left_to_save, 0, sizeof(left_to_save));
							strcpy(left_to_save, cmd_line);
							left_to_send = 1;
						}
					}
					
					break;
					
			}
		}
		else
		{
			
			//
			{
				tmpStr = (int8_t *)strstr(&cmd_line[0], "G");
				if( tmpStr)
				{
					if(printerStaus == pr_pause)
					{
						if((*(tmpStr+3)=='X')||(*(tmpStr+3)=='Y')||(*(tmpStr+3)=='Z'))
						{
							positionSaveFlag = 1;
						}
					}
				}
				strcat(cmd_line, "\n");
				if(checkFIFO(&gcodeCmdTxFIFO) != fifo_full)
				{
					left_to_send = 0;
					pushFIFO(&gcodeCmdTxFIFO, cmd_line);
					send_to_wifi("ok\r\n", strlen("ok\r\n"));
				}
				else
				{
					if(strlen(cmd_line) < sizeof(left_to_save))
					{
						memset(left_to_save, 0, sizeof(left_to_save));
						strcpy(left_to_save, cmd_line);
						left_to_send = 1;
					}
				}
			}
		}
	}
}

/*????????????????,?0??,???????-1*/
static int32_t charAtArray(const uint8_t *_array, uint32_t _arrayLen, uint8_t _char)
{
	uint32_t i;
	for(i = 0; i < _arrayLen; i++)
	{
		if(*(_array + i) == _char)
		{
			return i;
		}
	}
	
	return -1;
}

static void net_msg_handle(uint8_t * msg, uint16_t msgLen)
{
	int wifiNameLen, wifiKeyLen, hostLen, id_len, ver_len;
	char ipStr[16];
	
	if(msgLen <= 0)
		return;

	//ip
	sprintf(ipPara.ip_addr, "%d.%d.%d.%d", msg[0], msg[1], msg[2], msg[3]);

	//port
//

	//connect state
	if(msg[6] == 0x0a)
	{
		wifi_link_state = WIFI_CONNECTED;
	}
	else if(msg[6] == 0x0e)
	{
		wifi_link_state = WIFI_EXCEPTION;
	}
	else
	{
		wifi_link_state = WIFI_NOT_CONFIG;
	}

	//mode
	wifiPara.mode = msg[7];
	

	//wifi name
	wifiNameLen = msg[8];
	wifiKeyLen = msg[9 + wifiNameLen];
	if(wifiNameLen < 32)
	{
		memset(wifiPara.ap_name, 0, sizeof(wifiPara.ap_name));
		memcpy(wifiPara.ap_name, &msg[9], wifiNameLen);

		//wifi key		
		if(wifiKeyLen < 64)
		{			
			memset(wifiPara.keyCode, 0, sizeof(wifiPara.keyCode));
			memcpy(wifiPara.keyCode, &msg[10 + wifiNameLen], wifiKeyLen);
		}
	}

	
	cloud_para.state =msg[10 + wifiNameLen + wifiKeyLen];
	hostLen = msg[11 + wifiNameLen + wifiKeyLen];
	if(cloud_para.state)
	{			
		if(hostLen < 96)
		{
			memset(cloud_para.hostUrl, 0, sizeof(cloud_para.hostUrl));
			memcpy(cloud_para.hostUrl, &msg[12 + wifiNameLen + wifiKeyLen], hostLen);
		}
		cloud_para.port = msg[12 + wifiNameLen + wifiKeyLen + hostLen] + (msg[13 + wifiNameLen + wifiKeyLen + hostLen] << 8);
				
	}

	// id
	id_len = msg[14 + wifiNameLen + wifiKeyLen + hostLen];
	if(id_len == 20)
	{
		memset(cloud_para.id, 0, sizeof(cloud_para.id));
		memcpy(cloud_para.id, (const char *)&msg[15 + wifiNameLen + wifiKeyLen + hostLen], id_len);
	}
	ver_len = msg[15 + wifiNameLen + wifiKeyLen + hostLen + id_len];
	if(ver_len < 20)
	{
		memset(wifi_firm_ver, 0, sizeof(wifi_firm_ver));
		memcpy(wifi_firm_ver, (const char *)&msg[16 + wifiNameLen + wifiKeyLen + hostLen + id_len], ver_len);
	}

	if(cfg_wifi_flag == 1)
	{
		if((wifiPara.mode != gCfgItems.wifi_mode_sel)
			|| (strncmp(wifiPara.ap_name, (const char *)gCfgItems.wifi_ap, 32) != 0)
			|| (strncmp(wifiPara.keyCode, (const char *)gCfgItems.wifi_key, 64) != 0))
		{
			package_to_wifi(WIFI_PARA_SET, (char *)0, 0);
		}
		else
			cfg_wifi_flag = 0;
	}
	if(cfg_cloud_flag == 1)
	{
		if(((cloud_para.state >> 4) != gCfgItems.cloud_enable)
			|| (strncmp(cloud_para.hostUrl, (const char *)gCfgItems.cloud_hostUrl, 96) != 0)
			|| (cloud_para.port != gCfgItems.cloud_port))
		{
			package_to_wifi(WIFI_CLOUD_CFG, (char *)0, 0);
		}
		else
			cfg_cloud_flag = 0;
	}

	
	
}

static void gcode_msg_handle(uint8_t * msg, uint16_t msgLen)
{
	uint8_t gcodeBuf[100] = {0};
	char *index_s;
	char *index_e;
	
	if(msgLen <= 0)
		return;

	index_s = (char *)msg;
	index_e = (char *)strstr((char *)msg, "\n");
	if(*msg == 'N')
	{
		index_s = (char *)strstr(msg, " ");
		while((*index_s) == ' ')
		{
			index_s++;
		}
	}
	while((index_e != 0) && ((int)index_s < (int)index_e))
	{
		if(index_e - index_s < sizeof(gcodeBuf))	
		{
			memset(gcodeBuf, 0, sizeof(gcodeBuf));
			
			memcpy(gcodeBuf, index_s, index_e - index_s + 1);

		
			wifi_gcode_exec(gcodeBuf);
		}
		while((*index_e == '\r') || (*index_e == '\n'))
			index_e++;

		index_s = index_e;
		index_e = (char *)strstr(index_s, "\n");
	}
}

char saveFilePath[50];

static void file_first_msg_handle(uint8_t * msg, uint16_t msgLen)
{
	uint8_t fileNameLen = *msg;
	
	FRESULT res;
		
	if(msgLen != fileNameLen + 5)
	{
		return;
	}
	
	file_writer.fileLen = *((uint32_t *)(msg + 1));
	memset(file_writer.saveFileName, 0, sizeof(file_writer.saveFileName));

	memcpy(file_writer.saveFileName, msg + 5, fileNameLen);

	memset(file_writer.write_buf, 0, sizeof(file_writer.write_buf));

	if(strlen((const char *)file_writer.saveFileName) > sizeof(saveFilePath))
		return;

	memset(saveFilePath, 0, sizeof(saveFilePath));
	
	if(gCfgItems.fileSysType == FILE_SYS_SD)
	{
		sprintf((char *)saveFilePath, "1:/%s", file_writer.saveFileName);
		SD_Initialize();
	}
	else if(gCfgItems.fileSysType == FILE_SYS_USB)
	{
		sprintf((char *)saveFilePath, "0:/%s", file_writer.saveFileName);
	}
	mount_file_sys(gCfgItems.fileSysType);
	
	
	file_writer.write_index = 0;
	file_writer.file = &save_File;
	lastFragment = -1;

	wifiTransError.flag = 0;
	wifiTransError.start_tick = 0;
	wifiTransError.now_tick = 0;

	f_close(srcfp);


	wifi_delay(1000);
		
	res = f_open(&save_File, (const TCHAR *)saveFilePath, FA_CREATE_ALWAYS | FA_WRITE);

	
	if(res != FR_OK)
	{
		clear_cur_ui();
		upload_result = 2;

		wifiTransError.flag = 1;
		wifiTransError.start_tick = getTick();	
		
		draw_dialog(DIALOG_TYPE_UPLOAD_FILE);
		return;
	}

//	sprintf(tempBuf, "Writing to file: %s\n", file_writer.saveFileName);
//	raw_send_to_wifi(tempBuf, strlen(tempBuf));
							
	wifi_link_state = WIFI_TRANS_FILE;

	upload_result = 1;

	clear_cur_ui();
	draw_dialog(DIALOG_TYPE_UPLOAD_FILE);

	GUI_Exec();

	file_writer.tick_begin = getTick();
		
	
}

#define FRAG_MASK	~(1 << 31)

static void file_fragment_msg_handle(uint8_t * msg, uint16_t msgLen)
{
	uint32_t num_write;
	uint32_t frag = *((uint32_t *)msg);

	if((frag & FRAG_MASK) != (uint32_t)(lastFragment  + 1))
	{
		/*不是预期的分片号*/
		/*重发处理*/
		memset(file_writer.write_buf, 0, sizeof(file_writer.write_buf));
		file_writer.write_index = 0;
	//	f_close(&save_File);
	//	f_unlink((const char *)saveFilePath);
		
		wifi_link_state = WIFI_CONNECTED;	

		upload_result = 2; 
		
	}
	else
	{
		if(write_to_file((char *)msg + 4, msgLen - 4) < 0)
		{
			memset(file_writer.write_buf, 0, sizeof(file_writer.write_buf));
			file_writer.write_index = 0;
			
			wifi_link_state = WIFI_CONNECTED;	

			upload_result = 2; 
			return;
		}
		lastFragment = frag;
		
		if((frag & (~FRAG_MASK)) != 0)
		{
			/*??????*/
			FRESULT res =  f_write (file_writer.file, file_writer.write_buf, file_writer.write_index, &num_write);
			if((res != FR_OK) || (num_write != file_writer.write_index))
			{
				memset(file_writer.write_buf, 0, sizeof(file_writer.write_buf));
				file_writer.write_index = 0;
				
				wifi_link_state = WIFI_CONNECTED;	

				upload_result = 2; 
				return;
			}
			memset(file_writer.write_buf, 0, sizeof(file_writer.write_buf));
			file_writer.write_index = 0;
		//	f_close(&save_File);

			file_writer.tick_end = getTick();

			upload_time = getTickDiff(file_writer.tick_begin, file_writer.tick_end) / 1000;

			upload_size = f_size(&save_File);
			
			wifi_link_state = WIFI_CONNECTED;	

			upload_result = 3; 
			
		}
		//wifi_ret_ack();
		
	}
}


void esp_data_parser(char *cmdRxBuf, int len)
{
	int32_t head_pos;
	int32_t tail_pos;
	uint16_t cpyLen;
	int16_t leftLen = len; //剩余长度
	uint8_t loop_again = 0;

	ESP_PROTOC_FRAME esp_frame;

	while((leftLen > 0) || (loop_again == 1))
	//while(leftLen > 0)
	{
		loop_again = 0;
		
		/* 1. 查找帧头*/
		if(esp_msg_index != 0)
		{
			
			head_pos = 0;
			cpyLen = (leftLen < (sizeof(esp_msg_buf) - esp_msg_index)) ? leftLen : sizeof(esp_msg_buf) - esp_msg_index;
			
			memcpy(&esp_msg_buf[esp_msg_index], cmdRxBuf + len - leftLen, cpyLen);			

			esp_msg_index += cpyLen;

			leftLen = leftLen - cpyLen;
			tail_pos = charAtArray(esp_msg_buf, esp_msg_index, ESP_PROTOC_TAIL);
			
				
			if(tail_pos == -1)
			{
				//没有帧尾
				if(esp_msg_index >= sizeof(esp_msg_buf))
				{
					memset(esp_msg_buf, 0, sizeof(esp_msg_buf));
					esp_msg_index = 0;
				}
			
				return;
			}
		}
		else
		{
			head_pos = charAtArray((uint8_t const *)&cmdRxBuf[len - leftLen], leftLen, ESP_PROTOC_HEAD);
			if(head_pos == -1)
			{
				//没有帧头
				return;
			}
			else
			{
				//先缓存到buf	
				memset(esp_msg_buf, 0, sizeof(esp_msg_buf));
				memcpy(esp_msg_buf, &cmdRxBuf[len - leftLen + head_pos], leftLen - head_pos);

				esp_msg_index = leftLen - head_pos;

				leftLen = 0;

				head_pos = 0;
				
				tail_pos = charAtArray(esp_msg_buf, esp_msg_index, ESP_PROTOC_TAIL);
				
				if(tail_pos == -1)
				{
					//找到帧头，没有帧尾	
					if(esp_msg_index >= sizeof(esp_msg_buf))
					{
						memset(esp_msg_buf, 0, sizeof(esp_msg_buf));
						esp_msg_index = 0;
					}
					return;
				}
				
			}
		}
		
		/*3. 找到完整的一帧	, 判断数据长度*/
		esp_frame.type = esp_msg_buf[1];
		if((esp_frame.type != ESP_TYPE_NET) && (esp_frame.type != ESP_TYPE_GCODE)
			 && (esp_frame.type != ESP_TYPE_FILE_FIRST) && (esp_frame.type != ESP_TYPE_FILE_FRAGMENT))
		{
			//数据类型不正确，丢弃
			memset(esp_msg_buf, 0, sizeof(esp_msg_buf));
			esp_msg_index = 0;
			return;
		}
		
		esp_frame.dataLen = esp_msg_buf[2] + (esp_msg_buf[3] << 8);

		/*数据长度的判断不一定符合的，在帧长度不等于定长的情况下*/
		/*if(esp_frame.dataLen > esp_msg_index - 5)
		{
			//数据长度不正确，丢弃
			memset(esp_msg_buf, 0, sizeof(esp_msg_buf));
			esp_msg_index = 0;
			return;
		}*/
		if(4 + esp_frame.dataLen > sizeof(esp_msg_buf))
		{
			//数据长度不正确，丢弃
			memset(esp_msg_buf, 0, sizeof(esp_msg_buf));
			esp_msg_index = 0;
			return;
		}

		if(esp_msg_buf[4 + esp_frame.dataLen] != ESP_PROTOC_TAIL)
		{
			if(esp_msg_index >= sizeof(esp_msg_buf))
			{
				//帧尾不正确，丢弃
				memset(esp_msg_buf, 0, sizeof(esp_msg_buf));
				esp_msg_index = 0;
			}
			return;
		}
		
		/*4. 按照类型分别处理数据*/		
		esp_frame.data = &esp_msg_buf[4];
		switch(esp_frame.type)
		{
			case ESP_TYPE_NET:
				net_msg_handle(esp_frame.data, esp_frame.dataLen);
				break;

			case ESP_TYPE_GCODE:
				gcode_msg_handle(esp_frame.data, esp_frame.dataLen);
				break;

			case ESP_TYPE_FILE_FIRST:
				file_first_msg_handle(esp_frame.data, esp_frame.dataLen);
				break;

			case ESP_TYPE_FILE_FRAGMENT:
				file_fragment_msg_handle(esp_frame.data, esp_frame.dataLen);
				break;

			
			default:
				break;
				
		}
		/*5. 把已处理的数据截掉*/
	//	esp_msg_index = 0; //目前是固定帧长度，不做冗余处理
		esp_msg_index = cut_msg_head(esp_msg_buf, esp_msg_index, esp_frame.dataLen  + 5);
		if(esp_msg_index > 0)
		{
			if(charAtArray(esp_msg_buf, esp_msg_index,  ESP_PROTOC_HEAD) == -1)
			{
				memset(esp_msg_buf, 0, sizeof(esp_msg_buf));
				esp_msg_index = 0;
				return;
			}
			
			if((charAtArray(esp_msg_buf, esp_msg_index,  ESP_PROTOC_HEAD) != -1) && (charAtArray(esp_msg_buf, esp_msg_index, ESP_PROTOC_TAIL) != -1))
			{
				loop_again = 1;
			}
		}
	}
}

void hlk_data_parser(char *cmdRxBuf, int len)
{
	
	float  tmpTemp = 0;
	uint8_t *tmpStr = 0;
	int8_t rcv_ack_flag = 0;
	int32_t i, j, k;
	int8_t inc_flag = 0;
	int8_t num_valid = 0;
	int8_t  tempBuf[100] = {0};
	int8_t cmd_line[50] = {0};
	char binary_buf_ptr[50] = {0};
	int num_write = 0;
	int cr_index;
	int gcode_int_value;
	float gcode_float_value;
	int error_binary_head = 0;
	char *ignoreStr;
	
	int res;

	QUEUE cmd_queue;

	if(cmdRxBuf == 0)
	{
		return;
	}
	
	//memset(cmdRxBuf, 0, sizeof(cmdRxBuf));
	//while(popFIFO(&gcodeCmdRxFIFO,  cmdRxBuf) == fifo_ok)
	if(len > 0)	
	{	
		//link_mutex_detect_time = 0;
				
				i = 0;
		
		if(wifi_link_state == WIFI_TRANS_FILE)
		{
			char *endStr = 0;
			
			//char binary_head[2] = {0, 0};
				
				//unsigned char binary_data_len = 0;
			//	char binary_write_buf[512];
			//	int binary_write_index = 0;
				char binary_tmp_str[20];
				int binary_index_per_frame;
				int tmp_len;
			memset(binary_head, 0, sizeof(binary_head));
				 binary_data_len = 0;
			
			endStr = (char *)strstr(&cmdRxBuf[0], "M29");
		
			if((cmdRxBuf[0] & 0x80) || (lastBinaryCmd[0] != 0))	//binary
			{
				int binary_read_index = 0;
				
				
				
			total_write += len;
			while(1)
			{
				if(lastBinaryCmd[0] != 0)
				{
					if(lastBinaryCmd[0] < 2) //only one head byte
					{
						binary_head[0] = lastBinaryCmd[1];
						binary_head[1] = cmdRxBuf[0];
						
						if((binary_head[0] & 0x80) == 0)
						{
							memset(lastBinaryCmd, 0, sizeof(lastBinaryCmd));
							endStr = (char *)strstr(&cmdRxBuf[0], "M29");
							error_binary_head = 1;
							break;
						}
						
						binary_data_len = computeBinarySize(binary_head);
						
						if(len < binary_data_len - lastBinaryCmd[0] + 1)
						{
							memcpy(&lastBinaryCmd[2], cmdRxBuf, len);
							lastBinaryCmd[0] += len;
							break;
						}
						
						memcpy(binary_buf_ptr,  &lastBinaryCmd[1], lastBinaryCmd[0]);
						memcpy(binary_buf_ptr + lastBinaryCmd[0], cmdRxBuf, binary_data_len - lastBinaryCmd[0] );
						
						

						binary_read_index = binary_data_len - lastBinaryCmd[0];
					}
					else
					{
						binary_head[0] = lastBinaryCmd[1];
						binary_head[1] = lastBinaryCmd[2];
						
						if((binary_head[0] & 0x80) == 0)
						{
							memset(lastBinaryCmd, 0, sizeof(lastBinaryCmd));
							endStr = (char *)strstr(&cmdRxBuf[0], "M29");
							error_binary_head = 1;
							break;
						}
						
						binary_data_len = computeBinarySize(binary_head);
						
						if(len < binary_data_len - lastBinaryCmd[0])
						{
							memcpy(&lastBinaryCmd[lastBinaryCmd[0] + 1], cmdRxBuf, len);
							lastBinaryCmd[0] += len;
							break;
						}
						
						memcpy(binary_buf_ptr,  &lastBinaryCmd[1], lastBinaryCmd[0]);
						memcpy(binary_buf_ptr + lastBinaryCmd[0], cmdRxBuf, binary_data_len - lastBinaryCmd[0] );
						
						binary_read_index = binary_data_len - lastBinaryCmd[0];
					}
					
				}
				else
				{
					binary_head[0] = cmdRxBuf[binary_read_index];
					binary_head[1] = cmdRxBuf[binary_read_index + 1];
					
					if((binary_head[0] & 0x80) == 0)
					{
						endStr = (char *)strstr(&cmdRxBuf[binary_read_index], "M29");
						error_binary_head = 1;
						break;
					}
				
						
					
					binary_data_len = computeBinarySize(binary_head);

					if(len <= (binary_read_index + binary_data_len))
					{
						memcpy(&lastBinaryCmd[1], &cmdRxBuf[binary_read_index], len - binary_read_index);
						lastBinaryCmd[0] = len - binary_read_index;
						break;
					}

					
					memset(binary_buf_ptr, 0, sizeof(binary_buf_ptr));
					memcpy(binary_buf_ptr, &cmdRxBuf[binary_read_index], binary_data_len);
					
					binary_read_index += binary_data_len ;

					
					
				}

				memset(lastBinaryCmd, 0, sizeof(lastBinaryCmd));
			
				
					binary_index_per_frame = 2;
						
					if(binary_read_index >= len)
					{
						break;
					}
					//binary_head[0] = binary_buf_ptr[0];
					//binary_head[1] = binary_buf_ptr[1];
				//	binary_read_index += 2;
				//	binary_index_per_frame += 2;
				//	binary_data_len = computeBinarySize(binary_buf_ptr);


					if(binary_head[0] & 0x1) // N
					{							
						sprintf(binary_tmp_str, "N%d ", binary_buf_ptr[binary_index_per_frame] + (binary_buf_ptr[binary_index_per_frame + 1] << 8));
						write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						binary_index_per_frame += 2;
				//		binary_read_index += 2;
					}

					if(binary_head[0] & (0x1 << 1)) // M
					{							
						sprintf(binary_tmp_str, "M%d ", binary_buf_ptr[binary_index_per_frame]);
						write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						binary_index_per_frame += 1;

						/*after M117, text follow */
						if(binary_buf_ptr[binary_index_per_frame - 1] == 117)
						{
							write_to_file(&binary_buf_ptr[binary_index_per_frame], strlen(&binary_buf_ptr[binary_index_per_frame]));
							binary_index_per_frame += 16;
						}
					//	binary_read_index += 1;
					}

					if(binary_head[0] & (0x1 << 2)) // G
					{
						write_to_file("G", 1);							
						sprintf(binary_tmp_str, "%d ", binary_buf_ptr[binary_index_per_frame]);
					
						write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						binary_index_per_frame += 1;
					//	binary_read_index += 1;
					}

					if(binary_head[0] & (0x1 << 3)) // X
					{
						#if 0
						gcode_int_value = binary_buf_ptr[binary_index_per_frame + 2] + (binary_buf_ptr[binary_index_per_frame + 3] << 8)+ (binary_buf_ptr[binary_index_per_frame + 4] << 16);
						if(gcode_int_value & (1 << 23))
						{
							gcode_int_value |= 1 << 31;
						}
						if((binary_buf_ptr[binary_index_per_frame] != 0) || (binary_buf_ptr[binary_index_per_frame + 1] != 0))
						{
							
							gcode_float_value = (float)(gcode_int_value % 10) +  (float)(binary_buf_ptr[binary_index_per_frame] + (binary_buf_ptr[binary_index_per_frame + 1] << 8)) / 10000.0;
							if(gcode_int_value / 10 != 0)
								sprintf(binary_tmp_str, "%d%.5f", gcode_int_value / 10, gcode_float_value);
							else
								sprintf(binary_tmp_str, "%.5f", gcode_float_value);
							
							write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						}
						else
						{
							sprintf(binary_tmp_str, "%d", gcode_int_value);
							write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						}
						write_to_file(" ", 1);
						binary_index_per_frame += 5;
						#endif
						memcpy((char *)&gcode_float_value, &binary_buf_ptr[binary_index_per_frame], 4);
						sprintf(binary_tmp_str, "X%f ", gcode_float_value);
						write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						binary_index_per_frame += 4;
						
					//	binary_read_index += 4;
					}

					if(binary_head[0] & (0x1 << 4)) // Y
					{
						
						#if 0
						gcode_int_value = binary_buf_ptr[binary_index_per_frame + 2] + (binary_buf_ptr[binary_index_per_frame + 3] << 8)+ (binary_buf_ptr[binary_index_per_frame + 4] << 16);
						if(gcode_int_value & (1 << 23))
						{
							gcode_int_value |= 1 << 31;
						}
						if((binary_buf_ptr[binary_index_per_frame] != 0) || (binary_buf_ptr[binary_index_per_frame + 1] != 0))
						{
							
							gcode_float_value = (float)(gcode_int_value % 10) +  (float)(binary_buf_ptr[binary_index_per_frame] + (binary_buf_ptr[binary_index_per_frame + 1] << 8)) / 10000.0;
							if(gcode_int_value / 10 != 0)
								sprintf(binary_tmp_str, "%d%.5f", gcode_int_value / 10, gcode_float_value);
							else
								sprintf(binary_tmp_str, "%.5f", gcode_float_value);
							
							write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						}
						else
						{
							sprintf(binary_tmp_str, "%d", gcode_int_value);
							write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						}
						write_to_file(" ", 1);
						binary_index_per_frame += 5;
						#endif
						memcpy((char *)&gcode_float_value, &binary_buf_ptr[binary_index_per_frame], 4);
						sprintf(binary_tmp_str, "Y%f ", gcode_float_value);
						write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						binary_index_per_frame += 4;
					//	binary_read_index += 4;
					}

					if(binary_head[0] & (0x1 << 5)) // Z
					{
						#if 0
						gcode_int_value = binary_buf_ptr[binary_index_per_frame + 2] + (binary_buf_ptr[binary_index_per_frame + 3] << 8)+ (binary_buf_ptr[binary_index_per_frame + 4] << 16);
						if(gcode_int_value & (1 << 23))
						{
							gcode_int_value |= 1 << 31;
						}
						if((binary_buf_ptr[binary_index_per_frame] != 0) || (binary_buf_ptr[binary_index_per_frame + 1] != 0))
						{
							
							gcode_float_value = (float)(gcode_int_value % 10) +  (float)(binary_buf_ptr[binary_index_per_frame] + (binary_buf_ptr[binary_index_per_frame + 1] << 8)) / 10000.0;
							if(gcode_int_value / 10 != 0)
								sprintf(binary_tmp_str, "%d%.5f", gcode_int_value / 10, gcode_float_value);
							else
								sprintf(binary_tmp_str, "%.5f", gcode_float_value);
							
							write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						}
						else
						{
							sprintf(binary_tmp_str, "%d", gcode_int_value);
							write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						}
						write_to_file(" ", 1);
						binary_index_per_frame += 5;
						#endif
						memcpy((char *)&gcode_float_value, &binary_buf_ptr[binary_index_per_frame], 4);
						sprintf(binary_tmp_str, "Z%f ", gcode_float_value);
						write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						binary_index_per_frame += 4;
					//	binary_read_index += 4;
					}

					if(binary_head[0] & (0x1 << 6)) // E
					{

						#if 0
						gcode_int_value = binary_buf_ptr[binary_index_per_frame + 2] + (binary_buf_ptr[binary_index_per_frame + 3] << 8)+ (binary_buf_ptr[binary_index_per_frame + 4] << 16);
						if(gcode_int_value & (1 << 23))
						{
							gcode_int_value = gcode_int_value << 8;
							gcode_int_value /= 256;
						}
						if((binary_buf_ptr[binary_index_per_frame] != 0) || (binary_buf_ptr[binary_index_per_frame + 1] != 0))
						{
							
							gcode_float_value = (float)(gcode_int_value % 10) +  (float)(binary_buf_ptr[binary_index_per_frame] + (binary_buf_ptr[binary_index_per_frame + 1] << 8)) / 10000.0;
							if(gcode_int_value / 10 != 0)
								sprintf(binary_tmp_str, "%d%.5f", gcode_int_value / 10, gcode_float_value);
							else
								sprintf(binary_tmp_str, "%.5f", gcode_float_value);
							
							write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						}
						else
						{
							sprintf(binary_tmp_str, "%d", gcode_int_value);
							write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						}
						write_to_file(" ", 1);
						binary_index_per_frame += 5;
						#endif
						memcpy((char *)&gcode_float_value, &binary_buf_ptr[binary_index_per_frame], 4);
						sprintf(binary_tmp_str, "E%f ", gcode_float_value);
						write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						binary_index_per_frame += 4;
					//	binary_read_index += 4;
					}

					if(binary_head[1] & (0x1 << 0)) // F
					{
					
						#if 0
						if((binary_buf_ptr[binary_index_per_frame] != 0) || (binary_buf_ptr[binary_index_per_frame + 1] != 0))
						{
							gcode_int_value = binary_buf_ptr[binary_index_per_frame + 2] + (binary_buf_ptr[binary_index_per_frame + 3] << 8)+ (binary_buf_ptr[binary_index_per_frame + 4] << 16);
							gcode_float_value = (float)(gcode_int_value % 10) +  (float)(binary_buf_ptr[binary_index_per_frame] + (binary_buf_ptr[binary_index_per_frame + 1] << 8)) / 10000.0;
							if(gcode_int_value / 10 != 0)
								sprintf(binary_tmp_str, "%d%.5f", gcode_int_value / 10, gcode_float_value);
							else
								sprintf(binary_tmp_str, "%.5f", gcode_float_value);
							write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						}
						else
						{
							sprintf(binary_tmp_str, "%d", binary_buf_ptr[binary_index_per_frame + 2] + (binary_buf_ptr[binary_index_per_frame + 3] << 8)+ (binary_buf_ptr[binary_index_per_frame + 4] << 16));
							write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						}
						write_to_file(" ", 1);
						binary_index_per_frame += 5;
						#endif
						memcpy((char *)&gcode_float_value, &binary_buf_ptr[binary_index_per_frame], 4);
						sprintf(binary_tmp_str, "F%f ", gcode_float_value);
						write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						binary_index_per_frame += 4;
					//	binary_read_index += 4;
					}

					if(binary_head[1] & (0x1 << 1)) // T
					{						
						sprintf(binary_tmp_str, "T%d ", binary_buf_ptr[binary_index_per_frame]);
						write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						
						binary_index_per_frame += 1;
					//	binary_read_index += 1;
					}

					if(binary_head[1] & (0x1 << 2)) //S
					{
						#if 0
						gcode_int_value = binary_buf_ptr[binary_index_per_frame + 2] + (binary_buf_ptr[binary_index_per_frame + 3] << 8)+ (binary_buf_ptr[binary_index_per_frame + 4] << 16);
						if(gcode_int_value & (1 << 23))
						{
							gcode_int_value |= 1 << 31;
						}
						if((binary_buf_ptr[binary_index_per_frame] != 0) || (binary_buf_ptr[binary_index_per_frame + 1] != 0))
						{
							
							gcode_float_value = (float)(gcode_int_value % 10) +  (float)(binary_buf_ptr[binary_index_per_frame] + (binary_buf_ptr[binary_index_per_frame + 1] << 8)) / 10000.0;
							if(gcode_int_value / 10 != 0)
								sprintf(binary_tmp_str, "%d%.5f", gcode_int_value / 10, gcode_float_value);
							else
								sprintf(binary_tmp_str, "%.5f", gcode_float_value);
							
							write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						}
						else
						{
							sprintf(binary_tmp_str, "%d", gcode_int_value);
							write_to_file(binary_tmp_str, strlen(binary_tmp_str));
						}
						write_to_file(" ", 1);
						binary_index_per_frame += 5;
						#endif
						sprintf(binary_tmp_str, "S%d ", binary_buf_ptr[binary_index_per_frame] + (binary_buf_ptr[binary_index_per_frame+1] << 8) + (binary_buf_ptr[binary_index_per_frame + 2] << 16) + (binary_buf_ptr[binary_index_per_frame + 3] << 24));

						write_to_file(binary_tmp_str, strlen(binary_tmp_str));
					
						binary_index_per_frame += 4;
					//	binary_read_index += 4;
					}

					if(binary_head[1] & (0x1 << 3)) //P
					{							
						sprintf(binary_tmp_str, "P%d ", binary_buf_ptr[binary_index_per_frame] + (binary_buf_ptr[binary_index_per_frame+1] << 8) + (binary_buf_ptr[binary_index_per_frame + 2] << 16) + (binary_buf_ptr[binary_index_per_frame + 3] << 24));

						write_to_file(binary_tmp_str, strlen(binary_tmp_str));
					
						binary_index_per_frame += 4;
					//	binary_read_index += 4;
					}


					if(binary_index_per_frame == binary_data_len)
					{
						write_to_file("\n", 1);
					}

					
				}
				//res =	f_write (&save_File, &cmdRxBuf[0], strlen(&cmdRxBuf[0]), &num_write);
				
				/*total_write += len;*/

				
				//f_write (&file_brp, cmdRxBuf, len, &num_write);
				
				if(error_binary_head == 1)
				{
					error_binary_head = 0;
				}
				
				if(total_write >= 1000)
				{
					total_write -= 1000;
					wifi_ret_ack();
				}
				
				if(endStr != 0)
				{
					f_write (file_writer.file, file_writer.write_buf, file_writer.write_index, &num_write);
					memset(file_writer.write_buf, 0, sizeof(file_writer.write_buf));
					file_writer.write_index = 0;
					wifi_link_state = WIFI_CONNECTED;	
					f_close(&save_File);
				//	f_close(&file_brp);
					wifi_loop_cycle = 500;
					printerStaus == pr_idle;
					clear_cur_ui();
					//draw_dialog_filetransfer(1);
					draw_ready_print();
				}
			}
			else	//assic
			{
				#if 1
				if(endStr != 0)
				{
					res =	f_write (&save_File, &cmdRxBuf[0], endStr - (&cmdRxBuf[0]), &num_write);
					wifi_link_state = WIFI_CONNECTED;	
					f_close(&save_File);
					wifi_loop_cycle = 500;
					clear_cur_ui();
					//draw_dialog_filetransfer(1);
					draw_ready_print();
				}
				else
				{
					res =	f_write (&save_File, &cmdRxBuf[0], strlen(&cmdRxBuf[0]), &num_write);
				}
				total_write += strlen(&cmdRxBuf[0]);
				//if((res == 0) && (total_write >= 512))
				if(res == 0)
				{
					//total_write -= 512;
					wifi_ret_ack();
				}
				#endif
			}
			
			return;
		}

		
		init_queue(&cmd_queue);
		cr_index = 0;
		j = 0;
		
		while(1)
		{
			if((cmdRxBuf[j] == '\r') || (cmdRxBuf[j] == '\n'))
			{
				if(strlen(cmd_line) > 1)
				{
					cmd_line[cr_index] = '\n';
					push_queue(&cmd_queue, cmd_line, strlen(cmd_line));
				}
				memset(cmd_line, 0, sizeof(cmd_line));
				cr_index = 0;
			}
			else if(cmdRxBuf[j] == '\0')
				break;
			else
			{
				cmd_line[cr_index] = cmdRxBuf[j];
				cr_index++;
			}

			j++;
			if(j >= len)
				break;
		}
		

		while(pop_queue(&cmd_queue, cmd_line, sizeof(cmd_line)) >= 0)		
		{
			inc_flag = 0;	

			if(wifi_link_state == WIFI_CONFIG_OK)
			{
				#ifndef ESP_MODEL
				tmpStr = (int8_t *)strstr(&cmd_line[0], "at+net_wanip=?");
				#else
				tmpStr = (int8_t *)strstr(&cmd_line[0], "AT+CIFSR");
				#endif
				if(tmpStr)
				{
					wait_ip_back_flag = 1;				
					continue;
				}
			}

			if(wait_ip_back_flag)
			{
				#ifndef ESP_MODEL
				tmpStr = &cmd_line[0];
				#else
				tmpStr = (int8_t *)strstr(cmd_line, "CIFSR:");
				if(tmpStr == 0)
				{
					continue;
				}
				tmpStr = (int8_t *)strstr(cmd_line, "\"");
				if(tmpStr == 0)
				{
					continue;
				}
				tmpStr++;
				#endif
				if((*tmpStr >= '0') && (*tmpStr <= '9'))
				{
					int ip_index = 0;
					while((*(tmpStr + ip_index) == '.') || ((*(tmpStr + ip_index) >= '0') && (*(tmpStr + ip_index) <= '9')))
					{
						ipPara.ip_addr[ip_index] = *(tmpStr + ip_index);

						ip_index++;
						
						if(ip_index >= sizeof(ipPara.ip_addr))
							break;
							
					}
					ipPara.ip_addr[ip_index] = '\0';
					if((ipPara.ip_addr[0] != '0'))
					{
						wifi_link_state = WIFI_GET_IP_OK;
					}
					
				}
				wait_ip_back_flag = 0;
			
			}
			
			if(wifi_link_state == WIFI_GET_IP_OK)
			{
				if(gCfgItems.wifi_mode_sel == 1)
				{
					wifi_link_state = WIFI_RECONN;
				}
				else
				{				
					#ifndef ESP_MODEL
					tmpStr = (int8_t *)strstr(&cmd_line[0], "Connected");
					if(tmpStr)
					{
						wifi_link_state = WIFI_RECONN;
					}
					#else
					tmpStr = (int8_t *)strstr(&cmd_line[0], "STATUS:");
					if(tmpStr)
					{
					////////	if( (*(tmpStr + 7) >= '2') && (*(tmpStr + 7) <= '4'))
					//	if((ipPara.ip_addr[0] != '0'))
						{
							wifi_link_state = WIFI_RECONN;
							raw_send_to_wifi("AT+CIPMUX=1\r\n", strlen("AT+CIPMUX=1\r\n"));
						}
					}
					#endif
				}
				
			}
			

			

			if(wifi_link_state >= WIFI_CONNECTED)
			{

				wifi_gcode_exec(cmd_line);
				
				
			}
		}
	}

	
}

int32_t tick_net_time1, tick_net_time2;

static int esp_dma_pre(volatile uint8_t *pBuffer, uint32_t NumByteToRead)
{
	/*if ((SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == SET))
	{
		SPI_I2S_SendData(SPI2, 0x12);
	}*/
	DMA1_Channel5->CCR &= ~( 1 << 0 ) ;         //关闭DMA通道5
	DMA1_Channel5->CMAR = (u32)pBuffer ; //设置DMA存储器地址，注意MSIZE

	DMA1_Channel5->CNDTR = 0x0000   ;           //传输数量寄存器清零
	DMA1_Channel5->CNDTR = NumByteToRead ;         //传输数量设置为buffersize个

	DMA1->IFCR = 0xF0000 ;                         //清除通道5的标志位
	
	DMA1_Channel5->CCR |= 1 << 0 ;              //开启DMA通道5
	
	//while((DMA1->ISR & (0x200))== 0); //not polling , use interrupt

	//DMA1_Channel3->CCR &= ~( 1 << 0 ) ;         //关闭DMA通道3

	return 1;
	
}

static int storeRcvData(uint8_t *bufToCpy, int32_t len)
{
	unsigned char tmpW = wifiDmaRcvFifo.write_cur;
	
	if(len > UDISKBUFLEN)
		return 0;
	
	if(wifiDmaRcvFifo.state[tmpW] == udisk_buf_empty)
	{
		memcpy((unsigned char *)wifiDmaRcvFifo.bufferAddr[tmpW], (uint8_t *)bufToCpy, len);
		wifiDmaRcvFifo.state[tmpW] = udisk_buf_full;
		wifiDmaRcvFifo.write_cur = (tmpW + 1) % TRANS_RCV_FIFO_BLOCK_NUM;
		return 1;
	}
	else
		return 0;
	
}

int32_t readWifiFifo(uint8_t *retBuf, uint32_t bufLen)
{
	unsigned char tmpR = wifiDmaRcvFifo.read_cur;
	
	if(bufLen < UDISKBUFLEN)
		return 0;

	if(wifiDmaRcvFifo.state[tmpR] == udisk_buf_full)
	{		

		memcpy(retBuf, (unsigned char *)wifiDmaRcvFifo.bufferAddr[tmpR], UDISKBUFLEN);

		wifiDmaRcvFifo.state[tmpR] = udisk_buf_empty;

		wifiDmaRcvFifo.read_cur = (tmpR + 1) % TRANS_RCV_FIFO_BLOCK_NUM;

		 return UDISKBUFLEN;
	}
	else
		return 0;
}

void stopEspTransfer()
{
	char state;
	
	if(wifi_link_state == WIFI_TRANS_FILE)
		wifi_link_state = WIFI_CONNECTED;
					
	f_close(&save_File);
	f_close(&save_File);

	if(upload_result != 3)
	{
		//state = 0x01;
		wifiTransError.flag = 1;
		wifiTransError.start_tick = getTick();
		f_unlink((const char *)saveFilePath);
		
	}
	else
	{
		//state = 0x02;
	}
	//package_to_wifi(WIFI_EXCEP_INF, &state, 1);

	wifi_delay(200);

	//GPIO_SetBits(GPIOF, GPIO_Pin_1);
        WIFI_IO1_SET();
	
	
//	wifi_deInit();			
	//exchangeFlashMode(1);  //change spi flash to use dma mode

//	WIFI_COM.end();
//	WIFI_COM.begin(115200, true);
	esp_port_begin(1);
	
	//GPIO_ResetBits(GPIOF, GPIO_Pin_1);
	if(wifiTransError.flag != 0x1)
	{
		WIFI_IO1_RESET();
	}
}

void wifi_rcv_handle()
{
	 int8_t result;
	 int32_t len = 0;
	 uint8_t ucStr[(UART_RX_BUFFER_SIZE) + 1] = {0};
	 uint8_t tmpBuffer1[30];
	 int8_t getDataF = 0;
	 
   //	memset(ucStr, 0, sizeof(ucStr));
	
	if(gCfgItems.wifi_type == ESP_WIFI)
	{
		if(wifi_link_state == WIFI_TRANS_FILE)
		{
			len = readWifiFifo(ucStr, UART_RX_BUFFER_SIZE);
			if(len > 0)
			{
				esp_data_parser((char *)ucStr, len);
				if(wifi_link_state == WIFI_CONNECTED)
				{
					clear_cur_ui();
					
					draw_dialog(DIALOG_TYPE_UPLOAD_FILE);
					stopEspTransfer();

					

				}
				getDataF = 1;
			}
			
			if(esp_state == TRANSFER_STORE)
			{
				if(storeRcvData(WifiRxFifo.uartTxBuffer, UART_RX_BUFFER_SIZE))
				{
					esp_state = TRANSFERING;
					
					esp_dma_pre(WifiRxFifo.uartTxBuffer, UART_RX_BUFFER_SIZE);

					//wifi_ret_ack();

					//GPIO_ResetBits(GPIOF, GPIO_Pin_1);
					if(wifiTransError.flag != 0x1)
					{
						WIFI_IO1_RESET();
					}
				}
				else
				{
					//GPIO_SetBits(GPIOF, GPIO_Pin_1);
	                                WIFI_IO1_SET();
				}
			}
			
		}
		
		else	
		{
			len = readUsartFifo(&WifiRxFifo, ucStr, UART_RX_BUFFER_SIZE);
			if(len > 0)
			{
				
				esp_data_parser((char *)ucStr, len);
				if(wifi_link_state == WIFI_TRANS_FILE) // rcv file first frame
				{
				//	exchangeFlashMode(0);  //change spi flash not use dma mode
					
					wifi_delay(10);
					
					esp_port_begin(0);
					//esp_dma_pre(WifiRxFifo.uartTxBuffer, UART_RX_BUFFER_SIZE);
					wifi_delay(10);

					tick_net_time1 = 0;
					
				}
				
				//GPIO_ResetBits(GPIOF, GPIO_Pin_1);
				if(wifiTransError.flag != 0x1)
				{
					WIFI_IO1_RESET();
				}
				getDataF = 1;
			}
		}

		if(getDataF == 1)
		{
			
			tick_net_time1 = getTick();
			
		}	
		 else
		 {
		 
			tick_net_time2 = getTick();
			if(wifi_link_state == WIFI_TRANS_FILE)
			{
				if((tick_net_time1 != 0) && (getTickDiff(tick_net_time1, tick_net_time2) > 4500)) // transfer timeout
				{	
					wifi_link_state = WIFI_CONNECTED;

					upload_result = 2;

					clear_cur_ui();
					
					stopEspTransfer();

					
					
					draw_dialog(DIALOG_TYPE_UPLOAD_FILE);

				}
			}
			if((tick_net_time1 != 0) && (getTickDiff(tick_net_time1, tick_net_time2) > 10000)) // heart beat timeout
			{	
				wifi_link_state = WIFI_NOT_CONFIG;
				
			//	wifi_reset();

			//	tick_net_time1 = getTick();

			}
			if((tick_net_time1 != 0) && (getTickDiff(tick_net_time1, tick_net_time2) > 120000)) // reset
			{	
				wifi_link_state = WIFI_NOT_CONFIG;

				//HC-chen
				if(gCfgItems.wifi_state==1)
				{
					wifi_reset();
				}

				tick_net_time1 = getTick();

			}
			
		
		 }

		 
		if(wifiTransError.flag == 0x1)
		{
			wifiTransError.now_tick = getTick();
			if(getTickDiff(wifiTransError.start_tick, wifiTransError.now_tick) > WAIT_ESP_TRANS_TIMEOUT_TICK)
			{
				wifiTransError.flag = 0;
				WIFI_IO1_RESET();
			}
		}
		
	}
	else if(gCfgItems.wifi_type == HLK_WIFI)
	{
		len = readUsartFifo(&WifiRxFifo, ucStr, UART_RX_BUFFER_SIZE);
		if(len > 0)
		{
			hlk_data_parser(ucStr, len);
			getDataF = 1;
		}
		if(getDataF == 1)
		{
			if(wifi_link_state == WIFI_TRANS_FILE)
			{
				tick_net_time1 = getTick();
			}
		}	
		 else
		 {
		 
			tick_net_time2 = getTick();
			if((wifi_link_state == WIFI_TRANS_FILE) && (getTickDiff(tick_net_time1, tick_net_time2) > 50000)) // 50s timeout
			{
			
				wifi_link_state = WIFI_CONNECTED;
				wifi_loop_cycle = 500;
				
				file_writer.write_index = 0;
				memset(file_writer.write_buf, 0, sizeof(file_writer.write_buf));
				
				f_close(file_writer.file);			
				if(gCfgItems.fileSysType == FILE_SYS_SD)
				{
					memset(tmpBuffer1, 0, sizeof(tmpBuffer1));
					sprintf(tmpBuffer1, "1:/%s", file_writer.saveFileName);
				}
				else if(gCfgItems.fileSysType == FILE_SYS_USB)
				{
					memset(tmpBuffer1, 0, sizeof(tmpBuffer1));
					sprintf(tmpBuffer1, "0:/%s", file_writer.saveFileName);
				}
				
				f_unlink(tmpBuffer1);			

				clear_cur_ui();
				draw_dialog_filetransfer(0);
			}
		 }
	}

	if(left_to_send)
	{
		if(checkFIFO(&gcodeCmdTxFIFO) != fifo_full)
		{
			left_to_send = 0;
			pushFIFO(&gcodeCmdTxFIFO, left_to_save);
			send_to_wifi("ok\r\n", strlen("ok\r\n"));
		}
	}

	 
	
}

void wifi_looping()
{
	IP_PARA *ip_para = &ipPara;
	WIFI_PARA  *wifi_para = &wifiPara;
	
	char cfg_buf[100];

	if(gCfgItems.wifi_type == ESP_WIFI)
	{
		do
		 {
		 	wifi_rcv_handle();
			//GUI_Exec();
			//GUI_TOUCH_Exec();	
			//IWDG_ReloadCounter();    
		 } 
		 while(wifi_link_state == WIFI_TRANS_FILE);
		 return;
	}
	else
	{

		if(wifi_loop_time == 0)
		{
			return;
		}

		wifi_loop_time = 0;
		if(gCfgItems.wifi_mode_sel != 1)//client ??
		{
			switch(wifi_link_state)
			{
				case WIFI_NOT_CONFIG:
					#ifndef ESP_MODEL
					GPIO_ResetBits(GPIOA, GPIO_Pin_8);	
					wifi_link_state = WIFI_CONFIG_MODE;
					#else
					wifi_link_state = WIFI_CONFIG_MODE;
					#endif
					break;

				case WIFI_CONFIG_MODE:

					#ifndef ESP_MODEL
					GPIO_SetBits(GPIOA, GPIO_Pin_8);
					/*wifi mode*/
					memset(cfg_buf, 0, sizeof(cfg_buf));
					sprintf(cfg_buf, "at+netmode=%d\r\n", WIFI_MODE);	
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_DHCP;
					#else
					raw_send_to_wifi("AT+CWMODE=1\r\nAT+RST\r\n", strlen("AT+CWMODE=2\r\nAT+RST\r\n")); // 1:STATION MODE   2:AP MODE
					wifi_link_state = WIFI_CONFIG_AP;
					#endif
					break;

				case WIFI_CONFIG_DHCP:
					/*dhcp*/
					memset(cfg_buf, 0, sizeof(cfg_buf));
					sprintf(cfg_buf, "at+dhcpc=%d\r\n", ip_para->dhcp_flag);	
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_AP;
					break;

				case WIFI_CONFIG_AP:
					/*ap*/
					memset(cfg_buf, 0, sizeof(cfg_buf));

					#ifndef ESP_MODEL
					if(wifi_para->decodeType == 1)
					{
						sprintf(cfg_buf, "at+wifi_conf=%s,auto,%s\r\n", wifi_para->ap_name,  wifi_para->keyCode);	
					}
					else if(wifi_para->decodeType == 2)
					{
						sprintf(cfg_buf, "at+wifi_conf=%s,wpawpa2_aes,%s\r\n", wifi_para->ap_name,  wifi_para->keyCode);	
					}
				
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
				
					#else
					sprintf(cfg_buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", wifi_para->ap_name,  wifi_para->keyCode);				
				
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_loop_time = 2000;
				
					#endif
				
					wifi_link_state = WIFI_CONFIG_IP_INF;
					break;

				case WIFI_CONFIG_IP_INF:
					/*ip inf*/
					{
						char *tmpStr;
						memset(cfg_buf, 0, sizeof(cfg_buf));
						#ifndef ESP_MODEL
						sprintf(cfg_buf, "at+net_ip=%s,%s,%s\r\n", ip_para->ip_addr,  ip_para->mask,  ip_para->gate);	
						wifi_link_state = WIFI_CONFIG_DNS;
						raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
						#else
					//	strcpy(cfg_buf, "AT+CIPMUX=1\r\n");
						wifi_link_state = WIFI_CONFIG_SERVER;
					//	raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
						#endif

						
						/*gCfgItems.ipAddr[0] = atoi(ip_para->ip_addr);
						tmpStr = (int8_t *)strstr(ip_para->ip_addr, ".");
						if(tmpStr != 0)
						{
							tmpStr += 1;
							gCfgItems.ipAddr[1] = atoi(tmpStr);

							tmpStr = (int8_t *)strstr(tmpStr, ".");
							if(tmpStr != 0)
							{
								tmpStr += 1;
								gCfgItems.ipAddr[2] = atoi(tmpStr);

								tmpStr = (int8_t *)strstr(tmpStr, ".");
								if(tmpStr != 0)
								{
									tmpStr += 1;
									gCfgItems.ipAddr[3] = atoi(tmpStr);
								}
							}
						}*/

					}
					break;

				case WIFI_CONFIG_DNS:
					/*dns*/
					memset(cfg_buf, 0, sizeof(cfg_buf));
					sprintf(cfg_buf, "at+net_dns=%s,8.8.8.8\r\n", ip_para->dns);	
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_TCP;
					break;

				case WIFI_CONFIG_TCP:
					memset(cfg_buf, 0, sizeof(cfg_buf));	
					strcpy(cfg_buf, "at+remotepro=tcp\r\n");
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_SERVER;
					break;

			case WIFI_CONFIG_SERVER:
				memset(cfg_buf, 0, sizeof(cfg_buf));	
				#ifndef ESP_MODEL
				strcpy(cfg_buf, "at+mode=server\r\n");
				wifi_link_state = WIFI_CONFIG_REMOTE_PORT;
				#else
				strcpy(cfg_buf, "AT+CIPSTO=0\r\n");
				wifi_link_state = WIFI_CONFIG_OK;
				#endif
				
				raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
				
				break;

				case WIFI_CONFIG_REMOTE_PORT:
					memset(cfg_buf, 0, sizeof(cfg_buf));	
					strcpy(cfg_buf, "at+remoteport=8080\r\n");
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_BAUD;
					break;

				case WIFI_CONFIG_BAUD:
					memset(cfg_buf, 0, sizeof(cfg_buf));	
					sprintf(cfg_buf, "at+uart=%d,8,n,1\r\n", wifi_para->baud);	
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_COMMINT;
					break;

				case WIFI_CONFIG_COMMINT:
					/*commit and reconnect*/
					memset(cfg_buf, 0, sizeof(cfg_buf));	
					strcpy(cfg_buf, "at+net_commit=1\r\n");
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));	
					wifi_link_state = WIFI_CONFIG_OK;
					break;

			case WIFI_CONFIG_OK:
				#ifndef ESP_MODEL
				raw_send_to_wifi("at+net_wanip=?\r\n", strlen("at+net_wanip=?\r\n"));
				#else
				raw_send_to_wifi("AT+CIFSR\r\n", strlen("AT+CIFSR\r\n"));
				#endif
				wifi_rcv_handle();
				break;

				case WIFI_GET_IP_OK:
					#ifndef ESP_MODEL
						if(wifi_check_time)
						 {
						 	wifi_check_time = 0;
							raw_send_to_wifi("at+wifi_ConState=?\r\n", strlen("at+wifi_ConState=?\r\n"));
					
						 }
					 #else
						if(wifi_check_time)
						 {
						 	wifi_check_time = 0;
							raw_send_to_wifi("AT+CIPSTATUS\r\n", strlen("AT+CIPSTATUS\r\n"));
						
						 }
					#endif	
					wifi_rcv_handle();
					break;


				case WIFI_RECONN:
					#ifndef ESP_MODEL
					raw_send_to_wifi("at+reconn=1\r\n", strlen("at+reconn=1\r\n"));
					#else
					raw_send_to_wifi("AT+CIPSERVER=1,8080\r\n", strlen("AT+CIPSERVER=1,8080\r\n"));
					#endif
					wifi_link_state = WIFI_CONNECTED;
					wifi_connect_flg = 1;
				case WIFI_CONNECTED:
				case WIFI_WAIT_TRANS_START:
				case WIFI_TRANS_FILE:
					wifi_rcv_handle();
					//wifi_snd_handle();
					break;
					
			}
		}
		else//AP??
		{
			switch(wifi_link_state)
			{
				case WIFI_NOT_CONFIG:
					GPIO_ResetBits(GPIOA, GPIO_Pin_8);	
					wifi_link_state = WIFI_CONFIG_MODE;
					break;
			
				case WIFI_CONFIG_MODE:
					GPIO_SetBits(GPIOA, GPIO_Pin_8);
					/*wifi mode*/
					memset(cfg_buf, 0, sizeof(cfg_buf));
					sprintf(cfg_buf, "at+netmode=%d\r\n", WIFI_AP_MODE);	
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_AP;
					break;
			
				case WIFI_CONFIG_AP:
					/*ap*/
					memset(cfg_buf, 0, sizeof(cfg_buf));
					if(wifi_para->decodeType == 1)
					{
						sprintf(cfg_buf, "at+wifi_conf=%s,auto,%s\r\n", wifi_para->ap_name,  wifi_para->keyCode);	
					}
					else if(wifi_para->decodeType == 2)
					{
						sprintf(cfg_buf, "at+wifi_conf=%s,wpawpa2_aes,%s\r\n", wifi_para->ap_name,  wifi_para->keyCode);	
					}
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_DHCP;
					break;
					
				case WIFI_CONFIG_DHCP:
					/*dhcp*/
					memset(cfg_buf, 0, sizeof(cfg_buf));
					sprintf(cfg_buf, "at+dhcpc=%d\r\n", ip_para->dhcp_flag);	
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_DHCPD;
					break;	
						
				case WIFI_CONFIG_DHCPD:
					/*dhcpd*/
					memset(cfg_buf, 0, sizeof(cfg_buf));
					sprintf(cfg_buf, "at+dhcpd=%d\r\n",ip_para->dhcpd_flag);	
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_COFIG_DHCPD_IP;
					break;
				case WIFI_COFIG_DHCPD_IP:
					memset(cfg_buf, 0, sizeof(cfg_buf));
					sprintf(cfg_buf, "at+dhcpd_ip=%s,%s,%s,%s\r\n", ip_para->start_ip_addr,ip_para->end_ip_addr,ip_para->dhcpd_mask,ip_para->dhcpd_gate);
					//sprintf(cfg_buf, "at+dhcpd_ip=192.168.1.100,192.168.1.200,255.255.255.0,192.168.1.110\r\n");
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_COFIG_DHCPD_DNS;
					break;
				case WIFI_COFIG_DHCPD_DNS:
					memset(cfg_buf, 0, sizeof(cfg_buf));
					sprintf(cfg_buf, "at+dhcpd_dns=%s,8.8.8.8\r\n", ip_para->dhcpd_dns);
					//sprintf(cfg_buf, "at+dhcpd_dns=192.168.1.110,8.8.8.8\r\n");
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_IP_INF;			
					break;	
					
				case WIFI_CONFIG_IP_INF:
					/*ip inf*/
					{
						char *tmpStr;
						memset(cfg_buf, 0, sizeof(cfg_buf));
						sprintf(cfg_buf, "at+net_ip=%s,%s,%s\r\n", ip_para->dhcpd_ip,  ip_para->dhcpd_mask,  ip_para->dhcpd_gate);	
						//sprintf(cfg_buf, "at+net_ip=192.168.1.110,255.255.255.0,192.168.1.110\r\n");	
						raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
						wifi_link_state = WIFI_CONFIG_DNS;
					}
					break;
			
				case WIFI_CONFIG_DNS:
					/*dns*/
					memset(cfg_buf, 0, sizeof(cfg_buf));
					sprintf(cfg_buf, "at+net_dns=%s,8.8.8.8\r\n", ip_para->dhcpd_dns);	
					//sprintf(cfg_buf, "at+net_dns=192.168.1.110,8.8.8.8\r\n");
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_TCP;
					break;
			
				case WIFI_CONFIG_TCP:
					memset(cfg_buf, 0, sizeof(cfg_buf));	
					strcpy(cfg_buf, "at+remotepro=tcp\r\n");
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_SERVER;
					break;
			
				case WIFI_CONFIG_SERVER:
					memset(cfg_buf, 0, sizeof(cfg_buf));	
					strcpy(cfg_buf, "at+mode=server\r\n");
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_REMOTE_PORT;
					break;
			
				case WIFI_CONFIG_REMOTE_PORT:
					memset(cfg_buf, 0, sizeof(cfg_buf));	
					strcpy(cfg_buf, "at+remoteport=8080\r\n");
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_BAUD;
					break;
			
				case WIFI_CONFIG_BAUD:
					memset(cfg_buf, 0, sizeof(cfg_buf));	
					sprintf(cfg_buf, "at+uart=%d,8,n,1\r\n", wifi_para->baud);	
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf));
					wifi_link_state = WIFI_CONFIG_COMMINT;
					break;
			
				case WIFI_CONFIG_COMMINT:
					/*commit and reconnect*/
					memset(cfg_buf, 0, sizeof(cfg_buf));	
					strcpy(cfg_buf, "at+net_commit=1\r\n");
					raw_send_to_wifi(cfg_buf, strlen(cfg_buf)); 
					wifi_link_state = WIFI_CONFIG_OK;
					break;
			
				case WIFI_CONFIG_OK:
					raw_send_to_wifi("at+net_wanip=?\r\n", strlen("at+net_wanip=?\r\n"));
					wifi_rcv_handle();
					break;
			
				case WIFI_GET_IP_OK:
					if(wifi_check_time)
					 {
						wifi_check_time = 0;
						raw_send_to_wifi("at+wifi_ConState=?\r\n", strlen("at+wifi_ConState=?\r\n"));
						
					 }
					
					wifi_rcv_handle();
					break;
			
				case WIFI_RECONN:			
					raw_send_to_wifi("at+reconn=1\r\n", strlen("at+reconn=1\r\n"));
					wifi_link_state = WIFI_CONNECTED;
					wifi_connect_flg = 1;
				case WIFI_CONNECTED:
				case WIFI_WAIT_TRANS_START:
				case WIFI_TRANS_FILE:
					wifi_rcv_handle();
					//wifi_snd_handle();
					break;
					
			}
		}
	}

}



#define USART1_DR_Addr ((u32)0x40013804)


void DMA1_Channel5_IRQHandler()
{
	if(DMA_GetITStatus(DMA1_IT_HT5))
	{
		DMA_ClearITPendingBit(DMA1_IT_HT5);
		//GPIO_SetBits(GPIOF, GPIO_Pin_1);	//stm32 clear ready signal before the transfer is finished
		WIFI_IO1_SET();

	}
		
	if(DMA_GetITStatus(DMA1_IT_TC5))
	{
		DMA_ClearITPendingBit(DMA1_IT_TC5);
		
		if(esp_state == TRANSFER_IDLE)
		{
			esp_state = TRANSFERING;
		}
		
		if(storeRcvData(WifiRxFifo.uartTxBuffer, UART_RX_BUFFER_SIZE))
		{
			
			esp_dma_pre(WifiRxFifo.uartTxBuffer, UART_RX_BUFFER_SIZE);

			//GPIO_ResetBits(GPIOF, GPIO_Pin_1);
                      if(wifiTransError.flag != 0x1)
			{
				WIFI_IO1_RESET();
			}
		}
		else
		{
			//GPIO_SetBits(GPIOF, GPIO_Pin_1);
                        WIFI_IO1_SET();
			esp_state = TRANSFER_STORE;
		}
			
		

		//esp_dma_pre(WifiRxFifo.uartTxBuffer, UART_RX_BUFFER_SIZE);
		
	}



}



void wifi_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	uint32_t flash_inf_valid_flag = 0;

	
	//gCfgItems.wifi_type = HLK_WIFI;

	wifi_link_state = WIFI_NOT_CONFIG;
	
	
	/*hardware init*/
	
	/*PA8 for both hlk and esp rst*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	WIFI_SET();

	if(gCfgItems.wifi_type == ESP_WIFI) // esp wifi
	{
		/*PD9先作为输入，升级时候作为输出*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE );	 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		/*PD8作为输出，用于指示STM32准备好传送*/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		WIFI_IO1_SET();
	}

	if(gCfgItems.wifi_type == HLK_WIFI)
	{
		SZ_STM32_COMInit(COM3, 115200);

		USART_ITConfig(SZ_STM32_COM3, USART_IT_RXNE, ENABLE);

		NVIC_InitStructure.NVIC_IRQChannel = SZ_STM32_COM3_IRQn;
	//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

		NVIC_Init(&NVIC_InitStructure);

	}
	else
	{
		FRESULT res;
		int update_flag = 0;
		
		esp_state = TRANSFER_IDLE;

		esp_port_begin(1);		

		wifi_reset();
				

		res = f_open(&esp_upload.uploadFile, ESP_FIRMWARE_FILE,  FA_OPEN_EXISTING | FA_READ);

		if(res ==  FR_OK) 
		{
			f_close(&esp_upload.uploadFile);

			wifi_delay(2000);

			if(usartFifoAvailable(&WifiRxFifo) < 20)
			{
				return;
			}

			clear_cur_ui();

			draw_dialog(DIALOG_TYPE_UPDATE_ESP_FIRMARE);
			
			if(wifi_upload(0) >= 0)
			{
				
				f_unlink("1:/MKS_WIFI_CUR");
				f_rename(ESP_FIRMWARE_FILE,"/MKS_WIFI_CUR");
			}
			draw_return_ui();
			
			update_flag = 1;
		}
		if(update_flag == 0)
		{
			res = f_open(&esp_upload.uploadFile, ESP_WEB_FIRMWARE_FILE,  FA_OPEN_EXISTING | FA_READ);

			if(res ==  FR_OK) 
			{
				f_close(&esp_upload.uploadFile);

				wifi_delay(2000);

				if(usartFifoAvailable(&WifiRxFifo) < 20)
				{
					return;
				}

				clear_cur_ui();

				draw_dialog(DIALOG_TYPE_UPDATE_ESP_FIRMARE);
				if(wifi_upload(1) >= 0)
				{					
				
					f_unlink("1:/MKS_WIFI_CUR");
					f_rename(ESP_WEB_FIRMWARE_FILE,"/MKS_WIFI_CUR");
				}
				draw_return_ui();
				update_flag = 1;
			}
			
		}
		if(update_flag == 0)
		{
			res = f_open(&esp_upload.uploadFile, ESP_WEB_FILE,  FA_OPEN_EXISTING | FA_READ);
			if(res ==  FR_OK) 
			{
				f_close(&esp_upload.uploadFile);

				wifi_delay(2000);

				if(usartFifoAvailable(&WifiRxFifo) < 20)
				{
					return;
				}

				clear_cur_ui();
				
				draw_dialog(DIALOG_TYPE_UPDATE_ESP_DATA);

				if(wifi_upload(2) >= 0)
				{								

					f_unlink("1:/MKS_WEB_CONTROL_CUR");
					f_rename(ESP_WEB_FILE,"/MKS_WEB_CONTROL_CUR");
				}
				draw_return_ui();
			}
		}
		

	}


/*
	SPI_FLASH_BufferRead(&flash_inf_valid_flag,FlASH_INF_VALID_ADDR,4);
	if(flash_inf_valid_flag == FLASH_INF_VALID_FLAG)
	{
		SPI_FLASH_BufferRead(wifiPara.ap_name,WIFI_NAME_ADDR,200);
		SPI_FLASH_BufferRead(wifiPara.keyCode,WIFI_KEYCODE_ADDR,200);
		SPI_FLASH_BufferRead(ipPara.ip_addr,WIFI_IP_ADDR,15);
		SPI_FLASH_BufferRead(ipPara.mask,WIFI_MASK_ADDR,15);
		SPI_FLASH_BufferRead(ipPara.gate,WIFI_GATE_ADDR,15);
		SPI_FLASH_BufferRead((uint8_t*)&ipPara.dhcp_flag,WIFI_DHCP_FLAG_ADDR,1);
		SPI_FLASH_BufferRead((uint8_t*)&gCfgItems.wifi_mode_sel,WIFI_MODE_SEL_ADDR,1);	
		SPI_FLASH_BufferRead((uint8_t*)&gCfgItems.wifi_type,WIFI_MODE_TYPE_ADDR,1);
	}
	
*/
	wifiPara.decodeType = WIFI_DECODE_TYPE;
	wifiPara.baud = 115200;
	
	wifi_link_state = WIFI_NOT_CONFIG;

	//HC-chen
	if(gCfgItems.wifi_state==0)
	{
		WIFI_RESET();
	}
	
}


