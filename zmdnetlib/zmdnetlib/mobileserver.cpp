
#include "mobileserver.h"
#include "mobileuser.h"
#include "tcplib.h"

IMPLEMENT_SINGLEOBJ( CMobileServer )

CMobileServer::CMobileServer()
{
    m_nUserCount = 0 ;
}

CMobileServer::~CMobileServer()
{

}

//==============================================
//����������
bool CMobileServer::StartServer( char *addr , unsigned short port )
{
	//��ʼ�������Ự
	int i = 0 ;
    for( i = 0 ; i < MAX_NET_TCP_USER ; i ++ )
    {
        m_loginUserList[i].userobj = new CMobileUser() ;
        m_loginUserList[i].userid = i ;
        m_loginUserList[i].used = 0 ;
        m_loginUserList[i].mediaid = INVALID_USERID ;
        m_loginUserList[i].alarmid = INVALID_USERID ;
    }

    //����һ��tcp������
    m_serverHandle = CreateTcpServer( onServerTcpAccept , onServerTcpClose , onServerRecvData , onServerSessionIdle ) ;
    if( m_serverHandle )
    {
        //������tcp������
        if( StartTcpServer( m_serverHandle , addr , port , true ))
        {
            return true ;
        }
    }
    return false ;
}

bool CMobileServer::onServerTcpAccept( int &userData , void *session , sockaddr_in *client_addr )
{
    //֪ͨ�����ӵ���
    return getInstance()->onAccept( userData , session , client_addr ) ;
}

void CMobileServer::onServerTcpClose( int userData )
{
    //֪ͨ�����ӹر�
    getInstance()->onClose( userData ) ;
}

bool CMobileServer::onServerSessionIdle( int userData )
{
    //֪ͨ�����ӹر�
    return getInstance()->onIdle( userData ) ;
}

//--------------------------------------------
//�Խ��յ������ݽ��зְ�
bool CMobileServer::onServerRecvData( int userData , char *data , int len , int &used )
{
    return getInstance()->onReceive( userData , data , len , used ) ;
}

//=================================================
//��ȡһ�����е��û��ṹ.
int	CMobileServer::getUnuseWorkSession( )
{
    //���빤���Ự��
    CAutoMutex atlck( &m_loginUserListMutex ) ;

    int i = 0 ;
    for( i = 0 ; i < MAX_NET_TCP_USER ; i ++ )
    {
        if( !m_loginUserList[i].used )
        {
            m_loginUserList[i].used = 1 ;
            return i ;
        }
    }

    return INVALID_USERID ;
}

//=============================================
//�ͷ��û��ṹ
void CMobileServer::freeWorkSession( int userid )
{
    if( userid < 0 || userid >= MAX_NET_TCP_USER )
    {
        printf( "freeWorkSession() userid error = %d\r\n" , userid ) ;
        return ;
    }

    CAutoMutex atlck( &m_loginUserListMutex ) ;
    m_loginUserList[userid].used = 0 ;
}

void CMobileServer::onClose( int userData )
{
    //֪ͨ�����ӹر�
    int userid = userData ;

    if( userid < 0 || userid >= MAX_NET_TCP_USER )
        return ;

    m_nUserCount -- ;
    printf( "CMobileServer::onClose usercount = %d \r\n" , m_nUserCount ) ;

    m_loginUserList[userid].userobj->onClose() ;
}

bool CMobileServer::onReceive( int userData , char *data , int len , int &used )
{
    printf( "CMobileServer::onReceive\r\n" ) ;
    int userid = userData ;

    if( userid < 0 || userid >= MAX_NET_TCP_USER )
        return false ;

    return m_loginUserList[userid].userobj->onReceive( data , len , used ) ;
}

bool CMobileServer::onIdle( int userData )
{
    //֪ͨ����
    int userid = userData ;

    if( userid < 0 || userid >= MAX_NET_TCP_USER )
        return true ;

    return m_loginUserList[userid].userobj->onIdle() ;
}

bool CMobileServer::onAccept( int &userData , void *session , sockaddr_in *client_addr )
{
    //֪ͨ�����ӵ���
    //��ȡ�����Ự(�����Ự���������.)
    int userid = getUnuseWorkSession() ;
    if( INVALID_USERID == userid )
    {
        return false ;
    }

    m_nUserCount ++ ;
    printf( "CMobileServer::onAccep usercount = %d \r\n" , m_nUserCount ) ;

    //��ȡ�����ù����Ự�󣬶ԻỰ���г�ʼ��
    //���ڲ��ᱻ�ⲿ�̹߳رգ�����������Է���
    //m_loginUserList[userid].userobj.������Ҫ����

    userData = userid ;
    m_loginUserList[userid].userobj->ResetUser() ;
    m_loginUserList[userid].userobj->m_sessionHandle = session ;
    m_loginUserList[userid].userobj->m_nUserID = userid ;
    m_loginUserList[userid].userobj->m_remoteIP = client_addr->sin_addr.s_addr ;

    return true ;
}


