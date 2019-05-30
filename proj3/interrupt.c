#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <mach/gpio.h>
#include <linux/platform_device.h>
#include <asm/gpio.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/cdev.h>

#define DEV_NAME "/dev/stopwatch"
#define MAJOR_NUM 242"
#define INTVAL 100

#define IOM_FND_ADDRESS 0x08000004 // pysical address for fnd

#define INIT do{\
    device_timer.flag = 0;\
    device_timer.tic = 0;\
}while(0)

#define FND fnd_data[0] << 12 | fnd_data[1] << 8 | fnd_data[2] << 4 | fnd_data[3]

#define TIMER_WRITE outw((unsigned short int)(FND),(unsigned int)iom_fpga_fnd_addr)

#define TIMER_UP do{\
    fnd_data[3] ++;\
    fnd_data[2] += fnd_data[3] /10;\
    fnd_data[3] %= 10;\
    fnd_data[1] += fnd_data[2] / 6;\
    fnd_data[2] %= 6;\
    fnd_data[0] += fnd_data[1] /10;\
    fnd_data[1] %= 10;\
    fnd_data[0] %= 6;\
}while(0)

static int inter_major=MAJOR_NUM, inter_minor=0;
static int result;
static dev_t inter_dev;
static struct cdev inter_cdev;
static unsigned char *iom_fpga_fnd_addr;
static int inter_open(struct inode *, struct file *);
static int inter_release(struct inode *, struct file *);
static int inter_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

irqreturn_t inter_handler1(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler2(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler3(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler4(int irq, void* dev_id, struct pt_regs* reg);

static inter_usage=0;
int interruptCount=0;


//------new defined var & fun
unsigned int tic=0;
unsigned char fnd_data[4];
typedef struct _device_timer{
    struct timer_list timer;
    unsigned int tic;
    unsigned short flag;
}_device_timer;
_device_timer device_timer,exit_timer;

wait_queue_head_t que;
DECLARE_WAIT_QUEUE_HEAD(que);
int play_flag = 0;
static void timer_set(_device_timer *t);
//-------------------
static struct file_operations inter_fops =
{
    .open = inter_open,
    .write = inter_write,
    .release = inter_release,
};

irqreturn_t inter_start(int irq, void* dev_id, struct pt_regs* reg) {
    printk("start\n");
    if(device_timer.flag == 1)
        return IRQ_HANDLED;
    device_timer.flag = 1;
    timer_set(&device_timer,1,&timer_func);
    return IRQ_HANDLED;
}

irqreturn_t inter_pause(int irq, void* dev_id, struct pt_regs* reg) {
    printk("pause\n");
    if(device_timer.flag == 1){
        del_timer_sync(&t.timer);
        device_timer.flag = 0;
    }
    else{
        device_timer.flag = 1;
        timer_set(&device_timer);
        timer_set(&device_timer,1,&timer_func);
    }
    
    return IRQ_HANDLED;
}

irqreturn_t inter_reset(int irq, void* dev_id,struct pt_regs* reg) {
    printk("reset\n");
    device_timer.tic = 0;
    fnd_data[0] = fnd_data[1] = fnd_data[2] = fnd_data[3] = 0;
    TIMER_WRITE;
    return IRQ_HANDLED;
}

irqreturn_t inter_exit_low(int irq, void* dev_id, struct pt_regs* reg) {
    printk("exit_low\n");
    if(exit_timer.flag == 0){
        exit_timer.flag = 1;
        timer_set(&exit_timer,3*INTVAL,&exit_func);
    }
    return IRQ_HANDLED;
}
irqreturn_t inter_exit_rise(int irq, void* dev_id, struct pt_regs* reg) {
    printk("exit_fail\n");
    exit_timer.flag = 0;
    del_timer_sync(&exit_timer.timer);
    return IRQ_HANDLED;
}

static void timer_func(unsigned long timeout){
    struct_device_timer *temp = (_device_timer *)timeout;
    if(++(temp->tic)>=100){
        temp -> tic = 0;
        TIMER_UP;
        TIMER_WRITE;
    }
    timer_set(temp,1,&timer_func);
}
static void exit_func(unsigned long timeout){
    fnd_data[0] = fnd_data[1] = fnd_data[2] = fnd_data[3] = 0;
    TIMER_WRITE;
    __wake_up(&que,1,1,NULL);
}
static void timer_set(_device_timer *t,int sec,void (*func)(unsigned long)){
    del_timer_sync(&t.timer);
    t ->timer.expires = jiffies + (sec*HZ/INTVAL);
    t ->timer.data = (unsigned long)t;
    t ->timer.function = func;
    add_timer(&t.timer);
}
static int inter_open(struct inode *minode, struct file *mfile){
    int ret;
    int irq;

    printk(KERN_ALERT "Open Module\n");
    // int1
    gpio_direction_input(IMX_GPIO_NR(1,11));
    irq = gpio_to_irq(IMX_GPIO_NR(1,11));
    ret = request_irq(irq,inter_start,IRQF_TRIGGER_RISING,"timer_start",NULL);

    // int2
    gpio_direction_input(IMX_GPIO_NR(1,12));
    irq = gpio_to_irq(IMX_GPIO_NR(1,12));
    ret = request_irq(irq,inter_pause,IRQF_TRIGGER_RISING,"timer_pause",NULL);
    //ret=request_irq();

    // int3
    gpio_direction_input(IMX_GPIO_NR(2,15));
    irq = gpio_to_irq(IMX_GPIO_NR(2,15));
    ret = request_irq(irq,inter_reset,IRQF_TRIGGER_RISING,"timer_volup",NULL);
    //ret=request_irq();

    // int4
    gpio_direction_input(IMX_GPIO_NR(5,14));
    irq = gpio_to_irq(IMX_GPIO_NR(5,14));
    ret = request_irq(irq,inter_exit_low,IRQF_TRIGGER_LOW,"timer_voldown_low",NULL);
    ret = request_irq(irq,inter_exit_rise,IRQF_TRIGGER_RISING,"timer_voldown_rise",NULL);

    return 0;
}

static int inter_release(struct inode *minode, struct file *mfile){
    free_irq(gpio_to_irq(IMX_GPIO_NR(1, 11)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(1, 12)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(2, 15)), NULL);
    free_irq(gpio_to_irq(IMX_GPIO_NR(5, 14)), NULL);

    printk(KERN_ALERT "Release Module\n");
    return 0;
}

static int inter_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos ){
    printk("write\n");
    INIT;
    interruptible_sleep_on(&que);
    return 0;
}

static int inter_register_cdev(void)
{
    int error;
    if(inter_major) {
        inter_dev = MKDEV(inter_major, inter_minor);
        error = register_chrdev_region(inter_dev,1,DEV_NAME);
    }else{
        error = alloc_chrdev_region(&inter_dev,inter_minor,1,"inter");
        inter_major = MAJOR(inter_dev);
    }
    if(error<0) {
        printk(KERN_WARNING "inter: can't get major %d\n", inter_major);
        return result;
    }
    printk(KERN_ALERT "major number = %d\n", inter_major);
    cdev_init(&inter_cdev, &inter_fops);
    inter_cdev.owner = THIS_MODULE;
    inter_cdev.ops = &inter_fops;
    error = cdev_add(&inter_cdev, inter_dev, 1);
    if(error)
    {
        printk(KERN_NOTICE "inter Register Error %d\n", error);
    }
    return 0;
}

static int __init inter_init(void) {
    int result;
    if((result = inter_register_cdev()) < 0 )
        return result;
	iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
    printk(KERN_ALERT "Init Module Success \n");
    printk(KERN_ALERT "Device : /dev/inter, Major Num : 246 \n");
    return 0;
}

static void __exit inter_exit(void) {
    cdev_del(&inter_cdev);
    unregister_chrdev_region(inter_dev, 1);
	iounmap(iom_fpga_fnd_addr);
    del_timer_sync(&device_timer.timer);
    del_timer_sync(&exit_timer.timer);
    printk(KERN_ALERT "Remove Module Success \n");
}

module_init(inter_init);
module_exit(inter_exit);
MODULE_LICENSE("GPL");
