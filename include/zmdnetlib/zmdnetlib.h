#ifndef _JIANGHM_ZMD_NET_LIB_HEADER_324987432987432
#define _JIANGHM_ZMD_NET_LIB_HEADER_324987432987432

#include "parametermanage.h"
#include "systemparameterdefine.h"
#include "interfacedef.h"
#include "ModuleFuncInterface.h"

extern PARAMETER_MANAGE		*g_cParaManage ;				

#define GetNetModule	CNetModule::getInstance

#define SERVER_PORT				80		//http�������˿�
#define	TCPLISTENPORT			8000	// ��ͨ(8000) ��ͨ(3389 1433)
#define	MOBILETCPLISTENPORT		9000	// ��ͨ(9000) ��ͨ(3389 1433)
#define	UDPLISTENPORT			8080	//δʹ��

#define	PACKSIZE				88
#define	TALKBUFSIZE				100*PACKSIZE


/*get network card name*/
#define	LOCAL_NET_NAME  get_local_name()
#define	WIFI_NET_NAME get_wifi_name()

/*get config file*/
#define MAC_ID_FILE get_mac_id_file()


//����Ϊ����
class CNetModule
{
    //�������캯������ֹ�ⲿʵ����
protected:
    CNetModule() ;
    ~CNetModule() ;
protected:
    static CNetModule *m_instance ;	//�������
public:
    //�����ӿ�
    static CNetModule *getInstance()
    {
        if( 0 == m_instance )
        {
            m_instance = new CNetModule() ;
        }
        return m_instance ;
    };
    static void release()
    {
        if( m_instance)
        {
            delete m_instance ;
            m_instance = 0 ;
        }
    };

public:

    //���ŵĹ��ܽӿ�
    bool StartNetDevice( ) ;

    //=================================
    //�ж�����״̬
    int JudgeNetworkStatus( ) ;

    //==================================
    //ˢ��ddns״̬
    void Do_ddns( ) ;

    //==================================
    //ˢ��upnp״̬
    void Do_upnp( ) ;

    //==================================
    //������������.
    int	 SetNetAttrib( NETWORK_PARA *netset, int net_card_id ) ;
    int  SetLoaclNetMac( ); /*��������MAC*/
	int  SetDHCP(char* pNetName );/*��DHCP*/
	int  DownDHCP(char* pNetName );/*�ر�DHCP*/
	
	//��������ӿ�
	//return 0: success
	int GetDevIpaddr(char* pNetName, char *net_ip);
	int SetDevIpaddr(char* pNetName, char *net_ip);

    //==================================
    //��������ģ��
    bool StartNetModule( ) ;

    //=================================================
    //����http������
    bool StartHttpServer( char *ip , unsigned short port ) ;

    //=======================================
    //��ѯ�Ƿ���������״̬
    bool IsUpdating( ) ;

    ///================================================
    ///����onvif������
    bool StartOnvifServer( ) ;

/**
* @brief �㲥�豸��Ϣ
*/
    void BroadcastDeviceInfo();
    
protected:
    //����������
    bool StartNetServer( ) ;
    bool StartMobileServer() ;
};


#endif
