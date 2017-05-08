
#include "tcplib.h"
#include "coreobj.h"

//--------------------------------------------
//�������ṹ
typedef struct
{
    void					*server_handle ;		//���������
    int						server_sock ;			//����������sock
    void					*accept_thread_handle ;	//�����������߳̾��
    onAcceptTcpLibCallback	accept_callback ;		//���ӵ����¼��ص�
    onCloseTcpLibCallback	onclose_callback ;		//���ӹر��¼��ص�
    onReceiveTcpLibCallback	onrecv_callback ;		//�������ݵ����¼��ص�
    onIdleTcpLibCallback	onidle_callback ;
} ZMD_SERVER_INFO ;


typedef struct
{
    int					session_sock ;
    int					userData ;
    unsigned int		remote_ip ;
    unsigned int		remote_port ;
    void				*server_handle ;
    void				*session_thread_handle ;	//�Ự�߳̾��

    int					recv_buf_cursor ;		  	//���ջ�������
    int					run ;						//���б�־
    CMutex				send_mutex ;				//���������������ݰ������ͻ�����

    onReceiveTcpLibCallback	recv_cb ;				//��������֪ͨ�ص�
    onCloseTcpLibCallback	close_cb ;				//�ر��¼�֪ͨ�ص�
    onIdleTcpLibCallback	idle_cb ;				//����֪ͨ
    char				recv_buf[TCP_RECV_BUF_SIZE] ;

} ZMD_SESSION_INFO ;


//==============================================================
//Tcp�Ự��������
/*
* Function:		����һ��tcp�Ự��������ͻ���ͨѶ��

* Called By:
* Input:

	@serverHandle :	�ûỰ������tcp���������
	@sock :			�ûỰ����ͨѶ��socket���
	@recvcb :		�ûỰ���յ�����֪ͨ
	@closecb :		�ûỰ���ӹر�֪ͨ
	@idlecb :		�ûỰ����֪ͨ

* Output:
* Return:	�Ự���������NULLΪ����ʧ��

* Others:
*/
void *CreateTcpSession( void *serverHandle , int sock , onReceiveTcpLibCallback recvcb ,
                        onCloseTcpLibCallback closecb , onIdleTcpLibCallback idlecb );

/*
* Function:		����һ��tcp�Ự,�ýӿ�ֻ���Ÿ�tcplistenerʹ�ã��ⲿ��ֹʹ��
				Ҫ�ڻỰ�߳̽����Ự�����StopTcpSession.
				����Cû��protected, friend�������û�취�����ýӿڡ�

* Called By:
* Input:

	@handle :	�Ự���

* Output:
* Return:
* Others:
*/
void DestroyTcpSession( void *handle ) ;

/*
* Function:		�����Ự�������д����̣߳�ʹ��selectģ��
				�������¼�����ʱ�������е���recvcb��closecb,idlecb�ӿ�
				�ýӿ�ֻ���Ÿ�tcplistenerʹ�ã��ⲿ��ֹʹ��

* Called By:
* Input:

	@sessionHandle :	�Ự���
	@userData :			ҵ���ʹ������

* Output:
* Return:			true:�����ɹ�, false:����ʧ��
* Others:
*/
bool StartTcpSessionByThread( void *sessionHandle , int userData ) ;


bool OnTcpReceive( ZMD_SESSION_INFO *pSession  ) ;
void OnTcpClose( ZMD_SESSION_INFO *sessionHandle ) ;
bool OnTcpIdle( ZMD_SESSION_INFO *sessionHandle ) ;
//-----------------------------------
// �������Ự�����߳�
//------------------------------------
void *tcpSessionThreadProc( void *userData )
{
	TCP_PRINTF(" threadid %d \n", (unsigned)pthread_self());
	
	fd_set readfds;
	fd_set writefds;

    ZMD_SESSION_INFO *pSession = (ZMD_SESSION_INFO *)userData ;
    if( !pSession )
        return 0 ;
#ifdef DM368
	/**
	 * add by mike, 2013-11-21
	 * ��ʹ�ø�TCP�Ự��NVR������Ƶ����ʱ�����һ֡
	 * ���ݽϴ󣬶����绷���ֲ�̫���룬�������ͻ�
	 * ����IPC��֡���Ӵ��ͻ��������Խ�С��������
	 * ��ʱ�䡣
	 */
	TcpSetSendBuff( pSession->session_sock, 100*1024 );
	#endif
    while( pSession->run )
    {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);

        FD_SET( pSession->session_sock , &readfds);
        int maxfd = pSession->session_sock ;

        timeval to;
        to.tv_sec = 0;
        to.tv_usec = 0 ;

        int n = select(maxfd + 1, &readfds, &writefds, NULL, &to);
        if( n > 0 )
        {
            if (FD_ISSET( pSession->session_sock , &readfds ))
            {
                //���������¼�
                if( !OnTcpReceive( pSession ) )
                {
                    //ҵ������ִ����˳�
                    RELEASE_CPU( 20 ) ;
                    break ;
                }
            }
        }
        if( OnTcpIdle( pSession ) )
            RELEASE_CPU( 1 ) ;
    }
    //֪ͨ�ϲ�Ự����
    OnTcpClose( pSession ) ;

    return 0 ;
}

//----------------------------------------------------------
//����һ��tcp�Ự��������ͻ���ͨѶ
// @serverHanle :	���������
// @sock :			�ͻ���sock
// @recvcb :		���ݴﵽ֪ͨ�ص�
// @closecb :		���ӹر�֪ͨ�ص�
//-----------------------------------------------------------
void *CreateTcpSession( void *serverHandle , int sock,
                        onReceiveTcpLibCallback recvcb , onCloseTcpLibCallback closecb , onIdleTcpLibCallback idlecb )
{
    ZMD_SERVER_INFO *pServer = (ZMD_SERVER_INFO *)serverHandle ;
    //printf( "Before Malloc Session Info !\r\n" ) ;
    //����һ���Ự
    ZMD_SESSION_INFO *pSession = new ZMD_SESSION_INFO ;//(ZMD_SESSION_INFO*)DebugMalloc( sizeof( ZMD_SESSION_INFO ) ) ;
    if( !pSession || !pServer )
    {
        return 0 ;
    }
    //printf( "Malloc Session Info !\r\n" ) ;

    //����Ự��Ϣ
    pSession->run = 1 ;
    pSession->server_handle = serverHandle ;
    pSession->session_sock = sock ;
    pSession->recv_buf_cursor = 0 ;
    pSession->recv_cb = recvcb ;
    pSession->close_cb = closecb ;
    pSession->idle_cb = idlecb ;

    return pSession ;
}

void DestroyTcpSession( void *handle )
{
    delete (ZMD_SESSION_INFO *)handle ;
}

bool StartTcpSessionByThread( void *sessionHandle , int userData )
{
    ZMD_SESSION_INFO *pSession = (ZMD_SESSION_INFO *)sessionHandle ;
    if( !pSession )
        return false ;

    pSession->userData = userData ;

    //printf( "StartTcpSessionByThread !!\r\n" ) ;
    //�������ݴ����߳�
    pSession->session_thread_handle = CreateZmdThread( pSession , tcpSessionThreadProc ) ;

    //printf( "StartTcpSessionByThread successfully!!\r\n" ) ;

    if( !pSession->session_thread_handle )
    {
        //DebugFree( pSession ) ;
        return false ;
    }

    return true ;
}

//------------------------------------------------------
// ֪ͨ����TCP����
// @pSession :		�Ự���
//------------------------------------------------------
bool OnTcpReceive( ZMD_SESSION_INFO *pSession  )
{
    //���buf

    if( pSession->recv_buf_cursor >= TCP_RECV_BUF_SIZE )
    {
        ERR_TCP("error!!! recv_buf_cursor >= TCP_RECV_BUF_SIZE !error , reset session!\r\n" ) ;
        return false ;
    }

    //�������ջ�����
    char *recvBuf = pSession->recv_buf + pSession->recv_buf_cursor ;
    int buflen = TCP_RECV_BUF_SIZE - pSession->recv_buf_cursor ;
    int ret = recv( pSession->session_sock , recvBuf , buflen , 0) ;

    if( ret > 0)
    {
        pSession->recv_buf_cursor += ret ;
        if( pSession->recv_buf_cursor > TCP_RECV_BUF_SIZE )
        {
            ERR_TCP( "error!! recvbufcursor > TCP_RECV_BUF_SIZE !!\r\n" ) ;
            return false ;
        }

        int used = 0 ;
        //----------------------------------------------
        //�����ύ�ϲ㴦�����뷵�ز������ʹ�õ��ֽ���
        //����Ὣδʹ�����ݱ�������������������
        if( !pSession->recv_cb( pSession->userData , pSession->recv_buf , pSession->recv_buf_cursor , used ) )
        {
            return false ;
        }

        //���used.
        if( used < 0 || used >= TCP_RECV_BUF_SIZE )
        {
            ERR_TCP( "tcpclient used error %d !!!\r\n" , used ) ;
            return false ;
        }

        pSession->recv_buf_cursor -= used ;

        //��δ����������ݱ���������������������ֱ���ճ�һ����ɰ�
        if( pSession->recv_buf_cursor > 0 )
        {
            memmove( pSession->recv_buf , pSession->recv_buf + used , pSession->recv_buf_cursor ) ;
        }
    }
    else
    {

#ifdef _WIN32
        uint32 ercode = GetLastError() ;
        if( MYEWOULDBLOCK != ercode )
        {
			printf( "(%d)recv error sock = %d ercode = %d !!\r\n" , __LINE__,pSession->session_sock , ercode ) ;
#else
        if( MYEWOULDBLOCK != errno )
        {
			//printf( "(%d)recv error sock = %d ercode = %d !!\r\n" ,__LINE__, pSession->session_sock , errno ) ;
#endif
            return false ;
        }
    }

    return true ;
}


int TcpBlockRecv( void *sessionHandle , char *buf , int buflen , int timeout )
{
    ZMD_SESSION_INFO *pSession = (ZMD_SESSION_INFO *)sessionHandle ;
    if( !pSession )
        return 0 ;

    int nLeft = buflen ;
    int nRecved = 0 ;
    int nRet = 0 ;

    TCP_PRINTF( "TcpBlockRecv()!!!! buflen = %d , timeout=%d\r\n" , buflen , timeout) ;

    fd_set readfds;
    while( nLeft > 0 )
    {
        FD_ZERO(&readfds);

        FD_SET( pSession->session_sock , &readfds);
        int maxfd = pSession->session_sock + 1;

        timeval to;
        to.tv_sec = timeout / 1000 ;
        to.tv_usec = (timeout % 1000) * 1000 ;

        int n = select( maxfd , &readfds, NULL , NULL, &to);
        //timeout,ֱ�ӷ����ѽ��յ���������
        if( n <= 0)
        {
			perror("##### select\r\n");
            return nRecved ;
        }

        //����ֻ��һ��fd��fdset����ж�FD_ISSET��
        nRet = recv( pSession->session_sock , buf + nRecved , nLeft , 0 ) ;

        //���ճ���
        if( nRet <= 0 )
        {
            ERR_TCP( "recv failed!!!\r\n" ) ;
            return nRecved ;
        }

        nRecved += nRet ;
        nLeft -= nRet ;
    }

    TCP_PRINTF( "recv complete nLeft = %d , nRecved = %d \r\n" , nLeft , nRecved ) ;

    return nRecved ;
}

int GetSessionSock(void *sessionHandle)
{
	ZMD_SESSION_INFO *pSession = (ZMD_SESSION_INFO *)sessionHandle ;
	return pSession->session_sock;
}

//---------------------------------------------------------
// ��������һ������
// @sessionHandle : �Ự���
// @data : ����ָ��
// @datalen : ���ݳ���
//--------------------------------------------------
int TcpBlockSend( void *sessionHandle , char *data , int datalen , int timeout )						
{
    ZMD_SESSION_INFO *pSession = (ZMD_SESSION_INFO *)sessionHandle ;
    if( !pSession ) 										
    {
        ERR_TCP("TcpBlockSend() pSession is NULL!!\r\n" ) ;
        return 0 ;
	}
	
    //������֤ÿ�����ݵķ��͵�������
    CAutoMutex atlck( &(pSession->send_mutex ));

    int nLeft = datalen ;
    int nSend = 0 ;
    char *pCursor = data ;

    while( nLeft )
    {
		
		fd_set writefds;

        FD_ZERO(&writefds);

        FD_SET( pSession->session_sock , &writefds);
        int maxfd = pSession->session_sock ;

        timeval to;
        to.tv_sec = timeout;
        to.tv_usec = 0 ;

        int n = select(maxfd + 1, NULL, &writefds, NULL, &to);
        if( n > 0 )
        {
            if (FD_ISSET( pSession->session_sock , &writefds ))
            {
                nSend = ::send( pSession->session_sock , (char *)pCursor , nLeft , 0 ) ;
                //printf("send ret = %d\r\n" , nSend ) ;
            }
        }

        //nSend = ::send( pSession->session_sock , (char*)pCursor , nLeft , 0 ) ;

        if( nSend <= 0 )
        {

            /*
            #ifdef _WIN32
            			uint32 ercode = GetLastError() ;
            			if( MYEWOULDBLOCK != ercode ){
            #else
            			if( errno !=  MYEWOULDBLOCK ){
            #endif
            	*/
            //socket���� �˳�
            ERR_TCP( "TcpBlockSend send failed!!!!! errcode = %d\r\n" , errno ) ;
            pSession->run = false ;
            return 0 ;
//			}
//			continue ;
        }
        nLeft -= nSend ;
        pCursor += nSend ;
    }

    return (datalen - nLeft) ;
}

//---------------------------------------
//֪ͨ�ϲ㣬�Ự�ر�
void OnTcpClose( ZMD_SESSION_INFO *sessionHandle )
{
    if( !sessionHandle )
        return ;

    sessionHandle->close_cb( sessionHandle->userData ) ;
    void *sessionThread = sessionHandle->session_thread_handle ;

    //�رջỰsock
    if( -1 != sessionHandle->session_sock )
    {
        mysocketclose( sessionHandle->session_sock ) ;
        //printf( "close socket %d \r\n" , sessionHandle->session_sock ) ;
        sessionHandle->session_sock = -1 ;
    }

    //�ͷŻỰ
    //DebugFree( sessionHandle ) ;
    delete sessionHandle ;

    return ;

}

bool OnTcpIdle( ZMD_SESSION_INFO *sessionHandle )
{
    if( !sessionHandle )
        return true ;

    return sessionHandle->idle_cb( sessionHandle->userData ) ;
}

//-----------------------------------
// �º͹رջỰ
void StopTcpSession( void *sessionHandle )
{
    ZMD_SESSION_INFO *pSession = (ZMD_SESSION_INFO *)sessionHandle ;
    if( !pSession )
        return ;

    pSession->run = 0 ;
}

//-------------------------------------
//ǿ���ֶΣ�������ֹ�Ự
void TerminateTcpSession( void *sessionHandle )
{
    ZMD_SESSION_INFO *pSession = (ZMD_SESSION_INFO *)sessionHandle ;
    if( !pSession )
        return ;

    pSession->run = 1 ;

    void *sessionThread = pSession->session_thread_handle ;
    //�رվ��
    if( -1 != pSession->session_sock )
    {
        mysocketclose( pSession->session_sock ) ;
        //printf( "close socket %d \r\n" , pSession->session_sock ) ;
        pSession->session_sock = -1 ;
    }

    //�ͷŻỰ����
    //DebugFree( pSession ) ;
    delete pSession ;

    //���ⲿ��ֹ�߳�
    if( sessionThread )
    {
        DestroyThread( sessionThread ) ;
    }
}

void *listenThreadProc( void *obj )
{
	ZMD_SERVER_INFO* pInfo = (ZMD_SERVER_INFO*)obj ;
	TCP_PRINTF(" threadid %d \r\n", (unsigned)pthread_self());

    while( true )
    {
        struct sockaddr_in clientaddr ;
#ifdef _WIN32
        int clilen = sizeof( clientaddr ) ;
#else
        socklen_t clilen = sizeof( clientaddr ) ;
#endif

        int connectfd = accept( pInfo->server_sock , (struct sockaddr *)&clientaddr , &clilen ) ;
        if( connectfd < 0 )
        {
#ifdef _WIN32
            printf( "accept error!\r\n" ) ;
#else
            ERR_TCP( "accept error! errno = %d \r\n" , errno ) ;
#endif
            RELEASE_CPU( 20 ) ;
            continue ;
        }

        //printf( "accept session!!\r\n" ) ;
        //����accept������
        int userData = -1 ;

#ifndef _WIN32
        //int set = 1;
        //setsockopt( connectfd , SOL_SOCKET, SO_NOSIGPIPE, (void*)&set, sizeof(int));
#endif

        void *session = CreateTcpSession( pInfo , connectfd , pInfo->onrecv_callback , pInfo->onclose_callback , pInfo->onidle_callback )  ;

        //printf( "CreateTcpSession!!\r\n" ) ;

        if( pInfo->accept_callback( userData , session , &clientaddr ) )
        {
            StartTcpSessionByThread( session , userData ) ;
        }
        else
        {
            //delete session ;
            DestroyTcpSession( session ) ;
            mysocketclose( connectfd ) ;

        }


    }
    return 0 ;
}

// ����Tcp������,
// ���ط��������.
// 0Ϊ����ʧ��
void *CreateTcpServer( onAcceptTcpLibCallback acceptcb , onCloseTcpLibCallback onclosecb ,
                       onReceiveTcpLibCallback onrecvcb , onIdleTcpLibCallback idlecb)
{
    ZMD_SERVER_INFO *pInfo = (ZMD_SERVER_INFO *)DebugMalloc( sizeof( ZMD_SERVER_INFO ) ) ;
    if( !pInfo )
        return 0 ;

    pInfo->server_handle = (void *)pInfo ;
    pInfo->accept_callback = acceptcb ;
    pInfo->onclose_callback = onclosecb ;
    pInfo->onrecv_callback = onrecvcb ;
    pInfo->onidle_callback = idlecb ;

    return pInfo->server_handle ;
}

void DestroyTcpServer( int serverHandle )
{

    return ;
}

void StopTcpServer( int serverHandle )
{

}

//-----------------------------------------------
// ����������
// @serverHandle : ���������
// @addr [in] : ������������ַ
// @port [out] : �������˿�
// @isBlock : �Ƿ���Ҫ�ظ����԰󶨣�ֱ���ɹ�
//-----------------------------------------------
bool StartTcpServer( void *serverHandle , char *addr , unsigned short port , bool isBlock )
{
    ZMD_SERVER_INFO *pInfo = (ZMD_SERVER_INFO *)serverHandle ;
    if( !pInfo )
        return false ;

    int listensock = socket( AF_INET , SOCK_STREAM , 0 ) ;

    //�������ʧ��
    if( -1 == listensock )
    {
        ERR_TCP( "socket() failed !\r\n" ) ;
        return false ;
    }

    //���ÿ�����
    int on = 1;
    if ( setsockopt( listensock, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on) ) < 0 )
    {
        ERR_TCP( "setsockopt SO_REUSEADDR failed !\r\n" ) ;
    }

    //fork ��ʱ��, �ӽ��̲��̳�����������
    int flags = fcntl(listensock, F_GETFD); 
    flags |= FD_CLOEXEC; 
    fcntl(listensock, F_SETFD, flags);

    while( true )
    {
        struct sockaddr_in bindaddr ;
        bindaddr.sin_family = AF_INET ;
        bindaddr.sin_port = htons( port ) ;
#ifndef _WIN32
        inet_aton( addr , &(bindaddr.sin_addr ) ) ;
#else
        bindaddr.sin_addr.S_un.S_addr = inet_addr( addr ) ;
#endif

        if( bind( listensock , (struct sockaddr *)&bindaddr , sizeof( bindaddr ) ))
        {
            ERR_TCP( "bind error: %s:%d ercode=%d sock=%d\r\n" , addr , port , errno , listensock ) ;

            //����Ҫ�ظ����ԣ�ֱ�ӷ���ʧ��
            if( !isBlock )
            {
				mysocketclose( listensock ) ;
				return false ;
			}
			RELEASE_CPU( 5000 ) ;
            continue ;
        }
        break ;
    }

    //��ʼ����
    if( listen( listensock , 5 ) )
    {
		perror( "listen error : " ) ;
		mysocketclose( listensock ) ;
		return false ;
	}

    //��������ľ��
    pInfo->server_sock = listensock ;

    //���������߳�
    pInfo->accept_thread_handle = CreateZmdThread( pInfo , listenThreadProc ) ;

    return true ;
}

#ifdef DM368
//����TCP ���ͻ�������С
static void TcpSetSendBuff( int fd, int len )
{
	int retOpt;
	int nSndBuf = len;
	
	if( fd <= 0 || len <= 0 )
		return;

	retOpt = setsockopt( fd, SOL_SOCKET, SO_SNDBUF, (const char *) &nSndBuf, sizeof(int) );
	if( retOpt )
	{
		return;
	}
	
	//printf("TcpSetSendBuff: set send buff %d success!\n", len);
}

#endif


