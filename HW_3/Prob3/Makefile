obj-m := MyTimer.o

KDIR := ~/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220/

all:
	make ARCH=arm CROSS_COMPILE=arm-linux- \
	-C $(KDIR) M=$(shell pwd)

clean:
	make -C $(KDIR) M=$(shell pwd) clean
