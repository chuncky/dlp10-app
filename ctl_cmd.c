/****************************************************************************
 *                                                                          *
 * Copyright (c) 2017 V-tick Technology Corp. All rights reserved.         *
 *                                                                          *
 ****************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     ctl_cmd.c
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

#include "matrix.h"
#include "fifo.h"
#include "uart.h"
#include "net_ctl.h"
#include "can.h"
#include "status_file.h"
#include "ctl_cmd.h"

#include "softtimer.h"
//============================================================================================//
#define COMMAND_HEAD_LEN 4//´®¿Ú16½øÖÆÃüÁî¸ñÊ½ÖÐ´ú±íÆðÊ¼·û£¬Éè±¸ID£¬Êý¾Ý¿é³¤¶È¹²Õ¼¾ÝµÄ×Ö½Ú¸öÊý
#define COMMAND_TYPE_LEN 1//´®¿Ú16½øÖÆÃüÁî¸ñÊ½ÖÐ´ú±íÃüÁîÖÖÀàÕ¼¾ÝµÄ×Ö½Ú¸öÊý
#define COMMAND_TERMINATOR_LEN 1//FF´®¿Ú16½øÖÆÃüÁî¸ñÊ½ÖÐ´ú±íÃüÁî½áÊø·ûÕ¼¾ÝµÄ×Ö½Ú¸öÊý
//=============================================================================================//

extern void proc_rs_State(Ctl_Cmd_Info* cmd_info);
extern void proc_rs_Device_Hex(Ctl_Cmd_Info* cmd_info);
extern void proc_rs_Device(int data,Ctl_Cmd_Info* cmd_info);
extern void proc_rs_SwchALL(Ctl_Cmd_Info* cmd_info);
extern void proc_rs_SwchALL_Hex(int i,Ctl_Cmd_Info* cmd_info);
extern void proc_rs_SwchCH(Ctl_Cmd_Info* cmd_info);
extern void proc_rs_SwchCH_Hex(char *data,u16 len,Ctl_Cmd_Info* cmd_info);
extern void proc_rs_ID(Ctl_Cmd_Info* cmd_info);

extern void proc_rs_ID_Hex(char temp);
char tmp_buffer[1024];

char splice_data_buff[1024];//´æ´¢Æ´½ÓÃüÁîÖÐµÄ0x0fÃüÁîºóµ½0xffÖ®¼äµÄÊý¾Ý
u16 splice_data_len[9];		  //[0]´ú±íµ±Ç°µÄÆ´½ÓÄ£Ê½Êý¾Ý³¤¶È,[1]--[8]Îª8¸öÆ´½ÓÄ£Ê½¸÷×ÔµÄÆ´½ÓÊý¾Ý³¤¶È

void proc_rs_Err(Ctl_Cmd_Info* cmd_info) 
{

		cmd_info->uart_puts("Uart Command lenth error\n");
}

//A!  =>¾ØÕó×´Ì¬	
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
    	for (i=0; i<iMax_Output_Num; i++)//MAX_OUTPUT_NUMÂ·Êä³ö
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
					
			    	bSavedMtx[(int)(cmd_info->Buffer_Data[5])] = 1;					
				savematrixstatus(cmd_info->Buffer_Data[5]);
				cmd_info->uart_puts("OK");
			}
			return 0;
			break;
		case CMD_LOAD_STATUS:
			if ( bSavedMtx[(int)(cmd_info->Buffer_Data[5])] )
			{
				loadmatrixstatus(cmd_info->Buffer_Data[5]);
	 			Change_All();
				save_status();
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
				if(cmd_info->Buffer_Data[0+5]==0x00)//�0�5�0�3�0�3�0�8�0�8�0�6
				{
					iMtxState[LOGO_VISIBLE] = 0xaa;
					save_statusfile();
					cmd_info->uart_puts("LOGO_UNVISIBLE");
				}
				else if(cmd_info->Buffer_Data[0+5]==0x01)//�0�3�0�8�0�8�0�6
				{
					iMtxState[LOGO_VISIBLE] = 0x55;
					save_statusfile();
					cmd_info->uart_puts("LOGO_VISIBLE");
				}
				else if(cmd_info->Buffer_Data[0+5]==0x55)//�0�0���0�0�0�2�0�8�0�5�0�8�0�2�0�8�0�4�0�1�0�4���0�0
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
			}	
			return 0;
			break;
		case CMD_CHANGE_DEVICETYPE:
			if((cmd_info->Buffer_Data_Len<=7)&&(cmd_info->Buffer_Data_Len>=1))
			{
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
			}	


			return 0;
			break;
		case CMD_CHANGE_NETSETTING:

			if(cmd_info->Buffer_Data_Len == 12)
			{ 

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
				//net_init();	

			}

			return 0;
			break;
		case CMD_CHANGE_UDPPORT:
			if(cmd_info->Buffer_Data_Len == 2)
			{

				//sprintf(port,"%02x%02x",cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6]);
				port=(cmd_info->Buffer_Data[5]*256+cmd_info->Buffer_Data[6]);
                                pbuf=cmdbuf;
				sprintf(pbuf,"PORT:%04d",port);
				//printf("%d\n",port);
				cmd_info->uart_puts(cmdbuf);
				save_netconfig();
				//net_init();	

			}
			return 0;
			break;
		case CMD_CHANGE_MACADDR:
			if(cmd_info->Buffer_Data_Len == 6)
			{	
																			
				sprintf(macAddress,"%02x:%02x:%02x:%02x:%02x:%02x", cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6],cmd_info->Buffer_Data[7],cmd_info->Buffer_Data[8],
						cmd_info->Buffer_Data[9],cmd_info->Buffer_Data[10]);

				pbuf=cmdbuf;
				len=sprintf(pbuf,"MAC:%03d-%03d-%03d-%03d-%03d-%03d",cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6],cmd_info->Buffer_Data[7],cmd_info->Buffer_Data[8],
						cmd_info->Buffer_Data[9],cmd_info->Buffer_Data[10]);	
				cmdbuf[len]='\0';	
				cmd_info->uart_puts(cmdbuf);
				//printf("MAC: %s\n",macAddress);
				save_netconfig();
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
			{			
				save_splice_data(splice_data_buff,cmd_info->Buffer_Data[5]+1,MAX_CHANNEL_NUMS*5);									
				cmd_info->uart_puts("OK");
			}
			return 0;
			break;
		case 0X1D:
			tmp=get_splice_num();
			////tmp�0�5�0�3�0�2�0�90�0�8���0�7�������0�3���0�4�0�0�0�7�0�4�0�4�0�1�0�5�0�4�0�7�0�5�0�7�0�1���0�9�0�6,���0�5�0�7�0�3�0�7�0�4�0�4�0�7�0�5�0�7�0�2�0�5�0�8�0�5�0�8�0�5�0�6�0�6�0�3�0�5
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
				save_splice_data(splice_data_buff,0,MAX_CHANNEL_NUMS*5);
										
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

//===================modify====================//
void handle_ctl_cmd(Ctl_Cmd_Info* cmd_info)
{
	u8 data;
	//u16 i,count;
	//char cmdbuf[256];
	//char * cmd;
	//u8 buff[30]={0};//===================modify====================//
	//u32 tmp = 0;
	//DMA_InitTypeDef DMA_InitStructure;

	
	while(Read_Cmd_FIFO(&data,1,cmd_info->fifo))
	{	
		//printf("0x%x,",data);			
		//GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_RESET);
		CLEAR_LED_CMD;

		if(cmd_info->Cmd_Type==0)
		{		
			if((data == iMtxState[INDEX_MACHINE_ID])||(data == 0x2a)) //Ô­Ê¼¿ØÖÆÃüÁî
			{
				cmd_info->Cmd_Type = 1;
			  	cmd_info->Buffer_LEN = 1;
				cmd_info->Buffer_Data[0] = data;
				//cmd_info->timer->CNT=0;
				//TIM_Cmd(cmd_info->timer, ENABLE);
				cmd_info->timer->count=0;
				timer_start(cmd_info->timer);
			}
			else if(data == BATCHCMDSTR_STARTCHAR)	//Åú´¦Àí¿ØÖÆÃüÁî
	 		{
	  	  		cmd_info->Cmd_Type = 2;
		  		cmd_info->Buffer_LEN = 1;
				cmd_info->Buffer_Data[0] = data;
				sett2flag(1);
				//cmd_info->timer->CNT=0;
				//TIM_Cmd(cmd_info->timer, ENABLE);
				cmd_info->timer->count=0;
				timer_start(cmd_info->timer);
	
	 		}
		}
		else if(cmd_info->Cmd_Type==1)//¿ØÖÆÖ¸Áî¶þ½ÓÊÕ
   		{
			//cmd_info->timer->CNT=0;
			cmd_info->timer->count=0;
			if(	cmd_info->Buffer_LEN>=BUFFER_LEN_MAX)
				cmd_info->Buffer_LEN = 0;
 			cmd_info->Buffer_Data[cmd_info->Buffer_LEN++] = data;
			if(data == '!')
			{
				//TIM_Cmd(cmd_info->timer, DISABLE);	
				timer_stop(cmd_info->timer);
				switch (cmd_info->Buffer_LEN)
				{
					case 2: proc_rs_State(cmd_info);break;//A!
					case 3: proc_rs_Device(cmd_info->Buffer_Data[1],cmd_info);break;//A?!
					case 4: proc_rs_SwchALL(cmd_info);break;//A16!
					case 7: proc_rs_SwchCH(cmd_info);break;//A16[16!
					case 8: proc_rs_ID(cmd_info);break;//A<ID:B>!
					default: proc_rs_Err(cmd_info);
				}	
				cmd_info->Cmd_Type = 0;							
			}

		}
		else if(cmd_info->Cmd_Type == 2)//Åú´¦ÀíÃüÁî½ÓÊÕ½ÓÊÕ
		{
			if(cmd_info->Buffer_LEN >= BUFFER_LEN_MAX)
				cmd_info->Buffer_LEN = 0;
			cmd_info->Buffer_Data[cmd_info->Buffer_LEN++] = data;
			if(cmd_info->Buffer_LEN == 2)
			{
// 				cmd_info->uart_putchar(cmd_info->Buffer_Data[1]);
// 				cmd_info->uart_putchar(iMtxState[INDEX_MACHINE_ID]);
				if((cmd_info->Buffer_Data[1] == iMtxState[INDEX_MACHINE_ID])||(cmd_info->Buffer_Data[1] ==0x2a))
				{
				}
				else
				{
					cmd_info->Cmd_Type = 0;	
					cmd_info->uart_puts("Device ID Error");						
				} 
			}
			if(cmd_info->Buffer_LEN == 4) //½ÓÊÕµ½Êý¾Ý¿é³¤¶È×Ö½Ú
			{
 				cmd_info->Buffer_Data_Len = cmd_info->Buffer_Data[2]*256+cmd_info->Buffer_Data[3];
				cmd_info->DATA_AVAILABLE = 1;
			}
			if(cmd_info->DATA_AVAILABLE) //µÈ´ýÅÐ¶Ï½ÓÊÕ ½áÊø·û0xff
			{	
							
				if((cmd_info->Buffer_Data[cmd_info->Buffer_Data_Len+5] == 0xff)&&(cmd_info->Buffer_LEN == cmd_info->Buffer_Data_Len+6 )) //½ÓÊÕµ½½áÊø·û
				{						
					//TIM_Cmd(cmd_info->timer, DISABLE);
					timer_stop(cmd_info->timer);
					sett2flag(0);
					//printf("\n Type2 command end\n");
					cmd_info->Cmd_Type = 0;	
					cmd_info->DATA_AVAILABLE = 0;
		     			parse_cmd_progress(cmd_info);
				}
			}
		}
	}
	//GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_SET);
	//cmd_data_led = 0;
	SET_LED_CMD;
}





#if 0
//===================modify====================//
void handle_ctl_cmd(Ctl_Cmd_Info* cmd_info)
{
	u8 data;
	u16 i,count;
	char cmdbuf[256];
	char * cmd;
	u8 buff[30]={0};//===================modify====================//
		u32 tmp = 0;
		//DMA_InitTypeDef DMA_InitStructure;

	
		if(Read_Cmd_FIFO(&data,1,cmd_info->fifo))
		{				
			//GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_RESET);
			if(cmd_info->Cmd_Type==0)
			{		
				if((data == iMtxState[INDEX_MACHINE_ID])||(data == 0x2a)) //Ô­Ê¼¿ØÖÆÃüÁî
				{
	  		  cmd_info->Cmd_Type = 1;
			  	cmd_info->Buffer_LEN = 1;
					
					cmd_info->Buffer_Data[0] = data;
					
					//cmd_info->timer->CNT=0;
					//TIM_Cmd(cmd_info->timer, ENABLE);			
				}
	    	else if(data == BATCHCMDSTR_STARTCHAR)	//Åú´¦Àí¿ØÖÆÃüÁî
	 			{
	  	  	cmd_info->Cmd_Type = 2;
		  		cmd_info->Buffer_LEN = 1;

					cmd_info->Buffer_Data[0] = data;

					//cmd_info->timer->CNT=0;
					//TIM_Cmd(cmd_info->timer, ENABLE);	
	 			}
			}
   		else if(cmd_info->Cmd_Type==1)//¿ØÖÆÖ¸Áî¶þ½ÓÊÕ
   		{	
				//cmd_info->timer->CNT=0;
				if(	cmd_info->Buffer_LEN>=BUFFER_LEN_MAX)
					cmd_info->Buffer_LEN = 0;

 				cmd_info->Buffer_Data[cmd_info->Buffer_LEN++] = data;
			
				if(data == '!')
				{
					//TIM_Cmd(cmd_info->timer, DISABLE);	

					switch (cmd_info->Buffer_LEN)
					{
							case 2: proc_rs_State(cmd_info);break;//A!
							case 3: proc_rs_Device(cmd_info->Buffer_Data[1],cmd_info);break;//A?!
							case 4: proc_rs_SwchALL(cmd_info);break;//A16!
							case 7: proc_rs_SwchCH(cmd_info);break;//A16[16!
							case 8: proc_rs_ID(cmd_info);break;//A<ID:B>!
							default: proc_rs_Err(cmd_info);
					}	
					cmd_info->Cmd_Type = 0;							
				}

			}
			else if(cmd_info->Cmd_Type == 2)//Åú´¦ÀíÃüÁî½ÓÊÕ½ÓÊÕ
			{
				if(	cmd_info->Buffer_LEN >= BUFFER_LEN_MAX)
					cmd_info->Buffer_LEN = 0;
				cmd_info->Buffer_Data[cmd_info->Buffer_LEN++] = data;
				if(cmd_info->Buffer_LEN == 2)
				{
// 					cmd_info->uart_putchar(cmd_info->Buffer_Data[1]);
// 					cmd_info->uart_putchar(iMtxState[INDEX_MACHINE_ID]);
					if((cmd_info->Buffer_Data[1] == iMtxState[INDEX_MACHINE_ID])||(cmd_info->Buffer_Data[1] ==0x2a))
					{
					}
					else
					{
						cmd_info->Cmd_Type = 0;	
						cmd_info->uart_puts("ÃüÁîÉè±¸IDÒì³£");						
					} 
				}
				if(cmd_info->Buffer_LEN == 4) //½ÓÊÕµ½Êý¾Ý¿é³¤¶È×Ö½Ú
				{
 					cmd_info->Buffer_Data_Len = cmd_info->Buffer_Data[2]*256+cmd_info->Buffer_Data[3];

					cmd_info->DATA_AVAILABLE = 1;
				}
				if(cmd_info->DATA_AVAILABLE) //µÈ´ýÅÐ¶Ï½ÓÊÕ ½áÊø·û0xff
				{				
					if((cmd_info->Buffer_Data[cmd_info->Buffer_Data_Len+5] == 0xff)&&(cmd_info->Buffer_LEN == cmd_info->Buffer_Data_Len+6 )) //½ÓÊÕµ½½áÊø·û
					{						
						//TIM_Cmd(cmd_info->timer, DISABLE);
						cmd_info->Cmd_Type = 0;	
						cmd_info->DATA_AVAILABLE = 0;
//--------------------------------------------------------------------------------------------
		     			if(cmd_info->Buffer_Data[4] == 1)      //ÐÞ¸ÄÉè±¸ºÅ
			 			{
			  	  			if(cmd_info->Buffer_Data_Len==1)
									{
			      				proc_rs_ID_Hex(cmd_info->Buffer_Data[5]);
										cmd_info->uart_puts("OK");
									}
			 			}
			 			else if(cmd_info->Buffer_Data[4] == 2) //²éÑ¯Éè±¸ºÅ
			 			{
			  	  			if(cmd_info->Buffer_Data_Len==0)
			  	  				proc_rs_Device_Hex(cmd_info);
 		 
			 			}
			 			else if(cmd_info->Buffer_Data[4] == 3) //²éÑ¯×´Ì¬
			 			{
			  	  			if(cmd_info->Buffer_Data_Len==0)
			      					proc_rs_State(cmd_info);
							
			 			}
			 			else if(cmd_info->Buffer_Data[4] == 4) //ÅúÁ¿Í¨µÀÇÐ»»
			 			{
			      		if(cmd_info->Buffer_Data_Len%3==0)
				  			{
				    			proc_rs_SwchCH_Hex(&cmd_info->Buffer_Data[5],cmd_info->Buffer_Data_Len,cmd_info);
				  			}
			 			}
			 			else if(cmd_info->Buffer_Data[4] == 5) //ÇÐ»»È«²¿
			 			{			  
			  	  		if((cmd_info->Buffer_Data_Len==1)&&(cmd_info->Buffer_Data[0+5]<iMax_Input_Num))
				  			{
			       				proc_rs_SwchALL_Hex(cmd_info->Buffer_Data[5],cmd_info);
				  			}
			 			}	
			 			else if(cmd_info->Buffer_Data[4] == 6) //±£´æ×´Ì¬
			 			{		 
							if(cmd_info->Buffer_Data[5]==0||cmd_info->Buffer_Data[5]==1||cmd_info->Buffer_Data[5]==2||cmd_info->Buffer_Data[5]==3)
							{
			    					bSavedMtx[cmd_info->Buffer_Data[5]] = 1;					
								savematrixstatus(cmd_info->Buffer_Data[5]);
								cmd_info->uart_puts("OK");
							}				
						}
			 			else if(cmd_info->Buffer_Data[4] == 7) //µ÷ÓÃ×´Ì¬
			 			{
			  	  			if ( bSavedMtx[cmd_info->Buffer_Data[5]] )
				  			{
								loadmatrixstatus(cmd_info->Buffer_Data[5]);
	 							Change_All();
								save_status();	
								cmd_info->uart_puts("OK");
				  			} 
			 			}
			 			else if(cmd_info->Buffer_Data[4] == 0x08) //Ð´ËùÓÐÊäÈëÊä³öÍ¨µÀµÄDDC
			 			{	
							//23+(2a/Éè±¸ID)+00+03+08+ÊäÈë/Êä³öÍ¨µÀÀàÐÍ+ÊäÈë/Êä³öÍ¨µÀÊý+DDCÊý¾Ý±êºÅ
			   				if(cmd_info->Buffer_Data_Len==2&&(cmd_info->Buffer_Data[5]<=iMax_Input_Num)&&(cmd_info->Buffer_Data[6]==0x28||cmd_info->Buffer_Data[6]==0x45
			         		||cmd_info->Buffer_Data[6]==0x27||cmd_info->Buffer_Data[6]==0x25||cmd_info->Buffer_Data[6]==0x23||cmd_info->Buffer_Data[6]==0x19
				     		||cmd_info->Buffer_Data[6]==0x08||cmd_info->Buffer_Data[6]==0x36||cmd_info->Buffer_Data[6]==0x51||cmd_info->Buffer_Data[6]==0x52
				     		||cmd_info->Buffer_Data[6]==0x50||cmd_info->Buffer_Data[6]==0x55))
			    			{
// 									//IWDG_ReloadCounter();
									cmd_info->uart_puts("Wait");
// 			   	 				WRITE_SELECT_DDC(cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6]);		
									cmd_info->uart_puts("OK");	   	 
			    			}
			 			}
			 			else if(cmd_info->Buffer_Data[4] == 0x09) //ÉèÖÃ»úÆ÷ÐÍºÅÊÇ·ñÏÔÊ¾
			 			{
			  	  		if((cmd_info->Buffer_Data_Len==1)&&((cmd_info->Buffer_Data[0+5]==0x00)||(cmd_info->Buffer_Data[0+5]==0x01)||(cmd_info->Buffer_Data[0+5]==0x55)))
				  			{
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
				  			}	
			 			}
			 			else if(cmd_info->Buffer_Data[4] == 0x0a) //¸ü¸Ä»úÆ÷ÐÍºÅ
			 			{
								if((cmd_info->Buffer_Data_Len<=7)&&(cmd_info->Buffer_Data_Len>=1))
				  			{
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
				  		}							
			 			}	
//--------------------------------			
			 			else if(cmd_info->Buffer_Data[4] == 0x0b) //ÉèÖÃUDP¶Ë¿Ú IP¡¢×ÓÍøÑÚÂë¡¢Íø¹Ø
			 			{
			  	  			if(cmd_info->Buffer_Data_Len == 12)
									{
				sprintf(ipAddress,"%d.%d.%d.%d", cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6],cmd_info->Buffer_Data[7],cmd_info->Buffer_Data[8]);	
				printf("%s\n",ipAddress);
				sprintf(submaskAddress,"%d.%d.%d.%d", cmd_info->Buffer_Data[9],cmd_info->Buffer_Data[10],cmd_info->Buffer_Data[11],cmd_info->Buffer_Data[12]);	
				printf("%s\n",submaskAddress);					
				sprintf(gwAddress,"%d.%d.%d.%d", cmd_info->Buffer_Data[13],cmd_info->Buffer_Data[14],cmd_info->Buffer_Data[15],cmd_info->Buffer_Data[16]);	
				printf("%s\n",gwAddress);
				sprintf(cmdbuf,"IP:%s Sub_Mask:%s Gateway:%s ",ipAddress,submaskAddress,gwAddress);
				cmd_info->uart_puts(cmdbuf);	
											save_netconfig();
											net_init();	
										}
			 			}	
			 			else if(cmd_info->Buffer_Data[4] == 0x0c) //ÉèÖÃUDP¶Ë¿Ú PORT
			 			{
									if(cmd_info->Buffer_Data_Len == 2)
									{
				sprintf(port,"%02x%02x",cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6]);				
				cmd_info->uart_puts("PORT:");
				cmd_info->uart_puts(port);
											save_netconfig();
											net_init();	
									}		
			 			}	
						else if(cmd_info->Buffer_Data[4] == 0x0d) //ÉèÖÃUDP¶Ë¿Ú MAC
			 			{	
									if(cmd_info->Buffer_Data_Len == 6)
									{
				sprintf(macAddress,"%02x:%02x:%02x:%02x:%02x:%02x", cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6],cmd_info->Buffer_Data[7],cmd_info->Buffer_Data[8],
						cmd_info->Buffer_Data[9],cmd_info->Buffer_Data[10]);	
				cmd_info->uart_puts("MAC:");
				cmd_info->uart_puts(macAddress);	
											save_netconfig();
											net_init();	
									}								
			 			}	
						else if(cmd_info->Buffer_Data[4] == 0x0e) //ÏÔÊ¾µ±Ç°ÍøÂçÉèÖÃ
			 			{	
									if(cmd_info->Buffer_Data_Len == 0)
									{																		
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
											
				cmd_info->uart_puts("PORT:");
				cmd_info->uart_puts(port);
				cmd_info->uart_puts("\n");
				handle_ip_info(buff,ipAddress,port);
                                cmd=cmdbuf;
				sprintf(cmd,"IP:%s         ",ipAddress);
				cmd+=19;
				sprintf(cmd,"PORT:%s",port);

				if(cmd_info == &uart2_can_info)
				{
					can_send_data(cmdbuf,28);
				}	
											//===================modify====================//				
									}								
			 			}			
//-------------------------------								
/*						else if(UART5_data[4] == 0x0f) //Æ´½Ó¿ØÖÆ£¨18¸ö×Ö½Ú£©Êä³ö¿¨ºÅ + Í¨µÀºÅ+Ix0+Iy0+Ix1+Iy1+Ox0+Oy0+Ox1+Oy1
			 			{	
									if(cmd_info->Buffer_Data_Len == 0x12)
									{	
										
											Splice_Control_Buffer[0] = 0x23;
										  Splice_Control_Buffer[1] = 0x41;
										  Splice_Control_Buffer[2] = 0x00;
											Splice_Control_Buffer[3] = 0x12;
											Splice_Control_Buffer[4] = 0x0f;
										  for(i=0;i<18;i++)
											{
												Splice_Control_Buffer[i+5] = UART5_data[i+5]; 
											}
											Splice_Control_Buffer[23] = 0xff;
											
// 											DMA_DeInit(DMA1_Channel4);
// 											DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)SRC_USART1_DR;
// 											DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Splice_Control_Buffer;
// 											DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
// 											DMA_InitStructure.DMA_BufferSize = 24;
// 											DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
// 											DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
// 											DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
// 											DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
// 											DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
// 											DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
// 											DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
// 											DMA_Init(DMA1_Channel4, &DMA_InitStructure);
// 										
// 											DMA_Cmd(DMA1_Channel4, ENABLE);		
											
											for(i=0;i<24;i++)
											{
												uart1_putchar(Splice_Control_Buffer[i]);
											}
						
											cmd_info->uart_puts("OK");
									}								
			 			}	*/	
						
// //===============================================================================//
// 						else if(UART5_data[4] == 0x0f) //Æ´½Ó¿ØÖÆ£¨18¸ö×Ö½Ú£©Êä³ö¿¨ºÅ + Í¨µÀºÅ+Ix0+Iy0+Ix1+Iy1+Ox0+Oy0+Ox1+Oy1
// 			 			{											
// 									for(i=0;i<cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN ;i++)
// 									{ 												
// 										uart1_putchar(UART5_data[i]);
// 									}	
// 									cmd_info->uart_puts("OK");							
// 			 			}

// //===============================================================================//						
// 						
// 						else if(UART5_data[4] == 0x10) //¶ÁDDC
// 			 			{
// 									if(cmd_info->Buffer_Data_Len == 0x02)
// 									{				
// 											COPY_DDC(UART5_data[5],UART5_data[6]);
// 											cmd_info->uart_puts("OK");
// 									}								
// 			 			}			
// /*						else if(UART5_data[4] == 0x11) //Êä³ö·Ö±æÂÊÑ¡Ôñ£¨1¸ö×Ö½Ú£©
// 			 			{	
// 									if(cmd_info->Buffer_Data_Len == 0x01)
// 									{				
// 											Splice_Control_Buffer[0] = 0x23;
// 										  Splice_Control_Buffer[1] = 0x41;
// 										  Splice_Control_Buffer[2] = 0x00;
// 											Splice_Control_Buffer[3] = 0x01;
// 											Splice_Control_Buffer[4] = 0x11;
// 											Splice_Control_Buffer[5] = UART5_data[5]; 
// 											Splice_Control_Buffer[6] = 0xff;
// 											
// // 											DMA_DeInit(DMA1_Channel4);
// // 											DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)SRC_USART1_DR;
// // 											DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Splice_Control_Buffer;
// // 											DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
// // 											DMA_InitStructure.DMA_BufferSize = 7;
// // 											DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
// // 											DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
// // 											DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
// // 											DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
// // 											DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
// // 											DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
// // 											DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
// // 											DMA_Init(DMA1_Channel4, &DMA_InitStructure);
// // 										
// // 											DMA_Cmd(DMA1_Channel4, ENABLE);	
// 											
// 											for(i=0;i<7;i++)
// 											{
// 												uart1_putchar(Splice_Control_Buffer[i]);
// 											}
// 											
// 											cmd_info->uart_puts("OK");
// 									}								
// 			 			}	*/

// //====================================================================//
// 						else if(UART5_data[4] == 0x11) //Êä³ö·Ö±æÂÊÑ¡Ôñ£¨1¸ö×Ö½Ú£©
// 			 			{	
// 									if(cmd_info->Buffer_Data_Len == 0x02)
// 									{				
// 											Splice_Control_Buffer[0] = 0x23;
// 										  Splice_Control_Buffer[1] = 0x41;
// 										  Splice_Control_Buffer[2] = 0x00;
// 											Splice_Control_Buffer[3] = 0x02;
// 											Splice_Control_Buffer[4] = 0x11;
// 											Splice_Control_Buffer[5] = UART5_data[5]; 
// 											Splice_Control_Buffer[6] = UART5_data[6];
// 											Splice_Control_Buffer[7] = 0xff;
// 											
// 											
// 											for(i=0;i<8;i++)
// 											{
// 												uart1_putchar(Splice_Control_Buffer[i]);
// 											}
// 											
// 											cmd_info->uart_puts("OK");
// 									}								
// 			 			}


// //=====================================================================//
// 						else if(UART5_data[4] == 0x12) // Êä³öÄ£Ê½Ñ¡Ôñ£¨1¸ö×Ö½Ú£©
// 			 			{	
// 									if(cmd_info->Buffer_Data_Len == 0x01)
// 									{				
// 											Splice_Control_Buffer[0] = 0x23;
// 										  Splice_Control_Buffer[1] = 0x41;
// 										  Splice_Control_Buffer[2] = 0x00;
// 											Splice_Control_Buffer[3] = 0x01;
// 											Splice_Control_Buffer[4] = 0x12;
// 											Splice_Control_Buffer[5] = UART5_data[5]; 
// 											Splice_Control_Buffer[6] = 0xff;
// 											
// // 											DMA_DeInit(DMA1_Channel4);
// // 											DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)SRC_USART1_DR;
// // 											DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Splice_Control_Buffer;
// // 											DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
// // 											DMA_InitStructure.DMA_BufferSize = 7;
// // 											DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
// // 											DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
// // 											DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
// // 											DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
// // 											DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
// // 											DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
// // 											DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
// // 											DMA_Init(DMA1_Channel4, &DMA_InitStructure);
// // 										
// // 											DMA_Cmd(DMA1_Channel4, ENABLE);	

// 											for(i=0;i<7;i++)
// 											{
// 												uart1_putchar(Splice_Control_Buffer[i]);
// 											}
// 											
// 											cmd_info->uart_puts("OK");
// 									}								
// 			 			}		

						else if(cmd_info->Buffer_Data[4] == 0x0f) 
			 			{				
										count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;						
										can_send_data(cmd_info->Buffer_Data,count);								
			 			}
//===============================================================================//

						else if(cmd_info->Buffer_Data[4] == 0x10) //ÊäÈë¶ËDDC¸´ÖÆ(2¸ö×Ö½Ú),UART5_data[5], Êä³öÍ¨µÀºÅ
																					 //UART5_data[6],ÊäÈëÍ¨µÀºÅ 
			 			{	
									if(cmd_info->Buffer_Data_Len == 0x02)
									{				
// 											COPY_DDC(cmd_info->Buffer_Data[5],cmd_info->Buffer_Data[6]);
											cmd_info->uart_puts("OK");
									}	
			 			}			

//====================================================================//
// 						else if(UART5_data[4] == 0x11) // Êä³ö·Ö±æÂÊÑ¡Ôñ£¨2¸ö×Ö½Ú£© UART5_data[5],id  UART5_data[6],·Ö±æÂÊ
// 			 			{	
// 									
// 							    CONFIG_4052(DDC_OUT_4052,UART_OUT_4052);
// 									if(cmd_info->Buffer_Data_Len == 0x02)
// 									{							
// 											count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;										
// 											for(i=0;i<count;i++)
// 											{
// 												uart1_putchar(UART5_data[i]);
// 											}
// 											
// 											//cmd_info->uart_puts("OK");
// 									}								
// 			 			}
						else if(cmd_info->Buffer_Data[4] == 0x11) // Êä³ö·Ö±æÂÊÑ¡Ôñ£¨2¸ö×Ö½Ú£© UART5_data[5],id  UART5_data[6],·Ö±æÂÊ
			 			{	
									
//  							    CONFIG_4052(DDC_OUT_4052,UART_OUT_4052);
									if(cmd_info->Buffer_Data_Len == 0x02)
									{		
											if(cmd_info->Buffer_Data[6] > 0)
											{										
// 													Data_CPLD(OUT_REVE_NUM[cmd_info->Buffer_Data[5]]); 
																						
													count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;										
// 													for(i=0;i<count;i++)
// 													{
// 														uart1_putchar(cmd_info->Buffer_Data[i]);
// 													}
													can_send_data(cmd_info->Buffer_Data,count);
// 													delay_ms(80);
// // 													Data_CPLD(0x100);	
 													cmd_info->uart_puts("OK");
 											}	
 									}								
 			 			}
//=====================================================================//
// 						else if(UART5_data[4] == 0x12) // Êä³öÄ£Ê½Ñ¡Ôñ£¨1¸ö×Ö½Ú£©
// 			 			{	
// 									if(cmd_info->Buffer_Data_Len == 0x01)
// 									{				
// 											Splice_Control_Buffer[0] = 0x23;
// 										  Splice_Control_Buffer[1] = 0x41;
// 										  Splice_Control_Buffer[2] = 0x00;
// 											Splice_Control_Buffer[3] = 0x01;
// 											Splice_Control_Buffer[4] = 0x12;
// 											Splice_Control_Buffer[5] = UART5_data[5]; 
// 											Splice_Control_Buffer[6] = 0xff;
// 											
// // 											DMA_DeInit(DMA1_Channel4);
// // 											DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)SRC_USART1_DR;
// // 											DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Splice_Control_Buffer;
// // 											DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
// // 											DMA_InitStructure.DMA_BufferSize = 7;
// // 											DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
// // 											DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
// // 											DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
// // 											DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
// // 											DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
// // 											DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
// // 											DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
// // 											DMA_Init(DMA1_Channel4, &DMA_InitStructure);
// // 										
// // 											DMA_Cmd(DMA1_Channel4, ENABLE);	

// 											for(i=0;i<7;i++)
// 											{
// 												uart1_putchar(Splice_Control_Buffer[i]);
// 											}
// 											
// 											cmd_info->uart_puts("OK");
// 									}								
// 			 			}										
//=======================================2014.2.19Ìí¼Ó=====================================================//
// 						else if(UART5_data[4] == 0x12) // Êä³öÄ£Ê½Ñ¡Ôñ£¨2¸ö×Ö½Ú£© UART5_data[5],id  
// 																					 //UART5_data[6],Ä£Ê½(DVIÊÇ0x01,HDMIÎª0x02) 
// 			 			{	
// 							    CONFIG_4052(DDC_OUT_4052,UART_OUT_4052);
// 									if(cmd_info->Buffer_Data_Len == 0x02)
// 									{				
// 											count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;
// 											for(i=0;i<count;i++)
// 											{
// 												uart1_putchar(UART5_data[i]);
// 											}
// 											//cmd_info->uart_puts("OK");
// 									}								
// 			 			}	
// 						else if(cmd_info->Buffer_Data[4] == 0x12) // Êä³öÄ£Ê½Ñ¡Ôñ£¨2¸ö×Ö½Ú£© UART5_data[5],id  
// 																					 //UART5_data[6],Ä£Ê½(DVIÊÇ0x01,HDMIÎª0x02) 
// 			 			{	
// //  							    CONFIG_4052(DDC_OUT_4052,UART_OUT_4052);
// 									if(cmd_info->Buffer_Data_Len == 0x02)
// 									{
// 											if(cmd_info->Buffer_Data[6] > 0)
// 											{										
// // 													Data_CPLD(OUT_REVE_NUM[cmd_info->Buffer_Data[5]]);
// 													count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;
// // 													for(i=0;i<count;i++)
// // 													{
// // 														uart1_putchar(cmd_info->Buffer_Data[i]);
// // 													}
// 													can_send_data(cmd_info->Buffer_Data,count);
// 													delay_ms(80);
// // 													Data_CPLD(0x100);	
// 													cmd_info->uart_puts("OK");
// 											}
// 									}								
// 			 			}	
						else if(cmd_info->Buffer_Data[4] == 0x12) // Êä³öÄ£Ê½Ñ¡Ôñ£¨2¸ö×Ö½Ú£© UART5_data[5],id  (1,DVI 2,HDMI)
																					 //UART5_data[6],Ä£Ê½(DVIÊÇ0x01,HDMIÎª0x02) 
			 			{	
//  							    CONFIG_4052(DDC_OUT_4052,UART_OUT_4052);
									if(cmd_info->Buffer_Data_Len == 0x02)
									{
											if((cmd_info->Buffer_Data[6] > 0) &&  (cmd_info->Buffer_Data[6] <=2 ) )
											{										
// 													Data_CPLD(OUT_REVE_NUM[cmd_info->Buffer_Data[5]]);
													count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;
													can_send_data(cmd_info->Buffer_Data,count);
// 													delay_ms(80);
// 													Data_CPLD(0x100);	
													cmd_info->uart_puts("OK");
											}
									}								
			 			}							
						else if(cmd_info->Buffer_Data[4] == 0x16) //Íâ²¿×Ô¶¨ÒåDDC(256×Ö½Ú) UART5_data[5]£¬´ú±íÍ¨µÀºÅ;
																					 //UART5_data[6]µ½0xffÖ®Ç°ÊÇÍâ²¿ddcÊý¾ÝÄÚÈÝ
			 			{	
									if(cmd_info->Buffer_Data_Len == (256 + 1))
									{				
											cmd_info->uart_puts("start");
											exteral_ddc_do(cmd_info->Buffer_Data+6,cmd_info->Buffer_Data[5]);
											cmd_info->uart_puts("ok!");
									}								
			 			}	
						
// 						else if(UART5_data[4] == 0x17) //ÊäÈë·Ö±æÂÊÑ¡Ôñ£¨2¸ö×Ö½Ú£©
// 			 			{	
// //  							    HC595_Config(CPLD_DDC_CE2_H);
// 							    CONFIG_4052(DDC_OUT_4052,UART_IN_4052);
// 							
// 									if(cmd_info->Buffer_Data_Len == 0x02)
// 									{			
// //=====================================bug  bug  bug bug=========================================================//										
// 										  Data_CPLD(IN_REVE_NUM[UART5_data[5]]); //============´Ë´¦Îªbug  bug  bug
// 										  Data_CPLD(0xc0);
// //===============================================================================================================//											
// 											count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;
// 											for(i=0;i<count;i++)
// 											{
// 												uart1_putchar(UART5_data[i]);
// 											}
// 											
// 											//cmd_info->uart_puts("OK");
// 									}
//  									//HC595_Config(CPLD_CE_L);									
// 			 			}
// 						
// 						else if(UART5_data[4] == 0x19) // ÊäÈëÄ£Ê½Ñ¡Ôñ£¨2¸ö×Ö½Ú£©UART5_data[5],id  UART5_data[6],Ä£Ê½
// 			 			{	
// //  									HC595_Config(CPLD_DDC_CE2_H);
// 									CONFIG_4052(DDC_OUT_4052,UART_IN_4052);
// 									if(cmd_info->Buffer_Data_Len == 0x02)
// 									{			
// //=====================================bug  bug  bug bug=========================================================//										
// 										  Data_CPLD(IN_REVE_NUM[UART5_data[5]]); //============´Ë´¦Îªbug  bug  bug
// 										  Data_CPLD(0xc0);
// //===============================================================================================================//	
//                       count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;										
// 											for(i=0;i<count;i++)
// 											{
// 												uart1_putchar(UART5_data[i]);
// 											}											
// 											//cmd_info->uart_puts("OK");
// 									}		
//  									//HC595_Config(CPLD_CE_L);									
// 			 			}
						else if(cmd_info->Buffer_Data[4] == 0x17) //ÊäÈë·Ö±æÂÊÑ¡Ôñ£¨2¸ö×Ö½Ú£©
			 			{	
// 							    CONFIG_4052(DDC_OUT_4052,UART_IN_4052);
							
									if(cmd_info->Buffer_Data_Len == 0x02)
									{			
											if(cmd_info->Buffer_Data[6] > 0)
											{										
// 													Data_CPLD(IN_REVE_NUM[cmd_info->Buffer_Data[5]]); 											
													count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;
// 													for(i=0;i<count;i++)
// 													{
// 														uart1_putchar(cmd_info->Buffer_Data[i]);
// 													}
													can_send_data(cmd_info->Buffer_Data,count);
// 													delay_ms(80);
// 													Data_CPLD(0x100);
													cmd_info->uart_puts("OK");
											}
									}
 									//HC595_Config(CPLD_CE_L);									
			 			}
						else if(cmd_info->Buffer_Data[4] == 0x19) // ÊäÈëÄ£Ê½Ñ¡Ôñ£¨2¸ö×Ö½Ú£©UART5_data[5],id  UART5_data[6],Ä£Ê½
			 			{	

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
			 			}
						
// 						else if(cmd_info->Buffer_Data[4] == 0x19) // ÊäÈëÄ£Ê½Ñ¡Ôñ£¨2¸ö×Ö½Ú£©UART5_data[5],id  UART5_data[6],Ä£Ê½
// 			 			{	
// //  									CONFIG_4052(DDC_OUT_4052,UART_IN_4052);
// 									if(cmd_info->Buffer_Data_Len == 0x02)
// 									{		
// 											if(cmd_info->Buffer_Data[6] > 0)
// 											{										
// // 													Data_CPLD(IN_REVE_NUM[cmd_info->Buffer_Data[5]]); 
// 													count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;										
// // 													for(i=0;i<count;i++)
// // 													{
// // 														uart1_putchar(cmd_info->Buffer_Data[i]);
// // 													}	
// 													can_send_data(cmd_info->Buffer_Data,count);
// 													delay_ms(80);											
// // 													Data_CPLD(0x100);											
// 													cmd_info->uart_puts("OK");
// 											}
// 									}		
//  									//HC595_Config(CPLD_CE_L);									
// 			 			}
// 						else if(cmd_info->Buffer_Data[4] == 0x1A) // ²éÑ¯°å¿¨·Ö±æÂÊºÍ¸ñÊ½ UART5_data[5],id  UART5_data[6],ÊäÈë/Êä³ö
// 			 			{															 //UART5_data[6] :0x01´ú±íÊä³ö£¬0x02´ú±íÊäÈë						
// 									if(cmd_info->Buffer_Data_Len == 0x02)
// 									{						
// 											contrl_mode = CONTRL_MOD_UART5;
// 										  if(cmd_info->Buffer_Data[6] == 0x02)
// 											{

// 													Data_CPLD(IN_REVE_NUM[cmd_info->Buffer_Data[5]]);
// //    												CONFIG_4052(DDC_OUT_4052,UART_IN_4052);
// 											}
// 											else if(cmd_info->Buffer_Data[6] == 0x01)
// 											{
// 												  
// // 												Data_CPLD(0xaa);
// 													Data_CPLD(OUT_REVE_NUM[cmd_info->Buffer_Data[5]]);
// //   												CONFIG_4052(DDC_OUT_4052,UART_IN_4052);												
// 												
// 											}																						
// 											count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;
// 											
// 											for(i=0;i<count;i++)
// 											{
// 													uart1_putchar(cmd_info->Buffer_Data[i]);
// 											}				
// // 											cmd_info->uart_puts("OK");
// 											delay_ms(100);
// 											Data_CPLD(0x100);

// 									}								
// 			 			}
						
// 						else if(UART5_data[4] == 0x1B) // ²éÑ¯°å¿¨·Ö±æÂÊºÍ¸ñÊ½·´À¡ÃüÁî
// 			 			{															 //UART5_data[5],id;UART5_data[6],ÊäÈë/Êä³ö; 
// 																					 //UART5_data[7],·Ö±æÂÊ;UART5_data[8],¸ñÊ½;									
// 									if(cmd_info->Buffer_Data_Len == 0x04)
// 									{				
// 										  count = cmd_info->Buffer_Data_Len+COMMAND_HEAD_LEN +COMMAND_TYPE_LEN +COMMAND_TERMINATOR_LEN;
// 											for(i=0;i<count;i++)
// 											{
// 												cmd_info->uart_putchar(UART5_data[i]);
// 											}
// 											
// 											//cmd_info->uart_puts("OK");
// 									}								
// 			 			 }				

						else if(cmd_info->Buffer_Data[4] == 0x1C) // ±£´æÆ´½ÓÄ£Ê½(UART5_data[5]0,7 1---8Ä£Ê½)
			 			{	
							if((cmd_info->Buffer_Data_Len == 0x01) && ((cmd_info->Buffer_Data[5]+1) <= SPLICE_MODE_NUM))
							{			
								save_splice_data(splice_data_buff,cmd_info->Buffer_Data[5]+1,MAX_CHANNEL_NUMS*5);
								cmd_info->uart_puts("OK");
							}								
			 			}
						
						else if(cmd_info->Buffer_Data[4] == 0x1D) // µ÷ÓÃÆ´½ÓÄ£Ê½(UART5_data[5] 1---8Ä£Ê½)
			 			{	
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

								can_send_data(tmp_buffer,(MAX_CHANNEL_NUMS*5+6));
								save_splice_data(splice_data_buff,0,MAX_CHANNEL_NUMS*5);												
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
							}								
			 			}
						
						else if(cmd_info->Buffer_Data[4] == 0x21)//Ëø¶¨¾ØÕó,¹Ø±ÕËùÓÐ´®¿Ú ÍøÂç½ÓÊÕÖÐ¶Ï£¬Ö»±£ÁôcanÍ¨ÐÅ
						{
							  if((!cmd_info->Buffer_Data_Len) && (cmd_info == &uart2_can_info))
								{
									 net_interrupt_op(OFF);
									 uart_interrupt_op(OFF);
								}
								
						}
						
						else if(cmd_info->Buffer_Data[4] == 0x22)//¾ØÕó½âËø,´ò¿ª´®¿ÚÍøÂç½ÓÊÕÖÐ¶Ï
						{
							  if((!cmd_info->Buffer_Data_Len) && (cmd_info == &uart2_can_info))
								{
									 net_interrupt_op(ON);
									 uart_interrupt_op(ON);
								}
						}
						
					
// //-------------------------------		
					}					
				}
			}
		}
		//GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_SET);
		//cmd_data_led = 0;
}
#endif 

void * ctl_cmd_thread(void* arg)
			
{

   //(void) p_arg;
   		   	    
		while (1)
   {
		// handle_ctl_cmd(&uart1_info);
		 handle_ctl_cmd(&uart3_info);
		 handle_ctl_cmd(&uart4_info);
		 handle_ctl_cmd(&uart5_info);
		 //handle_ctl_cmd(&net_info);
		 handle_ctl_cmd(&uart2_can_info);
		 
		//IWDG_ReloadCounter();
		//OSTimeDlyHMSM(0, 0, 0, 1);							
	}  		  
}




