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

#define STUDENT_NAME "YUNJEHYEONG\0"
#define STUDENT_ID "20151575\0"

#define IOM_MAJOR 242		// ioboard fpga device major number
#define IOM_NAME "dev_driver""		// ioboard fpga device name

#define IOM_FND_ADDRESS 0x08000004 // pysical address
#define IOM_FPGA_DOT_ADDRESS 0x08000210 // pysical address
#define IOM_LED_ADDRESS 0x08000016 // pysical address
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x08000090 // pysical address - 32 Byte (16 * 2
#define FND(data) data[0] << 12 | data[1] << 8 | data[2] << 4 | data[0];
//Global variable
static int fpga_port_usage = 0;

static unsigned char *iom_fpga_fnd_addr;
static unsigned char *iom_fpga_text_lcd_addr;
static unsigned char *iom_fpga_dot_addr;
static unsigned char *iom_fpga_led_addr;

static struct _device_timer{
    struct itmer_list timer;
    unsigned int count;
    unsigned char idx;
    unsigned char first_dir,second_dir;
    unsigned char first_text_idx,second_idx;
};
struct _device_timer device_timer;
static unsigned char fnd_data[4];
// define functions...
ssize_t iom_fpga_fnd_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what);
ssize_t iom_fpga_fnd_read(struct file *inode, char *gdata, size_t length, loff_t *off_what);
int iom_fpga_open(struct inode *minode, struct file *mfile);
int iom_fpga_release(struct inode *minode, struct file *mfile);

// define file_operations structure 
struct file_operations iom_fpga_fops =
{
	.owner		=	THIS_MODULE,
	.open		=	iom_fpga_open,
	.write		=	iom_fpga_fnd_write,	
	.read		=	iom_fpga_fnd_read,
    .ioctl      =   device_ioctl,
	.release	=	iom_fpga_release,
};

// when fnd device open ,call this function
int iom_fpga_open(struct inode *minode, struct file *mfile) 
{	
	if(fpga_port_usage != 0) return -EBUSY;

	fpga_port_usage = 1;

	return 0;
}

// when fnd device close ,call this function
int iom_fpga_release(struct inode *minode, struct file *mfile) 
{
	fpga_port_usage = 0;

	return 0;
}

// when write to fnd device  ,call this function
ssize_t iom_fpga_fnd_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what) 
{
	int i;
	unsigned char value[4];
	unsigned short int value_short = 0;
	const char *tmp = gdata;

	if (copy_from_user(&value, tmp, 4))
		return -EFAULT;

    value_short = value[0] << 12 | value[1] << 8 |value[2] << 4 |value[3];
    outw(value_short,(unsigned int)iom_fpga_fnd_addr);	    

	return length;
}

// when read to fnd device  ,call this function
ssize_t iom_fpga_fnd_read(struct file *inode, char *gdata, size_t length, loff_t *off_what) 
{
	int i;
	unsigned char value[4];
	unsigned short int value_short = 0;
	char *tmp = gdata;

    value_short = inw((unsigned int)iom_fpga_fnd_addr);	    
    value[0] =(value_short >> 12) & 0xF;
    value[1] =(value_short >> 8) & 0xF;
    value[2] =(value_short >> 4) & 0xF;
    value[3] = value_short & 0xF;

    if (copy_to_user(tmp, value, 4))
        return -EFAULT;

	return length;
}
static void timer_foo(unsigned long timeout){

}
int device_ioctl(struct file *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param){
    unsigned int interval,start_val,start_idx, count;
    char *name = STUDENT_NAME, *id= STUDENT_ID;
    //fnd
    fnd_data[start_idx] = start_val;
    outw((unsigned short int)(FND(fnd_data)),(unsigned int)iom_fpga_fnd_addr);
    //dot
    for(int i = 0 ; i < 10; i++)
        outw((unsigned short int)dot_data[start_idx][i],(unsigned int)iom_fpga_dot_addr + i*2);

    //led
    outw((unsigned short)(1 << (start_val-1)),(unsigned int)iom_fpga_led_addr);

    //lcd text
    for(int i = 0 ; i < strlen(name); i++){
        unsigned short int temp = ((name[i] & 0xFF) << 8 | name[i+1] & 0xFF);
        outw(temp,(unsigned int)iom_fpga_text_lcd_addr+i);
    }
    for(int i = 0 ; i < strlen(id); i++){
        unsigned short int temp = ((id[i] & 0xFF) << 8 | id[i+1] & 0xFF);
        outw(temp,(unsigned int)iom_fpga_text_lcd_addr+16+i);
    }
    device_timer.idx = start_idx;
    device_timer.count = count;
    device_timer.first_text_idx = device_timer.second_text_idx = 0;

    del_timer_sync(&device_timer.timer);

    device_timer.timer.expires = jiffies + ((interval/10)*HZ);
    device_timer.timer.data = (unsigned long)&device_timer;
    device_timer.timer.function = timer_foo;

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

	iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
	iom_fpga_text_lcd_addr = ioremap(IOM_FPGA_TEXT_LCD_ADDRESS, 0x32);
	iom_fpga_dot_addr = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);
	iom_fpga_led_addr = ioremap(IOM_LED_ADDRESS, 0x1);

	printk("init module, %s major number : %d\n", IOM_NAME, IOM_MAJOR);

	return 0;
}

void __exit iom_fpga_exit(void) 
{
	iounmap(iom_fpga_fnd_addr);
	iounmap(iom_fpga_text_lcd_addr);
	iounmap(iom_fpga_dot_addr);
	iounmap(iom_fpga_led_addr);
	unregister_chrdev(IOM_MAJOR, IOM_NAME);
}

module_init(iom_fpga_init);
module_exit(iom_fpga_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Huins");
