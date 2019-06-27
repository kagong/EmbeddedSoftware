#include "EVSystem.h"


JNIEXPORT jint JNICALL Java_EVSystem_getSwitch (JNIEnv *env, jobject obj){
    int retval;
    /*
     * retval = ioctl()
     * 0 ~ 8
     */
    
    return retval;
}

JNIEXPORT jint JNICALL Java_EVSystem_getIntrBtn (JNIEnv *env, jobject obj){
    int retval;
    /*
     * retval = ioctl()
     * retval = 0 : vol+
     * retval = 1 : vol -
     * retval = 2 : back
     */
    return retval;
}

JNIEXPORT void JNICALL Java_EVSystem_setDot (JNIEnv *env, jobject obj, jint data){
    /*
     * ioctl() <- data
     * data == 0 : elev stop
     * data == 1 : elev up
     * data == 2 : elev down
     */
}

JNIEXPORT void JNICALL Java_EVSystem_setLed (JNIEnv *, jobject obj, jint data){
    /*
     * ioctl() <- data
     * data & 0x01 = 1st floor
     * data & 0x02 = 2nd floor
     * data & 0x03 = 3rd floor
     * data & 0x04 = 4th floor
     * .....
     * data & 0x07 = 7th floor
     */
}
