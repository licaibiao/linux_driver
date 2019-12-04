/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: memmap.c
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

#include "memmap.h" 

#define PAGE_SIZE       (0x1000)            /**4K**/
#define PAGE_SIZE_MASK  (0xfffff000)

static int fd = -1;
static const char dev[]="/dev/mem";

typedef struct tag_MMAP_Node
{
    unsigned int Start_P;
    unsigned int Start_V;
    unsigned int length;
    unsigned int refcount;  /* map后的空间段的引用计数 */
    struct tag_MMAP_Node * next;
}TMMAP_NODE_S;

TMMAP_NODE_S * pTMMAPNode = NULL;


/* no need considering page_size of 4K */
void * memmap(unsigned int phy_addr, unsigned int size)
{
    unsigned int phy_addr_in_page;
    unsigned int page_diff;
    unsigned int size_in_page;

    TMMAP_NODE_S * pTmp;
    TMMAP_NODE_S * pNew;
    
    void *addr=NULL;

    if(size == 0)
    {
        printf("memmap():size can't be zero!\n");
        return NULL;
    }

    /* check if the physical memory space have been mmaped */
    pTmp = pTMMAPNode;
    while(pTmp != NULL)
    {
        if( (phy_addr >= pTmp->Start_P) && 
            ( (phy_addr + size) <= (pTmp->Start_P + pTmp->length) ) )
        {
            pTmp->refcount++;   /* referrence count increase by 1  */
            return (void *)(pTmp->Start_V + phy_addr - pTmp->Start_P);
        }

        pTmp = pTmp->next;
    }

    /* not mmaped yet */
    if(fd < 0)
    {
        /* dev not opened yet, so open it */
        fd = open (dev, O_RDWR | O_SYNC);
        if (fd < 0)
        {
            printf("memmap():open %s error!\n", dev);
            return NULL;
        }
    }

    /* addr align in page_size(4K) */
    phy_addr_in_page = phy_addr & PAGE_SIZE_MASK;
    page_diff = phy_addr - phy_addr_in_page;

    /* size in page_size */
    size_in_page =((size + page_diff - 1) & PAGE_SIZE_MASK) + PAGE_SIZE;

    addr = mmap ((void *)0, size_in_page, PROT_READ|PROT_WRITE, MAP_SHARED, fd, phy_addr_in_page);
    if (addr == MAP_FAILED)
    {
        printf("memmap():mmap @ 0x%x error!\n", phy_addr_in_page);
        return NULL;
    }

    /* add this mmap to MMAP Node */
    pNew = (TMMAP_NODE_S *)malloc(sizeof(TMMAP_NODE_S));
    if(NULL == pNew)
    {
        printf("memmap():malloc new node failed!\n");
        return NULL;
    }
    pNew->Start_P = phy_addr_in_page;
    pNew->Start_V = (unsigned int)addr;
    pNew->length = size_in_page;
    pNew->refcount = 1;
    pNew->next = NULL;
    
    if(pTMMAPNode == NULL)
    {
        pTMMAPNode = pNew;
    }
    else
    {
        pTmp = pTMMAPNode;
        while(pTmp->next != NULL)
        {
            pTmp = pTmp->next;
        }

        pTmp->next = pNew;
    }

    return (void *)((unsigned int)addr+page_diff);

}

int memunmap(void * addr_mapped)
{
    TMMAP_NODE_S * pPre;
    TMMAP_NODE_S * pTmp;

    if(pTMMAPNode == NULL)
    {
        printf("memunmap(): address have not been mmaped!\n");
        return -1;
    }

    /* check if the physical memory space have been mmaped */
    pTmp = pTMMAPNode;
    pPre = pTMMAPNode;

    do
    {
        if( ((unsigned int)addr_mapped >= pTmp->Start_V) && 
            ((unsigned int)addr_mapped <= (pTmp->Start_V + pTmp->length)) )
        {
            pTmp->refcount--;   /* referrence count decrease by 1  */
            if(0 == pTmp->refcount)
            {
                /* 引用计数变为0, 被map的内存空间不再使用,此时需要进行munmap回收 */
 
                //printf("memunmap(): map node will be remove!\n");

                /* delete this map node from pMMAPNode */
                if(pTmp == pTMMAPNode)
                {
                    pTMMAPNode = NULL;
                }
                else
                {
                    pPre->next = pTmp->next;
                }

                /* munmap */
                if(munmap((void *)pTmp->Start_V, pTmp->length) != 0 )
                {
                    printf("memunmap(): munmap failed!\n");
                }

                free(pTmp);
            }
            
            return 0;
        }

        pPre = pTmp;
        pTmp = pTmp->next;
    }while(pTmp != NULL);

    printf("memunmap(): address have not been mmaped!\n");
    return -1;
}

