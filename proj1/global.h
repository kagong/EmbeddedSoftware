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
 *
 *
 *
 */
typedef struct _msg_input{
    long msgtype;
    union{
        unsigned int code;
        unsigned char buf_switch[MAX_BUTTON];
    }data;
}msg_input;

//msg type == 1 -> exit
//msg type == mode 2, 3, 4, 5, 6

//mode 1: flags 00000011 
//mode 2: flags 00001111
//mode 3: flags 00000000 
//mode 4: flags 00000001

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
