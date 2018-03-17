#ifndef _STATUS_FILE_H
#define _STATUS_FILE_H

void LoadDataFromFile(void);

//void data_write(char *src,u16 addr,u16 len);
//void data_read(char *dst,u16 addr,u16 len);
int save_configfile(void);
int save_netconfig(void);
int save_statusfile(void);

int loadmatrixstatus(int index);



void load_splice_data(char *splicedata,int num,int lenth);
void save_splice_data(char *splicedata,int num,int lenth);

int get_splice_num(void);
int set_splice_num(int num);
void save_status(void);
#endif
