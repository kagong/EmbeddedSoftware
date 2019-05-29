#ifndef _MACROS_H
#define _MACROS_H

/*
 * NAME:_exp , max_notation
 * these will be used by macros related to the notation
 */
extern int _exp[4][4];
extern int max_notation[4];

void set_value();

#define PWD_SWITCH "/dev/fpga_push_switch"
#define PWD_READKEY "/dev/input/event0"

#define FPGA_DOT_DEVICE "/dev/fpga_dot"
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define FPGA_FND_DEVICE "/dev/fpga_fnd"
#define FPGA_LED_DEVICE "/dev/mem"
#define FPGA_BUZZER_DEVICE "/dev/fpga_buzzer"

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

//--------------------------macro functions
/* NAME : exit_handlings
 * this macros work sensitive things at the end 
 * In following actions, it take right actions
 * 1. close file descriptor
 * 2. push the exit msg to child process or parent process
 * 3. wait child process
 * 4. err msg print
 * 5. exit process myself
 */
#define EXIT_HANDLING_INPUT do{\
    close(dev_switch);\
    close(dev_readkey);\
    msgsnd(key_id,&msg_error,sizeof(msg_input)-sizeof(long),0);\
    exit(0);\
}while(0)
#define EXIT_HANDLING_OUTPUT do{\
    close(dev_dot);\
    close(dev_text);\
    close(dev_fnd);\
    close(dev_led);\
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

/* NAME : TIME HANDLING MACROS
 * following macros work for time data in fpga_fnd
 * It fix the datas of fpga_fnd to right datas(Time observe 24:00)
 */
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

/* NAME : FlAG_TO_NOTATION
 * this function change flags to notation number 
 * 0 -> binary
 * 1 -> decimal
 * 2 -> octal
 * 3 -> quad
 */
#define FLAG_TO_NOTATION  ((now -> flags & 0x40) != 0) * 1 + ((now -> flags &0x20) != 0) * 2 + ((now ->flags &0x10)!=0) * 3 
/*
 * NAME: CHANGE_NOTATION
 * this function fix the fnd data to right notation data
 */
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


/* NAME : INIT_DEV
 * In output_process, they want to init their devices, the data shown 
 * so, init and print 0
 */
#define INIT_DEV do{\
    memset(&devices,0,sizeof(devices));\
    *led_addr = 0;\
    write(dev_dot,devices.dot_matrix,sizeof(devices.dot_matrix));\
    write(dev_fnd,&fnd_init,4);\
    write(dev_text,text_init,LEN_TEXT);\
}while(0)

/* NAME : PUSH_MAP
 * In mode_game function , 
 * push the data in the correct coordinates 
 */
#define PUSH_MAP(game_map,r)do{\
    unsigned char x,y;\
    y = r%3;\
    x = r/3;\
    game_map[x][y] ^= 1;\
    if(x > 0)\
    game_map[x-1][y] ^= 1;\
    if(x < 2)\
    game_map[x+1][y] ^= 1;\
    if(y > 0)\
    game_map[x][y-1] ^= 1;\
    if(y < 2)\
    game_map[x][y+1] ^= 1;\
}while(0)

#endif
