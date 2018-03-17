#  
# c.cpp混合编译的makefile模板  
#  
#  
LIBRARYPATH=/home/tester/dlp10/lib
  
BIN = dlp10_test 
CROSS = arm-linux-
CC = $(CROSS)gcc  
CPP = $(CROSS)g++  
LD = $(CROSS)ld  
#这里只加入库头文件路径及库路径  
INCS = -I${LIBRARYPATH}/Poco/include -I${LIBRARYPATH}/include
LIBS = -L${LIBRARYPATH}/Poco/lib  -L${LIBRARYPATH}/lib 
SUBDIRS =  
 
#  
#maintest.c tree/rbtree.c  多了子目录，那就直接添加 目录/*.c即可   所有的源文件--  .c文件列表  
CSRCS = $(wildcard ./*.c )  
CPPSRCS = $(wildcard ./*.cpp)  
#  
#所有的.o文件列表  
COBJS := $(CSRCS:.c=.o)  
CPPOBJS := $(CPPSRCS:.cpp=.o)  
#  

CFLAGS += $(INCS)  
CFLAGS += -O2 -Wall -g   
CPPFLAGS += $(INCS)  
CPPFLAGS += -O2 -Wall -g  
LDFLAGS +=  -lPocoNet -lPocoFoundation -lini -lpthread -lm -lrt -lstdc++ -static 
  
all:$(BIN)  
#$(OBJS):%.o :%.c  先用$(OBJS)中的一项，比如foo.o: %.o : %.c  含义为:试着用%.o匹配foo.o。如果成功%就等于foo。如果不成功，  
# Make就会警告，然后。给foo.o添加依赖文件foo.c(用foo替换了%.c里的%)  
# 也可以不要下面的这个生成规则，因为下面的 include $(DEF)  就隐含了。此处为了明了，易懂。故留着  
$(COBJS) : %.o: %.c  
	$(CC) $(CFLAGS) -c $< -o $@   
$(CPPOBJS) : %.o: %.cpp  
	$(CPP) $(CPPFLAGS) -c $< -o $@   
  
# $@--目标文件，$^--所有的依赖文件，$<--第一个依赖文件。每次$< $@ 代表的值就是列表中的  
#  
$(BIN) : $(COBJS) $(CPPOBJS)  
	$(CC) -o $(BIN) $(CPPOBJS) $(COBJS)  $(LDFLAGS) $(LIBS)   
#	rm $(COBJS)  
#	rm $(CPPOBJS)  
# 链接为最终目标  
  
  
#引入了.o文件对.c和.h的依赖情况。以后.h被修改也会重新生成，可看看.d文件内容即知道为何  
#引入了依赖就相当于引入了一系列的规则，因为依赖内容例如： 目录/xxx.o:目录/xxx.c 目录/xxx.h 也相当于隐含的引入了生成规则  
#故上面不能在出现如： $(OBJS) : $(DEF)之类。切记  
.PHONY:clean cleanall  
  
#清除所有目标文件以及生成的最终目标文件  
clean:              
	rm $(BIN)   *.o
#rm *.d  
cleanall:  
	rm $(BIN) $(COBJS) $(CPPOBJS)  
