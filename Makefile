obj-m = snapshotModule.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo insmod snapshotModule.ko

uninstall:
	sudo rmmod snapshotModule.ko
