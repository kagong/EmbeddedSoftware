#ifndef GLOBAL_H
#define GLOBAL_H

#include<time.h>
#include<sys/types.h>
#include"macros.h"

/* NAME: fpga_devices
 * In main.c and mode_functions.c
 * This structure is stored now status of devices
 * and this will copy to output msg and send to output_process()
 */
 
typedef struct _fpga_devices{
    unsigned char fnd_data[4];
    unsigned char flags; 
    unsigned char text_data[LEN_TEXT];
    unsigned char text_idx;
    unsigned char dot_matrix[10];
    unsigned char cursur[2];
    int prev_h,prev_m;
    unsigned char map[3][3];
    unsigned char buzzer; 
}fpga_devices;

/*NAME: msg_input
 * In input_process.c
 * this structure is sent to main logic
 * if msgtype is 1 then this msg is type with readkey's input
 * if msgtype is 2 then this msg is type with switch's input
 */
typedef struct _msg_input{
    long msgtype;
    union{
        unsigned int code;
        unsigned char buf_switch[MAX_BUTTON];
    }data;
}msg_input;

/* NAME: msg_output
 * this structure has data ASAP write to real device
 * msgtype respresent the mode number (msgtype -1 == mode#)
 * if msgtype is 1 then that means exit process
 * flags keep led_data, flash flag, and so....
 */
typedef struct _msg_output{
    long msgtype;
    unsigned char fnd_data[4];
    unsigned char flags; 
    union{
        struct _mode3{
            unsigned char dot_matrix[10];
            unsigned char text_data[LEN_TEXT];
        }mode3;
        struct _mode4{
            unsigned char dot_matrix[10];
            unsigned char cursur[2];
        }mode4;
        struct _mode5{
            unsigned char dot_matrix[10];
            unsigned char buzzer;
        }mode5;

    }data;
}msg_output;
#endif
