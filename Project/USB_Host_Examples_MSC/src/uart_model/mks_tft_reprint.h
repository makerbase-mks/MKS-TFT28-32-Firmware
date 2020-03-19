#ifndef MKS_TFT_REPRINT_H
#define MKS_TFT_REPRINT_H
#ifdef __cplusplus
 extern "C" {
#endif


#define PUTDOWN_TIME	50

//#define OFFSETLENGTH	0x100000	 	//1M
//#define OFFSETLENGTH	0x19000	 	//100K	回溯长度
	 
#define COORDINATELENGTH	20
/*	 
#define  EEPROM_HIGH_ADDR			256
#define  BAK_REPRINT_INFO			EEPROM_HIGH_ADDR - 4			// 续打标志(uint8_t) | 喷头选择(高4位) 风扇开关状态(低四位)|风速8位| 热床温度8位
#define  BAK_REPRINT_OFFSET		BAK_REPRINT_INFO - 4			// offset(uint32_t)
#define  BAK_REPRINT_HTEMP		BAK_REPRINT_OFFSET - 4		// T0Temp(uint16_t)	| T1Temp(uint16_t)
//#define  BAK_REPRINT_FAN				BAK_REPRINT_HTEMP - 4		 // BedTemp(uint8_t)	| fanOnoff(uint8_t) |fanSpeed(uint8_t)| null(uint8_t)
#define  BAK_REPRINT_TIME			BAK_REPRINT_HTEMP - 4//BAK_REPRINT_FAN - 4			 //hours(int16_t) | minutes(int8_t) ;
#define  BAK_REPRINT_X					BAK_REPRINT_TIME - 4		 // X position
#define  BAK_REPRINT_Y					BAK_REPRINT_X - 4			 //Y position
#define  BAK_REPRINT_Z					BAK_REPRINT_Y - 4			 //Z position
#define  BAK_REPRINT_S					BAK_REPRINT_Z - 4			//null(uint24_t) | saveFlag(uint8_t)
*/
//tan 2017.02.04
//采用连续写e2prom的方式，
//重新定义了断电数据的存储位置
#define  EEPROM_HIGH_ADDR			256
#define  BAK_REPRINT_TIME			EEPROM_HIGH_ADDR - 4		//hours(int16_t) | minutes(int8_t) ;
#define  BAK_REPRINT_HTEMP		BAK_REPRINT_TIME - 4		// T0Temp(uint16_t)	| T1Temp(uint16_t)
#define  BAK_REPRINT_OFFSET 	BAK_REPRINT_HTEMP - 4		// offset(uint32_t)
#define  BAK_REPRINT_INFO		  BAK_REPRINT_OFFSET - 4	// 续打标志(uint8_t) | 喷头选择(高4位) 风扇开关状态(低四位)|风速8位| 热床温度8位
#define  BAK_REPRINT_X					BAK_REPRINT_INFO - 4		 // X position
#define  BAK_REPRINT_Y					BAK_REPRINT_X - 4			 //Y position
#define  BAK_REPRINT_Z					BAK_REPRINT_Y - 4			 //Z position
#define  BAK_REPRINT_S					BAK_REPRINT_Z - 4			//null(uint24_t) | saveFlag(uint8_t)
#define  BAK_REPRINT_E					BAK_REPRINT_S - 4
#define  BAK_TEST_FALA_FLAG_DATA_ADDR          BAK_REPRINT_E-4
#define  BAK_TEST_FALA_FLAG_ADDR          BAK_TEST_FALA_FLAG_DATA_ADDR-1




//#define  BAK_REPRINT_DIGITAL		BAK_REPRINT_HTEMP - 4	 		//gcode 代码的数字，最多8byte
	 
	 
	 


/** @defgroup GPIO_Pin_sources 
  * @{
  */

#define GPIO_PinSource0            ((uint8_t)0x00)
#define GPIO_PinSource1            ((uint8_t)0x01)
#define GPIO_PinSource2            ((uint8_t)0x02)
#define GPIO_PinSource3            ((uint8_t)0x03)
#define GPIO_PinSource4            ((uint8_t)0x04)
#define GPIO_PinSource5            ((uint8_t)0x05)
#define GPIO_PinSource6            ((uint8_t)0x06)
#define GPIO_PinSource7            ((uint8_t)0x07)
#define GPIO_PinSource8            ((uint8_t)0x08)
#define GPIO_PinSource9            ((uint8_t)0x09)
#define GPIO_PinSource10           ((uint8_t)0x0A)
#define GPIO_PinSource11           ((uint8_t)0x0B)
#define GPIO_PinSource12           ((uint8_t)0x0C)
#define GPIO_PinSource13           ((uint8_t)0x0D)
#define GPIO_PinSource14           ((uint8_t)0x0E)
#define GPIO_PinSource15           ((uint8_t)0x0F)

#define IS_GPIO_PIN_SOURCE(PINSOURCE) (((PINSOURCE) == GPIO_PinSource0) || \
                                       ((PINSOURCE) == GPIO_PinSource1) || \
                                       ((PINSOURCE) == GPIO_PinSource2) || \
                                       ((PINSOURCE) == GPIO_PinSource3) || \
                                       ((PINSOURCE) == GPIO_PinSource4) || \
                                       ((PINSOURCE) == GPIO_PinSource5) || \
                                       ((PINSOURCE) == GPIO_PinSource6) || \
                                       ((PINSOURCE) == GPIO_PinSource7) || \
                                       ((PINSOURCE) == GPIO_PinSource8) || \
                                       ((PINSOURCE) == GPIO_PinSource9) || \
                                       ((PINSOURCE) == GPIO_PinSource10) || \
                                       ((PINSOURCE) == GPIO_PinSource11) || \
                                       ((PINSOURCE) == GPIO_PinSource12) || \
                                       ((PINSOURCE) == GPIO_PinSource13) || \
                                       ((PINSOURCE) == GPIO_PinSource14) || \
                                       ((PINSOURCE) == GPIO_PinSource15))

/**
  * @}
  */
	 
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define	BACKLIGHT  PDout(14)	 
#define SPEAKER			PAout(2)	 	 
#define FALACTL			PBout(12)	
#define FALA_ON			1
#define FALA_OFF		0
#define BACKLIGHT_ON		1
#define BACKLIGHT_OFF		0
#define SPEAKER_ON	1
#define SPEAKER_OFF	0
#define MKSLED1 PDout(2)
#define MKSLED2 PDout(3)
#define MKSLED3 PDout(4)
#define MKSLED4 PDout(7)

#define MKS_PWRDN PBin(0)
#define MKS_MTRDN PBin(1)


#define SAVE_ENA	0xa5
#define SAVE_DIS	0

typedef enum 
{
	printer_pause_reprint = 0xa7,		//暂停续打
	printer_pwdwn_reprint,					//断电续打
	printer_normal,									//无续打
} RE_PRINT_STATUS;



typedef struct			
{
	RE_PRINT_STATUS printerStatus;  
	unsigned long offset1;					// if udiskBuffer.state[(udiskBuffer.current+1)%2] = udisk_buf_empty; offset - UDISKBUFLEN; else  offset - 2*UDISKBUFLEN
	unsigned long offset;
	unsigned long break_offset;
	//unsigned char checkXor;	 
	//unsigned int checkSum;
	//unsigned char digxor;
	unsigned char bedTemp;
	unsigned int t0Temp;
	unsigned int t1Temp;
	int8_t fanOnoff; 
	uint8_t fanSpeed;
	unsigned char x_coordinate[COORDINATELENGTH];
	unsigned char y_coordinate[COORDINATELENGTH];
	unsigned char z_coordinate[COORDINATELENGTH];
	unsigned char e_coordinate[COORDINATELENGTH];
	unsigned char F[COORDINATELENGTH];
//tan 20170109
	unsigned char mov_x_coordinate[COORDINATELENGTH];
	unsigned char mov_y_coordinate[COORDINATELENGTH];
	unsigned char mov_z_coordinate[COORDINATELENGTH];
	unsigned char mov_e_coordinate[COORDINATELENGTH];
	unsigned char mov_F[COORDINATELENGTH];

	unsigned char unit;		//0 mm,1 inch
	volatile unsigned char saveFlag_temp;
	volatile unsigned char saveFlag;	//0 none,0xa5 save
	unsigned long saveX;
	unsigned long saveY;
	unsigned long saveZ;
	volatile unsigned char saveEnable;
	uint16_t hours;
	uint8_t minutes;
	uint8_t spayerchoose;
	uint32_t record_line;//已打印行数
}RE_PRINT_STRUCT;

extern  volatile uint32_t dataToEeprom;
//extern uint8_t dataToEeprom8;
extern void rePrintInit();
extern void rePrintSaveData();
extern void prSaveProcess();
extern void rePrintdata_show();

extern RE_PRINT_STRUCT RePrintData;
extern void Get_Z_Pos_display_value();
extern void getZPosition();
extern void BreakPointReGcode();
extern void Test_fala();

//extern void EXTI0_IRQHandler(void);
#ifdef __cplusplus
}
#endif
	 
#endif
