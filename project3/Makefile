obj-m := sbd_crypto.o
KDIR := /scratch/fall2017/44/linux-yocto-3.19/
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
