#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<unistd.h>
#include<time.h>

#include "mode_functions.h"
#include "global.h"
#include "macros.h"

//datas for dot_matrix
//they represent either "A" or "1"
const unsigned char A_1[2][10]={
    {0x1c,0x36,0x63,0x63,0x63,0x7f,0x7f,0x63,0x63,0x63}, // A
    {0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e}, // 1
};

/* NAME: modes_functions
 * this functions work for mode
 * they modify states of the devices
 * and generate an output msg corresponding to the states of the devices
 */
void mode_clock(msg_input *imsg, fpga_devices *now, msg_output *omsg){
    int hour,min,idx,i;
    time_t timer;
    struct tm *t;
    timer = time(NULL);
    t = localtime(&timer);
    hour = t->tm_hour;
    min = t -> tm_min;
    if(imsg == NULL){//init mode, and generate init msg to be sent to the output_process
        now->flags = BIT1;      //led_1 set

        now->fnd_data[0] = hour / 10 ;  //time set
        now->fnd_data[1] = hour % 10 ;
        now->fnd_data[2] = min  / 10 ;
        now->fnd_data[3] = min  % 10 ;
        
        //for time increase checking
        now ->prev_h =  hour;
        now ->prev_m =  min;
        goto end;
    }
    for(i = 0 ; i < 10 ; i++)
        if(imsg->data.buf_switch[i] == 1){
            idx = i;
            break;
        }
    if(idx > 10){
        printf("mode - swich input error!\n");
        return;
    }
        switch(idx+1){
        case 1: //save now time or enter time changing mode
            if ( now-> flags & BIT3){
                now -> prev_h = hour;
                now -> prev_m = min;
            }
            //now->flags's 3rd bit mean changing mode flag
            now -> flags ^= BIT3;
            break;
        case 2: // reset
            now->flags = BIT1;
            now->fnd_data[0] = hour / 10;
            now->fnd_data[1] = hour % 10;
            now->fnd_data[2] = min  / 10;
            now->fnd_data[3] = min  % 10;
            now ->prev_h =  hour;
            now ->prev_m =  min;
            break;
        case 3: //In time changing mode, hour increasing
            if(now->flags & BIT3){
                TIME_UP_HOUR;
            }
            break;
        case 4: //In time changing mode, min increasing
            if(now->flags & BIT3){
                TIME_UP_MIN;
            }
            break;
    }
end:
    omsg -> msgtype =2; 
    omsg->fnd_data[0] = now -> fnd_data[0];
    omsg->fnd_data[1] = now -> fnd_data[1];
    omsg->fnd_data[2] = now -> fnd_data[2];
    omsg->fnd_data[3] = now -> fnd_data[3];
    omsg -> flags = now -> flags;

}
/* NAME: mode_clock_checker
 * this function work for time increasing by nature
 * if we are not in time changing mode and time increasing by nature
 * then we increase time data of devices and generate msg to be sent to the output_process
 */
void mode_clock_checker(fpga_devices *now,msg_output *omsg){
    int hour,min,i;
    time_t timer;
    struct tm *t;
    timer = time(NULL);
    t = localtime(&timer);
    hour = t->tm_hour;
    min = t -> tm_min;
    if((now->flags & BIT3) == 0 && min != now -> prev_m){
        now -> prev_m = min;
        now -> prev_h = hour;
        TIME_UP_MIN;
        omsg -> msgtype = 2;
        omsg->fnd_data[0] = now -> fnd_data[0];
        omsg->fnd_data[1] = now -> fnd_data[1];
        omsg->fnd_data[2] = now -> fnd_data[2];
        omsg->fnd_data[3] = now -> fnd_data[3];
        omsg -> flags = now -> flags;
    }

}
/* NAME: modes_functions
 * this functions work for mode
 * they modify states of the devices
 * and generate an output msg corresponding to the states of the devices
 */
//we set the notation infomation in now->flags
void mode_counter(msg_input *imsg, fpga_devices *now, msg_output *omsg){
    int i,prev_n,n,target,result=0;
    if(imsg == NULL){//init mode, and generate init msg to be sent to the output_process
        now->flags = BIT2;
        goto end_counter;
    }
    for(i = 0 ; i < 4 ; i++)
        if(imsg->data.buf_switch[i] == 1)
            break;
    if(i>3)
        return ;
    target = i;
    switch(target){
        //bin n=0, dec n=1, oct n=2, quad n=3
        case 0://change notation in fnd_data
            prev_n = FLAG_TO_NOTATION;  //get now notation by now -> flags
            now->flags >>= 1;           //flag change 10 -> 8 ->4 -> 2 -> 10
            if(now -> flags < BIT4)     
                now -> flags = BIT1;

            n = FLAG_TO_NOTATION;       //get notation changed
            CHANGE_NOTATION(prev_n,n);  //change the data correct in fnd_data
            break;
        case 1:                         //increase the data and make the data correct
        case 2:
        case 3:
            now->fnd_data[i] += 1;      
            n = FLAG_TO_NOTATION;
            CHANGE_NOTATION(n,n);
            break;
    }
end_counter:
    omsg -> msgtype = 3; 
    omsg->fnd_data[0] = now -> fnd_data[0] = 0;
    omsg->fnd_data[1] = now -> fnd_data[1];
    omsg->fnd_data[2] = now -> fnd_data[2];
    omsg->fnd_data[3] = now -> fnd_data[3];
    omsg -> flags = now -> flags; 

}
/* NAME: modes_functions
 * this functions work for mode
 * they modify states of the devices
 * and generate an output msg corresponding to the states of the devices
 */
void mode_text_editor(msg_input *imsg, fpga_devices *now, msg_output *omsg){
    char *ABC = ".ADGJMPTWQBEHKNRUXZCFILOSVY";
    int i,n=0,match_idx,target_idx,result = 0;
    char target = 0;
    if(imsg == NULL){//init mode, and generate init msg to be sent to the output_process
        for(i = 0 ; i < 10 ; i++)               //At first,we want to be diplayed "A"             
            now -> dot_matrix[i] = A_1[0][i];
        goto end_text_edit;
    }
    for(i = 0 ; i < 9 ; i++){
        if(imsg -> data.buf_switch[i] == 1){
            n++;
            match_idx = i;
        }
    }
    if(n > 2 || n == 0){
        return ;
    }
    else if(n == 2){
        //sw 2 & 3 :clear
        if(imsg -> data.buf_switch[1] && imsg -> data.buf_switch[2]){
            memset(now->text_data,0,sizeof(now->text_data));
            now -> text_idx = 0;
        }
        //sw 5&6 :change a -> 1 (now ->flags = 1 or 0)
        else if(imsg -> data.buf_switch[4] && imsg -> data.buf_switch[5]){
            now -> flags = !now -> flags;

            for(i = 0 ; i < 10 ; i++)
                now -> dot_matrix[i] = A_1[now -> flags][i];
        }
        //sw 8 & 9 : blank
        else if(imsg -> data.buf_switch[7] && imsg -> data.buf_switch[8]){
            target = ' ';
        }
        else
            return ;
    }
    else{
        if(!now -> flags ){
            target = ABC[match_idx];
            for(i = 0 ; now -> text_idx !=0 && i < 3 ; i++){    //check either new charater or prev charater that convert to next
                if(now -> text_data[now->text_idx-1] == ABC[9*i+match_idx]){
                    target = ABC[9*((i+1)%3)+match_idx];
                    now->text_idx --;
                    break;
                }
            }
        }
        else
            target = match_idx + '1';
    }
    if(target != 0){            //calculate charater's index 
        if(now->text_idx == LEN_TEXT){
            for( i = 1 ; i < LEN_TEXT; i++)
                now -> text_data[i-1] = now -> text_data[i];
            now->text_data[i-1] = target;
        }
        else
            now->text_data[now->text_idx++] = target;
    }
    now->fnd_data[3] += 1;
    CHANGE_NOTATION(1,1);
end_text_edit:
    omsg -> msgtype = 4; 
    omsg->fnd_data[0] = now -> fnd_data[0];
    omsg->fnd_data[1] = now -> fnd_data[1];
    omsg->fnd_data[2] = now -> fnd_data[2];
    omsg->fnd_data[3] = now -> fnd_data[3];
    for(i=0;i<LEN_TEXT;i++)
        omsg -> data.mode3.text_data[i] = now -> text_data[i];
    for(i=0;i<10;i++)
        omsg -> data.mode3.dot_matrix[i] = now -> dot_matrix[i];


}
/* NAME: modes_functions
 * this functions work for mode
 * they modify states of the devices
 * and generate an output msg corresponding to the states of the devices
 */
//now -> flag means cursur should be flashing
void mode_draw_board(msg_input *imsg, fpga_devices *now, msg_output *omsg){
    int match_idx=-1,i,result;
    if(imsg == NULL){//init mode, and generate init msg to be sent to the output_process
        now -> flags = 1;   //flag set
        goto end_draw;
    }
    for(i=0 ; i < 9 ; i++){
        if(imsg->data.buf_switch[i] == 1){
            match_idx = i;
            break;
        }
    }
    if(i == 9)
        return ;
    switch(match_idx){
        case 0:             //reset
            now ->flags = 1;
            now ->cursur[0] = now ->cursur[1] = 0;
            memset(now->dot_matrix,0,sizeof(now->dot_matrix));
            break;
        case 1:             //cursur move to up
            now->cursur[0] = now ->cursur[0] - 1 < 0 ? now ->cursur[0] : now->cursur[0]-1;
            break;
        case 2:             //cursur flash flag set to 1 or 0
            now -> flags = !now -> flags;
            break;
        case 3:             //cursur move to left
            now->cursur[1] = now ->cursur[1] - 1 < 0 ? now ->cursur[1] : now->cursur[1]-1;
            break;
        case 4:             //change data correspoding to the cursur to !data
            now->dot_matrix[now->cursur[0]] ^= BIT2 >> now->cursur[1];
            break;
        case 5:             //cursur move to right
            now->cursur[1] = now ->cursur[1] + 1 >= 7 ? now ->cursur[1] : now->cursur[1]+1;
            break;
        case 6:             //clear
            memset(now->dot_matrix,0,sizeof(now->dot_matrix));
            break;
        case 7:             //cursur move to down
            now->cursur[0] = now ->cursur[0] + 1 >= 10 ? now ->cursur[0] : now->cursur[0]+1;
            break;
        case 8:             //reverse dot matrix
            for(i = 0 ; i < 10 ; i++)
                now->dot_matrix[i] ^= 0x7f;
            break;
    }
    now->fnd_data[3] += 1;
    CHANGE_NOTATION(1,1);
end_draw:
    omsg -> msgtype = 5; 
    omsg->fnd_data[0] = now -> fnd_data[0];
    omsg->fnd_data[1] = now -> fnd_data[1];
    omsg->fnd_data[2] = now -> fnd_data[2];
    omsg->fnd_data[3] = now -> fnd_data[3];
    omsg ->flags = now ->flags;
    omsg ->data.mode4.cursur[0] = now ->cursur[0];
    omsg ->data.mode4.cursur[1] = now ->cursur[1];
    for(i=0;i<10;i++)
        omsg -> data.mode4.dot_matrix[i] = now -> dot_matrix[i];
}
/* NAME: make_map
 * it used by mode_game
 * this generate map for level
 */
static void make_map(unsigned char map[3][3],int level){
    int n,r,i,prev_r = -1;
    for(i=0;i<9;i++)
        map[i/3][i%3] = 0;
    n = level * 3;
    for( i = 0 ; i < n ; i++){
        do{
            r = rand()%9;
        }while(r == prev_r);
        prev_r = r;
        PUSH_MAP(map,r);
    }
}
/* NAME: modes_functions
 * this functions work for mode
 * they modify states of the devices
 * and generate an output msg corresponding to the states of the devices
 */
void mode_game(msg_input *imsg, fpga_devices *now, msg_output *omsg){
    int i,j,temp,result=0;
    if(imsg == NULL){//init mode, and generate init msg to be sent to the output_process

        now -> flags = 1;                //level 1 setting
        make_map(now->map,now->flags);
        goto end;
    }
    now -> buzzer = 0;
    for(i=0 ; i < 9 ; i++){
        if(imsg->data.buf_switch[i] == 1){
            break;
        }
    }
    if(i>=9)
        return ;

    PUSH_MAP(now->map,i);               //ith switch is pushed. so, push to map
    for(i=0;i<9;i++){                   //check map
        if(now->map[i/3][i%3] == 0){
            break;
        }
    }
    if(i == 9){                         //level clear
        now -> buzzer = 1;              //buzzer on
        if(now->flags == 0xff)
            now->flags = 0;
        make_map(now->map,++now->flags);//generate next level
    }
    now->fnd_data[3] += 1;
    CHANGE_NOTATION(1,1);
end:
    for(i=0;i<3;i++){
        for(j=0;j<3;j++){
            temp = now->map[i][j];
            if(temp == 1){              //draw to dot matrix corresponding value of map
                now -> dot_matrix[i*3+0] |= 0x60 >> j*2;
                now -> dot_matrix[i*3+1] |= 0x60 >> j*2;
                now -> dot_matrix[i*3+2] |= 0x60 >> j*2;
            }
            else{
                now -> dot_matrix[i*3+0] &= ~(0x60 >> j*2);
                now -> dot_matrix[i*3+1] &= ~(0x60 >> j*2);
                now -> dot_matrix[i*3+2] &= ~(0x60 >> j*2);
            }
        }
    }
    omsg -> msgtype = 6; 
    omsg->flags = now -> flags;
    omsg->fnd_data[0] = now -> fnd_data[0];
    omsg->fnd_data[1] = now -> fnd_data[1];
    omsg->fnd_data[2] = now -> fnd_data[2];
    omsg->fnd_data[3] = now -> fnd_data[3];
    for(i=0;i<10;i++)
        omsg -> data.mode5.dot_matrix[i] = now -> dot_matrix[i];
    omsg ->data.mode5.buzzer = now -> buzzer;

}
