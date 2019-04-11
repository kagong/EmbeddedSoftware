#ifndef GLOBAL_H
#define GLOBAL_H

#define LEN_TEXT    32
#define MAX_BUTTON  9
#define BUF_SIZE    32
#define POWER_OFF   158
#define MODE_UP     115
#define MODE_DOWN   114

typedef struct _fpga_devices{
    unsigned char fnd_data[4];
    unsigned char flags; 
    unsigned char text_data[LEN_TEXT];
    unsigned char dot_matrix[10];
    unsigned char cursur[2];
}fpga_devices;

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

    }data;
}msg_output;
#endif
