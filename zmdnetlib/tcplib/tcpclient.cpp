
//+---------------------------------------------------------------------------
//
//
//
//  File:   	tcpclient.cpp
//
//  Author:		jianghm
//
//  Contents:   tcp�ͻ��˹��ܷ�װ
//
//  Notes:
//
//  Version:	1.0
//
//  Date:		2012-12-13
//
//  History:
// 			jianghm	2012-12-13	1.0		�����ļ�
//			jianghm 2012-12-20	1.1		modify
//	@�޸Ľӿڣ�����ResetClient�ӿ�
//	@����tcpclient�Ͽ�������û������recv_buf_cursor.��ɽ������ݲ���ȷ��bug.
//----------------------------------------------------------------------------

#include "tcplibdef.h"
#include "tcplib.h"
#include "coreobj.h"

typedef struct
{
    int					client_sock ;
    struct sockaddr_in	remote_addr ;			//Զ�̵�ַ
    int					connected ;				//socket���
    int					userData ;				//����֪ͨ�ϲ�ص�ʱ���û�����
    int					isReconnect ;

    void				*client_thread_handle ;	//�ͻ��߳̾��

    int					recv_buf_cursor ;		//���ջ�������
    int					run ;					//���б�־
    CMutex				send_mutex ;			//���������������ݰ������ͻ�����

    onConnectTcpLibCallback		connect_callback ;		//����֪ͨ
    onCloseTcpLibCallback		close_callback;			//��������֪ͨ
    onReceiveTcpLibCallback		receive_callback ;		//�ر��¼�֪ͨ�ص�
    onIdleTcpLibCallback		idle_callback ;			//����֪ͨ

    char				recv_buf[TCP_RECV_BUF_SIZE] ;	//tcp���ջ�����

} ZMD_CLIENT_INFO ;

bool ClientConnect( ZMD_CLIENT_INFO *pClient ) ;
bool SetNoneblocking( int sock ) ;
bool OnClientReceive( ZMD_CLIENT_INFO *pClient  ) ;
void OnClientClose( ZMD_CLIENT_INFO *clientHandle ) ;

bool SelectRecv( int sock )
{
    fd_set readfds;

    FD_ZERO(&readfds);

    FD_SET( sock , &readfds );
    int maxfd = sock + 1;

    timeval to;
    to.tv_sec = 0;
    to.tv_usec = 0 ;

    int n = select(maxfd , &readfds, NULL, NULL, &to);

    if( n > 0 )
    {
        if (FD_ISSET( sock , &readfds ))
        {
            return true ;
        }
    }
    return false ;
}

//------------------------------------
// �ͻ��˻Ự�����߳�
//------------------------------------
void *tcpClientThreadProc( void *userData )
{
	TCP_PRINTF("threadid %d \r\n", (unsigned)pthread_self());
	
    ZMD_CLIENT_INFO *pClient = (ZMD_CLIENT_INFO *)userData ;
    if( !pClient )
        return 0 ;

    //--------------------------
    //�߳��л�һֱ��������Զ�̵�ַ
    //�����ӳɹ������ӦOnConnect�¼�
    while( pClient->run )
    {
        if( !pClient->connected )
        {
            //���ӶϿ���״̬���ͳ�������
            if( !ClientConnect( pClient ) )
            {
                unsigned char *remoteip = (unsigned char *) & (pClient->remote_addr.sin_addr) ;

                //printf( "connect failed.%d.%d.%d.%d..retry after 5 seconds .\r\n" , remoteip[0] , remoteip[1] , remoteip[2] , remoteip[3] ) ;
                //�Ƿ���Ҫ����
                if( pClient->isReconnect )
                {
                    RELEASE_CPU( 3000 ) ;
                    continue ;
                }
                else
                {
                    break ;
                }
            }
        }
        else
        {

            if( SelectRecv( pClient->client_sock ) )
            {
                //�Ѿ�������������
                if( !OnClientReceive( pClient ) )
                {
                    //���ݴ������֪ͨҵ���ر�
                    OnClientClose( pClient ) ;
                    pClient->connected = 0 ;

                    //׼������
                    unsigned char *remoteip = (unsigned char *) & (pClient->remote_addr.sin_addr) ;
                    //printf( "disconnect %d.%d.%d.%d..retry after 3 seconds .\r\n" , remoteip[0] , remoteip[1] , remoteip[2] , remoteip[3] ) ;

                    //�Ƿ���Ҫ����
                    if( pClient->isReconnect )
                    {
                        RELEASE_CPU( 3000 ) ;
                        continue ;
                    }
                    else
                    {
                        break ;
                    }
                }
            }


            pClient->idle_callback( pClient->userData ) ;
            RELEASE_CPU( 20 ) ;
        }
    }

    //�رջỰsock���
    if( -1 != pClient->client_sock )
    {
        mysocketclose( pClient->client_sock ) ;
        //printf( "client close socket %d \r\n" , pClient->client_sock ) ;
        pClient->client_sock = -1 ;
    }

    TCP_PRINTF( "*********** Client Session stop!!***********\r\n" ) ;

    //���ٿͻ��˶���
    delete pClient ;

    return 0 ;
}

bool ClientConnect( ZMD_CLIENT_INFO *pClient )
{
    if( !pClient )
    {
        return false ;
    }

    //�ر�ǰ�ھ��
    if( -1 != pClient->client_sock )
    {
        mysocketclose( pClient->client_sock ) ;
        pClient->client_sock = -1 ;
    }

    //�������
    pClient->client_sock = socket( AF_INET , SOCK_STREAM , 0 ) ;

    if( -1 == pClient->client_sock )
    {
        return false ;
    }

    pClient->connected = 0 ;

    unsigned char *remoteip = (unsigned char *) & (pClient->remote_addr.sin_addr) ;

    //printf( "connect %d.%d.%d.%d...\r\n" , remoteip[0] , remoteip[1] , remoteip[2] , remoteip[3] ) ;


    //��������
    if (::connect( pClient->client_sock , (sockaddr *) &pClient->remote_addr, sizeof(sockaddr)) != 0)
    {
        mysocketclose( pClient->client_sock ) ;
        pClient->client_sock = -1 ;
        pClient->connect_callback( pClient->userData , pClient->connected );
        return false ;
    }

    pClient->connected = 1 ;
    pClient->recv_buf_cursor = 0 ;
    memset( pClient->recv_buf , 0 , sizeof( pClient->recv_buf ) ) ;
    //SetNoneblocking( pClient->client_sock ) ;
    pClient->connect_callback( pClient->userData , pClient->connected );

    return true ;
}

bool SetNoneblocking( int sock )
{
#ifdef _WIN32
    unsigned long ul = 1;
    if( ioctlsocket(sock, FIONBIO, &ul) == SOCKET_ERROR )
    {
        return false ;
    }

    //�������ӵ��������
    /*
    tcp_keepalive TcpLive = { 0 };
    tcp_keepalive OutTcpLive = { 0 };
    DWORD dwBytes ;

    TcpLive.onoff = 1;
    TcpLive.keepalivetime = 15000;
    TcpLive.keepaliveinterval = 3000;
    if (SOCKET_ERROR == ::WSAIoctl(	sock,
    	SIO_KEEPALIVE_VALS,
    	&TcpLive,
    	sizeof(struct tcp_keepalive),
    	&OutTcpLive,
    	sizeof(struct tcp_keepalive),
    	&dwBytes,
    	NULL,
    	NULL ))
    {
    	return true ;
    }
    */

    return true ;
#else

    /*
    int keepAlive = 1;		// Open keepalive
    int keepIdle = 15;		// ���������30����û���κ���������,�����̽��
    int keepInterval = 5; // ̽��ʱ������ʱ����Ϊ5 ��
    int keepCount = 3; // ̽�Ⳣ�ԵĴ���.�����1��̽������յ���Ӧ��,���2�εĲ��ٷ�.

    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
    setsockopt(sock, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
    setsockopt(sock, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
    setsockopt(sock, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
    */

    int ops ;
    ops = fcntl( sock , F_GETFL ) ;
    if( ops < 0 )
    {
        return false ;
    }

    ops |= O_NONBLOCK ;
    if( fcntl( sock , F_SETFL , ops ) < 0 )
    {
        return false ;
    }

    return true ;
#endif
}

//----------------------------------------------------------
//����һ��tcp�Ự��������ͻ���ͨѶ
// @serverHanle :	���������
// @sock :			�ͻ���sock
// @recvcb :		���ݴﵽ֪ͨ�ص�
// @closecb :		���ӹر�֪ͨ�ص�
//-----------------------------------------------------------
void *CreateClient(struct sockaddr_in *addr , int userData , int isReconnect ,
                   onConnectTcpLibCallback connect_cb , onCloseTcpLibCallback close_cb ,
                   onReceiveTcpLibCallback receive_cb , onIdleTcpLibCallback idle_cb )
{
    //����һ���ͻ���
    ZMD_CLIENT_INFO *pClient = new ZMD_CLIENT_INFO ; //(ZMD_CLIENT_INFO*)DebugMalloc( sizeof( ZMD_CLIENT_INFO ) ) ;
    if( !pClient )
    {
        return 0 ;
    }

    //����Ự��Ϣ
    //���ӵ�ַ
    memcpy( &pClient->remote_addr , addr , sizeof( struct sockaddr_in ) ) ;

    //�߳�����״̬
    pClient->run = 1 ;
    pClient->connected = 0 ;
    pClient->client_sock = -1 ;
    pClient->recv_buf_cursor = 0 ;
    pClient->userData = userData ;

    //���ûص�
    pClient->connect_callback = connect_cb ;
    pClient->close_callback = close_cb ;
    pClient->receive_callback = receive_cb ;
    pClient->idle_callback = idle_cb ;

    pClient->isReconnect = isReconnect ;

    //�������ݴ����߳�
    pClient->client_thread_handle = CreateZmdThread( pClient , tcpClientThreadProc ) ;
    if( !pClient->client_thread_handle )
    {
        DebugFree( pClient ) ;
        return 0 ;
    }

    return pClient ;

}

//------------------------------------------------------
// ����socket������ж��¼��󣬽���TCP����
// @pSession :		�Ự���
//------------------------------------------------------
bool OnClientReceive( ZMD_CLIENT_INFO *pClient  )
{
    //���buf
    if( pClient->recv_buf_cursor >= TCP_RECV_BUF_SIZE )
    {
        ERR_TCP("%d!!! recv_buf_cursor >= TCP_RECV_BUF_SIZE !error , reset session!\r\n" , pClient->recv_buf_cursor ) ;
        return false ;
    }

    //�������ʣ�����ݣ��������ջ�����
    char *recvBuf = pClient->recv_buf + pClient->recv_buf_cursor ;
    int buflen = TCP_RECV_BUF_SIZE - pClient->recv_buf_cursor ;
    int ret = recv( pClient->client_sock , recvBuf , buflen , 0) ;

    if( ret > 0)
    {
        pClient->recv_buf_cursor += ret ;
        if( pClient->recv_buf_cursor > TCP_RECV_BUF_SIZE )
        {
            ERR_TCP( "error!! recvbufcursor > TCP_RECV_BUF_SIZE !!\r\n" ) ;
            return false ;
        }

        int used = 0 ;
        //----------------------------------------------
        //�����ύ�ϲ㴦�����뷵�ز������ʹ�õ��ֽ���
        //����Ὣδʹ�����ݱ�������������������
        if( !pClient->receive_callback( pClient->userData , pClient->recv_buf , pClient->recv_buf_cursor , used ) )
        {
            return false ;
        }

        //���used.
        if( used < 0 || used >= TCP_RECV_BUF_SIZE )
        {
            ERR_TCP( "tcpclient used error %d !!!\r\n" , used ) ;
            return false ;
        }

        pClient->recv_buf_cursor -= used ;

        //��δ����������ݱ���������������������ֱ���ճ�һ����ɰ�
        if( pClient->recv_buf_cursor > 0 )
        {
            memmove( pClient->recv_buf , pClient->recv_buf + used , pClient->recv_buf_cursor ) ;
        }
    }
    else
    {

#ifdef _WIN32
        uint32 ercode = GetLastError() ;
        if( MYEWOULDBLOCK != ercode )
#else
        if( MYEWOULDBLOCK != errno )
#endif
        {
            //printf( "recv error sock = %d ercode = %d !!\r\n" , pClient->client_sock , errno ) ;
            //printf( "recv error sock = %d ercode = %d !!\r\n" , pClient->client_sock , ercode ) ;
            return false ;
        }
    }

    return true ;
}

bool SelectSend( int sock )
{
    fd_set writefds;

    FD_ZERO(&writefds);

    FD_SET( sock , &writefds );
    int maxfd = sock + 1;

    timeval to;
    to.tv_sec = 5;
    to.tv_usec = 0 ;

    int n = select(maxfd , NULL, &writefds, NULL, &to);

    if( n > 0 )
    {
        if (FD_ISSET( sock , &writefds ))
        {
            return true ;
        }
    }
    return false ;
}

//---------------------------------------------------------
// ��������һ������
// @sessionHandle : �Ự���
// @data : ����ָ��
// @datalen : ���ݳ���
//--------------------------------------------------
int ClientBlockSend( void *clientHandle , char *data , int datalen )
{
    ZMD_CLIENT_INFO *pClient = (ZMD_CLIENT_INFO *)clientHandle ;
    if( !pClient )
        return 0 ;

    //������֤ÿ�����ݰ��ķ��͵�������
    CAutoMutex atlck( &(pClient->send_mutex ));

    int nLeft = datalen ;
    int nSend = 0 ;
    char *pCursor = data ;

    while( nLeft && pClient->run )
    {
        //��ѯ�Ƿ��д
        if( !SelectSend( pClient->client_sock ) )
            return 0 ;

        nSend = ::send( pClient->client_sock , (char *)pCursor , nLeft , 0 ) ;
        if( nSend <= 0 )
        {
#ifdef _WIN32
            uint32 ercode = GetLastError() ;
            if( MYEWOULDBLOCK != ercode )
#else
            if( errno !=  MYEWOULDBLOCK )
#endif
            {
                //socket���� �������ӱ�ʶ�����ش���
                pClient->connected = 0 ;
                return 0 ;
            }
            continue ;
        }
        nLeft -= nSend ;
        pCursor += nSend ;
    }

    return (datalen - nLeft) ;
}

//--------------------------------------------
// ������������
// ����ָ����Сbuf�����ݣ�����������߳��ִ���Ż᷵��
bool ClientBlockRecv( void *clientHandle , char *buf , int *len )
{
    ZMD_CLIENT_INFO *pClient = (ZMD_CLIENT_INFO *)clientHandle ;
    if( !pClient )
        return 0 ;

    //�����С
    int nBufLen = *len ;
    int nRecv = 0 ;
    int nLeft = nBufLen ;

    while( nLeft > 0 )
    {
        int nRet = recv( pClient->client_sock , buf + nRecv , nLeft , 0 ) ;
        if( 0 == nRet )
        {
            //�����ѹر�,����ʧ��,��������ʧ��
            pClient->connected = 0 ;
            return false ;
        }

        if( nRet < 0 )
        {
#ifdef _WIN32
            uint32 ercode = GetLastError() ;
            if( MYEWOULDBLOCK != ercode )
#else
            if( errno !=  MYEWOULDBLOCK )
#endif
            {
                //socket���� �˳�����������ʧ��
                pClient->connected = 0 ;
                return false ;
            }
        }

        nRecv += nRet ;
        nLeft -= nRet ;
    }
    return true ;
}

//---------------------------------------
//֪ͨ�ϲ㣬�Ự�ر�
void OnClientClose( ZMD_CLIENT_INFO *clientHandle )
{
    if( !clientHandle )
        return ;

    //�رվ��
    if( -1 != clientHandle->client_sock )
    {
        mysocketclose( clientHandle->client_sock ) ;
        clientHandle->client_sock = -1 ;
    }

    clientHandle->recv_buf_cursor = 0 ;

    clientHandle->close_callback( clientHandle->userData  ) ;
    return ;
}

//-----------------------------------
// �º͹رջỰ
void StopClient( void *clientHandle )
{
    ZMD_CLIENT_INFO *pClient = (ZMD_CLIENT_INFO *)clientHandle ;
    if( !pClient )
        return ;

    pClient->run = 0 ;
}

//-------------------------------------
//ǿ���ֶΣ�������ֹ�Ự
void TerminateClient( void *clientHandle )
{
    ZMD_CLIENT_INFO *pClient = (ZMD_CLIENT_INFO *)clientHandle ;
    if( !pClient )
        return ;

    pClient->run = 1 ;

    void *sessionThread = pClient->client_thread_handle ;

    //�رվ��
    if( -1 != pClient->client_sock )
    {
        mysocketclose( pClient->client_sock ) ;
        TCP_PRINTF( "close socket %d \r\n" , pClient->client_sock ) ;
        pClient->client_sock = -1 ;
    }

    //�ͷŻỰ����
    //DebugFree( pClient ) ;
    delete pClient ;

    //���ⲿ��ֹ�߳�
    if( sessionThread )
    {
        DestroyThread( sessionThread ) ;
    }
}

void ResetClient( void *clientHandle , struct sockaddr_in *addr )
{
    ZMD_CLIENT_INFO *pClient = (ZMD_CLIENT_INFO *)clientHandle ;

    if( !pClient )
        return ;

    if( addr )
    {
        memcpy( &pClient->remote_addr , addr , sizeof( struct sockaddr_in ) ) ;
    }

    OnClientClose( pClient ) ;
    pClient->connected = 0 ;

}



