obj-m := shrink_dcache.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
insmod:
	insmod shrink_dcache.ko
rmmod:
	rmmod shrink_dcache.ko

disk:
	dd if=/dev/zero of=/tmp/disk bs=1M count=1000
	mkfs.xfs -f /tmp/disk

mount:
	mount -o loop -t xfs /tmp/disk /mnt

umount:
	umount /mnt

romount:
	mount -o loop -r -t xfs /tmp/disk /mnt
