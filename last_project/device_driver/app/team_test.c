#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define OUTPUT_MODE 1
#define INPUT_MODE 0


#define GETSWITCH_MODE 0
#define GETINTR_MODE 1
#define SETLED_MODE 3
#define SETDOT_MODE 4
#define SETBUZZER_MODE 5

#define DEV_NAME "/dev/elevator"

#define ELEV_UP 1
#define ELEV_DOWN 3

int make_data(int updown, int a[]){
	
	char ud, floor = 0;
	int res, i = 0;

	ud = (char)updown;

	while(i < 7){
		floor = floor << 1;
		if (a[i] == 1){
			floor = floor | 0x01;
		}
		i++;
	}

	printf("ff %x\n", floor);

	res = 0;
	res = ud;
	res = res << 24;
	res = res | floor;

	return res;

}


int main(int argc, char **argv){

	int inout, data = 0, dev;

	int i, updown, a[7];

	dev = open(DEV_NAME, O_RDWR);

	if (dev < 0){
		printf("Device open error!\n");
		return -1;
	}

	int qq, res;
	scanf("%d", &qq);

	while (qq > 0){

		printf("inout :? input == 0, output == 1\n");
		scanf("%d", &inout);
		if (inout >= 2){
			printf("up? down? 1 == up, 3 == down, 0 == stop\n");
			scanf("%d", &updown);
			printf("1 to 7 floor, 1 -> go, 0 -> no\n");
			for (i = 0 ; i < 7 ; i++)
				scanf("%d", a+i);

			data = make_data(updown, a);
		}
		printf("inout : %di data : %x\n", inout, data);			
		res = ioctl(dev, inout, &data);
		printf ("do here\n");
		if (res == -1){
			printf("output mode\n");
		}
		else if (inout == GETINTR_MODE){
			printf("input mode\n");
/*
			int inter[3];
			printf("res : %x\n", res);
			inter[0] = res & 0xFF0000;
			inter[0] /= 0x10000;
			inter[1] = res & 0xFF00;
			inter[1] /= 0x100;
			inter[2] = res & 0xFF;

			printf("volup : %d\nvoldown : %d \nback : %d\n", inter[0], inter[1], inter[2]);*/

			if (res == 0) printf("volup\n");
			if (res == 1) printf("voldown\n");
			if (res == 2) printf("back\n");
		}
		else{
			printf("swi : %d\n",res);
		}

		qq -= 1;
	}


	sleep(3);
	close(dev);

	return 0;
}
