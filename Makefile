obj-m := shrink_dcache.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
insmod:
	insmod shrink_dcache.ko
rmmod:
	rmmod shrink_dcache.ko
