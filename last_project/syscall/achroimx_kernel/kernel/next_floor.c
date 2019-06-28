#include <linux/kernel.h>
#include <linux/uaccess.h>

#define NUM_FLOOR 7

//Compute Next floor
//int current_floor 	: current floor where elevator exists
//int current_state 	: current state of elevator (up = 1, down = 3, stop = 0)
//int *u_pushed_floor 	: pushed buttons in each floor (up = 1, down = 2, updown = 3, none = 0)	INDEX = FLOOR
//						  array size = 8, index start = 1 
//int *u_pushed_elevator : pushed buttons inside elevator (up = 1, down = 3, none = 0)	INDEX = FLOOR
//						   array size = 8, index start = 1 
//If success, return Next floor which elevator must go to
//If error return -1
asmlinkage long sys_next_floor(int current_floor, int current_state, int *u_pushed_floor, int *u_pushed_elevator)
{
	int total_floor = NUM_FLOOR;	//the number of floors in which elevator can move
	int pushed_floor[NUM_FLOOR + 1];
	int pushed_elevator[NUM_FLOOR + 1];
	int i;
    if(copy_from_user(pushed_floor,u_pushed_floor,sizeof(int) * (NUM_FLOOR+1)))
        return -1;
	if(copy_from_user(pushed_elevator,u_pushed_elevator,sizeof(int) * (NUM_FLOOR + 1)))
        return -1;

	if(current_state == 0){	//stop
		for(i = 1 ; i <= total_floor ; i++){
			if(pushed_floor[i] || pushed_elevator[i])
				return i; // go i(th) floor
		}
	}
    else if(current_state == 1){ //up
		for(i = current_floor + 1; i <= total_floor ; i++ ){
			if(pushed_floor[i] == 1 || pushed_floor[i] == 3 || pushed_elevator[i])
				return i; // go i(th) floor (up -> up)
		}
		for(i = total_floor;i>current_floor;i--)
			if(pushed_floor[i] == 2)
				return i;
		for(i = current_floor - 1; i> 0 ;i--){
			if(pushed_floor[i] == 2 || pushed_floor[i] == 3 || pushed_elevator[i])
				return i; 
		}
		for(i = 1 ; i< current_floor ; i++)
			if(pushed_floor[i] == 1)
				return i;
	}
	else{ //down
		for(i = current_floor - 1; i > 0; i--){
			if(pushed_floor[i] == 2 || pushed_floor[i] == 3|| pushed_elevator[i])
				return i; // go i(th) floor (down -> down)
		}
		for(i = 1 ;i < current_floor;i++)
			if(pushed_floor[i] == 1)
				return i;

		for(i = current_floor + 1; i <= total_floor ; i++ ){
			if(pushed_floor[i] == 1 || pushed_floor[i] == 3 || pushed_elevator[i])
				return i; // go i(th) floor (down -> up)
		}
		for(i = total_floor; i>current_floor;i--)
			if(pushed_floor[i] == 2)
				return i;
	}

    return current_floor;
}
