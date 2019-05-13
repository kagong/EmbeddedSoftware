#include <unistd.h>
#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc,char** argv){
    unsigned int interval,count,val;
    interval = atoi(argv[1]);
    count = atoi(argv[2]);
    val = atoi(argv[3]):
    unsigned int data = syscall(376,interval,count,val);
    open();
    ioctl();
    close(();
    return 0;
}
