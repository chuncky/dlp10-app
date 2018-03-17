
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <bits/pthreadtypes.h>

#include "linuxdef.h"


//#include <ucos_ii.h>
//#include <os_cpu.h> 
#include "fifo.h"

// extern void uart5_putchar(char data);
__FIFO COM1FIFO,COM2_CAN_FIFO,COM3FIFO,COM4FIFO,COM5FIFO,NETFIFO,UDPFIFO;

//==============================================================================//
void Create_Cmd_FIFO(u32 FIFOLength,__FIFO * fifo_ptr)
{
    u8 *pmyFIFO;
    int res;
    pmyFIFO = (u8 *)malloc(FIFOLength);

    fifo_ptr->pFirst = pmyFIFO;
    fifo_ptr->pLast = pmyFIFO + FIFOLength-1;
    fifo_ptr->Length = FIFOLength;
    fifo_ptr->pIn     = fifo_ptr->pFirst;
    fifo_ptr->pOut    = fifo_ptr->pFirst;
    fifo_ptr->Enteres = 0; 
    res=pthread_rwlock_init(&(fifo_ptr->rwlock),NULL);
    if (res != 0)
    {
        printf("rwlock initialization failed");

    }
 

}



u8 Write_Cmd_FIFO(u8 *pSource,u32 WriteLength,__FIFO * fifo_ptr)
{

    u32 i;
//printf("%s-01\n",__func__);
    pthread_rwlock_wrlock(&(fifo_ptr->rwlock));
    for (i = 0; i < WriteLength; i++)
    {
        //OS_ENTER_CRITICAL();
	* (fifo_ptr->pIn ++ ) = *(pSource ++);

	fifo_ptr->Enteres ++;

	if (fifo_ptr->pIn == fifo_ptr->pLast)
	{
			fifo_ptr->pIn = fifo_ptr->pFirst;
	}
		//OS_EXIT_CRITICAL();	  //恢复全局中断标志			
     }
     pthread_rwlock_unlock(&(fifo_ptr->rwlock));
//printf("%s-02\n",__func__);
    return i;
}


u8 Read_Cmd_FIFO(u8 *pAim,u32 ReadLength,__FIFO * fifo_ptr)
{
//printf("%s-01\n",__func__);

    u32 i;
    pthread_rwlock_rdlock(&(fifo_ptr->rwlock));
    for (i = 0; i < ReadLength; i++)
    {
		if (fifo_ptr->Enteres <= 0)
		{
			pthread_rwlock_unlock(&(fifo_ptr->rwlock));
			//printf("%s-02\n",__func__);
			return i;//返回从FIFO中读取到的数据个数

		}

	   // OS_ENTER_CRITICAL();

		*(pAim ++) = *(fifo_ptr->pOut ++);

		fifo_ptr->Enteres -- ;

		if (fifo_ptr->pOut == fifo_ptr->pLast)
		{
			fifo_ptr->pOut = fifo_ptr->pFirst;
		}

		//OS_EXIT_CRITICAL();	  //恢复全局中断标志		
	}
	pthread_rwlock_unlock(&(fifo_ptr->rwlock));
//printf("%s-03\n",__func__);
	return i;
}

void Init_Fifos(void)
{
	Create_Cmd_FIFO(1024,&COM1FIFO);

	Create_Cmd_FIFO(1024,&COM2_CAN_FIFO);
	Create_Cmd_FIFO(1024,&COM3FIFO);
	Create_Cmd_FIFO(1024,&COM4FIFO);
	Create_Cmd_FIFO(1024,&COM5FIFO);
	Create_Cmd_FIFO(1024,&NETFIFO);
	Create_Cmd_FIFO(1024,&UDPFIFO);

}


