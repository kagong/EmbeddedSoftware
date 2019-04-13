CC = arm-none-linux-gnueabi-gcc
#CC = gcc
OBJS = main.o input_process.o output_process.o
TARGET = 20151575
 
.SUFFIXES : .c .o
	 
all : $(TARGET)
	 
clean :
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
#	$(CC) -o $@ $(OBJS)
	$(CC) -static -o $@ $(OBJS)

main.o: global.h input_process.h output_process.h
input_process.o: global.h input_process.c
output_process.o: global.h output_process.c
