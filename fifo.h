#ifndef _FIFO_H
#define _FIFO_H
#include <pthread.h>
#include <bits/pthreadtypes.h>
//#include "stm32f10x.h"

#define  ON						1
#define OFF 	 				0

typedef struct _FIFO
{
    u8    *pFirst; //指向FIFO的首地址
    u8    *pLast;  //指向FIFO的末地址
    u8    *pIn;	   //当前写地址
    u8    *pOut;   //当前读地址
    u32  Length;   //FIFO的总长度
	u32  Enteres;  //当前FIFO中的数据量
	pthread_rwlock_t rwlock;

}__FIFO;


extern __FIFO COM1FIFO,COM2_CAN_FIFO,COM3FIFO,COM4FIFO,COM5FIFO,NETFIFO,UDPFIFO;

extern void Create_Cmd_FIFO(u32 FIFOLength,__FIFO * fifo_ptr);
#ifdef __cplusplus
 extern "C"
{
 #endif
	u8 Write_Cmd_FIFO(u8* pSource,u32 WriteLength,__FIFO * fifo_ptr);
#ifdef __cplusplus

}
 #endif
extern u8 Read_Cmd_FIFO(u8 *pAim,u32 ReadLength,__FIFO * fifo_ptr);
extern void Init_Fifos(void);
// extern void CreateFIFO1(u32 FIFOLength);
// extern void CreateFIFO4(u32 FIFOLength);
// extern void CreateFIFO5(u32 FIFOLength);
// extern void CreateFIFONET(u32 FIFOLength);
// extern u8 WriteFIFO1(u8* pSource,u32 WriteLength);
// extern u8 WriteFIFO4(u8* pSource,u32 WriteLength);
// extern u8 WriteFIFO5(u8* pSource,u32 WriteLength);
// extern u8 WriteFIFONET(u8* pSource,u32 WriteLength);
// extern u8 ReadFIFO1(u8 *pAim,u32 ReadLength);
// extern u8 ReadFIFO4(u8 *pAim,u32 ReadLength);
// extern u8 ReadFIFO5(u8 *pAim,u32 ReadLength);
// extern u8 ReadFIFONET(u8 *pAim,u32 ReadLength);

#endif
