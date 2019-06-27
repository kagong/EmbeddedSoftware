#include "EVSystem.h"

//retval == -1 none
JNIEXPORT jint JNICALL Java_EVSystem_getSwitch (JNIEnv *env, jobject obj){
    int retval;
    /*
     * retval = ioctl()
     * 0 ~ 8
     */
    
    return retval;
}

//retval == -1 none
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

JNIEXPORT void JNICALL Java_EVSystem_setLed (JNIEnv *env, jobject obj, jint data){
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
JNIEXPORT void JNICALL Java_EVSystem_setBuzzer (JNIEnv *env, jobject obj){
}
JNIEXPORT void JNICALL Java_EVSystem_openDevice (JNIEnv *env, jobject obj){
}
JNIEXPORT void JNICALL Java_EVSystem_closeDevice (JNIEnv *env, jobject obj){
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
    jint *arr1 = (*env) - > GetIntArrayElements(env,data3,0);
    jint *arr2 = (*env) - > GetIntArrayElements(env,data4,0);
    

    /*
     * retval =  syscall(data1,data2,arr1,arr2)
     */
    (*env) -> ReleaseIntArrayElements(env,data1,arr1,0);
    (*env) -> ReleaseIntArrayElements(env,data2,arr2,0);
    return retval;
}


