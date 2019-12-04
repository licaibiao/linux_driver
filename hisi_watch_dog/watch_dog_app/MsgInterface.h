/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*FileName: MsgInterface.h
*BlogAddr: caibiao-lee.blog.csdn.net
*Description: 
*Date:     2019-12-01
*Author:   Caibiao Lee
*Version:  V1.0
*Others:   
*History:
***********************************************************/
#ifndef MSG_INTERFACE_H
#define MSG_INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define WSD_SUCCEED (0)
#define WSD_FAILURE (-1)
#define MAXSIZE (1024+128)
#define QUEUE_FEEDDOG_KEY   (0x1252)
#define STOP_ROOTFS_FEEDDOG_CMD     (0xa1)
#define HEART_BEATS_ROOTFS_FEEDDOG_CMD  (0xa2)

typedef struct msg_buf
{
    long int MsgType;
    unsigned char SomeText[MAXSIZE];
}MSG_BUF_T;

int Msg_Send(key_t key, MSG_BUF_T *pstMsgBuf, int iMsgLen);
int Msg_Recv(key_t key, MSG_BUF_T *pstMsgBuf);

#endif
