/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*BlogAddr: caibiao-lee.blog.csdn.net
*FileName: watch_dog_drv.c
*Description: 
*Date:     2019-12-01
*Author:   Caibiao Lee
*Version:  V1.0
*Others:   
*History:
***********************************************************/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/fs.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/kthread.h>
#include <linux/string.h>
#include <linux/sched.h>

#include "watch_dog_drv.h"

static DEFINE_SPINLOCK(watchdog_lock);
void __iomem *reg_ctl_base_va;
void __iomem *reg_wdt_base_va;

#define IO_WDT_ADDRESS(x) (reg_wdt_base_va + ((x)-(WDT_BASE)))

static unsigned long rate = 3*MHZ;

static inline void watchdog_set_timeout(unsigned int nr)
{
    unsigned long cnt = (~0x0)/rate;        /* max cnt */
    unsigned long flags; 

    spin_lock_irqsave(&watchdog_lock, flags); 

    if( nr==0 || nr>cnt)
        cnt=~0x0; 
    else
        cnt = nr*rate;
    /* unlock watchdog registers */ 
    watchdog_writel(WDT_UNLOCK_VAL, WDT_LOCK); 
    watchdog_writel(cnt, WDT_LOAD); 
    watchdog_writel(cnt, WDT_VALUE); 
    /* lock watchdog registers */ 
    watchdog_writel(0, WDT_LOCK); 
    spin_unlock_irqrestore(&watchdog_lock, flags); 
};

static inline void watchdog_start(void)
{
    unsigned long flags;
    unsigned long t;
    
    spin_lock_irqsave(&watchdog_lock, flags);
    /* unlock watchdog registers */
    watchdog_writel(WDT_UNLOCK_VAL, WDT_LOCK);
    watchdog_writel(0x00, WDT_CTRL);
    watchdog_writel(0x00, WDT_INTCLR);
    watchdog_writel(0x03, WDT_CTRL);
    /* lock watchdog registers */
    watchdog_writel(0, WDT_LOCK);
    /* enable watchdog clock --- set the frequency to 3MHz */
    t = readl(reg_ctl_base_va);
    writel(t & ~0x00800000, reg_ctl_base_va);
    spin_unlock_irqrestore(&watchdog_lock, flags);   
}

static inline void watchdog_stop(void)
{
    unsigned long flags;

    spin_lock_irqsave(&watchdog_lock, flags);

    /* unlock watchdog registers */
    watchdog_writel(WDT_UNLOCK_VAL, WDT_LOCK);

    /* stop watchdog timer */
    watchdog_writel(0x00, WDT_CTRL);
    watchdog_writel(0x00, WDT_INTCLR);

    /* lock watchdog registers */
    watchdog_writel(0, WDT_LOCK);

    spin_unlock_irqrestore(&watchdog_lock, flags);

    watchdog_set_timeout(0);

}

static inline void watchdog_feed(void)
{
    unsigned long flags; 

    spin_lock_irqsave(&watchdog_lock, flags); 
    /* unlock watchdog registers */ 
    watchdog_writel(WDT_UNLOCK_VAL, WDT_LOCK); 
    /* clear watchdog */ 
    watchdog_writel(0x00, WDT_INTCLR); 
    /* lock watchdog registers */ 
    watchdog_writel(0, WDT_LOCK); 
    spin_unlock_irqrestore(&watchdog_lock, flags); 
};


static int watchdog_set_heartbeat(void)
{
    watchdog_set_timeout(WATCHDOG_TIMER_MARGIN);
    watchdog_feed();
 
    return 0;
}

static int __init watchdog_start_init(void)
{
    watchdog_start();

    return 0;
}

static int watchdog_open(struct inode *inode, struct file *file)
{
    int ret = 0;

    if (!capable(CAP_SYS_RAWIO) || !capable(CAP_SYS_ADMIN))
    {
        return -EPERM;
    }
    
    watchdog_feed();
    ret = nonseekable_open(inode, file);

    return ret;
}

static ssize_t watchdog_write(struct file *filp, const char __user *buf, size_t size, loff_t *off)
{
    unsigned char val = 0;
    
    copy_from_user(&val, buf, 1);
    if(val=0x88)
    {
        watchdog_feed();
    }
    
    return size;
}

static int watchdog_release(struct inode *inode, struct file *file)
{

    return 0;
}

static int watchdog_notifier_sys(struct notifier_block *this, unsigned long code,
    void *unused)
{
    if(code==SYS_DOWN || code==SYS_HALT) {
        /* Turn the WDT off */
        watchdog_stop();
    }
    return NOTIFY_DONE;
}

static struct file_operations watchdog_fops = {
    .owner      = THIS_MODULE,
    .llseek     = no_llseek,
    .write      = watchdog_write,
    .open       = watchdog_open,
    .release    = watchdog_release,
};

static struct miscdevice watchdog_miscdev = {
    .minor       = WATCHDOG_MINOR,
    .name        = "watchdog",
    .fops        = &watchdog_fops,
};

static struct notifier_block watchdog_notifier = {
    .notifier_call    = watchdog_notifier_sys,
};

static int __init watchdog_init(void)
{
    int ret = 0;

    reg_wdt_base_va = ioremap_nocache((unsigned long)WDT_BASE, (unsigned long)0x10000);
    if (NULL == reg_wdt_base_va)
    {
        printk(KERN_ERR WATCHDOG_PFX "function %s line %u failed\n", 
            __FUNCTION__, __LINE__);
        return -1;
    }
    
    reg_ctl_base_va = ioremap_nocache((unsigned long)SCTL_BASE, 4);
    if (NULL == reg_ctl_base_va)
    {
        printk(KERN_ERR WATCHDOG_PFX "function %s line %u failed\n", 
            __FUNCTION__, __LINE__);
        iounmap(reg_wdt_base_va);
        return -1;
    }
    
    watchdog_set_heartbeat();

    printk(KERN_WARNING WATCHDOG_PFX "default_margin value must be 0<default_margin<%lu, using %d\n",
            ~0x0/rate, WATCHDOG_TIMER_MARGIN);

    ret = register_reboot_notifier(&watchdog_notifier);
    if(ret) {
        printk(KERN_ERR WATCHDOG_PFX "cannot register reboot notifier (err=%d)\n", ret);
        goto watchdog_init_errA;
    }

    ret = misc_register(&watchdog_miscdev);
    if(ret) {
        printk (KERN_ERR WATCHDOG_PFX "cannot register miscdev on minor=%d (err=%d)\n",
            WATCHDOG_MINOR, ret);
        goto watchdog_init_errB;
    }
    
    ret = watchdog_start_init();
    if(ret) {
        goto watchdog_init_errC;
    }
    return ret;

watchdog_init_errC:
    misc_deregister(&watchdog_miscdev);
watchdog_init_errB:
    unregister_reboot_notifier(&watchdog_notifier);
watchdog_init_errA:
    iounmap(reg_ctl_base_va);
    iounmap(reg_wdt_base_va);
    return ret;
}

static void __exit watchdog_exit(void)
{    
    misc_deregister(&watchdog_miscdev);
    unregister_reboot_notifier(&watchdog_notifier);

    watchdog_set_timeout(0);
    watchdog_stop();

    iounmap(reg_ctl_base_va);
    iounmap(reg_wdt_base_va);
    printk("remove Watchdog ok!\n");
}

module_init(watchdog_init);
module_exit(watchdog_exit);

MODULE_AUTHOR("caibiao lee");
MODULE_DESCRIPTION(" Watchdog Device Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
