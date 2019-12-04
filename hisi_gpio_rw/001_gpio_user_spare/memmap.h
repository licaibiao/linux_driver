/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: memmap.h
*BlogAddr: caibiao-lee.blog.csdn.net
*Description: 内存映射接口实现
*Date:     2019-10-02
*Author:   Caibiao Lee
*Version:  V1.0
*Others:  1.该套内存映射接口带有一个链表，如果需要频繁操作某一地址，
          可以不用释放该映射，在映射函数中回去链表中查询是否之前
          映射了并且没有释放。
          2.在HI3520DV400设备中，内存最小对齐单位是128字节，
          而不是一页4k
*History:
***********************************************************/

#ifndef __MEM_MAP_H__
#define __MEM_MAP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void * memmap(unsigned int phy_addr, unsigned int size);
extern int memunmap(void * addr_mapped);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif

