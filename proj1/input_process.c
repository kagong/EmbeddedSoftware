#include "input_process.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/msg.h>
#include<unistd.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>
#include <signal.h>
#include "global.h"


#define MAX_BUTTON 9
#define PWD_SWITCH "/dev/fpga_push_switch"
#define PWD_READKEY "/dev/input/event0"
void input_process(){
	int dev_switch,dev_readkey;
    msg_input msg;
    key_t key_id;
	int buff_size;
    unsigned char push_sw_buff[MAX_BUTTON];
    printf("input start!\n");

    memset(&msg,0,sizeof(msg));

	dev_switch = open(PWD_SWITCH, O_RDWR);
    dev_readkey = open(PWD_READKEY, O_RDONLY);
    key_id = msgget((key_t)875,IPC_CREAT|0666);
    if(key_id == -1){//to do additional error handling
        perror("error!!\n");
        exit(0);
    }
	if (dev_switch<0 || dev_readkey){
		printf("Device Open Error\n");
		close(dev_switch);
		return ;
	}
	buff_size=sizeof(push_sw_buff);

    while(1){
        usleep(400000);
        //-----------switch------
        read(dev_switch,&push_sw_buff,buff_size);
        //-----------switch-----

        if(msgsnd(key_id,(void*)&msg,sizeof(msg),0) == -1){
            perror("error!!\n");
            exit(0);
        }
        sleep(1);
    }
    exit(1);
}
