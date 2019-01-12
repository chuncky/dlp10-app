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
#include    <signal.h>
#include    <pthread.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <sys/mman.h>
#include "linuxdef.h"
#include "fifo.h"
#include "uart.h"
#include "matrix.h"
#include "can.h"
#include "net_ctl.h"
#include "ctl_cmd.h"
#include "status_file.h"
#include <pthread.h>
#include <bits/pthreadtypes.h>
pthread_rwlock_t status_rwlock;


#define SW8x8_MAX_OUTPUT_NUM 8
#define SW10x10_MAX_OUTPUT_NUM 10
#define SW20x20_MAX_OUTPUT_NUM 20
#define SW16x16_MAX_OUTPUT_NUM 16
#define SW36x36_MAX_OUTPUT_NUM 36

unsigned char iMax_Input_Num,iMax_Output_Num;

unsigned char SW8x8_iMtxState[SW8x8_MAX_OUTPUT_NUM +3] = 
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,MACHINE_ID,0x02,0x55}; //mode+logo_visible
// unsigned char iMtxState[iMax_Output_Num +3] = 
// {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,MACHINE_ID,0x02,0x55}; //mode+logo_visible


char Software_version[]  		= { "Matrix Switcher V4.0" };	//
char *DEVICE_NAME;				   		//
char *DEVICE_NAME_STATIC;

char SW8x8_DEVICE_NAME[8]					= { "DHS0808" }; 				   		//
char SW8x8_DEVICE_NAME_STATIC[8]		= { "DHS0808" };


Switch_Info SV8x8_info={
	.version=Software_version,
	.device_name=SW8x8_DEVICE_NAME,
	.device_name_static=SW8x8_DEVICE_NAME_STATIC,
	.iMtxState=SW8x8_iMtxState,
	.output_maxnum=8,
	.input_maxnum=8,

};
char SW10x10_DEVICE_NAME[8]					= { "DHS1010" }; 				   		//
char SW10x10_DEVICE_NAME_STATIC[8]		= { "DHS1010" };

unsigned char SW10x10_iMtxState[SW10x10_MAX_OUTPUT_NUM +3] = 
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,MACHINE_ID,0x02,0x55}; //mode+logo_visible

Switch_Info SV10x10_info={
	.version=Software_version,
	.device_name=SW10x10_DEVICE_NAME,
	.device_name_static=SW10x10_DEVICE_NAME_STATIC,
	.iMtxState=SW10x10_iMtxState,
	.output_maxnum=10,
	.input_maxnum=10,

};
char SW20x20_DEVICE_NAME[8]					= { "DHS2020" }; 				   		//脡猫卤赂脙没鲁脝
char SW20x20_DEVICE_NAME_STATIC[8]		= { "DHS2020" };				 

unsigned char SW20x20_iMtxState[SW20x20_MAX_OUTPUT_NUM +3] = 
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
MACHINE_ID,0x02,0x55}; //mode+logo_visible
// unsigned char iMtxState[iMax_Output_Num +3] = 
// {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,MACHINE_ID,0x02,0x55}; //mode+logo_visible

Switch_Info SV20x20_info={
	.version=Software_version,
	.device_name=SW20x20_DEVICE_NAME,
	.device_name_static=SW20x20_DEVICE_NAME_STATIC,
	.iMtxState=SW20x20_iMtxState,
	.output_maxnum=20,
	.input_maxnum=20,

};
char SW16x16_DEVICE_NAME[8]					= { "DHS1616" }; 				   		//脡猫卤赂脙没鲁脝
char SW16x16_DEVICE_NAME_STATIC[8]		= { "DHS1616" };				 

unsigned char SW16x16_iMtxState[SW16x16_MAX_OUTPUT_NUM +3] = 
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
MACHINE_ID,0x02,0x55}; //mode+logo_visible
// unsigned char iMtxState[iMax_Output_Num +3] = 
// {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,MACHINE_ID,0x02,0x55}; //mode+logo_visible


Switch_Info SV16x16_info={
	.version=Software_version,
	.device_name=SW16x16_DEVICE_NAME,
	.device_name_static=SW16x16_DEVICE_NAME_STATIC,
	.iMtxState=SW16x16_iMtxState,
	.output_maxnum=16,
	.input_maxnum=16,

};

char SW36x36_DEVICE_NAME[8]					= { "DHS3636" }; 				   		//脡猫卤赂脙没鲁脝
char SW36x36_DEVICE_NAME_STATIC[8]		= { "DHS3636" };				 

unsigned char SW36x36_iMtxState[SW36x36_MAX_OUTPUT_NUM +3] = 
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	MACHINE_ID,0x02,0x55}; //mode+logo_visible
// unsigned char iMtxState[iMax_Output_Num +3] = 
// {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,MACHINE_ID,0x02,0x55}; //mode+logo_visible


Switch_Info SV36x36_info={
	.version=Software_version,
	.device_name=SW36x36_DEVICE_NAME,
	.device_name_static=SW36x36_DEVICE_NAME_STATIC,
	.iMtxState=SW36x36_iMtxState,
	.output_maxnum=36,
	.input_maxnum=36,

};
Switch_Info *sw_info;

char tmp_buffer[1024];				 

#define SWITCH_IOCTL_MAGIC		'S'

#define VIDEO_SWITCH_GETTYPE          	_IOWR(SWITCH_IOCTL_MAGIC, 20, unsigned int)	// get device type
#define VIDEO_SWITCH_SETONECHANNEL	_IOW(SWITCH_IOCTL_MAGIC, 30, unsigned int)	// Change One Video
//#define VIDEO_SWITCH_GETONECHANNEL	_IOR(SWITCH_IOCTL_MAGIC, 31, unsigned int)	//get One Video


int vs_fd;

char  video_dev[]= "/dev/video_switch";


int device_type;

unsigned char iMtxState_buff[256];

unsigned char *iMtxState;



unsigned char bSavedMtx[MAX_SAVE_STATE_NUM]   = { 0, 0, 0, 0 };
unsigned int  iSaveMtxAddr[MAX_SAVE_STATE_NUM] = {0x1f4,0x3e8,0x5dc,0x7d0}; 


void DCS_ALL_Init(void)
{
	printf("%s-01\n",__func__);
}

#if 0
void Up_Config_All_Data(void)
{
	printf("%s-01\n",__func__);

}
#endif

void Change_One_Video(unsigned int Out_Channel,unsigned int Input_Channel) //脟脨禄禄碌楼脗路脢脫脝碌
{
	//Up_Config_One_Data(Out_Channel,Input_Channel);
        int data;

	//printf("%s-01:IN %d to OUT %d\n",__func__,Input_Channel,Out_Channel);
	data =Out_Channel<<16;
	data |=Input_Channel;

	ioctl(vs_fd,VIDEO_SWITCH_SETONECHANNEL,&data);

}



void Change_All_Video(void)//脟脨禄禄脣霉脫脨脥篓碌脌脢脫脝碌
{
	memcpy(iMtxState_buff,iMtxState,sw_info->output_maxnum);
	ioctl(vs_fd,VIDEO_SWITCH_SETALLCHANNEL,iMtxState_buff);

}

void Change_Audio(void)//脟脨禄禄脪么脝碌
{

}

void Change_All(void)//脟脨禄禄脢脫脝碌潞脥脪么脝碌
{
		Change_All_Video();
		Change_Audio();
}

int Matrix_init(void)
{
	int res;
	printf("open video_switch....\n");
	if((vs_fd=open(video_dev,O_RDWR | O_NOCTTY)) < 0 )
	{
   		printf("open video_switch %s fail \n ",video_dev);
   		return vs_fd;
	}

	ioctl(vs_fd,VIDEO_SWITCH_GETDEVICE,&device_type);
	printf("%s:device_type=0x%x\n",__func__,device_type);
	if (device_type==SV_8X8){
		printf("%s:sv8x8=0x%x\n",__func__,device_type);
		sw_info=&SV8x8_info;
	}
	else if (device_type==SV_10X10){
		printf("%s:sv10x10=0x%x\n",__func__,device_type);
		sw_info=&SV10x10_info;
	}
	else if(device_type==SV_20X20){
		printf("%s:sv20x20=0x%x\n",__func__,device_type);
		sw_info=&SV20x20_info;
	}
	else if(device_type==SV_16X16){
		printf("%s:sv16x16=0x%x\n",__func__,device_type);
		sw_info=&SV16x16_info;
	}
	else if(device_type==SV_36X36){
		printf("%s:sv36x36=0x%x\n",__func__,device_type);
		sw_info=&SV36x36_info;
	}
	else {

		printf("%s:unknow device type type=0x%x\n",__func__,device_type);
		exit(-1);
	
	}
//printf("%s-02\n",__func__);
	iMtxState=sw_info->iMtxState;
	iMax_Input_Num=sw_info->input_maxnum;
	iMax_Output_Num=sw_info->output_maxnum;
	DEVICE_NAME=sw_info->device_name;
	DEVICE_NAME_STATIC=sw_info->device_name_static;
//printf("%s-03\n",__func__);
	LoadDataFromFile();
//printf("%s-04\n",__func__);

    	res=pthread_rwlock_init(&status_rwlock,NULL);
    	if (res != 0)
    	{
        	printf("rwlock initialization failed");

    	}
	DCS_ALL_Init();	
	Change_All();
	return 0;
}





//Ax! =>戮脴脮贸鹿忙赂帽
void proc_rs_Device_Hex(Ctl_Cmd_Info* cmd_info)
{

  		cmd_info->uart_putchar( iMtxState[INDEX_MACHINE_ID] );
  		cmd_info->uart_putchar( 0x3a );

		if(iMtxState[LOGO_VISIBLE] == 0x55)
  		cmd_info->uart_puts( DEVICE_NAME );
	

}

void proc_rs_Device(int data,Ctl_Cmd_Info* cmd_info)
{
	if(data==0x3f)
	{
			cmd_info->uart_putchar( iMtxState[INDEX_MACHINE_ID] );
			cmd_info->uart_putchar( 0x3a );
			cmd_info->uart_puts( DEVICE_NAME );
	}
}

////in->all  A01!
void proc_rs_SwchALL(Ctl_Cmd_Info* cmd_info)
{
	u16 k,i;
	i = (cmd_info->Buffer_Data[1]-'0')*10+(cmd_info->Buffer_Data[2]-'0');
	i--;
		
   if(i < iMax_Input_Num)
	{
		pthread_rwlock_wrlock(&status_rwlock); 
		for(k=0;k<iMax_Output_Num;k++)
		{
			iMtxState[k]=i;	
		}
		save_status();
		Change_All();
		pthread_rwlock_unlock(&status_rwlock);	
	}
}
void proc_rs_SwchALL_extron(Ctl_Cmd_Info* cmd_info)
{
	u16 k,i;
	i = (cmd_info->Buffer_Data[0]-'0');
	i--;
		
  if(i < iMax_Input_Num)
	{
		pthread_rwlock_wrlock(&status_rwlock); 
		for(k=0;k<iMax_Output_Num;k++)
		{
			iMtxState[k]=i;	
		}
		save_status();
		Change_All();
		pthread_rwlock_unlock(&status_rwlock);	
	}
}

void proc_rs_SwchCH_extron(Ctl_Cmd_Info* cmd_info)
{
		unsigned char i,iNdx;
		cmd_info->Buffer_Data[0] -= 0x30;
		cmd_info->Buffer_Data[2] -= 0x30;


		iNdx = cmd_info->Buffer_Data[2];
		i    = cmd_info->Buffer_Data[0];
		i--;
		iNdx--;
	
		if ( (i < iMax_Input_Num) && (iNdx < (iMax_Output_Num)) )
		{
			pthread_rwlock_wrlock(&status_rwlock); 
			 iMtxState[iNdx] = i;  	  	  	 			   //Video
			 Change_One_Video(iNdx,i);
			 save_status();
			pthread_rwlock_unlock(&status_rwlock);	
		}
		
}
////脟脨禄禄脠芦虏驴脦陋脥鲁脪禄脥篓碌脌
void proc_rs_SwchALL_Hex(int i,Ctl_Cmd_Info* cmd_info)
{
  unsigned char k;

	if(i < iMax_Input_Num)
	{
		pthread_rwlock_wrlock(&status_rwlock); 
		for(k=0;k<iMax_Output_Num;k++)
		{
			iMtxState[k]=i;	
		}
		save_status();
		Change_All();
		pthread_rwlock_unlock(&status_rwlock);	
		cmd_info->uart_puts("OK");
	
  }
		
	
}

//////////////////////////
//Aoo?ii! =>戮脴脮贸脟脨禄禄 ?=</(/[/{=ALL/VGA/Video/Audio
void proc_rs_SwchCH(Ctl_Cmd_Info* cmd_info)
{
		unsigned char i,iNdx;
		cmd_info->Buffer_Data[1] -= 0x30;
		cmd_info->Buffer_Data[2] -= 0x30;
		cmd_info->Buffer_Data[4] -= 0x30;
		cmd_info->Buffer_Data[5] -= 0x30;

		iNdx = cmd_info->Buffer_Data[1]*10+cmd_info->Buffer_Data[2];
		i    = cmd_info->Buffer_Data[4]*10+cmd_info->Buffer_Data[5];
		i--;
		iNdx--;	
			
		if ((cmd_info->Buffer_Data[3]==0x5b)||(cmd_info->Buffer_Data[3]==0x3c)) //脢脫脝碌碌楼脗路脟脨禄禄
		{
			 
			if ( (i < iMax_Input_Num) && (iNdx < (iMax_Output_Num)) )
		 	{
				pthread_rwlock_wrlock(&status_rwlock);
		   	   	 iMtxState[iNdx] = i;  	  	  	 			   //Video
				 Change_One_Video(iNdx,i);
				 save_status();
				pthread_rwlock_unlock(&status_rwlock);	
		  	} 
			
		}
							 
}


//////////////////////////
//Aoo?ii! =>戮脴脮贸脟脨禄禄 ?=</(/[/{=ALL/VGA/Video/Audio
void proc_rs_SwchCH_Hex(char *data,u16 len,Ctl_Cmd_Info* cmd_info)
{
	unsigned char i,iNdx;
	unsigned int n;
	pthread_rwlock_wrlock(&status_rwlock); 
	for(n=0;n<len;n+=3)
	{
		iNdx = *(data+n);
		i = *(data+n+1);
	
		if((*(data+n+2)==0x56)||(*(data+n+2)==0x46))//脢脫脝碌脟脨禄禄
		{
		 	if ( (i < iMax_Input_Num) && (iNdx < (iMax_Output_Num)) )
			{
				  iMtxState[iNdx] = i;  	  	  	 			   //Video
				  Change_One_Video(iNdx,i);	  
			}
		}
	}
	save_status();
	cmd_info->uart_puts("OK");
	pthread_rwlock_unlock(&status_rwlock);			 	
  		
}



void proc_rs_ID(Ctl_Cmd_Info* cmd_info)
{
     if ( (cmd_info->Buffer_Data[1]==0x3C)&&(cmd_info->Buffer_Data[2]==0x49)&&(cmd_info->Buffer_Data[3]==0x44)&&(cmd_info->Buffer_Data[4]==0x3A)&&(cmd_info->Buffer_Data[6]==0x3E) )   //*<ID:*>!
		 {
			pthread_rwlock_wrlock(&status_rwlock); 
		    	iMtxState[INDEX_MACHINE_ID] = cmd_info->Buffer_Data[5];
			save_configfile();
			pthread_rwlock_unlock(&status_rwlock);
			cmd_info->uart_puts("OK"); 
		 }
}

void proc_rs_ID_Hex(char temp)
{
	pthread_rwlock_wrlock(&status_rwlock);       
  	iMtxState[INDEX_MACHINE_ID] = temp;
	save_configfile();
	pthread_rwlock_unlock(&status_rwlock);
		  
}



//====================================================//

void exteral_ddc_do(char* ptr,unsigned char In_num)
{
		int i;
		
		for(i=0;i<256;i++)
		{
// 			IN_I2C_Write(*(ptr+i),i); 
		}
		

}


//A!  =>戮脴脮贸脳麓脤卢	
void proc_rs_State(Ctl_Cmd_Info* cmd_info)
{
	unsigned char i;
	char cmdbuf[256];
	if(cmd_info == &uart2_can_info)
	{
		CAN_proc_rs_State();
		return ;
	}
	cmd_info->uart_putchar(0x23);
	cmd_info->uart_putchar(iMtxState[INDEX_MACHINE_ID]);
	if((iMax_Input_Num>=100)||(iMax_Output_Num>=100))
	{
		cmd_info->uart_putchar(iMax_Output_Num*4/256);
		cmd_info->uart_putchar(iMax_Output_Num*4%256);
	}
	else
	{
		cmd_info->uart_putchar(iMax_Output_Num*3/256);
		cmd_info->uart_putchar(iMax_Output_Num*3%256);					
	}
	cmd_info->uart_putchar(0x1E);
    	for (i=0; i<iMax_Output_Num; i++)//MAX_OUTPUT_NUM脗路脢盲鲁枚
	{
		
		sprintf(cmdbuf,"%02d[",(iMtxState[i]+1));
		cmd_info->uart_puts(cmdbuf);
	}
	cmd_info->uart_putchar(0xff);		 
}

int parse_cmd_progress(Ctl_Cmd_Info* cmd_info)
{
	int cmd;
	char cmdbuf[256];
	char * pbuf;
	u16 i,count;
	u32 tmp = 0;
	u32 len;
	cmd=cmd_info->Buffer_Data[4];
	switch (cmd)
	{
		case CMD_CHANGE_DEVICEID: 
			if(cmd_info->Buffer_Data_Len==1)
			{
				proc_rs_ID_Hex(cmd_info->Buffer_Data[5]);
				cmd_info->uart_puts("OK");
				
			}
			return 0;
			break;
		case CMD_CHECK_DEVICEID:
			if(cmd_info->Buffer_Data_Len==0)
				proc_rs_Device_Hex(cmd_info);
			return 0;
			break;
		case CMD_CHECK_STATUS:

			if(cmd_info->Buffer_Data_Len==0){
				proc_rs_State(cmd_info);
			}
			return 0;
			break;
		case CMD_BYTE_CHANNELSWTICH:
			if(cmd_info->Buffer_Data_Len%3==0)
			{
				proc_rs_SwchCH_Hex(&cmd_info->Buffer_Data[5],cmd_info->Buffer_Data_Len,cmd_info);
			}
			return 0;
			break;
		case CMD_CHANNELSWTICH_ALL:
			if((cmd_info->Buffer_Data_Len==1)&&(cmd_info->Buffer_Data[0+5]<iMax_Input_Num))
			{
				proc_rs_SwchALL_Hex(cmd_info->Buffer_Data[5],cmd_info);
			}
			return 0;
			break;
		case CMD_SAVE_STATUS:
			if(cmd_info->Buffer_Data[5]==0||cmd_info->Buffer_Data[5]==1||cmd_info->Buffer_Data[5]==2||cmd_info->Buffer_Data[5]==3)
			{
				
				pthread_rwlock_wrlock(&status_rwlock); 	
			    	bSavedMtx[(int)(cmd_info->Buffer_Data[5])] = 1;					
				savematrixstatus(cmd_info->Buffer_Data[5]);
				pthread_rwlock_unlock(&status_rwlock);
				cmd_info->uart_puts("OK");
			}
			return 0;
			break;
		case CMD_LOAD_STATUS:
			if ( bSavedMtx[(int)(cmd_info->Buffer_Data[5])] )
			{
				pthread_rwlock_wrlock(&status_rwlock);
				loadmatrixstatus(cmd_info->Buffer_Data[5]);

	 			Change_All();
				save_status();
				pthread_rwlock_unlock(&status_rwlock);
				cmd_info->uart_puts("OK");
			} 
			return 0;
			break;
		case CMD_WRITE_DDC:
			if(cmd_info->Buffer_Data_Len==2&&(cmd_info->Buffer_Data[5]<=iMax_Input_Num)&&(cmd_info->Buffer_Data[6]==0x28||cmd_info->Buffer_Data[6]==0x45
				||cmd_info->Buffer_Data[6]==0x27||cmd_info->Buffer_Data[6]==0x25||cmd_info->Buffer_Data[6]==0x23||cmd_info->Buffer_Data[6]==0x19
				||cmd_info->Buffer_Data[6]==0x08||cmd_info->Buffer_Data[6]==0x36||cmd_info->Buffer_Data[6]==0x51||cmd_info->Buffer_Data[6]==0x52
				||cmd_info->Buffer_Data[6]==0x50||cmd_info->Buffer_Data[6]==0x55))
			{
// 				IWDG_ReloadCounter();
				cmd_info->uart_puts("Wait");
// 			   	 WRITE_SELECT_DDC(cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6]);		
				cmd_info->uart_puts("OK");
			}
			return 0;
			break;
		case CMD_CHANGE_DEVICETYPEDISPLAY:
			if((cmd_info->Buffer_Data_Len==1)&&((cmd_info->Buffer_Data[0+5]==0x00)||(cmd_info->Buffer_Data[0+5]==0x01)||(cmd_info->Buffer_Data[0+5]==0x55)))
			{
				pthread_rwlock_wrlock(&status_rwlock);
				if(cmd_info->Buffer_Data[0+5]==0x00)//²»ÏÔÊ¾
				{
					iMtxState[LOGO_VISIBLE] = 0xaa;
					save_statusfile();
					cmd_info->uart_puts("LOGO_UNVISIBLE");
				}
				else if(cmd_info->Buffer_Data[0+5]==0x01)//ÏÔÊ¾
				{
					iMtxState[LOGO_VISIBLE] = 0x55;
					save_statusfile();
					cmd_info->uart_puts("LOGO_VISIBLE");
				}
				else if(cmd_info->Buffer_Data[0+5]==0x55)//¸ü¸Äµ½Ô­Ê¼Ãû×Ö
				{
					DEVICE_NAME[0] = DEVICE_NAME_STATIC[0];
					DEVICE_NAME[1] = DEVICE_NAME_STATIC[1];
					DEVICE_NAME[2] = DEVICE_NAME_STATIC[2];
					DEVICE_NAME[3] = DEVICE_NAME_STATIC[3];
					DEVICE_NAME[4] = DEVICE_NAME_STATIC[4];
					DEVICE_NAME[5] = DEVICE_NAME_STATIC[5];
					DEVICE_NAME[6] = DEVICE_NAME_STATIC[6];
					DEVICE_NAME[7] = DEVICE_NAME_STATIC[7];
					cmd_info->uart_puts("NAME_CHANGE_TO_OLD");
					save_statusfile();
				}
				pthread_rwlock_unlock(&status_rwlock);
			}	
			return 0;
			break;
		case CMD_CHANGE_DEVICETYPE:
			if((cmd_info->Buffer_Data_Len<=7)&&(cmd_info->Buffer_Data_Len>=1))
			{
				pthread_rwlock_wrlock(&status_rwlock);
				if(cmd_info->Buffer_Data_Len==1)
				{
					DEVICE_NAME[0] = cmd_info->Buffer_Data[5];
					DEVICE_NAME[1] = 0;
					DEVICE_NAME[2] = 0;
					DEVICE_NAME[3] = 0;
					DEVICE_NAME[4] = 0;
					DEVICE_NAME[5] = 0;
					DEVICE_NAME[6] = 0;
					DEVICE_NAME[7] = 0;
					cmd_info->uart_puts("LOGO_CHANGE");
					save_statusfile();
				}
				else if(cmd_info->Buffer_Data_Len==2)
				{
					DEVICE_NAME[0] = cmd_info->Buffer_Data[5];
					DEVICE_NAME[1] = cmd_info->Buffer_Data[6];
					DEVICE_NAME[2] = 0;
					DEVICE_NAME[3] = 0;
					DEVICE_NAME[4] = 0;
					DEVICE_NAME[5] = 0;
					DEVICE_NAME[6] = 0;
					DEVICE_NAME[7] = 0;
					cmd_info->uart_puts("LOGO_CHANGE");
					save_statusfile();
				}
				else if(cmd_info->Buffer_Data_Len==3)
				{
					DEVICE_NAME[0] = cmd_info->Buffer_Data[5];
					DEVICE_NAME[1] = cmd_info->Buffer_Data[6];
					DEVICE_NAME[2] = cmd_info->Buffer_Data[7];
					DEVICE_NAME[3] = 0;
					DEVICE_NAME[4] = 0;
					DEVICE_NAME[5] = 0;
					DEVICE_NAME[6] = 0;
					DEVICE_NAME[7] = 0;
					cmd_info->uart_puts("LOGO_CHANGE");
					save_statusfile();
				}
				else if(cmd_info->Buffer_Data_Len==4)
				{
					DEVICE_NAME[0] = cmd_info->Buffer_Data[5];
					DEVICE_NAME[1] = cmd_info->Buffer_Data[6];
					DEVICE_NAME[2] = cmd_info->Buffer_Data[7];
					DEVICE_NAME[3] = cmd_info->Buffer_Data[8];
					DEVICE_NAME[4] = 0;
					DEVICE_NAME[5] = 0;
					DEVICE_NAME[6] = 0;
					DEVICE_NAME[7] = 0;
					cmd_info->uart_puts("LOGO_CHANGE");
					save_statusfile();
				}
				else if(cmd_info->Buffer_Data_Len==5)
				{
					DEVICE_NAME[0] = cmd_info->Buffer_Data[5];
					DEVICE_NAME[1] = cmd_info->Buffer_Data[6];
					DEVICE_NAME[2] = cmd_info->Buffer_Data[7];
					DEVICE_NAME[3] = cmd_info->Buffer_Data[8];
					DEVICE_NAME[4] = cmd_info->Buffer_Data[9];
					DEVICE_NAME[5] = 0;
					DEVICE_NAME[6] = 0;
					DEVICE_NAME[7] = 0;
					cmd_info->uart_puts("LOGO_CHANGE");
					save_statusfile();
				}
				else if(cmd_info->Buffer_Data_Len==6)
				{
					DEVICE_NAME[0] = cmd_info->Buffer_Data[5];
					DEVICE_NAME[1] = cmd_info->Buffer_Data[6];
					DEVICE_NAME[2] = cmd_info->Buffer_Data[7];
					DEVICE_NAME[3] = cmd_info->Buffer_Data[8];
					DEVICE_NAME[4] = cmd_info->Buffer_Data[9];
					DEVICE_NAME[5] = cmd_info->Buffer_Data[10];
					DEVICE_NAME[6] = 0;
					DEVICE_NAME[7] = 0;
					cmd_info->uart_puts("LOGO_CHANGE");
					save_statusfile();
				}
				else if(cmd_info->Buffer_Data_Len==7)
				{
					DEVICE_NAME[0] = cmd_info->Buffer_Data[5];
					DEVICE_NAME[1] = cmd_info->Buffer_Data[6];
					DEVICE_NAME[2] = cmd_info->Buffer_Data[7];
					DEVICE_NAME[3] = cmd_info->Buffer_Data[8];
					DEVICE_NAME[4] = cmd_info->Buffer_Data[9];
					DEVICE_NAME[5] = cmd_info->Buffer_Data[10];
					DEVICE_NAME[6] = cmd_info->Buffer_Data[11];
					DEVICE_NAME[7] = 0;
					cmd_info->uart_puts("LOGO_CHANGE");
					save_statusfile();									
				}
				pthread_rwlock_unlock(&status_rwlock);
			}	


			return 0;
			break;
		case CMD_CHANGE_NETSETTING:

			if(cmd_info->Buffer_Data_Len == 12)
			{ 
				pthread_rwlock_wrlock(&status_rwlock);
				sprintf(ipAddress,"%d.%d.%d.%d", cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6],cmd_info->Buffer_Data[7],cmd_info->Buffer_Data[8]);	
				//printf("%s\n",ipAddress);
				sprintf(submaskAddress,"%d.%d.%d.%d", cmd_info->Buffer_Data[9],cmd_info->Buffer_Data[10],cmd_info->Buffer_Data[11],cmd_info->Buffer_Data[12]);	
				//printf("%s\n",submaskAddress);					
				sprintf(gwAddress,"%d.%d.%d.%d", cmd_info->Buffer_Data[13],cmd_info->Buffer_Data[14],cmd_info->Buffer_Data[15],cmd_info->Buffer_Data[16]);	
				//printf("%s\n",gwAddress);
				//sprintf(cmdbuf,"IP:%s Sub_Mask:%s Gateway:%s ",ipAddress,submaskAddress,gwAddress);

				pbuf=cmdbuf;
				len=0;
				tmp=sprintf(pbuf,"IP:%03d.%03d.%03d.%03d ",cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6],cmd_info->Buffer_Data[7],cmd_info->Buffer_Data[8]);	
				len+=tmp;
				pbuf+=tmp;
				tmp=sprintf(pbuf,"Sub_Mask:%03d.%03d.%03d.%03d ",cmd_info->Buffer_Data[9],cmd_info->Buffer_Data[10],cmd_info->Buffer_Data[11],cmd_info->Buffer_Data[12]);
				len+=tmp;
				pbuf+=tmp;
				tmp=sprintf(pbuf,"Gateway:%03d.%03d.%03d.%03d ",cmd_info->Buffer_Data[13],cmd_info->Buffer_Data[14],cmd_info->Buffer_Data[15],cmd_info->Buffer_Data[16]);
				len+=tmp;
				cmdbuf[len]='\0';
				cmd_info->uart_puts(cmdbuf);	

				save_netconfig();
				pthread_rwlock_unlock(&status_rwlock);
				//net_init();	

			}

			return 0;
			break;
		case CMD_CHANGE_UDPPORT:
			if(cmd_info->Buffer_Data_Len == 2)
			{
				pthread_rwlock_wrlock(&status_rwlock);
				//sprintf(port,"%02x%02x",cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6]);
				port=(cmd_info->Buffer_Data[5]*256+cmd_info->Buffer_Data[6]);
                                pbuf=cmdbuf;
				sprintf(pbuf,"PORT:%04d",port);
				//printf("%d\n",port);
				cmd_info->uart_puts(cmdbuf);
				save_netconfig();
				//net_init();	
				pthread_rwlock_unlock(&status_rwlock);
			}
			return 0;
			break;
		case CMD_CHANGE_MACADDR:
			if(cmd_info->Buffer_Data_Len == 6)
			{	
				pthread_rwlock_wrlock(&status_rwlock);														
				sprintf(macAddress,"%02x:%02x:%02x:%02x:%02x:%02x", cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6],cmd_info->Buffer_Data[7],cmd_info->Buffer_Data[8],
						cmd_info->Buffer_Data[9],cmd_info->Buffer_Data[10]);

				pbuf=cmdbuf;
				len=sprintf(pbuf,"MAC:%03d-%03d-%03d-%03d-%03d-%03d",cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6],cmd_info->Buffer_Data[7],cmd_info->Buffer_Data[8],
						cmd_info->Buffer_Data[9],cmd_info->Buffer_Data[10]);	
				cmdbuf[len]='\0';	
				cmd_info->uart_puts(cmdbuf);
				//printf("MAC: %s\n",macAddress);
				save_netconfig();
				pthread_rwlock_unlock(&status_rwlock);
				//net_init();
	
			}
			return 0;
			break;
		case CMD_DISPLAY_NET:
			if(cmd_info->Buffer_Data_Len == 0)
			{
#if 0																		
				
				convertIp(ipAddress, IP_Addr);
				convertIp(submaskAddress, Sub_Mask);
				convertIp(gwAddress, Gateway_IP);
				convertMac(macAddress, Phy_Addr);

				cmd_info->uart_putchar((IP_Addr[0]/100)%10+'0');			
				cmd_info->uart_putchar((IP_Addr[0]/10)%10+'0');												
				cmd_info->uart_putchar((IP_Addr[0]/1)%10+'0');	
				cmd_info->uart_putchar('.');	
				cmd_info->uart_putchar((IP_Addr[1]/100)%10+'0');			
				cmd_info->uart_putchar((IP_Addr[1]/10)%10+'0');												
				cmd_info->uart_putchar((IP_Addr[1]/1)%10+'0');	
				cmd_info->uart_putchar('.');	
				cmd_info->uart_putchar((IP_Addr[2]/100)%10+'0');			
				cmd_info->uart_putchar((IP_Addr[2]/10)%10+'0');												
				cmd_info->uart_putchar((IP_Addr[2]/1)%10+'0');	
				cmd_info->uart_putchar('.');	
				cmd_info->uart_putchar((IP_Addr[3]/100)%10+'0');			
				cmd_info->uart_putchar((IP_Addr[3]/10)%10+'0');												
				cmd_info->uart_putchar((IP_Addr[3]/1)%10+'0');	
				cmd_info->uart_putchar(' ');
				cmd_info->uart_puts("Sub_Mask:");
				cmd_info->uart_putchar((Sub_Mask[0]/100)%10+'0');			
				cmd_info->uart_putchar((Sub_Mask[0]/10)%10+'0');												
				cmd_info->uart_putchar((Sub_Mask[0]/1)%10+'0');	
				cmd_info->uart_putchar('.');	
				cmd_info->uart_putchar((Sub_Mask[1]/100)%10+'0');			
				cmd_info->uart_putchar((Sub_Mask[1]/10)%10+'0');												
				cmd_info->uart_putchar((Sub_Mask[1]/1)%10+'0');	
				cmd_info->uart_putchar('.');	
				cmd_info->uart_putchar((Sub_Mask[2]/100)%10+'0');			
				cmd_info->uart_putchar((Sub_Mask[2]/10)%10+'0');												
				cmd_info->uart_putchar((Sub_Mask[2]/1)%10+'0');	
				cmd_info->uart_putchar('.');	
				cmd_info->uart_putchar((Sub_Mask[3]/100)%10+'0');			
				cmd_info->uart_putchar((Sub_Mask[3]/10)%10+'0');												
				cmd_info->uart_putchar((Sub_Mask[3]/1)%10+'0');	
				cmd_info->uart_putchar(' ');
				cmd_info->uart_puts("Gateway:");
				cmd_info->uart_putchar((Gateway_IP[0]/100)%10+'0');			
				cmd_info->uart_putchar((Gateway_IP[0]/10)%10+'0');												
				cmd_info->uart_putchar((Gateway_IP[0]/1)%10+'0');	
				cmd_info->uart_putchar('.');	
				cmd_info->uart_putchar((Gateway_IP[1]/100)%10+'0');			
				cmd_info->uart_putchar((Gateway_IP[1]/10)%10+'0');												
				cmd_info->uart_putchar((Gateway_IP[1]/1)%10+'0');	
				cmd_info->uart_putchar('.');	
				cmd_info->uart_putchar((Gateway_IP[2]/100)%10+'0');			
				cmd_info->uart_putchar((Gateway_IP[2]/10)%10+'0');												
				cmd_info->uart_putchar((Gateway_IP[2]/1)%10+'0');	
				cmd_info->uart_putchar('.');	
				cmd_info->uart_putchar((Gateway_IP[3]/100)%10+'0');			
				cmd_info->uart_putchar((Gateway_IP[3]/10)%10+'0');												
				cmd_info->uart_putchar((Gateway_IP[3]/1)%10+'0');	
				cmd_info->uart_putchar(' ');	
				cmd_info->uart_puts("MAC:");
				cmd_info->uart_putchar((Phy_Addr[0]/100)%10+'0');
				cmd_info->uart_putchar((Phy_Addr[0]/10)%10+'0');										
				cmd_info->uart_putchar((Phy_Addr[0]/1)%10+'0');
				cmd_info->uart_putchar('-');
				cmd_info->uart_putchar((Phy_Addr[1]/100)%10+'0');
				cmd_info->uart_putchar((Phy_Addr[1]/10)%10+'0');										
				cmd_info->uart_putchar((Phy_Addr[1]/1)%10+'0');
				cmd_info->uart_putchar('-');
				cmd_info->uart_putchar((Phy_Addr[2]/100)%10+'0');
				cmd_info->uart_putchar((Phy_Addr[2]/10)%10+'0');										
				cmd_info->uart_putchar((Phy_Addr[2]/1)%10+'0');
				cmd_info->uart_putchar('-');
				cmd_info->uart_putchar((Phy_Addr[3]/100)%10+'0');
				cmd_info->uart_putchar((Phy_Addr[3]/10)%10+'0');										
				cmd_info->uart_putchar((Phy_Addr[3]/1)%10+'0');
				cmd_info->uart_putchar('-');
				cmd_info->uart_putchar((Phy_Addr[4]/100)%10+'0');
				cmd_info->uart_putchar((Phy_Addr[4]/10)%10+'0');										
				cmd_info->uart_putchar((Phy_Addr[4]/1)%10+'0');
				cmd_info->uart_putchar('-');
				cmd_info->uart_putchar((Phy_Addr[5]/100)%10+'0');
				cmd_info->uart_putchar((Phy_Addr[5]/10)%10+'0');										
				cmd_info->uart_putchar((Phy_Addr[5]/1)%10+'0');
				cmd_info->uart_putchar(' ');
                                pbuf=cmdbuf;
				sprintf(pbuf,"PORT:%05d",port);	

#endif

				len=netdisplayconvert(cmdbuf);
				//len=sprintf(sbuf,"IP:%s Sub_Mask:%s Gateway:%s MAC:001-002-003-004-005-006 PORT:%05d",ipAddress,submaskAddress,gwAddress,port);
				cmdbuf[len]='\0';			
				cmd_info->uart_puts(cmdbuf);

#if 0
				cmd_info->uart_puts("IP:");
				cmd_info->uart_puts(ipAddress);
				cmd_info->uart_putchar(' ');
				cmd_info->uart_puts("Sub_Mask:");
				cmd_info->uart_puts(submaskAddress);
				cmd_info->uart_putchar(' ');			
				cmd_info->uart_puts("Gateway:");
				cmd_info->uart_puts(gwAddress);
				cmd_info->uart_putchar(' ');					

				cmd_info->uart_puts("MAC:");
				cmd_info->uart_puts(macAddress);
				cmd_info->uart_putchar(' ');	
											
                                pbuf=cmdbuf;
				sprintf(pbuf,"PORT:%05d",port);				
				cmd_info->uart_puts(cmdbuf);
#endif

                                pbuf=cmdbuf;
				sprintf(pbuf,"IP:%s         ",ipAddress);
				pbuf+=19;
				sprintf(pbuf,"PORT:%d",port);
				if(cmd_info == &uart2_can_info)
				{
					can_send_data(cmdbuf,28);
				}			
			}


			return 0;
			break;
		case CMD_NETRESET:
			if(cmd_info->Buffer_Data_Len == 0){

			printf("NET system reboot\n");
			system("reboot");
			system("sleep 3");

			}
			return 0;
			break;
		case 0X0F:
			count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;						
			can_send_data(cmd_info->Buffer_Data,count);
			return 0;
			break;
		case CMD_DDC_COPY:
			if(cmd_info->Buffer_Data_Len == 0x02)
			{				
// 				COPY_DDC(cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6]);
				cmd_info->uart_puts("OK");
			}
			return 0;
			break;
		case CMD_DDC_OUTPIXSEL:
			if(cmd_info->Buffer_Data_Len == 0x02)
			{		
				if(cmd_info->Buffer_Data[6] > 0)
				{										
// 					Data_CPLD(OUT_REVE_NUM[cmd_info->Buffer_Data[5]]); 
																						
					count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;										
// 					for(i=0;i<count;i++)
// 					{
// 						uart1_putchar(cmd_info->Buffer_Data[i]);
// 					}
					can_send_data(cmd_info->Buffer_Data,count);
// 					delay_ms(80);
// // 					Data_CPLD(0x100);	
 					cmd_info->uart_puts("OK");
 				}	
 			}

			return 0;
			break;
		case CMD_DDC_OUTMODE:
//  			CONFIG_4052(DDC_OUT_4052,UART_OUT_4052);
			if(cmd_info->Buffer_Data_Len == 0x02)
			{
				if((cmd_info->Buffer_Data[6] > 0) &&  (cmd_info->Buffer_Data[6] <=2 ) )
				{										
// 					Data_CPLD(OUT_REVE_NUM[cmd_info->Buffer_Data[5]]);
					count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;
					can_send_data(cmd_info->Buffer_Data,count);
// 					delay_ms(80);
// 					Data_CPLD(0x100);	
					cmd_info->uart_puts("OK");
				}
			}
			return 0;
			break;
		case 0X16:
			if(cmd_info->Buffer_Data_Len == (256 + 1))
			{				
				cmd_info->uart_puts("start");
				exteral_ddc_do(cmd_info->Buffer_Data+6,cmd_info->Buffer_Data[5]);
				cmd_info->uart_puts("ok!");
			}
			return 0;
			break;
		case 0X17:
// 			CONFIG_4052(DDC_OUT_4052,UART_IN_4052);
							
			if(cmd_info->Buffer_Data_Len == 0x02)
			{			
				if(cmd_info->Buffer_Data[6] > 0)
				{										
// 					Data_CPLD(IN_REVE_NUM[cmd_info->Buffer_Data[5]]); 											
					count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;
// 					for(i=0;i<count;i++)
// 					{
// 						uart1_putchar(cmd_info->Buffer_Data[i]);
// 					}
					can_send_data(cmd_info->Buffer_Data,count);
// 					delay_ms(80);
// 					Data_CPLD(0x100);
					cmd_info->uart_puts("OK");
				}
			}
 			//HC595_Config(CPLD_CE_L);
			return 0;
			break;
		case CMD_DDC_INMODE:
			if(cmd_info->Buffer_Data_Len == 0x02)
			{			
				if((cmd_info->Buffer_Data[6] > 0) &&  (cmd_info->Buffer_Data[6] <= 2 ) )
				{	
					count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;													
					can_send_data(cmd_info->Buffer_Data,count);										
					cmd_info->uart_puts("OK");
				}
			}		
 			//HC595_Config(CPLD_CE_L);
			return 0;
			break;
		case 0X1C:
			if((cmd_info->Buffer_Data_Len == 0x01) && ((cmd_info->Buffer_Data[5]+1) <= SPLICE_MODE_NUM))
			{	pthread_rwlock_wrlock(&status_rwlock);		
				save_splice_data(splice_data_buff,cmd_info->Buffer_Data[5]+1,MAX_CHANNEL_NUMS*5);
				pthread_rwlock_unlock(&status_rwlock);									
				cmd_info->uart_puts("OK");

			}
			return 0;
			break;
		case 0X1D:
			tmp=get_splice_num();
			////tmp²»Îª0Ê±´ú±í»úÆ÷Ö´ÐÐ¹ýÆ´½ÓÃüÁî,±£´æÓÐÆ´½ÓÄ£Ê½Êý¾Ý¡£
			if((cmd_info->Buffer_Data_Len == 0x01) && ((cmd_info->Buffer_Data[5]+1) <= SPLICE_MODE_NUM) && tmp)
			{	
				load_splice_data(splice_data_buff,cmd_info->Buffer_Data[5]+1,MAX_CHANNEL_NUMS*5);
				tmp_buffer[0]=0x23;
				tmp_buffer[1]=MACHINE_ID;
				tmp_buffer[2]=(MAX_CHANNEL_NUMS*5 / 0x100);
				tmp_buffer[3]=(MAX_CHANNEL_NUMS*5 % 0x100);
				tmp_buffer[4]=0x0f;
				for(i=0;i<MAX_CHANNEL_NUMS*5;i++)
				{
					tmp_buffer[5+i] = splice_data_buff[i];
				}	
				tmp_buffer[5+i] = 0xff;

				//IWDG_ReloadCounter();


				can_send_data(tmp_buffer,(MAX_CHANNEL_NUMS*5+6));
				pthread_rwlock_wrlock(&status_rwlock);
				save_splice_data(splice_data_buff,0,MAX_CHANNEL_NUMS*5);
				pthread_rwlock_unlock(&status_rwlock);						
#if DEBUG_MODE_ON													
				cmd_info->uart_putchar(0x23);											
				cmd_info->uart_putchar(MACHINE_ID);
				cmd_info->uart_putchar(MAX_CHANNEL_NUMS*5 / 0x100);
				cmd_info->uart_putchar(MAX_CHANNEL_NUMS*5 % 0x100);
				cmd_info->uart_putchar(0x0f);
				for(i=0;i<MAX_CHANNEL_NUMS*5;i++)
				{
					cmd_info->uart_putchar(splice_data_buff[i]);
				}	
				cmd_info->uart_putchar(0xff);
																									
													
#endif
				cmd_info->uart_puts("OK");	
// 				}
			}		
			return 0;
			break;
		case CMD_MATRIX_LOCK:
			if((!cmd_info->Buffer_Data_Len) && (cmd_info == &uart2_can_info))
			{
				//net_interrupt_op(OFF);
				//uart_interrupt_op(OFF);
				matrixlock_op(OFF);
			}					
			return 0;
			break;
		case CMD_MATRIX_DISLOCK:
			if((!cmd_info->Buffer_Data_Len) && (cmd_info == &uart2_can_info))
			{
				//net_interrupt_op(ON);
				//uart_interrupt_op(ON);
				matrixlock_op(ON);
			}
			return 0;
			break;

		default: 
			break;
	}	
			
	return 0;


}

int  proc_rs_State_buf(char *sbuf)
{
	char * pbuf;
	int len;
	u16 i;
	u32 tmp = 0;
	len=0;
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
}

int proc_rs_Device_buf(int data,char *sbuf)
{
	int len;
	char * pbuf;
	if(data==0x3f)
	{
			sbuf[0]=iMtxState[INDEX_MACHINE_ID];
			sbuf[1]=0x3a;
			pbuf=sbuf+2;
  			len=sprintf(pbuf,"%s", DEVICE_NAME);
			len=len+2;
			sbuf[len]='\0';
			printf("%s--%s\n",__func__,sbuf);
			return len;
	}
	return 0;
}
////in->all  A01!
void proc_rs_SwchALL_cmd(char * Buffer_Data)
{
	u16 k,i;
	i = (Buffer_Data[1]-'0')*10+(Buffer_Data[2]-'0');
	i--;
		
   if(i < iMax_Input_Num)
	{
		pthread_rwlock_wrlock(&status_rwlock); 
		for(k=0;k<iMax_Output_Num;k++)
		{
			iMtxState[k]=i;	
		}
		save_status();
		Change_All();
		pthread_rwlock_unlock(&status_rwlock);	
	}
}

//////////////////////////
//Aoo?ii! =>Â¾Ã˜Ã•Ã³Ã‡ÃÂ»Â» ?=</(/[/{=ALL/VGA/Video/Audio
void proc_rs_SwchCH_cmd(char * Buffer_Data)
{
		unsigned char i,iNdx;
		Buffer_Data[1] -= 0x30;
		Buffer_Data[2] -= 0x30;
		Buffer_Data[4] -= 0x30;
		Buffer_Data[5] -= 0x30;

		iNdx = Buffer_Data[1]*10+Buffer_Data[2];
		i    = Buffer_Data[4]*10+Buffer_Data[5];
		i--;
		iNdx--;	
			
		if ((Buffer_Data[3]==0x5b)||(Buffer_Data[3]==0x3c)) //ÃŠÃ“Ã†ÂµÂµÂ¥Ã‚Â·Ã‡ÃÂ»Â»
		{
			 
			if ( (i < iMax_Input_Num) && (iNdx < (iMax_Output_Num)) )
		 	{
				pthread_rwlock_wrlock(&status_rwlock);
		   	   	 iMtxState[iNdx] = i;  	  	  	 			   //Video
				 Change_One_Video(iNdx,i);
				 save_status();
				pthread_rwlock_unlock(&status_rwlock);	
		  	} 
			
		}
							 
}

void proc_rs_ID_cmd(char * Buffer_Data)
{
     if ( (Buffer_Data[1]==0x3C)&&(Buffer_Data[2]==0x49)&&(Buffer_Data[3]==0x44)&&(Buffer_Data[4]==0x3A)&&(Buffer_Data[6]==0x3E) )   //*<ID:*>!
		 {
			pthread_rwlock_wrlock(&status_rwlock); 
		    	iMtxState[INDEX_MACHINE_ID] = Buffer_Data[5];
			save_configfile();
			pthread_rwlock_unlock(&status_rwlock);
			//cmd_info->uart_puts("OK"); 
		 }
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
			return  proc_rs_State_buf(sbuf);
			break;//A!
		case 3: 
			return proc_rs_Device_buf(Buffer_Data[1],sbuf);
			break;//A?!

		case 4: 
			proc_rs_SwchALL_cmd(Buffer_Data);
			return 0; 
			break;//A16!
		case 7: 
			proc_rs_SwchCH_cmd(Buffer_Data);
			return 0; 
			break;//A16[16!
		case 8: 
			proc_rs_ID_cmd(Buffer_Data);
			return 0;
			break;//A<ID:B>!
							
		default: 
			pbuf=sbuf;
			len=sprintf(pbuf,"UDP command error");
			return len;
		}	
}
void proc_rs_SwchCH_Hex_cmd(char *data,u16 len)
{
	unsigned char i,iNdx;
	unsigned int n;
	pthread_rwlock_wrlock(&status_rwlock); 
	for(n=0;n<len;n+=3)
	{
		iNdx = *(data+n);
		i = *(data+n+1);
	
		if((*(data+n+2)==0x56)||(*(data+n+2)==0x46))//ÃŠÃ“Ã†ÂµÃ‡ÃÂ»Â»
		{
		 	if ( (i < iMax_Input_Num) && (iNdx < (iMax_Output_Num)) )
			{
				  iMtxState[iNdx] = i;  	  	  	 			   //Video
				  Change_One_Video(iNdx,i);	  
			}
		}
	}
	save_status();
	pthread_rwlock_unlock(&status_rwlock);			 	
  		
}

////Ã‡ÃÂ»Â»ÃˆÂ«Â²Â¿ÃŽÂªÃÂ³Ã’Â»ÃÂ¨ÂµÃ€
void proc_rs_SwchALL_Hex_cmd(int i)
{
  unsigned char k;

	if(i < iMax_Input_Num)
	{
		pthread_rwlock_wrlock(&status_rwlock); 
		for(k=0;k<iMax_Output_Num;k++)
		{
			iMtxState[k]=i;	
		}
		save_status();
		Change_All();
		pthread_rwlock_unlock(&status_rwlock);	
  }
		
}


int parse_status_cmd(char * Buffer_Data,int lenth,char *sbuf)
{

	int cmd,len,Buffer_Data_Len;

	char * pbuf;
	u16 i,count;
	u32 tmp = 0;
	Buffer_Data_Len=Buffer_Data[2]*256+Buffer_Data[3];
	cmd=Buffer_Data[4];
	len=0;
	switch (cmd)
	{	case CMD_CHANGE_DEVICEID: 
			if(Buffer_Data_Len==1)
			{
				proc_rs_ID_Hex(Buffer_Data[5]);
				//cmd_info->uart_puts("OK");
			}
			return 0;
			break;
		case CMD_CHECK_DEVICEID:
			if(Buffer_Data_Len==0){
				sbuf[0]=iMtxState[INDEX_MACHINE_ID];
				sbuf[1]=0x3a;
				pbuf=sbuf+2;
				if(iMtxState[LOGO_VISIBLE] == 0x55)
  					len=sprintf(pbuf,"%s", DEVICE_NAME);
				len=len+2;
				sbuf[len]='\0';
				//printf("%s--%s\n",__func__,sbuf);
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
		case CMD_BYTE_CHANNELSWTICH:
			if(Buffer_Data_Len%3==0)
			{
				proc_rs_SwchCH_Hex_cmd(&Buffer_Data[5],Buffer_Data_Len);
			}
			return 0;
			break;

		case CMD_CHANNELSWTICH_ALL:
			if((Buffer_Data_Len==1)&&(Buffer_Data[0+5]<iMax_Input_Num))
			{
				proc_rs_SwchALL_Hex_cmd(Buffer_Data[5]);
			}
			return 0;
			break;
		case CMD_SAVE_STATUS:
			if(Buffer_Data[5]==0||Buffer_Data[5]==1||Buffer_Data[5]==2||Buffer_Data[5]==3)
			{
				pthread_rwlock_wrlock(&status_rwlock); 	
			    	bSavedMtx[(int)(Buffer_Data[5])] = 1;					
				savematrixstatus(Buffer_Data[5]);
				pthread_rwlock_unlock(&status_rwlock);
			}
			return 0;
			break;
		case CMD_LOAD_STATUS:
			if ( bSavedMtx[(int)(Buffer_Data[5])] )
			{
				pthread_rwlock_wrlock(&status_rwlock);
				loadmatrixstatus(Buffer_Data[5]);
	 			Change_All();
				save_status();
				pthread_rwlock_unlock(&status_rwlock);
			} 
			return 0;
			break;
		case CMD_WRITE_DDC:
			if(Buffer_Data_Len==2&&(Buffer_Data[5]<=iMax_Input_Num)&&(Buffer_Data[6]==0x28||Buffer_Data[6]==0x45
				||Buffer_Data[6]==0x27||Buffer_Data[6]==0x25||Buffer_Data[6]==0x23||Buffer_Data[6]==0x19
				||Buffer_Data[6]==0x08||Buffer_Data[6]==0x36||Buffer_Data[6]==0x51||Buffer_Data[6]==0x52
				||Buffer_Data[6]==0x50||Buffer_Data[6]==0x55))
			{
// 				IWDG_ReloadCounter();
				//cmd_info->uart_puts("Wait");
// 			   	 WRITE_SELECT_DDC(cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6]);		
				//cmd_info->uart_puts("OK");
			}
			return 0;
		case CMD_CHANGE_DEVICETYPEDISPLAY:
			if((Buffer_Data_Len==1)&&((Buffer_Data[0+5]==0x00)||(Buffer_Data[0+5]==0x01)||(Buffer_Data[0+5]==0x55)))
			{
				pthread_rwlock_wrlock(&status_rwlock);
				if(Buffer_Data[0+5]==0x00)//0…50†30ˆ30ˆ80‡80†6
				{
					iMtxState[LOGO_VISIBLE] = 0xaa;
					save_statusfile();
					len=sprintf(sbuf,"LOGO_UNVISIBLE");
					
				}
				else if(Buffer_Data[0+5]==0x01)//0ˆ30ˆ80‡80†6
				{
					iMtxState[LOGO_VISIBLE] = 0x55;
					save_statusfile();
					len=sprintf(sbuf,"LOGO_VISIBLE");
				}
				else if(Buffer_Data[0+5]==0x55)//0†0¨¹0†00‡20…80†50ˆ80…20‡80†40‡10‹4¡Á0‰0
				{
					DEVICE_NAME[0] = DEVICE_NAME_STATIC[0];
					DEVICE_NAME[1] = DEVICE_NAME_STATIC[1];
					DEVICE_NAME[2] = DEVICE_NAME_STATIC[2];
					DEVICE_NAME[3] = DEVICE_NAME_STATIC[3];
					DEVICE_NAME[4] = DEVICE_NAME_STATIC[4];
					DEVICE_NAME[5] = DEVICE_NAME_STATIC[5];
					DEVICE_NAME[6] = DEVICE_NAME_STATIC[6];
					DEVICE_NAME[7] = DEVICE_NAME_STATIC[7];
					len=sprintf(sbuf,"NAME_CHANGE_TO_OLD");
					save_statusfile();
				}
				pthread_rwlock_unlock(&status_rwlock);
			}	
			return len;
			break;
		case CMD_CHANGE_DEVICETYPE:
			if((Buffer_Data_Len<=7)&&(Buffer_Data_Len>=1))
			{
				pthread_rwlock_wrlock(&status_rwlock);
				if(Buffer_Data_Len==1)
				{
					DEVICE_NAME[0] = Buffer_Data[5];
					DEVICE_NAME[1] = 0;
					DEVICE_NAME[2] = 0;
					DEVICE_NAME[3] = 0;
					DEVICE_NAME[4] = 0;
					DEVICE_NAME[5] = 0;
					DEVICE_NAME[6] = 0;
					DEVICE_NAME[7] = 0;
					len=sprintf(sbuf,"LOGO_CHANGE");
					save_statusfile();
				}
				else if(Buffer_Data_Len==2)
				{
					DEVICE_NAME[0] = Buffer_Data[5];
					DEVICE_NAME[1] = Buffer_Data[6];
					DEVICE_NAME[2] = 0;
					DEVICE_NAME[3] = 0;
					DEVICE_NAME[4] = 0;
					DEVICE_NAME[5] = 0;
					DEVICE_NAME[6] = 0;
					DEVICE_NAME[7] = 0;
					len=sprintf(sbuf,"LOGO_CHANGE");
					save_statusfile();
				}
				else if(Buffer_Data_Len==3)
				{
					DEVICE_NAME[0] = Buffer_Data[5];
					DEVICE_NAME[1] = Buffer_Data[6];
					DEVICE_NAME[2] = Buffer_Data[7];
					DEVICE_NAME[3] = 0;
					DEVICE_NAME[4] = 0;
					DEVICE_NAME[5] = 0;
					DEVICE_NAME[6] = 0;
					DEVICE_NAME[7] = 0;
					len=sprintf(sbuf,"LOGO_CHANGE");
					save_statusfile();
				}
				else if(Buffer_Data_Len==4)
				{
					DEVICE_NAME[0] = Buffer_Data[5];
					DEVICE_NAME[1] = Buffer_Data[6];
					DEVICE_NAME[2] = Buffer_Data[7];
					DEVICE_NAME[3] = Buffer_Data[8];
					DEVICE_NAME[4] = 0;
					DEVICE_NAME[5] = 0;
					DEVICE_NAME[6] = 0;
					DEVICE_NAME[7] = 0;
					len=sprintf(sbuf,"LOGO_CHANGE");
					save_statusfile();
				}
				else if(Buffer_Data_Len==5)
				{
					DEVICE_NAME[0] = Buffer_Data[5];
					DEVICE_NAME[1] = Buffer_Data[6];
					DEVICE_NAME[2] = Buffer_Data[7];
					DEVICE_NAME[3] = Buffer_Data[8];
					DEVICE_NAME[4] = Buffer_Data[9];
					DEVICE_NAME[5] = 0;
					DEVICE_NAME[6] = 0;
					DEVICE_NAME[7] = 0;
					len=sprintf(sbuf,"LOGO_CHANGE");
					save_statusfile();
				}
				else if(Buffer_Data_Len==6)
				{
					DEVICE_NAME[0] = Buffer_Data[5];
					DEVICE_NAME[1] = Buffer_Data[6];
					DEVICE_NAME[2] = Buffer_Data[7];
					DEVICE_NAME[3] = Buffer_Data[8];
					DEVICE_NAME[4] = Buffer_Data[9];
					DEVICE_NAME[5] = Buffer_Data[10];
					DEVICE_NAME[6] = 0;
					DEVICE_NAME[7] = 0;
					len=sprintf(sbuf,"LOGO_CHANGE");
					save_statusfile();
				}
				else if(Buffer_Data_Len==7)
				{
					DEVICE_NAME[0] = Buffer_Data[5];
					DEVICE_NAME[1] = Buffer_Data[6];
					DEVICE_NAME[2] = Buffer_Data[7];
					DEVICE_NAME[3] = Buffer_Data[8];
					DEVICE_NAME[4] = Buffer_Data[9];
					DEVICE_NAME[5] = Buffer_Data[10];
					DEVICE_NAME[6] = Buffer_Data[11];
					DEVICE_NAME[7] = 0;
					len=sprintf(sbuf,"LOGO_CHANGE");
					save_statusfile();									
				}
				pthread_rwlock_unlock(&status_rwlock);
			}	


			return len;
			break;
		case CMD_CHANGE_NETSETTING:

			if(Buffer_Data_Len == 12)
			{ 
				pthread_rwlock_wrlock(&status_rwlock);
				sprintf(ipAddress,"%d.%d.%d.%d", Buffer_Data[5],Buffer_Data[6],Buffer_Data[7],Buffer_Data[8]);	
				//printf("%s\n",ipAddress);
				sprintf(submaskAddress,"%d.%d.%d.%d", Buffer_Data[9],Buffer_Data[10],Buffer_Data[11],Buffer_Data[12]);	
				//printf("%s\n",submaskAddress);					
				sprintf(gwAddress,"%d.%d.%d.%d", Buffer_Data[13],Buffer_Data[14],Buffer_Data[15],Buffer_Data[16]);	
				//printf("%s\n",gwAddress);
				//sprintf(cmdbuf,"IP:%s Sub_Mask:%s Gateway:%s ",ipAddress,submaskAddress,gwAddress);
				save_netconfig();
				pthread_rwlock_unlock(&status_rwlock);
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
				pthread_rwlock_wrlock(&status_rwlock);
				//sprintf(port,"%02x%02x",cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6]);
				port=(Buffer_Data[5]*256+Buffer_Data[6]);
				save_netconfig();
				pthread_rwlock_unlock(&status_rwlock);
                                pbuf=sbuf;
				len=sprintf(pbuf,"PORT:%04d",port);
				sbuf[len]='\0';
				printf("%s--%s\n",__func__,sbuf);

			}
			return len;
			break;
		case CMD_CHANGE_MACADDR:
			if(Buffer_Data_Len == 6)
			{	
				pthread_rwlock_wrlock(&status_rwlock);														
				sprintf(macAddress,"%02x:%02x:%02x:%02x:%02x:%02x", Buffer_Data[5],Buffer_Data[6],Buffer_Data[7],Buffer_Data[8],
						Buffer_Data[9],Buffer_Data[10]);
				save_netconfig();
				pthread_rwlock_unlock(&status_rwlock);
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
		case CMD_NETRESET:
			if(Buffer_Data_Len == 0){
			printf("NET system reboot\n");
			system("reboot");
			system("sleep 3");
			}
			return 0;
			break;
		case CMD_DDC_COPY:
			if(Buffer_Data_Len == 0x02)
			{				
// 				COPY_DDC(cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6]);
				//cmd_info->uart_puts("OK");
			}
			return 0;
			break;
		case CMD_DDC_OUTPIXSEL:
			if(Buffer_Data_Len == 0x02)
			{		
				if(Buffer_Data[6] > 0)
				{										
// 					Data_CPLD(OUT_REVE_NUM[cmd_info->Buffer_Data[5]]); 
					pthread_rwlock_wrlock(&status_rwlock);																		
					count = Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;										
// 					for(i=0;i<count;i++)
// 					{
// 						uart1_putchar(cmd_info->Buffer_Data[i]);
// 					}
					can_send_data(Buffer_Data,count);
// 					delay_ms(80);
// // 					Data_CPLD(0x100);	
 					//cmd_info->uart_puts("OK");
					pthread_rwlock_unlock(&status_rwlock);
 				}	
 			}

			return 0;
			break;
		case CMD_DDC_OUTMODE:
//  			CONFIG_4052(DDC_OUT_4052,UART_OUT_4052);
			if(Buffer_Data_Len == 0x02)
			{
				if((Buffer_Data[6] > 0) &&  (Buffer_Data[6] <=2 ) )
				{										
// 					Data_CPLD(OUT_REVE_NUM[cmd_info->Buffer_Data[5]]);
					pthread_rwlock_wrlock(&status_rwlock);
					count = Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;
					can_send_data(Buffer_Data,count);
// 					delay_ms(80);
// 					Data_CPLD(0x100);	
					//cmd_info->uart_puts("OK");
					pthread_rwlock_unlock(&status_rwlock);
				}
			}
			return 0;
			break;
		case 0X16:
			if(Buffer_Data_Len == (256 + 1))
			{	
				pthread_rwlock_wrlock(&status_rwlock);			
				//cmd_info->uart_puts("start");
				exteral_ddc_do(Buffer_Data+6,Buffer_Data[5]);
				//cmd_info->uart_puts("ok!");
				pthread_rwlock_unlock(&status_rwlock);
			}
			return 0;
			break;
		case 0X17:
// 			CONFIG_4052(DDC_OUT_4052,UART_IN_4052);
							
			if(Buffer_Data_Len == 0x02)
			{			
				if(Buffer_Data[6] > 0)
				{										
// 					Data_CPLD(IN_REVE_NUM[cmd_info->Buffer_Data[5]]); 
					pthread_rwlock_wrlock(&status_rwlock);						
					count = Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;
// 					for(i=0;i<count;i++)
// 					{
// 						uart1_putchar(cmd_info->Buffer_Data[i]);
// 					}
					can_send_data(Buffer_Data,count);
// 					delay_ms(80);
// 					Data_CPLD(0x100);
					pthread_rwlock_unlock(&status_rwlock);
					//cmd_info->uart_puts("OK");
				}
			}
 			//HC595_Config(CPLD_CE_L);
			return 0;
			break;
		case CMD_DDC_INMODE:
			if(Buffer_Data_Len == 0x02)
			{			
				if((Buffer_Data[6] > 0) &&  (Buffer_Data[6] <= 2 ) )
				{	
					count = Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;
					pthread_rwlock_wrlock(&status_rwlock);													
					can_send_data(Buffer_Data,count);
					pthread_rwlock_unlock(&status_rwlock);										
					//cmd_info->uart_puts("OK");
				}
			}		
 			//HC595_Config(CPLD_CE_L);
			return 0;
			break;
		case 0X1C:
			if((Buffer_Data_Len == 0x01) && ((Buffer_Data[5]+1) <= SPLICE_MODE_NUM))
			{	pthread_rwlock_wrlock(&status_rwlock);		
				save_splice_data(splice_data_buff,Buffer_Data[5]+1,MAX_CHANNEL_NUMS*5);
				pthread_rwlock_unlock(&status_rwlock);									
				//cmd_info->uart_puts("OK");

			}
			return 0;
			break;
		case 0X1D:
			tmp=get_splice_num();
			////tmp0…50†30ˆ20„900‡8¡À0…7¨²¡À¨ª0†3¨²0‡4¡Â0‰00…70ˆ40ˆ40†10‹50‡40…70†50ˆ70‡1¨¹0†90Š6,¡À0„50…70Š30ˆ70ˆ40‡40…70†50ˆ70‡20„50‡80†50‡80‹50†60‰60„30„5
			if((Buffer_Data_Len == 0x01) && ((Buffer_Data[5]+1) <= SPLICE_MODE_NUM) && tmp)
			{	
				load_splice_data(splice_data_buff,Buffer_Data[5]+1,MAX_CHANNEL_NUMS*5);
				tmp_buffer[0]=0x23;
				tmp_buffer[1]=MACHINE_ID;
				tmp_buffer[2]=(MAX_CHANNEL_NUMS*5 / 0x100);
				tmp_buffer[3]=(MAX_CHANNEL_NUMS*5 % 0x100);
				tmp_buffer[4]=0x0f;
				for(i=0;i<MAX_CHANNEL_NUMS*5;i++)
				{
					tmp_buffer[5+i] = splice_data_buff[i];
				}	
				tmp_buffer[5+i] = 0xff;

				//IWDG_ReloadCounter();


				can_send_data(tmp_buffer,(MAX_CHANNEL_NUMS*5+6));
				pthread_rwlock_wrlock(&status_rwlock);
				save_splice_data(splice_data_buff,0,MAX_CHANNEL_NUMS*5);
				pthread_rwlock_unlock(&status_rwlock);						
#if DEBUG_MODE_ON													
				cmd_info->uart_putchar(0x23);											
				cmd_info->uart_putchar(MACHINE_ID);
				cmd_info->uart_putchar(MAX_CHANNEL_NUMS*5 / 0x100);
				cmd_info->uart_putchar(MAX_CHANNEL_NUMS*5 % 0x100);
				cmd_info->uart_putchar(0x0f);
				for(i=0;i<MAX_CHANNEL_NUMS*5;i++)
				{
					cmd_info->uart_putchar(splice_data_buff[i]);
				}	
				cmd_info->uart_putchar(0xff);
																									
													
#endif
				//cmd_info->uart_puts("OK");	
// 				}
			}		
			return 0;
			break;
#if 0
		case CMD_MATRIX_LOCK:
			if((!Buffer_Data_Len) )
			{
				//net_interrupt_op(OFF);
				//uart_interrupt_op(OFF);
				matrixlock_op(OFF);
			}					
			return 0;
			break;
		case CMD_MATRIX_DISLOCK:
			if((!Buffer_Data_Len))
			{
				//net_interrupt_op(ON);
				//uart_interrupt_op(ON);
				matrixlock_op(ON);
			}
			return 0;
			break;
#endif

		default:
			return -1;
			break;
	}
}




















