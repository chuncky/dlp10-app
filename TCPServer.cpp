//#include "stdafx.h"

#include     <stdio.h>
#include     <stdlib.h>
#include "TCPServer.h"
#include "linuxdef.h"
#include "fifo.h"
#include "uart.h"
#include "net_ctl.h"


#include "ctl_cmd.h"
#include "matrix.h"
#include "can.h"
#include "status_file.h"
//extern "C"
//{
//u8 Write_Cmd_FIFO(u8* pSource,u32 WriteLength,__FIFO * fifo_ptr);
//}

char pStatus[256]; 
int prepare_netstatus(void)
{
	int lenth;
	lenth=sprintf(pStatus,"IP:%s Sub_Mask:%s Gateway:%s MAC:%s PORT:%05d ",ipAddress,submaskAddress,gwAddress,macAddress,port);

        //printf("\n---lenth=%d,pStatus=%s---\n",lenth,pStatus);
	return lenth;

}
int prepare_matrixstatus(void)
{
	char *buf;
	int lenth=0;
	int i=0;
	int offset =0;
        buf=pStatus;
	while(i<iMax_Output_Num){
		offset=sprintf(buf,"%d,",(iMtxState[i]+1));

		buf+=offset;
		lenth+=offset;
		i++;
	}
        //printf("\n---lenth=%d,pStatus=%s---\n",lenth,pStatus);
	return lenth;

}


int select_checkcommand(char *cmd)
{
	if (cmd[0]!=0x23)
		return -1;
	else if(cmd[1]!=0x2a)
		return -1;
	else if(cmd[2]!=0x00)
		return -1;
	else if(cmd[3]!=0x00)
		return -1;
	else if(cmd[5]!=0xff)
		return -1;
	else if (cmd[4]==0x03)
		return 0;
	else if (cmd[4]==0x0e)
		return 1;
	else return -1;

}


void send_n(void)
{




}




CTCPServer::CTCPServer(const std::string & ip, Poco::UInt16 port)
{
	_bQuit = false;

	_serverSocket.bind(port);
	_serverSocket.listen();
}


CTCPServer::~CTCPServer(void)
{
}

void CTCPServer::run()
{
	while(!_bQuit)
	{
		StreamSocket streamSocket = _serverSocket.acceptConnection();

		//printf( "acceptConnection: 0x%d \n", &streamSocket );

		char* pCommand = new char[256];

		int ret;

		for( int i = 0; i < 256; i ++ )
			*(pCommand + i) = 0;

//		unsigned char command[16] = {0};
		int count = streamSocket.receiveBytes( pCommand, 256 );
		printf( "\n RECEIVE:%d|(0x%x,0x%x,0x%x,0x%x,0x%x,0x%x)\n", count,pCommand[0],pCommand[1],pCommand[2],pCommand[3],pCommand[4],pCommand[5]);

		Write_Cmd_FIFO(pCommand,count,&NETFIFO);

		ret=select_checkcommand(pCommand);

		if (ret==-1)
			count = streamSocket.sendBytes( "RETURN", 6 );
		else if (ret== 0)
		{
			//printf("ctcpserver-02\n");
			ret=prepare_matrixstatus();
			count = streamSocket.sendBytes( pStatus, ret );
		}
		else if (ret== 1)
		{	
			//printf("ctcpserver-03\n");
			ret=prepare_netstatus();
			count = streamSocket.sendBytes( pStatus, ret);
		}

		delete pCommand;
	
	}
}

void CTCPServer::setQuit()
{
	_bQuit = true;
}
