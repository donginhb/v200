
#ifndef  _ADAPTERMESSAGE_H
#define _ADAPTERMESSAGE_H

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h> 
#include <signal.h>
#include <sys/ipc.h>

#include <sys/msg.h>

/*�ź����ȴ�ģʽ */
#define     NO_WAIT                     (0)
#define     WAIT_FOREVER                (1)
#define     MAXLEN_MSGBUF               (256)
#define 	MAX_QUEUEMSGNUM         (16)                /*��Ϣ���е���Ϣ��*/
#define 	MAX_QUEUEMSGSIZE        (256)                /*��Ϣ���е���Ϣ��С*/


#define 	MESSAGENAME			"DHCP"


#define 	CMD_SETDHCP			0x1
#define 	CMD_UPGRADE			0x2

typedef struct tagDHCP_STR
{
	int		MsgCmd;		//hdcp ������Ϊ1
	int   	Aciton;		//0:�ر�dhcp  1:����dhcp                      
	char    NetName[16];//��������
}T_CMDDHCP;



typedef struct tagMSGBUFFER_STR
{
	int   	mtype;                      /*��Ϣ���ͱ������0*/
	char    mtext[MAXLEN_MSGBUF];
}T_MSGBUF;
/***********************************/
/*��Ϣ����*/
/***********************************/

/*��ȡ������Ϣ���е�keyID*/
int OSPGetKeyID(char *szName);
/*������Ϣ����*/
int OSPQueueCreate(char *szName,int *udwQueueID);

/*������Ϣ*/
int OSPQueueSendMessage(int udwQueueID,T_MSGBUF *msg,int dwFlag);

/*������Ϣ*/
int OSPQueueRcvMessage(int udwQueueID,int msgtype,T_MSGBUF *msg,int dwFlag);

/*ɾ����Ϣ����*/
int OSPQueueDelete(int udwQueueID);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*_SYSADAPTER_H*/
