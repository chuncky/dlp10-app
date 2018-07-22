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


#include "status_file.h"
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
char SW20x20_DEVICE_NAME[8]					= { "DHS2020" }; 				   		//Éè±¸Ãû³Æ
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
char SW16x16_DEVICE_NAME[8]					= { "DHS1616" }; 				   		//Éè±¸Ãû³Æ
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

char SW36x36_DEVICE_NAME[8]					= { "DHS3636" }; 				   		//Éè±¸Ãû³Æ
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

void Change_One_Video(unsigned int Out_Channel,unsigned int Input_Channel) //ÇÐ»»µ¥Â·ÊÓÆµ
{
	//Up_Config_One_Data(Out_Channel,Input_Channel);
        int data;

	//printf("%s-01:IN %d to OUT %d\n",__func__,Input_Channel,Out_Channel);
	data =Out_Channel<<16;
	data |=Input_Channel;

	ioctl(vs_fd,VIDEO_SWITCH_SETONECHANNEL,&data);

}



void Change_All_Video(void)//ÇÐ»»ËùÓÐÍ¨µÀÊÓÆµ
{
	memcpy(iMtxState_buff,iMtxState,sw_info->output_maxnum);
	ioctl(vs_fd,VIDEO_SWITCH_SETALLCHANNEL,iMtxState_buff);

}

void Change_Audio(void)//ÇÐ»»ÒôÆµ
{

}

void Change_All(void)//ÇÐ»»ÊÓÆµºÍÒôÆµ
{
		Change_All_Video();
		Change_Audio();
}

int Matrix_init(void)
{

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
	DCS_ALL_Init();	
	Change_All();
	return 0;
}





//Ax! =>¾ØÕó¹æ¸ñ
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
		for(k=0;k<iMax_Output_Num;k++)
		{
			iMtxState[k]=i;	
		}
		save_status();
		Change_All();
	}
}
void proc_rs_SwchALL_extron(Ctl_Cmd_Info* cmd_info)
{
	u16 k,i;
	i = (cmd_info->Buffer_Data[0]-'0');
	i--;
		
  if(i < iMax_Input_Num)
	{
		for(k=0;k<iMax_Output_Num;k++)
		{
			iMtxState[k]=i;	
		}
		save_status();
		Change_All();
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
			 iMtxState[iNdx] = i;  	  	  	 			   //Video
			 Change_One_Video(iNdx,i);
			 save_status();
		}
		
}
////ÇÐ»»È«²¿ÎªÍ³Ò»Í¨µÀ
void proc_rs_SwchALL_Hex(int i,Ctl_Cmd_Info* cmd_info)
{
  unsigned char k;

	if(i < iMax_Input_Num)
	{
		for(k=0;k<iMax_Output_Num;k++)
		{
			iMtxState[k]=i;	
		}
		save_status();
		Change_All();
		cmd_info->uart_puts("OK");
	
  }
		
	
}

//////////////////////////
//Aoo?ii! =>¾ØÕóÇÐ»» ?=</(/[/{=ALL/VGA/Video/Audio
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
			
		if ((cmd_info->Buffer_Data[3]==0x5b)||(cmd_info->Buffer_Data[3]==0x3c)) //ÊÓÆµµ¥Â·ÇÐ»»
		{
			if ( (i < iMax_Input_Num) && (iNdx < (iMax_Output_Num)) )
		 	{
		   	   	 iMtxState[iNdx] = i;  	  	  	 			   //Video
				 Change_One_Video(iNdx,i);
				 save_status();
		  } 
		}
							 
}


//////////////////////////
//Aoo?ii! =>¾ØÕóÇÐ»» ?=</(/[/{=ALL/VGA/Video/Audio
void proc_rs_SwchCH_Hex(char *data,u16 len,Ctl_Cmd_Info* cmd_info)
{
	unsigned char i,iNdx;
	unsigned int n;

	for(n=0;n<len;n+=3)
	{
		iNdx = *(data+n);
		i = *(data+n+1);
	
		if((*(data+n+2)==0x56)||(*(data+n+2)==0x46))//ÊÓÆµÇÐ»»
		{
		 	if ( (i < iMax_Input_Num) && (iNdx < (iMax_Output_Num)) )
			{
				  iMtxState[iNdx] = i;  	  	  	 			   //Video
				  Change_One_Video(iNdx,i);
				  save_status();
			}
		}

		cmd_info->uart_puts("OK");
	}				 	
  		
}



void proc_rs_ID(Ctl_Cmd_Info* cmd_info)
{

     if ( (cmd_info->Buffer_Data[1]==0x3C)&&(cmd_info->Buffer_Data[2]==0x49)&&(cmd_info->Buffer_Data[3]==0x44)&&(cmd_info->Buffer_Data[4]==0x3A)&&(cmd_info->Buffer_Data[6]==0x3E) )   //*<ID:*>!
		 {
		    	iMtxState[INDEX_MACHINE_ID] = cmd_info->Buffer_Data[5];
			save_configfile();
			cmd_info->uart_puts("OK");			 
		 }
	




}

void proc_rs_ID_Hex(char temp)
{
       
  	iMtxState[INDEX_MACHINE_ID] = temp;
	save_configfile();		  
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


























