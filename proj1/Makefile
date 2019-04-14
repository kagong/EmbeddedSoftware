#CC = arm-none-linux-gnueabi-gcc
CC = gcc
OBJS = main.o input_process.o output_process.o mode_functions.o macros.o
TARGET = 20151575
 
.SUFFIXES : .c .o
	 
all : $(TARGET)
	 
clean :
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)
#	$(CC) -static -o $@ $(OBJS)

main.o: global.h macros.h mode_functions.h input_process.h output_process.h
input_process.o: global.h macros.h input_process.c
output_process.o: global.h macros.h output_process.c
mode_functions.o: global.h macros.h mode_functions.c
macros.o: macros.c
