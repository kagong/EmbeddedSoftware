#include "output_process.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/msg.h>
#include<unistd.h>
#include "global.h"

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

