/****************************************************************************
 *                                                                          *
 * Copyright (c) 2017 V-tick Technology Corp. All rights reserved.         *
 *                                                                          *
 ****************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     uart.c
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This is the test program used to test the UARTs on NUC970 EV board
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     
 *
 * REMARK
 *     None
 ****************************************************************************/
#include     <stdio.h>
#include     <stdlib.h>
#include     <unistd.h> 
#include     <sys/types.h> 
#include     <sys/stat.h> 
#include     <fcntl.h> 
#include     <termios.h>  
#include     <errno.h>
#include     <string.h>
#include 	<signal.h>
#include    <pthread.h>
#include "linuxdef.h"
#include "fifo.h"
#include "uart.h"
#include "ctl_cmd.h"
#include "softtimer.h"


#define FALSE 0
#define TRUE  1
int timeout2=0;
int t2flag=0;

char  uart_port5[]= "/dev/ttyS5";
char  uart_port6[]= "/dev/ttyS6";
char  uart_port10[]= "/dev/ttyS10";
char *dev[3]={"/dev/ttyS5", "/dev/ttyS6","/dev/ttyS10"};
int fd[3];

pthread_t threads[10];

char buff[101];


Ctl_Cmd_Info uart2_can_info,uart3_info,uart4_info,uart5_info,net_info,udp_info;



volatile char matrixlock=0;

void matrixlock_op(char status)
{
	matrixlock=!status;
}



static struct termios newtios,oldtios; /*termianal settings */
static int saved_portfd=-1;            /*serial port fd */

void sett2flag(int flag)
{
	t2flag =flag;

}
static void reset_tty_atexit(void)
{
	if(saved_portfd != -1)
	{
		tcsetattr(saved_portfd,TCSANOW,&oldtios);
	} 
}

/*cheanup signal handler */
static void reset_tty_handler(int signal)
{
	if(saved_portfd != -1)
	{
		tcsetattr(saved_portfd,TCSANOW,&oldtios);
	}
	_exit(EXIT_FAILURE);
}


static int open_port(const char *portname,speed_t speed)
{
	struct sigaction sa;
	int portfd;

	printf("opening serial port:%s\n",portname);
	/*open serial port */
	if((portfd=open(portname,O_RDWR | O_NOCTTY | O_NDELAY)) < 0 )
	{
   		printf("open serial port %s fail \n ",portname);
   		return portfd;
	}




    //»Ö¸´´®¿ÚÎª×èÈû×´Ì¬                                   
    if(fcntl(portfd, F_SETFL, 0) < 0)    
    {    
        printf("fcntl failed!\n");    
        return(FALSE);    
    }         
    else    
    {    
        printf("fcntl=%d\n",fcntl(portfd, F_SETFL,0));    
    } 
	/*get serial port parnms,save away */
	tcgetattr(portfd,&newtios);
	memcpy(&oldtios,&newtios,sizeof newtios);
	/* configure new values */
	cfmakeraw(&newtios); /*see man page */
	newtios.c_iflag |=IGNPAR; /*ignore parity on input */
	newtios.c_oflag &= ~(OPOST | ONLCR | OLCUC | OCRNL | ONOCR | ONLRET | OFILL); 
	newtios.c_cflag = CS8 | CLOCAL | CREAD;
	newtios.c_cc[VMIN]=1; /* block until 1 char received */
	newtios.c_cc[VTIME]=0; /*no inter-character timer */
	/* 115200 bps */
	cfsetospeed(&newtios,speed);
	cfsetispeed(&newtios,speed);
	/* register cleanup stuff */
	atexit(reset_tty_atexit);
	memset(&sa,0,sizeof sa);
	sa.sa_handler = reset_tty_handler;
	sigaction(SIGHUP,&sa,NULL);
	sigaction(SIGINT,&sa,NULL);
	sigaction(SIGPIPE,&sa,NULL);
	sigaction(SIGTERM,&sa,NULL);
	/*apply modified termios */
	saved_portfd=portfd;
	tcflush(portfd,TCIFLUSH);
	tcsetattr(portfd,TCSADRAIN,&newtios);
	return portfd;
}

void * uartreceive_process0(void* arg)
{
	int portfd = (int) arg;
	int rev1;
	char RxBuffer[101];

	rev1 = 0;
	while(1)
	{
		rev1 = read(portfd,(RxBuffer),100);
		if (rev1>0)
		{
			//printf("\n uart receive %d bytes \n", rev1);
			write(fd[2],RxBuffer,rev1); //loop to uart2
			if(matrixlock)
				Write_Cmd_FIFO(RxBuffer,rev1,&COM3FIFO);
		}
	}
}
void * uartreceive_process1(void* arg)
{
	int portfd = (int) arg;
	int rev1;
	char RxBuffer[101];
	rev1 = 0;
	//buf=RxBuffer;
	while(1)
	{
		rev1 = read(portfd,(RxBuffer),100);
		if (rev1>0)
		{
			//printf("\n uart1 receive %d bytes \n", rev1);
			if(matrixlock)
				Write_Cmd_FIFO(RxBuffer,rev1,&COM4FIFO);
		}
	}

}


void * uartreceive_process2(void* arg)
{
	int portfd = (int) arg;
	int rev1;
	char RxBuffer[101];
	rev1 = 0;
	while(1)
	{
		rev1 = read(portfd,(RxBuffer),100);
		if (rev1>0)
		{
			//printf("\n uart2 receive %d bytes \n", rev1);
			write(fd[0],RxBuffer,rev1);// loop to uart 0
			if(matrixlock)
				Write_Cmd_FIFO(RxBuffer,rev1,&COM5FIFO);
		}
	}



#if 0
    int retval;
    fd_set rfds;
    struct timeval tv;

    tv.tv_sec = 0;//set the rcv wait time
    tv.tv_usec = 100000;//100000us = 0.1s
	timeout2=0;

    while(1)
    {
        FD_ZERO(&rfds);
        FD_SET(portfd,&rfds);
        retval = select(portfd+1,&rfds,NULL,NULL,&tv);
        if(retval>0)
        {	

		rev1 = read(portfd,(RxBuffer),(100));
		printf("uart2 rev1 %d\n", rev1);
		Write_Cmd_FIFO(RxBuffer,rev1,&COM5FIFO);
		timeout2=0;
            continue;
        }
        else if(retval==0)
        {
		//printf("\n uart2 receive timeout \n");
		if (t2flag)
			timeout2+=1;

        }
        else
        {
            printf("\n uart2 receive error \n");
        }
    }

		rev1 = read(portfd,(RxBuffer),100);
		if (rev1>0)
		{
			//printf("\n uart receive %d bytes \n", rev1);
			Write_Cmd_FIFO(RxBuffer,rev1,&COM5FIFO);
		}







	while(1)
	{
		rev1 = read(portfd,(buf),(100-len));
		if (rev1<0)
		{
			if (len>0){
				printf("\n uart2 receive %d bytes \n", len);
				for(i=0;i<len;i++)
					printf("0x%x,",RxBuffer[i]);
				printf("\n uart2 receive print end \n");

				Write_Cmd_FIFO(RxBuffer,len,&COM5FIFO);
			}
			len=0;
			buf=RxBuffer;
			

		}
		else
		{
			buf+=rev1;
			len+=rev1;
		}

	}
#endif

}



void uart1_putchar(char data)
{
 int rev;
	rev = write(fd[0],&data,1);
        if (rev<0)
	{
		printf("uart port5 write error \n");
	}
}


void uart1_puts( char s[])
{
   char c;
	int i;
   i = 0;
   do {
      c = s[i++];
	  if (c != 0)
	  {
	     uart1_putchar(c);
	  }
   } while (c != 0);  
}

void uart2_putchar(char data)
{
 int rev;
	rev = write(fd[1],&data,1);
        if (rev<0)
	{
		printf("uart port6 write error \n");
	}
}


void uart2_puts( char s[])
{
   char c;
	int i;
   i = 0;
   do {
      c = s[i++];
	  if (c != 0)
	  {
	     uart2_putchar(c);
	  }
   } while (c != 0);  
}

void uart3_putchar(char data)
{
 int rev;
	rev = write(fd[2],&data,1);
        if (rev<0)
	{
		printf("uart port10 write error \n");
	}
}


void uart3_puts( char s[])
{
   char c;
	int i;
   i = 0;
   do {
      c = s[i++];
	  if (c != 0)
	  {
	     uart3_putchar(c);
	  }
   } while (c != 0);  
}

void candebug_putchar(char data)
{
#if DEBUG
	printf("%c",data);
#endif
}


void candebug_puts( char s[])
{
#if DEBUG
   printf("%s",s); 
#endif
}


void init_ctl_cmd_info(Ctl_Cmd_Info* cmd_info,__FIFO * fifo,com_puts puts,com_putchar putchar,etimer_def * etimer)
{
		cmd_info->Cmd_Type = 0;
		cmd_info->Buffer_LEN = 0;
		cmd_info->Buffer_Data_Len = 0;
	  cmd_info->Buffer_Data = (u8 *)malloc(BUFFER_LEN_MAX);
		cmd_info->fifo = fifo;
		cmd_info->timer = etimer;
	  cmd_info->uart_puts = puts;
	  cmd_info->uart_putchar = putchar;
}


void init_uart_net_info(void)
{

		init_ctl_cmd_info(&uart3_info,&COM3FIFO,uart1_puts,uart1_putchar,&stimer0);
		init_ctl_cmd_info(&uart4_info,&COM4FIFO,uart2_puts,uart2_putchar,&stimer1);
		init_ctl_cmd_info(&uart5_info,&COM5FIFO,uart3_puts,uart3_putchar,&stimer2);
	  	init_ctl_cmd_info(&uart2_can_info,&COM2_CAN_FIFO,candebug_puts,candebug_putchar,&stimer3);
	  	init_ctl_cmd_info(&net_info,&NETFIFO,candebug_puts,candebug_putchar,&stimer4);
	  	init_ctl_cmd_info(&udp_info,&UDPFIFO,candebug_puts,candebug_putchar,&stimer5);
		matrixlock_op(OFF);

}



int uart_init(void)
{
	//init uartport 5

	if((fd[0] = open_port(uart_port5,B9600))<0)
	{
		printf("open uart port5 fail \n");
   		return -1;
	}
	if((fd[1] = open_port(uart_port6,B115200))<0)
	{
		printf("open uart port6 fail \n");
   		return -1;
	}
	if((fd[2] = open_port(uart_port10,B9600))<0)
	{
		printf("open uart port10 fail \n");
   		return -1;
	}

	return 0;
}

void uart_interrupt_op(FunctionalState state)
{
printf("%s test\n",__func__);	
}


pthread_t uart_ctl_th[3];

int uart_thread_init(void)
{
	pthread_create(&uart_ctl_th[0], NULL, uartreceive_process0, (void*)(fd[0]));
	pthread_create(&uart_ctl_th[1], NULL, uartreceive_process1, (void*)(fd[1]));
	pthread_create(&uart_ctl_th[2], NULL, uartreceive_process2, (void*)(fd[2]));

	return 0;

}


int uart_thread_join(void)
{
	pthread_join(uart_ctl_th[0], NULL);
	pthread_join(uart_ctl_th[1], NULL);
	pthread_join(uart_ctl_th[2], NULL);
	return 0;
}





/**
*@breif 	main()
*/
#if 0
int main(int argc, char **argv)
{
	char *dev[10]={"/dev/ttyS1", "/dev/ttyS2"};
	unsigned int i;

	printf("\n demo uart1/uart2 external loop back function \n");

	for(i = 0; i < 100; i++)
	{
		buff[i] = (i & 0xff);
	}


	for(i = 0; i < 2; i++)
	{
		if((fd[i] = open_port(dev[i]))<0)
   			return -1;
	}
	
	pthread_create(&threads[0], NULL, process1, (void*)(fd[0]));
	pthread_create(&threads[1], NULL, process2, (void*)(fd[1]));

	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

		   
  return 0;
}
#endif
