//#define  	MAX_INPUT_NUM  					8						 //ŸØÕóÊäÈëÊý					  							  
//#define   	MAX_OUTPUT_NUM	 				8						 //ŸØÕóÊä³öÊý


extern unsigned char iMax_Input_Num,iMax_Output_Num; 


#define		MACHINE_ID						0x41					 //Éè±žIDºÅ: A
#define	 	INDEX_MACHINE_ID				(iMax_Output_Num)		     //Éè±žIDºÅµÄË÷Òý
#define	 	INDEX_SWITCH_MODE				(iMax_Output_Num+1)		 //°ŽŒüÇÐ»»·œÊœµÄË÷Òý
#define 	LOGO_VISIBLE					(iMax_Output_Num+2)		 //ÊÇ·ñÏÔÊŸÉè±žÐÍºÅ
#define		BATCHCMDSTR_STARTCHAR           '#'						 //ÅúŽŠÀíÃüÁî¿ªÍ·
#define		BATCHCMDSTR_ENDCHAR             0xff					 //ÅúŽŠÀíÃüÁîœáÎ²
#define		BATCH_MAX_LEN 					2048

#define 	MAX_SAVE_STATE_NUM 				4

#define   	KEY_FUNC_SAVE					0x41					 //ŒüÅÌ-SAVE
#define    	KEY_FUNC_CALL					0x42					 //ŒüÅÌ-CALL
#define	 	KEY_BackSpace					0x43					 //ŒüÅÌ-ÍËžñŒü
#define 	KEY_FUNC_F						0x50					 //ŒüÅÌ-F
#define 	KEY_SFUNC_ALL					0x51					 //ŒüÅÌ-ALL
#define 	KEY_FUNC_AV						0x52					 //ŒüÅÌ-AV
#define	 	KEY_RESET						0x60					 //ŒüÅÌ-»ÖžŽŒü
#define 	KEY_FUNC_VIDEO					0x61					 //ŒüÅÌ-VIDEO
#define 	KEY_FUNC_AUDIO					0x62					 //ŒüÅÌ-Audio
#define		KEY_ENTER						0x63					 //ŒüÅÌ-¹ŠÄÜŒü

#define		SWITCH_SMODE_ALL				0						 //°ŽŒüÇÐ»»·œÊœ-È«²¿
#define	    SWITCH_MODE_AV				    1						 //°ŽŒüÇÐ»»·œÊœ-AV
#define	    SWITCH_MODE_VIDEO				2						 //°ŽŒüÇÐ»»·œÊœ-Video
#define	    SWITCH_MODE_AUDIO				3						 //°ŽŒüÇÐ»»·œÊœ-Audio

#define	    SAVE_DATA_FROM_EEPROM_ADDR		0x00					 //ÊýŸÝ±£ŽæµœEEPROMµÄ¿ªÊŒµØÖ·
#define	    MTRX_SAVED_FLAG_EEPROM_ADDR		0x9c4					 //±£Žæ±£Žæ×ŽÌ¬µÄEEPROMµØÖ·
#define	    NAME_SAVED_FLAG_EEPROM_ADDR		0xa28					 //±£Žæ±£Žæ×ŽÌ¬µÄEEPROMµØÖ·

#define     iMtxState_MODE 					0x02					 //»úÆ÷ÐÍºÅ£¬²»±äµÄ



//===========================================================================//


// #define			DDC_IN_CE										0X0100
// #define			UART_IN_CE                  0X0200
// #define			DDC_OUT_CE									0X0400
// #define			UART_OUT_CE									0X0800

#define	 		CONTRL_MOD_UART4						0X04							
#define	    CONTRL_MOD_UART5						0x05
#define			CONTRL_MOD_NET							0X06


#define			MAX_CHANNEL_NUMS							8
#define			BYTES_1K											0x400
#define 		SPLICE_MODE_NUM								8
//ÊµŒÊ±£Žæµ±Ç°ÆŽœÓ×ŽÌ¬ÊýŸÝµÄ³€¶È£¬Îª0Ê±Žú±íÎŽÕýÈ·ÖŽÐÐ»òÔòÎŽÖŽÐÐ0x0fÆŽœÓÃüÁî£¬
//·ñÔòŽú±í³É¹ŠÖŽÐÐ¹ýÆŽœÓÃüÁî,¿ÉÒÔÔÚÉÏµç³õÊŒ»¯Ê±·¢ËÍÉÏŽÎ¶Ïµç±£ŽæµÄÆŽœÓ×ŽÌ¬ÊýŸÝ
//¹²ÓÐ9ÖÖÆŽœÓÄ£ÊœÊýŸÝ³€¶ÈÒª±£Žæ£¬³€¶È±ŸÉíÕŒÓÃ2žö×ÖœÚ£¬µØÖ·ÊýÖµÐ¡µÄÊÇµÍ8Î»£¬µØÖ·ÊýÖµŽóµÄÊÇžß8Î»¡£
#define 		SPLICE_DATA_LEN_ADDR  			 0XAF0                          //±£Žæ»úÆ÷ÊÇ·ñ±»ÕýÈ·ÖŽÐÐ¹ýÆŽœÓÃüÁî

#define 		SPLICE_DATA_BASE_ADDR 		   0XB00                          //±£Žæµ±Ç°ÆŽœÓÊýŸÝµØÖ·(ÆŽœÓÄ£Êœ0)
#define			SPLICE_DATA_ADDR(base_addr,mode_num) (base_addr + (BYTES_1K*(mode_num)))
																																				//Ò»¹²ÓÐ8žöÆŽœÓÄ£Êœ£¬1-8
/*																																							
   CURRENT_SPLICE_DATA_ADDR + 0x400*1	//±£ŽæÆŽœÓÄ£Êœ1ÊýŸÝµØÖ·
   CURRENT_SPLICE_DATA_ADDR + 0x400*2	//±£ŽæÆŽœÓÄ£Êœ2ÊýŸÝµØÖ·
   CURRENT_SPLICE_DATA_ADDR + 0x400*3	//±£ŽæÆŽœÓÄ£Êœ3ÊýŸÝµØÖ·
   CURRENT_SPLICE_DATA_ADDR + 0x400*4	//±£ŽæÆŽœÓÄ£Êœ4ÊýŸÝµØÖ·
   CURRENT_SPLICE_DATA_ADDR + 0x400*5	//±£ŽæÆŽœÓÄ£Êœ5ÊýŸÝµØÖ·
   CURRENT_SPLICE_DATA_ADDR + 0x400*6	//±£ŽæÆŽœÓÄ£Êœ6ÊýŸÝµØÖ·
   CURRENT_SPLICE_DATA_ADDR + 0x400*7	//±£ŽæÆŽœÓÄ£Êœ7ÊýŸÝµØÖ·
   CURRENT_SPLICE_DATA_ADDR + 0x400*8	//±£ŽæÆŽœÓÄ£Êœ8ÊýŸÝµØÖ·
*/

extern int Matrix_init(void);
extern char splice_data_buff[1024];//ŽæŽ¢ÆŽœÓÃüÁîÖÐµÄ0x0fÃüÁîºóµœ0xffÖ®ŒäµÄÊýŸÝ
extern u16 splice_data_len[9];		//[0]Žú±íµ±Ç°µÄÆŽœÓÄ£ÊœÊýŸÝ³€¶È,[1]--[8]Îª8žöÆŽœÓÄ£Êœž÷×ÔµÄÆŽœÓÊýŸÝ³€¶È
//========================¹Ø±Õµ÷ÊÔÐÅÏ¢==================================//
#define DEBUG_MODE_ON      (0) 

//===========================================================================//
//#include "uart_usr.h"
//#include "can_usr.h"
extern unsigned char *iMtxState;


extern unsigned char bSavedMtx[MAX_SAVE_STATE_NUM];
extern unsigned int  iSaveMtxAddr[MAX_SAVE_STATE_NUM];

extern char Software_version[];
extern char *DEVICE_NAME;//
extern char *DEVICE_NAME_STATIC;//
//=================================================================================//
//==========================================================================================//

// #define			UAR4_BAUND_ADDR				0xae0						//Žæ·ÅŽ®¿Ú4ÉèÖÃÊýŸÝ

// enum BaudRateType 
// {
//     BAUD600=600,
//     BAUD1200=1200,
//     BAUD2400=2400,
//     BAUD4800=4800,
//     BAUD9600=9600,
//     BAUD19200=19200,
//     BAUD38400=38400,
// 	  BAUD43000=43000,
//     BAUD56000=56000,
//     BAUD57600=57600,
//     BAUD115200=115200
// };

// enum DataBitsType 
// {
//     DATA_8=8,
//     DATA_9=9
// };

// enum ParityType 
// {
//     PAR_No,
//     PAR_Even,
// 	  PAR_Odd,
// };

// enum StopBitsType 
// {
//     STOP_1=1,               
//     STOP_2=2
// };

// enum FlowType 
// {
//     FLOW_None,
//     FLOW_RTS,
//     FLOW_CTS,
// 		FLOW_RTS_CTS
// };



#define SWITCH_IOCTL_MAGIC		'S'

#define VIDEO_SWITCH_GETDEVICE          _IOWR(SWITCH_IOCTL_MAGIC, 20, unsigned int)	// get/set device id
#define VIDEO_SWITCH_SETONECHANNEL	_IOW(SWITCH_IOCTL_MAGIC, 30, unsigned int)	// Change One Video
#define VIDEO_SWITCH_GETONECHANNEL	_IOR(SWITCH_IOCTL_MAGIC, 31, unsigned int)	//get One Video

#define VIDEO_SWITCH_SETALLCHANNEL	_IOW(SWITCH_IOCTL_MAGIC, 301, unsigned int)	// Change all Video

typedef struct SW__INFO
{
	char * version;
	char * device_name;
	char * device_name_static;
	unsigned char * iMtxState;
	unsigned char output_maxnum;
	unsigned char input_maxnum;


}Switch_Info;

#define SV_8X8  0x1
#define SV_10X10 0x2
#define SV_9X9 0x3
#define SV_20X20 0x4
#define SV_18X18 0x5
#define SV_16X16 0x6
#define SV_36X36 0x8
#define SV_72X72 0xa
#define SV_144X144 0xc



void Change_All(void);

void exteral_ddc_do(char* ptr,unsigned char In_num);




//==========================================================================================//

//=================================================================================//
//==================modify_2016.12.22=========================//
//#define CLI()      __set_PRIMASK(1)  
//#define SEI()      __set_PRIMASK(0)  
//==================modify_2016.12.22=========================//
