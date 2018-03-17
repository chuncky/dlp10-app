/****************************************************************************
 *                                                                          *
 * Copyright (c) 2017 V-tick Technology Corp. All rights reserved.         *
 *                                                                          *
 ****************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     matrix.c
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
#include "net_ctl.h"
#include "ctl_cmd.h"


/* Network parameter registers */
char Gateway_IP[4]={192,168,0,1};			/* Gateway IP */
char Sub_Mask[4]={255,255,255,0};	   	    /* Subnet mask */
char IP_Addr[4]={192,168,0,251};		    /* W5200 IP */
char Phy_Addr[6]={0x00, 0x08, 0xDC, 0x01, 0x02, 0x03};//MAC Address
char S_Port[2]={0x04,0xd2};   /* Port Number */


char netstatus_buf[256];

int convertIp(char* ip, char* pData )
{
	char *buf;
	int i=0;
	unsigned char temp;
        buf=ip;
	while(i<4){
		temp=atoi(buf);
		pData[i]=temp;
		//printf("pData[%d]=%d\n",i,pData[i]);
 		buf= strchr(buf,'.');
		if (buf==0){
			//printf("i=%d\n",i);
			break;
		}
		buf++;
		i++;
	}



	return i;
}

int getIndexOfSigns(char ch)
{
    if(ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    if(ch >= 'A' && ch <='F')
    {
        return ch - 'A' + 10;
    }
    if(ch >= 'a' && ch <= 'f')
    {
        return ch - 'a' + 10;
    }
    return -1;
}
long hexToDec(char *source)
{
    long sum = 0;
    long t = 1;
    int i, len;

    len = 2;
    for(i=len-1; i>=0; i--)
    {
        sum += t * getIndexOfSigns(*(source + i));
        t *= 16;
    }

    return sum;
}

int convertMac(char* mac, char* pData )
{
	char *buf;
	int i=0;
	unsigned char temp;
        buf=mac;
	while(i<6){
		temp=hexToDec(buf);
		pData[i]=temp;
		//printf("pData[%d]=%d\n",i,pData[i]);
 		buf= strchr(buf,':');
		if (buf==0){
			//printf("i=%d\n",i);
			break;
		}
		buf++;
		i++;
	}
	return i;
}

int netdisplayconvert(char * netbuf)
{
	int temp,len;
	char * pbuf;
	len=0;
	convertIp(ipAddress, IP_Addr);
	convertIp(submaskAddress, Sub_Mask);
	convertIp(gwAddress, Gateway_IP);
	convertMac(macAddress, Phy_Addr);
	pbuf=netbuf;
	temp=sprintf(pbuf,"IP:%03d.%03d.%03d.%03d ",IP_Addr[0],IP_Addr[1],IP_Addr[2],IP_Addr[3]);
	len+=temp;
	pbuf+=temp;
	temp=sprintf(pbuf,"Sub_Mask:%03d.%03d.%03d.%03d ",Sub_Mask[0],Sub_Mask[1],Sub_Mask[2],Sub_Mask[3]);
	len+=temp;
	pbuf+=temp;
	temp=sprintf(pbuf,"Gateway:%03d.%03d.%03d.%03d ",Gateway_IP[0],Gateway_IP[1],Gateway_IP[2],Gateway_IP[3]);
	len+=temp;
	pbuf+=temp;
	temp=sprintf(pbuf,"MAC:%03d-%03d-%03d-%03d-%03d-%03d ",Phy_Addr[0],Phy_Addr[1],Phy_Addr[2],Phy_Addr[3],Phy_Addr[4],Phy_Addr[5]);
	len+=temp;
	pbuf+=temp;
	temp=sprintf(pbuf,"PORT:%05d",port);
	len+=temp;
	pbuf+=temp;
	*pbuf=0;
	return len;
}





void * netprocess(void * arg);
void * udpprocess(void* arg);

//void w5200_init(void)
//{
//	printf("%s test\n",__func__);	
//}

void net_init(void)
{
	char cmdbuf[50];
	//printf("deviceId: 0x%x\n",deviceId); 
	//printf("inCount: 0x%x\n",inCount); 
	//printf("outCount: 0x%x\n",outCount); 
	printf("ipAddress: %s\n",ipAddress); 
	printf("submaskAddress: %s\n",submaskAddress); 
	printf("gwAddress: %s\n",gwAddress); 
	printf("port: %d\n",port); 

	printf("macAddress: %s\n",macAddress); 
	printf("statusStr: %s\n",statusStr); 
	//eth0 down
	sprintf(cmdbuf,"ifconfig eth0 down\n");
	//printf("%s\n",cmdbuf);
	system(cmdbuf);
	//set mac address
	sprintf(cmdbuf,"ifconfig eth0 hw ether %s\n", macAddress);
	//printf("%s\n",cmdbuf);
	system(cmdbuf);
	//set ip netmask
	sprintf(cmdbuf,"ifconfig eth0 %s netmask %s\n", ipAddress,submaskAddress);
	//printf("%s\n",cmdbuf);
	system(cmdbuf);
	//eth0 up
	sprintf(cmdbuf,"ifconfig eth0 up\n");
	printf("%s\n",cmdbuf);
	system(cmdbuf);
	//set hostname
	sprintf(cmdbuf,"hostname %s\n",ipAddress);
	//printf("%s\n",cmdbuf);
	system(cmdbuf);
	//set gw
	sprintf(cmdbuf,"route add default gw %s\n",gwAddress);
	//printf("%s\n",cmdbuf);
	system(cmdbuf);
	system("killall boa");
	system("boa");
}
void NET_to_flash(void)
{

	printf("%s test\n",__func__);	
	    
}


void net_interrupt_op(FunctionalState state)
{

	printf("%s test\n",__func__);	
	
}



pthread_t tcp_ctl_th,udp_ctl_th;


int net_thread_init(void)
{
	pthread_create(&tcp_ctl_th, NULL, netprocess, NULL);
	pthread_create(&udp_ctl_th, NULL, udpprocess, NULL);
	return 0;
}



int net_thread_join(void)
{
	pthread_join(tcp_ctl_th, NULL);
	pthread_join(udp_ctl_th, NULL);
	return 0;

}






