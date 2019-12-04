/************************************************************
*Copyright (C),lcb0281at163.com lcb0281atgmail.com
*BlogAddr: caibiao-lee.blog.csdn.net
*FileName: MsgInterface.c
*Description: 
*Date:     2019-12-01
*Author:   Caibiao Lee
*Version:  V1.0
*Others:   
*History:
***********************************************************/

#include "MsgInterface.h"

int Msg_Init(int msgKey)  
{  
    int qid;  
    key_t key = msgKey;
 
    qid = msgget(key, 0);  
    if(qid < 0)
    {  
        qid = msgget(key, IPC_CREAT|0666);  
        printf("Create msg queue id:%d\n", qid);  
    }

    printf("msg queue id:%d\n", qid);  
    return qid;  
}  
 
int Msg_Kill(int qid)  
{  
    msgctl(qid, IPC_RMID, NULL);  
    printf("Kill queue id:%d\n", qid);  
    return 0;  
}


int Msg_GetFreeSpace(int msgid)
{
    struct msqid_ds msg_info;

    if(-1 == msgctl(msgid, IPC_STAT, &msg_info))
    {
        perror("msgctl:");
        return WSD_FAILURE;
    }
    
    return msg_info.msg_qbytes - msg_info.msg_cbytes;
}

int Msg_Send(key_t key, MSG_BUF_T *pstMsgBuf, int iMsgLen)
{
    int nret=WSD_FAILURE;
    int iMsgId = msgget(key, 666|IPC_CREAT);
    struct msgbuf *pmsg = (msgbuf *)pstMsgBuf;
    pstMsgBuf->MsgType = 1;
    
    if(iMsgId >= 0)
    {
        unsigned char count = 10;
        
        while(--count)
        {
            if(Msg_GetFreeSpace(iMsgId) > iMsgLen)
                break;
            usleep(300000);
        }
        
        if(count > 0)
        {
            nret = msgsnd(iMsgId,pmsg,iMsgLen,IPC_NOWAIT);
        }
        else
        {
            if( msgctl(iMsgId,IPC_RMID,0)==-1)
            {
                perror("msgctl IPC_RMID !!!!!!!!!!!! err:\n");
                return WSD_FAILURE;
            }           
        }
        
        if(nret < 0)
        {
            perror("msgsnd send err:\n");
        }
        else
        {
            printf("msg:%d sent,nret=%d\n",key,iMsgLen);
        }
    }
    else
    {
        perror("00 msgget\n");
    }    
    return nret;
}

int  Msg_Recv(key_t key, MSG_BUF_T *pstMsgBuf)
{
    int nret=-1;
    int iMsgId = -1;
    struct msgbuf *pmsg = (msgbuf *)pstMsgBuf;
    iMsgId = msgget(key, 666|IPC_CREAT);

    if(iMsgId >= 0)
    {
        nret = msgrcv(iMsgId, pmsg, MAXSIZE, 0, IPC_NOWAIT);
    }
    else
    {
        perror("key msgget err");
    }
    return nret;
}

