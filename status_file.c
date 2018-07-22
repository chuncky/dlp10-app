
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
#include "libini.h"
#include "linuxdef.h"
#include "fifo.h"
#include "uart.h"
#include "matrix.h"



char inifile[]="/home/config.ini";
char matrixstatusfile[]="/home/matrixstatus.ini";
int fd_file;

char iMatrixName[40];
char iMatrixNameStatic[40];

int deviceId = 0;
int inCount = 0;
int outCount = 0;

int swtich_mode = 0;
int logo_visible =0;

char ipAddress[40] = {0};
char submaskAddress[40] = {0};
char gwAddress[40] = {0};
int port= 1234;
char macAddress[40] = {0};

char statusStr[256] = {0};

char savedmtx[40];


int splicenum = 0;

char statusStr0[256] = {0};
char statusStr1[256] = {0};
char statusStr2[256] = {0};
char statusStr3[256] = {0};

 
int save_netconfig(void)
{
	int ret;
	ini_fd_t fd_t = ini_open (inifile, "w","test only");
	ret = ini_locateHeading( fd_t, "Network");
	ret = ini_locateKey( fd_t, "Ip" );
	ret = ini_writeString( fd_t, ipAddress );
	ret = ini_locateKey( fd_t, "Mask" );
	ret = ini_writeString( fd_t, submaskAddress);
	ret = ini_locateKey( fd_t, "Gateway" );
	ret = ini_writeString( fd_t, gwAddress );
	ret = ini_locateKey( fd_t, "Port" );
	ret = ini_writeInt( fd_t, port );
	ret = ini_locateKey( fd_t, "Mac" );
	ret = ini_writeString( fd_t, macAddress );
	ini_flush(fd_t);
	ini_close(fd_t);
	return ret;
}
unsigned char status_test[256];
int statustoint(char *status, unsigned char * iStatus,char lenth)
{
	char *buf;
	int i=0;
	unsigned char temp;
        buf=status;
	while(i<lenth){
		temp=atoi(buf);
		iStatus[i]=temp;
		printf("iStatus[%d]=%d\n",i,iStatus[i]);
 		buf= strchr(buf,',');
		if (buf==0){
			printf("i=%d\n",i);
			break;
		}
		buf++;
		i++;
	}
	return i;
}



int statustochar(char *status, unsigned char * iStatus,char lenth)
{
	char *buf;
	int i=0;
	int offset =0;
        buf=status;
	while(i<lenth){
		offset=sprintf(buf,"%d,",iStatus[i]);

		buf+=offset;
		i++;
	}
   	//status[offset-1]='\n';
    	//printf(status);
	//printf("\n");
	return i;
}
int load_statusfile(void)
{

	ini_fd_t fd_t = ini_open (matrixstatusfile, "a","test only");

	int ret ;

#if 0
	
	ret = ini_locateKey( fd_t, "Id" );
	ret = ini_readInt( fd_t, &deviceId );
	//ret = ini_locateKey( fd_t, "In" );
	//ret = ini_readInt( fd_t, &inCount );
	//ret = ini_locateKey( fd_t, "Out" );
	//ret = ini_readInt( fd_t, &outCount );
	ret = ini_locateKey( fd_t, "Name" );
	ret = ini_readString( fd_t, iMatrixName, 256 );
#endif

	ret = ini_locateHeading( fd_t, "Status");
	ret = ini_locateKey( fd_t, "Name" );
	ret = ini_writeString( fd_t, iMatrixName);
	ret = ini_locateKey( fd_t, "SStr" );
	ret = ini_readString( fd_t, statusStr, 256 );


	ret = ini_locateKey( fd_t, "Switch_mode" );
	ret = ini_readInt( fd_t, &swtich_mode );
	ret = ini_locateKey( fd_t, "Logo_visible" );
	ret = ini_readInt( fd_t, &logo_visible );

	ret = ini_locateHeading( fd_t, "SavedStatus");
	ret = ini_locateKey( fd_t, "SavedMtx" );
	ret = ini_readString( fd_t, savedmtx, 40 );


	ret= ini_locateHeading( fd_t, "SpliceData");
	ret = ini_locateKey( fd_t, "Splicenum" );
	ret = ini_readInt( fd_t, &splicenum );

	ini_close(fd_t);
	return ret;

}

int save_statusfile(void)
{

	int ret;
	ini_fd_t fd_t = ini_open (matrixstatusfile, "w","test only");
#if 0
	ret = ini_locateHeading( fd_t, "Global");
	ret = ini_locateKey( fd_t, "Id" );
	ret = ini_writeInt( fd_t, deviceId );
	ret = ini_locateKey( fd_t, "In" );
	ret = ini_writeInt( fd_t, iMax_Input_Num );
	ret = ini_locateKey( fd_t, "Out" );
	ret = ini_writeInt( fd_t, iMax_Output_Num );
	ret = ini_locateKey( fd_t, "Name" );
	ret = ini_writeString( fd_t, iMatrixName);
#endif


	ret = ini_locateHeading( fd_t, "Status");
	ret = ini_locateKey( fd_t, "SStr" );
	ret = ini_writeString( fd_t, statusStr );
	ret = ini_locateKey( fd_t, "Name" );
	ret = ini_writeString( fd_t, iMatrixName);

	ret = ini_locateKey( fd_t, "Switch_mode" );
	ret = ini_writeInt( fd_t, swtich_mode );
	printf("swtich_mode=%d\n",swtich_mode);
	ret = ini_locateKey( fd_t, "Logo_visible" );
	ret = ini_writeInt( fd_t, logo_visible );
	printf("logo_visible=0x%x\n",logo_visible);

	ret = ini_locateHeading( fd_t, "SavedStatus");
	ret = ini_locateKey( fd_t, "SavedMtx" );
	ret = ini_writeString( fd_t, savedmtx);

	ret = ini_locateHeading( fd_t, "SpliceData");
	ret = ini_locateKey( fd_t, "Splicenum" );
	ret = ini_writeInt( fd_t, splicenum );

	ini_flush(fd_t);
	ini_close(fd_t);
	return ret;
}


int save_status(void)
{

	int ret;
	char statusStr_temp[256];
	ini_fd_t fd_t = ini_open (matrixstatusfile, "w","test only");

	statustochar(statusStr_temp,iMtxState,iMax_Output_Num);

	ret = ini_locateHeading( fd_t, "Status");
	ret = ini_locateKey( fd_t, "SStr" );
	ret = ini_writeString( fd_t, statusStr_temp);

	ini_flush(fd_t);
	ini_close(fd_t);
	return ret;
}

int load_configfile(void)
{

	ini_fd_t fd_t = ini_open (inifile, "a","test only");
	int ret = ini_locateHeading( fd_t, "Global");
	ret = ini_locateKey( fd_t, "Id" );
	ret = ini_readInt( fd_t, &deviceId );
	//ret = ini_locateKey( fd_t, "In" );
	//ret = ini_readInt( fd_t, &inCount );
	//ret = ini_locateKey( fd_t, "Out" );
	//ret = ini_readInt( fd_t, &outCount );
	//ret = ini_locateKey( fd_t, "NameStatic" );
	//ret = ini_readString( fd_t, iMatrixNameStatic, 256 );

	ret = ini_locateHeading( fd_t, "Network");
	ret = ini_locateKey( fd_t, "Ip" );
	ret = ini_readString( fd_t, ipAddress, 256 );

	ret = ini_locateKey( fd_t, "Mask" );
	ret = ini_readString( fd_t, submaskAddress, 256 );

	ret = ini_locateKey( fd_t, "Gateway" );
	ret = ini_readString( fd_t, gwAddress, 256 );

	ret = ini_locateKey( fd_t, "Port" );
	ret = ini_readInt( fd_t, &port);

	ret = ini_locateKey( fd_t, "Mac" );
	ret = ini_readString( fd_t, macAddress, 256 );
#if 0
	ret = ini_locateHeading( fd_t, "Status");
	ret = ini_locateKey( fd_t, "SStr" );
	ret = ini_readString( fd_t, statusStr, 256 );
	ret = ini_locateKey( fd_t, "Switch_mode" );
	ret = ini_readInt( fd_t, &swtich_mode );
	ret = ini_locateKey( fd_t, "Logo_visible" );
	ret = ini_readInt( fd_t, &logo_visible );

	ret = ini_locateHeading( fd_t, "SavedStatus");
	ret = ini_locateKey( fd_t, "SavedMtx" );
	ret = ini_readString( fd_t, savedmtx, 40 );

	ret = ini_locateKey( fd_t, "SStr0" );
	ret = ini_readString( fd_t, statusStr0, 256 );
	ret = ini_locateKey( fd_t, "SStr0" );
	ret = ini_readString( fd_t, statusStr0, 256 );
	ret = ini_locateKey( fd_t, "SStr1" );
	ret = ini_readString( fd_t, statusStr1, 256 );
	ret = ini_locateKey( fd_t, "SStr2" );
	ret = ini_readString( fd_t, statusStr2, 256 );
	ret = ini_locateKey( fd_t, "SStr3" );
	ret = ini_readString( fd_t, statusStr3, 256 );
#endif
	ini_close(fd_t);
	return ret;

}

int save_configfile(void)
{

	int ret;
	ini_fd_t fd_t = ini_open (inifile, "w","test only");




	deviceId=iMtxState[INDEX_MACHINE_ID];

	ret = ini_locateHeading( fd_t, "Global");
	ret = ini_locateKey( fd_t, "Id" );
	ret = ini_writeInt( fd_t, deviceId );
	ret = ini_locateKey( fd_t, "In" );
	ret = ini_writeInt( fd_t, iMax_Input_Num );
	ret = ini_locateKey( fd_t, "Out" );
	ret = ini_writeInt( fd_t, iMax_Output_Num );
	ret = ini_locateKey( fd_t, "NameStatic" );
	ret = ini_writeString( fd_t, iMatrixNameStatic);



	ret = ini_locateHeading( fd_t, "Network");
	ret = ini_locateKey( fd_t, "Ip" );
	ret = ini_writeString( fd_t, ipAddress );
	ret = ini_locateKey( fd_t, "Mask" );
	ret = ini_writeString( fd_t, submaskAddress);
	ret = ini_locateKey( fd_t, "Gateway" );
	ret = ini_writeString( fd_t, gwAddress );
	ret = ini_locateKey( fd_t, "Port" );
	ret = ini_writeInt( fd_t, port );
	ret = ini_locateKey( fd_t, "Mac" );
	ret = ini_writeString( fd_t, macAddress );
#if 0
	ret = ini_locateHeading( fd_t, "Status");
	ret = ini_locateKey( fd_t, "SStr" );
	ret = ini_writeString( fd_t, statusStr );

	ret = ini_locateKey( fd_t, "Switch_mode" );
	ret = ini_writeInt( fd_t, swtich_mode );
	printf("swtich_mode=%d\n",swtich_mode);
	ret = ini_locateKey( fd_t, "Logo_visible" );
	ret = ini_writeInt( fd_t, logo_visible );
	printf("logo_visible=0x%x\n",logo_visible);

	ret = ini_locateHeading( fd_t, "SavedStatus");
	ret = ini_locateKey( fd_t, "SavedMtx" );
	ret = ini_writeString( fd_t, savedmtx);

	ret = ini_locateKey( fd_t, "SStr0" );
	ret = ini_writeString( fd_t, statusStr0);
	ret = ini_locateKey( fd_t, "SStr1" );
	ret = ini_writeString( fd_t, statusStr1);
	ret = ini_locateKey( fd_t, "SStr2" );
	ret = ini_writeString( fd_t, statusStr2);
	ret = ini_locateKey( fd_t, "SStr3" );
	ret = ini_writeString( fd_t, statusStr3);

#endif

	ini_flush(fd_t);
	ini_close(fd_t);
	return ret;

}
int loadmatrixstatus(int index)
{
	ini_fd_t fd_t = ini_open (matrixstatusfile, "a","test only");
	int ret ;
	u16 i;
	char statusStr_temp[256];
	ret = ini_locateHeading( fd_t, "SavedStatus");

	switch(index)
	{
		case 0:
		ret = ini_locateKey( fd_t, "SStr0" );
		ret = ini_readString( fd_t, statusStr_temp,256);
		break;
		case 1:
		ret = ini_locateKey( fd_t, "SStr1" );
		ret = ini_readString( fd_t, statusStr_temp,256);
		break;
		case 2:	
		ret = ini_locateKey( fd_t, "SStr2" );
		ret = ini_readString( fd_t, statusStr_temp,256);
		break;
		case 3:
		ret = ini_locateKey( fd_t, "SStr3" );
		ret = ini_readString( fd_t, statusStr_temp,256);
		break;
		default:
                printf("index error\n");
		break;
	}
	ini_close(fd_t);
	statustoint(statusStr_temp,iMtxState,iMax_Output_Num);

	for (i=0; i<iMax_Output_Num; i++) 
	{
		if ( iMtxState[i] >= iMax_Input_Num )
		{
			iMtxState[i] = 0;
		}
	}
	return ret;

}

int savematrixstatus(int index)
{

	int ret;
	char statusStr_temp[256];
	ini_fd_t fd_t = ini_open (matrixstatusfile, "w","test only");

	statustochar(statusStr_temp,iMtxState,iMax_Output_Num);
	statustochar(savedmtx,bSavedMtx,MAX_SAVE_STATE_NUM);
	ret = ini_locateHeading( fd_t, "SavedStatus");
	ret = ini_locateKey( fd_t, "SavedMtx" );
	ret = ini_writeString( fd_t, savedmtx);
	switch(index)
	{
		case 0:
		ret = ini_locateKey( fd_t, "SStr0" );
		ret = ini_writeString( fd_t, statusStr_temp);
		break;
		case 1:
		ret = ini_locateKey( fd_t, "SStr1" );
		ret = ini_writeString( fd_t, statusStr_temp);
		break;
		case 2:	
		ret = ini_locateKey( fd_t, "SStr2" );
		ret = ini_writeString( fd_t, statusStr_temp);
		break;
		case 3:
		ret = ini_locateKey( fd_t, "SStr3" );
		ret = ini_writeString( fd_t, statusStr_temp);
		break;
		default:
                printf("index error\n");
		break;
	}

	ini_flush(fd_t);
	ini_close(fd_t);
	return ret;

}



void load_splice_data(char *splicedata,int num,int lenth)
{
	printf("%s:num=%d,lenth=%d\n",__func__,num,lenth);


}


void save_splice_data(char *splicedata,int num,int lenth)
{

	printf("%s:num=%d,lenth=%d\n",__func__,num,lenth);

}



int get_splice_num(void)
{
	return splicenum;


}
int set_splice_num(int num)
{
	splicenum=num;
	save_statusfile();
	return 0;


}



void LoadDataFromFile(void)
{
	u16 i;
printf("%s-01\n",__func__);
	load_configfile();
printf("%s-02\n",__func__);
	load_statusfile();
printf("%s-03\n",__func__);
	printf("%s\n",statusStr);
	statustoint(statusStr,iMtxState,iMax_Output_Num);
printf("%s-04\n",__func__);
	for (i=0; i<iMax_Output_Num; i++) 
	{
		if ( iMtxState[i] >= iMax_Input_Num )
		{
			iMtxState[i] = 0;
		}
	}
	statustochar(statusStr,iMtxState,iMax_Output_Num);
	iMtxState[INDEX_MACHINE_ID]=deviceId;

printf("%s-05\n",__func__);
	for(i=0;i<8;i++)
	{
		iMatrixNameStatic[i]= DEVICE_NAME_STATIC[i];
	}
	if ( (iMtxState[INDEX_MACHINE_ID]<0x41)||(iMtxState[INDEX_MACHINE_ID]>0x5a) )
	{
		iMtxState[INDEX_MACHINE_ID] = MACHINE_ID;
		deviceId=MACHINE_ID;
	}

	iMtxState[INDEX_SWITCH_MODE]=swtich_mode;
	if  ((iMtxState[INDEX_SWITCH_MODE]>3 )||(iMtxState[INDEX_SWITCH_MODE]==0 ))
	{
		   iMtxState[INDEX_SWITCH_MODE] = iMtxState_MODE;
			swtich_mode=iMtxState[INDEX_SWITCH_MODE];
	}
	iMtxState[LOGO_VISIBLE]=logo_visible;
	if  ((iMtxState[LOGO_VISIBLE]!=0x55 )&&(iMtxState[LOGO_VISIBLE]!=0xaa ))
	{
		   iMtxState[LOGO_VISIBLE] = 0x55;//ÏÔÊ¾
			logo_visible=iMtxState[LOGO_VISIBLE];
	}

	statustoint(savedmtx,bSavedMtx,MAX_SAVE_STATE_NUM);

	for (i=0; i<MAX_SAVE_STATE_NUM; i++)
	{
		if ( bSavedMtx[i] > 1 )
		{
			bSavedMtx[i] = 0;
		}
	}
	statustochar(savedmtx,bSavedMtx,MAX_SAVE_STATE_NUM);

	printf("iMax_Output_Num=%d,iMax_input_Num=%d\n",iMax_Output_Num,iMax_Input_Num);
	save_configfile();
	save_statusfile();

	printf("%s-end\n",__func__);

}






















