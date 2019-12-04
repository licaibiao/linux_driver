/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*BlogAddr: caibiao-lee.blog.csdn.net
*FileName: watch_dog_drv.h
*Description: 
*Date:     2019-12-01
*Author:   Caibiao Lee
*Version:  V1.0
*Others:   
*History:
***********************************************************/

#ifndef _WATCH_DOG_DRV_H_
#define _WATCH_DOG_DRV_H_

#define SCTL_BASE             0x12050000
#define WDT_BASE              0x12070000
#define WDT_REG(x)            (WDT_BASE + (x))

#define WDT_LOAD              0x000
#define WDT_VALUE             0x004
#define WDT_CTRL              0x008
#define WDT_INTCLR            0x00C
#define WDT_RIS               0x010
#define WDT_MIS               0x014
#define WDT_LOCK              0xC00
#define WDT_UNLOCK_VAL        0x1ACCE551

#define WATCHDOG_PFX "WatchDog: "

#define watchdog_readl(x)      readl(IO_WDT_ADDRESS(WDT_REG(x)))
#define watchdog_writel(v,x)   writel(v, IO_WDT_ADDRESS(WDT_REG(x)))
#define WATCHDOG_TIMER_MARGIN  3
#define MHZ (1000*1000)

#endif  /* ifndef _LINUX_WATCHDOG_H */
