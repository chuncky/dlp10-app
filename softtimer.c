

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "linuxdef.h"
#include "matrix.h"
#include "fifo.h"
#include "uart.h"
#include "net_ctl.h"
#include "can.h"
#include "status_file.h"
#include "ctl_cmd.h"

#include "softtimer.h"


#define SET_LED_SYS system("echo 1 > /sys/class/gpio/gpio102/value")
#define CLEAR_LED_SYS system("echo 0 > /sys/class/gpio/gpio102/value")

#define TIMECNT 100


etimer_def stimer0,stimer1,stimer2,stimer3,stimer4,stimer5;


		//init_ctl_cmd_info(&uart3_info,&COM3FIFO,uart1_puts,uart1_putchar,&stimer0);
		//init_ctl_cmd_info(&uart4_info,&COM4FIFO,uart2_puts,uart2_putchar,&stimer1);
		//init_ctl_cmd_info(&uart5_info,&COM5FIFO,uart3_puts,uart3_putchar,&stimer2);
	  	//init_ctl_cmd_info(&uart2_can_info,&COM2_CAN_FIFO,candebug_puts,candebug_putchar,&stimer3);
	  	//init_ctl_cmd_info(&net_info,&NETFIFO,candebug_puts,candebug_putchar,&stimer4);
	  	//init_ctl_cmd_info(&udp_info,&UDPFIFO,candebug_puts,candebug_putchar,&stimer5);


static char counter=0;
static char ledcnt=0;

void timerprocess(void)
{
	if(stimer0.enable){
		stimer0.count++;
		if(stimer0.count>stimer0.timeout){
			stimer0.enable=0;
			uart3_info.Cmd_Type=0;
			printf("timer0 timeout \n");
		}
	}
	if(stimer1.enable){
		stimer1.count++;
		if(stimer1.count>stimer1.timeout){
			stimer1.enable=0;
			uart4_info.Cmd_Type=0;
			printf("timer1 timeout \n");
		}
	}
	if(stimer2.enable){
		stimer2.count++;
		if(stimer2.count>stimer2.timeout){
			stimer2.enable=0;
			uart5_info.Cmd_Type=0;
			printf("timer2 timeout \n");
		}
	}
	if(stimer3.enable){
		stimer3.count++;
		if(stimer3.count>stimer3.timeout){
			stimer3.enable=0;
			uart2_can_info.Cmd_Type=0;
			printf("timer3 timeout \n");
		}
	}

	if(stimer4.enable){
		stimer4.count++;
		if(stimer4.count>stimer4.timeout){
			stimer4.enable=0;
			net_info.Cmd_Type=0;
			printf("timer4 timeout \n");
		}
	}

	if(stimer5.enable){
		stimer5.count++;
		if(stimer5.count>stimer5.timeout){
			stimer5.enable=0;
			udp_info.Cmd_Type=0;
			printf("timer5 timeout \n");
		}
	}
	if(counter<5)
		counter++;
	else {
		counter=0;
		ledcnt++;
		ledcnt&=0x01;

		if (ledcnt&0x1)
			SET_LED_SYS;
		else
			CLEAR_LED_SYS;
	}



}

void time_init(void)
{
	int res;
	struct itimerval tick;



	system("echo 102 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio102/direction");
	system("echo 1 > /sys/class/gpio/gpio102/value");
	system("echo 101 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio101/direction");
	system("echo 1 > /sys/class/gpio/gpio101/value");

#if 0
	signal(SIGALRM,timerprocess);
	// Initialize struct 
	memset(&tick,0,sizeof(tick));
	// Timeout to run function first time
	tick.it_value.tv_sec=0;//sec
	tick.it_value.tv_usec=500000;//micro sec
	// Interval time to run function  
	tick.it_interval.tv_sec=0;
	tick.it_interval.tv_usec=500000;
	res=setitimer(ITIMER_REAL,&tick,NULL);
	if(res){
		printf("Set timer failed!!\n");
	}
#endif
	counter=0;
	stimer0.id=0;
	stimer1.id=1;
	stimer2.id=2;
	stimer3.id=3;
	stimer4.id=4;
	stimer5.id=5;
	stimer0.enable=0;
	stimer1.enable=0;
	stimer2.enable=0;
	stimer3.enable=0;
	stimer4.enable=0;
	stimer5.enable=0;
	stimer0.count=0;
	stimer1.count=0;
	stimer2.count=0;
	stimer3.count=0;
	stimer4.count=0;
	stimer5.count=0;
	stimer0.timeout=TIMECNT;
	stimer1.timeout=TIMECNT;
	stimer2.timeout=TIMECNT;
	stimer3.timeout=TIMECNT;
	stimer4.timeout=TIMECNT;
	stimer5.timeout=TIMECNT;


}


void timer_start(etimer_def *etimer)
{
	etimer->count=0;
	etimer->enable=1;
}

void timer_stop(etimer_def *etimer)
{
	etimer->enable=0;
	etimer->count=0;
}
















