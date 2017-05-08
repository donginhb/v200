

#ifndef _JIANGHM_MEDIA_SESSION_MANAGER_HEADER_32439876432234
#define _JIANGHM_MEDIA_SESSION_MANAGER_HEADER_32439876432234

#include "tcplibdef.h"
#include "coreobj.h"

#define GetMediaMgr		CMediaSessionMgr::getInstance

typedef struct
{
    int		used[MAX_MEDIA_PLAYER] ;
} STRUCT_MEDIA_SESSION ;

class CMediaSessionMgr
{
protected:
    CMediaSessionMgr() ;
    ~CMediaSessionMgr() ;

public:

    DECLARE_SINGLEOBJ( CMediaSessionMgr ) ;

    //��ȡ���е�ý��Ự
    int							getUnuseMediaSession( int channel , int type ) ;
    //�ͷ�ý��Ự
    void						freeMediaSession( int channel , int type , int mediaid ) ;

protected:

    int							getUnuseVgaSession( int channel ) ;
    int							getUnuseQvgaSession( int channel ) ;
    int							getUnuse720PSession( int channel ) ;

    int							getUnuseRecordSession( int channel ) ;


    void						freeVgaSession( int channel , int mediaid ) ;
    void						freeQvgaSession( int channel , int mediaid ) ;
    void						free720PSession( int channel , int mediaid ) ;

    void						freeRecordSession( int channel , int mediaid ) ;

protected:

    //=========================================
    //�Ƿֱ�Ϊ720p,vga,qvga����3���Ự����
    STRUCT_MEDIA_SESSION		m_vgaSessionList[IPC_CLIENT_COUNT] ;
    CMutex						m_vgaSessionListMutex ;

    //=========================================
    STRUCT_MEDIA_SESSION		m_qvgaSessionList[IPC_CLIENT_COUNT] ;
    CMutex						m_qvgaSessionListMutex ;

    //=========================================
    STRUCT_MEDIA_SESSION		m_p720SessionList[IPC_CLIENT_COUNT] ;
    CMutex						m_p720SessionListMutex ;

    //=========================================
    //¼�񲥷ź����ض���
    STRUCT_MEDIA_SESSION		m_recordSessionList[IPC_CLIENT_COUNT] ;
    CMutex						m_recordSessionListMutex ;

    //����Ŀǰ�����ý��ͨ������
    int							m_mediaChnCount ;
    CMutex						m_mediaChntMutex ;

};


#endif






