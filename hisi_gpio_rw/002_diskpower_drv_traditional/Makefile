KERN_DIR = /home/biao/VTD-30M/linux-3.18.y

all:
	make ARCH=arm CROSS_COMPILE=arm-hisiv500-linux- -C $(KERN_DIR) M=`pwd` modules 

clean:
	make ARCH=arm CROSS_COMPILE=arm-hisiv500-linux- -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= disk_power_traditional.o
