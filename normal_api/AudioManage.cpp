#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
//#include "netserver.h"
#include "AudioManage.h"
#include "ModuleFuncInterface.h"


typedef struct _DECODE_ARGUMENT_
{
	char  fileName[150];
	int AoDevId;		//ao����豸ID
	int DecCh;		//����ͨ��
	int AoCh;		//Ao���ͨ��
	int startTd;		//�߳����п���
	int Runing;		//�߳��������еı�־
	pthread_t ThreadId;		//�����߳�ID
}DECODE_ARG;





void *DecodeAlarmvoice(void *arg)
{


	return NULL;
}


int StopPlayAlarmVoice()
{


	return HI_SUCCESS;
}


//return :1:���ڲ��ţ�0:�Ѿ�ֹͣ����
int PlayAlarmStatus()
{


	return HI_SUCCESS;
}

int StartPlayAlarmVoice(MUSICINDEX index)
{


	return HI_SUCCESS;
}






void *AudioTalkDecodeProcess(void *Arg)
{


	return NULL;
}


int StartAudioTalkDecode()
{


	return HI_SUCCESS;
}



