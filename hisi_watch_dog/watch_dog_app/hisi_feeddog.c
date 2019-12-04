/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*BlogAddr: caibiao-lee.blog.csdn.net
*FileName: hisi_feeddog.c
*Description: 
*Date:     2019-12-01
*Author:   Caibiao Lee
*Version:  V1.0
*Others:   
*History:
***********************************************************/

#include <sys/time.h>
#include <time.h>

#include "hisi_feeddog.h"
#include "MsgInterface.h"

static char gs_s8RunFlag = 0;

static bool         gs_bFDThrdStart;
static pthread_t    gs_FDThrdPid;

static bool         gs_bIPCMsgThrdStart;
static pthread_t    gs_IPCMsgThrdPid;

int hisi_watchdog_feed(void)
{

    int fd;
    unsigned char l_u8Data = 0x88;

    fd = open("/dev/watchdog", O_RDWR);
    if (fd < 0)
    {
        printf("can't open!\n");
        return -1;
    }

    while(1)
    {
        write(fd, &l_u8Data, 1);
        sleep(1);
    }

    close(fd);  

}


void* feed_dog_thread(void *p)
{
    printf("【RootFs】 [%s]\n", __func__);

    printf("【RootFs】 [使用计数延时方式喂狗]\n", __func__);

    while(true == gs_bFDThrdStart)
    {
        hisi_watchdog_feed();
        usleep(500000);
    }

    return NULL;
}

void* ipc_msg_thread(void *p)
{
    printf("【RootFs】 [%s]\n", __func__);

    MSG_BUF_T stMsgBuf = {0};
    int iRecvLen = 0;
    unsigned char *pucData = NULL;

    unsigned int HeartBeatCnt = 0;

    while(true == gs_bIPCMsgThrdStart)
    {
        usleep(500000);

        HeartBeatCnt++;

        iRecvLen = Msg_Recv(QUEUE_FEEDDOG_KEY, &stMsgBuf);
        //printf("【RootFs】 [HeartBeatCnt:%d][iRecvLen:%d]\n", HeartBeatCnt,iRecvLen);
        if(iRecvLen > 0)
        {
            pucData = stMsgBuf.SomeText;
            printf("【RootFs】 [HeartBeatCnt:%d]\n", HeartBeatCnt);
            if(HEART_BEATS_ROOTFS_FEEDDOG_CMD == pucData[0])
            {
                HeartBeatCnt = 0;
            }
        }

        if(HeartBeatCnt >= 600 )
        {
            printf("【RootFs】 [Stop rootfs feed dog!]\n");
            printf("【RootFs】 [HeartBeatCnt:%d]\n", HeartBeatCnt);
            gs_bFDThrdStart = false;
            gs_bIPCMsgThrdStart = false;
            gs_s8RunFlag = 0;
        }
    }

    return NULL;
}

int creat_feeddog_thread(void)
{
    int iRet = 0;
    gs_bFDThrdStart = true;

    iRet = pthread_create(&gs_FDThrdPid, NULL, feed_dog_thread, \
    (void*)&gs_bFDThrdStart);

    if(iRet == 0)
    {
        printf("【RootFs】 [%s] pthread_create ok!\n", __func__);
        pthread_detach(gs_FDThrdPid);
    }
    else
    {
        printf("【RootFs】 [%s] pthread_create failed!\n", __func__);
    }

    return 0;
}

int creat_ipcmsg_thread(void)
{
    int iRet = 0;
    gs_bIPCMsgThrdStart = true;

    iRet = pthread_create(&gs_IPCMsgThrdPid, NULL, ipc_msg_thread, \
    (void*)&gs_bIPCMsgThrdStart);

    if(iRet == 0)
    {
        printf("【RootFs】 [%s] pthread_create ok!\n", __func__);
        pthread_detach(gs_IPCMsgThrdPid);
    }
    else
    {
        printf("【RootFs】 [%s] pthread_create failed!\n", __func__);
    }

    return 0;
}

int main(int argc, char** argv)
{
    printf("【RootFs】 [The PID of this FeedDog Process is %d]\n", getpid());

    gs_s8RunFlag = 1;

    creat_feeddog_thread();
    creat_ipcmsg_thread();

    do
    {
        sleep(1); 
    }while(gs_s8RunFlag);
       

    printf("【RootFs】 [Exit Process %d]\n", getpid());

    return 0;
}
