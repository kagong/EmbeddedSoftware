#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/msg.h>
#include<unistd.h>
#include "global.h"
#include "input_process.h"
#include "output_process.h"

#define INIT(devices) do{\
}while(0)
//input device : switch, prog, vol+/-, back


void main_process(int,int);

static msg_output* mode_clock(msg_input *, fpga_devices *);
static msg_output* mode_counter(msg_input *, fpga_devices *);
static msg_output* mode_text_editor(msg_input *, fpga_devices *);
static msg_output* mode_draw_board(msg_input *, fpga_devices *);
static msg_output* mode_foo(msg_input *, fpga_devices *);

int main(){
    pid_t pid_input=0, pid_output =0;

    pid_input = fork();
    if(pid_input == 0){
        input_process();
        return 0;
    }
    else if(pid_input < 0){
        printf("fail!\n");
        return 0;
    }

    pid_output = fork();
    if(pid_output == 0){
        output_process();
        return 0;
    }
    else if(pid_output < 0){
        waitpid(pid_input,NULL,0);
        printf("fail!\n");
        return 0;
    }
    main_process(pid_input,pid_output);

    return 0;
}
void main_process(pid_t pid_input,pid_t pid_output){
    msg_input imsg;
    msg_output *omsg = NULL;
    fpga_devices now;
    key_t key_input_id, key_output_id;
    msg_output* (*mode_functions[5])(msg_input*, fpga_devices *);
    short mode=0;

    printf("main start!\n");

    memset(&imsg,0,sizeof(imsg));

    key_input_id = msgget((key_t)875,IPC_CREAT|0666);
    key_output_id = msgget((key_t)5975,IPC_CREAT|0666);

    mode_functions[0] = mode_clock;
    mode_functions[1] = mode_counter;
    mode_functions[2] = mode_text_editor;
    mode_functions[3] = mode_draw_board;
    mode_functions[4] = mode_foo;

    if(key_input_id == -1 || key_output_id == -1){
        perror("error!!\n");
        waitpid(pid_input,NULL,0);
        waitpid(pid_output,NULL,0);
        exit(0);
    }

    while(1){
        if (msgrcv( key_input_id, &imsg, sizeof(imsg)-sizeof(long), 0, IPC_NOWAIT) != -1)
        {
            if(imsg.msgtype == 1){
                switch(imsg.code){
                    case POWER_OFF:
                        // todo
                        waitpid(pid_output,NULL,0);
                        exit(0);
                        break;
                    case MODE_UP:
                        mode+=2;
                    case MODE_DOWN
                        mode -=1;
                        mode%=5;
                        INIT(now);
                        break;
                }
            }
            else
            omsg=mode_functions[mode](&imsg,&now);
        }
        else
            omsg=mode_functions[mode](NULL,&now);

        if (omsg == NULL)
            continue;

        if(msgsnd(key_output_id,(void*)&omsg,sizeof(omsg),0) == -1){
            perror("error!!\n");
            exit(0);
        }
    }
}
msg_output* mode_clock(msg_input *imsg, fpga_devices *now){
}
msg_output* mode_counter(msg_input *imsg, fpga_devices *now){
}
msg_output* mode_text_editor(msg_input *imsg, fpga_devices *now){
}
msg_output* mode_draw_board(msg_input *imsg, fpga_devices *now){
}
msg_output* mode_foo(msg_input *imsg, fpga_devices *now){
}
