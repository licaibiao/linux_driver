/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: disk_power_test.c
*BlogAddr: caibiao-lee.blog.csdn.net
*Description: 硬盘上电下电驱动测试程序
*Date:     2019-10-02
*Author:   Caibiao Lee
*Version:  V1.0
*Others:   1.该驱动适用于海思HI3520X系列芯片
           2.硬盘上下电的引脚为GPIO_0_2 
*History:
***********************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int fd;
    unsigned char val = 1;
    fd = open("/dev/disk_power", O_RDWR);
    if (fd < 0)
    {
        printf("can't open!\n");
    }
    if (argc != 2)
    {
        printf("Usage :\n");
        printf("%s <on|off>\n", argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "on") == 0)
    {
        val  = 1;
    }
    else
    {
        val = 0;
    }
    
    write(fd, &val, 1);
    
    close(fd);
    
    return 0;
}
