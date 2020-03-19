#ifndef _MKS_TOUCH_SCREEN_H_
#define _MKS_TOUCH_SCREEN_H_

#include "stm32f10x.h"
#include "stm32f10x_spi.h"


#define  PEN_NONE  	0
#define  PEN_DOWN  	1
#define  PEN_UP   	2



/* SPI_1 pins definition for test */
#define SPI1_PORT   GPIOC
#define SPI1_SCK    GPIO_Pin_10
#define SPI1_MISO   GPIO_Pin_11  ////////lihao  SPI3
#define SPI1_MOSI   GPIO_Pin_12
#define RCC_SPI1    RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO


/* ARMJISHU_TouchScreen_ADS7843 */

#define RCC_ADS7843_CS                         RCC_APB2Periph_GPIOC
#define GPIO_ADS7843_CS_PORT                   GPIOC 
#define GPIO_ADS7843_CS                        GPIO_Pin_9//GPIO_Pin_1  for 3db board, 12 for shenzhou iii board

#define RCC_ADS7843_INT                        RCC_APB2Periph_GPIOC
#define GPIO_ADS7843_INT_PORT                  GPIOC  
#define GPIO_ADS7843_INT                       GPIO_Pin_5  
#define GPIO_ADS7843_EXTI_LINE                 EXTI_Line5
#define GPIO_ADS7843_EXTI_PORT_SOURCE          GPIO_PortSourceGPIOC
#define GPIO_ADS7843_EXTI_PIN_SOURCE           GPIO_PinSource5
#define GPIO_ADS7843_EXTI_IRQn                 EXTI9_5_IRQn 

#define GPIO_ADS7843_INT_VALID   !GPIO_ReadInputDataBit(GPIO_ADS7843_INT_PORT, GPIO_ADS7843_INT)

/* Select ADS7843: ChipSelect pin low  */
#define ADS7843_CS_LOW()     GPIO_ResetBits(GPIO_ADS7843_CS_PORT, GPIO_ADS7843_CS)
/* Deselect ADS7843: ChipSelect pin high */
#define ADS7843_CS_HIGH()    GPIO_SetBits(GPIO_ADS7843_CS_PORT, GPIO_ADS7843_CS)

/* pin low  */
#define SPI1_SCK_LOW()     GPIO_ResetBits(SPI1_PORT, SPI1_SCK)
/* pin high */
#define SPI1_SCK_HIGH()    GPIO_SetBits(SPI1_PORT, SPI1_SCK)

/* pin read  */
#define SPI1_MISO_READ()     GPIO_ReadInputDataBit(SPI1_PORT, SPI1_MISO)

/* pin low  */
#define SPI1_MOSI_LOW()     GPIO_ResetBits(SPI1_PORT, SPI1_MOSI)
/* pin high */
#define SPI1_MOSI_HIGH()    GPIO_SetBits(SPI1_PORT, SPI1_MOSI)


typedef struct
{
	uint16_t  x;
	uint16_t  y;
	int8_t  state;
	int32_t time;
} TOUCH_EVENT;

TOUCH_EVENT *getTouchEvent();


void ADS7843_Init(void);
static void ADS7843_INT_EXIT_Init(void);
static void ADS7843_INT_config(void);
static void ADS7843_InterruptConfig(void);
void ADS7843_CS_config(void);
void ADS7843_Init(void);
void SPI1_Config(void);
void SPI1_Init_For_Byte(void);
void SPI1_SendByte(u8 byte);
u8 SPI1_RecvByte(void);
void SPI1_SendWord(u16 word);
u16 SPI1_RecvWord(void);

void ARMJISHU_TouchScreen_ADS7843(void);




#endif

