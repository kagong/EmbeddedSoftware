#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/timer.h>

#include <asm/irq.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/cdev.h>

#include <mach/gpio.h>

#include "fpga_dot_font.h"

#define TP_DEV_MAJOR 242
#define TP_DEV_NAME "elevator"

#define TRUE 1
#define FALSE 0

#define GETSWITCH_MODE 0
#define GETINTR_MODE 1
#define SETLED_MODE 3
#define SETDOT_MODE 4
#define SETBUZ_MODE 5

#define ELEV_UP 1
#define ELEV_DOWN 3


// **** For dev driver **** //
static int inter_major=TP_DEV_MAJOR, inter_minor=0;
static int result;
static dev_t inter_dev;
static struct cdev inter_cdev;
static int inter_open(struct inode *, struct file *);
static int inter_release(struct inode *, struct file *);
static long inter_ioctl(struct file *filp, unsigned int ioctl_num, unsigned long ioctl_param);
static int inter_usage = 0;

static struct file_operations inter_fops =
{
	.open = 			inter_open,
	.unlocked_ioctl =	inter_ioctl,
	.release = 			inter_release,
};





// **** For output **** //
void init_output(void);
//void print_start(const char *buf);
void print_led(char data);
void calculate_dot(char data);
void print_dot(int data);

struct timer_data{
	int status;
	int pos;
	int set;
	struct timer_list timer;
};

static struct timer_data my_timer;
void callback_handler(unsigned long arg);


#define DOT_PRINT_INTERVAL HZ * 1


#define IOM_LED_ADDRESS 0x08000016
#define IOM_DOT_ADDRESS 0x08000210
#define IOM_BUZ_ADDRESS 0x08000070

static unsigned char *iom_led_addr;
static unsigned char *iom_dot_addr;
static unsigned char *iom_buz_addr;



// **** For input **** //
irqreturn_t inter_handler_back(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_volup(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_voldown(int irq, void* dev_id, struct pt_regs* reg);

static int volup_pressed;
static int voldown_pressed;
static int back_pressed;

// **** Input **** //
irqreturn_t inter_handler_back(int irq, void* dev_id, struct pt_regs* reg) {
    printk(KERN_ALERT "interrupt2!!! = %x\n", gpio_get_value(IMX_GPIO_NR(1, 12)));

	back_pressed += 1;
    return IRQ_HANDLED;
}

irqreturn_t inter_handler_volup(int irq, void* dev_id,struct pt_regs* reg) {
    printk(KERN_ALERT "interrupt3!!! = %x\n", gpio_get_value(IMX_GPIO_NR(2, 15)));

	volup_pressed += 1;
    return IRQ_HANDLED;
}

irqreturn_t inter_handler_voldown(int irq, void* dev_id, struct pt_regs* reg) {
	printk(KERN_ALERT "interrupt4!!! = %x\n", gpio_get_value(IMX_GPIO_NR(5, 14)));

	voldown_pressed += 1;
 	return IRQ_HANDLED;
}
// **** Control **** //
static long inter_ioctl(struct file *filep, unsigned int ioctl_num, unsigned long ioctl_param){
	long res = -1;	
	char decode_data;
	int i;

	unsigned short int s_val;
	unsigned char val;


	switch(ioctl_num){
		case GETSWITCH_MODE:
			break;
		case GETINTR_MODE:
			res = -1;
			if (volup_pressed != 0){
				res = 0;
				volup_pressed = 0;
			}

			if (voldown_pressed != 0){
//				res = res << 8;
//				res = res | voldown_pressed;
				res = 1;
				voldown_pressed = 0;
			}
			if (back_pressed != 0){
//				res = res << 8;
//				res = res | back_pressed;
				res = 2;
				back_pressed = 0;
			}		
			break;
		case SETLED_MODE:
			decode_data = ((char*)ioctl_param)[0];
			print_led(decode_data);

			break;
		case SETDOT_MODE:
			decode_data = ((char*)ioctl_param)[3];
			calculate_dot(decode_data);
			break;
		
		case SETBUZ_MODE:
			val = 1;
			s_val = val & 0xF;
			outw(s_val, (unsigned int)iom_buz_addr);


			for (i = 0 ; i < 100 ; i++)
				mdelay(5);

			val = 2;
			s_val = val & 0xF;
			outw(s_val, (unsigned int)iom_buz_addr);
			break;
	}

	return res;
}



void init_output(void){
	int i;
	unsigned short int s_value;

	/* led device init */
	s_value = 0;
	outw(s_value, (unsigned int)iom_led_addr);

	/* dot device init */
	for(i = 0 ; i < 10 ; i++){
		s_value = 0x00;
		outw(s_value, (unsigned int)iom_dot_addr + i*2);
	}
}

void print_led(char data){
	unsigned short s_value;

	s_value = (unsigned short)data & 0xFF;
	outw(s_value, (unsigned int) iom_led_addr);
}


void calculate_dot(char data){

	static int start_elevator = FALSE;

	switch(data){
		case 0:									// stop elevator

            printk(KERN_ALERT "pp %d %d\n", start_elevator, my_timer.status);
			start_elevator = FALSE;
			del_timer_sync(&my_timer.timer);
            
			init_output();

			my_timer.set = FALSE;

			break;

		case ELEV_UP:									// going up
			if ((start_elevator == TRUE && my_timer.status == ELEV_DOWN) || start_elevator == FALSE){	// elevator down to up
	
                printk(KERN_ALERT "ww %d %d\n", start_elevator, my_timer.status);
                start_elevator = TRUE;
				if (my_timer.set == TRUE)
					del_timer_sync(&my_timer.timer);

				init_timer(&my_timer.timer);
	
	
				my_timer.status = ELEV_UP;
				my_timer.pos = 0;
					
				print_dot(my_timer.status + my_timer.pos);

				my_timer.timer.expires = get_jiffies_64() + DOT_PRINT_INTERVAL;
				my_timer.timer.data = (unsigned long)&my_timer;
				my_timer.timer.function = callback_handler;

				my_timer.set = TRUE;
				add_timer(&my_timer.timer);
			}

			break;

		case ELEV_DOWN:									// going down
			if ((start_elevator == TRUE && my_timer.status == ELEV_UP) || start_elevator == FALSE){		// elevator up to down
                printk(KERN_ALERT "ww %d %d\n", start_elevator, my_timer.status);

                start_elevator = TRUE;
				if (my_timer.set == TRUE)
					del_timer_sync(&my_timer.timer);

				init_timer(&my_timer.timer);
	
				my_timer.status = ELEV_DOWN;
				my_timer.pos = 0;

				print_dot(my_timer.status + my_timer.pos);

				my_timer.timer.expires = get_jiffies_64() + DOT_PRINT_INTERVAL;
				my_timer.timer.data = (unsigned long)&my_timer;
				my_timer.timer.function = callback_handler;

				my_timer.set = TRUE;
				add_timer(&my_timer.timer);
			}

			break;
	}
	return;
}

void print_dot(int data){
	int i;
	unsigned short int s_value;

	for (i = 0 ; i < 10 ; i++){
		s_value = dot_output[data][i] & 0x7F;
		outw(s_value, (unsigned int) iom_dot_addr + i*2);
	}
}


void callback_handler(unsigned long arg){
	struct timer_data *tmp_timer;
	tmp_timer = (struct timer_data *)arg;
    printk(KERN_ALERT "%d\n",tmp_timer->pos);
	if (tmp_timer->pos == 1)		tmp_timer->pos = 0;
	else if (tmp_timer->pos == 0) 	tmp_timer->pos = 1;




	print_dot(tmp_timer->status + tmp_timer->pos);

	tmp_timer->timer.expires = get_jiffies_64() + DOT_PRINT_INTERVAL;
	tmp_timer->timer.data = (unsigned long) tmp_timer;
	tmp_timer->timer.function = callback_handler;

	add_timer(&tmp_timer->timer);
}










// **** When dev device is open **** //
static int inter_open(struct inode *minode, struct file *mfile){
	int ret;
	int irq;

	if (my_timer.set == TRUE){
		del_timer_sync(&my_timer.timer);
		my_timer.set = FALSE;
	}

	/* input init */
	my_timer.set = FALSE;
	volup_pressed = 0;
	voldown_pressed = 0;
	back_pressed = 0;

	printk(KERN_ALERT "Open Module\n");
	
	// interrupt back button
	gpio_direction_input(IMX_GPIO_NR(1,12));
	irq = gpio_to_irq(IMX_GPIO_NR(1,12));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret = request_irq(irq, (void*)inter_handler_back, IRQF_TRIGGER_FALLING, "back", NULL);

	// interrupt volume up button
	gpio_direction_input(IMX_GPIO_NR(2,15));
	irq = gpio_to_irq(IMX_GPIO_NR(2,15));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret = request_irq(irq, (void*)inter_handler_volup, IRQF_TRIGGER_FALLING, "volup", NULL);

	// interrupt volume down button
	gpio_direction_input(IMX_GPIO_NR(5,14));
	irq = gpio_to_irq(IMX_GPIO_NR(5,14));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret = request_irq(irq, (void*)inter_handler_voldown, IRQF_TRIGGER_FALLING, "voldown", NULL);



	init_output();
	init_timer(&my_timer.timer);

	inter_usage = 1;
	return 0;
}

// **** Dev driver release **** //
static int inter_release(struct inode *minode, struct file *mfile){
	free_irq(gpio_to_irq(IMX_GPIO_NR(1, 12)), NULL);
	free_irq(gpio_to_irq(IMX_GPIO_NR(2, 15)), NULL);
	free_irq(gpio_to_irq(IMX_GPIO_NR(5, 14)), NULL);


	del_timer_sync(&my_timer.timer);
	init_output();


	printk(KERN_ALERT "Release Module\n");
	inter_usage = 0;
	return 0;
}




static int inter_register_cdev(void)
{
	int error;
	inter_dev = MKDEV(inter_major, inter_minor);
	error = register_chrdev_region(inter_dev,1,"inter");
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

	// **** Mapping fnd device **** //
	iom_led_addr = ioremap(IOM_LED_ADDRESS, 0x1);
	iom_dot_addr = ioremap(IOM_DOT_ADDRESS, 0x10);
	iom_buz_addr = ioremap(IOM_BUZ_ADDRESS, 0x1);

	printk(KERN_ALERT "Init Module Success \n");
	printk(KERN_ALERT "Device : /dev/elevator, Major Num : 242 \n");
	return 0;
}

static void __exit inter_exit(void) {
	cdev_del(&inter_cdev);
	unregister_chrdev_region(inter_dev, 1);

	// **** Unmap fnd device **** //
	iounmap(iom_led_addr);
	iounmap(iom_dot_addr);
	iounmap(iom_buz_addr);

	printk(KERN_ALERT "Remove Module Success \n");
}

module_init(inter_init);
module_exit(inter_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("kideok Hwang");
