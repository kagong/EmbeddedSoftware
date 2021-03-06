/* FPGA FND Ioremap Control
FILE : fpga_fpga_driver.c 
AUTH : largest@huins.com */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/version.h>
#include "global.h"

#define STUDENT_NAME "YUNJEHYEONG\0"
#define STUDENT_ID "20151575\0"

#define IOM_MAJOR 242		// ioboard fpga device major number
#define IOM_NAME "dev_driver"		// ioboard fpga device name

#define IOM_FND_ADDRESS 0x08000004 // pysical address for fnd
#define IOM_FPGA_DOT_ADDRESS 0x08000210 // pysical address for dot matrix
#define IOM_LED_ADDRESS 0x08000016 // pysical address for led
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x08000090 // pysical address for lcd

static int fpga_port_usage = 0;

static unsigned char *iom_fpga_fnd_addr;
static unsigned char *iom_fpga_text_lcd_addr;
static unsigned char *iom_fpga_dot_addr;
static unsigned char *iom_fpga_led_addr;

typedef struct _device_timer{
    struct timer_list timer;
    unsigned int count,interval,start_val,idx;
    short first_text_idx,second_text_idx;
    short first_text_dir,second_text_dir;
}_device_timer;
_device_timer device_timer;

static unsigned char fnd_data[4];

// define functions...
int iom_fpga_open(struct inode *minode, struct file *mfile);
int iom_fpga_release(struct inode *minode, struct file *mfile);
long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);

// define file_operations structure 
struct file_operations iom_fpga_fops =
{
    .owner=	THIS_MODULE,
    .unlocked_ioctl = device_ioctl,
	.open=	iom_fpga_open,
	.release=	iom_fpga_release
};

int iom_fpga_open(struct inode *minode, struct file *mfile) 
{
	if(fpga_port_usage != 0) return -EBUSY;

	fpga_port_usage = 1;

	return 0;
}

int iom_fpga_release(struct inode *minode, struct file *mfile) 
{
	fpga_port_usage = 0;

	return 0;
}

// called by timer 
// change the data and print to devices
static void timer_func(unsigned long timeout){
    unsigned int val;
    char *name = STUDENT_NAME, *id= STUDENT_ID;
    char text[2][16];
    int i;
    struct _device_timer *temp = (_device_timer *)timeout;
    short new_idx;
    if(temp->count == 0){
        INIT(fnd_data);
        return ;
    }
    //find index,value in fnd and increasing them alright
    fnd_data[temp->idx]++; 
    fnd_data[temp->idx] = (fnd_data[temp->idx]-1)%8 + 1;
    if(fnd_data[temp->idx] == temp->start_val){
        fnd_data[temp->idx++] = 0;
        temp->idx %= 4;
        fnd_data[temp->idx] = temp->start_val;
    }
    val = fnd_data[temp->idx];

    write_to_device(val,temp->first_text_idx,temp->first_text_dir,temp->second_text_idx,temp->second_text_dir);

    temp->count -= 1;
    // after interval/10 sec, call timer_func()
    temp->timer.expires = get_jiffies_64() + (temp->interval*HZ/10);
    temp->timer.data = (unsigned long)temp;
    temp->timer.function = timer_func;

    add_timer(&temp->timer);
}

//ioctl function
long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param){
    unsigned int interval,start_val,start_idx, count;
    char *name = STUDENT_NAME, *id= STUDENT_ID;
    unsigned int data;
    char text[2][16];
    int i;
    short new_idx;

    //init device
    INIT(fnd_data);

    //take the data form user level memory and check the invalid
    get_user(data,(unsigned int *)ioctl_param);
    start_idx = (data >> (8*3)) & 0xFF;
    start_val = (data >> (8*2)) & 0xFF;
    count = (data >> (8*1)) & 0xFF;
    interval = (data >> (8*0)) & 0xFF;
    fnd_data[start_idx] = start_val;

    //data structure init
    device_timer.idx = start_idx;
    device_timer.count = count-1;
    device_timer.start_val = start_val;
    device_timer.interval = interval;
    device_timer.first_text_idx = device_timer.second_text_idx = -1;
    device_timer.first_text_dir = device_timer.second_text_dir = 1;

    write_to_device(start_val,device_timer.first_text_idx,device_timer.first_text_dir,device_timer.second_text_idx,device_timer.second_text_dir);
    
    del_timer_sync(&device_timer.timer);
    // after interval/10 sec, call timer_func()
    device_timer.timer.expires = jiffies + (interval*HZ/10);
    device_timer.timer.data = (unsigned long)&device_timer;
    device_timer.timer.function = timer_func;

    //call timer
    add_timer(&device_timer.timer);
    return 1;

}


int __init iom_fpga_init(void)
{
	int result;

	result = register_chrdev(IOM_MAJOR, IOM_NAME, &iom_fpga_fops);
	if(result < 0) {
		printk(KERN_WARNING"Can't get any major\n");
		return result;
	}
    //allocate resource

    //mmep init
	iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
	iom_fpga_text_lcd_addr = ioremap(IOM_FPGA_TEXT_LCD_ADDRESS, 0x32);
	iom_fpga_dot_addr = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);
	iom_fpga_led_addr = ioremap(IOM_LED_ADDRESS, 0x1);
    init_timer(&(device_timer.timer));
	printk("init module, %s major number : %d\n", IOM_NAME, IOM_MAJOR);

	return 0;
}

void __exit iom_fpga_exit(void) 
{
    //free resource
	iounmap(iom_fpga_fnd_addr);
	iounmap(iom_fpga_text_lcd_addr);
	iounmap(iom_fpga_dot_addr);
	iounmap(iom_fpga_led_addr);
    del_timer_sync(&device_timer.timer);
	unregister_chrdev(IOM_MAJOR, IOM_NAME);
}

module_init(iom_fpga_init);
module_exit(iom_fpga_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huins");
