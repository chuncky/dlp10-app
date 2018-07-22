// PocoTcpServer.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <stdlib.h>
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/IPAddress.h"
#include "TCPServer.h"
#include "linuxdef.h"

#include "matrix.h"
#include "fifo.h"
#include "uart.h"
#include "net_ctl.h"
#include "can.h"
#include "status_file.h"
#include "ctl_cmd.h"
extern "C"
void * netprocess(void* arg)
{
	Poco::Thread _controllerThread;
	CTCPServer* _controllerRunnable;

	_controllerRunnable = new CTCPServer("127.0.0.1",1111);
	_controllerThread.start(*_controllerRunnable);	//传入对象而不是对象指针  

	while (true)
	{

	}

	return 0;
}
char buffer[1024];
char sendbuf[1024];

int checkcommand(char * buf,int lenth)
{
	int comlen;

	if((buf[0] == iMtxState[INDEX_MACHINE_ID])||(buf[0] == 0x2a)){
		if(buf[lenth-1]=='!')
			return 0;
		return -4;

	}

	if (buf[0]!=BATCHCMDSTR_STARTCHAR)
		return -1;
	if((buf[1]!=0x2a)&&(buf[1]!=iMtxState[INDEX_MACHINE_ID]))
		return -2;
	comlen=buf[2]*256+buf[3];
	if((comlen+6)!=lenth)
		return -3;
	if(buf[lenth-1]!=0xff)
		return -4;

	return 0;

}

int parse_ascii_cmd(char * Buffer_Data,int lenth,char *sbuf)
{
	int len;
	u16 i;
	char * pbuf;
	u32 tmp = 0;
	len=0;
	switch (lenth)
	{
		case 2: 
				sbuf[0]=0x23;
				sbuf[1]=iMtxState[INDEX_MACHINE_ID];
				if((iMax_Input_Num>=100)||(iMax_Output_Num>=100))
				{
					sbuf[2]=(iMax_Output_Num*4/256);
					sbuf[3]=(iMax_Output_Num*4%256);
				}
				else
				{
					sbuf[2]=(iMax_Output_Num*3/256);
					sbuf[3]=(iMax_Output_Num*3%256);					
				}
				sbuf[4]=0x1E;
				pbuf=sbuf+5;
				len=5;
    				for (i=0; i<iMax_Output_Num; i++)//MAX_OUTPUT_NUM脗路脢盲鲁枚
				{
		
					tmp=sprintf(pbuf,"%02d[",(iMtxState[i]+1));
					len+=tmp;
					pbuf+=tmp;
				}
				sbuf[len]=0xff;
				len++;
				sbuf[len]='\0';
				//printf("%s-cmd=%d,len=%d,--%s\n",__func__,cmd,len,(sbuf+5));
				return len;

			break;//A!
		case 3: 
			sbuf[0]=iMtxState[INDEX_MACHINE_ID];
			sbuf[1]=0x3a;
			pbuf=sbuf+2;
  			len=sprintf(pbuf,"%s", DEVICE_NAME);
			len=len+2;
			sbuf[len]='\0';
			printf("%s--%s\n",__func__,sbuf);
			return len;
			break;//A?!

		case 4: return -1; break;//A16!
		case 7: return -1; break;//A16[16!
		case 8: 
			return -1;
			break;//A<ID:B>!
							
		default: 
			pbuf=sbuf;
			len=sprintf(pbuf,"UDP command error");
			return len;
		}	


}



int parse_status_cmd(char * Buffer_Data,int lenth,char *sbuf)
{

	int cmd,len,Buffer_Data_Len;

	char * pbuf;
	u16 i;
	u32 tmp = 0;
	Buffer_Data_Len=Buffer_Data[2]*256+Buffer_Data[3];
	cmd=Buffer_Data[4];
	len=0;
	switch (cmd)
	{
		case CMD_CHECK_DEVICEID:
			if(Buffer_Data_Len==0){
				sbuf[0]=iMtxState[INDEX_MACHINE_ID];
				sbuf[1]=0x3a;
				pbuf=sbuf+2;
				if(iMtxState[LOGO_VISIBLE] == 0x55)
  					len=sprintf(pbuf,"%s", DEVICE_NAME);
				len=len+2;
				sbuf[len]='\0';
				printf("%s--%s\n",__func__,sbuf);
			}
			return len;
			break;

		case CMD_CHECK_STATUS:
			if(Buffer_Data_Len==0){
				sbuf[0]=0x23;
				sbuf[1]=iMtxState[INDEX_MACHINE_ID];
				if((iMax_Input_Num>=100)||(iMax_Output_Num>=100))
				{
					sbuf[2]=(iMax_Output_Num*4/256);
					sbuf[3]=(iMax_Output_Num*4%256);
				}
				else
				{
					sbuf[2]=(iMax_Output_Num*3/256);
					sbuf[3]=(iMax_Output_Num*3%256);					
				}
				sbuf[4]=0x1E;
				pbuf=sbuf+5;
				len=5;
    				for (i=0; i<iMax_Output_Num; i++)//MAX_OUTPUT_NUM脗路脢盲鲁枚
				{
		
					tmp=sprintf(pbuf,"%02d[",(iMtxState[i]+1));
					len+=tmp;
					pbuf+=tmp;
				}
				sbuf[len]=0xff;
				len++;
				sbuf[len]='\0';
				//printf("%s-cmd=%d,len=%d,--%s\n",__func__,cmd,len,(sbuf+5));
			}
			return len;
			break;

		case CMD_CHANGE_NETSETTING:

			if(Buffer_Data_Len == 12)
			{ 
				pbuf=sbuf;
				tmp=sprintf(pbuf,"IP:%03d.%03d.%03d.%03d ",Buffer_Data[5],Buffer_Data[6],Buffer_Data[7],Buffer_Data[8]);
				len+=tmp;
				pbuf+=tmp;
				tmp=sprintf(pbuf,"Sub_Mask:%03d.%03d.%03d.%03d ",Buffer_Data[9],Buffer_Data[10],Buffer_Data[11],Buffer_Data[12]);
				len+=tmp;
				pbuf+=tmp;
				tmp=sprintf(pbuf,"Gateway:%03d.%03d.%03d.%03d ",Buffer_Data[13],Buffer_Data[14],Buffer_Data[15],Buffer_Data[16]);
				len+=tmp;

				sbuf[len]='\0';
				printf("%s--%s\n",__func__,sbuf);

			}

			return len;
			break;
		case CMD_CHANGE_UDPPORT:
			if(Buffer_Data_Len == 2)
			{
				tmp=(Buffer_Data[5]*256+Buffer_Data[6]);
                                pbuf=sbuf;
				len=sprintf(pbuf,"PORT:%04d",tmp);
				sbuf[len]='\0';
				printf("%s--%s\n",__func__,sbuf);

			}
			return len;
			break;
		case CMD_CHANGE_MACADDR:
			if(Buffer_Data_Len == 6)
			{	
				pbuf=sbuf;
				len=sprintf(pbuf,"MAC:%03d-%03d-%03d-%03d-%03d-%03d",Buffer_Data[5],Buffer_Data[6],Buffer_Data[7],Buffer_Data[8],
						Buffer_Data[9],Buffer_Data[10]);	

				sbuf[len]='\0';
				printf("%s--%s\n",__func__,sbuf);
			}
			return len;
			break;

		case CMD_DISPLAY_NET:
			if(Buffer_Data_Len == 0)
			{																		
				len=netdisplayconvert(sbuf);
				//len=sprintf(sbuf,"IP:%s Sub_Mask:%s Gateway:%s MAC:001-002-003-004-005-006 PORT:%05d",ipAddress,submaskAddress,gwAddress,port);
				sbuf[len]='\0';
				printf("%s--%s\n",__func__,sbuf);
			}
			return len;
			break;






		default:
			return -1;
			break;
	}
}

extern "C"
void * udpprocess(void* arg)
{
	int iport=port;
	int  ret;
	printf("iport=%d\n",iport);
	Poco::Net::SocketAddress socketAddress(Poco::Net::IPAddress(), iport);
	Poco::Net::DatagramSocket datagramSocket(socketAddress);
	Poco::Net::SocketAddress sender;


	while (true)
    	{
        	int n = datagramSocket.receiveFrom(buffer, sizeof(buffer) - 1, sender);
        	buffer[n] = '\0';
        	//printf("receive data: %s\n",buffer);
		if(matrixlock){

			ret=checkcommand(buffer,n);
			if (ret<0){
        			datagramSocket.sendTo("Command error",13,sender);
				continue;
			}
			//printf("receive data: %s\n",buffer);
			if(buffer[0]==BATCHCMDSTR_STARTCHAR){
				ret=parse_status_cmd(buffer,n,sendbuf);
			}
			else // buf[0] == iMtxState[INDEX_MACHINE_ID])||(buf[0] == 0x2a)
			{
				ret=parse_ascii_cmd(buffer,n,sendbuf);
			}
			if (ret<0){
				datagramSocket.sendTo("OK",2,sender);
			}
			else 
				datagramSocket.sendTo(sendbuf,ret,sender);	
			Write_Cmd_FIFO(buffer,n,&UDPFIFO);
		}
		else{
			datagramSocket.sendTo("Matrix locked",12,sender);
		}
		//printf("command ok\n");
		//datagramSocket.sendTo("Command ok",10,sender);

    	}

    	return 0;
}
