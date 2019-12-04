/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: main.cpp
*BlogAddr: caibiao-lee.blog.csdn.net
*Description:≤‚ ‘≥Ã–Ú
*Date:     2019-10-12
*Author:   Caibiao Lee
*Version:  V1.0
*Others:
*History:
***********************************************************/
#include <stdio.h>  
#include <stdlib.h>  
#include <string>  
#include <stdio.h>
#include <iostream>
#include <cstring>

#include "GPT_DiskPart.h"

int String2int(char *strChar)
{
	int len=0;
	const char *pstrCmp1="0123456789ABCDEF";
	const char *pstrCmp2="0123456789abcdef";
	
	char *pstr=NULL;
	int uiValue=0;
	int j=0;	
	unsigned int t=0;
	int i=0;
	if(NULL==strChar)
		return -1;
	if(0>=(len=strlen((const char *)strChar)))
		return -1;
	if(NULL!=(pstr=strstr(strChar,"0x"))||NULL!=(pstr=strstr(strChar,"0X")))
	{
		pstr=(char *)strChar+2;
		
		if(0>=(len=strlen((const char *)pstr)))
			return -1;
		for(i=(len-1);i>=0;i--)
		{
			if(pstr[i]>'F')
			{
				for(t=0;t<strlen((const char *)pstrCmp2);t++)
				{	
					if(pstrCmp2[t]==pstr[i])
						uiValue|=(t<<(j++*4));
				}
			}
			else
			{
				for(t=0;t<strlen((const char *)pstrCmp1);t++)
				{	
					if(pstrCmp1[t]==pstr[i])
						uiValue|=(t<<(j++*4));
				}
			}
		}
	}
	else
	{
		uiValue=atoi((const char*)strChar);
	}
	return uiValue;
}



int main(int argc, char **argv)
{
    int l_s32Ret = 0;
    unsigned char l_u8PartCount = 0;
    GPTDiskPart* l_pclsDiskPart = NULL;

    l_pclsDiskPart = GPTDiskPart::GPTDiskPartInstance();
    if(NULL==l_pclsDiskPart)
    {
        printf("%s %d get disk part instance error \n",__FUNCTION__,__LINE__);
        return -1;
    }
#if 1
    if(3!=argc)
	{
		printf("=====ERROR!====== argc = %d\n",argc);
		printf("usage:sudo %s parth partcount \n", argv[0]);
		printf("eg 1:sudo %s  /dev/sdb  3\n", argv[0]);
		return -2;
	}

    l_s32Ret = String2int(argv[2]);
    if(l_s32Ret>0)
    {
        l_u8PartCount = l_s32Ret;
        printf("path:%s count=%d \n",argv[1],l_u8PartCount);
        l_pclsDiskPart->GPTDP_CreateGPTDiskPart(argv[1], l_u8PartCount);
    }
#endif
    //l_pclsDiskPart->GPTDP_CreateGPTDiskPart("/dev/mmcblk0", 5);

    return 0;
    
}


