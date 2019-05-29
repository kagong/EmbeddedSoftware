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
#define INTVAL 10

#define IOM_FND_ADDRESS 0x08000004 // pysical address for fnd
#define FND(data) (data)[0] << 12 | (data)[1] << 8 | (data)[2] << 4 | (data)[3]
static int inter_major=0, inter_minor=0;
static int result;
static dev_t inter_dev;
static struct cdev inter_cdev;
static unsigned char *iom_fpga_fnd_addr;
static int inter_open(struct inode *, struct file *);
static int inter_release(struct inode *, struct file *);
static int inter_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

typedef struct _device_timer{
    struct timer_list timer;
    unsigned char fnd_data[4];
}_device_timer;
_device_timer device_timer;

wait_queue_head_t que;
DECLARE_WAIT_QIEIE_HEAD(que);

irqreturn_t inter_handler1(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler2(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler3(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler4(int irq, void* dev_id, struct pt_regs* reg);
static void timer_set(_device_timer *t);

static inter_usage=0;
int interruptCount=0;

static struct file_operations inter_fops =
{
    .open = inter_open,
    .write = inter_write,
    .release = inter_release,
};

irqreturn_t inter_start(int irq, void* dev_id, struct pt_regs* reg) {
    timer_set(&device_timer);
    timer_up_write();
    printk("start\n");
    return IRQ_HANDLED;
}

irqreturn_t inter_pause(int irq, void* dev_id, struct pt_regs* reg) {
    printk("pause\n");
    return IRQ_HANDLED;
}

irqreturn_t inter_reset(int irq, void* dev_id,struct pt_regs* reg) {
    printk("reset\n");
    return IRQ_HANDLED;
}

irqreturn_t inter_exit(int irq, void* dev_id, struct pt_regs* reg) {
    printk("exit\n");
    return IRQ_HANDLED;
}
static void timer_tp_write(_device_timer * t){
    t->fnd_data[3] ++;
    t->fnd_data[2] += fnd_data[3] /10;
    fnd_data[3] %= 10;
    t->fnd_data[1] += fnd_data[2] / 6;
    fnd_data[2] %= 6;
    t->fnd_data[0] += fnd_data[1] /10;
    fnd_data[1] %= 10;
    fnd_data[0] %= 6;
    outw((unsigned short int)(FND(fnd_data)),(unsigned int)iom_fpga_fnd_addr);\
}
static void timer_func(unsigned long timeout){
    struct_device_timer *temp = (_device_timer *)timeout;
    timer_set(temp);
    timer_up_write();
}
static void timer_set(_device_timer *t){
    del_timer_sync(&t.timer);
    t ->timer.expires = jiffies + (INTVAL*HZ/10);
    t ->timer.data = (unsigned long)&device_timer;
    t ->timer.function = timer_func;
    add_timer(&t.timer);
}
static int inter_open(struct inode *minode, struct file *mfile){
    int ret;
    int irq;

    printk(KERN_ALERT "Open Module\n");
    // int1
    gpio_direction_input(IMX_GPIO_NR(1,11));
    irq = gpio_to_irq(IMX_GPIO_NR(1,11));
    ret = request_irq(irq,inter_handler,IRQF_TRIGGER_RISING,"name",NULL);

    // int2
    gpio_direction_input(IMX_GPIO_NR(1,12));
    irq = gpio_to_irq(IMX_GPIO_NR(1,12));
    //ret=request_irq();

    // int3
    gpio_direction_input(IMX_GPIO_NR(2,15));
    irq = gpio_to_irq(IMX_GPIO_NR(2,15));
    //ret=request_irq();

    // int4
    gpio_direction_input(IMX_GPIO_NR(5,14));
    irq = gpio_to_irq(IMX_GPIO_NR(5,14));
    //ret=request_irq();

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
    return 0;
}

static int inter_register_cdev(void)
{
    int error;
    if(inter_major) {
        inter_dev = MKDEV(inter_major, inter_minor);
        error = register_chrdev_region(inter_dev,1,"inter");
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
    printk(KERN_ALERT "Remove Module Success \n");
}

module_init(inter_init);
module_exit(inter_exit);
MODULE_LICENSE("GPL");
