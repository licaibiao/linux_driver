/*
 * DS1286 Real Time Clock interface for Linux
 *
 * Copyright (C) 1998, 1999, 2000 Ralf Baechle
 * Copyright (C) 2008 Thomas Bogendoerfer
 *
 * Based on code written by Paul Gortmaker.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#include <linux/module.h>
#include <linux/rtc.h>
#include <linux/platform_device.h>
#include <linux/bcd.h>
#include <linux/hi_rtc.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <mach/io.h> 
#define DRV_VERSION		"1.0"

/* RTC Control over SPI */
#define RTC_SPI_BASE_ADDR	IO_ADDRESS(0x120b0000)
#define SPI_CLK_DIV			(RTC_SPI_BASE_ADDR + 0x000)
#define SPI_RW				(RTC_SPI_BASE_ADDR + 0x004)

/* Define the union SPI_RW */
typedef union {
	struct {
		unsigned int spi_wdata		: 8; /* [7:0] */
		unsigned int spi_rdata		: 8; /* [15:8] */
		unsigned int spi_addr		: 7; /* [22:16] */
		unsigned int spi_rw		    : 1; /* [23] */
		unsigned int spi_start		: 1; /* [24] */
		unsigned int reserved		: 6; /* [30:25] */
		unsigned int spi_busy		: 1; /* [31] */
	} bits;
	/* Define an unsigned member */
	unsigned int u32;
} U_SPI_RW;

#define SPI_WRITE		(0)
#define SPI_READ		(1)

#define RTC_IRQ			(37)	

/* RTC REG */
#define RTC_10MS_COUN	0x00
#define RTC_S_COUNT  	0x01
#define RTC_M_COUNT  	0x02  
#define RTC_H_COUNT  	0x03
#define RTC_D_COUNT_L	0x04
#define RTC_D_COUNT_H	0x05
#define RTC_MR_10MS		0x06
#define RTC_MR_S		0x07
#define RTC_MR_M		0x08
#define RTC_MR_H		0x09
#define RTC_MR_D_L		0x0A
#define RTC_MR_D_H		0x0B
#define RTC_LR_10MS		0x0C
#define RTC_LR_S		0x0D
#define RTC_LR_M		0x0E
#define RTC_LR_H		0x0F
#define RTC_LR_D_L		0x10
#define RTC_LR_D_H		0x11
#define RTC_LORD		0x12
#define RTC_IMSC		0x13
#define RTC_INT_CLR		0x14
#define RTC_INT_MASK	0x15
#define RTC_INT_RAW		0x16
#define RTC_CLK			0x17
#define RTC_POR_N		0x18
#define RTC_SAR_CTRL	0x1A

#define RTC_FREQ_H		0x51
#define RTC_FREQ_L		0x52

#define RETRY_CNT 100

#define FREQ_MAX_VAL	3277000
#define FREQ_MIN_VAL	3276000

static DEFINE_MUTEX(hirtc_lock);

struct biao_rtc_priv {
	struct rtc_device *rtc;
	u32 __iomem *rtcregs;
	spinlock_t lock;
};
static int spi_write(char reg, char val)
{
	U_SPI_RW w_data, r_data;
	r_data.u32 = 0;
	w_data.u32 = 0;
	
	w_data.bits.spi_wdata = val;
	w_data.bits.spi_addr = reg;
	w_data.bits.spi_rw = SPI_WRITE;
	w_data.bits.spi_start = 0x1;
	writel(w_data.u32, (volatile void __iomem *)SPI_RW);

	do {
		r_data.u32 = readl((volatile void __iomem *)SPI_RW);
	} while (r_data.bits.spi_busy);

	return 0;
}

static int spi_rtc_write(char reg, char val)
{
	mutex_lock(&hirtc_lock);
	spi_write(reg, val);
	mutex_unlock(&hirtc_lock);
	
	return 0;
}

static int spi_read(char reg, char *val)
{
	U_SPI_RW w_data, r_data;

	r_data.u32 = 0;
	w_data.u32 = 0;
	w_data.bits.spi_addr = reg;
	w_data.bits.spi_rw = SPI_READ;
	w_data.bits.spi_start = 0x1;
	

	writel(w_data.u32, (volatile void __iomem *)SPI_RW);

	do {
		r_data.u32 = readl((volatile void __iomem *)SPI_RW);
	} while (r_data.bits.spi_busy);
	
	*val = r_data.bits.spi_rdata;

	return 0;
}

static int spi_rtc_read(char reg, char *val)
{
	mutex_lock(&hirtc_lock);
	spi_read(reg, val);
	mutex_unlock(&hirtc_lock);

	return 0;
}

static int hirtc_get_alarm(struct device *dev, struct rtc_wkalrm *alm)
{
	unsigned char dayl, dayh;
	unsigned char second, minute, hour;
	unsigned long seconds = 0;
	unsigned int day;

	spi_rtc_read(RTC_MR_S, &second);
	spi_rtc_read(RTC_MR_M, &minute);
	spi_rtc_read(RTC_MR_H, &hour);
	spi_rtc_read(RTC_MR_D_L, &dayl);
	spi_rtc_read(RTC_MR_D_H, &dayh);
	day = (unsigned int)(dayl | (dayh << 8)); 
	seconds = second + minute*60 + hour*60*60 + day*24*60*60;
	rtc_time_to_tm(seconds,&(alm->time));

	return 0;
}

static int hirtc_set_alarm(struct device *dev, struct rtc_wkalrm *alm)
{
	unsigned int days;
	unsigned long seconds = 0;

	if(rtc_valid_tm(&alm->time))
		return -1;
    
    rtc_tm_to_time(&alm->time,&seconds);
	days = seconds/(60*60*24);

	spi_rtc_write(RTC_MR_10MS, 0);
	spi_rtc_write(RTC_MR_S, alm->time.tm_sec);
	spi_rtc_write(RTC_MR_M, alm->time.tm_min);
	spi_rtc_write(RTC_MR_H, alm->time.tm_hour);
	spi_rtc_write(RTC_MR_D_L, (days & 0xFF));
	spi_rtc_write(RTC_MR_D_H, (days >> 8));

	return 0;
}
static int hirtc_get_time(struct device *dev, struct rtc_time *tm)
{
	unsigned char dayl, dayh;
	unsigned char second, minute, hour;
	unsigned long seconds = 0;
	unsigned int day;
	
    unsigned char raw_value;
    
    spi_rtc_read(RTC_LORD, &raw_value);
    if(raw_value & 0x4)
    {
        spi_rtc_write(RTC_LORD, (~(1<<2)) & raw_value);
    }

	spi_rtc_read(RTC_LORD, &raw_value);
    spi_rtc_write(RTC_LORD, (1<<1) | raw_value);//lock the time

    do
    {
        spi_rtc_read(RTC_LORD, &raw_value);
    }while(raw_value & 0x2);
	udelay(1000);
	spi_rtc_read(RTC_S_COUNT, &second);
	spi_rtc_read(RTC_M_COUNT, &minute);
	spi_rtc_read(RTC_H_COUNT, &hour);
	spi_rtc_read(RTC_D_COUNT_L, &dayl);
	spi_rtc_read(RTC_D_COUNT_H, &dayh);
	day = (dayl | (dayh << 8));
	seconds = second + minute*60 + hour*60*60 + day*24*60*60;
	rtc_time_to_tm(seconds, tm);
    printk("[FJW]:hirtc_get_time [year %d] [mon %d] [mday %d] [hour %d] [min %d] [sec %d] \n"
        ,tm->tm_year,tm->tm_mon,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);

	return 0;
}

static int hirtc_set_time(struct device *dev, struct rtc_time *tm)
{
	unsigned char ret;
	unsigned int days;
	unsigned long seconds = 0;
	unsigned int cnt = 0;
    
	if(rtc_valid_tm(tm))
		return -1;
    
    rtc_tm_to_time(tm,&seconds);
	days = seconds/(60*60*24);
    
    printk("[FJW]:hirtc_set_time [year %d] [mon %d] [mday %d] [hour %d] [min %d] [sec %d] \n"
        ,tm->tm_year,tm->tm_mon,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
    
	do {
		spi_rtc_read(RTC_LORD, &ret);
		udelay(1000);
		cnt++;
	} while ( (ret&0x1) && (cnt < RETRY_CNT));
	
	if (cnt >= RETRY_CNT) {
		printk(KERN_ERR "check state error!\n");
		return -1;
	}
	spi_rtc_write(RTC_LR_10MS, 0);
	spi_rtc_write(RTC_LR_S, tm->tm_sec);
	spi_rtc_write(RTC_LR_M, tm->tm_min);
	spi_rtc_write(RTC_LR_H, tm->tm_hour);
	spi_rtc_write(RTC_LR_D_L, (days & 0xFF));
	spi_rtc_write(RTC_LR_D_H, (days >> 8));
	spi_rtc_write(RTC_LORD, (ret|0x1));
	return 0;
}

static const struct rtc_class_ops hi_rtc_ops = {
	.read_time	= hirtc_get_time,
	.set_time	= hirtc_set_time,
	.read_alarm	= hirtc_get_alarm,
	.set_alarm	= hirtc_set_alarm,
};

static int hi_rtc_probe(struct platform_device *pdev)
{
	struct rtc_device *rtc;
	struct resource *res;
	struct biao_rtc_priv *priv;
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODEV;
	priv = devm_kzalloc(&pdev->dev, sizeof(struct biao_rtc_priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;
	priv->rtcregs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(priv->rtcregs))
		return PTR_ERR(priv->rtcregs);
	spin_lock_init(&priv->lock);
	platform_set_drvdata(pdev, priv);
	rtc = devm_rtc_device_register(&pdev->dev, "biao_rtc", &hi_rtc_ops,THIS_MODULE);
	if (IS_ERR(rtc))
		return PTR_ERR(rtc);
	priv->rtc = rtc;
	return 0;
}

static int hi_rtc_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver hi_biao_rtc_platform_driver = {
	.driver		= {
		.name	= "biao_rtc",
		.owner	= THIS_MODULE,
	},
	.probe		= hi_rtc_probe,
	.remove		= hi_rtc_remove,
};

module_platform_driver(hi_biao_rtc_platform_driver);

MODULE_AUTHOR("Caibiao Lee");
MODULE_DESCRIPTION("biao_rtc RTC driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
MODULE_ALIAS("platform:biao_rtc");
