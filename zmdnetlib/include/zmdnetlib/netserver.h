#ifndef _JIANGHM_NET_TCP_SERVER_HEADER_34324897243342
#define _JIANGHM_NET_TCP_SERVER_HEADER_34324897243342

#include "tcplibdef.h"
#include "coreobj.h"
#include "interfacedef.h"

#define GetNetServerObj		CNetServer::getInstance

/*********************************************/

#define CHAE_IP_LEN			64
#define ZMDNETLIB_BUILD_VERSION "V1.6.1"     

/*********************************************/

//#define HAY_DEBUG

#ifdef HAY_DEBUG
	#define NDB(fmt, args...) fprintf(stderr, "\033[1;32m             NETLIB DEBUG(%s:%d):             \033[0m" fmt, __func__, __LINE__, ## args)

#else
	#define NDB(fmt, args...) 
#endif

#define ERR(fmt, args...) fprintf(stderr, "\033[1;31m             ZMDNETLIB ERROR (%s:%d):             \033[0m" fmt, __func__, __LINE__, ## args)





				


#ifdef _BARCODE_SUP_
#include "zsip_api.h"
#endif

class CNetUser ;

typedef struct
{
    int		userid ;
    int		mediaid ;

    int		used ;
} STRUCT_MEDIA_USER ;

typedef struct
{
    CNetUser		*userobj ;		//�����û�,�˴��Ӷ��з������
    int				userid ;		//�����û�id .
    int				mediaid ;		//��¼�û���media�����е�������
    int				alarmid ;		//��¼�û���alarm�����е�������

    int				used ;			//�Ƿ�ʹ��
} STRUCT_LOGIN_USER ;

typedef struct
{
    int userid ;
    unsigned int ip ;
    unsigned int port ;
    int devType ;
    int	used ;
} STRUCT_ALARM_USER ;

class CNetServer
{
protected:
    CNetServer() ;
    ~CNetServer() ;

    DECLARE_SINGLEOBJ( CNetServer )

public:
    //����net server
    bool						StartServer( char *addr , unsigned short port ) ;

    //��ȡ���еĹ����Ự
    int							getUnuseWorkSession( ) ;
    //�ͷŹ����Ự
    void						freeWorkSession( int userid ) ;

    bool						StartPingServer( ) ;
    int							JudgeNetworkStatus() ;

    void						CheckNetCardStatus( ) ;
	
	//��֤�û�������
	int							VerifyUser( char* user , char* pwd , int& permit ) ;
	int							VerifyUserByMd5( char* user , char* pwd , int& permit , char* userReal ) ;
	/*add by hayson 2014.1.17*/
	/*��֤��Ƶ�˿�*/
	int 						VerifyVideoPort(int port);
	
	int							getUnuseAlarmSession( int userid , unsigned int ip , unsigned int port , int devType ) ;
	void						freeAlarmSession( int alarmid );
	/*��ȡָ����������*/
	/*@gateway_addr  ����*/
	/*@name ָ������*/
	/*@ ����:��ȡ�ɹ�����0 ʧ�ܷ��� < 0*/
	int							GetGw(char *gateway_addr, char *name);
	int 						DelGwAddr(char *gateway_addr, char *name);
	int 						SetGwAddr(char *gateway_addr, char *name);
	//������������
	int 						GetIPaddr(char *net_ip, char *name);
	int 						SetIPaddr(char *net_ip, char *name);
	int 						GetMaskAddr(char *net_mask, char *name);
	int 						SetMaskAddr(char *mask_ip, char *name);
	int 						SetMacAddr(char *addr, char *name);
	
	int							SetNetAttrib( NETWORK_PARA *netset, int net_card_id);
	int 						RunDHCP(char* pNetName);/*��DHCP*/
	int							DownDHCP(char*pNetName);/*�ر�DHCP*/
	void						RegistDDNS( ) ;
	void						RegistUPNP( ) ;
	int  						InitNtp(); /*init ntp*/
	void 						PrintZmdnetlibVer();/*printf zmdnetlib version*/
	void 						DelcurrentGW(); /*ɾ���������� add by hayson*/
	void 						BroadcastDeviceInfo();/*�㲥�豸��Ϣ add by hayson*/
    bool						IsUpdating( )
    {
		return 	(bool)m_isUpdating ;
	}
	
    void						SetUpdating( bool bSet )
    {
		m_isUpdating = bSet ;
	};

	bool						IsTalkOn( ) ;
	bool						RequestTalkOn( unsigned userid );
	bool						RequestTalkOff( unsigned userid ) ;

    int							GetTcpUserCount( )
    {
		return m_nUserCount ;
	};
	bool                        IsUpnpSuccess(){return m_upnpSuccess;}

protected:
	// tcp�����Ự����
	STRUCT_LOGIN_USER			m_loginUserList[MAX_NET_TCP_USER] ;
	CMutex						m_loginUserListMutex ;
	
	//tcpý��Ự����
	//STRUCT_MEDIA_USER			m_mediaUserList[MAX_MEDIA_PLAYER] ;
	//CMutex						m_mediaUserListMutex ;
	
	//�����Ự
	STRUCT_ALARM_USER			m_alarmUserList[MAX_MEDIA_PLAYER] ;
	CMutex						m_alarmUserListMutex ;
	
	//�����ã����ؿͻ��˳ɹ���½���������Ӵ���
	int							m_nLoginTimes ;
	int							m_nConnectTimes ;
	
	//���������ӵ�tcp����
	int							m_nUserCount ;
	
	//��������ping�¼���sock���
	int							m_pingSock ;
	
	//�Ƿ����������ļ�
	int							m_isUpdating ;
	//�Ƿ������Upnp������
	int							m_isUPNPComplete ;
	//�Ƿ���Ҫ����ע��DDNS
	int							m_doDdns ;
	//DDns�ϴ�����ʱ��
	unsigned int				m_ddnsTime ;
	
	//�Ƿ���Ҫ���½���UPNP����
	int							m_doUPNP ;
	//UPNP�ϴ�����ʱ��
	unsigned int				m_upnpTime ;

	//��ǰʹ������
	int							m_netCard ;
	
	//�Խ���־״̬
	bool			m_talkOn ;
	int				m_talkUserID ;
	CMutex			m_taklMutex ;
	int				m_serverStartTime ;
	
	//======================================
	//��ǰupnpע����Ϣ��������������Ƿ�仯
	unsigned int				m_localHttpPort ;
	unsigned int				m_localVideoPort ;
	unsigned int				m_localMobilePort ;

	unsigned int				m_upnpHttpPort ;
	unsigned int				m_upnpVideoPort ;
	unsigned int				m_upnpMobilePort ;
	
			char 				m_ip[CHAE_IP_LEN];
	unsigned int				m_nNetCardID ;
	unsigned int				m_nUpnpSwitch ;
	bool                        m_upnpSuccess;

	//======================================
	//��ǰddnsע����Ϣ��������������Ƿ�仯
	

	unsigned int				m_netcnt;
	int							m_QuenceMsgID;
protected:
	//=============================================
	//�������߳̾��
	void*						m_serverHandle ;
	void*						m_serverThread ;
	void*						m_alarmThread ;
	void*						m_pingThread ;
	void*						m_emailThread ;
	void*						m_registThread ;
	void*						m_checkNetCardThread ;
	void*						m_ddnsThread ;
	void*						m_upnpThread ;
	void	 					*m_ntpThread;
protected:
    //===============================================
    //�������Ļص�����
    static bool onServerTcpAccept( int &userData , void *session , sockaddr_in *client_addr ) ;
    static void onServerTcpClose( int userData ) ;
    static bool onServerRecvData( int userData , char *data , int len , int &used ) ;
    static bool onServerSessionIdle( int userData ) ;

    //==============================================
    //�����������̣߳���������һЩ��������ҵ��
    //����: ping��Ϣ�Ļ�Ӧ,upnp,ddns��
    static void *checkNetCardThreadProc( void *pObj ) ;							

    //==============================================					
    //ddns�߳�
    static void *ddnsThreadProc( void *pObj ) ;

    //==============================================
    //upnp�߳�
    static void *upnpThreadProc( void *pObj ) ;

    //==============================================							
    //ping�߳�
    static void *pingThreadProc( void *pObj ) ;

    //==============================================
    //������Ϣ�����߳�
    static void *alarmThreadProc( void *pObj ) ;

    //==============================================
    //mail�����߳�
    static void *emailThreadProc( void *pObj ) ;

    //==============================================
    //�豸ע���߳�
    static void *registThreadProc( void *pObj ) ;
	
	//==============================================
    //NTP ��ʼ���߳�    
	static void *ntpThreadProc( void *arg);/*int ntp proc*/
protected:

    //==============================================
    //�¼�������
    void onClose( int userData ) ;
    bool onReceive( int userData , char *data , int len , int &used ) ;
    bool onIdle( int userData ) ;
    bool onAccept( int &userData , void *session , sockaddr_in *client_addr ) ;


    //==============================================
    //ping ��Ϣ�Ĵ���
    void handlePing( ) ;
    void handleAlarm( ) ;
    void handleUPNP( ) ;
    void handleDDNS( ) ;
    void handleEmail( ) ;
    void handleRegist( ) ;
#ifdef _BARCODE_SUP_
	static void regist_result_cb (reg_res_t *status) ;
#endif
	static int  m_staticSipRegState ;
	static bool	m_staticGetCallback ;
	static bool m_staticRegistNow ;

	int	FillBroadcastInfo(STRUCT_PING_ECHO* Broadcast, NETWORK_PARA* netset); /*add by hayson 2014.5.5*/
    void onPingCmd( int sock , char *data , int len , sockaddr_in remoteAddr ) ;
    void onIDPingCmd( int sock , char *data , int len , sockaddr_in remoteAddr ) ;
    void onUdpReboot( int sock , char *data , int len , sockaddr_in remoteAddr ) ;					

	int	hi_netcfg_update_dns(char *cfgFile, unsigned char* dnsaddr, unsigned char* dnsaddr2);
	
	int GetLocalNetInfo(  const char* lpszEth, char* szIpAddr, char* szNetmask, char* szMacAddr ) ;

	/*add by hayson begin 20131028 */
	/*dhcp ����*/
	int KillUdhcpc(char* pNetName);
	long GetDhcpPid(char *name, char* pNetName);
	long* FindPid(const char* pidName, char* pNetName);
	int CheckPid(char* pid, char* pNetName);
	int CookBuf(FILE * fp, char* pNetName);
	char* safe_strncpy(char *dst, const char *src, size_t size);
};


#endif



