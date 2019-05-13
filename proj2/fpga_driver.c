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
#define IOM_NAME "dev_driver"		// ioboard fpga device name

#define IOM_FND_ADDRESS 0x08000004 // pysical address
#define IOM_FPGA_DOT_ADDRESS 0x08000210 // pysical address
#define IOM_LED_ADDRESS 0x08000016 // pysical address
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x08000090 // pysical address - 32 Byte (16 * 2
#define FND(data) data[0] << 12 | data[1] << 8 | data[2] << 4 | data[0];
//Global variable
unsigned char dot_data[10][10] = {
	{0x3e,0x7f,0x63,0x73,0x73,0x6f,0x67,0x63,0x7f,0x3e}, // 0
	{0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e}, // 1
	{0x7e,0x7f,0x03,0x03,0x3f,0x7e,0x60,0x60,0x7f,0x7f}, // 2
	{0xfe,0x7f,0x03,0x03,0x7f,0x7f,0x03,0x03,0x7f,0x7e}, // 3
	{0x66,0x66,0x66,0x66,0x66,0x66,0x7f,0x7f,0x06,0x06}, // 4
	{0x7f,0x7f,0x60,0x60,0x7e,0x7f,0x03,0x03,0x7f,0x7e}, // 5
	{0x60,0x60,0x60,0x60,0x7e,0x7f,0x63,0x63,0x7f,0x3e}, // 6
	{0x7f,0x7f,0x63,0x63,0x03,0x03,0x03,0x03,0x03,0x03}, // 7
	{0x3e,0x7f,0x63,0x63,0x7f,0x7f,0x63,0x63,0x7f,0x3e}, // 8
	{0x3e,0x7f,0x63,0x63,0x7f,0x3f,0x03,0x03,0x03,0x03} // 9
};

unsigned char fpga_set_full[10] = {
	// memset(array,0x7e,sizeof(array));
	0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f
};

unsigned char fpga_set_blank[10] = {
	// memset(array,0x00,sizeof(array));
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


static int fpga_port_usage = 0;

static unsigned char *iom_fpga_fnd_addr;
static unsigned char *iom_fpga_text_lcd_addr;
static unsigned char *iom_fpga_dot_addr;
static unsigned char *iom_fpga_led_addr;

static struct _device_timer{
    struct itmer_list timer;
    unsigned int count,interval,start_val,idx;
    unsigned char first_dir,second_dir;
    unsigned char first_text_idx,second_idx;
    short first_text_dir,second_dir;
};
struct _device_timer device_timer;
static unsigned char fnd_data[4];
// define functions...
int iom_fpga_open(struct inode *minode, struct file *mfile);
int iom_fpga_release(struct inode *minode, struct file *mfile);

// define file_operations structure 
struct file_operations iom_fpga_fops =
{
	.owner		=	THIS_MODULE,
	.open		=	iom_fpga_open,
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
static void timer_func(unsigned long timeout){
    unsigned int val;
    char *name = STUDENT_NAME, *id= STUDENT_ID;
    struct _device_timer *temp = (struct _device_timer *)timeout;
    if(temp.count == 0)){
        //init
        return ;
    }
   
    fnd_data[temp.idx]++; 
    fnd_data[temp.idx] %= 9;
    if(fnd_data[temp.idx] == temp.start_val){
        fnd_data[temp.idx++] = 0;
        temp.idx %= 4;
        fnd_data[temp.idx] = temp.start_val;
    }
    val = fnd_data[temp.idx];

    outw((unsigned short int)(FND(fnd_data)),(unsigned int)iom_fpga_fnd_addr);
    for(i = 0 ; i < 10; i++)
        outw((unsigned short int)dot_data[val][i],(unsigned int)iom_fpga_dot_addr + i*2);
    outw((unsigned short)(1 << (val-1)),(unsigned int)iom_fpga_led_addr);
    for(i = 0 ; i < 10; i++)
        outw((unsigned short int)dot_data[fnd_data[start_val]][i],(unsigned int)iom_fpga_dot_addr + i*2);

    //led
    outw((unsigned short)(1 << (start_val-1)),(unsigned int)iom_fpga_led_addr);

    //lcd text 
    short new_idx = device_timer.first_text_idx + device_timer.first_text_dir;
    if (0 >  new_idx || new_idx + strlen(name) >=16){
        device_timer.first_text_dir *= -1;
        new_idx = device_timer.first_text_idx + device_timer.first_text_dir;
        if (0 <=  new_idx && new_idx + strlen(name) <16){  
            for(i = 0 ; i < strlen(name); i++){
                unsigned short int temp = ((name[i] & 0xFF) << 8 | name[i+1] & 0xFF);
                outw(temp,(unsigned int)iom_fpga_text_lcd_addr+i+new_idx);
            }
            device_timer.first_text_idx = new_idx;
        }

    }    
    new_idx = device_timer.second_text_idx + device_timer.second_text_dir;
    if (0 >  new_idx || new_idx + strlen(id) >=16){
        device_timer.second_text_dir *= -1;
        new_idx = device_timer.second_text_idx + device_timer.second_text_dir;
        if (0 <=  new_idx && new_idx + strlen(id) <16){  
            for(i = 0 ; i < strlen(id); i++){
                unsigned short int temp = ((name[i] & 0xFF) << 8 | name[i+1] & 0xFF);
                outw(temp,(unsigned int)iom_fpga_text_lcd_addr+i+new_idx);
            }
            device_timer.second_text_idx = new_idx;
        }

    }

    device_timer.first_text_idx = device_timer.second_text_idx = 0;
    device_timer.count = count-1;
    device_timer.timer.expires = jiffies + ((interval/10)*HZ);
    device_timer.timer.data = (unsigned long)&device_timer;
    device_timer.timer.function = timer_func;

    add_timer(&device_timer.timer);
}
int device_ioctl(struct file *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param){
    unsigned int interval,start_val,start_idx, count;
    char *name = STUDENT_NAME, *id= STUDENT_ID;
    unsigned int data;

    get_user(&data,(unsigned int *)ioctl_param);
    start_idx = (data >> (8*3)) & 0xFF;
    start_val = (data >> (8*2)) & 0xFF;
    count = (data >> (8*1)) & 0xFF;
    interval = (data >> (8*0)) & 0xFF;

    fnd_data[start_idx] = start_val;
    outw((unsigned short int)(FND(fnd_data)),(unsigned int)iom_fpga_fnd_addr);
    //dot
    for(i = 0 ; i < 10; i++)
        outw((unsigned short int)dot_data[fnd_data[start_val]][i],(unsigned int)iom_fpga_dot_addr + i*2);

    //led
    outw((unsigned short)(1 << (start_val-1)),(unsigned int)iom_fpga_led_addr);

    //lcd text
    for(i = 0 ; i < strlen(name); i++){
        unsigned short int temp = ((name[i] & 0xFF) << 8 | name[i+1] & 0xFF);
        outw(temp,(unsigned int)iom_fpga_text_lcd_addr+i);
    }
    for(i = 0 ; i < strlen(id); i++){
        unsigned short int temp = ((id[i] & 0xFF) << 8 | id[i+1] & 0xFF);
        outw(temp,(unsigned int)iom_fpga_text_lcd_addr+16+i);
    }
    device_timer.idx = start_idx;
    device_timer.count = count;
    device_timer.start_val = start_val;
    device_timer.interval = interval;.
    device_timer.first_text_idx = device_timer.second_text_idx = 0;
    device_timer.first_text_dir = device_timer.second_text_dir = 1;

    del_timer_sync(&device_timer.timer);

    device_timer.timer.expires = jiffies + ((interval/10)*HZ);
    device_timer.timer.data = (unsigned long)&device_timer;
    device_timer.timer.function = timer_func;

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
