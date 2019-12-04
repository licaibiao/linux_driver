export ARCH=arm

CUR_DIR=`pwd`
export CROSS_COMPILE=$(CUR_DIR)/../out/sunivw1p1/linux/common/buildroot/external-toolchain/bin/arm-none-linux-gnueabi-
KERN_DIR = ../../linux-3.10
KBUILD_EXTRA_SYMBOLS = ../../linux-3.10/output/lib/modules/3.10.65/Module.symvers
all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= mpu6050.o bmp280.o hmc5883.o ks103.o fbm320.o awp100.o
