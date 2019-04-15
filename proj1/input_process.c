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
#include "macros.h"


/* NAME : input_process
 * this functions read datas from either readkey or switch
 * And when it data change 1 to 0 , we assume that the key is entered
 * then we sent msg to main_process
 * if we send readkey's data then msgtype = 1
 * if we send switch's data then msgtype = 2
 */

void input_process(){
    struct input_event ev[EV_BUFF_SIZE],prev[3];
    int dev_switch,dev_readkey,sw_buf_size,ev_size = sizeof(struct input_event),i;
    msg_input msg,msg_error;
    key_t key_id;
    unsigned char sw_buf[MAX_BUTTON],prev_sw_buf[MAX_BUTTON];

    printf("input_process start!\n");
    //init processs
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
    dev_readkey = open(PWD_READKEY, O_RDWR|O_NONBLOCK);

    if (dev_switch < 0 || dev_readkey < 0){
        printf("Device Open Error\n");
        EXIT_HANDLING_INPUT;
    }

    sw_buf_size=sizeof(sw_buf);

    //input_logic
    while(1){
        usleep(70000);
        memset(&msg,0,sizeof(msg));

        //read readkeys's input
        if(read(dev_readkey, ev, ev_size * EV_BUFF_SIZE) >= ev_size){
            for(i = 0 ; i < 3 ; i++){
                if(prev[i].code == ev[0].code){
                    if(prev[i].value == 1 && ev[0].value == 0){
                        msg.msgtype = 1;
                        msg.data.code = ev[0].code; 
                    }
                    prev[i].value = ev[0].value;
                }
            }
        }
        else{//case that there is not any input by readkey
            //read switch's input 
            read(dev_switch,&sw_buf,sw_buf_size);
            for (i=0;i<MAX_BUTTON;i++){
                if(prev_sw_buf[i] == 1 && sw_buf[i] == 0){
                    msg.msgtype = 2;
                    msg.data.buf_switch[i] = 1;
                }
                prev_sw_buf[i] = sw_buf[i];
            }
        }
        if(msg.msgtype == 1 && msg.data.code == POWER_OFF){
            EXIT_HANDLING_INPUT;
        }
        else if(msg.msgtype != 0){
            int a = msgsnd(key_id,&msg,sizeof(msg),0);
            if(a == -1){
                perror("error!!\n");
                EXIT_HANDLING_INPUT;
            }
        }
    }
    exit(1);
}
