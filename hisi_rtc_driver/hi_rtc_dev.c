#include <linux/platform_device.h>
#include <linux/device.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <linux/io.h>
#include <mach/io.h>
#include <linux/module.h>

#define RTC_SPI_BASE_ADDR	IO_ADDRESS(0x120b0000)
#define RTC_SPI_CLK_DIV		(RTC_SPI_BASE_ADDR + 0x000)
#define RTC_SPI_RW			(RTC_SPI_BASE_ADDR + 0x004)
#define HI_RTC_IRQ			(37)	

static struct resource  rtc_resource[] = {
	[0] = {
		.start = RTC_SPI_BASE_ADDR,
		.end   = RTC_SPI_RW,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
	.start = HI_RTC_IRQ,
	.end   = HI_RTC_IRQ,
	.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device wisdom_device_rtc = {
	.name = "biao_rtc",
	.id   = 0,
	.resource = rtc_resource,
	.num_resources = ARRAY_SIZE(rtc_resource),
};


static int biao_rtc_dev_init(void)
{
	platform_device_register(&wisdom_device_rtc);
	return 0;
}

static void biao_rtc_dev_exit(void)
{
	platform_device_unregister(&wisdom_device_rtc);
}

module_init(biao_rtc_dev_init);
module_exit(biao_rtc_dev_exit);

MODULE_LICENSE("GPL");