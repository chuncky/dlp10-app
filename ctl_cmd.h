#ifndef _CTL_CMD_H
#define _CTL_CMD_H
#include "uart.h"
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;


#define CMD_CHANGE_DEVICEID 0x01
#define CMD_CHECK_DEVICEID 0x02
#define CMD_CHECK_STATUS 0X03
#define CMD_BYTE_CHANNELSWTICH 0X04
#define CMD_CHANNELSWTICH_ALL 0X05
#define CMD_SAVE_STATUS 0X06
#define CMD_LOAD_STATUS 0X07
#define CMD_WRITE_DDC 0X08
#define CMD_CHANGE_DEVICETYPEDISPLAY 0X09
#define CMD_CHANGE_DEVICETYPE 0X0A

#define CMD_CHANGE_NETSETTING 0X0B
#define CMD_CHANGE_UDPPORT 0X0C
#define CMD_CHANGE_MACADDR 0X0D
#define CMD_DISPLAY_NET 0X0E
#define CMD_NETRESET 0X1F
#define CMD_DDC_COPY 0X10

#define CMD_DDC_OUTPIXSEL 0X11
#define CMD_DDC_OUTMODE 0X12
//#define CMD_DDC_COPY 0X13
#define CMD_DDC_INMODE 0X19

#define CMD_MATRIX_LOCK 0X21
#define CMD_MATRIX_DISLOCK 0X22

void * ctl_cmd_thread(void* arg);
void savematrixstatus(int index);

void handle_ctl_cmd(Ctl_Cmd_Info* cmd_info);

void init_etimer(void);

#endif
