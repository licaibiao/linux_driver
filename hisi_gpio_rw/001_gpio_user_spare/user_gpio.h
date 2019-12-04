/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: user_gpio.h
*BlogAddr: caibiao-lee.blog.csdn.net
*Description: GPIO¶ÁÐ´½Ó¿Ú
*Date:     2019-10-02
*Author:   Caibiao Lee
*Version:  V1.0
*Others:
*History:
***********************************************************/
#ifndef _USER_GPIO_H
#define _USER_GPIO_H

#include "user_gpio_struct.h"

int Set_Gpio_DirVal(GPIO_GROUP_E enGroup, GPIO_BIT_E enBit, GPIO_DIR_E enVal);
int Set_Gpio_DataVal(GPIO_GROUP_E enGroup, GPIO_BIT_E enBit, GPIO_DATA_E enVal);
int Get_Gpio_DataVal(GPIO_GROUP_E enGroup, GPIO_BIT_E enBit, char *pcVal);

#endif

