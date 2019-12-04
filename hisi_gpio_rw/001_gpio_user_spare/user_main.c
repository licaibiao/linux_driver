/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: user_main.h
*BlogAddr: caibiao-lee.blog.csdn.net
*Description: ≤‚ ‘≥Ã–Ú
*Date:     2019-10-02
*Author:   Caibiao Lee
*Version:  V1.0
*Others:
*History:
***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user_gpio.h"

int main(int argc, char** argv)
{
    GPIO_GROUP_E GrpNo; 
    GPIO_BIT_E PinNo;
    char PinValue;

    if(argc<3)
    {
        printf("=====ERROR!======\n");
        printf("usage1: %s GrpNo PinNo \n", argv[0]);
        printf("usage1: %s GrpNo PinNo PinValue\n", argv[0]);
        return -1;
    }

    GrpNo=(GPIO_GROUP_E)atoi(argv[1]); 
    PinNo=(GPIO_BIT_E)atoi(argv[2]); 

    if(argc==4)
    {
        PinValue=atoi(argv[3]); 
        Set_Gpio_DirVal(GrpNo, PinNo, GPIO_OUPUT);    
        Set_Gpio_DataVal(GrpNo, PinNo,(GPIO_DATA_E)PinValue);
        printf("----[  GPIO%d_%d   :  GPIO_OUPUT val %d  ]----\n",GrpNo,PinNo,PinValue);
    }
    else if(argc==3)
    {
        Set_Gpio_DirVal(GrpNo, PinNo, GPIO_INPUT);    
        Get_Gpio_DataVal(GrpNo, PinNo,&PinValue);
        printf("----[  GPIO%d_%d   :  GPIO_INPUT val %d  ]----\n", GrpNo,PinNo,PinValue);
    }
    else
    {    
        printf("argc %d error!\n", argc);
        return -1;
    }

    return 0;
}

