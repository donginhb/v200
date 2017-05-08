

#ifndef _JIANGHM_ZMD_MOBILE_USER_HEADER_3294878934279342849832
#define _JIANGHM_ZMD_MOBILE_USER_HEADER_3294878934279342849832

#include "tcplibdef.h"
#include "tcplib.h"
#include "owsp_def.h"

#define VGA_FRAME_BUF_SIZE		512*1024

class CMobileUser
{
public:
	CMobileUser() ;
	~CMobileUser() ;

public:

public:

    //��ʼ���û������û�����Ϣȫ����ΪĬ��ֵ
    void ResetUser( ) ;

    //======================================
    //�û��¼�������
    void onClose() ;
    bool onReceive( char *data , int len , int &used ) ;
    bool onIdle() ;

protected:

    bool onPacketProcesser( char *data , int len ) ;
    bool processerTlvPacket( char *data ,  int len ) ;

    //=============================================
    //��Ϣ������

    bool onVersionInfoRequest( char *data , int len ) ;
    bool onPhoneInfoRequest( char *data , int len ) ;
    bool onLoginRequest( char *data , int len ) ;
    bool onChannelRequest( char *data , int len ) ;
    bool onControlRequest( char *data , int len ) ;

    //===============================================
    //��Ӧ����


    //===============================================
    //���ܴ�����
    int	RequestStartVideoLiving( int channel ) ;

    int SendLoginSuccess(  ) ;

    int SendVideoLiving( ) ;

    unsigned int GetSeq( )
    {
        return m_packetSeq ++ ;
    }

    unsigned int GetFrameCount( )
    {
        return m_frameCount ++ ;
    }

public:

    void			*m_sessionHandle ;
    int				m_mediaCmd ;		//��¼�û�����ý�����ݵ������(CMD_START_VIDEO,CMD_START_SUBVIDEO,CMD_START_720P)
    int				m_Channel ;			//��¼�û������ͨ��id
    int				m_ChannelType ;		//��¼�û��������������( VGA,QVGA,720P)

    //�û�ID,ȫ��Ϊһ���û���ʶ
    int				m_nUserID ;
    //�û������ý��ͨ��id.
    int				m_mediaSid ;
    //�û�����ı���ͨ��id
    int				m_alarmID ;


    //ֱ����־
    int				m_bStartSendMedia ;

    unsigned int	m_nSendPos ;				//�ϴη��͵��ļ�λ��
    unsigned int	m_timeTick ;			//��ʱ��

    unsigned int	m_remoteIP ;

    int				m_bUpdateFile ;

    char			m_FrameBuf[VGA_FRAME_BUF_SIZE] ;  

    TLV_V_VersionInfoRequest	m_VersionInfoRequest;

    unsigned int	m_packetSeq ;
    unsigned int	m_frameCount ;


};


#endif

