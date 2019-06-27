#include "EVSystem.h"
#include <syscall.h>

#define GETSWI_MODE 0
#define GETINTR_MODE 1
#define SETLED_MODE 3
#define SETDOT_MODE 4
#define SETBUZ_MODE 5

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>


JNIEXPORT jint JNICALL Java_EVSystem_getSwitch (JNIEnv *env, jobject obj, jint dev){
    int retval;
	int flag = 1,  i, qq = 0;
	unsigned char push_sw_buff[9];

    /*
     * retval = ioctl()
     * 0 ~ 8
     */

	int cdev;
	cdev = dev;

	while (qq < 10 && flag == 1){
		read(cdev, &push_sw_buff, 9);
		for (i = 0 ; i < 9 ; i++){
			if (push_sw_buff[i] == 1) {
				flag = 0;
				retval = i;
			}
		}
		qq++;
	}
	if (flag == 1) retval = -1;    

    return retval;
}

JNIEXPORT jint JNICALL Java_EVSystem_getIntrBtn (JNIEnv *env, jobject obj, jint dev){
    int retval, cdev;
	cdev = (int)dev;
    /*
     * retval = ioctl()
     * retval = 0 : vol+
     * retval = 1 : vol -
     * retval = 2 : back
     */

	retval = ioctl(cdev, GETINTR_MODE, 0);
    return retval;
}

JNIEXPORT void JNICALL Java_EVSystem_setDot (JNIEnv *env, jobject obj, jint dev, jint data){
	int cdata, cdev;
	cdata = (int)data;
	cdev = (int)dev;
    /*
     * ioctl() <- data
     * data == 0 : elev stop
     * data == 1 : elev up
     * data == 2 : elev down
     */
	
	if (cdata == 2)
		cdata = 3;
	cdata = cdata << 24;

	ioctl(cdev, SETDOT_MODE, cdata);
}

JNIEXPORT void JNICALL Java_EVSystem_setLed (JNIEnv *env, jobject obj, jint dev, jint data){
    /*
     * ioctl() <- data
     * data & 0x01 = 1st floor
     * data & 0x02 = 2nd floor
     * data & 0x03 = 3rd floor
     * data & 0x04 = 4th floor
     * .....
     * data & 0x07 = 7th floor
     */
	int cdata, cdev;
	cdata = (int)data;
	cdev = (int)dev;

	ioctl(cdev, SETLED_MODE, data);
}
JNIEXPORT void JNICALL Java_EVSystem_setBuzzer (JNIEnv *env, jobject obj, jint dev){
	int cdev;
	cdev = (int)dev;

	ioctl(cdev, SETBUZ_MODE, 0);
}
JNIEXPORT jint JNICALL Java_EVSystem_openDevice_11 (JNIEnv *env, jobject obj){
	int dev;
	dev = open("/dev/elevator", O_RDWR);

	return (jint)dev;
}
JNIEXPORT jint JNICALL Java_EVSystem_openDevice_12 (JNIEnv *env, jobject obj){
	int dev;
	dev = open("/dev/fpga_push_switch", O_RDWR);

	return (jint)dev;
}
JNIEXPORT void JNICALL Java_EVSystem_closeDevice (JNIEnv *env, jobject obj, jint dev1, jint dev2){
	int dev;
	dev = (int)dev1;
	close(dev);

	dev = (int)dev2;
	close(dev);
}

/*
 * data1 : now floor number (start 1)
 * data2 : elevator state (0 : NONE, 1:UP 3:DOWN)
 * data3[7] : each floor's button state (0 : NONE, 1: UP, 2: DOWN ,3: UPDOWN)
 * data4[7] : each elev buttons state (on : 1 , off :0)
 * return value = state (1: UP 2: DOWN)
 */
JNIEXPORT jint JNICALL Java_EVSystem_callSyscall (JNIEnv *env, jobject obj, jint data1, jint data2, jintArray data3, jintArray data4){
    int retval;
    jint *arr1 = (*env) -> GetIntArrayElements(env,data3,0);
    jint *arr2 = (*env) -> GetIntArrayElements(env,data4,0);
    

    
    retval =  syscall(376, data1,data2,arr1,arr2);
    
    (*env) -> ReleaseIntArrayElements(env,data3,arr1,0);
    (*env) -> ReleaseIntArrayElements(env,data4,arr2,0);
    return retval;
}


