/********************   (C) COPYRIGHT 2013 www.armjishu.com   ********************
 * 文件名  ：SZ_STM32F107VC_LIB.c
 * 描述    ：提供STM32F107VC神舟IV号开发板的库函数
 * 实验平台：STM32神舟开发板
 * 作者    ：www.armjishu.com 
**********************************************************************************/
  
/* Includes ------------------------------------------------------------------*/
#include "SZ_STM32F107VC_LIB.h"
#include "printer.h"
#include "wifi_module.h"

__IO uint32_t TimingDelay;

extern CFG_ITMES gCfgItems;


GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT, LED4_GPIO_PORT};
const uint16_t GPIO_PIN_NUM[LEDn] = {LED1_PIN_NUM, LED2_PIN_NUM, LED3_PIN_NUM, LED4_PIN_NUM};
const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};
const uint32_t GPIO_CLK[LEDn] = {LED1_GPIO_CLK, LED2_GPIO_CLK, LED3_GPIO_CLK, LED4_GPIO_CLK};

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {KEY1_BUTTON_GPIO_PORT, KEY2_BUTTON_GPIO_PORT, KEY3_BUTTON_GPIO_PORT, KEY4_BUTTON_GPIO_PORT}; 
const uint16_t BUTTON_PIN_NUM[BUTTONn] = {KEY1_BUTTON_PIN_NUM, KEY2_BUTTON_PIN_NUM, KEY3_BUTTON_PIN_NUM, KEY4_BUTTON_PIN_NUM}; 
const uint16_t BUTTON_PIN[BUTTONn] = {KEY1_BUTTON_PIN, KEY2_BUTTON_PIN, KEY3_BUTTON_PIN, KEY4_BUTTON_PIN}; 
const uint32_t BUTTON_CLK[BUTTONn] = {KEY1_BUTTON_GPIO_CLK, KEY2_BUTTON_GPIO_CLK, KEY3_BUTTON_GPIO_CLK, KEY4_BUTTON_GPIO_CLK};
const uint16_t BUTTON_EXTI_LINE[BUTTONn] = {KEY1_BUTTON_EXTI_LINE, KEY2_BUTTON_EXTI_LINE, KEY3_BUTTON_EXTI_LINE, KEY4_BUTTON_EXTI_LINE};
const uint16_t BUTTON_PORT_SOURCE[BUTTONn] = {KEY1_BUTTON_EXTI_PORT_SOURCE, KEY2_BUTTON_EXTI_PORT_SOURCE, KEY3_BUTTON_EXTI_PORT_SOURCE, KEY4_BUTTON_EXTI_PORT_SOURCE};
const uint16_t BUTTON_PIN_SOURCE[BUTTONn] = {KEY1_BUTTON_EXTI_PIN_SOURCE, KEY2_BUTTON_EXTI_PIN_SOURCE, KEY3_BUTTON_EXTI_PIN_SOURCE, KEY4_BUTTON_EXTI_PIN_SOURCE}; 
const uint16_t BUTTON_IRQn[BUTTONn] = {KEY1_BUTTON_EXTI_IRQn, KEY2_BUTTON_EXTI_IRQn, KEY3_BUTTON_EXTI_IRQn, KEY4_BUTTON_EXTI_IRQn};

USART_TypeDef* COM_USART[COMn] = {SZ_STM32_COM1, SZ_STM32_COM2, SZ_STM32_COM3}; 
GPIO_TypeDef* COM_TX_PORT[COMn] = {SZ_STM32_COM1_TX_GPIO_PORT, SZ_STM32_COM2_TX_GPIO_PORT, SZ_STM32_COM3_TX_GPIO_PORT};
GPIO_TypeDef* COM_RX_PORT[COMn] = {SZ_STM32_COM1_RX_GPIO_PORT, SZ_STM32_COM2_RX_GPIO_PORT, SZ_STM32_COM3_RX_GPIO_PORT};
const uint32_t COM_USART_CLK[COMn] = {SZ_STM32_COM1_CLK, SZ_STM32_COM2_CLK, SZ_STM32_COM3_CLK};
const uint32_t COM_TX_PORT_CLK[COMn] = {SZ_STM32_COM1_TX_GPIO_CLK, SZ_STM32_COM2_TX_GPIO_CLK, SZ_STM32_COM3_TX_GPIO_CLK};
const uint32_t COM_RX_PORT_CLK[COMn] = {SZ_STM32_COM1_RX_GPIO_CLK, SZ_STM32_COM2_RX_GPIO_CLK, SZ_STM32_COM3_RX_GPIO_CLK};
const uint16_t COM_TX_PIN[COMn] = {SZ_STM32_COM1_TX_PIN, SZ_STM32_COM2_TX_PIN, SZ_STM32_COM3_TX_PIN};
const uint16_t COM_RX_PIN[COMn] = {SZ_STM32_COM1_RX_PIN, SZ_STM32_COM2_RX_PIN, SZ_STM32_COM3_RX_PIN};


#if 0
const uint8_t STM32F10x_STR[] = {"\r\n"\
           "  _____ _______ __  __ ____ ___  ______ __  ___\r\n"\
           " / ____|__   __|  \\/  |___ \\__ \\|  ____/_ |/ _ \\\r\n"\
           "| (___    | |  | \\  / | __) | ) | |__   | | | | |_  __\r\n"\
           " \\___ \\   | |  | |\\/| ||__ < / /|  __|  | | | | \\ \\/ /\r\n"\
           " ____) |  | |  | |  | |___) / /_| |     | | |_| |>  <\r\n"\
           "|_____/   |_|  |_|  |_|____/____|_|     |_|\\___//_/\\_\\神舟系列开发板\r\n"\
           "\r\n"};

#endif					 
uint32_t STM32DeviceSerialID[3]; /* 全局变量IntDeviceSerial存放读到的设备ID */

/**-------------------------------------------------------
  * @函数名 delay
  * @功能   简单的delay延时函数.
  * @参数   延迟周期数 0--0xFFFFFFFF
  * @返回值 无
***------------------------------------------------------*/
void delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--);
}

/**-------------------------------------------------------
  * @函数名 NVIC_GroupConfig
  * @功能   配置NVIC中断优先级分组函数.
  *         默认配置为1比特表示主优先级, 3比特表示次优先级
  *         用户可以根据需要修改
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void NVIC_GroupConfig(void)
{
    /* 配置NVIC中断优先级分组:
     - 1比特表示主优先级  主优先级合法取值为 0 或 1 
     - 3比特表示次优先级  次优先级合法取值为 0..7
     - 数值越低优先级越高，取值超过合法范围时取低bit位 
    */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /*==================================================================================
      NVIC_PriorityGroup   |  主优先级范围  |  次优先级范围  |   描述
      ==================================================================================
     NVIC_PriorityGroup_0  |      0         |      0-15      |   0 比特表示主优先级
                           |                |                |   4 比特表示次优先级 
     ----------------------------------------------------------------------------------
     NVIC_PriorityGroup_1  |      0-1       |      0-7       |   1 比特表示主优先级
                           |                |                |   3 比特表示次优先级 
     ----------------------------------------------------------------------------------
     NVIC_PriorityGroup_2  |      0-3       |      0-3       |   2 比特表示主优先级
                           |                |                |   2 比特表示次优先级 
     ----------------------------------------------------------------------------------
     NVIC_PriorityGroup_3  |      0-7       |      0-1       |   3 比特表示主优先级
                           |                |                |   1 比特表示次优先级 
     ----------------------------------------------------------------------------------
     NVIC_PriorityGroup_4  |      0-15      |      0         |   4 比特表示主优先级
                           |                |                |   0 比特表示次优先级   
    ==================================================================================*/
}

/**-------------------------------------------------------
  * @函数名 SZ_STM32_SysTickInit
  * @功能   初始化系统定时器SysTick
  *         用户可以根据需要修改
  * @参数   每秒中断次数
  * @返回值 无
***------------------------------------------------------*/
void SZ_STM32_SysTickInit(uint32_t HzPreSecond)
{
    /* HzPreSecond = 1000 to Setup SysTick Timer for 1 msec interrupts.
     ------------------------------------------
    1. The SysTick_Config() function is a CMSIS function which configure:
       - The SysTick Reload register with value passed as function parameter.
       - Configure the SysTick IRQ priority to the lowest value (0x0F).
       - Reset the SysTick Counter register.
       - Configure the SysTick Counter clock source to be Core Clock Source (HCLK).
       - Enable the SysTick Interrupt.
       - Start the SysTick Counter.
    
    2. You can change the SysTick Clock source to be HCLK_Div8 by calling the
       SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8) just after the
       SysTick_Config() function call. The SysTick_CLKSourceConfig() is defined
       inside the misc.c file.

    3. You can change the SysTick IRQ priority by calling the
       NVIC_SetPriority(SysTick_IRQn,...) just after the SysTick_Config() function 
       call. The NVIC_SetPriority() is defined inside the core_cm3.h file.

    4. To adjust the SysTick time base, use the following formula:
                            
         Reload Value = SysTick Counter Clock (Hz) x  Desired Time base (s)
       - Reload Value is the parameter to be passed for SysTick_Config() function
       - Reload Value should not exceed 0xFFFFFF
    */
    if (SysTick_Config(SystemCoreClock / HzPreSecond))
    { 
        /* Capture error */ 
        while (1);
    }
}

/**-------------------------------------------------------
  * @函数名 SysTickDelay
  * @功能   系统定时器实现的延时函数，可以在用户程序中调用
  *         用户可以根据需要修改
  * @参数   延迟周期数，延迟时间为nTime/HzPreSecond
  *         误差为 -1/HzPreSecond
  * @返回值 无
***------------------------------------------------------*/
void SysTickDelay(__IO uint32_t nTime)
{ 
    TimingDelay = nTime;

    while(TimingDelay != 0);
}

/**-------------------------------------------------------
  * @函数名 TimingDelay_Decrement
  * @功能   系统节拍定时器服务函数调用的子函数
  *         将全局变量TimingDelay减一，用于实现延时
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    { 
        TimingDelay--;
    }
}

extern void SysTick_Handler_User(void);
/**-------------------------------------------------------
  * @函数名 SysTick_Handler
  * @功能   系统节拍定时器服务请求处理函数
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void SysTick_Handler(void)
{
    TimingDelay_Decrement();

    SysTick_Handler_User();
}
#if 0

void GetDeviceSerialID(void)
{
    STM32DeviceSerialID[0] = *(__IO uint32_t*)(0x1FFFF7E8);
    STM32DeviceSerialID[1] = *(__IO uint32_t*)(0x1FFFF7EC);
    STM32DeviceSerialID[2] = *(__IO uint32_t*)(0x1FFFF7F0);
}

/**-------------------------------------------------------
  * @函数名 SZ_STM32_LEDInit
  * @功能   初始化LED的GPIO管脚，配置为推挽输出
  * @参数   LED1  对应开发板上第一个指示灯
  *         LED2  对应开发板上第二个指示灯
  *         LED3  对应开发板上第三个指示灯
  *         LED4  对应开发板上第四个指示灯
  * @返回值 无
***------------------------------------------------------*/
void SZ_STM32_LEDInit(Led_TypeDef Led)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* Enable the GPIO_LED Clock */
    /* 使能LED对应GPIO的Clock时钟 */
    RCC_APB2PeriphClockCmd(GPIO_CLK[Led], ENABLE);

    /* Configure the GPIO_LED pin */
    /* 初始化LED的GPIO管脚，配置为推挽输出 */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);
}

/**-------------------------------------------------------
  * @函数名 SZ_STM32_LEDOn
  * @功能   点亮对应的LED指示灯
  * @参数   LED1  对应开发板上第一个指示灯
  *         LED2  对应开发板上第二个指示灯
  *         LED3  对应开发板上第三个指示灯
  *         LED4  对应开发板上第四个指示灯
  * @返回值 无
***------------------------------------------------------*/
void SZ_STM32_LEDOn(Led_TypeDef Led)
{
    /* 指定管脚输出低电平，点亮对应的LED指示灯 */
    GPIO_PORT[Led]->BRR = GPIO_PIN[Led];  
    //若要提高效率，建议直接调用 LEDnOBB = 0;
}

/**-------------------------------------------------------
  * @函数名 SZ_STM32_LEDOn
  * @功能   熄灭对应的LED指示灯
  * @参数   LED1  对应开发板上第一个指示灯
  *         LED2  对应开发板上第二个指示灯
  *         LED3  对应开发板上第三个指示灯
  *         LED4  对应开发板上第四个指示灯
  * @返回值 无
***------------------------------------------------------*/
void SZ_STM32_LEDOff(Led_TypeDef Led)
{
    /* 指定管脚输出高电平，熄灭对应的LED指示灯 */
    GPIO_PORT[Led]->BSRR = GPIO_PIN[Led];   
    //若要提高效率，建议直接调用 LEDnOBB = 1;
}

/**-------------------------------------------------------
  * @函数名 SZ_STM32_LEDToggle
  * @功能   将对应的LED指示灯状态取反
  * @参数   LED1  对应开发板上第一个指示灯
  *         LED2  对应开发板上第二个指示灯
  *         LED3  对应开发板上第三个指示灯
  *         LED4  对应开发板上第四个指示灯
  * @返回值 无
***------------------------------------------------------*/
void SZ_STM32_LEDToggle(Led_TypeDef Led)
{
    /* 指定管脚输出异或 1，实现对应的LED指示灯状态取反目的 */
    GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
	//若要提高效率，建议直接调用 LEDnOBB = !LEDnOBB;
}


/**-------------------------------------------------------
  * @函数名 SZ_STM32_BEEPInit
  * @功能   初始化蜂鸣器的GPIO管脚，配置为推挽输出
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void SZ_STM32_BEEPInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* Enable the GPIO_LED Clock */
    /* 使能蜂鸣器对应GPIO的Clock时钟 */
    RCC_APB2PeriphClockCmd(BEEP_GPIO_CLK, ENABLE);

    /* Configure the GPIO_LED pin */
    /* 初始化蜂鸣器的GPIO管脚，配置为推挽输出 */
    GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);

    BEEPOBB = 1;//初始化时关闭蜂鸣器
}
#endif

void SZ_STM32_COM_Init(int rateId)
{
/*
	SZ_USART1_DATA.uart = SZ_STM32_COM1;
	SZ_USART1_DATA.uart_rx_done = 0;
	SZ_USART1_DATA.uart_rx_length= 0;
	SZ_USART1_DATA.uart_rx_read_point = 0;
	SZ_USART1_DATA.uart_rx_write_point = 0;
	SZ_USART1_DATA.uart_rx_write_length = 0;
*/	
	SZ_USART2_DATA.uart = SZ_STM32_COM2;
	SZ_USART2_DATA.uart_rx_length= 0;
	SZ_USART2_DATA.uart_rx_read_point = 0;
	SZ_USART2_DATA.uart_rx_write_point = 0;
	SZ_USART2_DATA.uart_rx_write_length = 0;
	
	/* 注意串口2使用Printf时"SZ_STM32F107VC_LIB.c"文件中fputc定义中设备改为SZ_STM32_COM2 */
    /* 串口2初始化 */
	if(rateId == 1)
    		SZ_STM32_COMInit(COM2, 9600);
	else if(rateId == 2)
		SZ_STM32_COMInit(COM2, 57600);
	else if(rateId == 3)
		SZ_STM32_COMInit(COM2, 115200);
	else
		SZ_STM32_COMInit(COM2, 250000);
        
    /* 串口1初始化 注意JP3和JP5跳到2<->3侧 */
    //xPrintf_Init();

    //NVIC_GroupConfig();
    NVIC_COMConfiguration();
}

#if 0
/**-------------------------------------------------------
  * @函数名 SZ_STM32_BEEPOn
  * @功能   使蜂鸣器开始鸣响
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void SZ_STM32_BEEPOn(void)
{
    /* 指定管脚输出低电平，使蜂鸣器开始鸣响 */
    //BEEP_GPIO_PORT->BRR = BEEP_PIN;  
    BEEPOBB = 0;
}

/**-------------------------------------------------------
  * @函数名 SZ_STM32_BEEPOff
  * @功能   使蜂鸣器停止鸣响
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void SZ_STM32_BEEPOff(void)
{
    /* 指定管脚输出高电平，使蜂鸣器停止鸣响 */
    //BEEP_GPIO_PORT->BSRR = BEEP_PIN;   
    BEEPOBB = 1;
}

/**-------------------------------------------------------
  * @函数名 SZ_STM32_BEEPToggle
  * @功能   使蜂鸣器状态取反
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void SZ_STM32_BEEPToggle(void)
{
    /* 指定管脚输出异或 1，实现对蜂鸣器状态取反目的 */
    //BEEP_GPIO_PORT->ODR ^= BEEP_PIN;
    BEEPOBB = !BEEPOBB;
}


/**-------------------------------------------------------
  * @函数名 SZ_STM32_KEYInit
  * @功能   初始化KEY按键的GPIO管脚，配置为带上拉的输入
  *         按键按下时为低电平0
  * @参数1  KEY1  对应开发板上第一个KEY按键
  *         KEY2  对应开发板上第二个KEY按键
  *         KEY3  对应开发板上第三个KEY按键
  *         KEY4  对应开发板上第四个KEY按键
  * @参数2  BUTTON_MODE_GPIO  配置为普通输入管脚使用
  *         BUTTON_MODE_EXTI  配置为外部EXTI中断使用
  *                           需要完善中断处理函数
  * @返回值 无
***------------------------------------------------------*/
void SZ_STM32_KEYInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the BUTTON Clock */
    /* 使能KEY按键对应GPIO的Clock时钟 */
    RCC_APB2PeriphClockCmd(BUTTON_CLK[Button] | RCC_APB2Periph_AFIO, ENABLE);

    /* Configure Button pin as input floating */
    /* 初始化KEY按键的GPIO管脚，配置为带上拉的输入 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = BUTTON_PIN[Button];
    GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStructure);

    /* 初始化KEY按键为中断模式 */
    if (Button_Mode == BUTTON_MODE_EXTI)
    {
        /* Connect Button EXTI Line to Button GPIO Pin */
        /* 将KEY按键对应的管脚连接到内部中断线 */    
        GPIO_EXTILineConfig(BUTTON_PORT_SOURCE[Button], BUTTON_PIN_SOURCE[Button]);

        /* Configure Button EXTI line */
        /* 将KEY按键配置为中断模式，下降沿触发中断 */    
        EXTI_InitStructure.EXTI_Line = BUTTON_EXTI_LINE[Button];
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);

        /* Enable and set Button EXTI Interrupt to the lowest priority */
        /* 将KEY按键的中断优先级配置为最低 */  
        NVIC_InitStructure.NVIC_IRQChannel = BUTTON_IRQn[Button];
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure); 
    }
}

/**-------------------------------------------------------
  * @函数名 SZ_STM32_KEYGetState
  * @功能   获取KEY按键的输入电平状态，按键按下时为低电平0
  * @参数   KEY1  对应开发板上第一个KEY按键
  *         KEY2  对应开发板上第二个KEY按键
  *         KEY3  对应开发板上第三个KEY按键
  *         KEY4  对应开发板上第四个KEY按键
  * @返回值 无
***------------------------------------------------------*/
uint32_t SZ_STM32_KEYGetState(Button_TypeDef Button)
{
    return GPIO_ReadInputDataBit(BUTTON_PORT[Button], BUTTON_PIN[Button]);
	//若要提高效率，建议直接调用 KeyState = KEYnIBB;
}

/**-------------------------------------------------------
  * @函数名 SZ_STM32_KEYScan
  * @功能   获取KEY按键的输入电平状态，按键按下时为低电平0
  * @参数   无
  * @返回值 1  对应开发板上第一个KEY按键处于按下状态
  *         2  对应开发板上第二个KEY按键处于按下状态
  *         3  对应开发板上第三个KEY按键处于按下状态
  *         4  对应开发板上第四个KEY按键处于按下状态
  *         0  开发板上没有KEY按键处于按下状态
***------------------------------------------------------*/
uint32_t SZ_STM32_KEYScan(void)
{
    /* 获取KEY按键的输入电平状态，按键按下时为低电平0 */
    if(0 == KEY1IBB)
    {
         /* 延迟去抖 */
        delay(150000);
        if(0 == KEY1IBB)
        {
            return 1;
        }
    }

    /* 获取KEY按键的输入电平状态，按键按下时为低电平0 */
    if(0 == KEY2IBB)
    {
         /* 延迟去抖 */
        delay(150000);
        if(0 == KEY2IBB)
        {
            return 2;
        }
    }

    /* 获取KEY按键的输入电平状态，按键按下时为低电平0 */
    if(0 == KEY3IBB)
    {
         /* 延迟去抖 */
        delay(150000);
        if(0 == KEY3IBB)
        {
            return 3;
        }
    }

    /* 获取KEY按键的输入电平状态，按键按下时为低电平0 */
    if(0 == KEY4IBB)
    {
         /* 延迟去抖 */
        delay(150000);
        if(0 == KEY4IBB)
        {
            return 4;
        }
    }
    
    return 0;
}
#endif

/**-------------------------------------------------------
  * @函数名 __SZ_STM32_COMInit
  * @功能   对STM32的USART初始化底层函数
  * @参数1  COM1  对应STM32的USART1 对应开发板上串口1
  *         COM2  对应STM32的USART2 对应开发板上串口2
  * @参数2  指向一个成员已赋值USART_InitTypeDef结构体的指针
  * @返回值 无
***------------------------------------------------------*/
void __SZ_STM32_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO clock */
    /* 使能STM32的USART对应GPIO的Clock时钟 */
    RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM] | RCC_APB2Periph_AFIO, ENABLE);

    if (COM == COM1)
    {
        /* 使能STM32的USART1的Clock时钟 */
        RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE); 
    }
    else if (COM == COM2)
    {
        /* Enable the USART2 Pins Software Remapping */
        /* 使能STM32的USART2的管脚重映射 */
        GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
        /* 使能STM32的USART的Clock时钟 */
        RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
    }
    else if(COM == COM3)
    {
    	
	/* Enable the USART3 Pins Software Remapping */
        /* 使能STM32的USART2的管脚重映射 */
        GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
        /* 使能STM32的USART的Clock时钟 */
        RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
    }

    /* Configure USART Tx as alternate function push-pull */
    /* 初始化STM32的USART的TX管脚，配置为复用功能推挽输出 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

    /* Configure USART Rx as input floating */
    /* 初始化STM32的USART的RX管脚，配置为复用功能输入 */
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
    GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

    /* USART configuration */
    /* 根据传入的参数初始化STM32的USART配置 */
    USART_Init(COM_USART[COM], USART_InitStruct);

    /* Enable USART */
    /* 使能STM32的USART功能模块 */
    USART_Cmd(COM_USART[COM], ENABLE);
}

 void setUartBaud(int8_t baud_id)
 {
 	int32_t  baudrate ;
	
 	USART_Cmd(COM_USART[COM2], DISABLE);

	if(baud_id == 1)
	{
		baudrate = 9600;
	}
	else if(baud_id == 2)
	{
		baudrate = 57600;
	}
	else if(baud_id == 3)
	{
		baudrate = 115200;
	}
	else
	{
		baudrate = 250000;
	}

	 SZ_STM32_COMInit(COM2, baudrate);

	USART_Cmd(COM_USART[COM2], ENABLE);
	
 }

/**-------------------------------------------------------
  * @函数名 __SZ_STM32_COMInit
  * @功能   面向用户的STM32的USART初始化函数
  * @参数1  COM1  对应STM32的USART1 对应开发板上串口1
  *         COM2  对应STM32的USART2 对应开发板上串口2
  * @参数2  BaudRate 串口的波特率，例如"115200"
  * @返回值 无
***------------------------------------------------------*/
void SZ_STM32_COMInit(COM_TypeDef COM, uint32_t BaudRate)
{
  
    USART_InitTypeDef USART_InitStructure;
  
    /* USARTx 默认配置:
          - BaudRate = 115200 baud  
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = BaudRate;              //串口的波特率，例如115200 最高达4.5Mbits/s
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据字长度(8位或9位)
    USART_InitStructure.USART_StopBits = USART_StopBits_1;      //可配置的停止位-支持1或2个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;         //无奇偶校验  
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //双工模式，使能发送和接收
  
    __SZ_STM32_COMInit(COM, &USART_InitStructure);  // 调用STM32的USART初始化底层函数
  

    /*printf("\r\n\n\n\r WWW.ARMJISHU.COM  %s printf configured....", 
  	                            (COM == COM1)? SZ_STM32_COM1_STR:SZ_STM32_COM2_STR);
    printf("\n\r ############ WWW.ARMJISHU.COM! ############ ("__DATE__ " - " __TIME__ ")");
  
    printf("%s", STM32F10x_STR);
  
    printf(" WWW.ARMJISHU.COM use __STM32F10X_STDPERIPH_VERSION %d.%d.%d",
  			__STM32F10X_STDPERIPH_VERSION_MAIN,
  			__STM32F10X_STDPERIPH_VERSION_SUB1,
  			__STM32F10X_STDPERIPH_VERSION_SUB2);
    printf("\n\r 产品内部Flash大小为：%dK字节！ \t www.armjishu.com",
              *(__IO uint16_t*)(0x1FFFF7E0));
							
	*/
		
//    SystemCoreClockUpdate();
		
	
  /*  printf("\n\r 系统内核时钟频率(SystemCoreClock)为：%dHz.\n\r",
            SystemCoreClock);*/
	
}
#if 0
void UART2_Printf(uint8_t *buf, uint32_t len)
{
	uint32_t i;

	if(buf == 0  ||  len <= 0)
	{
		return;
	}
	for(i = 0; i < len; i++)
	{
		while (USART_GetFlagStatus(SZ_STM32_COM2, USART_FLAG_TC) == RESET);/*等待发送完成*/
	
	//	__disable_irq(); 
		USART_SendData(SZ_STM32_COM2, *(buf + i)); /*发送一个字符函数*/ 
	//	 __enable_irq();
	
	}
}
#endif
 

/**-------------------------------------------------------
  * @函数名 NVIC_COMConfiguration
  * @功能   配置串口1和串口2的中断参数
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void NVIC_COMConfiguration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
/*
    USART_ITConfig(SZ_STM32_COM1, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = SZ_STM32_COM1_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
*/
    USART_ITConfig(SZ_STM32_COM2, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = SZ_STM32_COM2_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

	
}


/*
    加入以下代码,支持printf函数,不需要选择use MicroLIB	  
*/
#ifndef MicroLIB
//#pragma import(__use_no_semihosting)             //没有实现fgetc时需要声明该参数   
/* 标准库需要的支持函数 使用printf()调试打印不需要实现该函数 */               
struct __FILE 
{ 
	int handle; 
    /* Whatever you require here. If the only file you are using is */    
    /* standard output using printf() for debugging, no file handling */    
    /* is required. */
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
/* 重定义fputc函数 如果使用MicroLIB只需要重定义fputc函数即可 */  
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(SZ_STM32_COM2, USART_FLAG_TC) == RESET)
    {}

    /* e.g. write a character to the USART */
    USART_SendData(SZ_STM32_COM2, (uint8_t) ch);

    return ch;
}
/*
可以直接使用putchar
不需要再定义 int putchar(int ch)，因为stdio.h中有如下定义
 #define putchar(c) putc(c, stdout)
*/

int ferror(FILE *f) {  
    /* Your implementation of ferror */  
    return EOF;  
} 
#endif 

FILE __stdin;

int fgetc(FILE *fp)
{
	int ch = 0;
	
    while(USART_GetFlagStatus(SZ_STM32_COM2, USART_FLAG_RXNE) == RESET)
    {
    }

    ch = (int)SZ_STM32_COM2->DR & 0xFF;
	
    putchar(ch); //回显
	
	return ch;
}


SZ_USART_DATA_TypeDef SZ_USART1_DATA, SZ_USART2_DATA;
volatile SZ_USART_FIFO  UartRxFifo, WifiRxFifo;

#define BUF_INC_POINTER(p)	((p + 1 == UART_FIFO_BUFFER_SIZE) ? 0:(p + 1))

int usartFifoAvailable(SZ_USART_FIFO *fifo)
{
	if(fifo->uart_read_point <= fifo->uart_write_point)
	{
		return fifo->uart_write_point - fifo->uart_read_point;
	}
	else
	{
		return UART_FIFO_BUFFER_SIZE + fifo->uart_write_point - fifo->uart_read_point;
	}
}
	
//uint8_t tmpStr[UART_FIFO_BUFFER_SIZE] = {0};	
int readUsartFifo(SZ_USART_FIFO *fifo, int8_t *buf, int32_t len)
{
	int i = 0 ;
	
	
	/*if((buf == 0) || (len <= 0))
	{
		return -1;
	}*/

	while(i < len )
	{
	//	memset(tmpStr, 0, sizeof(tmpStr));
		
		if(fifo->uart_read_point != fifo->uart_write_point)
		{
			/*if((fifo->uartTxBuffer[fifo->uart_read_point] != '\r') || (fifo->uartTxBuffer[fifo->uart_read_point] != '\n'))
			{
				if(fifo->uart_read_point < fifo->uart_write_point)
				{
					strncpy(tmpStr, &fifo->uartTxBuffer[fifo->uart_read_point], fifo->uart_write_point - fifo->uart_read_point);
					
				}
				else 
				{
					strncpy(tmpStr, &fifo->uartTxBuffer[fifo->uart_read_point], UART_FIFO_BUFFER_SIZE - fifo->uart_read_point);
					strncpy(tmpStr + UART_FIFO_BUFFER_SIZE - fifo->uart_read_point, fifo->uartTxBuffer, fifo->uart_write_point);
				}
				if((strstr(tmpStr, "\n") != 0) || (strstr(tmpStr, "\r") != 0))
				{
					buf[i] = fifo->uartTxBuffer[fifo->uart_read_point];
					fifo->uart_read_point = BUF_INC_POINTER(fifo->uart_read_point);
					i++;
				}
				else
				{
					break;
				}
			}
			else
			{
				buf[i] = fifo->uartTxBuffer[fifo->uart_read_point];
				fifo->uart_read_point = BUF_INC_POINTER(fifo->uart_read_point);
				i++;
			}*/
			buf[i] = fifo->uartTxBuffer[fifo->uart_read_point];
			fifo->uart_read_point = BUF_INC_POINTER(fifo->uart_read_point);
			i++;

		/*	if(i > 2)
			{
				if((buf[i -1]  == '\n')  || (buf[i -1]  == '\r'))
				{
					
					break;
				}
			}*/
			
				
			
		}
		else
		{
			break;
		}
		
	}
	return i;
	
}

int writeUsartFifo(SZ_USART_FIFO *fifo, int8_t *buf, int32_t len)
{
	int i = 0 ;
	
	if((buf == 0) || (len <= 0))
	{
		return -1;
	}

	while(i < len )
	{

		
		if(fifo->uart_read_point != BUF_INC_POINTER(fifo->uart_write_point))
		{
			fifo->uartTxBuffer[fifo->uart_write_point] = buf[i] ;

			fifo->uart_write_point = BUF_INC_POINTER(fifo->uart_write_point);

			i++;
		}
		else
		{
			break;
		}
		
	}
	return i;
	
}
/**-------------------------------------------------------
  * @函数名 UsartIrqHandlerUser
  * @功能   共用的串口中断处理函数
  * @参数   SZ_USART 串口数据结构体指针
  * @返回值 无
***------------------------------------------------------*/
void UsartIrqHandlerUser(SZ_USART_DATA_TypeDef* SZ_USART)
{
    uint8_t uartRxData = 0;
    uint8_t i = 0;

   if(SZ_USART == 0)
   {
   	return;
   }

    if(USART_GetITStatus(SZ_USART->uart, USART_IT_RXNE) != RESET)
    {
        /* save on rx buffer */
        while (SZ_USART->uart->SR & USART_FLAG_RXNE)
        {
	            /* save character */
	            uartRxData = SZ_USART->uart->DR & 0xff;
			writeUsartFifo(&UartRxFifo, &uartRxData, 1);
			 if ((uartRxData == '\r')  || (uartRxData == '\n'))
		        {           
		            SZ_USART->uart_rx_done= 1;
		        }

					
         
        }

        /* clear interrupt */
        USART_ClearITPendingBit(SZ_USART->uart, USART_IT_RXNE);

        /* 接收完成 */
       
    }


    /* If overrun condition occurs, clear the ORE flag 
                              and recover communication */
    if (USART_GetFlagStatus(SZ_USART->uart, USART_FLAG_ORE) != RESET)
    {
        (void)USART_ReceiveData(SZ_USART->uart);
    }
  
	if (USART_GetITStatus(SZ_USART->uart, USART_IT_TC) != RESET)
	{
		/* clear interrupt */
		USART_ClearITPendingBit(SZ_USART->uart, USART_IT_TC);
	}
}


#ifdef ESP_MODEL
static uint8_t espTemBuf[20];
static uint32_t espTempIndex;

void flush_esp_tem_buf()
{
	writeUsartFifo(&WifiRxFifo, espTemBuf, espTempIndex);
	memset(espTemBuf, 0, sizeof(espTemBuf));
	espTempIndex = 0;
}

#endif

void mksWifiIrqHandlerUser()
{
	USART_TypeDef *wifi_com;
    uint8_t uartRxData = 0;
    uint8_t i = 0;


	if(gCfgItems.wifi_type == ESP_WIFI) 
	{
		wifi_com = SZ_STM32_COM1;
	}
	else
	{
		wifi_com = SZ_STM32_COM3;
	}


    if(USART_GetITStatus(wifi_com, USART_IT_RXNE) != RESET)
    {
        /* save on rx buffer */
        while (wifi_com->SR & USART_FLAG_RXNE)
        {
		/* save character */
		uartRxData = wifi_com->DR & 0xff;
	//	gBuf[gIndex++] = uartRxData;
		
		writeUsartFifo(&WifiRxFifo, &uartRxData, 1);
	/*	writeUsartFifo(&UartRxFifo, &uartRxData, 1);
		if ((uartRxData == '\r')  || (uartRxData == '\n'))
		{           
		    SZ_USART->uart_rx_done= 1;
		}	*/
         	WIFI_IO1_SET();
        }

        /* clear interrupt */
        USART_ClearITPendingBit(wifi_com, USART_IT_RXNE);

        /* 接收完成 */
       
    }


    /* If overrun condition occurs, clear the ORE flag 
                              and recover communication */
    if (USART_GetFlagStatus(wifi_com, USART_FLAG_ORE) != RESET)
    {
        (void)USART_ReceiveData(wifi_com);
    }
  
	if (USART_GetITStatus(wifi_com, USART_IT_TC) != RESET)
	{
		/* clear interrupt */
		USART_ClearITPendingBit(wifi_com, USART_IT_TC);
	}
}

/**-------------------------------------------------------
  * @函数名 UsartReadData
  * @功能   共用的串口数据读取子函数
  * @参数1  SZ_USART 串口数据结构体指针
  * @参数2  串口数据存放的Buffer
  * @返回值 无
***------------------------------------------------------*/
uint32_t UsartReadData(SZ_USART_DATA_TypeDef* SZ_USART,  uint8_t* Buffer)
{
   uint32_t uartRxLength = 0, i;



    /* if the next position is read index, discard this 'read char' */
   /*  if (SZ_USART->uart_rx_length != 0)
    {
        

        for(i=0; i<uartRxLength; i++)
        {
            Buffer[i] = SZ_USART->uartRxBuffer[SZ_USART->uart_rx_read_point];
            SZ_USART->uart_rx_read_point++;

            if (SZ_USART->uart_rx_read_point >= UART_RX_BUFFER_SIZE)
            {
                SZ_USART->uart_rx_read_point = 0;
            }
        }

        Buffer[i] = '\0';
    }
		
    return uartRxLength;*/
		return 0;
}

/**-------------------------------------------------------
  * @函数名 USART1_IRQHandler
  * @功能   串口1中断处理函数
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void USART1_IRQHandler(void)
{
    /* 开始临界资源访问，禁止中断 */
	__disable_irq(); 

    /* 串口1中断时将指示灯1状态取反 */
	//LED1OBB = !LED1OBB;

    /* 调用共用的串口中断处理子函数 */
  //  UsartIrqHandlerUser(&SZ_USART1_DATA);
  if(gCfgItems.wifi_type == ESP_WIFI) 
	mksWifiIrqHandlerUser();
    
    /* 结束临界资源访问，开放中断 */
    __enable_irq();
}

/**-------------------------------------------------------
  * @函数名 USART2_IRQHandler
  * @功能   串口2中断处理函数
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void USART2_IRQHandler(void)
{
    /* 开始临界资源访问，禁止中断 */
	__disable_irq(); 
    
    /* 串口2中断时将指示灯2状态取反 */
	//LED2OBB = !LED2OBB;

    /* 调用共用的串口中断处理子函数 */
   //////////// UsartIrqHandlerUser(&SZ_USART2_DATA);
   mksUsart2IrqHandlerUser();

    /* 结束临界资源访问，开放中断 */
    __enable_irq();
}

/**-------------------------------------------------------
  * @函数名 USART3_IRQHandler
  * @功能   串口3中断处理函数
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void USART3_IRQHandler(void)
{
    /* 开始临界资源访问，禁止中断 */
	__disable_irq(); 
    
    /* 串口2中断时将指示灯2状态取反 */
	//LED2OBB = !LED2OBB;

    /* 调用共用的串口中断处理子函数 */
   //////////// UsartIrqHandlerUser(&SZ_USART2_DATA);
    if(gCfgItems.wifi_type == HLK_WIFI) 
  	 mksWifiIrqHandlerUser();

    /* 结束临界资源访问，开放中断 */
    __enable_irq();
}

/**
  * @brief  DeInitializes the SD/SD communication.
  * @param  None
  * @retval None
  */
void SD_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  SPI_Cmd(SD_SPI, DISABLE); /*!< SD_SPI disable */
  SPI_I2S_DeInit(SD_SPI);   /*!< DeInitializes the SD_SPI */
  
  /*!< SD_SPI Periph clock disable */
  RCC_APB1PeriphClockCmd(SD_SPI_CLK, DISABLE);
  /*!< DeRemap SPI3 Pins */
  //GPIO_PinRemapConfig(GPIO_Remap_SPI3, DISABLE);  
  
  /*!< Configure SD_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
  GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
  GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
  GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  Initializes the SD_SPI and CS pins.
  * @param  None
  * @retval None
  */
void SD_LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;

  /*!< SD_SPI_CS_GPIO, SD_SPI_MOSI_GPIO, SD_SPI_MISO_GPIO, SD_SPI_DETECT_GPIO 
       and SD_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(SD_CS_GPIO_CLK | SD_SPI_MOSI_GPIO_CLK | SD_SPI_MISO_GPIO_CLK |
                         SD_SPI_SCK_GPIO_CLK | SD_DETECT_GPIO_CLK, ENABLE);

  /*!< SD_SPI Periph clock enable */
  if(SPI3 == SD_SPI)
  {
    RCC_APB1PeriphClockCmd(SD_SPI_CLK, ENABLE);
  }
  else
  {
    RCC_APB2PeriphClockCmd(SD_SPI_CLK, ENABLE);
  }

  /*!< AFIO Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  /*!< Remap SPI3 Pins */
  if(SPI3 == SD_SPI)
  {
    GPIO_PinRemapConfig(GPIO_Remap_SPI3,ENABLE);  
  }
  
  /*!< Configure SD_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
  GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
  GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin *//*
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);	   */

  /*!< SD_SPI Config */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SD_SPI, &SPI_InitStructure);
  
  SPI_Cmd(SD_SPI, ENABLE); /*!< SD_SPI enable */
}


void SD_SPI_SetSpeed(uint16_t SPI_BaudRatePrescaler)
{
  if((SPI_BaudRatePrescaler & (uint16_t)0x0038) == SPI_BaudRatePrescaler)
  {
    SD_SPI->CR1 = (SD_SPI->CR1 & (uint16_t)0xFFC7) |SPI_BaudRatePrescaler;
 
    SPI_Cmd(SD_SPI, ENABLE); /*!< SD_SPI enable */
  }
}

void SD_SPI_SetSpeedLow(void)
{
    SD_SPI_SetSpeed(SPI_BaudRatePrescaler_256);
}
void SD_SPI_SetSpeedHi(void)
{
	SD_SPI_SetSpeed(SPI_BaudRatePrescaler_2);
}



#ifdef  USE_FULL_ASSERT
// 需要在工程设置Option(快捷键ALT+F7)C++属性页的define栏输入"USE_FULL_ASSERT"
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
     
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/******************* (C) COPYRIGHT 2013 www.armjishu.com *****END OF FILE****/
