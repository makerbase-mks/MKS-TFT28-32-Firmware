/*------------------------------------------------------------------------/
/  Universal string handler for user console interface
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2011, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/ modified by www.armjishu.com
/-------------------------------------------------------------------------*/
#if 0	//tan 20170112

#include "stm32f10x.h"
#include "SZ_STM32F107VC_LIB.h"
#include "xprintf.h"

#if _USE_XFUNC_OUT
#include <stdarg.h>
void (*xfunc_out)(unsigned char);	/* Pointer to the output stream */
static char *outptr;

#ifndef SZ_STM32_COM1_STR
  #define SZ_STM32_COM1_STR                    "USART1"
#endif

extern const uint8_t STM32F10x_STR[];

/* Private functions ---------------------------------------------------------*/
void xPrintf_Init(void)
{
  USART_InitTypeDef USART_InitStructure;   
       
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  __SZ_STM32_COMInit(COM1, &USART_InitStructure);
  xdev_out(xUSART1_putchar);
  xdev_in(xUSART1_getchar);  
  
  /* Output a message on Hyperterminal using printf function */
  //printf("\n\rUSART Printf Example: retarget the C library printf function to the USART\n\r");
  xprintf("\r\n\n\n\r WWW.ARMJISHU.COM  %s xprintf configured....", SZ_STM32_COM1_STR);
  xprintf("\n\r ############ WWW.ARMJISHU.COM! ############ ("__DATE__ " - " __TIME__ ")");

  xprintf("%s", STM32F10x_STR);

  xprintf(" WWW.ARMJISHU.COM use __STM32F10X_STDPERIPH_VERSION %d.%d.%d",
			__STM32F10X_STDPERIPH_VERSION_MAIN,
			__STM32F10X_STDPERIPH_VERSION_SUB1,
			__STM32F10X_STDPERIPH_VERSION_SUB2);
  xprintf("\n\r 产品内部Flash大小为：%dK字节！ \t www.armjishu.com",
            *(__IO uint16_t*)(0x1FFFF7E0));
  SystemCoreClockUpdate();
  xprintf("\n\r 系统内核时钟频率(SystemCoreClock)为：%dHz.\n\r",
            SystemCoreClock);
}


unsigned char xUSART2_putchar(unsigned char ch)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(SZ_STM32_COM2, (uint8_t) ch); /*发送一个字符函数*/ 

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(SZ_STM32_COM2, USART_FLAG_TC) == RESET)/*等待发送完成*/
  {
  
  }
  return ch;
}

unsigned char xUSART1_putchar(unsigned char ch)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(SZ_STM32_COM1, (uint8_t) ch); /*发送一个字符函数*/ 

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(SZ_STM32_COM1, USART_FLAG_TC) == RESET)/*等待发送完成*/
  {
  
  }
  return ch;
}
/*----------------------------------------------*/
/* Put a character                              */
/*----------------------------------------------*/
void xputc (char c /* A character to be output */) 
{
	if (_CR_CRLF && c == '\n') xputc('\r');		/* CR -> CRLF */

	if (outptr) {
		*outptr++ = (unsigned char)c;
		return;
	}

	if (xfunc_out) xfunc_out((unsigned char)c);
}



/*----------------------------------------------*/
/* Put a null-terminated string                 */
/*----------------------------------------------*/

void xputs (					/* Put a string to the default device */
	const char* str				/* Pointer to the string */
)
{
	while (*str)
		xputc(*str++);
}


void xfputs (					/* Put a string to the specified device */
	void(*func)(unsigned char),	/* Pointer to the output function */
	const char*	str				/* Pointer to the string */
)
{
	void (*pf)(unsigned char);


	pf = xfunc_out;		/* Save current output device */
	xfunc_out = func;	/* Switch output to specified device */
	while (*str)		/* Put the string */
		xputc(*str++);
	xfunc_out = pf;		/* Restore output device */
}



/*----------------------------------------------*/
/* Formatted string output                      */
/*----------------------------------------------*/
/*  xprintf("%d", 1234);			"1234"
    xprintf("%6d,%3d%%", -200, 5);	"  -200,  5%"
    xprintf("%-6u", 100);			"100   "
    xprintf("%ld", 12345678L);		"12345678"
    xprintf("%04x", 0xA3);			"00a3"
    xprintf("%08LX", 0x123ABC);		"00123ABC"
    xprintf("%016b", 0x550F);		"0101010100001111"
    xprintf("%s", "String");		"String"
    xprintf("%-4s", "abc");			"abc "
    xprintf("%4s", "abc");			" abc"
    xprintf("%c", 'a');				"a"
    xprintf("%f", 10.0);            <xprintf lacks floating point support>
*/

static
void xvprintf (
	const char*	fmt,	/* Pointer to the format string */
	va_list arp			/* Pointer to arguments */
)
{
	unsigned int r, i, j, w, f;
	unsigned long v;
	char s[16], c, d, *p;


	for (;;) {
		c = *fmt++;					/* Get a char */
		if (!c) break;				/* End of format? */
		if (c != '%') {				/* Pass through it if not a % sequense */
			xputc(c); continue;
		}
		f = 0;
		c = *fmt++;					/* Get first char of the sequense */
		if (c == '0') {				/* Flag: '0' padded */
			f = 1; c = *fmt++;
		} else {
			if (c == '-') {			/* Flag: left justified */
				f = 2; c = *fmt++;
			}
		}
		for (w = 0; c >= '0' && c <= '9'; c = *fmt++)	/* Minimum width */
			w = w * 10 + c - '0';
		if (c == 'l' || c == 'L') {	/* Prefix: Size is long int */
			f |= 4; c = *fmt++;
		}
		if (!c) break;				/* End of format? */
		d = c;
		if (d >= 'a') d -= 0x20;
		switch (d) {				/* Type is... */
		case 'S' :					/* String */
			p = va_arg(arp, char*);
			for (j = 0; p[j]; j++) ;
			while (!(f & 2) && j++ < w) xputc(' ');
			xputs(p);
			while (j++ < w) xputc(' ');
			continue;
		case 'C' :					/* Character */
			xputc((char)va_arg(arp, int)); continue;
		case 'B' :					/* Binary */
			r = 2; break;
		case 'O' :					/* Octal */
			r = 8; break;
		case 'D' :					/* Signed decimal */
		case 'U' :					/* Unsigned decimal */
			r = 10; break;
		case 'X' :					/* Hexdecimal */
			r = 16; break;
		default:					/* Unknown type (passthrough) */
			xputc(c); continue;
		}

		/* Get an argument and put it in numeral */
		v = (f & 4) ? va_arg(arp, long) : ((d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int));
		if (d == 'D' && (v & 0x80000000)) {
			v = 0 - v;
			f |= 8;
		}
		i = 0;
		do {
			d = (char)(v % r); v /= r;
			if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
			s[i++] = d + '0';
		} while (v && i < sizeof(s));
		if (f & 8) s[i++] = '-';
		j = i; d = (f & 1) ? '0' : ' ';
		while (!(f & 2) && j++ < w) xputc(d);
		do xputc(s[--i]); while(i);
		while (j++ < w) xputc(' ');
	}
}


/*----------------------------------------------/
/  xprintf - Formatted string output
/----------------------------------------------*/
/*  xprintf("%d", 1234);            "1234"
    xprintf("%6d,%3d%%", -200, 5);  "  -200,  5%"
    xprintf("%-6u", 100);           "100   "
    xprintf("%ld", 12345678L);      "12345678"
    xprintf("%04x", 0xA3);          "00a3"
    xprintf("%08LX", 0x123ABC);     "00123ABC"
    xprintf("%016b", 0x550F);       "0101010100001111"
    xprintf("%s", "String");        "String"
    xprintf("%-4s", "abc");         "abc "
    xprintf("%4s", "abc");          " abc"
    xprintf("%c", 'a');             "a"
    xprintf("%f", 10.0);            <xprintf lacks floating point support>
*/
void xprintf (			/* Put a formatted string to the default device */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
	va_list arp;


	va_start(arp, fmt);
	xvprintf(fmt, arp);
	va_end(arp);
}


void xsprintf (			/* Put a formatted string to the memory */
	char* buff,			/* Pointer to the output buffer */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
)
{
	va_list arp;


	outptr = buff;		/* Switch destination for memory */

	va_start(arp, fmt);
	xvprintf(fmt, arp);
	va_end(arp);

	*outptr = 0;		/* Terminate output string with a \0 */
	outptr = 0;			/* Switch destination for device */
}


void xfprintf (					/* Put a formatted string to the specified device */
	void(*func)(unsigned char),	/* Pointer to the output function */
	const char*	fmt,			/* Pointer to the format string */
	...							/* Optional arguments */
)
{
	va_list arp;
	void (*pf)(unsigned char);


	pf = xfunc_out;		/* Save current output device */
	xfunc_out = func;	/* Switch output to specified device */

	va_start(arp, fmt);
	xvprintf(fmt, arp);
	va_end(arp);

	xfunc_out = pf;		/* Restore output device */
}



/*----------------------------------------------*/
/* Dump a line of binary dump                   */
/*----------------------------------------------*/

void put_dump (
	const void* buff,		/* Pointer to the array to be dumped */
	unsigned long addr,		/* Heading address value */
	int len,				/* Number of items to be dumped */
	int width				/* Size of the items (DF_CHAR, DF_SHORT, DF_LONG) */
)
{
	int i;
	const unsigned char *bp;
	const unsigned short *sp;
	const unsigned long *lp;


	xprintf("%08lX ", addr);		/* address */

	switch (width) {
	case DW_CHAR:
		bp = buff;
		for (i = 0; i < len; i++)		/* Hexdecimal dump */
			xprintf(" %02X", bp[i]);
		xputc(' ');
		for (i = 0; i < len; i++)		/* ASCII dump */
			xputc((bp[i] >= ' ' && bp[i] <= '~') ? bp[i] : '.');
		break;
	case DW_SHORT:
		sp = buff;
		do								/* Hexdecimal dump */
			xprintf(" %04X", *sp++);
		while (--len);
		break;
	case DW_LONG:
		lp = buff;
		do								/* Hexdecimal dump */
			xprintf(" %08LX", *lp++);
		while (--len);
		break;
	}

	xputc('\n');
}

#endif /* _USE_XFUNC_OUT */



#if _USE_XFUNC_IN
unsigned char (*xfunc_in)(void);	/* Pointer to the input stream */


unsigned char xUSART2_getchar(void)
{
  unsigned char key = 0;

  /* Waiting for user input */
  while (1)
  {
    if ( USART_GetFlagStatus(SZ_STM32_COM2, USART_FLAG_RXNE) != RESET)
    {
      key = (uint8_t)SZ_STM32_COM2->DR & 0xFF;
      break;
    }
  }
  return key;
}

unsigned char xUSART1_getchar(void)
{
  unsigned char key = 0;

  /* Waiting for user input */
  while (1)
  {
    if ( USART_GetFlagStatus(SZ_STM32_COM1, USART_FLAG_RXNE) != RESET)
    {
      key = (uint8_t)SZ_STM32_COM1->DR & 0xFF;
      break;
    }
  }
  return key;
}

/*----------------------------------------------*/
/* Get a line from the input                    */
/*----------------------------------------------*/

int xgets (		/* 0:End of stream, 1:A line arrived */
	char* buff,	/* Pointer to the buffer */
	int len		/* Buffer length */
)
{
	int c, i;


	if (!xfunc_in) return 0;		/* No input function specified */

	i = 0;
	for (;;) {
		c = xfunc_in();				/* Get a char from the incoming stream */
		if (!c) return 0;			/* End of stream? */
		if (c == '\r') break;		/* End of line? */
		if (c == '\b' && i) {		/* Back space? */
			i--;
			if (_LINE_ECHO) xputc(c);
			continue;
		}
		if (c >= ' ' && i < len - 1) {	/* Visible chars */
			buff[i++] = c;
			if (_LINE_ECHO) xputc(c);
		}
	}
	buff[i] = 0;	/* Terminate with a \0 */
	if (_LINE_ECHO) xputc('\n');
	return 1;
}

/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param  key: The key pressed
  * @retval 1: Correct
  *         0: Error
  */
uint32_t xUSART2_ValidInput(void)
{

  if ( USART_GetFlagStatus(SZ_STM32_COM2, USART_FLAG_RXNE) != RESET)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param  key: The key pressed
  * @retval 1: Correct
  *         0: Error
  */
uint32_t xUSART1_ValidInput(void)
{

  if ( USART_GetFlagStatus(SZ_STM32_COM1, USART_FLAG_RXNE) != RESET)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

uint32_t xgets_nowait (uint8_t * buffP, uint32_t length)
{
    static uint32_t bytes_read = 0;
    uint8_t c = 0;
  
    if(xUSART2_ValidInput())
    {
        c = xUSART2_getchar();
        if (c == '\r')
        {
          xUSART2_putchar('\n');
          xUSART2_putchar('\r');          
          buffP[bytes_read] = '\0';
          bytes_read = 0;

          return 1;
        }

        if (c == '\b') /* Backspace */
        {
          if (bytes_read > 0)
          {
            xUSART2_putchar('\b');
            xUSART2_putchar(' ');
            xUSART2_putchar('\b');
            bytes_read--;
          }
          return 0;          
        }
        //if (bytes_read >= (CMD_STRING_SIZE))
        if (bytes_read >= (length))
        {
          //printf("Command string size overflow\r\n");
          bytes_read = 0;
          return 0;
        }
        if (c >= 0x20 && c <= 0x7E)
        {
          buffP[bytes_read] = c;
          bytes_read++;
          xUSART2_putchar(c);
        }
    }
    
    return 0;
}


int xfgets (	/* 0:End of stream, 1:A line arrived */
	unsigned char (*func)(void),	/* Pointer to the input stream function */
	char* buff,	/* Pointer to the buffer */
	int len		/* Buffer length */
)
{
	unsigned char (*pf)(void);
	int n;


	pf = xfunc_in;			/* Save current input device */
	xfunc_in = func;		/* Switch input to specified device */
	n = xgets(buff, len);	/* Get a line */
	xfunc_in = pf;			/* Restore input device */

	return n;
}


/*----------------------------------------------*/
/* Get a value of the string                    */
/*----------------------------------------------*/
/*	"123 -5   0x3ff 0b1111 0377  w "
	    ^                           1st call returns 123 and next ptr
	       ^                        2nd call returns -5 and next ptr
                   ^                3rd call returns 1023 and next ptr
                          ^         4th call returns 15 and next ptr
                               ^    5th call returns 255 and next ptr
                                  ^ 6th call fails and returns 0
*/

int xatoi (			/* 0:Failed, 1:Successful */
	char **str,		/* Pointer to pointer to the string */
	long *res		/* Pointer to the valiable to store the value */
)
{
	unsigned long val;
	unsigned char c, r, s = 0;


	*res = 0;

	while ((c = **str) == ' ') (*str)++;	/* Skip leading spaces */

	if (c == '-') {		/* negative? */
		s = 1;
		c = *(++(*str));
	}

	if (c == '0') {
		c = *(++(*str));
		switch (c) {
		case 'x':		/* hexdecimal */
			r = 16; c = *(++(*str));
			break;
		case 'b':		/* binary */
			r = 2; c = *(++(*str));
			break;
		default:
			if (c <= ' ') return 1;	/* single zero */
			if (c < '0' || c > '9') return 0;	/* invalid char */
			r = 8;		/* octal */
		}
	} else {
		if (c < '0' || c > '9') return 0;	/* EOL or invalid char */
		r = 10;			/* decimal */
	}

	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
			if (c <= 9) return 0;	/* invalid char */
		}
		if (c >= r) return 0;		/* invalid char for current radix */
		val = val * r + c;
		c = *(++(*str));
	}
	if (s) val = 0 - val;			/* apply sign if needed */

	*res = val;
	return 1;
}

#endif /* _USE_XFUNC_IN */
#endif//tan 20170112
