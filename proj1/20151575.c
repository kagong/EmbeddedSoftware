#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/msg.h>
#include<unistd.h>

#define BUF_SIZE 32
#define INIT(devices) do{\
}while(0)
//input device : switch, prog, vol+/-, back

typedef struct _fpga_devices{
    unsigned char string[32];
    unsigned char fnd_data[4];
    unsigned int led_data;
    int dot_number;
}fpga_devices;
typedef struct _msg_input{
    long msgtype;
    char buf[BUF_SIZE];
}msg_input;
typedef struct _msg_output{
    long msgtype;
    char buf[BUF_SIZE];
}msg_output;

void main_process(int,int);
void input_process();
void output_process();

msg_output* mode_clock(msg_input *, fpga_devices *);
msg_output* mode_counter(msg_input *, fpga_devices *);
msg_output* mode_text_editor(msg_input *, fpga_devices *);
msg_output* mode_draw_board(msg_input *, fpga_devices *);
msg_output* mode_foo(msg_input *, fpga_devices *);

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
        if (msgrcv( key_input_id, (void *)&imsg, sizeof(imsg), 1, IPC_NOWAIT) == 0)
        {
            if(0){
                // exit all
                waitpid(pid_output,NULL,0);
                exit(0);
            }
            else if(imsg.buf[0] == 'C'){
                //check imsg ,change mode?
                mode++;
                mode%=5;
                INIT(now);
            }
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
void input_process(){
    msg_input msg;
    key_t key_id;

    printf("input start!\n");

    memset(&msg,0,sizeof(msg));

    key_id = msgget((key_t)875,IPC_CREAT|0666);
    if(key_id == -1){
        perror("error!!\n");
        exit(0);
    }
    while(1){
        char temp = 0;
        msg.msgtype = 1;
        msg.buf[0] = ++temp;

        if(msgsnd(key_id,(void*)&msg,sizeof(msg),0) == -1){
            perror("error!!\n");
            exit(0);
        }
        sleep(1);
    }
    exit(1);
}
void output_process(){
    msg_output msg;
    key_t key_id;

    printf("output start!\n");

    memset(&msg,0,sizeof(msg));

    key_id = msgget((key_t)5975,IPC_CREAT|0666);
    if(key_id == -1){
        perror("error!!\n");
        exit(0);
    }
    while(1){

        if (msgrcv( key_id, (void *)&msg, sizeof(msg), 1,0) == 0){
            printf("In output, ");

        }
    }
    exit(1);
}

