obj-m := p07470p33l3r.o
KERNEL_DIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd)
	rm -rf *.o *.symvers *.mod *.mod.c *.order .*.cmd
clean:
	rm -rf *.o *.ko *.symvers *.mod *.mod.c *.order .*.cmd
