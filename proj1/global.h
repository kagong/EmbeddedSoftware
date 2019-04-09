#ifndef GLOBAL_H
#define GLOBAL_H
#define BUF_SIZE 32
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
#endif
