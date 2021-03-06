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
#include "macros.h"

/* NAME: output_process()
 * this function receive msg from main_process()
 * and Using data of msg, write to the devices
 */
void output_process(){
    msg_output msg;
    key_t key_id;
    unsigned long *fpga_addr = 0;
    unsigned char *led_addr =0;

    unsigned char fnd_init[4] = {0,};
    unsigned char text_init[LEN_TEXT] = {0,};
    int i, mode,prev_flash_flag;
    int dev_dot,dev_text,dev_fnd,dev_led,dev_buzzer;

    struct _devices{     //data structure used by flash mode       
        unsigned char flash_led_34_flag;
        unsigned char flash_cursur_flag;
        unsigned char led_data;
        unsigned char cursur[2];
        unsigned char dot_matrix[10];
        time_t time_stamp;
    }devices;

    printf("output start!\n");
    
    //init

    memset(&msg,0,sizeof(msg));

    dev_dot  = open(FPGA_DOT_DEVICE, O_WRONLY);
    dev_text = open(FPGA_TEXT_LCD_DEVICE, O_WRONLY);
    dev_fnd  = open(FPGA_FND_DEVICE, O_WRONLY);
    dev_led = open( FPGA_LED_DEVICE, O_RDWR | O_SYNC); 
	dev_buzzer = open(FPGA_BUZZER_DEVICE, O_RDWR);

    if(dev_dot < 0  || dev_text < 0 || dev_fnd < 0|| dev_led < 0 ||dev_buzzer < 0){
        printf("OUTPUT- device open error\n");
        EXIT_HANDLING_OUTPUT;
    }

    fpga_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED,dev_led, FPGA_BASE_ADDRESS);
    led_addr=(unsigned char*)((void*)fpga_addr+LED_ADDR);

    if (fpga_addr == MAP_FAILED) {
        printf("error\n");
        EXIT_HANDLING_OUTPUT;
    }

    INIT_DEV;

    mode = 2;       //default mode is clock
    key_id = msgget((key_t)5975,IPC_CREAT|0666);
    if(key_id == -1){
        munmap(led_addr, 4096); 
        EXIT_HANDLING_OUTPUT;
    }
    int temp;
    while(1){
        usleep(4000);
        temp = msgrcv( key_id, &msg, sizeof(msg), 0,IPC_NOWAIT) ;
        if(temp != -1){
            if(msg.msgtype == 1){               //exit
                INIT_DEV;
                munmap(led_addr, 4096); 
                EXIT_HANDLING_OUTPUT;
            }
            if(mode != msg.msgtype){            //mode change , so init devices
                mode = msg.msgtype;
                INIT_DEV;
            }


            write(dev_fnd,&msg.fnd_data,4);
            if(mode == 2){                      //clock mode
                prev_flash_flag = devices.flash_led_34_flag;
                devices.flash_led_34_flag = msg.flags & BIT3;
                if(!devices.flash_led_34_flag){ //not flag
                    devices.led_data = BIT1;
                    *led_addr = devices.led_data;
                }
                else if(prev_flash_flag == 0){ //time at which set flag
                    time(&devices.time_stamp);
                    devices.led_data = BIT3;
                    *led_addr = devices.led_data;
                }
            }
            else if(mode == 3){             //counter mode
                devices.led_data = msg.flags;
                *led_addr = devices.led_data;
            }
            else if(mode == 4){             //text_editor mode
                for(i=0;i<10;i++)
                    devices.dot_matrix[i] = msg.data.mode3.dot_matrix[i];
                write(dev_text,msg.data.mode3.text_data,LEN_TEXT);	
                write(dev_dot,devices.dot_matrix,sizeof(devices.dot_matrix));
            }
            else if(mode == 5){             //draw mode
                devices.flash_cursur_flag = msg.flags;
                devices.cursur[0] = msg.data.mode4.cursur[0];
                devices.cursur[1] = msg.data.mode4.cursur[1];
                for(i=0;i<10;i++)
                    devices.dot_matrix[i] = msg.data.mode4.dot_matrix[i];
                write(dev_dot,devices.dot_matrix,sizeof(devices.dot_matrix));
            }
            else if(mode == 6){             //game mode
                
			    write(dev_buzzer,&msg.data.mode5.buzzer,1);
                devices.led_data = msg.flags;
                *led_addr = devices.led_data;
                for(i=0;i<10;i++)
                    devices.dot_matrix[i] = msg.data.mode5.dot_matrix[i];
                write(dev_dot,devices.dot_matrix,sizeof(devices.dot_matrix));
            }


        }
        else{
            time_t timer;
            time(&timer);

            //when time diff >= 1sec & flag set
            if(devices.flash_led_34_flag && (timer != devices.time_stamp) ){
                devices.time_stamp = timer;
                devices.led_data ^= BIT3 | BIT4;
                *led_addr = devices.led_data;
            }
            else if(devices.flash_cursur_flag && (timer != devices.time_stamp) ){

                devices.dot_matrix[devices.cursur[0]] ^= BIT2 >> devices.cursur[1];
                write(dev_dot,devices.dot_matrix,sizeof(devices.dot_matrix));
                devices.time_stamp = timer;
            }
        }
    }
    munmap(led_addr, 4096); 
    EXIT_HANDLING_OUTPUT;
}

