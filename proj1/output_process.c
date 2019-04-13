#include "output_process.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/msg.h>
#include<unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include<time.h>
#include "global.h"

#define FPGA_DOT_DEVICE "/dev/fpga_dot"
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define FPGA_FND_DEVICE "/dev/fpga_fnd"
#define FPGA_LED_DEVICE "/dev/mem"
#define FPGA_BASE_ADDRESS 0x08000000
#define LED_ADDR 0x16				

#define INIT_DEV do{\
    memset(&devices,0,sizeof(devices));\
    *led_addr = 0;\
    write(dev_fnd,&devices.fnd_data,4);\
    write(dev_text,devices.text,LEN_TEXT);\
    write(dev_dot,devices.dot_matrix,sizeof(devices.dot_matrix));\
}while(0)
void output_process(){
    msg_output msg;
    key_t key_id;
    unsigned long *fpga_addr = 0;
    unsigned char *led_addr =0;

    int i, mode = 2;
    int dev_dot,dev_text,dev_fnd,dev_led;

    struct _devices{
        unsigned char flash_led_34_flag;
        unsigned char flash_cursur_flag;
        unsigned char fnd_data[4];
        unsigned char led_data;
        unsigned char cursur[2];
        unsigned char text[LEN_TEXT];
        unsigned char dot_matrix[10];
        time_t time_stamp;
    }devices;

    printf("output start!\n");

    memset(&msg,0,sizeof(msg));

    dev_dot  = open(FPGA_DOT_DEVICE, O_WRONLY);
    dev_text = open(FPGA_TEXT_LCD_DEVICE, O_WRONLY);
    dev_fnd  = open(FPGA_FND_DEVICE, O_WRONLY);
    dev_led = open( FPGA_LED_DEVICE, O_RDWR | O_SYNC); //memory device open

    if(dev_dot < 0 || dev_text < 0 || dev_fnd < 0){
        printf("error!\n");
        close(dev_dot);
        close(dev_text);
        close(dev_fnd);
        exit(0);
    }

    fpga_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED,
            dev_led, FPGA_BASE_ADDRESS);
    led_addr=(unsigned char*)((void*)fpga_addr+LED_ADDR);
    if (fpga_addr == MAP_FAILED) {
        printf("error\n");
        exit(0);
    }

    INIT_DEV;

    key_id = msgget((key_t)5975,IPC_CREAT|0666);
    if(key_id == -1){
        perror("error!!\n");
        munmap(led_addr, 4096); 
        close(dev_dot);
        close(dev_text);
        close(dev_fnd);
        exit(0);
    }
    int temp;
    while(1){
        usleep(4000);
        temp = msgrcv( key_id, &msg, sizeof(msg), 0,IPC_NOWAIT) ;
        if(temp != -1){
            if(msg.msgtype == 1){
                close(dev_dot);
                close(dev_text);
                close(dev_fnd);
                exit(1);
            }
            if(mode != msg.msgtype){
                mode = msg.msgtype;
                INIT_DEV;
                continue;
            }

            for(i = 0 ; i < 4 ; i++)
                devices.fnd_data[i] = msg.fnd_data[i];

            if(mode == 2){//clock
                devices.flash_led_34_flag = msg.flags & 1 << 5;
                if(!devices.flash_led_34_flag){
                    devices.led_data = 1<<7;
                    *led_addr = devices.led_data;
                }
                else{
                    time(&devices.time_stamp);
                    devices.led_data = 1<<5;
                    *led_addr = devices.led_data;
                }
                write(dev_fnd,&devices.fnd_data,4);
            }
            else if(mode == 3){//counter
                devices.led_data = msg.flags;
                *led_addr = devices.led_data;
                write(dev_fnd,&devices.fnd_data,4);
            }
            else if(mode == 4){//text
                for(i=0;i<10;i++)
                    devices.dot_matrix[i] = msg.data.mode3.dot_matrix[i];
                write(dev_text,msg.data.mode3.text_data,LEN_TEXT);	
                write(dev_dot,devices.dot_matrix,sizeof(devices.dot_matrix));
            }
            else if(mode == 5){//draw
                devices.flash_cursur_flag = msg.flags;
                devices.cursur[0] = msg.data.mode4.cursur[0];
                devices.cursur[1] = msg.data.mode4.cursur[1];
                for(i=0;i<10;i++)
                    devices.dot_matrix[i] = msg.data.mode4.dot_matrix[i];
                write(dev_fnd,&devices.fnd_data,4);
                write(dev_dot,devices.dot_matrix,sizeof(devices.dot_matrix));
            }


        }
        else{
            time_t timer;
            time(&timer);
            if(devices.flash_led_34_flag && (timer != devices.time_stamp) ){
                devices.time_stamp = timer;
                devices.led_data ^= (1<<5);
                devices.led_data ^= (1<<4);
                *led_addr = devices.led_data;
            }
            else if(devices.flash_cursur_flag && (timer != devices.time_stamp) ){

                devices.dot_matrix[devices.cursur[0]] ^= 0x40 >> devices.cursur[1];
                //                usleep(10000);
                devices.dot_matrix[devices.cursur[0]] ^= 0x40 >> devices.cursur[1];
            }
        }
    }
    close(dev_dot);
    close(dev_text);
    close(dev_fnd);
    exit(1);
}

