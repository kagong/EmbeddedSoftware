#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/msg.h>
#include<unistd.h>
#include "global.h"
#include "input_process.h"
#include "output_process.h"

#define ERR false
#define NOMAL true

#define EXIT_HANDLING(flag) do{\
    if(flag == ERR)\
        perror("error!!\n");\
    msgsnd(key_output_id,&msg_error,sizeof(msg_input)-sizeof(long),0);\
    waitpid(pid_input,NULL,0);\
    waitpid(pid_output,NULL,0);\
    exit(0);\
}while(0)


void main_process(int,int);


void mode_clock(msg_input *imsg, fpga_devices *now, msg_output *omsg);
void mode_counter(msg_input *imsg, fpga_devices *now, msg_output *omsg);
void mode_text_editor(msg_input *imsg, fpga_devices *now, msg_output *omsg);
void mode_draw_board(msg_input *imsg, fpga_devices *now, msg_output *omsg);
void mode_foo(msg_input *imsg, fpga_devices *now, msg_output *omsg);

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
    msg_output omsg,msg_error;
    fpga_devices now;
    key_t key_input_id, key_output_id;
    void (*mode_functions[5])(msg_input*, fpga_devices *, msg_output*);
    short mode=0;

    printf("main start!\n");

    memset(&now,0,sizeof(fpga_devices));
    memset(&msg_error,0,sizeof(msg_error));
    msg_error.msgtype = 1;

    mode_functions[0] = mode_clock;
    mode_functions[1] = mode_counter;
    mode_functions[2] = mode_text_editor;
    mode_functions[3] = mode_draw_board;
    mode_functions[4] = mode_foo;

    key_input_id = msgget((key_t)875,IPC_CREAT|0666);
    key_output_id = msgget((key_t)5975,IPC_CREAT|0666);

    if(key_input_id == -1 || key_output_id == -1)
        EXIT_HANDLING(ERR);
    

    while(1){
        memset(&omsg,0,sizeof(omsg));
        memset(&imsg,0,sizeof(imsg));

        if (msgrcv( key_input_id, &imsg, sizeof(imsg)-sizeof(long), 0, IPC_NOWAIT) != -1)
        {
            if(imsg.msgtype == 1){
                switch(imsg.data.code){
                    case POWER_OFF:
                        EXIT_HANDLING(NOMAL);
                    case MODE_UP:
                        mode+=2;
                    case MODE_DOWN:
                        mode -=1;
                        mode%=5;
                        memset(&now,0,sizeof(fpga_devices));
                        omsg.msgtype = mode+2;
                        break;
                }
            }
            else
                mode_functions[mode](&imsg,&now,&omsg);
        }
        //else
          //  mode_functions[mode](NULL,&now,&omsg);

        if (omsg.msgtype == 0)
            continue;

        if(msgsnd(key_output_id,&omsg,sizeof(omsg)-sizeof(long),0) == -1) 
            EXIT_HANDLING(ERR);
    }
}
//output msg가 없을 수도.
void mode_clock(msg_input *imsg, fpga_devices *now, msg_output *omsg){
}
void mode_counter(msg_input *imsg, fpga_devices *now, msg_output *omsg){
}
void mode_text_editor(msg_input *imsg, fpga_devices *now, msg_output *omsg){
}
void mode_draw_board(msg_input *imsg, fpga_devices *now, msg_output *omsg){
}
void mode_foo(msg_input *imsg, fpga_devices *now, msg_output *omsg){
}
