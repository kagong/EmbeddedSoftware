#include <unistd.h>
#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
int main(int argc,char** argv){
    unsigned int interval,count,val;
    int dev;
    interval = atoi(argv[1]);
    count = atoi(argv[2]);
    val = atoi(argv[3]);
    unsigned int data = syscall(376,interval,count,val);
    dev = open("/dev/dev_driver",O_WRONLY);
    if(dev <0 ){
        printf("open error!\n");
        return -1;
    }
    ioctl(dev,_IOR(242,0,unsigned int),data);
    close(dev);
    return 0;
}
