#include <linux/kernel.h>

#define NUM_FLOOR 7

//Compute Next floor which elevator must go
//int current_floor 	: current floor where elevator exists
//int current_state 	: current state of elevator (up = 1, down = -1, stop = 0)
//int *u_pushed_floor 	: pushed buttons in each floor
//int *u_pushed_elevator 	: pushed buttons inside elevator (up = 1, down = -1, none = 0)
asmlinkage long sys_next_floor(int current_floor, int current_state, int *u_pushed_floor, int *u_pushed_elevator)
{
	int total_floor = NUM_FLOOR;	//the number of floors in which elevator can move
	int pushed_floor[7];
	int pushed_elevator[7];
	
	copy_from_user(pushed_floor,u_pushed_floor,sizeof(int) * NUM_FLOOR);
	copy_from_user(pushed_elevator,u_pushed_elevator,sizeof(int) * NUM_FLOOR);

	int i;
	if(current_state == 0){	//stop
		for(i = 0 ; i < total_floor ; i++){
			if(pushed_floor[i] || pushed_elevator[i])
				return i; // go i(th) floor
		}
	}
	else(current_state == 1){ //up
		for(i = current_floor + 1; i < total_floor ; i++ ){
			if(pushed_floor[i] == 1 || pushed_elevator[i])
				return i; // go i(th) floor (up -> up)
		}
		for(i = current_floor - 1; i >= 0; i--){
			if(pushed_floor[i] == -1 || pushed_elevator[i])
				return i; // go i(th) floor (up -> down)
		}
		return current_floor; //(up -> stop)
	}
	else{ //down
		for(i = current_floor - 1; i >= 0; i--){
			if(pushed_floor[i]|| pushed_elevator[i])
				return i; // go i(th) floor (down -> down)
		}
		for(i = current_floor + 1; i < total_floor ; i++ ){
			if(pushed_floor[i] || pushed_elevator[i])
				return i; // go i(th) floor (down -> up)
		}
		return current_floor; //(down -> stop)
	}
}
