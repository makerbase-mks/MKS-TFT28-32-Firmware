/******************** (C) COPYRIGHT 2009 www.armjishu.com ************************
* File Name          : ARMJISHU_TouchScreen_ADS7843.c
* Author             : www.armjishu.com Team
* Version            : V3.0.1
* Date               : 03/20/2010
* Description        : 
                        ADS7843_CS   is PB1
                        ADS7843_INT  is PC1
*******************************************************************************/
#include "SZ_TouchScreen.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "SZ_STM32F107VC_LIB.h"
#include "stm3210c_eval_lcd.h"


//extern u16 DeviceCode;

/*#define  EVENT_FIFO_DEPTH	20
typedef struct
{	
	TOUCH_EVENT event[EVENT_FIFO_DEPTH];
	int8_t  r_idx;
	int8_t  w_idx;
	
} EVENT_FIFO;

static EVENT_FIFO  touch_event_fifo;*/

static int16_t x_touch,  y_touch;

static int32_t touch_time = 0;

static uint8_t lastTouchState;

static TOUCH_EVENT  TouchEvent;

#define  times  4


// A/D 通道选择命令字和工作寄存器
#define	CHX 	0x90//0x90 	//通道Y+的选择控制字	//0x94
#define	CHY 	0xD0//0xd0	//通道X+的选择控制字	//0xD4

//#define ADS7843_WrCmd  SPI1_SendByte
//#define ADS7843_Read   SPI1_RecvByte

void ADS7843_CS_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable GPIOB, GPIOC and AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_ADS7843_CS , ENABLE);  //RCC_APB2Periph_AFIO
  
  /* LEDs pins configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_ADS7843_CS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_ADS7843_CS_PORT, &GPIO_InitStructure);
}

static void ADS7843_INT_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable GPIOB, GPIOC and AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_ADS7843_INT , ENABLE);  //RCC_APB2Periph_AFIO
  
  /* LEDs pins configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_ADS7843_INT;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIO_ADS7843_INT_PORT, &GPIO_InitStructure);
}

static void ADS7843_INT_EXIT_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_ADS7843_EXTI_PORT_SOURCE, GPIO_ADS7843_EXTI_PIN_SOURCE);  

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = GPIO_ADS7843_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

}


/*******************************************************************************
* Function Name  : InterruptConfig
* Description    : Configures the used IRQ Channels and sets their priority.NVIC_Configuration
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void ADS7843_InterruptConfig(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Set the Vector Table base address at 0x08000000 */
  //2016 2 22
  //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
  
  /* Configure the Priority Group to 2 bits */
	//2016 2 22
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Enable the EXTI5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = GPIO_ADS7843_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*20160222
static void LCD_BIG_POINT(u16 xScreen, u16 yScreen)
 {
   ili9320_SetPoint(xScreen, yScreen, Magenta);
   ili9320_SetPoint(xScreen-1, yScreen, Magenta);
   ili9320_SetPoint(xScreen+1, yScreen, Magenta);
   ili9320_SetPoint(xScreen, yScreen-1, Magenta);
   ili9320_SetPoint(xScreen, yScreen+1, Magenta);
   ili9320_SetPoint(xScreen-1, yScreen-1, Magenta);
   ili9320_SetPoint(xScreen-1, yScreen+1, Magenta);
   ili9320_SetPoint(xScreen+1, yScreen-1, Magenta);
   ili9320_SetPoint(xScreen+1, yScreen+1, Magenta);

  }
*/
/*=====================================================================*/
u16 _AD2Y(u16 adx) //240
{
  u16 sx=0;
  int r = adx - 200;
  r *= 240;
  sx=r / (4000 - 280);
  if (sx<=0 || sx>240)
    return 0;
  return sx;
}


u16 _AD2X(u16 ady) //320
{
  u16 sy=0;
  int r = ady - 260;
  r *= 320;
  sy=r/(3960 - 360);
  if (sy<=0 || sy>320)
    return 0;
  return sy;
}


/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
// A/D 通道选择命令字和工作寄存器
//#define	CHX 	0x90 	//通道Y+的选择控制字	//0x94
//#define	CHY 	0xd0	//通道X+的选择控制字	//0xD4



//读ADS7843的IO控制口
	
/*=====================================================================*/

//void ADS7843_Init(void)
void SZ_TS_Init()

{
    ADS7843_CS_config();     // 使能LCD
    ADS7843_CS_HIGH() ;     // 关闭LCD
    SPI1_Config();
    SPI1_Init_For_Byte();
    SPI1_MOSI_HIGH();
    SPI1_SCK_LOW();
   // ADS7843_INT_config();
   // ADS7843_INT_EXIT_Init();
   // ADS7843_InterruptConfig();

	// touch_event_fifo.r_idx = 0;
  //  touch_event_fifo.w_idx = 0;
  //  memset(touch_event_fifo.event, 0, sizeof(touch_event_fifo.event));

   lastTouchState = PEN_UP;
}
/*=====================================================================*/
/*=====================================================================*/


void SPI1_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
    
  // Set as Output push-pull - SCK and MOSI
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = SPI1_SCK | SPI1_MOSI;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SPI1_PORT, &GPIO_InitStructure);
  
  //SPI_MISO
  GPIO_InitStructure.GPIO_Pin = SPI1_MISO;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(SPI1_PORT, &GPIO_InitStructure);

}

/*=====================================================================*/
void SPI1_Init_For_Byte(void)
{

}

/*=====================================================================*/
static void ADS7843_SPI_Start( void )
{
    ADS7843_CS_HIGH();
    SPI1_MOSI_HIGH();
    SPI1_SCK_LOW();
    ADS7843_CS_LOW();	      		//芯片允许
}

/*=====================================================================*/
static void SPI_MOSI(u8 data)
{
    if(data)
        SPI1_MOSI_HIGH();
    else
        SPI1_MOSI_LOW();
}

/*=====================================================================*/
/*
#define ShortDelayTime 1
#define LongDelayTime (ShortDelayTime*2)
SPI1_Delay_Short()
{
    __IO uint32_t nCount;
    
    for( nCount= 0; nCount < ShortDelayTime; nCount++ );
}
*/
/*=====================================================================*/


/*=====================================================================*/
static void ADS7843_WrCmd(u8 cmd)
{
    u8 buf, i;

    for( i = 0; i < 8; i++ )
    {
        buf = (cmd >> (7-i)) & 0x1 ;	//MSB在前,LSB在后
        SPI_MOSI(buf);	 	//时钟上升沿锁存DIN
        SPI1_SCK_HIGH();			//时钟脉冲，一共8个
        SPI1_SCK_LOW();			//开始发送命令字
    }
	
}

/*=====================================================================*/
static u16 ADS7843_Read(void)
{
	u16 buf = 0 ;
	u8 i;

    SPI1_SCK_HIGH(); //wait busy
	SPI1_SCK_LOW();	
	for( i = 0; i < 12; i++ )
	{
		buf = buf << 1 ;
		SPI1_SCK_HIGH();
		if ( SPI1_MISO_READ() )	
		{
			buf = buf + 1 ;
		}
		SPI1_SCK_LOW();			
	}

	for( i = 0; i < 3; i++ )
	{
		SPI1_SCK_HIGH();
		SPI1_SCK_LOW();			
	}
    
	return( buf ) ;
}

/*=====================================================================*/

static void ADS7843_Rd_Addata(u16 *X_Addata,u16 *Y_Addata)
{

	u16		i,j,k,x_addata[times],y_addata[times];
    int result;

        
	for(i=0;i<times;i++)					//采样4次.
	{
	#if 1
	  ADS7843_SPI_Start();
		ADS7843_WrCmd( CHX ) ;
		x_addata[i] = ADS7843_Read() ;
    ADS7843_CS_HIGH(); 

	  ADS7843_SPI_Start();
		ADS7843_WrCmd( CHY ) ;
		y_addata[i] = ADS7843_Read() ;
    ADS7843_CS_HIGH(); 
	#else
		
		SPI3_SetSpeed(SPI_BaudRatePrescaler_64);
		ADS7843_CS_LOW();
		
		SPI3_ReadWriteByte(CHX);
		y_addata[i] = SPI3_ReadWrite2Bytes();
	       ADS7843_CS_HIGH(); 
		ADS7843_CS_LOW();
		SPI3_ReadWriteByte(CHY);
		x_addata[i] = SPI3_ReadWrite2Bytes(); 
	       ADS7843_CS_HIGH(); 
		SPI3_SetSpeed(SPI_BaudRatePrescaler_2);
	#endif
	}

	result = x_addata[0];
	for(i=0;i<times;i++)					
	{
		for(j = i + 1; j < times; j++)
		{
			if(x_addata[j] > x_addata[i])
			{
				k = x_addata[j];
				x_addata[j] = x_addata[i];
				x_addata[i] = k;
			}
		}
	}
	if(x_addata[times / 2 -1] - x_addata[times / 2 ] > 50)
	{
         *X_Addata = 0;
         *Y_Addata = 0;
         return ;
   }

	*X_Addata = (x_addata[times / 2 -1] + x_addata[times / 2 ]) /2;

	
	result = y_addata[0];
	for(i=0;i<times;i++)					
	{
		for(j = i + 1; j < times; j++)
		{
			if(y_addata[j] > y_addata[i])
			{
				k = y_addata[j];
				y_addata[j] = y_addata[i];
				y_addata[i] = k;
			}
		}
	}

	
	if(y_addata[times / 2 -1] - y_addata[times / 2 ] > 50)
	{
            *X_Addata = 0;
            *Y_Addata = 0;
            return ;
        }

	*Y_Addata = (y_addata[times / 2 -1] + y_addata[times / 2 ]) /2;
	
	

}

/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
u16 distence(u16 data1,u16 data2)
{
    if((data1 > data2) && (data1 > data2 + 10))
    {
        return 0;
    }

    if((data2 > data2) && (data2 > data2 + 10))
    {
        return 0;
    }

    return 1;    
}

/*=====================================================================*/
/*
extern uint32_t TSC_Value_X;
extern uint32_t TSC_Value_Y;
void ARMJISHU_TouchScreen_ADS7843(void)
{
  u16 xdata, ydata;
  u32 xScreen, yScreen;

    static u16 sDataX,sDataY;
    
 // printf("\n\r ARMJISHU_TouchScreen_ADS7843 \n\r");
  ADS7843_Rd_Addata(&xdata, &ydata);
  xScreen = _AD2X(ydata);
  yScreen = _AD2Y(xdata);

   //xScreen = 320 - ((ydata*320)>>12);
   //yScreen = (xdata*240)>>12;

 
  //printf("\n\r (0x%x, 0x%x), (%d, %d)", xdata, ydata, xScreen, yScreen);
  if((xScreen>1)&&(yScreen>1)&&(xScreen<320-1)&&(yScreen<240-1))
  {
       printf("\n\r%d,%d", xScreen, yScreen);
      if((GPIO_ADS7843_INT_VALID) && distence(sDataX,xScreen) && distence(sDataY,yScreen))
        {
//             LCD_BIG_POINT(xScreen, yScreen);
//			 printf("\n\r%d,%d", xScreen, yScreen);
		        TSC_Value_X = xScreen;
				if(DeviceCode==0x9325)
					TSC_Value_Y = 240-yScreen;
				else
					TSC_Value_Y = yScreen;
        }
      sDataX = xScreen;
      sDataY = yScreen;
  }
    
}
*/

TOUCH_EVENT *getTouchEvent()
{
	uint32_t tmpTime, diffTime = 0;
	int16_t x, y;

	tmpTime = getTick();

	diffTime = getTickDiff(touch_time, tmpTime);
		
	//if(diffTime > 10 && diffTime < 2000)	//按下时间限制在10ms到2s
	if(diffTime > 5)
	//if(diffTime > 2000)
	{
		x = x_touch;
		y = y_touch;

		ADS7843_Rd_Addata(&x_touch, &y_touch);
	//	x_touch = ADS_Read_X();
	//	y_touch = ADS_Read_Y();

		touch_time = tmpTime;
		
		//if(x - x_touch >50 || x_touch - x >50 || y - y_touch >50 || y_touch - y >50)
		if(abs(x - x_touch) > 50 || abs(y-y_touch) > 50)
		{
			return 0;
		}

		TouchEvent.x  = (x + x_touch) >> 1;
		TouchEvent.y = (y + y_touch) >> 1;

	}
	else
	{
		return 0;
	}
	
	return &TouchEvent;

}

/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/
/*=====================================================================*/

