
//+---------------------------------------------------------------------------
//
//  File:   	mutex.h
//
//  Author:		jianghm tablejiang@21cn.com
//
//  Contents:   
//
//  Notes:		��������źŵƣ����ü��������Զ���
//
//  Version:	1.11
//  			
//  Date:		2012-12-13
//
//  History:		
// 			 jianghm	2012-12-13   1.0	   �����ļ�
//
//----------------------------------------------------------------------------

#ifndef _JIANGHM_TCPLIB_CORE_OBJECT_HEADER_3474329203480234
#define _JIANGHM_TCPLIB_CORE_OBJECT_HEADER_3474329203480234


#include "tcplibdef.h"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#endif

#include <list>

#ifdef _BUILD_FOR_ARM_
#define myhtons(var) ((var & 0xff) << 8)|((var & 0xff00) >> 8)
#define myhtonl(var) ((n & 0xff) << 24) |((n & 0xff00) << 8) | \
					 ((n & 0xff0000UL) >> 8) | ((n & 0xff000000UL) >> 24)

#define myntohs		 myhtons
#define myntohl		 myhtonl

#else

#define myhtons(var) (var)
#define myntohs(var) (var)
#define myhtonl(var) (var)
#define myhtonl(var) (var)

#endif

//--------------------------------------------------------------
// ����Cʵ��
typedef struct
{
#ifdef _WIN32
    CRITICAL_SECTION cs;
#else
    pthread_mutex_t cs;
    pthread_mutexattr_t mta ;
#endif
} Zmd_Mutex ;

//������
Zmd_Mutex	*CreateZmdMutex(  );
//������
void		DestroyZmdMutex( Zmd_Mutex *m );
//������
void		LockZmdMutex( Zmd_Mutex *m );
//�˳���
void		UnlockZmdMutex( Zmd_Mutex *m ) ;

//===========================================================
// �źŵƵ�Cʵ��

typedef struct
{
#ifdef _WIN32
    HANDLE sem;
#else
    sem_t sem;
#endif
} Zmd_Sem ;


Zmd_Sem	*CreateZmdSem( ) ;
void		DestroyZmdSem( Zmd_Sem *sem ) ;
void		ZmdSemUp( Zmd_Sem *sem ) ;
void		ZmdSemDown( Zmd_Sem *sem ) ;

//===========================================================
//�߳�ʵ��

//�̻߳ص�����ԭ��
typedef void *(*threadCallback)( void *userData ) ;

//�����߳�
void *CreateZmdThread( void *userData , threadCallback callback ) ;

//�����߳�
//������߳��ṩ��threadCallback���˳��߳�,����ú���
//�ṩ��ǿ����ֹ�߳�,��Ҫ���ŵĹرգ���Ҫҵ����Լ���Э��
void DestroyThread( void *threadHandle ) ;


//------------------------------------------------------
//�źŵ�C++ʵ��
//
//------------------------------------------------------
class CSemaphore
{
public:
    CSemaphore() ;

    ~CSemaphore() ;

    void down() ;
    void up() ;

private:
#ifdef _WIN32
    HANDLE sem;
#else
    sem_t sem;
#endif

};

//---------------------------------------------------------
//������C++ʵ��

class CMutex
{
public:
    CMutex() ;
    ~CMutex() ;

    void lock() ;
    void unlock() ;

private:

#ifdef _WIN32
    CRITICAL_SECTION cs;
#else
    pthread_mutex_t cs;
    pthread_mutexattr_t mta ;
#endif
};

//-------------------------------------
// �Զ������������������������ٽ���
class CAutoMutex
{
public:
    CAutoMutex( CMutex *mutex )
    {
        m_pmutex = mutex ;
        if( m_pmutex )
            m_pmutex->lock() ;
    }

    virtual ~CAutoMutex()
    {
        if( m_pmutex )
        {
            m_pmutex->unlock() ;
            m_pmutex = NULL ;
        }
    };
private:
    CMutex				*m_pmutex ;
};

class CRefObject
{
public:
    CRefObject()
    {
        m_nRefCount = 1 ;
    };
    ~CRefObject() {}	;

    int addRef()
    {
        m_csRef.lock() ;
        //++m_nRefCount ;
        register int ret = ++m_nRefCount ;
#ifdef _DEBUG

#endif
        m_csRef.unlock() ;
        return ret ;
    } ;

    int release()
    {
        m_csRef.lock() ;

        register int ret = --m_nRefCount;

#ifdef _DEBUG

#endif

        if (!ret)
        {
            m_csRef.unlock() ;
            destroyThis();
            return ret ;
        }
        m_csRef.unlock() ;
        return ret ;
    };

protected:
    //public:

    virtual void destroyThis() = 0 ;

    CMutex	m_csRef ;
    int	m_nRefCount ;
};

#endif







