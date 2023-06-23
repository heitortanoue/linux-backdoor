obj-m = snapshot_sender.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo insmod snapshot_sender.ko

uninstall:
	sudo rmmod snapshot_sender.ko
