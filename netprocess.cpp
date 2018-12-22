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
#include "ctl_cmd.h"
#include "uart.h"
#include "net_ctl.h"
#include "can.h"
#include "status_file.h"

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
			if (ret==0){
				datagramSocket.sendTo("OK",2,sender);
			}
			else if(ret<0)
				datagramSocket.sendTo("Error",5,sender);
			else 
				datagramSocket.sendTo(sendbuf,ret,sender);	
			//Write_Cmd_FIFO(buffer,n,&UDPFIFO);
		}
		else{
			datagramSocket.sendTo("Matrix locked",12,sender);
		}
		//printf("command ok\n");
		//datagramSocket.sendTo("Command ok",10,sender);

    	}

    	return 0;
}
