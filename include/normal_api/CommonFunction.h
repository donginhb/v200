/******************************************************************************
  File Name     : ViVo_Api.cpp
  Version       : Initial Draft
  Last Modified :
  Description   : the functions of vi and vi inplement  
  Function List :
  History       :

******************************************************************************/
#ifndef _COMMON_FUNCTION_H_
#define _COMMON_FUNCTION_H_

typedef	void *(*ThreadEntryPtrType)(void *);

#define ADD_THREAD_LOCK(mutex)			pthread_mutex_lock(&mutex)	
#define FREE_THREAD_LOCK(mutex)			pthread_mutex_unlock(&mutex)
	
void mSleep(unsigned int  MilliSecond);

int CreateNormalThreadJpeg(ThreadEntryPtrType entry, void *para, pthread_t *pid);

int CreateNormalThread(ThreadEntryPtrType entry, void *para, pthread_t *pid);

void ExitNormalThread(int ThreadHandle);

void ShutdownSystem();

//#define DEBUG_LOG
 #ifdef DEBUG_LOG
/*****************************************************************************
��������:������־�򿪴�����
�������:��
�������:��
���ز���:�ɹ�����0
��ע��Ϣ:
******************************************************************************/
int OpenDebugLog();

 /*****************************************************************************
��������:д������־������
�������:��
�������:��
���ز���:�ɹ�����0
��ע��Ϣ:
******************************************************************************/
int WriteDebugLog(char *logstr);

 /*****************************************************************************
��������:�رյ�����־������
�������:��
�������:��
���ز���:�ɹ�����0
��ע��Ϣ:
******************************************************************************/
int CloseDebugLog();
 #else
 #define OpenDebugLog()	
 #define WriteDebugLog(logstr)
 #define CloseDebugLog()
#endif
#endif
