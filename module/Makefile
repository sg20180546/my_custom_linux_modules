obj-m:=hello_module.o

KERNEL_DIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules -j2
clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean