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
#define TIME_UP(n,m) do{\
    now -> fnd_data[m] += 1;\
    if(now -> fnd_data[m] > 9){\
        now -> fnd_data[m] = 0;\
        now -> fnd_data[n] += 1;\
    }\
}while(0)

#define TIME_UP_HOUR do{\
    TIME_UP(0,1);\
    if(now ->fnd_data[0]*10 + now->fnd_data[1] > 24)\
    now -> fnd_data[0] = now -> fnd_data[1] = 0;\
}while(0);
#define TIME_UP_MIN do{\
    TIME_UP(2,3);\
    if(now ->fnd_data[2]*10 + now->fnd_data[3] > 60){\
        now -> fnd_data[2] = now -> fnd_data[3] = 0;\
        TIME_UP_HOUR;\
    }\
}while(0);

#define FLAG_TO_NOTATION  now -> flags >> 6 * 1 + now -> flags >> 5 * 2 + now -> flags >> 4 * 3 
#define CHANGE_NOTATION(from,to) do{\
    for(i=0;i<4;i++)\
    result += now -> fnd_data[i]*_exp[from][i];\
    for(i=0;i<4;i++){\
        now -> fnd_data[i] = result / _exp[to][i];\
        result %= _exp[to][i];\
    }\
    if(now -> fnd_data[0] >= max_notation[to])\
    now -> fnd_data[0] = 0;\
}while(0)
const int _exp[4][4]={
    {1<<3,1<<2,1<<1,1},
    {1000,100,10,1},
    {1<<9,1<<6,1<<3,1},
    {1<<6,1<<4,1<<2,1}
};
const int max_notation[4]={2,10,8,4};
const unsigned char A_1[2][10]={
    {0x1c,0x36,0x63,0x63,0x63,0x7f,0x7f,0x63,0x63,0x63},
    {0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e}, // 1
};
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
    int hour,min,i;
    time_t timer;
    struct tm *t;
    timer = time(NULL);
    t = localtime(&timer);
    hour = t->tm_hour;
    min = t -> tm_min;
    if(now->flags == 0){//start mode 1
        now->flags = 1<<7;      //led_1
        now->fnd_data[0] = hour / 10 ;
        now->fnd_data[1] = hour % 10 ;
        now->fnd_data[2] = min  / 10 ;
        now->fnd_data[3] = min  % 10 ;
        now ->prev_h =  hour;
        now ->prev_m =  min;
    }
    for(i = 0 ; i < 10 ; i++)
        if(imsg->data.buf_switch[i] == 1)
            break;

    if(i > 10){
        printf("error!\n");
        return;
    }
    switch(i+1){
        case 2:

            now->fnd_data[0] = hour / 10;
            now->fnd_data[1] = hour % 10;
            now->fnd_data[2] = min  / 10;
            now->fnd_data[3] = min  % 10;
            now->flags = 0;

            break;
        case 1:
            if ( now-> flags == 0 )
                now->flags = 1;
            else{
                now -> prev_h = hour;
                now -> prev_m = min;
                now -> flags = 0;
            }
            break;
        case 3:
            if(now->flags){
                TIME_UP_HOUR;
            }
            break;
        case 4:
            if(now->flags){
                TIME_UP_MIN;
            }
            break;
    }
    if(now->flags == 0 && min != now -> prev_m){
        now -> prev_m = min;
        now -> prev_m = hour;
        TIME_UP_MIN;
    }
    omsg -> msgtype =2; 
    omsg->fnd_data[0] = now -> fnd_data[0];
    omsg->fnd_data[1] = now -> fnd_data[1];
    omsg->fnd_data[2] = now -> fnd_data[2];
    omsg->fnd_data[3] = now -> fnd_data[3];
    omsg -> flags = now -> flags;

}
void mode_counter(msg_input *imsg, fpga_devices *now, msg_output *omsg){
    int i,prev_n,n,target,result=0;
    if(now -> flags == 0){
        now->flags = 1<< 6;
    }
    for(i = 0 ; i < 4 ; i++)
        if(imsg->data.buf_switch[i] == 1)
            break;
    if(i>3)
        return ;
    target = i;
    switch(target){
        //bin n=0, dec n=1, oct n=2, quad n=3
        case 0:
            prev_n = FLAG_TO_NOTATION;

            now->flags >>= 1;
            if(now -> flags < 1 << 4)
                now -> flags = 1<< 7;

            n = FLAG_TO_NOTATION;
            CHANGE_NOTATION(prev_n,n);
            break;
        case 1:
        case 2:
        case 3:
            now->fnd_data[i] += 1;
            n = FLAG_TO_NOTATION;
            CHANGE_NOTATION(n,n);
            break;
    }

    omsg -> msgtype = 3; 
    omsg->fnd_data[0] = now -> fnd_data[0] = 0;
    omsg->fnd_data[1] = now -> fnd_data[1];
    omsg->fnd_data[2] = now -> fnd_data[2];
    omsg->fnd_data[3] = now -> fnd_data[3];
    omsg -> flags = now -> flags; 

}
void mode_text_editor(msg_input *imsg, fpga_devices *now, msg_output *omsg){
    //all 27 char, splity by 9
    //now ->text_idx is next of last text
    char *ABC = ".ADGJMPTWQBEHKNRUXZCFILOSVY";
    int i,n=0,match_idx,target_idx,result = 0;
    char target = 0;
    if(now -> dot_matrix[0] == 0){//start_mode3
        for(i = 0 ; i < 10 ; i)
            now -> dot_matrix[i] = A_1[0][i];
    }
    for(i = 0 ; i < 9 ; i++)
        if(imsg -> data.buf_switch[i] == 1){
            n++;
            match_idx = i;
        }

    if(n > 2 || n == 0){
        return ;
    }
    else if(n == 2){
        //sw 2 & 3 clear
        if(imsg -> data.buf_switch[1] && imsg -> data.buf_switch[2]){
            memset(now->text_data,0,sizeof(now->text_data));
            now -> text_idx = 0;
        }
        //sw 5&6 change a -> 1 (now ->flags = 1 or 0)
        else if(imsg -> data.buf_switch[4] && imsg -> data.buf_switch[5]){
            now -> flags = !now -> flags;
            for(i = 0 ; i < 10 ; i)
                now -> dot_matrix[i] = A_1[now -> flags][i];
        }
        //sw 8 & 9 black
        else if(imsg -> data.buf_switch[7] && imsg -> data.buf_switch[8]){
            target = ' ';
        }
        else
            return ;
    }
    else{
        if(!now -> flags ){
            target = ABC[match_idx];
            for(i = 0 ; now -> text_idx !=0 && i < 3 ; i++){
                if(now -> text_data[now->text_idx-1] == ABC[9*i+match_idx]){
                    target = ABC[9*((i+1)%3)+match_idx];
                    break;
                }
            }
        }
        else
            target = match_idx + '1';
    }
    if(target != 0){
        if(now->text_idx == LEN_TEXT){
            for( i = 1 ; i < LEN_TEXT; i++)
                now -> text_data[i-1] = now -> text_data[i];
            now->text_data[i-1] = target;
        }
        else
            now->text_data[now->text_idx++] = target;
    }

    now->fnd_data[3] += 1;
    CHANGE_NOTATION(10,10);

    omsg -> msgtype = 4; 
    omsg->fnd_data[0] = now -> fnd_data[0];
    omsg->fnd_data[1] = now -> fnd_data[1];
    omsg->fnd_data[2] = now -> fnd_data[2];
    omsg->fnd_data[3] = now -> fnd_data[3];
    for(i=0;i<LEN_TEXT;i++)
        omsg -> data.mode3.text_data[i] = now -> text_data[i];
    for(i=0;i<10;i++)
        omsg -> data.mode3.dot_matrix[i] = now -> dot_matrix[i];


}
//cursur [0] -> dot_matrix[#]
void mode_draw_board(msg_input *imsg, fpga_devices *now, msg_output *omsg){
    int match_idx=-1,i,result;
    for(i=0 ; i < 9 ; i++){
        if(imsg->data.buf_switch[i] == 1){
            match_idx = i;
            break;
        }
    }
    if(match_idx == 0)
        return ;
    switch(match_idx){
        case 0:
            now ->flags = now ->cursur[0] = now ->cursur[1] = 0;
            memset(now->dot_matrix,0,sizeof(now->dot_matrix));
            break;
        case 1:
            now->cursur[0] = now ->cursur[0] - 1 < 0 ? now ->cursur[0] : now->cursur[0]-1;
            break;
        case 2:
            now -> flags = !now -> flags;
            break;
        case 3:
            now->cursur[1] = now ->cursur[1] - 1 < 0 ? now ->cursur[1] : now->cursur[1]-1;
            break;
        case 4:
            now->dot_matrix[now->cursur[0]] ^= 0x30 >> now->cursur[1];
            break;
        case 5:
            now->cursur[1] = now ->cursur[1] + 1 < 7 ? now ->cursur[1] : now->cursur[1]+1;
            break;
        case 6:
            memset(now->dot_matrix,0,sizeof(now->dot_matrix));
            break;
        case 7:
            now->cursur[0] = now ->cursur[0] + 1 < 10 ? now ->cursur[0] : now->cursur[0]+1;
            break;
        case 8:
            for(i = 0 ; i < 10 ; i++)
                now->dot_matrix[i] ^= 0x7f;
            break;
    }
    now->fnd_data[3] += 1;
    CHANGE_NOTATION(10,10);

    omsg -> msgtype = 5; 
    omsg->fnd_data[0] = now -> fnd_data[0];
    omsg->fnd_data[1] = now -> fnd_data[1];
    omsg->fnd_data[2] = now -> fnd_data[2];
    omsg->fnd_data[3] = now -> fnd_data[3];
    omsg ->flags = now ->flags;
    omsg ->data.mode4.cursur[0] = now ->cursur[0];
    omsg ->data.mode4.cursur[1] = now ->cursur[1];
    for(i=0;i<10;i++)
        omsg -> data.mode3.dot_matrix[i] = now -> dot_matrix[i];
}
void mode_foo(msg_input *imsg, fpga_devices *now, msg_output *omsg){
}
