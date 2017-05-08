
//+---------------------------------------------------------------------------
//
//  File:   	mobileserver.h
//
//  Author:		jianghm
//
//  Contents:   mobileserver������ʵ��
//
//  Notes:
//
//  Version:	1.0
//
//  Date:		2012-12-11
//
//  History:
// 			 jianghm	2012-12-11   1.0	   �����ļ�
//----------------------------------------------------------------------------


#ifndef _JIANGHM_MOBILE_SERVER_HEADER_2349832423423
#define _JIANGHM_MOBILE_SERVER_HEADER_2349832423423

#include "tcplibdef.h"
#include "coreobj.h"


#define GetMobileServerObj		CMobileServer::getInstance

class CMobileUser ;

typedef struct
{
    CMobileUser	*userobj ;		//�����û�,�˴��Ӷ��з������
    int				userid ;		//�����û�id .
    int				mediaid ;		//��¼�û���media�����е�������
    int				alarmid ;		//��¼�û���alarm�����е�������

    int				used ;			//�Ƿ�ʹ��
} STRUCT_MOBILE_USER ;

//////////////////////////////////////////////////////////////////////
//MobileЭ����������ṹ��CNetServer���ơ������ͬ
class CMobileServer
{
    //����������Ϊ������������ֹ�ⲿ��ʵ����
protected:
    CMobileServer() ;
    ~CMobileServer() ;

public:

    DECLARE_SINGLEOBJ( CMobileServer )

public:
    //����server
    bool		StartServer( char *addr , unsigned short port ) ;

    //��ȡ���еĹ����Ự
    int							getUnuseWorkSession( ) ;
    //�ͷŹ����Ự
    void						freeWorkSession( int userid ) ;


protected:

    static bool onServerTcpAccept( int &userData , void *session , sockaddr_in *client_addr ) ;
    static void onServerTcpClose( int userData ) ;
    static bool onServerRecvData( int userData , char *data , int len , int &used ) ;
    static bool onServerSessionIdle( int userData ) ;

    //==============================================
    //�¼�������
    void onClose( int userData ) ;
    bool onReceive( int userData , char *data , int len , int &used ) ;
    bool onIdle( int userData ) ;
    bool onAccept( int &userData , void *session , sockaddr_in *client_addr ) ;

protected:
    // tcp�����Ự����
    STRUCT_MOBILE_USER			m_loginUserList[MAX_NET_TCP_USER] ;
    CMutex						m_loginUserListMutex ;

    void						*m_serverHandle ;

    //���������ӵ�tcp����
    int							m_nUserCount ;
};


#endif


