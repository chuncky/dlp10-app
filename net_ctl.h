#ifndef _NET_CTL_H
#define _NET_CTL_H


//extern unsigned char Gateway_IP[4];			/* Gateway IP */
//extern unsigned char Sub_Mask[4];	   	  /* Subnet mask */
//extern unsigned char IP_Addr[4];		    /* W5200 IP */
//extern unsigned char Phy_Addr[6];       //MAC Address
//extern unsigned char S_Port[2];   		  /* Port Number */
extern char ipAddress[40];
extern char submaskAddress[40];
extern char gwAddress[40];
extern int port;
extern char macAddress[40];
extern char statusStr[256];

/* Network parameter registers */
extern char Gateway_IP[4];			/* Gateway IP */
extern char Sub_Mask[4];	   	    /* Subnet mask */
extern char IP_Addr[4];		    /* W5200 IP */
extern char Phy_Addr[6];//MAC Address
extern char S_Port[2];   				/* Port Number */


extern char netstatus_buf[256];

int convertIp(char* ip, char* pData );
int convertMac(char* mac, char* pData );

#ifdef __cplusplus
 extern "C"
{
 #endif

int netdisplayconvert(char * netbuf);
#ifdef __cplusplus

}
 #endif

void net_init(void);

int net_thread_init(void);
int net_thread_join(void);



#endif
