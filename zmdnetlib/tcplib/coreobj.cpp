
#include "coreobj.h"

//-----------------------
//�̶߳��������ṹ
//-----------------------
typedef struct
{
#ifdef _WIN32
    HANDLE			thread_handle ;		//Win32�߳̾��
#else
    pthread_t		thread_handle ;		//linux�߳̾��
#endif
    threadCallback	callback ;			//�ص��ӿ�
    void			*userData ;			//�û�����
} ZMD_THREAD_INFO ;

//-------------------------------------------
//ָ��linuxϵͳ���̶߳�ջ��С
//0Ϊʹ��Ĭ���̶߳�ջ
static int g_threadStackSize = 0 ;

//����������ڴ���Ժ�
#ifdef DEBUG_MALLOC
CMutex	mallocMutex ;
int		malloctick = 0 ;

//-----------------------------------------------
//���ڴ��������һ��������,���ڵ����ڴ�й¶
void *debugMalloc( int size )
{
    void *pmem = 0 ;
    mallocMutex.lock() ;

    //malloc����+1
    malloctick ++ ;
    pmem = malloc( size ) ;

    //printf( "debugMalloc count = %d\r\n" , malloctick ) ;

    mallocMutex.unlock() ;

    return pmem ;
}

//-----------------------------------------------
//���ڵ����ڴ�й¶
void	debugFree( void *pmem )
{
    mallocMutex.lock() ;
    //malloc����-1
    malloctick -- ;
    free( pmem ) ;
    //printf( "debugFree count = %d\r\n" , malloctick ) ;
    mallocMutex.unlock() ;
}
#endif	//DEBUG_MALLOC

#ifdef _WIN32
//-----------------------------------
// WIN32�̻߳ص�����
//-----------------------------------
DWORD WINAPI threadFunction(void *pV)
{
    ZMD_THREAD_INFO *pThread = (ZMD_THREAD_INFO *)pV ;
    if( !pThread )
    {
        return 0 ;
    }

    //�����û����̻߳ص�����
    if( pThread->callback )
    {
        pThread->callback( pThread->userData ) ;
    }

    //�����߳̾�����ͷ������󣬹ر��߳̾��
    HANDLE hThread = pThread->thread_handle ;
    DebugFree( pThread ) ;
    //printf( "******************thread exit****************\r\n" ) ;
    if( hThread )
        CloseHandle( hThread ) ;
    return 0 ;
}
#else
//-------------------------------------------
// linux�̻߳ص�����
//-------------------------------------------
void *threadFunction(void *pV)
{
    pthread_detach(pthread_self()) ;

    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction( SIGPIPE, &sa, 0 );

    //�����߳̿��Ա�ȡ��
    pthread_setcancelstate( PTHREAD_CANCEL_ENABLE , NULL ) ;
    //ȡ��ģʽ����Ϊ������ֹ
    pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS , NULL ) ;

    ZMD_THREAD_INFO *pThread = (ZMD_THREAD_INFO *)pV ;
    if( !pThread )
    {
        pthread_exit(0) ;
        return 0;
    }

    //�����û����̻߳ص�����
    if( pThread->callback )
    {
        pThread->callback( pThread->userData ) ;
    }

    //�ͷ��߳̾��
    DebugFree( pThread ) ;
    pthread_exit(0) ;
    return 0 ;
}
#endif	//WIN32

//----------------------------------------------------
// �����߳�
// @userData : �û����ݴ���(�ص����Ὣ�����ݷ��ظ��û�)
// @cb		: �û��ص�����(ԭ�� threadCallback)
//----------------------------------------------------
void *CreateZmdThread( void *userData , threadCallback callback )
{
    ZMD_THREAD_INFO *pThread = (ZMD_THREAD_INFO *)DebugMalloc( sizeof( ZMD_THREAD_INFO ) ) ;
    if( !pThread )
    {
        printf("function %s ,line:%d\n", __FUNCTION__, __LINE__);
        return 0 ;
    }
    memset( pThread , 0 , sizeof( ZMD_THREAD_INFO ) ) ;

    pThread->callback = callback ;
    pThread->userData = userData ;

#ifdef _WIN32
    //win32�����߳�
    unsigned long threadID = 0 ;
    //�����߳�
    pThread->thread_handle = (HANDLE)CreateThread( NULL , 0 , threadFunction, (void *)pThread, 0, &threadID);
    if( !threadID )
    {
        DebugFree( pThread ) ;
        return 0 ;
    }
#else
    /*
    //linux�����߳�
    //linux�߳���������ָ���̶߳�ջ
    //�̶߳�ջ��С��g_threadStackSizeָ��
    bool bSetStackSuccessed = false ;

    //�����߳�Ĭ�϶�ջ��С
    pthread_attr_t attr ;
    pthread_attr_init( &attr ) ;

    if( g_threadStackSize ){

    	if( pthread_attr_setstacksize( &attr , g_threadStackSize )){
    		printf( "pthread_attr_setstacksize failed!!\r\n" ) ;
    		pthread_attr_destroy( &attr ) ;
    		bSetStackSuccessed = false ;
    	}
    	else{
    		bSetStackSuccessed = true ;
    	}
    }

    int ret = 0 ;
    //���ݶ�ջ�������������߳�
    if( bSetStackSuccessed ){
    	//ָ���̶߳�ջ��С
    	ret = pthread_create( &pThread->thread_handle, &attr, threadFunction, (void*)pThread);
    }
    else{
    	printf( "pthread_create() !!\r\n" ) ;
    	//ʹ��ϵͳĬ���̶߳�ջ��С
    	ret = pthread_create( &pThread->thread_handle, NULL, threadFunction, (void*)pThread);
    	printf( "********pthread_create() complete!!\r\n" ) ;
    }

    pthread_attr_destroy( &attr ) ;

    if( ret != 0 ){
    	//����ʧ��
    	printf( "pthread_create failed! ret =%d ! stacksize = %d \r\n" , ret , g_threadStackSize ) ;
    	DebugFree( pThread ) ;
    	return 0 ;
    }
    */

    int ret = 0 ;

    //printf( "pthread_create() !!\r\n" ) ;
    //ʹ��ϵͳĬ���̶߳�ջ��С
    ret = pthread_create( &pThread->thread_handle, NULL, threadFunction, (void *)pThread);
    //printf( "********pthread_create() complete!!\r\n" ) ;

    if( ret != 0 )
    {
        //����ʧ��
        printf( "pthread_create failed! ret =%d ! stacksize = %d \r\n" , ret , g_threadStackSize ) ;
        DebugFree( pThread ) ;
        return 0 ;
    }

#endif

    return pThread ;
}

//----------------------------------------------
// �ⲿ������ֹ�߳�
// @threadHandle :	�̶߳�����
//----------------------------------------------
void DestroyThread( void *threadHandle )
{
    ZMD_THREAD_INFO *pThread = (ZMD_THREAD_INFO *)threadHandle ;
    if( !pThread )
    {
        return ;
    }

    printf( "Destroy Thread !!\r\n" ) ;

    if( pThread->thread_handle )
    {
#ifdef _WIN32
        HANDLE hThread = pThread->thread_handle ;
        TerminateThread( hThread , 0 ) ;
        CloseHandle(hThread);
        DebugFree( pThread ) ;
#else
        pthread_t hThread = pThread->thread_handle ;
        pthread_cancel(  hThread  ) ;
        pthread_join( hThread , NULL ) ;
        DebugFree( pThread ) ;
#endif
    }
}


#ifdef _WIN32
//-----------------------------------
// win32���ں˶����װ Cʵ��
//-----------------------------------

Zmd_Mutex *CreateZmdMutex(  )
{
    Zmd_Mutex *pMutex = (Zmd_Mutex *)malloc( sizeof( Zmd_Mutex ) ) ;
    if( !pMutex )
        return 0 ;

#ifdef _WIN32
    InitializeCriticalSection(&pMutex->cs);
#else
    pthread_mutexattr_init( &pMutex->mta ) ;
    pthread_mutexattr_settype( &pMutex->mta , PTHREAD_MUTEX_RECURSIVE_NP ) ;
    pthread_mutex_init( &pMutex->cs , &mta ) ;
#endif

    return pMutex ;
}

void DestroyZmdMutex( Zmd_Mutex *m )
{
    if( !m )
        return ;
#ifdef _WIN32
    DeleteCriticalSection(&m->cs);
#else
    pthread_mutexattr_destroy( &m->mta ) ;
    pthread_mutex_destroy(&m->cs);
#endif

    free( m ) ;

}

void LockZmdMutex( Zmd_Mutex *m )
{
    if( !m )
        return ;

#ifdef _WIN32
    EnterCriticalSection(&m->cs);
#else
    pthread_mutex_lock(&m->cs);
#endif

}

void UnlockZmdMutex( Zmd_Mutex *m )
{
    if( !m )
        return ;

#ifdef _WIN32
    LeaveCriticalSection(&m->cs);
#else
    pthread_mutex_unlock(&m->cs);
#endif

}


Zmd_Sem *CreateZmdSem( )
{
    Zmd_Sem *pSem = (Zmd_Sem *)malloc( sizeof(Zmd_Sem) ) ;
    if( !pSem )
        return 0 ;

#ifdef _WIN32
    pSem->sem = CreateSemaphore(NULL, 0, 65535 , NULL);
#else
    sem_init(&pSem->sem, 0, 0);
#endif
    return pSem ;
}

void DestroyZmdSem( Zmd_Sem *sem )
{
    if( !sem )
        return ;

#ifdef _WIN32
    CloseHandle(sem->sem);
#else
    sem_destroy(&sem->sem);
#endif
    free( sem ) ;

}

void ZmdSemUp( Zmd_Sem *sem )
{
    if( !sem )
        return ;

#ifdef _WIN32
    ReleaseSemaphore(sem->sem, 1, NULL);
#else
    sem_post(&sem->sem);
#endif

}

void ZmdSemDown( Zmd_Sem *sem )
{
    if( !sem )
        return ;

#ifdef _WIN32
    WaitForSingleObject(sem->sem, INFINITE);
#else
    int ret = 0 ;
    while (0 != (ret = sem_wait(&sem->sem)))
    {
        if (errno != EINTR)
        {
            printf ("Sem_wait returned %ld\n", (unsigned long)ret);
            printf("sem_wait for handler failed");
        }
    }
#endif

}

//-----------------------------------
// win32���ں˶����װ C++ʵ��
//-----------------------------------
CSemaphore::CSemaphore( )
{
    sem = CreateSemaphore(NULL, 0, 65535 , NULL);
}

CSemaphore::~CSemaphore()
{
    if (sem)
        CloseHandle(sem);
}

void CSemaphore::down()
{
    WaitForSingleObject(sem, INFINITE);
}

void CSemaphore::up()
{
    ReleaseSemaphore(sem, 1, NULL);
}


CMutex::CMutex()
{
    InitializeCriticalSection(&cs);
}

CMutex::~CMutex()
{
    DeleteCriticalSection(&cs);
}

void CMutex::lock()
{
    EnterCriticalSection(&cs);
}

void CMutex::unlock()
{
    LeaveCriticalSection(&cs);
}

#else		// Linux implementation

//-----------------------------------
// linux���ں˶����װ
//-----------------------------------
CSemaphore::CSemaphore()
{
    sem_init(&sem, 0, 0);
}

CSemaphore::~CSemaphore()
{
    sem_destroy(&sem);
}

void CSemaphore::down()
{
    int ret = 0 ;
    while (0 != (ret = sem_wait(&sem)))
    {
        if (errno != EINTR)
        {
            printf ("Sem_wait returned %ld\n", (unsigned long)ret);
            printf("sem_wait for handler failed");
            exit(1);
        }
    }

    //sem_wait(&sem);
}

void CSemaphore::up()
{
    sem_post(&sem);
}


CMutex::CMutex()
{
    pthread_mutexattr_init( &mta ) ;
    pthread_mutexattr_settype( &mta , PTHREAD_MUTEX_RECURSIVE_NP ) ;
    pthread_mutex_init( &cs , &mta ) ;
}

CMutex::~CMutex()
{
    pthread_mutexattr_destroy( &mta ) ;
    pthread_mutex_destroy(&cs);
}

void CMutex::lock()
{
    //printf( " lock the %d \r\n" , &cs ) ;
    pthread_mutex_lock(&cs);
}

void CMutex::unlock()
{
    //printf( " unlock the %d \r\n" , &cs ) ;
    pthread_mutex_unlock(&cs);
}

#endif

