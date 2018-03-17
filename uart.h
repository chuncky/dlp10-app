#ifndef _UART_H
#define _UART_H

#define BUFFER_LEN_MAX   400
#include "softtimer.h"
int uart_init(void);
void * uartreceive_process0(void* arg);
void * uartreceive_process1(void* arg);
void * uartreceive_process2(void* arg);



extern void uart1_putchar(char data);
extern void uart2_putchar(char data);
extern void uart3_putchar(char data);

extern void uart1_puts(char s[]);
extern void uart2_puts(char s[]);
extern void uart3_puts(char s[]);

extern void candebug_putchar(char data);
extern void candebug_puts(char s[]);

#define  ON						1
#define OFF 	 				0

extern volatile char matrixlock;

void matrixlock_op(char status);

typedef void (*com_puts)(char s[]);
typedef void (*com_putchar)(char data);

typedef struct __CMD__INFO
{
		volatile u8 Cmd_Type;//ascii »òÕßHEX ÃüÁî		
		u16 Buffer_LEN;//½ÓÊÕµ½µÄÃüÁî³¤¶È
		u8 *Buffer_Data;//´æ·Å½ÓÊÕµ½Ò»ÌõÍêÕûµÄÃüÁîbuffer,³¤¶ÈÎªBUFFER_LEN;
	  u16 Buffer_Data_Len;//ÃüÁîÖÐÓÐÐ§Êý¾Ý³¤¶È
	  u8 DATA_AVAILABLE;//ÃüÁîÓÐÐ§±êÖ¾.
	  __FIFO * fifo;
	 	etimer_def * timer;
		com_puts uart_puts ;
		com_putchar uart_putchar;
}Ctl_Cmd_Info;

extern Ctl_Cmd_Info uart2_can_info,uart3_info,uart4_info,uart5_info,net_info,udp_info;
extern void init_uart_net_info(void);


int uart_thread_init(void);
int uart_thread_join(void);
void sett2flag(int flag);
#endif
