/* dev driver test application
File : app.c
Auth : sk7755@naver.com*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <syscall.h>

typedef enum state{
    STOP = 0, UP = 1, DOWN = 3
}state;

int main(void)
{
    int current_floor;
    int current_state;
    int elevators[8];
    int floors[8]; 
    int i;

    printf("Current Floor :");
    scanf("%d",&current_floor);
    printf("Current State(0 = stop, 1 = up, 3 = down) :");
    scanf("%d",&current_state);
    
    printf("Pushed Floors(0 = none, 1 = up, 3 = down) :");
    for(i=1;i<=7;i++){
        scanf("%d",&floors[i]);
    }
    printf("Pushed Elevators(0 = none, 1 = pushed) :");
    for(i=1;i<=7;i++){
        scanf("%d",&elevators[i]);
    }
    int retval = syscall(376,current_floor, current_state, floors, elevators);

    printf("retval : %d\n",retval); 
	return 0;
}
