#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/msg.h>
#include<unistd.h>
#include<time.h>
#include "global.h"
#include "input_process.h"
#include "output_process.h"
#include "mode_functions.h"
#include "macros.h"

#define ERR false
#define NOMAL true
#define MODE_NUM 5
void main_process(int,int);


/* NAME : main
 * create child processes (input,output)
 * and call main_process
 */
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

/* NAME : main_process
 * this function work for main logic
 * they recevive input_msg from input_process
 * and call each mode's functions matched input_msg
 * then they receive an output msg is maked by the mode's function
 * thus send the output msg to output_process
 */
void main_process(pid_t pid_input,pid_t pid_output){
    msg_input imsg;
    msg_output omsg,msg_error;
    fpga_devices now;
    key_t key_input_id, key_output_id;
    void (*mode_functions[5])(msg_input*, fpga_devices *, msg_output*);     //functions table taken mode's functions
    short mode=0;
    int n;
    printf("main start!\n");

    //init process
    memset(&now,0,sizeof(fpga_devices));
    memset(&msg_error,0,sizeof(msg_error));
    msg_error.msgtype = 1;

    mode_functions[0] = mode_clock;
    mode_functions[1] = mode_counter;
    mode_functions[2] = mode_text_editor;
    mode_functions[3] = mode_draw_board;
    mode_functions[4] = mode_game;

    set_value();
    srand(time(NULL));  

    key_input_id = msgget((key_t)875,IPC_CREAT|0666);
    key_output_id = msgget((key_t)5975,IPC_CREAT|0666);

    if(key_input_id == -1 || key_output_id == -1)
        EXIT_HANDLING(ERR);

    //push init msg about mode1 clock
    mode_functions[0](NULL,&now,&omsg);
    msgsnd(key_output_id,&omsg,sizeof(omsg),0);

    while(1){
        //per itertaion, init msg
        memset(&omsg,0,sizeof(omsg));
        memset(&imsg,0,sizeof(imsg));

        //receive msg from input_process
        n = msgrcv( key_input_id, &imsg, sizeof(imsg), 0, IPC_NOWAIT);
        if (n != -1)
        {
            if(imsg.msgtype == 1){// readkey's input
                switch(imsg.data.code){
                    case POWER_OFF:
                        EXIT_HANDLING(NOMAL);
                    case MODE_UP:
                        mode+=2;
                    case MODE_DOWN:
                        mode += MODE_NUM-1;
                        mode %= MODE_NUM;
                        memset(&now,0,sizeof(fpga_devices));
                        omsg.msgtype = mode+2;
                        mode_functions[mode](NULL,&now,&omsg);//push init msg match mode's value
                        break;
                }
            }
            else//switch's input
                mode_functions[mode](&imsg,&now,&omsg);
        }
        else if(mode == 0)//if we didn't receive any msg and mode is clock , then check the increasing time
            mode_clock_checker(&now,&omsg);


        //case that we didn't receive msg
        if (omsg.msgtype == 0)
            continue;

        if(msgsnd(key_output_id,&omsg,sizeof(omsg),0) == -1) 
            EXIT_HANDLING(ERR);
    }
}

