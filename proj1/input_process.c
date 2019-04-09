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
#include <linux/input.h>
#include "global.h"

#define ERR_HANDLING do{\
    close(dev_switch);\
    close(dev_readkey);\
    msgsnd(key_id,&msg_error,sizeof(msg_input)-sizeof(long),0);\
    exit(-1);\
}while(0)

#define BUFF_SIZE 64
//Buf 줄일수 있는지 확인
#define PWD_SWITCH "/dev/fpga_push_switch"
#define PWD_READKEY "/dev/input/event0"

void input_process(){
    struct input_event ev[BUFF_SIZE],prev[3];
    int dev_switch,dev_readkey,sw_buf_size,ev_size = sizeof(struct input_event),i;
    msg_input msg,msg_end;
    key_t key_id;
    unsigned char sw_buf[MAX_BUTTON],prev_sw_buf[MAX_BUTTON];

    memset(&msg_error,0,sizeof(msg_error));
    memset(&prev,0,3*sizeof(struct input_event));
    memset(&prev_sw_buf,0,MAX_BUTTON*sizeof(unsigned char));

    msg_error.msgtype = 1;
    msg_error.data.code = POWER_OFF;

    prev[0].code = POWER_OFF;
    prev[1].code = MODE_UP;
    prev[2].code = MODE_DOWN;
    prev[0].value = prev[1].value = prev[2].value = 0;

    key_id = msgget((key_t)875,IPC_CREAT|0666);
    if(key_id == -1){
        perror("error!!\n");
        exit(-1);
    }


    dev_switch = open(PWD_SWITCH, O_RDWR);
    dev_readkey = open(PWD_READKEY, O_RDONLY);

    if (dev_switch < 0 || dev_readkey < 0){
        printf("Device Open Error\n");
        ERR_HANDLING;
    }

    sw_buf_size=sizeof(sw_buf);

    while(1){
        usleep(400000);
        memset(&msg,0,sizeof(msg));
        //-----------switch------
        read(dev_switch,&sw_buf,sw_buf_size);
        //-----------readkey-----
        read(dev_readkey, ev, ev_size * BUFF_SIZE);
        //----------------------
        //안눌렸을때 정확히 해야함
        /*
           i = ev[0].code%3;
           if(prev[i].value == 1 && ev[0].value == 0){
           prev[i].value = ev[0].value;
           msg.msgtype = 1;
           msg.data.code = ev[0].code; 
           break;
           }

           prev[i].value = ev[0].value;
           */
        if(msg.type != 1){
            for (i=0;i<MAX_BUTTON;i++){
                if(prev_sw_buf[i] == 1 && sw_buf_size[i] == 0){
                    msg.msgtype = 2;
                    msg.data.buf_switch[i] = 1;
                    break;
                }
            }
        }
        if(msg.msgtype != 0 && msgsnd(key_id,(void*)&msg,sizeof(msg),0) == -1){
            perror("error!!\n");
            ERR_HANDLING;
        }
    }
    exit(1);
}
