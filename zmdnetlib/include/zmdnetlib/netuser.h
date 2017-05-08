

#ifndef _JIANGHM_NET_USER_HEADER_2343298u7324423
#define _JIANGHM_NET_USER_HEADER_2343298u7324423

#include "interfacedef.h"
#include "tcplibdef.h"

#ifdef SUPPORT_WIFI
#include "wificonfig.h"
#endif
#include "zspadapter.h"


#ifdef APP3531
#define MAX_UPDATEFILE_LEN		24*1024*1024
#endif
#ifdef DM368
#define MAX_UPDATEFILE_LEN		16*1024*1024
#endif

#define UPDATEFILE_LEN			1024*1024


#define UPDATEFILE "/tmp/UpdateFile"

#define GET_HEADER_LEN( type )		(sizeof(type) - sizeof( Cmd_Header ) )

/***********************************************/

enum ZmdProtoType
{
	ProtoUnknown = -1,
	ProtoZSP = 0,
	ProtoP2P = 1
};
class CNetUser
{
public:
	CNetUser( ) ;
	~CNetUser( ) ;

public:

	//��ʼ���û������û�����Ϣȫ����ΪĬ��ֵ
	void ResetUser( ) ;
	
	//======================================
	//�û��¼�������
	void onClose() ;
	bool onReceive( char* data , int len , int& used ) ;
	bool onIdle() ;

protected:

	bool onPacketProcesser( char* data , int len ) ;
	
	//---------------------------------------------------------
	//��Ϣ������
	bool onDeviceType( char* data , int len ) ;
	bool onRequestVideoLiving( char* data , int len ) ;

	bool onStopVideoLiving( char* data , int len ) ;

	bool onCmdFormatDisk( char* data , int len ) ;
	
	bool onGetDeviceMac( char* data , int len ) ;
	bool onSetDeviceMac( char* data , int len ) ;

	bool onTalkOn( char* data , int len ) ;
	bool onTalOff( char* data , int len ) ;

	bool onGetTalkSetting( char* data , int len ) ;
	bool onSetTalkSetting( char* data , int len ) ;
	bool onAlarmUpdate( char* data , int len ) ;
	bool onTalkData( char* data , int len ) ;

	bool onReadDevParam( char* data , int len ) ;
	bool onSetDevParam( char* data , int len ) ;
	bool onSetPtz( char* data , int len ) ;

	bool onSetReboot( char* data , int len ) ;

	bool onRequestUpdate( char* data , int len ) ;
	bool onCmdNpt( char* data , int len ) ;

	bool onRequestResotre( char* data , int len ) ;
	bool on3GCtrlCmd( char* data , int len ) ;

	bool on3GRepower( char* data , int len ) ;
	bool onGet3GRepower( char* data , int len ) ;
	bool onGet3GInfo( char* data , int len ) ;

	bool onReadLog( char* data , int len ) ;
	
	bool onReadDevInfo( char* data , int len ) ;
	bool onSearchPlayList( char* data , int len ) ;
	bool searchPlaybackListInDates( char *data , int len );

	bool onPlaybackPlay( char* data , int len ) ;
	bool onPlaybackStop( char* data , int len ) ;

	bool onReqLogin( char* data , int len ) ;
	bool onResetChnAna( char* data , int len ) ;

	bool onSetChnAnaLog( char* data , int len ) ;
	bool onGetChnAnaLog( char* data , int len ) ;

	bool onGetWifiAP( char* data , int len ) ;
	bool onSetWifiConnect( char* data , int len ) ;

	bool onGetWifiStatus( char* data , int len ) ;
	bool onSetPicInfo( char* data , int len ) ;

	bool onSetVgaPicInfo( char* data , int len );
	bool onSet720PPicInfo( char* data , int len ) ;

	bool onNvrPort( char* data , int len ) ;

	bool onReadVideoCoderParam( char* data , int len ) ;

	//===========================================
	//ˮ����������Э��
	bool onGetSnapshot( char* data , int len ) ;
	bool onGetSensorAlarmInfo( char* data , int len ) ;
	bool onSetSensorAlarmInfo( char* data , int len ) ;

	//===========================================
	//Ԥ�õ���Ϣ
	bool onGetPreset( char* data , int len ) ;
	
	//
	bool onSetVideoCode( char* data , int len ) ;

	//==========================================
	//���úͻ�ȡ�豸id
	bool onGetDeviceID( char* data , int len ) ;
	bool onSetDeviceID( char* data , int len ) ;

	bool onSetAudioSwitch( char* data , int len ) ;
	bool onGetAudioParm( char* data , int len );


	bool onSetWifiLED( char* data , int len );
	bool onGetWifiLED( char* data , int len );	
	//=======================================
	//ǿ��I ֡
	//add hayson 2013.8.6
	bool onSetIframe(char* data , int len);
	//��¼�豸�ͺ������ļ� 
	//add hayson 2013.11.26
	bool onSetDevConfig(char* data , int len);

	//add by mike,2013-12-19,���Ӵ���Զ��������ص�����
	//������������Ϣ
	bool onCheckAppUpdateInfo( char* data, int len );
	
	//��ʼ����Զ���Զ�����
	bool onStartRemoteUpdate( char* data, int len );
	
	//��ȡԶ���������е�״̬
	bool onGetRemoteUpdateStat( char* data, int len );
	
	//ȡ��Զ���Զ�����
	bool onCancelRemoteUpdate( char* data, int len );
	
	//��ͣԶ���Զ�����
	bool onPauseRemoteUpdate( char* data, int len );
	
	//�ָ�Զ���Զ�����
	bool onResumeRemoteUpdate( char* data, int len );

	//��ȡʱ���ʽ
	bool onGetDateFormat(char* data, int len);

	//����ʱ���ʽ
	bool onSetDateFormat(char* data, int len);

	//��ȡsd����Ϣ
	bool onGetDiskInfo( char* data, int len);

	//��ȡ�豸�汾��Ϣ
	bool onGetVersion(char* data, int len);
	// ����wifi NVR ����ģʽ
	bool onWifiNVRMode(char *data, int len);

	// ����ipc ����
	bool onChangeStreamRate(char *data, int len);

	bool onGetViRate(char *data, int len);
	
	bool onSetViRate(char *data, int len);
	
	//��ȡ��Ƶ������Կ
	bool onGetVideoKey(char *data, int len);

	//���úͻ�ȡ�ƶ��������
	bool onSetMdRegion(char *data, int len);
	bool onGetMdRegion(char *data, int len);

	//���ת������
	bool onSetMotorTest(char *data, int len);
	//��λ���ò���
	bool onResetTest(char *data, int len);
	//ҹ�����ò���
	bool onNightVisionTest(char *data, int len);

	

protected:
	//======================================
	// �������ڲ����ܺ���

	//ѯ��ϵͳ���Ƿ��ܽ��и��������Ƶֱ��
	int		RequestStartVideoLiving( int command , int channel ) ;
	//������ֱ��
	int		StartVideoLiving( int command , int channel ) ;
	
	//=======================================
	//����ֱ������
	bool	SendLivingData(  ) ;

	//=======================================
	//����¼������
	bool	SendRecordPlay( ) ;

	//=======================================
	//����¼���ļ�
	bool	SendRecordFile( );

	//=======================================
	//����¼������
	bool	SendAudioData( ) ;
	

	//NTP��ʱ
	int netSetTimezone( char* data , int len );
	int netGetTimezone( char* data , int len );

	int		CheckWifiSaveParaMeter(TYPE_WIFI_LOGIN  wifilogin) ;
	
public:

	void*			m_sessionHandle ;
	int				m_mediaCmd ;		//��¼�û�����ý�����ݵ������(CMD_START_VIDEO,CMD_START_SUBVIDEO,CMD_START_720P)
	int				m_Channel ;			//��¼�û������ͨ��id
	int				m_ChannelType ;		//��¼�û��������������( VGA,QVGA,720P)

	//�û�ID,ȫ��Ϊһ���û���ʶ
	int				m_nUserID ;
	//�û������ý��ͨ��id.
	int				m_mediaSid ;
	//�û�����ı���ͨ��id
	int				m_alarmID ;

	//
	int				m_bSendAudio ;


	//ֱ����־
	int				m_bStartSendMedia ;

	//�Խ���־
	int				m_bStartTalk ;

	//�㲥��־.¼�񲥷�
	int				m_bStartSendRecord ;	//0:δ����,1:����,2:����
	PLAYBACK_ITEM	m_RecordItem ;			//
	FILE*			m_RecordFp ;
	unsigned int	m_nSendPos ;				//�ϴη��͵��ļ�λ��
	unsigned int	m_timeTick ;			//��ʱ��

	unsigned int	m_remoteIP ;

	int				m_bUpdateFile ;
	char			m_FrameBuf[TCP_RECV_BUF_SIZE] ;

	int				m_bufType ;

	int				m_ActiveTime ;

	/* ��¼����״��, һ����ͳ��һ�� */
	int				m_NetSpeed; // ��ǰ����
	struct timeval	m_StartCalcTime; // ��ʼ����ʱ��
	int 			m_SendTime; // send �����ۼƺķ�ʱ��
	int				m_BytesSend; // ���͵��ֽ���

	int				m_protoType;//��־��zspЭ�黹�� p2pЭ��, 0 ��zsp 1�� p2p
};

int  Set_NightSwtich(unsigned char status);
#endif

