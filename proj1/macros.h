#ifndef _MACROS_H
#define _MACROS_H

extern int _exp[4][4];
extern int max_notation[4];

void set_value();

#define PWD_SWITCH "/dev/fpga_push_switch"
#define PWD_READKEY "/dev/input/event0"

#define FPGA_DOT_DEVICE "/dev/fpga_dot"
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define FPGA_FND_DEVICE "/dev/fpga_fnd"
#define FPGA_LED_DEVICE "/dev/mem"

#define FPGA_BASE_ADDRESS   0x08000000
#define LED_ADDR            0x16				
#define EV_BUFF_SIZE        64
#define LEN_TEXT            32
#define MAX_BUTTON          9
#define BUF_SIZE            32
#define POWER_OFF           158
#define MODE_UP             115
#define MODE_DOWN           114

#define BIT1                0x80
#define BIT2                0x40
#define BIT3                0x20
#define BIT4                0x10
#define BIT5                0x08
#define BIT6                0x04
#define BIT7                0x02
#define BIT8                0x01

//--------------------------macro function
#define EXIT_HANDLING_INPUT do{\
    close(dev_switch);\
    close(dev_readkey);\
    msgsnd(key_id,&msg_error,sizeof(msg_input)-sizeof(long),0);\
    exit(0);\
}while(0)

#define EXIT_HANDLING(flag) do{\
    if(flag == false)\
    perror("error!!\n");\
    msgsnd(key_output_id,&msg_error,sizeof(msg_input),0);\
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
    if(now ->fnd_data[0]*10 + now->fnd_data[1] >= 24)\
    now -> fnd_data[0] = now -> fnd_data[1] = 0;\
}while(0)

#define TIME_UP_MIN do{\
    TIME_UP(2,3);\
    if(now ->fnd_data[2]*10 + now->fnd_data[3] >= 60){\
        now -> fnd_data[2] = now -> fnd_data[3] = 0;\
        TIME_UP_HOUR;\
    }\
}while(0)

#define FLAG_TO_NOTATION  ((now -> flags & 0x40) != 0) * 1 + ((now -> flags &0x20) != 0) * 2 + ((now ->flags &0x10)!=0) * 3 
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


#define INIT_DEV do{\
    memset(&devices,0,sizeof(devices));\
    *led_addr = 0;\
    write(dev_fnd,&devices.fnd_data,4);\
    write(dev_text,devices.text,LEN_TEXT);\
    write(dev_dot,devices.dot_matrix,sizeof(devices.dot_matrix));\
}while(0)

#endif
