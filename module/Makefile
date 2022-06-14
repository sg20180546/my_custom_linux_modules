obj-m:=hello_module.o

KERNEL_DIR:=/lib/modules/5.18.0-rc6+/build
PWD :=$(shell pwd)

default:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd) modules -j2
clean:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd) clean