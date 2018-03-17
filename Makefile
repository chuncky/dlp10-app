#  
# c.cpp��ϱ����makefileģ��  
#  
#  
LIBRARYPATH=/home/tester/dlp10/lib
  
BIN = dlp10_test 
CROSS = arm-linux-
CC = $(CROSS)gcc  
CPP = $(CROSS)g++  
LD = $(CROSS)ld  
#����ֻ�����ͷ�ļ�·������·��  
INCS = -I${LIBRARYPATH}/Poco/include -I${LIBRARYPATH}/include
LIBS = -L${LIBRARYPATH}/Poco/lib  -L${LIBRARYPATH}/lib 
SUBDIRS =  
 
#  
#maintest.c tree/rbtree.c  ������Ŀ¼���Ǿ�ֱ����� Ŀ¼/*.c����   ���е�Դ�ļ�--  .c�ļ��б�  
CSRCS = $(wildcard ./*.c )  
CPPSRCS = $(wildcard ./*.cpp)  
#  
#���е�.o�ļ��б�  
COBJS := $(CSRCS:.c=.o)  
CPPOBJS := $(CPPSRCS:.cpp=.o)  
#  

CFLAGS += $(INCS)  
CFLAGS += -O2 -Wall -g   
CPPFLAGS += $(INCS)  
CPPFLAGS += -O2 -Wall -g  
LDFLAGS +=  -lPocoNet -lPocoFoundation -lini -lpthread -lm -lrt -lstdc++ -static 
  
all:$(BIN)  
#$(OBJS):%.o :%.c  ����$(OBJS)�е�һ�����foo.o: %.o : %.c  ����Ϊ:������%.oƥ��foo.o������ɹ�%�͵���foo��������ɹ���  
# Make�ͻᾯ�棬Ȼ�󡣸�foo.o��������ļ�foo.c(��foo�滻��%.c���%)  
# Ҳ���Բ�Ҫ�����������ɹ�����Ϊ����� include $(DEF)  �������ˡ��˴�Ϊ�����ˣ��׶���������  
$(COBJS) : %.o: %.c  
	$(CC) $(CFLAGS) -c $< -o $@   
$(CPPOBJS) : %.o: %.cpp  
	$(CPP) $(CPPFLAGS) -c $< -o $@   
  
# $@--Ŀ���ļ���$^--���е������ļ���$<--��һ�������ļ���ÿ��$< $@ �����ֵ�����б��е�  
#  
$(BIN) : $(COBJS) $(CPPOBJS)  
	$(CC) -o $(BIN) $(CPPOBJS) $(COBJS)  $(LDFLAGS) $(LIBS)   
#	rm $(COBJS)  
#	rm $(CPPOBJS)  
# ����Ϊ����Ŀ��  
  
  
#������.o�ļ���.c��.h������������Ժ�.h���޸�Ҳ���������ɣ��ɿ���.d�ļ����ݼ�֪��Ϊ��  
#�������������൱��������һϵ�еĹ�����Ϊ�����������磺 Ŀ¼/xxx.o:Ŀ¼/xxx.c Ŀ¼/xxx.h Ҳ�൱�����������������ɹ���  
#�����治���ڳ����磺 $(OBJS) : $(DEF)֮�ࡣ�м�  
.PHONY:clean cleanall  
  
#�������Ŀ���ļ��Լ����ɵ�����Ŀ���ļ�  
clean:              
	rm $(BIN)   *.o
#rm *.d  
cleanall:  
	rm $(BIN) $(COBJS) $(CPPOBJS)  
