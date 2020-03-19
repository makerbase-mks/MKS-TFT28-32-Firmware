/**
  ******************************************************************************
  * @file    stm3210c_eval_lcd.c
  * @author  MCD Application Team
  * @version V4.2.0
  * @date    04/16/2010
  * @brief   This file includes the LCD driver for AM-240320L8TNQW00H (LCD_ILI9320)
  *          Liquid Crystal Display Module of STM3210C-EVAL board.
  * @updata  www.armjishu.com Team
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm3210c_eval_lcd.h"
#include "font.h" 
#include "xprintf.h"
#include "Printer.h"
/******************** (C) COPYRIGHT 2013 www.armjishu.com  ********************
 * 文件名  ：SZ_STM32F107VC_LCD.c
 * 描述    ：提供STM32F107VC神舟IV号开发板的TFT_LCD彩屏相关库函数
 * 实验平台：STM32神舟开发板
 * 作者    ：www.armjishu.com 
*******************************************************************************/



/**-------------------------------------------------------
  * LCD默认采用横屏显示，其中坐标表示为(x,y)
  * x LCD的行数  
  * y LCD的列数
  (0,0)     表示LCD左上角的点 
  (239,319) 表示LCD右下角的点   
  (0,2)     表示LCD第1行第3列的点行
  (238,2)   表示LCD第239行第3列的点行
          yyyy                               Y
          0123                               319             
  X = 0   |--(0,2)-------- ~~~ --------------|
  X = 1   |--------------- ~~~ --------------|
  X = 2   |--------------- ~~~ --------------|
  .....
  X = 237 |--------------- ~~~ --------------|
  X = 238 |--(238,2)------ ~~~ --------------|  
  X = 239 |--------------- ~~~ --------------|(239,319)
  
***------------------------------------------------------*/
//extern const unsigned char asc2_1608[95][16];
//extern const unsigned char asc2_1206[95][16];
//LCD的画笔颜色和背景色	   
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色 

//#define LCD_DEBUG_PRINTF               printf
#define LCD_DEBUG_PRINTF(str,args...)    ((void)0)


#define SetCs  LCD_PORT_CS->BSRR |= LCD_Pin_CS;//GPIO_SetBits(LCD_PORT_CS, LCD_Pin_CS);
#define ClrCs  LCD_PORT_CS->BRR |= LCD_Pin_CS;//GPIO_ResetBits(LCD_PORT_CS, LCD_Pin_CS);
 

#define SetWr  LCD_PORT_WR->BSRR |= LCD_Pin_WR;//GPIO_SetBits(LCD_PORT_WR, LCD_Pin_WR);
#define ClrWr  LCD_PORT_WR->BRR |= LCD_Pin_WR;//GPIO_ResetBits(LCD_PORT_WR, LCD_Pin_WR);

#define SetRs  LCD_PORT_RS->BSRR |= LCD_Pin_RS;//GPIO_SetBits(LCD_PORT_RS, LCD_Pin_RS);
#define ClrRs  LCD_PORT_RS->BRR |= LCD_Pin_RS;//GPIO_ResetBits(LCD_PORT_RS, LCD_Pin_RS);

#define SetRd  LCD_PORT_RD->BSRR |= LCD_Pin_RD;//GPIO_SetBits(LCD_PORT_RD, LCD_Pin_RD);
#define ClrRd  LCD_PORT_RD->BRR |= LCD_Pin_RD;//GPIO_ResetBits(LCD_PORT_RD, LCD_Pin_RD);

#define LCD_Write(LCD_DATA)  GPIOE->ODR = LCD_DATA//GPIO_Write(GPIOE, LCD_DATA)
#define LCD_Read()  ((uint16_t)GPIOE->IDR)//GPIO_ReadInputData(GPIOE)


/* Delay函数在"SZ_STM32F107VC_LIB.h"有定义 */
//#define Delay(nCount)  Delay(12000 * nCount) 

/* 前景色TextColor和背景色BackColor */
static  __IO uint16_t TextColor = 0x0000, BackColor = 0xFFFF;
/* LCD模组驱动芯片ID */
static  __IO uint16_t DeviceIdCode;
uint16_t readLcdId;
/* LCD当前的英文字库 */
sFONT *LCD_Currentfonts;

static __IO uint8_t HyalineBack = HyalineBackDis;
/** @defgroup STM3210C_EVAL_LCD_Private_Defines
  * @{
  */ 

#define MAX_POLY_CORNERS   200
#define POLY_Y(Z)          ((int32_t)((Points + Z)->X))
#define POLY_X(Z)          ((int32_t)((Points + Z)->Y))

#define ABS(X)  ((X) > 0 ? (X) : -(X))    

#define Delay(nCount)  lcd_delay(12000 * nCount) 

static void lcd_delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--);
}


/*******************************************************************************
* Function Name  : LCD_DB_AS_InPut
* Description    : config MCU LCD_DB pins AS InPut
* Input          : None
* Output         : None
* Return         : None.
*******************************************************************************/
void LCD_DB_AS_InPut(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // DB15--0
  GPIO_InitStructure.GPIO_Pin = LCD_PIN_DATA;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(LCD_PORT_DAT, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : LCD_DB_AS_OutPut
* Description    : config MCU LCD_DB pins AS OutPut
* Input          : None
* Output         : None
* Return         : None.
*******************************************************************************/
void LCD_DB_AS_OutPut(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // DB15--0
  GPIO_InitStructure.GPIO_Pin = LCD_PIN_DATA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(LCD_PORT_DAT, &GPIO_InitStructure);
}


//写寄存器函数FOR 9341
void LCD_WR_REG(u8 data)
{ 
        ClrRs	//写地址  
        ClrCs         
        SetRd
        ClrWr

        LCD_Write(data);       
        SetWr
        SetRs
        SetCs  
}

//写数据函数FOR 9341
void LCD_WR_DATA(u8 data)
{ 
        SetRs	//写地址  
        ClrCs         
        SetRd
        ClrWr

        LCD_Write(data);       
        SetWr
        SetRs
        SetCs  
}

u16 LCD_RD_DATA(void)
{										   
	u16 t;
 	LCD_DB_AS_InPut();

	SetRs	//写地址  
       ClrCs 
	//读取数据(读寄存器时,并不需要读2次)
	ClrRd
	//if(lcddev.id==0X8989)delay_us(2);//FOR 8989,延时2us					   
	t=LCD_Read();   
	SetRd
	SetCs

	LCD_DB_AS_OutPut();
	return t;  
}
extern CFG_ITMES gCfgItems;
/****************************************************************************
* 名    称：void LCD_Setup()
* 功    能：初始化 LCD 控制器
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：LCD_Setup();
* 作    者： www.armjishu.com TEAM
****************************************************************************/
volatile uint8_t lcd_init_cnt=0;

void LCD_Setup(void)
{ 
   	//const uint8_t LogoStr[] = "  www.armjishu.com  ";
   	// uint8_t Devicestr[] = " LCD Driver ID:9320 ";
   	lcd_init_cnt=0;
		while(DeviceIdCode == 0x0000)
		{
	    /* Configure the LCD pins config --------------------------------------------*/
	    LCD_Pins_Config();

	    Delay(200); /* Delay 50 ms */
	    LCD_WriteReg(0x0000,0x0001);          //start internal osc
	    Delay(3); /* Delay 50 ms */
	    LCD_DB_AS_InPut();
	    Delay(2); /* Delay 50 ms */
	    DeviceIdCode = LCD_ReadReg(0x0000);
	    DeviceIdCode = LCD_ReadReg(0x0000);

		//if(DeviceIdCode == 0)
		if(DeviceIdCode<0XFF||DeviceIdCode==0XFFFF||DeviceIdCode==0X9300)//读到ID不正确,新增DeviceIdCode==0X9300判断，因为9341在未被复位的情况下会被读成9300
		{

			//HC-chen 4.10
			/*
			LCD_WR_REG(0X04);						//读取0x8552			   
			LCD_RD_DATA(); 							//dummy read 	
	 		LCD_RD_DATA();   	    			//读到0X00
	  		DeviceIdCode=LCD_RD_DATA();   	
	 		DeviceIdCode<<=8;
			DeviceIdCode|=LCD_RD_DATA(); 
			
			if(DeviceIdCode != 0x8552)//读不到ST7789 的ID再读其他显示屏
			*/
			{
				LCD_WR_REG(0XD3);
				LCD_RD_DATA(); 									//dummy read 	
		 		LCD_RD_DATA();   	    					//读到0X00
		  		DeviceIdCode=LCD_RD_DATA();   	//读取93								   
		 		DeviceIdCode<<=8;
				DeviceIdCode|=LCD_RD_DATA();  	//读取41	 
			}
		}
		if((DeviceIdCode != 0x9320)&&(DeviceIdCode != 0x9300)
	 	&&(DeviceIdCode != 0x9331)&&(DeviceIdCode != 0x8989)
	 	&&(DeviceIdCode != 0x9325)&&(DeviceIdCode != 0x9328)
	 	&&(DeviceIdCode != 0x9341)&&(DeviceIdCode != 0x1505))
		 {
	 			LCD_WR_REG(0X04);						//读取0x8552			   
				LCD_RD_DATA(); 							//dummy read 	
		 		LCD_RD_DATA();   	    			//读到0X00
		  		DeviceIdCode=LCD_RD_DATA();   	
		 		DeviceIdCode<<=8;
				DeviceIdCode|=LCD_RD_DATA(); 
		 }

		readLcdId = DeviceIdCode;
		lcd_init_cnt++;
		if(lcd_init_cnt>=5)
		{
			break;
		}

		
	    //LCD_DEBUG_PRINTF("\n\r ###### www.armjishu.com DeviceIdCode = 0x%x ###### ", DeviceIdCode);

	    //Devicestr[15] = ((DeviceIdCode >> 12) & 0xF) + 0x30;
	    //Devicestr[16] = ((DeviceIdCode >> 8) & 0xF) + 0x30;
	    //Devicestr[17] = ((DeviceIdCode >> 4) & 0xF) + 0x30;
	    //Devicestr[18] = ((DeviceIdCode >> 0) & 0xF) + 0x30;
	}
	LCD_DB_AS_OutPut();

    Delay(1); /* Delay 50 ms */            //start internal osc
#if 0
    if(DeviceIdCode == 0x8989)
    {
        //LCD_DEBUG_PRINTF("\n\r This LCD is SSD1289 ");
    }

    if(DeviceIdCode == 0x9325)
    {
        //LCD_DEBUG_PRINTF("\n\r This LCD is ili9325 ");
    }
  #endif
    if(DeviceIdCode==0x9325||DeviceIdCode==0x9328)
    {
 /*   	
        LCD_WriteReg(0x00e3,0x3008);
        LCD_WriteReg(0x00e7,0x0012);
        LCD_WriteReg(0x00ef,0x1231);        //Set the internal vcore voltage
        // LCD_WriteReg(0x00e7,0x0010);      
        LCD_WriteReg(0x0000,0x0001);        //start internal osc
        LCD_WriteReg(0x0001,0x0100);     
        LCD_WriteReg(0x0002,0x0700);        //Driving Wave Control                   
        LCD_WriteReg(0x0003,(1<<12)|(0<<5)|(1<<4)|(1<<3) );// );    (1<<12)|(1<<4)|(1 << 3) //65K 
        LCD_WriteReg(0x0004,0x0000);                                   
        LCD_WriteReg(0x0008,0x0207);               
        LCD_WriteReg(0x0009,0x0000);         
        LCD_WriteReg(0x000a,0x0000);        //display setting         
        LCD_WriteReg(0x000c,0x0001);        //display setting          
        LCD_WriteReg(0x000d,0x0000);        //0f3c          
        LCD_WriteReg(0x000f,0x0000);
        //Power On sequence //
        LCD_WriteReg(0x0010,0x0000);   
        LCD_WriteReg(0x0011,0x0007);
        LCD_WriteReg(0x0012,0x0000);                                                                 
        LCD_WriteReg(0x0013,0x0000);                 
        Delay(5);
        Delay(5);
        LCD_WriteReg(0x0010,0x1590);   
        LCD_WriteReg(0x0011,0x0227);
        Delay(5);
        Delay(5);
        LCD_WriteReg(0x0012,0x009c);                  
        Delay(5);
        Delay(5);
        LCD_WriteReg(0x0013,0x1900);   
        LCD_WriteReg(0x0029,0x0023);
        LCD_WriteReg(0x002b,0x000e);
        Delay(5);
        Delay(5);
        LCD_WriteReg(0x0020,0x0000);                                                            
        LCD_WriteReg(0x0021,0x0000);           
        Delay(5);
        Delay(5);
        LCD_WriteReg(0x0030,0x0007); 
        LCD_WriteReg(0x0031,0x0707);   
        LCD_WriteReg(0x0032,0x0006);
        LCD_WriteReg(0x0035,0x0704);
        LCD_WriteReg(0x0036,0x1f04); 
        LCD_WriteReg(0x0037,0x0004);
        LCD_WriteReg(0x0038,0x0000);        
        LCD_WriteReg(0x0039,0x0706);     
        LCD_WriteReg(0x003c,0x0701);
        LCD_WriteReg(0x003d,0x000f);
        Delay(5);
        Delay(5);
        LCD_WriteReg(0x0050,0x0000);        
        LCD_WriteReg(0x0051,0x00ef);   
        LCD_WriteReg(0x0052,0x0000);     
        LCD_WriteReg(0x0053,0x013f);
        LCD_WriteReg(0x0060,0xa700);        
        LCD_WriteReg(0x0061,0x0001); 
        LCD_WriteReg(0x006a,0x0000);
        LCD_WriteReg(0x0080,0x0000);
        LCD_WriteReg(0x0081,0x0000);
        LCD_WriteReg(0x0082,0x0000);
        LCD_WriteReg(0x0083,0x0000);
        LCD_WriteReg(0x0084,0x0000);
        LCD_WriteReg(0x0085,0x0000);
      
        LCD_WriteReg(0x0090,0x0010);     
        LCD_WriteReg(0x0092,0x0600);  
        if(DeviceIdCode==0x9328)
        {   
            LCD_WriteReg(0x0093,0x0003);
            LCD_WriteReg(0x0095,0x0110);
            LCD_WriteReg(0x0097,0x0000);        
            LCD_WriteReg(0x0098,0x0000);  
         }
         //display on sequence     
        LCD_WriteReg(0x0007,0x0133);
    
        LCD_WriteReg(0x0020,0x0000);                                                            
        LCD_WriteReg(0x0021,0x0000);
*/
//液晶样品测试配置。
      	LCD_WriteReg(0x00e3,0x3008);
        LCD_WriteReg(0x00e7,0x0012);
        LCD_WriteReg(0x00ef,0x1231);        //Set the internal vcore voltage
        // LCD_WriteReg(0x00e7,0x0010);      
        //LCD_WriteReg(0x0000,0x0001);        //start internal osc
        /*#if OVERTURN_180
        LCD_WriteReg(0x0001,0x0000);
				#else
        LCD_WriteReg(0x0001,0x0100);  
				#endif*/
				
	 //HC-chen 2017.10.11
  	 if(gCfgItems.OVERTURN_180!=0xee) //不翻转
  	 {
  	 	LCD_WriteReg(0x0001,0x0100); 
  	 }
	 else
	 {
	 	LCD_WriteReg(0x0001,0x0000);
	 }
        LCD_WriteReg(0x0002,0x0700);        //Driving Wave Control                   
        LCD_WriteReg(0x0003,(1<<12)|(0<<5)|(1<<4)|(1<<3) );//(1<<12)|(1<<4)|(1 << 3) );     //65K 
        LCD_WriteReg(0x0004,0x0000);                                   
        LCD_WriteReg(0x0008,0x0207);               
        LCD_WriteReg(0x0009,0x0000);         
        LCD_WriteReg(0x000a,0x0000);        //display setting         
        LCD_WriteReg(0x000c,0x0000);        //display setting          
        LCD_WriteReg(0x000d,0x0000);        //0f3c          
        LCD_WriteReg(0x000f,0x0000);
        //Power On sequence //
        LCD_WriteReg(0x0010,0x0000);   
        LCD_WriteReg(0x0011,0x0007);
        LCD_WriteReg(0x0012,0x0000);                                                                 
        LCD_WriteReg(0x0013,0x0000);                 
        Delay(5);
        Delay(5);
        LCD_WriteReg(0x0010,0x1290);   
        LCD_WriteReg(0x0011,0x0227);
        Delay(5);
        Delay(5);
        LCD_WriteReg(0x0012,0x001A);                  
        Delay(5);
        Delay(5);
        LCD_WriteReg(0x0013,0x1800);   
        LCD_WriteReg(0x0029,0x0028);
        LCD_WriteReg(0x002b,0x000C);
        Delay(5);
        Delay(5);
        LCD_WriteReg(0x0020,0x0000);                                                            
        LCD_WriteReg(0x0021,0x0000);           
        Delay(5);
        Delay(5);
        LCD_WriteReg(0x0030,0x0000); 
        LCD_WriteReg(0x0031,0x0305);   
        LCD_WriteReg(0x0032,0x0003);
        LCD_WriteReg(0x0035,0x0304);
        LCD_WriteReg(0x0036,0x000F); 
        LCD_WriteReg(0x0037,0x0407);
        LCD_WriteReg(0x0038,0x0204);        
        LCD_WriteReg(0x0039,0x0707);     
        LCD_WriteReg(0x003c,0x0403);
        LCD_WriteReg(0x003d,0x1604);
        Delay(5);
        Delay(5);
        LCD_WriteReg(0x0050,0x0000);        
        LCD_WriteReg(0x0051,0x00ef);   
        LCD_WriteReg(0x0052,0x0000);     
        LCD_WriteReg(0x0053,0x013f);
	/*			#if OVERTURN_180
				LCD_WriteReg(0x0060,0x2700); 
				#else
        LCD_WriteReg(0x0060,0xa700);  
				#endif*/
				
	 //HC-chen 2017.10.11
  	 if(gCfgItems.OVERTURN_180!=0xee) //不翻转
  	 {
  	 	LCD_WriteReg(0x0060,0xa700); 
  	 }
	 else
	 {
	 	LCD_WriteReg(0x0060,0x2700);
	 }
        LCD_WriteReg(0x0061,0x0001); 
        LCD_WriteReg(0x006a,0x0000);
        LCD_WriteReg(0x0080,0x0000);
        LCD_WriteReg(0x0081,0x0000);
        LCD_WriteReg(0x0082,0x0000);
        LCD_WriteReg(0x0083,0x0000);
        LCD_WriteReg(0x0084,0x0000);
        LCD_WriteReg(0x0085,0x0000);
      
        LCD_WriteReg(0x0090,0x0010);     
        LCD_WriteReg(0x0092,0x0600);  
        if(DeviceIdCode==0x9328)
        {   
            LCD_WriteReg(0x0093,0x0003);
            LCD_WriteReg(0x0095,0x0110);
            LCD_WriteReg(0x0097,0x0000);        
            LCD_WriteReg(0x0098,0x0000);  
         }
         //display on sequence     
        LCD_WriteReg(0x0007,0x0133);
    
        //LCD_WriteReg(0x0020,0x0000);                                                            
        //LCD_WriteReg(0x0021,0x0000);
 
    }
    else if(DeviceIdCode==0x9320||DeviceIdCode==0x9300)
    {
        //LCD_DEBUG_PRINTF("\n\r This LCD is ili%x.", DeviceIdCode);
        LCD_WriteReg(0x00,0x0001);
        LCD_WriteReg(0x01,0x0100);      //Driver Output Contral. 0x0100
        LCD_WriteReg(0x02,0x0700);      //LCD Driver Waveform Contral.
        LCD_WriteReg(0x03,0x1018);      //Entry Mode Set. 0x1018 0x1030
    
        LCD_WriteReg(0x04,0x0000);      //Scalling Contral.
        LCD_WriteReg(0x08,0x0202);      //Display Contral 2.(0x0207)
        LCD_WriteReg(0x09,0x0000);      //Display Contral 3.(0x0000)
        LCD_WriteReg(0x0a,0x0000);      //Frame Cycle Contal.(0x0000)
        LCD_WriteReg(0x0c,(1<<0));      //Extern Display Interface Contral 1.(0x0000)
        LCD_WriteReg(0x0d,0x0000);      //Frame Maker Position.
        LCD_WriteReg(0x0f,0x0000);      //Extern Display Interface Contral 2.
    
        Delay(5);
        LCD_WriteReg(0x07,0x0101);      //Display Contral.
        Delay(5);
    
        LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));     //Power Control 1.(0x16b0)
        LCD_WriteReg(0x11,0x0007);                                  //Power Control 2.(0x0001)
        LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));                    //Power Control 3.(0x0138)
        LCD_WriteReg(0x13,0x0b00);                                  //Power Control 4.
        LCD_WriteReg(0x29,0x0000);                                  //Power Control 7.
        LCD_WriteReg(0x2b,(1<<14)|(1<<4));
        
        LCD_WriteReg(0x50,0);           //Set X Start.
        LCD_WriteReg(0x51,239);         //Set X End.
        LCD_WriteReg(0x52,0);           //Set Y Start.
        LCD_WriteReg(0x53,319);         //Set Y End.
        
        LCD_WriteReg(0x60,0x2700);      //Driver Output Control.
        LCD_WriteReg(0x61,0x0001);      //Driver Output Control.
        LCD_WriteReg(0x6a,0x0000);      //Vertical Srcoll Control.
    
        LCD_WriteReg(0x80,0x0000);      //Display Position? Partial Display 1.
        LCD_WriteReg(0x81,0x0000);      //RAM Address Start? Partial Display 1.
        LCD_WriteReg(0x82,0x0000);      //RAM Address End-Partial Display 1.
        LCD_WriteReg(0x83,0x0000);      //Displsy Position? Partial Display 2.
        LCD_WriteReg(0x84,0x0000);      //RAM Address Start? Partial Display 2.
        LCD_WriteReg(0x85,0x0000);      //RAM Address End? Partial Display 2.
    
        LCD_WriteReg(0x90,(0<<7)|(16<<0));    //Frame Cycle Contral.(0x0013)
        LCD_WriteReg(0x92,0x0000);      //Panel Interface Contral 2.(0x0000)
        LCD_WriteReg(0x93,0x0001);      //Panel Interface Contral 3.
        LCD_WriteReg(0x95,0x0110);      //Frame Cycle Contral.(0x0110)
        LCD_WriteReg(0x97,(0<<8));      //
        LCD_WriteReg(0x98,0x0000);      //Frame Cycle Contral.
        Delay(5);
        LCD_WriteReg(0x07,0x0173);      //(0x0173)
        Delay(5);
    }
		#if 1
    else if(DeviceIdCode==0x9331)
    {
        LCD_WriteReg(0x00E7, 0x1014);
        LCD_WriteReg(0x0001, 0x0100);   // set SS and SM bit   0x0100
        LCD_WriteReg(0x0002, 0x0200);   // set 1 line inversion
        LCD_WriteReg(0x0003, 0x1030);   // set GRAM write direction and BGR=1.     0x1030
        LCD_WriteReg(0x0008, 0x0202);   // set the back porch and front porch
        LCD_WriteReg(0x0009, 0x0000);   // set non-display area refresh cycle ISC[3:0]
        LCD_WriteReg(0x000A, 0x0000);   // FMARK function
        LCD_WriteReg(0x000C, 0x0000);   // RGB interface setting
        LCD_WriteReg(0x000D, 0x0000);   // Frame marker Position
        LCD_WriteReg(0x000F, 0x0000);   // RGB interface polarity
        //*************Power On sequence ****************//
        LCD_WriteReg(0x0010, 0x0000);   // SAP, BT[3:0], AP, DSTB, SLP, STB
        LCD_WriteReg(0x0011, 0x0007);   // DC1[2:0], DC0[2:0], VC[2:0]
        LCD_WriteReg(0x0012, 0x0000);   // VREG1OUT voltage
        LCD_WriteReg(0x0013, 0x0000);   // VDV[4:0] for VCOM amplitude
        Delay(5); // Dis-charge capacitor power voltage
        LCD_WriteReg(0x0010, 0x1690);   // SAP, BT[3:0], AP, DSTB, SLP, STB
        LCD_WriteReg(0x0011, 0x0227);   // DC1[2:0], DC0[2:0], VC[2:0]
        Delay(2); // Delay 50ms
        LCD_WriteReg(0x0012, 0x000C);   // Internal reference voltage= Vci;
        Delay(2); // Delay 50ms
        LCD_WriteReg(0x0013, 0x0800);   // Set VDV[4:0] for VCOM amplitude
        LCD_WriteReg(0x0029, 0x0011);   // Set VCM[5:0] for VCOMH
        LCD_WriteReg(0x002B, 0x000B);   // Set Frame Rate
        Delay(2); // Delay 50ms
        LCD_WriteReg(0x0020, 0x0000);   // GRAM horizontal Address
        LCD_WriteReg(0x0021, 0x0000);   // GRAM Vertical Address
        // ----------- Adjust the Gamma Curve ----------//
        LCD_WriteReg(0x0030, 0x0000);
        LCD_WriteReg(0x0031, 0x0106);
        LCD_WriteReg(0x0032, 0x0000);
        LCD_WriteReg(0x0035, 0x0204);
        LCD_WriteReg(0x0036, 0x160A);
        LCD_WriteReg(0x0037, 0x0707);
        LCD_WriteReg(0x0038, 0x0106);
        LCD_WriteReg(0x0039, 0x0707);
        LCD_WriteReg(0x003C, 0x0402);
        LCD_WriteReg(0x003D, 0x0C0F);
        //------------------ Set GRAM area ---------------//
        LCD_WriteReg(0x0050, 0x0000);   // Horizontal GRAM Start Address
        LCD_WriteReg(0x0051, 0x00EF);   // Horizontal GRAM End Address
        LCD_WriteReg(0x0052, 0x0000);   // Vertical GRAM Start Address
        LCD_WriteReg(0x0053, 0x013F);   // Vertical GRAM Start Address
        LCD_WriteReg(0x0060, 0x2700);   // Gate Scan Line
        LCD_WriteReg(0x0061, 0x0001);   // NDL,VLE, REV
        LCD_WriteReg(0x006A, 0x0000);   // set scrolling line
        //-------------- Partial Display Control ---------//
        LCD_WriteReg(0x0080, 0x0000);
        LCD_WriteReg(0x0081, 0x0000);
        LCD_WriteReg(0x0082, 0x0000);
        LCD_WriteReg(0x0083, 0x0000);
        LCD_WriteReg(0x0084, 0x0000);
        LCD_WriteReg(0x0085, 0x0000);
        //-------------- Panel Control -------------------//
        LCD_WriteReg(0x0090, 0x0010);
        LCD_WriteReg(0x0092, 0x0600);
        LCD_WriteReg(0x0007,0x0021);        
        Delay(2);
        LCD_WriteReg(0x0007,0x0061);
        Delay(2);
        LCD_WriteReg(0x0007,0x0133);    // 262K color and display ON
        Delay(2);
    }
    else if(DeviceIdCode==0x9919)
    {
        //*********POWER ON &RESET DISPLAY OFF
         LCD_WriteReg(0x28,0x0006);
         LCD_WriteReg(0x00,0x0001);
         LCD_WriteReg(0x10,0x0000);
         LCD_WriteReg(0x01,0x72ef);
         LCD_WriteReg(0x02,0x0600);
         LCD_WriteReg(0x03,0x6a38);
         LCD_WriteReg(0x11,0x6874);
         
         //  RAM WRITE DATA MASK
         LCD_WriteReg(0x0f,0x0000); 
         //  RAM WRITE DATA MASK
         LCD_WriteReg(0x0b,0x5308); 
         LCD_WriteReg(0x0c,0x0003);
         LCD_WriteReg(0x0d,0x000a);
         LCD_WriteReg(0x0e,0x2e00);
         LCD_WriteReg(0x1e,0x00be);
         LCD_WriteReg(0x25,0x8000);
         LCD_WriteReg(0x26,0x7800);
         LCD_WriteReg(0x27,0x0078);
         LCD_WriteReg(0x4e,0x0000);
         LCD_WriteReg(0x4f,0x0000);
         LCD_WriteReg(0x12,0x08d9);
         // -----------------Adjust the Gamma Curve----//
         LCD_WriteReg(0x30,0x0000);     //0007
         LCD_WriteReg(0x31,0x0104);     //0203
         LCD_WriteReg(0x32,0x0100);     //0001
         LCD_WriteReg(0x33,0x0305);     //0007
         LCD_WriteReg(0x34,0x0505);     //0007
         LCD_WriteReg(0x35,0x0305);     //0407
         LCD_WriteReg(0x36,0x0707);     //0407
         LCD_WriteReg(0x37,0x0300);     //0607
         LCD_WriteReg(0x3a,0x1200);     //0106
         LCD_WriteReg(0x3b,0x0800);         
         LCD_WriteReg(0x07,0x0033);
    }

    else if(DeviceIdCode==0x1505)
		{ 
		//*********************************************Start intial Sequence	  	
		LCD_WriteReg(0x00e5,0x8000);
		LCD_WriteReg(0x0000,0x0001);
		//HC-chen 2017.10.11
		if(gCfgItems.OVERTURN_180 != 0xee) //不翻转
		{
			LCD_WriteReg(0x0001,0x0100);
		}
		else
		{
		LCD_WriteReg(0x0001,0x0000);
		} 
		Delay(50);
		//LCD_WriteReg(0x0001,0x0100);
		LCD_WriteReg(0x0002,0x0700);
		//LCD_WriteReg(0x0003,0x1030);
		LCD_WriteReg(0x0003,0x1018);
		LCD_WriteReg(0x0004,0x0000);
		LCD_WriteReg(0x0008,0x0202);
		LCD_WriteReg(0x0009,0x0000);
		LCD_WriteReg(0x000a,0x0000);	
		LCD_WriteReg(0x000c,0x0000);
		LCD_WriteReg(0x000d,0x0000);
		LCD_WriteReg(0x000f,0x0000);
		//*********************************************Power On
		LCD_WriteReg(0x0010,0x0000);
		LCD_WriteReg(0x0011,0x0000);
		LCD_WriteReg(0x0012,0x0000);
		LCD_WriteReg(0x0013,0x0000);
		Delay(50);
		LCD_WriteReg(0x0010,0x17b0);
		LCD_WriteReg(0x0011,0x0037);
		Delay(50);
		LCD_WriteReg(0x0012,0x0138);
		Delay(50);
		LCD_WriteReg(0x0013,0x1700);
		LCD_WriteReg(0x0029,0x001f);//0x000d
		Delay(50);
		LCD_WriteReg(0x0020,0x0000);
		LCD_WriteReg(0x0021,0x0000);
		//*********************************************Set gamma
		//LCD_WriteReg(0x0030,0x0001);
		//LCD_WriteReg(0x0031,0x0606);
		//LCD_WriteReg(0x0032,0x0304);
		//LCD_WriteReg(0x0033,0x0202);
		//LCD_WriteReg(0x0034,0x0202);
		//LCD_WriteReg(0x0035,0x0103);
		//LCD_WriteReg(0x0036,0x011d);
		//LCD_WriteReg(0x0037,0x0404);
		//LCD_WriteReg(0x0038,0x0404);
		//LCD_WriteReg(0x0039,0x0404);
		//LCD_WriteReg(0x003c,0x0700);
		//LCD_WriteReg(0x003d,0x0a1f);
		LCD_WriteReg(0x0030,0x0707);
		LCD_WriteReg(0x0031,0x0007); //0x0707
		LCD_WriteReg(0x0032,0x0603); 
		LCD_WriteReg(0x0033,0x0700); 
		LCD_WriteReg(0x0034,0x0202); 
		LCD_WriteReg(0x0035,0x0002); //?0x0606
		LCD_WriteReg(0x0036,0x1F0F);
		LCD_WriteReg(0x0037,0x0707); //0x0f0f  0x0105
		LCD_WriteReg(0x0038,0x0000); 
		LCD_WriteReg(0x0039,0x0000); 
		LCD_WriteReg(0x003A,0x0707); 
		LCD_WriteReg(0x003B,0x0000); //0x0303
		LCD_WriteReg(0x003C,0x0007); //?0x0707
		LCD_WriteReg(0x003D,0x0000); //0x1313//0x1f08

		//**********************************************Set Gram aera
		LCD_WriteReg(0x0050,0x0000);
		LCD_WriteReg(0x0051,0x00ef);
		LCD_WriteReg(0x0052,0x0000);
		LCD_WriteReg(0x0053,0x013f);
		//LCD_WriteReg(0x0060,0x2700);
		//HC-chen 2017.10.11
		if(gCfgItems.OVERTURN_180 != 0xee) //不翻转
		{
			LCD_WriteReg(0x0060,0x2700);
		}
		else
		{
		LCD_WriteReg(0x0060,0xA700);
		}
		LCD_WriteReg(0x0061,0x0001);
		LCD_WriteReg(0x006a,0x0000);
		//*********************************************Paratial display
		LCD_WriteReg(0x0090,0x0010);
		LCD_WriteReg(0x0092,0x0000);
		LCD_WriteReg(0x0093,0x0003);
		LCD_WriteReg(0x0095,0x0101);
		LCD_WriteReg(0x0097,0x0000);
		LCD_WriteReg(0x0098,0x0000);
		//******************************************** Plan Control
		LCD_WriteReg(0x0007,0x0021);
		Delay(50);
		LCD_WriteReg(0x0007,0x0031);
		Delay(50);
		LCD_WriteReg(0x0007,0x0173);
		Delay(50);
		//LLCD_WRITE_CMD(0x0022);
		}
		else if(DeviceIdCode == 0x8552)//ST7789
		{
		#if 0
			//----------------------------------------end Reset Sequence---------------------------------------//
			//--------------------------------display control setting----------------------------------------//
			LCD_WR_REG(0xfe);
			LCD_WR_REG(0xef);
			LCD_WR_REG(0x36);

			if(gCfgItems.OVERTURN_180 != 0xee) //不翻转
			{
				LCD_WR_DATA(0xA0);//
			}
			else
			{
				LCD_WR_DATA(0x60);
			}
				
			LCD_WR_REG(0x3a);
			LCD_WR_DATA(0x05);

			LCD_WR_REG(0xe8);
			LCD_WR_DATA(0x12);
			LCD_WR_DATA(0x22);	
			//-------------------------------------end display control setting----------------------------------//
			//----------------------------------Power Control Registers Initial--------------------------------//
			LCD_WR_REG(0xe3);
			LCD_WR_DATA(0x01);
			LCD_WR_DATA(0x04);
			LCD_WR_REG(0xa5);
			LCD_WR_DATA(0x40);
			LCD_WR_DATA(0x40);
			LCD_WR_REG(0xa4);
			LCD_WR_DATA(0x44);
			LCD_WR_DATA(0x44);
			LCD_WR_REG(0xab);
			LCD_WR_DATA(0x08);
			LCD_WR_REG(0xaa);
			LCD_WR_DATA(0x88);
			LCD_WR_DATA(0x88);
			LCD_WR_REG(0xae);
			LCD_WR_DATA(0x0b);
			LCD_WR_REG(0xac);
			LCD_WR_DATA(0x00);
			LCD_WR_REG(0xaf);
			LCD_WR_DATA(0x77);
			LCD_WR_REG(0xad);
			LCD_WR_DATA(0x77);
			//----------------------------------display window 240X320-------------------------------------//
			LCD_WR_REG(0x2a);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0xef);
			LCD_WR_REG(0x2b);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x01);
			LCD_WR_DATA(0x3f);
			LCD_WR_REG(0x2c);
			//------------------------------------end display window ------------------------------------------//
			//----------------------------------------gamma setting----------------------------------------------//
			LCD_WR_REG(0xF0);
			LCD_WR_DATA(0x02);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x01);
			LCD_WR_DATA(0x01);
			LCD_WR_DATA(0x07);
			LCD_WR_REG(0xF1);
			LCD_WR_DATA(0x01);
			LCD_WR_DATA(0x03);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x36);
			LCD_WR_DATA(0x29);
			LCD_WR_DATA(0x13);
			LCD_WR_REG(0xF2);
			LCD_WR_DATA(0x08);
			LCD_WR_DATA(0x06);
			LCD_WR_DATA(0x24);
			LCD_WR_DATA(0x03);
			LCD_WR_DATA(0x05);
			LCD_WR_DATA(0x34);
			LCD_WR_REG(0xF3);
			LCD_WR_DATA(0x16);
			LCD_WR_DATA(0x0C);
			LCD_WR_DATA(0x5A);
			LCD_WR_DATA(0x04);
			LCD_WR_DATA(0x03);
			LCD_WR_DATA(0x69);
			LCD_WR_REG(0xF4);
			LCD_WR_DATA(0x0D);
			LCD_WR_DATA(0x18);
			LCD_WR_DATA(0x15);
			LCD_WR_DATA(0x05);
			LCD_WR_DATA(0x05);
			LCD_WR_DATA(0x00);
			LCD_WR_REG(0xF5);
			LCD_WR_DATA(0x0D);
			LCD_WR_DATA(0x18);
			LCD_WR_DATA(0x17);
			LCD_WR_DATA(0x35);
			LCD_WR_DATA(0x39);
			LCD_WR_DATA(0x00);
			//------------------------------------end gamma setting------------------------------------------//
			LCD_WR_REG(0x11);
			lcd_delay(120);
			LCD_WR_REG(0x29);
			LCD_WR_REG(0x2c);	
			#else
//---------------------------------------------------------------------------------------------------//
LCD_WR_REG(0x11);
lcd_delay(120); //Delay 120ms
//------------------------------display and color format setting--------------------------------//
LCD_WR_REG(0x36);
//LCD_WR_DATA(0x00);
if(gCfgItems.OVERTURN_180 != 0xee) //不翻转
{
	LCD_WR_DATA(0xA4);//
}
else
{
	LCD_WR_DATA(0x64);
}

LCD_WR_REG(0x3a);
LCD_WR_DATA(0x05);
//--------------------------------ST7789V Frame rate setting----------------------------------//
LCD_WR_REG(0xb2);
LCD_WR_DATA(0x0c);
LCD_WR_DATA(0x0c);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x33);
LCD_WR_DATA(0x33);
LCD_WR_REG(0xb7);
LCD_WR_DATA(0x35);
//---------------------------------ST7789V Power setting--------------------------------------//
LCD_WR_REG(0xbb);
LCD_WR_DATA(0x29);
LCD_WR_REG(0xc0);
LCD_WR_DATA(0x2c);
LCD_WR_REG(0xc2);
LCD_WR_DATA(0x01);
LCD_WR_REG(0xc3);
LCD_WR_DATA(0x10);
LCD_WR_REG(0xc4);
LCD_WR_DATA(0x20);
LCD_WR_REG(0xc6);
LCD_WR_DATA(0x0f);
LCD_WR_REG(0xd0);
LCD_WR_DATA(0xa4);
LCD_WR_DATA(0xa1);
//--------------------------------ST7789V gamma setting---------------------------------------//
LCD_WR_REG(0xe0);
LCD_WR_DATA(0xd0);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x02);
LCD_WR_DATA(0x07);
LCD_WR_DATA(0x0a);
LCD_WR_DATA(0x28);
LCD_WR_DATA(0x32);
LCD_WR_DATA(0x44);
LCD_WR_DATA(0x42);
LCD_WR_DATA(0x06);
LCD_WR_DATA(0x0e);
LCD_WR_DATA(0x12);
LCD_WR_DATA(0x14);
LCD_WR_DATA(0x17);
LCD_WR_REG(0xe1);
LCD_WR_DATA(0xd0);
LCD_WR_DATA(0x00);
LCD_WR_DATA(0x02);
LCD_WR_DATA(0x07);
LCD_WR_DATA(0x0a);
LCD_WR_DATA(0x28);
LCD_WR_DATA(0x31);
LCD_WR_DATA(0x54);
LCD_WR_DATA(0x47);
LCD_WR_DATA(0x0e);
LCD_WR_DATA(0x1c);
LCD_WR_DATA(0x17);
LCD_WR_DATA(0x1b);
LCD_WR_DATA(0x1e);
LCD_WR_REG(0x29);			
			#endif
		}
    else if(DeviceIdCode==0x8989)
    {
    /*
        LCD_WriteReg(0x0000,0x0001);    //打开晶振
        LCD_WriteReg(0x0003,0xA8A4);    //0xA8A4
		//LCD_WriteReg(0x0003,0x6664);    //0xA8A4
        LCD_WriteReg(0x000C,0x0000);    
        LCD_WriteReg(0x000D,0x080C);    
        LCD_WriteReg(0x000E,0x2B00);    
        LCD_WriteReg(0x001E,0x00B0);    
        LCD_WriteReg(0x0001,0x293F);    //驱动输出控制320*240  0x693F  0x2B3F 0x293F
        LCD_WriteReg(0x0002,0x0600);     //LCD Driving Waveform control
        LCD_WriteReg(0x0010,0x0000);    
        LCD_WriteReg(0x0011,0x6078);    //0x4030    //定义数据格式  16位色 0x6070    横屏 0x6058
        LCD_WriteReg(0x0005,0x0000);    
        LCD_WriteReg(0x0006,0x0000);    
        LCD_WriteReg(0x0016,0xEF1C);    
        LCD_WriteReg(0x0017,0x0003);    
        LCD_WriteReg(0x0007,0x0233);    //0x0233       
        LCD_WriteReg(0x000B,0x0000);    
        LCD_WriteReg(0x000F,0x0000);    //扫描开始地址
        LCD_WriteReg(0x0041,0x0000);    
        LCD_WriteReg(0x0042,0x0000);    
        LCD_WriteReg(0x0048,0x0000);    
        LCD_WriteReg(0x0049,0x013F);    
        LCD_WriteReg(0x004A,0x0000);    
        LCD_WriteReg(0x004B,0x0000);    
        LCD_WriteReg(0x0044,0xEF00);    //Specify the start/end positions of the window address in the horizontal direction by an address unit
        LCD_WriteReg(0x0045,0x0000);    //Specify the start positions of the window address in the vertical direction by an address unit 
        LCD_WriteReg(0x0046,0x013F);    //Specify the end positions of the window address in the vertical direction by an address unit 
        LCD_WriteReg(0x0030,0x0707);    
        LCD_WriteReg(0x0031,0x0204);    
        LCD_WriteReg(0x0032,0x0204);    
        LCD_WriteReg(0x0033,0x0502);    
        LCD_WriteReg(0x0034,0x0507);    
        LCD_WriteReg(0x0035,0x0204);    
        LCD_WriteReg(0x0036,0x0204);    
        LCD_WriteReg(0x0037,0x0502);    
        LCD_WriteReg(0x003A,0x0302);    
        LCD_WriteReg(0x003B,0x0302);    
        LCD_WriteReg(0x0023,0x0000);    
        LCD_WriteReg(0x0024,0x0000);    
        LCD_WriteReg(0x0025,0xC000);    //Frame Frequency Control 
        LCD_WriteReg(0x004e,0);         //列(X)首址0
        LCD_WriteReg(0x004f,0);         //行(Y)首址0
        */
		LCD_WriteReg(0x0000,0x0001);//打开晶振
		LCD_WriteReg(0x0003,0xA8A4);//0xA8A4
		LCD_WriteReg(0x000C,0x0000);    
		LCD_WriteReg(0x000D,0x080C);   
		LCD_WriteReg(0x000E,0x2B00);    
		LCD_WriteReg(0x001E,0x00B0);    
		LCD_WriteReg(0x0001,0x2B3F);//驱动输出控制320*240  0x6B3F
		LCD_WriteReg(0x0002,0x0600);
		LCD_WriteReg(0x0010,0x0000);  
		LCD_WriteReg(0x0011,0x6058); //定义数据格式  16位色 		横屏 0x6058
		LCD_WriteReg(0x0005,0x0000);  
		LCD_WriteReg(0x0006,0x0000);  
		LCD_WriteReg(0x0016,0xEF1C);  
		LCD_WriteReg(0x0017,0x0003);  
		LCD_WriteReg(0x0007,0x0233); //0x0233       
		LCD_WriteReg(0x000B,0x0000);  
		LCD_WriteReg(0x000F,0x0000); //扫描开始地址
		LCD_WriteReg(0x0041,0x0000);  
		LCD_WriteReg(0x0042,0x0000);  
		LCD_WriteReg(0x0048,0x0000);  
		LCD_WriteReg(0x0049,0x013F);  
		LCD_WriteReg(0x004A,0x0000);  
		LCD_WriteReg(0x004B,0x0000);  
		LCD_WriteReg(0x0044,0xEF00);  
		LCD_WriteReg(0x0045,0x0000);  
		LCD_WriteReg(0x0046,0x013F);  
		LCD_WriteReg(0x0030,0x0707);  
		LCD_WriteReg(0x0031,0x0204);  
		LCD_WriteReg(0x0032,0x0204);  
		LCD_WriteReg(0x0033,0x0502);  
		LCD_WriteReg(0x0034,0x0507);  
		LCD_WriteReg(0x0035,0x0204);  
		LCD_WriteReg(0x0036,0x0204);  
		LCD_WriteReg(0x0037,0x0502);  
		LCD_WriteReg(0x003A,0x0302);  
		LCD_WriteReg(0x003B,0x0302);  
		LCD_WriteReg(0x0023,0x0000);  
		LCD_WriteReg(0x0024,0x0000);  
		LCD_WriteReg(0x0025,0x8000);  
		LCD_WriteReg(0x004f,0);        //行首址0
		LCD_WriteReg(0x004e,0);        //列首址0		        
    }
		#endif
    else
    {
    	/*9341*/
			LCD_WR_REG(0X01);//RESET
			lcd_delay(720000);
			LCD_WR_REG(0xCF);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0xDB);//83
			LCD_WR_DATA(0X30);

			LCD_WR_REG(0xB1);           
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x18);

			LCD_WR_REG(0xED);
			LCD_WR_DATA(0x64);
			LCD_WR_DATA(0x03);
			LCD_WR_DATA(0X12);
			LCD_WR_DATA(0x81);

			LCD_WR_REG(0xE8);
			LCD_WR_DATA(0x85); 
			LCD_WR_DATA(0x00); 
			LCD_WR_DATA(0x70); 

			LCD_WR_REG(0xCB);
			LCD_WR_DATA(0x39);
			LCD_WR_DATA(0x2C);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x34);
			LCD_WR_DATA(0x02);

			LCD_WR_REG(0xF7);
			LCD_WR_DATA(0x20);

			LCD_WR_REG(0xEA);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x00);

			LCD_WR_REG(0xC0);
			LCD_WR_DATA(0x22);

			LCD_WR_REG(0xC1);
			LCD_WR_DATA(0x12);

			LCD_WR_REG(0xC5);
			LCD_WR_DATA(0x5C);
			LCD_WR_DATA(0x4C);

			LCD_WR_REG(0xC7);
			LCD_WR_DATA(0x8F);

			LCD_WR_REG(0x36);
			
			 //HC-chen 2017.10.11
		  if(gCfgItems.OVERTURN_180!=0xee) //不翻转
	  	{
	  	 	LCD_WR_DATA(0xe8);//use this to use ucgui
	  	}
			else
			{
			 	LCD_WR_DATA(0x28);//use this to use ucgui
			} 

			LCD_WR_REG(0x3A);
			LCD_WR_DATA(0x55);

			LCD_WR_REG(0xF2);
			LCD_WR_DATA(0x02);

			LCD_WR_REG(0x26);
			LCD_WR_DATA(0x01);

			LCD_WR_REG(0xE0);
			LCD_WR_DATA(0x0F);
			LCD_WR_DATA(0x20);
			LCD_WR_DATA(0x19);
			LCD_WR_DATA(0x0F);
			LCD_WR_DATA(0x10);
			LCD_WR_DATA(0x08);
			LCD_WR_DATA(0x4A);
			LCD_WR_DATA(0xF6);
			LCD_WR_DATA(0x3A);
			LCD_WR_DATA(0x0F);
			LCD_WR_DATA(0x14);
			LCD_WR_DATA(0x09);
			LCD_WR_DATA(0x18);
			LCD_WR_DATA(0x0B);
			LCD_WR_DATA(0x08);
			LCD_WR_REG(0xE1);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x1E);
			LCD_WR_DATA(0x1E);
			LCD_WR_DATA(0x05);
			LCD_WR_DATA(0x0F);
			LCD_WR_DATA(0x04);
			LCD_WR_DATA(0x31);
			LCD_WR_DATA(0x33);
			LCD_WR_DATA(0x43);
			LCD_WR_DATA(0x04);
			LCD_WR_DATA(0x0B);
			LCD_WR_DATA(0x06);
			LCD_WR_DATA(0x27);
			LCD_WR_DATA(0x34);
			LCD_WR_DATA(0x0F);

			LCD_WR_REG(0x11);

			lcd_delay(120);

			LCD_WR_REG(0x29);

			LCD_WR_REG(0x2A);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x00);
			// LCD_WR_DATA(0x00);
			// LCD_WR_DATA(0xEF);
			LCD_WR_DATA(0x01);
			LCD_WR_DATA(0x3F);
			LCD_WR_REG(0x2B);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0x00);
			//  LCD_WR_DATA(0x01);
			LCD_WR_DATA(0x00);
			LCD_WR_DATA(0xEF);
			LCD_WR_REG(0x2C);
		
		
      //  LCD_DEBUG_PRINTF("\n\r ###### Err: Unknow DeviceIdCode 0x%x ###### ", DeviceIdCode);
    }

		
    //LCD_Clear(LCD_COLOR_GREEN);

    /* 设置LCD的前景色和背景色 */
    //LCD_SetTextColor(LCD_COLOR_YELLOW);
    //LCD_SetBackColor(LCD_COLOR_RED);

    /* 设置默认字体 Setups the default font */
  //  LCD_SetFont(&Font16x24);    
   // LCD_DisplayStringLine(LCD_LINE_5, (uint8_t *)LogoStr);
   // LCD_DisplayStringLine(LCD_LINE_6, (uint8_t *)Devicestr);
   // LCD_DisplayWelcomeStr(LCD_LINE_3);
    
    /* LCD显示一个灰度二值图片，理解这个函数是理解中英文显示的基础 */
   /* LCD_Image2LcdDrawMonoBinaryPic(2, 2, (const uint8_t *)gImage_ST_ICON);
    LCD_Image2LcdDrawMonoBinaryPic(2, 66, (const uint8_t *)gImage_ST_ICON);
    LCD_Image2LcdDrawMonoBinaryPic(2, 130, (const uint8_t *)gImage_ST_ICON);
    LCD_Image2LcdDrawMonoBinaryPic(2, 194, (const uint8_t *)gImage_ST_ICON);
    LCD_Image2LcdDrawMonoBinaryPic(2, 258, (const uint8_t *)gImage_ST_ICON);
    
    LCD_Image2LcdDrawMonoBinaryPic(206, 2, (const uint8_t *)gImage_ST_ICON);
    LCD_Image2LcdDrawMonoBinaryPic(206, 66, (const uint8_t *)gImage_ST_ICON);
    LCD_Image2LcdDrawMonoBinaryPic(206, 130, (const uint8_t *)gImage_ST_ICON);
    LCD_Image2LcdDrawMonoBinaryPic(206, 194, (const uint8_t *)gImage_ST_ICON);
    LCD_Image2LcdDrawMonoBinaryPic(206, 258, (const uint8_t *)gImage_ST_ICON);*/
        
    //Delay(1000);
}


/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval None
  */
void SZ_STM32_LCDInit(void)
{
    /* Setups the LCD */
    LCD_Setup();

    /* 设置默认字体 Setups the default font */
  //  LCD_SetFont(&LCD_DEFAULT_FONT);
}

/**
  * @brief  Sets the LCD Text and Background colors.
  * @param  _TextColor: specifies the Text Color.
  * @param  _BackColor: specifies the Background Color.
  * @retval None
  */
  #if 0
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
{
    TextColor = _TextColor; 
    BackColor = _BackColor;
}

/**
  * @brief  Gets the LCD Text and Background colors.
  * @param  _TextColor: pointer to the variable that will contain the Text 
            Color.
  * @param  _BackColor: pointer to the variable that will contain the Background 
            Color.
  * @retval None
  */
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor)
{
    *_TextColor = TextColor; *_BackColor = BackColor;
}


/**
  * @brief  Sets the Text color.
  * @param  Color: specifies the Text color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetTextColor(__IO uint16_t Color)
{
    TextColor = Color;
}



/**
  * @brief  Sets the Background color.
  * @param  Color: specifies the Background color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetBackColor(__IO uint16_t Color)
{
    BackColor = Color;
}
#endif


/**
  * @brief  Clears the hole LCD.
  * @param  Color: the color of the background.
  * @retval None
  */
void LCD_Clear(uint16_t Color)
{
    uint32_t index = 0;

    LCD_SetCursor(0,0); 
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

    // 视频加速 www.armjishu.com
    ClrCs
    SetRs
    LCD_Write(Color);
    for(index=0; index<(LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT); index++)
    {
        ClrWr
        //LCD_Write(Color);
        SetWr
    }
    SetCs
}


/**
  * @brief  Sets the cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position. 
  * @retval None
  */
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
/*
	if(DeviceIdCode==0x8989)
	{
	    LCD_WriteReg(0x004e,Xpos);      //行
	    LCD_WriteReg(0x004f,Ypos);      //列
	}
	*/
	
	if(DeviceIdCode==0X8989)
	{
		LCD_WriteReg(0X4E, Ypos);
		LCD_WriteReg(0X4F, 319-Xpos);
   }
	else 
	if((DeviceIdCode==0x9320))
	{
	    LCD_WriteReg(0x0020,Xpos);      // 行
	    LCD_WriteReg(0x0021,0x13f-Ypos);// 列
	}
	
	else if((DeviceIdCode==0x9919))
	{
	    LCD_WriteReg(0x004e,Xpos);      // 行
	    LCD_WriteReg(0x004f,Ypos);      // 列    
	}
	
	else if((DeviceIdCode==0x9325)||(DeviceIdCode==0x9328)||(DeviceIdCode==0x1505))
	{
	  	LCD_WriteReg(0x0020,Ypos);      // 列
        LCD_WriteReg(0x0021,0x13f-Xpos);// 行
	}
	else
	{
		LCD_WR_REG(0X2A); 
		LCD_WR_DATA(Xpos>>8); 
		LCD_WR_DATA(Xpos&0XFF);         
		LCD_WR_REG(0X2B); 
		LCD_WR_DATA(Ypos>>8); 
		LCD_WR_DATA(Ypos&0XFF);
	}
}

/**
  * @brief  Displays a Point.
  * @param  Color: specifies the Color.
  * @retval None
  */
void LCD_SetPoint(uint16_t Xpos, uint16_t Ypos, uint16_t Color)
{ 
    LCD_SetCursor(Xpos, Ypos);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(Color);
    SetCs
}

/**
  * @brief  Displays a pixel.
  * @param  x: pixel x.
  * @param  y: pixel y.  
  * @retval None
  */
void PutPixel(int16_t Xpos, int16_t Ypos)
{ 
    if(Xpos < 0 || Xpos >LCD_PIXEL_WIDTH  || Ypos < 0 || Ypos > LCD_PIXEL_HEIGHT)
    {
        return;  
    }
    LCD_SetPoint(Xpos, Ypos, TextColor);
}

/**
  * @brief  Displays a Point.
  * @param  Color: specifies the Color.
  * @retval None
  */
uint16_t LCD_GetPoint(uint16_t Xpos, uint16_t Ypos)
{ 
    uint16_t Color;
    LCD_SetCursor(Xpos, Ypos);

    /* Write 16-bit Index (then Read Reg) */
    ClrCs
    ClrRs
    ClrWr
    LCD_Write(LCD_REG_34);
    SetWr
    SetRs


    LCD_DB_AS_InPut();
    /* Read 16-bit Reg */
    ClrRd  //Dummy read
    SetRd
    ClrRd
    SetRd
    Color = LCD_Read();
    SetCs

    LCD_DB_AS_OutPut();

    return (Color);
}
#if 0

void LCD_RAM_WrRdTest(void)
{
    uint16_t ReadData = 0;  
    
    LCD_SetPoint(1,1,0x0);
    ReadData = LCD_GetPoint(1,1);
    if(0x0 != ReadData)
    {
        //LCD_DEBUG_PRINTF("\r\n Err: LCD_SetPoint(1,1,0x0) LCD_GetPoint(1,1)=0x%04X", ReadData);
    }
    LCD_SetPoint(1,1,0xFFFF);
    ReadData = LCD_GetPoint(1,1);
    if(0xFFFF != ReadData)
    {
        //LCD_DEBUG_PRINTF("\r\n Err: LCD_SetPoint(1,1,0x0) LCD_GetPoint(1,1)=0x%04X", ReadData);
    }

    LCD_SetPoint(1,1,0x5a5a);
    ReadData = LCD_GetPoint(1,1);
    if(0x5a5a != ReadData)
    {
        //LCD_DEBUG_PRINTF("\r\n Note: LCD Write order is R-G-B, but Read order is B-G-R");
    }
}


/*=====================================================================*/
void LCD_DrawBigPoint(uint16_t xScreen, uint16_t yScreen, uint16_t Color)
{
    LCD_SetPoint(xScreen,   yScreen,   Color);
    LCD_SetPoint(xScreen-1, yScreen,   Color);
    LCD_SetPoint(xScreen+1, yScreen,   Color);
    LCD_SetPoint(xScreen,   yScreen-1, Color);
    LCD_SetPoint(xScreen,   yScreen+1, Color);
    LCD_SetPoint(xScreen-1, yScreen-1, Color);
    LCD_SetPoint(xScreen-1, yScreen+1, Color);
    LCD_SetPoint(xScreen+1, yScreen-1, Color);
    LCD_SetPoint(xScreen+1, yScreen+1, Color);
}
#endif
/**
  * @brief  Displays a line.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Length: line length.
  * @param  Direction: line direction.
  *   This parameter can be one of the following values: Vertical or Horizontal.
  * @retval None
  */
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction)
{
    uint32_t i = 0;

    LCD_SetCursor(Xpos, Ypos);

    if(Direction == LCD_DIR_HORIZONTAL)
    { 
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        LCD_WriteRAM(TextColor);
        for(i = 1; i < Length; i++)
        {
            ClrWr
            SetWr
        }
    }
    else
    {
        for(i = 0; i < Length; i++)
        {
            LCD_WriteRAMWord(TextColor);
            Xpos++;
            LCD_SetCursor(Xpos, Ypos);
        }
    }
    SetCs
}

/**
  * @brief  Displays a Cross.
  * @param  Xpos: specifies the left X position.
  * @param  Ypos: specifies the botton Y position.
  * @retval None
  */
void LCD_DrawCross(uint16_t Xpos, uint16_t Ypos)
{
    LCD_DrawLine(Xpos, Ypos - 15, 30, LCD_DIR_HORIZONTAL); /* Horizontal Line */
    LCD_DrawLine(Xpos - 15, Ypos, 30, LCD_DIR_VERTICAL);   /* Vertical Line   */
}


/**
  * @brief  Displays a rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: display rectangle height.
  * @param  Width: display rectangle width.
  * @retval None
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
    LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
    LCD_DrawLine((Xpos + Height), Ypos, Width, LCD_DIR_HORIZONTAL);

    LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
    LCD_DrawLine(Xpos, (Ypos + Width - 1), Height, LCD_DIR_VERTICAL);
}


/**
  * @brief  Displays a circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
    int32_t   D;    /* Decision Variable */
    uint32_t  CurX; /* Current X Value */
    uint32_t  CurY; /* Current Y Value */ 

    D = 3 - (Radius << 1);
    CurX = 0;
    CurY = Radius;

    while (CurX <= CurY)
    {
        LCD_SetCursor(Xpos + CurX, Ypos + CurY);
        LCD_WriteRAMWord(TextColor);
        LCD_SetCursor(Xpos + CurX, Ypos - CurY);
        LCD_WriteRAMWord(TextColor);
        LCD_SetCursor(Xpos - CurX, Ypos + CurY);
        LCD_WriteRAMWord(TextColor);
        LCD_SetCursor(Xpos - CurX, Ypos - CurY);
        LCD_WriteRAMWord(TextColor);
        LCD_SetCursor(Xpos + CurY, Ypos + CurX);
        LCD_WriteRAMWord(TextColor);
        LCD_SetCursor(Xpos + CurY, Ypos - CurX);
        LCD_WriteRAMWord(TextColor);
        LCD_SetCursor(Xpos - CurY, Ypos + CurX);
        LCD_WriteRAMWord(TextColor);
        LCD_SetCursor(Xpos - CurY, Ypos - CurX);
        LCD_WriteRAMWord(TextColor);
        if (D < 0)
        { 
            D += (CurX << 2) + 6;
        }
        else
        {
            D += ((CurX - CurY) << 2) + 10;
            CurY--;
        }
        CurX++;
    }
}
#if 0

/**
  * @brief  Displays a full rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: rectangle height.
  * @param  Width: rectangle width.
  * @retval None
  */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
    uint16_t OldTextColor = TextColor;

    LCD_DrawRect(Xpos, Ypos, Width, Height);

    Width -= 2;
    Height--;
    Ypos++;

    LCD_SetTextColor(BackColor);

    while(Height--)
    {
        LCD_DrawLine(++Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);    
    }

    LCD_SetTextColor(OldTextColor);
}

/**
  * @brief  Displays a full circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
    int32_t  D;    /* Decision Variable */ 
    uint32_t  CurX;/* Current X Value */
    uint32_t  CurY;/* Current Y Value */ 
    uint16_t OldTextColor = TextColor;

    D = 3 - (Radius << 1);

    CurX = 0;
    CurY = Radius;

    LCD_SetTextColor(BackColor);

    while (CurX <= CurY)
    {
        if(CurY > 0) 
        {
            LCD_DrawLine(Xpos - CurX, Ypos - CurY, 2*CurY, LCD_DIR_HORIZONTAL);
            LCD_DrawLine(Xpos + CurX, Ypos - CurY, 2*CurY, LCD_DIR_HORIZONTAL);
        }

        if(CurX > 0) 
        {
            LCD_DrawLine(Xpos - CurY, Ypos - CurX, 2*CurX, LCD_DIR_HORIZONTAL);
            LCD_DrawLine(Xpos + CurY, Ypos - CurX, 2*CurX, LCD_DIR_HORIZONTAL);
        }
        if (D < 0)
        { 
            D += (CurX << 2) + 6;
        }
        else
        {
            D += ((CurX - CurY) << 2) + 10;
            CurY--;
        }
        CurX++;
    }

    LCD_SetTextColor(OldTextColor);
    LCD_DrawCircle(Xpos, Ypos, Radius);
}

/**
  * @brief  Displays an uni line (between two points).
  * @param  x1: specifies the point 1 x position.
  * @param  y1: specifies the point 1 y position.
  * @param  x2: specifies the point 2 x position.
  * @param  y2: specifies the point 2 y position.
  * @retval None
  */
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
    yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
    curpixel = 0;

    deltax = ABS(x2 - x1);        /* The difference between the x's */
    deltay = ABS(y2 - y1);        /* The difference between the y's */
    x = x1;                       /* Start x off at the first pixel */
    y = y1;                       /* Start y off at the first pixel */

    if (x2 >= x1)                 /* The x-values are increasing */
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else                          /* The x-values are decreasing */
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1)                 /* The y-values are increasing */
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else                          /* The y-values are decreasing */
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay)         /* There is at least one x-value for every y-value */
    {
        xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
        yinc2 = 0;                  /* Don't change the y for every iteration */
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;         /* There are more x-values than y-values */
    }
    else                          /* There is at least one y-value for every x-value */
    {
        xinc2 = 0;                  /* Don't change the x for every iteration */
        yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;         /* There are more y-values than x-values */
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        PutPixel(x, y);             /* Draw the current pixel */
        num += numadd;              /* Increase the numerator by the top of the fraction */
        if (num >= den)             /* Check if numerator >= denominator */
        {
            num -= den;               /* Calculate the new numerator value */
            x += xinc1;               /* Change the x as appropriate */
            y += yinc1;               /* Change the y as appropriate */
        }
        x += xinc2;                 /* Change the x as appropriate */
        y += yinc2;                 /* Change the y as appropriate */
    }
}

/**
  * @brief  Displays an polyline (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_PolyLine(pPoint Points, uint16_t PointCount)
{
    int16_t X = 0, Y = 0;

    if(PointCount < 2)
    {
        return;
    }

    while(--PointCount)
    {
        X = Points->X;
        Y = Points->Y;
        Points++;
        LCD_DrawUniLine(X, Y, Points->X, Points->Y);
    }
}

/**
  * @brief  Displays an relative polyline (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @param  Closed: specifies if the draw is closed or not.
  *           1: closed, 0 : not closed.
  * @retval None
  */
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed)
{
    int16_t X = 0, Y = 0;
    pPoint First = Points;

    if(PointCount < 2)
    {
        return;
    }  
    X = Points->X;
    Y = Points->Y;
    while(--PointCount)
    {
        Points++;
        LCD_DrawUniLine(X, Y, X + Points->X, Y + Points->Y);
        X = X + Points->X;
        Y = Y + Points->Y;
    }
    if(Closed)
    {
        LCD_DrawUniLine(First->X, First->Y, X, Y);
    }  
}

/**
  * @brief  Displays a closed polyline (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount)
{
    LCD_PolyLine(Points, PointCount);
    LCD_DrawUniLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
}

/**
  * @brief  Displays a relative polyline (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount)
{
    LCD_PolyLineRelativeClosed(Points, PointCount, 0);
}

/**
  * @brief  Displays a closed relative polyline (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount)
{
    LCD_PolyLineRelativeClosed(Points, PointCount, 1);
}


/**
  * @brief  Displays a  full polyline (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount)
{
    /*  public-domain code by Darel Rex Finley, 2007 */
    uint16_t  nodes = 0, nodeX[MAX_POLY_CORNERS], pixelX = 0, pixelY = 0, i = 0,
    j = 0, swap = 0;
    uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;

    IMAGE_LEFT = IMAGE_RIGHT = Points->X;
    IMAGE_TOP= IMAGE_BOTTOM = Points->Y;

    for(i = 1; i < PointCount; i++)
    {
        pixelX = POLY_X(i);
        if(pixelX < IMAGE_LEFT)
        {
            IMAGE_LEFT = pixelX;
        }
        if(pixelX > IMAGE_RIGHT)
        {
            IMAGE_RIGHT = pixelX;
        }

        pixelY = POLY_Y(i);
        if(pixelY < IMAGE_TOP)
        { 
            IMAGE_TOP = pixelY;
        }
        if(pixelY > IMAGE_BOTTOM)
        {
            IMAGE_BOTTOM = pixelY;
        }
    }

    LCD_SetTextColor(BackColor);  

    /*  Loop through the rows of the image. */
    for (pixelY = IMAGE_TOP; pixelY < IMAGE_BOTTOM; pixelY++) 
    {  
        /* Build a list of nodes. */
        nodes = 0; j = PointCount-1;

        for (i = 0; i < PointCount; i++) 
        {
            if (POLY_Y(i)<(double) pixelY && POLY_Y(j)>=(double) pixelY || POLY_Y(j)<(double) pixelY && POLY_Y(i)>=(double) pixelY) 
            {
                nodeX[nodes++]=(int) (POLY_X(i)+((pixelY-POLY_Y(i))*(POLY_X(j)-POLY_X(i)))/(POLY_Y(j)-POLY_Y(i))); 
            }
            j = i; 
        }

        /* Sort the nodes, via a simple 揃ubble?sort. */
        i = 0;
        while (i < nodes-1) 
        {
            if (nodeX[i]>nodeX[i+1]) 
            {
                swap = nodeX[i]; 
                nodeX[i] = nodeX[i+1]; 
                nodeX[i+1] = swap; 
            if(i)
            {
                i--; 
            }
            }
            else 
            {
                i++;
            }
        }

        /*  Fill the pixels between node pairs. */
        for (i = 0; i < nodes; i+=2) 
        {
            if(nodeX[i] >= IMAGE_RIGHT) 
            {
                break;
            }
            if(nodeX[i+1] > IMAGE_LEFT) 
            {
                if (nodeX[i] < IMAGE_LEFT)
                {
                    nodeX[i]=IMAGE_LEFT;
                }
                if(nodeX[i+1] > IMAGE_RIGHT)
                {
                    nodeX[i+1] = IMAGE_RIGHT;
                }
                LCD_SetTextColor(BackColor);
                LCD_DrawLine(pixelY, nodeX[i+1], nodeX[i+1] - nodeX[i], LCD_DIR_HORIZONTAL);
                LCD_SetTextColor(TextColor);
                PutPixel(pixelY, nodeX[i+1]);
                PutPixel(pixelY, nodeX[i]);
                /* for (j=nodeX[i]; j<nodeX[i+1]; j++) PutPixel(j,pixelY); */
            }
        }
    } 

    /* draw the edges */
    LCD_SetTextColor(TextColor);
}
#endif

/**
  * @brief  Writes index to select the LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @retval None
  */
void LCD_WriteRegIndex(uint8_t LCD_Reg)
{
    ClrCs
    ClrRs
    ClrWr
    LCD_Write(LCD_Reg);
    SetWr
    SetCs
}


/**
  * @brief  Reads the selected LCD Register.
  * @param  None
  * @retval LCD Register Value.
  */
uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
    uint16_t data;

    /* Write 16-bit Index (then Read Reg) */
    ClrCs
    ClrRs
    ClrWr
    LCD_Write(LCD_Reg);
    SetWr

    LCD_DB_AS_InPut();

    /* Read 16-bit Reg */
    SetRs
    ClrRd
    SetRd
    data = LCD_Read(); 
    SetCs
    LCD_DB_AS_OutPut();

    return    data;
}


/**
  * @brief  Prepare to write to the LCD RAM.
  * @param  None
  * @retval None
  */
void LCD_WriteRAM_Prepare(void)
{
    /* Write 16-bit Index, then Write Reg */
    ClrCs
    ClrRs
    ClrWr
	if((DeviceIdCode==0x9325)||(DeviceIdCode==0x9328)||(DeviceIdCode==0x8989)||(DeviceIdCode==0x1505))
    {
    	LCD_Write(LCD_REG_34);
	}
	else
	{
		LCD_Write(0x2c);
	}
    SetWr
    SetRs
}


/**
  * @brief  Writes 1 word to the LCD RAM.
  * @param  RGB_Code: the pixel color in RGB mode (5-6-5).
  * @retval None
  */
void LCD_WriteRAMWord(uint16_t RGB_Code)
{
    LCD_WriteRAM_Prepare();
    LCD_WriteRAM(RGB_Code);
    SetCs
}


/**
  * @brief  Writes to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @param  LCD_RegValue: value to write to the selected register.
  * @retval None
  */
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
#if 1
    /* Write 16-bit Index, then Write Reg */
    ClrCs
    ClrRs
    ClrWr
    LCD_Write(LCD_Reg);
    SetWr
    /* Write 16-bit Reg */
    SetRs
    ClrWr
    LCD_Write(LCD_RegValue);
    SetWr
    SetCs
    #else
LCD_WR_REG(LCD_Reg);  
     LCD_WR_DATA(LCD_RegValue); 
	 #endif
}

void LLCD_WRITE_CMD(uint8_t LCD_Reg)
{
	    /* Write 16-bit Index, then Write Reg */
	    ClrCs
	    ClrRs
	    ClrWr
	    LCD_Write(LCD_Reg);
	    SetWr
}

/**
  * @brief  Writes to the LCD RAM.
  * @param  RGB_Code: the pixel color in RGB mode (5-6-5).
  * @retval None
  */
void LCD_WriteRAM(uint16_t RGB_Code)
{
    ClrWr
    LCD_Write(RGB_Code);
    SetWr
}


/**
  * @brief  Power on the LCD.
  * @param  None
  * @retval None
  */
void LCD_PowerOn(void)
{ 
}


/*******************************************************************************
* Function Name  : LCD_Pins_Config
* Description    : Configures LCD Pins
                   Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Pins_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(LCD_CLK_RS  | LCD_CLK_DATA |
                            LCD_CLK_WR | LCD_CLK_RD   |
                            LCD_CLK_CS, ENABLE);

    // DB15--0
    GPIO_InitStructure.GPIO_Pin = LCD_PIN_DATA;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_PORT_DAT, &GPIO_InitStructure);

    //LCD_Pin_WR
    GPIO_InitStructure.GPIO_Pin = LCD_Pin_WR;
    GPIO_Init(LCD_PORT_WR, &GPIO_InitStructure);

    //LCD_Pin_CS
    GPIO_InitStructure.GPIO_Pin = LCD_Pin_CS;
    GPIO_Init(LCD_PORT_CS, &GPIO_InitStructure);

    //LCD_Pin_RS
    GPIO_InitStructure.GPIO_Pin = LCD_Pin_RS;
    GPIO_Init(LCD_PORT_RS, &GPIO_InitStructure);

    //LCD_Pin_RD
    GPIO_InitStructure.GPIO_Pin = LCD_Pin_RD;
    GPIO_Init(LCD_PORT_RD, &GPIO_InitStructure);

    SetCs
    SetWr
    SetRd
    SetRs
}

/* ST的LOGO灰度二值图片数据库 */
/*const unsigned char gImage_ST_ICON[262] = { 0X00,0X01,0X3B,0X00,0X20,0X00,
0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,
0X00,0X07,0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,0X00,0X07,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,
0X00,0X0F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0E,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X1C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X18,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X30,0X1F,0XFF,0XFF,0X03,0XFE,0X00,0X00,0X30,0X3F,0XFF,0XFF,0X03,0XFE,0X00,
0X00,0X70,0X3F,0XFF,0XFE,0X07,0XFC,0X00,0X00,0X70,0X3F,0XFF,0XFE,0X07,0XFC,0X00,
0X00,0XF8,0X1F,0XFF,0XFC,0X0F,0XF8,0X00,0X00,0XF8,0X0F,0XFF,0XFC,0X0F,0XF8,0X00,
0X01,0XFC,0X07,0XFF,0XF8,0X1F,0XF0,0X00,0X01,0XFE,0X03,0XFF,0XF8,0X1F,0XF0,0X00,
0X03,0XFF,0X01,0XFF,0XF0,0X3F,0XE0,0X00,0X03,0XFF,0X80,0XFF,0XF0,0X3F,0XE0,0X00,
0X07,0XFF,0XC0,0X7F,0XE0,0X7F,0XC0,0X00,0X07,0XFF,0XE0,0X3F,0XE0,0X7F,0XC0,0X00,
0X0F,0XFF,0XF0,0X1F,0XC0,0XFF,0X80,0X00,0X0F,0XFF,0XF8,0X0F,0XC0,0XFF,0X80,0X00,
0X1F,0XFF,0XFC,0X0F,0X81,0XFF,0X00,0X00,0X1F,0XFF,0XFE,0X0F,0X81,0XFF,0X00,0X00,
0X3F,0XFF,0XFE,0X0F,0X03,0XFE,0X00,0X00,0X3F,0XFF,0XFE,0X0F,0X03,0XFE,0X00,0X00,
0X7F,0XFF,0XFC,0X0E,0X07,0XFC,0X00,0X00,0X00,0X00,0X00,0X1E,0X07,0XFC,0X00,0X00,
0X00,0X00,0X00,0X1C,0X0F,0XF8,0X00,0X00,0X00,0X00,0X00,0X3C,0X0F,0XF8,0X00,0X00,
0X00,0X00,0X00,0X78,0X1F,0XF0,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XF0,0X00,0X00,
};*/
/*
               *********************************************
               ******************************************** 
              ********************************************* 
              ********************************************  
             ****                                           
             ***                                            
            ***                                             
            **                                              
           **       *********************      *********    
           **      **********************      *********    
          ***      *********************      *********     
          ***      *********************      *********     
         *****      *******************      *********      
         *****       ******************      *********      
        *******       ****************      *********       
        ********       ***************      *********       
       **********       *************      *********        
       ***********       ************      *********        
      *************       **********      *********         
      **************       *********      *********         
     ****************       *******      *********          
     *****************       ******      *********          
    *******************      *****      *********           
    ********************     *****      *********           
   *********************     ****      *********            
   *********************     ****      *********            
  *********************      ***      *********             
                            ****      *********             
                            ***      *********              
                           ****      *********              
                          ****      *********               
 ******************************************** 
*/


void LCD_setWindowArea(uint16_t StartX, uint16_t StartY, uint16_t Width, uint16_t Height)
{
	int8_t s_h,s_l, e_h, e_l;

	 if((DeviceIdCode==0x9325)||(DeviceIdCode==0x9328)||(DeviceIdCode==0x1505))
    {
    	LCD_WriteReg(0x0050,StartY);        //Specify the start/end positions of the window address in the horizontal direction by an address unit
		LCD_WriteReg(0x0051,StartY + Height -1);        //Specify the start positions of the window address in the vertical direction by an address unit 
		LCD_WriteReg(0x0052,320 - StartX - Width); 
		LCD_WriteReg(0x0053,320 - StartX - 1 );        //Specify the end positions of the window address in the vertical direction by an address unit
	 }
	 
	 else if(DeviceIdCode==0x8989)
	 {
        uint16_t h_value;
        h_value = ((StartY + Height -1)<<8)|StartY;
        LCD_WriteReg(0x44,h_value);     //水平
        LCD_WriteReg(0x45,320 - StartX - Width); //垂直start pos
        LCD_WriteReg(0x46,320 - StartX - 1 ); //垂直 end pos
        LCD_SetCursor(StartX, StartY);//设置起始绘图的GRAM地址    
   	    LCD_WR_REG(0x0022);//开始读/写  	 	
	 }	 
	 
	 else
	 {
	 	s_h = (StartX >> 8) & 0Xff;
		s_l = StartX & 0Xff;
		e_h = ((StartX + Width - 1) >> 8) & 0Xff;
		e_l = (StartX + Width - 1) & 0Xff;
		
		LCD_WR_REG(0x2A);
	                        LCD_WR_DATA(s_h);
	                        LCD_WR_DATA(s_l);
	                       // LCD_WR_DATA(0x00);
	                       // LCD_WR_DATA(0xEF);
	                        LCD_WR_DATA(e_h);
	                        LCD_WR_DATA(e_l);

		s_h = (StartY >> 8) & 0Xff;
		s_l = StartY & 0Xff;
		e_h = ((StartY + Height - 1) >> 8) & 0Xff;
		e_l = (StartY + Height - 1) & 0Xff;
		
	        LCD_WR_REG(0x2B);
	                        LCD_WR_DATA(s_h);
	                        LCD_WR_DATA(s_l);
	                      //  LCD_WR_DATA(0x01);
	                        LCD_WR_DATA(e_h);
	                        LCD_WR_DATA(e_l);
	 }
    
	

/*	 
*/
}
#if 0

/**-------------------------------------------------------
  * @函数名 LCD_DrawMonoBinaryPic
  * @功能   在指定座标范围显示一幅已知长宽的二值图片
  * @参数   StartX     行起始座标
  *         StartY     列起始座标
  *         Width      图片宽度
  *         Height     图片高度
  *         BmpAddress 图片数据区域头指针(不含文件信息)
  * @返回值 无
  * 作者    www.armjishu.com
***------------------------------------------------------*/
void LCD_DrawMonoBinaryPic(uint16_t StartX, uint16_t StartY, uint16_t Width, uint16_t Height, const uint8_t *BmpDataAddress)
{
    uint32_t i, j;
    uint16_t line;
    uint8_t  Mask, Temp = 0x00;

    //LCD_DEBUG_PRINTF("\n\r LCD_DrawMonoPic Start X %d Y %d Width %d Height %d"
    //                            , StartX, StartY, Width, Height);

    line=StartX;
    for (i=0;i<Height;i++)
    {
        LCD_SetCursor(line, StartY);
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        Mask = 0x80;
        Temp = *BmpDataAddress;
        BmpDataAddress++;
        //printf("\r\n ");
        for (j=0;j<Width;j++)
        {
            if (Mask == 0)
            {
                /*Read next byte of bit map image and initiate mask*/
                Temp = *BmpDataAddress;
                BmpDataAddress++;
                Mask = 0x80;
             }
            /*Mask actual byte of cursor bitmap image*/
            if (Temp & Mask)
            {
                 LCD_WriteRAM(TextColor); //putchar('*');
            }
            else
            {
                 LCD_WriteRAM(BackColor); //putchar(' ');           
            }
            /*Shift a mask right*/
            Mask >>= 1;
        }
        line++;
    }
}


/**-------------------------------------------------------
  * @函数名 LCD_Image2LcdDrawMonoBinaryPic
  * @功能   在指定座标范围显示一幅Image2Lcd转换的二值图片
  * @参数   StartX     行起始座标
  *         StartY     列起始座标
  *         BmpAddress 图片文件头的地址指针(含有文件信息)
  * @返回值 无
  * 作者    www.armjishu.com
***------------------------------------------------------*/
void LCD_Image2LcdDrawMonoBinaryPic(uint16_t StartX, uint16_t StartY, const uint8_t *BmpAddress)
{
    HEADGRAY * BmpHeadr;
    uint16_t Width, Height;
    uint8_t *BmpData;

    BmpHeadr = (HEADGRAY *)BmpAddress;
    Width  = BmpHeadr->w;
    Height = BmpHeadr->h;
    BmpData = (uint8_t *)(BmpAddress + sizeof(HEADGRAY));

    LCD_DrawMonoBinaryPic(StartX, StartY, Width, Height, (const uint8_t *)BmpData);
}


/**-------------------------------------------------------
  * @函数名 LCD_DrawBmp565Pic
  * @功能   在指定座标范围显示一幅Image2Lcd转换的RGB565图片
  * @参数   StartX     行起始座标
  *         StartY     列起始座标
  *         Width      图片宽度
  *         Height     图片高度
  *         BmpAddress 图片数据区域头指针(不含文件信息)
  * @返回值 无
  * 作者    www.armjishu.com
***------------------------------------------------------*/
void LCD_DrawBmp565Pic(uint16_t StartX, uint16_t StartY, uint16_t Width, uint16_t Height, const uint16_t *BmpAddress)
{
    uint32_t total;
    uint32_t i, j;
    uint32_t pointor;
    uint16_t line;

    //LCD_DEBUG_PRINTF("LCD_DrawBmp565Picture StartX %d StartY %d Width %d Height %d \n\r"
    //                              , StartX, StartY, Width, Height);

    line=StartY;
#if 0  // 这种方法绘制速度快，但是寄存器因为LCD驱动芯片的不同而不同
    total = Width * Height;

    LCD_WriteReg(0x0044,0xEF00 + StartX);        //Specify the start/end positions of the window address in the horizontal direction by an address unit
    LCD_WriteReg(0x0045,StartY);        //Specify the start positions of the window address in the vertical direction by an address unit 
    LCD_WriteReg(0x0046,StartY + Width -1 );        //Specify the end positions of the window address in the vertical direction by an address unit 

    LCD_SetCursor(line, StartY);
  	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

    for (i=0;i<total;i++)
    {
        LCD_WriteRAM(BmpAddress[i]);
    }

    LCD_WriteReg(0x0044,0xEF00);        //Specify the start/end positions of the window address in the horizontal direction by an address unit
    LCD_WriteReg(0x0045,0x0000);        //Specify the start positions of the window address in the vertical direction by an address unit 
    LCD_WriteReg(0x0046,0x013F);        //Specify the end positions of the window address in the vertical direction by an address unit 
#else
    pointor = 0;
    for (i=0;i<Height;i++)
    {
        LCD_SetCursor(StartX, line);
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        for (j=0;j<Width;j++)
        {
            LCD_WriteRAM(BmpAddress[pointor]);
            pointor++;
        }
        line++;
    }
#endif
}


/**-------------------------------------------------------
  * @函数名 LCD_Image2LcdDrawBmp565Pic
  * @功能   在指定座标范围显示一幅Image2Lcd转换的Bmp565图片
  * @参数   StartX     行起始座标
  *         StartY     列起始座标
  *         BmpAddress 图片文件头的地址指针(含有文件信息)
  * @返回值 无
  * 作者    www.armjishu.com
***------------------------------------------------------*/
void LCD_Image2LcdDrawBmp565Pic(uint16_t StartX, uint16_t StartY, const uint8_t *BmpAddress)
{
    HEADCOLOR * BmpHeadr;

    uint16_t Width, Height;
    uint8_t *BmpData;

    BmpHeadr = (HEADCOLOR *)BmpAddress;
    Width  = BmpHeadr->w;
    Height = BmpHeadr->h;
    BmpData = (uint8_t *)(BmpAddress + sizeof(HEADCOLOR));
    if((BmpHeadr->is565 == 1) && (BmpHeadr->gray == 16))
    {
        LCD_DrawBmp565Pic(StartX, StartY, Width, Height, (const uint16_t *)BmpData);
    }
    else
    {
        //LCD_DEBUG_PRINTF("\n\r LCD_Image2LcdDrawBmp565Pic: Not Image2Lcd or Bmp565 format.");
    }
}



/*  字符串"★欢迎您使用神舟系列开发板"的字库  */ 
/*const uint8_t WelcomeStr[13][72]={
    //★欢迎您使用神舟系列开发板
    //No:0	★   使用频度=1
    {	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,
    0x18,0x00,0x00,0x3C,0x00,0x00,0x3C,0x00,0x00,0x7C,0x00,0x3F,0xFF,0xFC,0x1F,0xFF,0xF8,0x07,0xFF,
    0xE0,0x03,0xFF,0xC0,0x01,0xFF,0x80,0x01,0xFF,0x80,0x01,0xFF,0x80,0x01,0xFF,0x80,0x03,0xE7,0xC0,
    0x03,0x81,0xC0,0x03,0x00,0xC0,0x04,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00},
    //No:1	欢   使用频度=1
    {	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x06,0x00,0x00,0x46,0x00,0x3F,0xC4,0x00,0x00,
    0xC4,0x04,0x00,0x8F,0xFE,0x20,0x88,0x08,0x11,0x89,0x90,0x09,0x11,0x80,0x05,0x21,0x80,0x02,0x21,
    0x80,0x03,0x03,0x80,0x05,0x82,0x80,0x05,0x82,0x40,0x08,0xC2,0x40,0x08,0xC4,0x60,0x10,0x4C,0x20,
    0x20,0x08,0x30,0x40,0x10,0x18,0x00,0x60,0x0E,0x00,0x80,0x00,0x00,0x00,0x00},
    //No:2	迎   使用频度=1
    {	0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x02,0x00,0x0C,0x0E,0x00,0x06,0x72,0x08,0x04,0x41,0xFC,0x00,
    0x41,0x08,0x00,0x41,0x08,0x04,0x41,0x08,0x7E,0x41,0x08,0x04,0x41,0x08,0x04,0x41,0x08,0x04,0x41,
    0x08,0x04,0x45,0x08,0x04,0x59,0x08,0x04,0x61,0x78,0x04,0x41,0x18,0x04,0x01,0x00,0x1A,0x01,0x00,
    0x71,0x00,0x00,0x60,0xE0,0x02,0x00,0x3F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00},
    //No:3	您   使用频度=1
    {	0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x18,0x00,0x06,0x18,0x00,0x04,0x30,0x00,0x0C,0x3F,0xFC,0x0C,
    0x44,0x10,0x14,0x43,0x20,0x24,0x82,0x00,0x44,0x32,0x40,0x04,0x22,0x20,0x04,0x42,0x18,0x04,0x82,
    0x18,0x05,0x1E,0x08,0x04,0x06,0x00,0x00,0x20,0x00,0x01,0x98,0x20,0x09,0x8C,0x10,0x09,0x88,0x4C,
    0x19,0x80,0x4C,0x31,0x80,0x44,0x01,0x80,0xE0,0x00,0xFF,0xC0,0x00,0x00,0x00},
    //No:4	使   使用频度=1
    {	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x81,0x00,0x03,0x01,0x00,0x03,0x01,0x04,0x02,0xFF,0xFE,0x06,
    0x01,0x00,0x04,0x01,0x00,0x0E,0x21,0x18,0x0E,0x3F,0xE8,0x16,0x21,0x08,0x26,0x21,0x08,0x26,0x21,
    0x08,0x46,0x3F,0xF8,0x06,0x21,0x00,0x06,0x13,0x00,0x06,0x12,0x00,0x06,0x0A,0x00,0x06,0x06,0x00,
    0x06,0x06,0x00,0x06,0x0F,0x00,0x06,0x10,0xE0,0x06,0x60,0x3C,0x01,0x80,0x00},
    //No:5	用   使用频度=1
    {	0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x10,0x0F,0xFF,0xF8,0x0C,0x08,0x10,0x0C,0x08,0x10,0x0C,
    0x08,0x10,0x0C,0x08,0x10,0x0C,0x08,0x10,0x0F,0xFF,0xF0,0x08,0x08,0x10,0x08,0x08,0x10,0x08,0x08,
    0x10,0x08,0x08,0x10,0x0F,0xFF,0xF0,0x08,0x08,0x10,0x08,0x08,0x10,0x08,0x08,0x10,0x10,0x18,0x10,
    0x10,0x18,0x10,0x20,0x18,0x10,0x20,0x18,0xF0,0x40,0x00,0x30,0x00,0x00,0x00},
    //No:6	神   使用频度=1
    {	0x00,0x00,0x00,0x00,0x01,0x00,0x04,0x00,0x80,0x02,0x00,0x80,0x02,0x00,0x80,0x00,0x00,0x88,0x00,
    0x9F,0xFC,0x3F,0x90,0x88,0x01,0x10,0x88,0x03,0x10,0x88,0x02,0x1F,0xF8,0x06,0x10,0x88,0x0D,0x90,
    0x88,0x0C,0xD0,0x88,0x14,0x50,0x8C,0x24,0x1F,0xFC,0x44,0x10,0x88,0x04,0x00,0x80,0x04,0x00,0x80,
    0x04,0x00,0x80,0x04,0x00,0x80,0x04,0x00,0x80,0x04,0x01,0x80,0x00,0x01,0x00},
    //No:7	舟   使用频度=1
    {	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x10,0x00,0x00,0x20,0x00,0x03,0xFF,0xE0,0x02,
    0x00,0x40,0x02,0x20,0x40,0x02,0x10,0x40,0x02,0x18,0x40,0x02,0x08,0x40,0x02,0x00,0x40,0x02,0x00,
    0x4E,0x3F,0xFF,0xF0,0x02,0x00,0x40,0x02,0x20,0x40,0x02,0x10,0x40,0x06,0x18,0x40,0x06,0x18,0x40,
    0x04,0x00,0x40,0x08,0x00,0x40,0x10,0x07,0xC0,0x20,0x01,0x80,0x40,0x00,0x00},
    //No:8	系   使用频度=1
    {	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,0x7F,0x00,0x1F,0x98,0x00,0x00,0x30,0x00,0x00,
    0x40,0x40,0x00,0x80,0xC0,0x07,0x3F,0x00,0x07,0xC4,0x00,0x00,0x18,0x00,0x00,0x60,0x40,0x00,0x80,
    0x20,0x07,0x7F,0xF8,0x07,0x88,0x18,0x01,0x08,0x00,0x00,0xC8,0x80,0x01,0x88,0x60,0x02,0x08,0x38,
    0x04,0x08,0x1C,0x18,0x08,0x0C,0x20,0xF8,0x0C,0x00,0x18,0x00,0x00,0x00,0x00},
    //No:9	列   使用频度=1
    {	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x0C,0x08,0x3F,0xF0,0x08,0x01,0x00,0x08,0x03,
    0x01,0x88,0x02,0x00,0x88,0x02,0x18,0x88,0x07,0xF8,0x88,0x04,0x10,0x88,0x0C,0x30,0x88,0x0A,0x20,
    0x88,0x13,0x20,0x88,0x21,0x60,0x88,0x01,0x40,0x88,0x00,0xC0,0x88,0x00,0x81,0x88,0x01,0x01,0x08,
    0x02,0x00,0x08,0x04,0x00,0x08,0x18,0x00,0x78,0x20,0x00,0x18,0x00,0x00,0x00},
    //No:10	开   使用频度=1
    {	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x1F,0xFF,0xE0,0x00,0x81,0x80,0x00,
    0x81,0x80,0x00,0x81,0x80,0x00,0x81,0x80,0x00,0x81,0x80,0x00,0x81,0x84,0x00,0x81,0x8C,0x1F,0xFF,
    0xF0,0x00,0x81,0x80,0x00,0x81,0x80,0x00,0x81,0x80,0x01,0x81,0x80,0x01,0x01,0x80,0x01,0x01,0x80,
    0x02,0x01,0x80,0x04,0x01,0x80,0x08,0x01,0x80,0x30,0x01,0x80,0x00,0x00,0x00},
    //No:11	发   使用频度=1
    {	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x31,0x00,0x04,0x60,0xC0,0x0C,0x60,0x60,0x0C,0x60,0x60,0x08,
    0x40,0x00,0x18,0x40,0x0C,0x17,0xFF,0xF0,0x00,0xC0,0x00,0x00,0xC0,0x00,0x00,0xFF,0xE0,0x00,0xA0,
    0x60,0x01,0xA0,0x40,0x01,0x10,0x80,0x03,0x09,0x80,0x02,0x09,0x00,0x06,0x06,0x00,0x04,0x06,0x00,
    0x08,0x0D,0x80,0x10,0x30,0xE0,0x20,0x40,0x3E,0x43,0x80,0x08,0x00,0x00,0x00},
    //No:12	板   使用频度=1
    {	0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x30,0x04,0x00,0xF8,0x04,0x1F,0x00,0x04,0x10,0x00,0x04,
    0x90,0x00,0x3F,0x10,0x00,0x0C,0x10,0x10,0x0C,0x1F,0xF0,0x0E,0x14,0x30,0x0D,0x14,0x20,0x1C,0x94,
    0x20,0x14,0xB2,0x60,0x14,0x32,0x40,0x24,0x23,0xC0,0x44,0x21,0x80,0x44,0x21,0x80,0x04,0x23,0xC0,
    0x04,0x46,0x60,0x04,0x88,0x38,0x04,0xB0,0x1C,0x05,0xC0,0x00,0x00,0x00,0x00},
};*/


/**-------------------------------------------------------
  * @函数名 LCD_DrawChinaChar
  * @功能   在指定座标显示中文字符
  * @参数   StartX     行起始座标
  *         StartY     列起始座标
  *         cpFontArray 字库数组的地址指针
  * @返回值 无
  * 作者    www.armjishu.com
***------------------------------------------------------*/
void LCD_DrawChinaChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *cpFontArray)
{
    uint32_t index = 0, i = 0, j = 0;
    uint8_t Xaddress = 0;

    Xaddress = Xpos;

    LCD_SetCursor(Xaddress, Ypos);

    for(index = 0; index < 24; index++)
    {
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
        for(j = 0; j < 3; j++)
        {
            for(i = 0; i < 8; i++)
            {
                if((cpFontArray[3*index + j] & (0x80 >> i)) == 0x00)
                {
                    if(HyalineBack == HyalineBackDis)
                    {
                        LCD_WriteRAM(BackColor);
                    }
                    else
                    {
                        LCD_SetCursor(Xaddress, Ypos+(j*8)+i+1);
                        LCD_WriteRAM_Prepare();
                    }
                }
                else
                {
                    LCD_WriteRAM(TextColor);
                }
            }   
        }
        SetCs
        Xaddress++;
        LCD_SetCursor(Xaddress, Ypos);
    }
}

/**-------------------------------------------------------
  * @函数名 LCD_DisplayWelcomeStr
  * @功能   在指定座标显示"★欢迎您使用神舟系列开发板"字符
  * @参数   Line     行起始座标
  * @返回值 无
  * 作者    www.armjishu.com
***------------------------------------------------------*/
#if 0
void LCD_DisplayWelcomeStr(uint16_t Line)
{
    uint16_t num = 0;

    /* Send the string character by character on LCD */
    for(num=0; num<13; num++)
    {
        /* Display one China character on LCD */
        LCD_DrawChinaChar(Line, num*24+4, (uint8_t *)WelcomeStr[num]);
    }
}
#endif

/**
  * @brief  Sets the Text Font.
  * @param  fonts: specifies the font to be used.
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
    LCD_Currentfonts = fonts;
}

/**
  * @brief  Gets the Text Font.
  * @param  None.
  * @retval the used font.
  */
sFONT *LCD_GetFont(void)
{
    return LCD_Currentfonts;
}

/**
  * @brief  Sets the Text Font.
  * @param  fonts: specifies the font to be used.
  * @retval None
  */
void LCD_SetHyaline(uint8_t NewHyalineBack)
{
    HyalineBack = NewHyalineBack;
}

/**
  * @brief  Clears the selected line.
  * @param  Line: the Line to be cleared.
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..n
  * @retval None
  */
void LCD_ClearLine(uint16_t Line)
{
    uint16_t index = 0;

    LCD_SetCursor(Line,0); 
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    
    ClrCs
    SetRs
    LCD_Write(BackColor);
    for(index=0; index<(LCD_PIXEL_WIDTH * LCD_Currentfonts->Height); index++)
    {
        ClrWr
        //LCD_Write(Color);
        SetWr
    }
    SetCs
}


/**
  * @brief  Draws a character on LCD.
  * @param  Xpos: the Line where to display the character shape.
  * @param  Ypos: start column address.
  * @param  c: pointer to the character data.
  * @retval None
  */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *cpFontArray)
{
    uint32_t index = 0, i = 0, j = 0, k = 0, y;
    uint8_t Xaddress = 0;

    Xaddress = Xpos;

    LCD_SetCursor(Xaddress, Ypos);

    for(y = 0; y < LCD_Currentfonts->Height; y++)
    {
        LCD_WriteRAM_Prepare(); /* Prepare to write GRAM *///putchar('\r');putchar('\n');
        for(i = LCD_Currentfonts->Width; i > 0;)
        {
            if(i>=8)
            {
                k = 8;
            }
            else
            {
                k = i;
            }

            for(j = 0; j < k; j++)
            {
                if((cpFontArray[index] & (0x80 >> j)) == 0x00)
                {
                    if(HyalineBack == HyalineBackDis)
                    {
                        LCD_WriteRAM(BackColor); //putchar(' ');
                    }
                    else
                    {
                        LCD_SetCursor(Xaddress, Ypos+(LCD_Currentfonts->Width - i)+1);
                        LCD_WriteRAM_Prepare();
                    }
                }
                else
                {
                    LCD_WriteRAM(TextColor); //putchar('*');
                }
                i--;
                //Delay(2);//此处加延迟可以清楚的看到组成字符的一个个的点绘制过程
            }
            index++;
        }   
        SetCs
        Xaddress++;
        LCD_SetCursor(Xaddress, Ypos);
    }
    //putchar('\r');putchar('\n');
}

/**
  * @brief  Displays one character (16dots width, 24dots height).
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  Column: start column address.
  * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
  * @retval None
  */
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii)
{
    Ascii -= 32;
    LCD_DrawChar(Line, Column, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->BytesPreChar]);
}


/**
  * @brief  Displays a maximum of 20 char on the LCD.
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  *ptr: pointer to string to display on LCD.
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{
  uint16_t refcolumn = 0;

    /* Send the string character by character on lCD */
    //while ((*ptr != 0) & (((refcolumn + 1) & 0xFFFF) <= LCD_Currentfonts->Width))
    while ((*ptr != 0))
    {
        /* Display one character on LCD */
        LCD_DisplayChar(Line, refcolumn, *ptr);
        /* Decrement the column position by 16 */
        refcolumn += LCD_Currentfonts->Width;
        if(refcolumn + LCD_Currentfonts->Width > LCD_PIXEL_WIDTH)
        {
            break;
        }
        /* Point on the next character */
        ptr++;
    }
}

void LCD_DisplayString(uint16_t Xpos, uint16_t Ypos, uint8_t *ptr)
{
    uint16_t refcolumn = Ypos;

    /* Send the string character by character on lCD */
    //while ((*ptr != 0) & (((refcolumn + 1) & 0xFFFF) <= LCD_Currentfonts->Width))
    while ((*ptr != 0))
    {
        /* Display one character on LCD */
        LCD_DisplayChar(Xpos, refcolumn, *ptr);
        /* Decrement the column position by 16 */
        refcolumn += LCD_Currentfonts->Width;
        if(refcolumn + LCD_Currentfonts->Width > LCD_PIXEL_WIDTH)
        {
            break;
        }
        /* Point on the next character */
        ptr++;
    }
}

/**-------------------------------------------------------
  * @函数名 LCD_DisplayCurrentFont
  * @功能   将系统当前使用的ASCII字库的所有字符展示一次
  * @参数   无
  * @返回值 无
  * 作者    www.armjishu.com
***------------------------------------------------------*/
void LCD_DisplayCurrentFont(void)
{
    uint16_t cChar = 0, line = 0, refcolumn = 0;

    uint8_t sStr[20] = {0};

    sprintf((char *)sStr, "SIZE:%dX%d", LCD_Currentfonts->Width, LCD_Currentfonts->Height);

    LCD_Clear(LCD_COLOR_WHITE);
    LCD_DisplayStringLine(0, sStr);
    line = LCD_Currentfonts->Height + 1;
    for(cChar = 0x20; cChar<=0x7F; cChar++)
    {
        LCD_DisplayChar(line, refcolumn, cChar);

        refcolumn += LCD_Currentfonts->Width;
        refcolumn++;

        if(refcolumn + LCD_Currentfonts->Width > LCD_PIXEL_WIDTH)
        {
            line += LCD_Currentfonts->Height;
            line++;
            refcolumn = 0;
            if((line+LCD_Currentfonts->Height) > LCD_PIXEL_HEIGHT)
            {
                line = LCD_Currentfonts->Height + 1;
                if(cChar<=0x7F)
                {
                    Delay(36000000);
                    LCD_Clear(LCD_COLOR_WHITE);
                    LCD_DisplayStringLine(0, sStr);
                }
            }
        }
    }
}

/**-------------------------------------------------------
  * @函数名 LCD_DisplayAllFont
  * @功能   通过修改系统当前使用的ASCII字库类型，
  *         依次展示所有支持的字库
  * @参数   无
  * @返回值 无
  * 作者    www.armjishu.com
***------------------------------------------------------*/
/*void LCD_DisplayAllFont(void)
{
    uint32_t i;
    sFONT *LCD_OldCurrentfonts;

    LCD_OldCurrentfonts = LCD_GetFont();

    for(i= 0; pFontTable[i] != 0; i++)
    {
        LCD_SetFont(pFontTable[i]);
        LCD_DisplayCurrentFont();
        Delay(36000000);
    }

    LCD_SetFont(LCD_OldCurrentfonts);
}*/
//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//设置光标位置 
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	//myLCD->LCD_RAM=POINT_COLOR; 
	LCD_Write(POINT_COLOR);
}
//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u16 colortemp=POINT_COLOR;      			     
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	if(!mode) //非叠加方式
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //调用1206字体
			else temp=asc2_1608[num][t];		 //调用1608字体 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)POINT_COLOR=colortemp;
				else POINT_COLOR=BACK_COLOR;
				LCD_DrawPoint(x,y);	
				temp<<=1;
				y++;
				if(x>=320){POINT_COLOR=colortemp;return;}//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=320){POINT_COLOR=colortemp;return;}//超区域了
					break;
				}
			}  	 
	    }    
	}else//叠加方式
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //调用1206字体
			else temp=asc2_1608[num][t];		 //调用1608字体 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)LCD_DrawPoint(x,y); 
				temp<<=1;
				y++;
				if(x>=240){POINT_COLOR=colortemp;return;}//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=320){POINT_COLOR=colortemp;return;}//超区域了
					break;
				}
			}  	 
	    }     
	}
	POINT_COLOR=colortemp;	    	   	 	  
}   
//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=320){x=x0;y+=size;}
        if(y>=240)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}
#endif 
/*******************  (C) COPYRIGHT 2013 www.armjishu.com  *****END OF FILE****/

