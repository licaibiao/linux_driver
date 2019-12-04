/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: user_gpio.c
*BlogAddr: caibiao-lee.blog.csdn.net
*Description: GPIO读写接口实现
*Date:     2019-10-02
*Author:   Caibiao Lee
*Version:  V1.0
*Others:
*History:
***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memmap.h"
#include "user_gpio.h"

#define DEFAULT_MD_LEN 128   /**内存对齐大小**/

#define PINMUX_BASE_PHYADDR  (0x120F0000) /**复用寄存器基地址**/
#define PINCTRL_BASE_PHYADDR (0x120F0800)

#define GPIO0_BASE_PHYADDR    (0x12150000)
#define GPIO1_BASE_PHYADDR    (0x12160000)
#define GPIO2_BASE_PHYADDR    (0x12170000)
#define GPIO3_BASE_PHYADDR    (0x12180000)
#define GPIO4_BASE_PHYADDR    (0x12190000)
#define GPIO5_BASE_PHYADDR    (0x121a0000)
#define GPIO6_BASE_PHYADDR    (0x121b0000)
#define GPIO7_BASE_PHYADDR    (0x121c0000)
#define GPIO8_BASE_PHYADDR    (0x121d0000)
#define GPIO9_BASE_PHYADDR    (0x121e0000)
#define GPIO10_BASE_PHYADDR   (0x121f0000)
#define GPIO11_BASE_PHYADDR   (0x12200000)
#define GPIO12_BASE_PHYADDR   (0x12210000)
#define GPIO13_BASE_PHYADDR   (0x12220000)

#define DATA_REG_OFFSET       (0x3fc) /**数据寄存器偏移地址**/
#define GPIO0_DATA_PHYADDR    (GPIO0_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO1_DATA_PHYADDR    (GPIO1_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO2_DATA_PHYADDR    (GPIO2_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO3_DATA_PHYADDR    (GPIO3_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO4_DATA_PHYADDR    (GPIO4_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO5_DATA_PHYADDR    (GPIO5_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO6_DATA_PHYADDR    (GPIO6_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO7_DATA_PHYADDR    (GPIO7_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO8_DATA_PHYADDR    (GPIO8_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO9_DATA_PHYADDR    (GPIO9_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO10_DATA_PHYADDR   (GPIO10_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO11_DATA_PHYADDR   (GPIO11_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO12_DATA_PHYADDR   (GPIO12_BASE_PHYADDR + DATA_REG_OFFSET)
#define GPIO13_DATA_PHYADDR   (GPIO13_BASE_PHYADDR + DATA_REG_OFFSET)

#define DIR_REG_OFFSET       (0x400) /**方向寄存器偏移地址**/
#define GPIO0_DIR_PHYADDR    (GPIO0_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO1_DIR_PHYADDR    (GPIO1_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO2_DIR_PHYADDR    (GPIO2_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO3_DIR_PHYADDR    (GPIO3_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO4_DIR_PHYADDR    (GPIO4_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO5_DIR_PHYADDR    (GPIO5_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO6_DIR_PHYADDR    (GPIO6_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO7_DIR_PHYADDR    (GPIO7_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO8_DIR_PHYADDR    (GPIO8_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO9_DIR_PHYADDR    (GPIO9_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO10_DIR_PHYADDR   (GPIO10_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO11_DIR_PHYADDR   (GPIO11_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO12_DIR_PHYADDR   (GPIO12_BASE_PHYADDR + DIR_REG_OFFSET)
#define GPIO13_DIR_PHYADDR   (GPIO13_BASE_PHYADDR + DIR_REG_OFFSET)

unsigned int GPIO_DirRegAddrMap(GPIO_GROUP_E enGroup)
{
    unsigned int uiDirRegAddr = 0;

    switch(enGroup)
    {
        case GROUP_0:
        uiDirRegAddr = GPIO0_DIR_PHYADDR;
        break;

        case GROUP_1:
        uiDirRegAddr = GPIO1_DIR_PHYADDR;
        break;

        case GROUP_2:
        uiDirRegAddr = GPIO2_DIR_PHYADDR;
        break;

        case GROUP_3:
        uiDirRegAddr = GPIO3_DIR_PHYADDR;
        break;

        case GROUP_4:
        uiDirRegAddr = GPIO4_DIR_PHYADDR;
        break;

        case GROUP_5:
        uiDirRegAddr = GPIO5_DIR_PHYADDR;
        break;

        case GROUP_6:
        uiDirRegAddr = GPIO6_DIR_PHYADDR;
        break;

        case GROUP_7:
        uiDirRegAddr = GPIO7_DIR_PHYADDR;
        break;

        case GROUP_8:
        uiDirRegAddr = GPIO8_DIR_PHYADDR;
        break;

        case GROUP_9:
        uiDirRegAddr = GPIO9_DIR_PHYADDR;
        break;

        case GROUP_10:
        uiDirRegAddr = GPIO10_DIR_PHYADDR;
        break;

        case GROUP_11:
        uiDirRegAddr = GPIO11_DIR_PHYADDR;
        break;

        case GROUP_12:
        uiDirRegAddr = GPIO12_DIR_PHYADDR;
        break;

        case GROUP_13:
        uiDirRegAddr = GPIO13_DIR_PHYADDR;
        break;
    }

    return uiDirRegAddr;
}

unsigned int GPIO_DataRegAddrMap(GPIO_GROUP_E enGroup)
{
    unsigned int uiDataRegAddr = 0; 
    switch(enGroup)
    {
        case GROUP_0:
            uiDataRegAddr = GPIO0_DATA_PHYADDR;
            break;

        case GROUP_1:
            uiDataRegAddr = GPIO1_DATA_PHYADDR;
            break;

        case GROUP_2:
            uiDataRegAddr = GPIO2_DATA_PHYADDR;
            break;

        case GROUP_3:
            uiDataRegAddr = GPIO3_DATA_PHYADDR;
            break;
        
        case GROUP_4:
            uiDataRegAddr = GPIO4_DATA_PHYADDR;
            break;
        
        case GROUP_5:
            uiDataRegAddr = GPIO5_DATA_PHYADDR;
            break;
        
        case GROUP_6:
            uiDataRegAddr = GPIO6_DATA_PHYADDR;
            break;
        
        case GROUP_7:
            uiDataRegAddr = GPIO7_DATA_PHYADDR;
            break;
        
        case GROUP_8:
            uiDataRegAddr = GPIO8_DATA_PHYADDR;
            break;
        
        case GROUP_9:
            uiDataRegAddr = GPIO9_DATA_PHYADDR;
            break;
        
        case GROUP_10:
            uiDataRegAddr = GPIO10_DATA_PHYADDR;
            break;
        
        case GROUP_11:
            uiDataRegAddr = GPIO11_DATA_PHYADDR;
            break;
        
        case GROUP_12:
            uiDataRegAddr = GPIO12_DATA_PHYADDR;
            break;
        
        case GROUP_13:
            uiDataRegAddr = GPIO13_DATA_PHYADDR;
            break;
    }

    return uiDataRegAddr;
}

int Set_Gpio_MUXCTRL()
{
    unsigned int uiDataRegAddr  = 0;
    unsigned int uiDataBaseAddr = 0;
    unsigned int uiDataRegVal   = 0;
    void* pMem  = NULL;

    uiDataBaseAddr = 0x120F0000;
    uiDataRegAddr=uiDataBaseAddr+0x164;
    pMem = memmap(uiDataRegAddr, DEFAULT_MD_LEN);
    *(unsigned int*)pMem = 0;
    memunmap(pMem);

    uiDataRegAddr=uiDataBaseAddr+0x170;
    pMem = memmap(uiDataRegAddr, DEFAULT_MD_LEN);
    *(unsigned int*)pMem = 0;
    memunmap(pMem);
    
    return 0;
}

int Set_Gpio_DirVal(GPIO_GROUP_E enGroup, GPIO_BIT_E enBit, GPIO_DIR_E enVal)
{
    unsigned int  uiOldRegVal = 0;
    unsigned int uiDirRegAddr = 0;
    unsigned int uiDirRegVal = 0;
    void* pMem  = NULL;

    uiDirRegAddr = GPIO_DirRegAddrMap(enGroup);

    pMem = memmap(uiDirRegAddr, DEFAULT_MD_LEN);
    if(NULL==pMem)
    {
        printf("%s %d memmap error \n",__FUNCTION__,__LINE__);
        return -1;
    }

    uiOldRegVal = *(unsigned int*)pMem;

    if(GPIO_OUPUT == enVal)
    {
        uiDirRegVal = (1 << enBit) |uiOldRegVal;
    }
    else
    {
        uiDirRegVal = (~(1 << enBit) ) & uiOldRegVal;
    }
    
    *(unsigned int*)pMem = uiDirRegVal;

    memunmap(pMem);
    return 0;
}


int Set_Gpio_DataVal(GPIO_GROUP_E enGroup, GPIO_BIT_E enBit, GPIO_DATA_E enVal)
{
    unsigned int uiOldRegVal = 0;
    unsigned int uiDataRegAddr = 0;
    unsigned int uiDataRegVal = 0;
    void* pMem  = NULL;

    uiDataRegAddr = GPIO_DataRegAddrMap(enGroup);
    
    pMem = memmap(uiDataRegAddr, DEFAULT_MD_LEN);
    if(NULL==pMem)
    {
        printf("%s %d memmap error \n",__FUNCTION__,__LINE__);
        return -1;
    }

    uiOldRegVal = *(unsigned int*)pMem;

    if(GPIO_OUTPUT_HIGH == enVal)
    {
        uiDataRegVal = (1 << enBit) |uiOldRegVal;
    }
    else
    {
        uiDataRegVal = (~(1 << enBit) ) & uiOldRegVal;
    }
    
    *(unsigned int*)pMem = uiDataRegVal;

    memunmap(pMem);
    
    return 0;
}



int Get_Gpio_DataVal(GPIO_GROUP_E enGroup, GPIO_BIT_E enBit, char *pcVal)
{
    unsigned int uiDataRegAddr = 0;
    unsigned char ucDataRegVal = 0;
    void* pMem  = NULL;
    
    if(NULL == pcVal)
    {
        printf("[%s : gpio%d_%d]  error !\n", __func__,  enGroup, enBit);
        return -1;
    }

    uiDataRegAddr = GPIO_DataRegAddrMap(enGroup);
    uiDataRegAddr = uiDataRegAddr&0xffff0003;
    uiDataRegAddr = (uiDataRegAddr|(1<<(enBit+2)));
    pMem = memmap(uiDataRegAddr, DEFAULT_MD_LEN);
    if(NULL==pMem)
    {
        printf("%s %d memmap error \n",__FUNCTION__,__LINE__);
        return -1;
    }

    ucDataRegVal = *(unsigned char*)pMem;
    *pcVal = ucDataRegVal>>enBit;

    memunmap(pMem);
    
    return 0;

}


