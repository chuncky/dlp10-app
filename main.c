
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>


#include "linuxdef.h"
#include "matrix.h"
#include "fifo.h"
#include "can.h"
#include "uart.h"
#include "ctl_cmd.h"
#include "net_ctl.h"
#include "softtimer.h"

typedef  unsigned char SOCKET;
#define TRUE	0xffffffff

char Splice_Control_Buffer[30];
#define SRC_USART1_DR (&(USART1->DR)) 

//--------------------------------

//===========================DEBUG_DDC===============================================//

#if DEBUG_MODE_ON

#define 	DEBUG_OUT_DDC   		1
#define 	DEBUG_IN_DDC				0
//===========================================================================//
//debug_type:DEBUG_OUT_DDC或者DEBUG_IN_DDC,channel_num:输入卡或者输出卡通道号

//1.STM32与CPLD通信的管脚绑定位置是否正确,共有4个管脚，clk(CPLD_CLK),ce(CPLD_OUT_RX),up(CPLD_UP),data(CPLD_DIN),
//2.确认ddc.h内的OUT_DDC_NUM[],IN_DDC_NUM[],两个数组内的数据是否符合CPLD内部定义的数据格式。
//3.确认Data_CPLD()函数实现的数据发送时序是否符合要求。
//4.根据上面的确认的正确结果可以进行测试DDC，输入和输出DDC测试。
//===========================================================================//

void DEBUG_DDC(unsigned char debug_type,unsigned char channel_num)
{
		int count = 0;
		unsigned char tmp = 0;	
//     Data_CPLD(debug_type ? (OUT_DDC_NUM[channel_num-1]):(IN_DDC_NUM[channel_num-1]));//0xc1到0xe0共32路输出（单个通道选通测试）
		for(count=0;count<128;count++)
		{
			tmp = debug_type ? OUT_I2C_Read(count) : IN_I2C_Read(count);
			uart5_putchar(tmp);
		}
		delay_ms(100);
// 		Data_CPLD(0x40);

}

#endif





pthread_t main_th;

void * main_process(void* arg)
{
 	uart1_puts("uart3 init ok\r\n");
 	uart2_puts("uart4 init ok\r\n");
	uart3_puts("uart5 init ok\r\n");
  	candebug_puts("candebug init ok\r\n");
//------------------------------------------------------------------	
	uart3_putchar(0x0d);
	uart3_putchar(0x0a);

	if(iMtxState[LOGO_VISIBLE] == 0x55)
	{
 		uart3_puts(DEVICE_NAME);		//
		uart3_putchar(' ');
	}

	uart3_puts(Software_version);		//					
	uart3_putchar(0x0d);
	uart3_putchar(0x0a);
	uart3_putchar(0x49);			// I
	uart3_putchar(0x44);			// D
	uart3_putchar(0x3A);			// :
	uart3_putchar(iMtxState[INDEX_MACHINE_ID]);
	uart3_putchar(0x0d);
	uart3_putchar(0x0a);
// 	

	while(1)
	{
		 //handle_ctl_cmd(&uart1_info);
		 handle_ctl_cmd(&uart3_info);
		 handle_ctl_cmd(&uart4_info);
		 handle_ctl_cmd(&uart5_info);
		 handle_ctl_cmd(&net_info);
		handle_ctl_cmd(&uart2_can_info);
		handle_ctl_cmd(&udp_info);
		timerprocess();


		usleep(1000); //10ms
		//printf("%s-01\n",__func__);
	}

}


int  main (void)
{
	//int  os_err;
	Init_Fifos();
	uart_init();
	can_init();
	uart_thread_init();
	can_thread_init();

	//BSP_Init();         /* Disable all ints until we are ready to accept them.  */
	init_uart_net_info();
	Matrix_init();
	time_init();
	pthread_create(&main_th, NULL, main_process, NULL);
	net_init();
	net_thread_init();
	uart_thread_join();

	can_thread_join();
	net_thread_join();

	pthread_join(main_th, NULL);


	while(1)
	{
	printf("%s-03\n",__func__);	
 		//handle_ctl_cmd(&uart1_info);
		 //handle_ctl_cmd(&uart3_info);
		 //handle_ctl_cmd(&uart4_info);
		 //handle_ctl_cmd(&uart5_info);
		 //handle_ctl_cmd(&net_info);
		 //handle_ctl_cmd(&uart2_can_info);
		usleep(100000); //100ms
	}
                 
    return (0);
}








