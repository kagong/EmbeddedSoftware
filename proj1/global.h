#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_BUTTON   9
#define BUF_SIZE    32
#define POWER_OFF   158
#define MODE_UP     115
#define MODE_DOWN   114

typedef struct _fpga_devices{
    unsigned char string[32];
    unsigned char fnd_data[4];
    unsigned int led_data;
    int dot_number;
}fpga_devices;
typedef struct _msg_input{
    long msgtype;
    union{
        unsigned int code;
        unsigned char buf_switch[MAX_BUTTON];
    }data;
}msg_input;
typedef struct _msg_output{
    long msgtype;
    char buf[BUF_SIZE];
}msg_output;
#endif
