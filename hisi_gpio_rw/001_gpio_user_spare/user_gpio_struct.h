/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: user_gpio_struct.h
*BlogAddr: caibiao-lee.blog.csdn.net
*Description: GPIO组和引脚结构体定义
*Date:     2019-10-02
*Author:   Caibiao Lee
*Version:  V1.0
*Others:
*History:
***********************************************************/
#ifndef _USER_GPIO_STRUCT_H_
#define _USER_GPIO_STRUCT_H_

typedef enum
{
    GPIO_INPUT = 0,
    GPIO_OUPUT = 1,
}GPIO_DIR_E;

typedef enum
{
    GPIO_OUTPUT_LOW  = 0,
    GPIO_OUTPUT_HIGH = 1, 
}GPIO_DATA_E;

typedef enum
{
    GROUP_0 = 0,
    GROUP_1 = 1,
    GROUP_2 = 2,
    GROUP_3 = 3,
    GROUP_4 = 4,
    GROUP_5 = 5,
    GROUP_6 = 6,
    GROUP_7 = 7,
    GROUP_8 = 8,
    GROUP_9 = 9,
    GROUP_10 = 10,
    GROUP_11 = 11,
    GROUP_12 = 12,
    GROUP_13 = 13,
}GPIO_GROUP_E;
 
typedef enum
{
    BIT_0 = 0,
    BIT_1 = 1,
    BIT_2 = 2,
    BIT_3 = 3,
    BIT_4 = 4,
    BIT_5 = 5,
    BIT_6 = 6,
    BIT_7 = 7,
}GPIO_BIT_E;

#endif

