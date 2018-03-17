#ifndef _CAN_H
#define _CAN_H
#include <pthread.h>
#include <bits/pthreadtypes.h>
#define		SEND_ID_SELF   0X01
int can_init(void);

void * candata_receive_process(void* arg);

int can_thread_init(void);
int can_thread_join(void);
void CAN_proc_rs_State(void);

void can_send_data(u8 *ptr,u16 len);

#endif
