
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>

#define MAX_BUTTON 9


int main(void)
{
		int flag = 1;
		int dev;
		int buff_size;
		int i;
		unsigned char push_sw_buff[MAX_BUTTON];

		dev = open("/dev/fpga_push_switch", O_RDWR);

		if (dev<0){
			printf("Device Open Error\n");
			close(dev);
			return -1;
		}


		buff_size=sizeof(push_sw_buff);
		printf("Press <ctrl+c> to quit. \n");
		while(flag){
			usleep(400000);
			read(dev, &push_sw_buff, buff_size);
			for(i=0;i<MAX_BUTTON;i++) {
				printf("[%d] ",push_sw_buff[i]);
				if (push_sw_buff[i] == 1) flag = 0;
			}
			printf("\n");
		}
	close(dev);
}
