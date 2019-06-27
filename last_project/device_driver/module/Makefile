obj-m := elevator.o

KDIR := /work/achroimx_kernel
PWD := $(shell pwd)

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

clean :
	rm -rf *.o
	rm -rf elevator.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers
	rm -rf .tmp*
	rm -rf .elev*
