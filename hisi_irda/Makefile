

obj-m := hiir.o

KDIR := /home/biao/NVR_Hi3520/linux-3.10.y_Wifi_NandFlash/
PWD ?= $(shell pwd)

#make -C $(KDIR) M=$(PWD) modules
#make ARCH=arm CROSS_COMPILE=arm-hisiv300-linux-  -C $(KDIR) M=$(PWD) modules
all:
	make ARCH=arm CROSS_COMPILE=arm-hisiv300-linux-  -C $(KDIR) M=$(PWD) modules

clean: 
	rm -rf *.o


