obj-m = keylogger.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo insmod keylogger.ko

uninstall:
	sudo rmmod keylogger.ko
