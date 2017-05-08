

#ifndef _JIANGHM_ZMD_IPCAMERA_INTERFACE_HEADER_33432329324
#define _JIANGHM_ZMD_IPCAMERA_INTERFACE_HEADER_33432329324

//=================================================
// ע��: 
// ����Э���������Arm��,��ЩarmϵͳҪ���ϸ���ֽڶ���
// ��������ṹ����ҲҪ���ϸ��ֽڶ��룬�ֽ��޷�������
// ���ڴ濽������������ֽ�
// ��Щ�ṹ�а����ṹ�ģ�����Ҫ�ϸ���ṹ�Ƿ��ֽڶ���
//
#include "hi_type.h"
#include "hi_comm_rc.h"

#include "systemparameterdefine.h"
#include "ptz.h"
#include "RecordManage.h"


//==================================
//�û������ȶ��壬�������û�����صĲ�����
//ʹ�øú��������û���buf���ȡ�������ڵ���
#define USER_NAME_LEN		16
//==================================
//ͬ��
#define USER_PASS_LEN		16

//Ԥ�õ���Ϣbuf


#ifdef __cplusplus
extern "C" {
#endif
	
#pragma pack( 1 )
	
	//===============================================
	//��Ϣͷ����
    typedef struct
    {
		int						head;
		int						length;		//���ݳ���,ȥ��head
		unsigned char			type;
		unsigned char			channel;
		unsigned short			commd;
	}Cmd_Header ;
	
	//==============================================
	//����������Ϣͷ�ĺ�.
	//����Э����չ�����Ϣ�嶨��ҲҪʹ�øú�,����ÿ����Ϣ��
	//����Ϣͷ�Ķ��嶼һ����
#define DEF_CMD_HEADER		Cmd_Header		header
	
	//===============================================
	// �����Ǿ��������.
	// ���������뱣����ͬ��ʽ���䳤����Ҳ����ע��˵����Ա�����
	// ��Ϣ��  ��Ϣ����ṹ ��Ϣ��Ӧ�ṹ 
	// ��Ϣ����ṹ���� STRUCT_XXX_XXX_REQUEST
	// ��Ϣ��Ӧ�ṹ���� STRUCT_XXX_XXX_ECHO
	//==============================================
#define CMD_FORMATHDD			0x9c02	//����������ʽ��Ӳ��
	
	//==============================================
	//��д�豸MAC��ַ
#define CMD_S_MAC				0x9c00	
	
	typedef struct
	{
		DEF_CMD_HEADER ;
		char		macAddr[6] ;			//��Ҫ���õ�mac��ַ
	}STRUCT_SET_MAC_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER ;
		int			echo ;					//���ý�� 0 = �ɹ� , -1 = ʧ��
	}STRUCT_SET_MAC_ECHO ;
	
	//==============================================
	//��ȡ�豸MAC��ַ
#define CMD_G_MAC				0x9c01	
	
	typedef struct
	{
		DEF_CMD_HEADER ;
	}STRUCT_GET_MAC_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER;
		char		macAddr[6] ;		//mac��ַ
	}STRUCT_GET_MAC_ECHO ;
	
	//===================================================
	//PING��
#define CMD_PING				0x9001	
	
	//==========================================================
	//ping ����addr��CmdHeader.Type = 0 
	
	typedef struct
	{
		char			ipaddr[20];
		char			geteway[20];
		char			submask[20];
		char			mac[20];
	}ipaddr_tmp;
	
	typedef struct
	{
		unsigned short	webPort;//���ڸ��������߷���web�����˿�
		unsigned short	videoPort;//���ڸ��������߷���video�����˿�
		unsigned short	phonePort;//���ڸ��������߷���phone�����˿�
		unsigned short	recver;
	}devTypeInfo;
	

	typedef struct
	{
		int	alarmType;	//��������0:�ƶ����1:IO����
		int	alarmChl;	//����ͨ���Ż��߱���IO��,��0��ʼ
		int	recv[2];
	}alarmType;
	
	typedef struct
	{
		DEF_CMD_HEADER;
	}STRUCT_PING_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER;	
		TYPE_DEVICE_INFO	devInfo ;
		devTypeInfo			portInfo ;
		ipaddr_tmp			ipAddr ;
	}STRUCT_PING_ECHO ;
	
	//==========================================================
	//ping ����addr��CmdHeader.Type = 1 
	typedef struct
	{
		DEF_CMD_HEADER;
		ipaddr_tmp		ipAddr;
	}STRUCT_PING_SETADDR_REQUEST ;

	//ping ����addr��CmdHeader.Type = 2 ,����DHCP
	typedef struct
	{
		DEF_CMD_HEADER;
		char		mac[20];
	}STRUCT_PING_SETDHCP_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER;	
		int			echo ;
	}STRUCT_PING_SETADDR_ECHO ;
	
	//===========================================================
	
#define CMD_START_VIDEO			0x9002	//������Ƶ
#define CMD_START_SUBVIDEO		0x90a2	//����������
#define CMD_STOP_VIDEO			0x9003	//�ر���Ƶ
#define CMD_AUDIO_ON			0x9004  //�豸��������
#define CMD_AUDIO_OFF			0x9005  //�豸�����ر�

#define CMD_TALK_ON				0x9006  //�Խ�����
	
	typedef struct
	{
		unsigned char 			sampleRate; 		//������ 0:8K ,1:12K,  2: 11.025K, 3:16K ,4:22.050K ,5:24K ,6:32K ,7:48K;	
		unsigned char 			audioType;			//��������0 :g711 ,1:2726
		unsigned char 			enBitwidth;			//λ��0 :8 ,1:16 2:32
		unsigned char			recordVolume;		//�豸��ǰ��������0 --31 
		unsigned char 			speakVolume;		//�豸��ǰ�������0 --31
		unsigned short			framelen ;		//��Ƶ֡��С(80/160/240/320/480/1024/2048)
		unsigned char			reserved ;		//����		
	}Audio_Coder_Param ;

	typedef struct
	{
		DEF_CMD_HEADER;	
		//int			audio_code ;	//0:G711  1:G726
	}STRUCT_TALK_ON_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER;	
		int					talkFlag ;
		Audio_Coder_Param	audioParam ;
	}STRUCT_TALK_ON_ECHO ;

#define CMD_TALK_OFF			0x9007  //�Խ��ر�
	
	typedef struct
	{
		DEF_CMD_HEADER;	
	}STRUCT_TALK_OFF_REQUEST ;

#define CMD_TALK_DATA			0x9008	//�Խ�����
	
	typedef struct
	{
		DEF_CMD_HEADER;	
		char			audioData[0] ;		//����header�б�־
	}STRUCT_TALK_DATA ;

	//===================================================
	//��ȡ�豸�ͺţ���½
#define CMD_DECIVE_TYPE			0x9009	//�豸�ͺ�
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		char			name[USER_NAME_LEN] ;
		char			pwd[USER_PASS_LEN] ;
	}STRUCT_DEVICE_TYPE_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER ;
		TYPE_DEVICE_INFO	deviceInfo ;	//�豸��Ϣ
		int					echoCode ;		//��½�ɹ�Ϊ0 ,ʧ����Ϊ������
		int					permit ;		//Ȩ�ޣ�
	}STRUCT_DEVIDE_TYPE_ECHO ;
	
	//=========================================================
	//����֪ͨ
#define CMD_ALARM_UPDATE		0x9010	
	
	typedef struct
	{
		DEF_CMD_HEADER ;
		int	alarmType;						//��������0:�ƶ����1:IO����
		int	alarmChl;						//����ͨ���Ż��߱���IO��,��0��ʼ
		int	recv[2];
	}STRUCT_ALARM_UPDATE_REQUEST ;
	
	
	
#define CMD_SEND_SPEED			0x9020	//������Ƶ�طŴ����ٶ�
	
	//=========================================================
	//����ʵʱ��Ƶֱ��
#define CMD_START_720P          0x5000	//����720P
#define CMD_START_VGA			0x9002	//������Ƶ /*panjy ˵�� �޸���������*/
#define CMD_START_QVGA			0x90a2	//����������
	
	//������д����3������
	typedef struct
	{
		DEF_CMD_HEADER ;
	}STRUCT_START_LIVING ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int					result ;
	}STRUCT_START_LIVING_ECHO ;
	
	//==========================================================
	// ����ϵͳ����
#define CMD_S_DEV_PARA			0xa100	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		SYSTEM_PARAMETER	param ;			//ϵͳ����
	}STRUCT_SET_DEV_PARAM_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int				echo ;				//��Ӧ���(0:OK-1:Failed)
	}STRUCT_SET_DEV_PARAM_ECHO ;
	
	
	//=========================================================
	// ��̨����
#define CMD_S_PTZ				0xa300	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		PTZ_CMD_E	cmd;
		unsigned short para0;
		unsigned short para1;
	}STRUCT_SET_PTZ_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int	echo ;							//��Ӧ���(0:OK-1:Failed)
	}STRUCT_SET_PTZ_ECHO ;
	
	
	//=========================================================
	// ��������
#define CMD_S_REBOOT			0xa400	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		unsigned int	headFlag1	;		//0x55555555
		unsigned int	headFlag2	;		//0xaaaaaaaa
	}STRUCT_SET_REBOOT_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int echo ;							//��Ӧ���(0:OK-1:Failed)
	}STRUCT_SET_REBOOT_ECHO ;
	
	
	
	//=========================================================
	// �����������
#define CMD_S_REBOOT_OK			0xa4ff	
	
	//=========================================================
	// ��������
#define CMD_S_UPDATE			0xa500	
	typedef struct
	{
		DEF_CMD_HEADER	;
		unsigned int		checkFlag ;			//must be 0x5555aaaa
		unsigned int		fileLength ;		//�����ļ�����
		char				filename[96] ;		//�����ļ���
		//char				file[0] ;			//�ļ����ݣ�������fileLength����
	}STRUCT_SET_UPDATE_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int			echo ;						//0:succeed,1:malloc failed,2:get shared memory failed��
		//3:file length incorrect,4:file checksum incorrect,5:file version same
	}STRUCT_SET_UPDATE_ECHO ;
	
	
	//==========================================================
#define CMD_S_UPDATE_OK			0xa5ff	// �����������
	
	
	//==========================================================
	// ��client��ʱ
#define CMD_S_NTP				0xa600	
	typedef struct
	{
		DEF_CMD_HEADER	;
		datetime_setting		datetime ;	
	}STRUCT_SET_NTP_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int echo ;							//
	}STRUCT_SET_NTP_ECHO ;
	
	//===========================================================
	// �ָ���������
#define	CMD_S_RESTORE			0xa700	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		unsigned int	headFlag1	;		//0x55555555
		unsigned int	headFlag2	;		//0xaaaaaaaa
	}STRUCT_SET_RESTORE_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int echo ;
		
	}STRUCT_SET_RESTORE_ECHO ;
	
	
#define CMD_S_PANEL_RESTORE		0xa755	// �ָ�����������������
#define CMD_S_PANEL_STAT		0xa800	// ��������������
#define CMD_S_AUTOSWCTL			0xa900  // �������ؿ���
	
	//==========================================================
	// ����3G���ӻ����
#define CMD_S_3GCTL				0xaa00	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int			status ;				//ȡֵ��0���߷�0
	}STRUCT_SET_3GCTL_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int			echo ;					//
	}STRUCT_SET_3GCTL_ECHO ;
	
	//==========================================================
	
#define CMD_S_3G_REPOWER		0xaa05	// ����3g�ϵ縴λ
	
	
	//==========================================================
	// �ָ���ͨ��Ĭ����ɫ
#define CMD_S_CHN_ANA			0xa10f	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
	}STRUCT_RESET_CHNCOLOR_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int				echo ;
		ANALOG_CHANNEL	chnSet ;
	}STRUCT_RESET_CHNCOLOR_RESPONSE ;
	
	
	//===========================================================
	//���õ�ͨ��ģ����	
#define CMD_S_CHN_ANALOG		0xa110	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		ANALOG_CHANNEL	chnSet ;
	}STRUCT_SET_CHNANALOG_REQUEST ;
	
	//�޻�Ӧ
	
	//===========================================================
	//��ȡ��ͨ��ģ����
#define CMD_G_CHN_ANALOG		0xa111	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
	}STRUCT_GET_CHNANALOG_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int			echo ;
		ANALOG_CHANNEL	chnSet ;
	}STRUCT_GET_CHNANALOG_ECHO ;
	
	
	//======================================================
	//��ȡWIFI�ȵ�
#define CMD_G_WIFI_AP			0xa112	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
	}STRUCT_GET_WIFIAP_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		TYPE_WIFI_LOGIN		wifiAP[0] ;
	}STRUCT_GET_WIFIAP_ECHO ;
	
	//========================================================
	//����WIFI�ȵ�
#define CMD_S_WIFI_CONNECT		0xa113	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		TYPE_WIFI_LOGIN		wifiAP ;
	}STRUCT_SET_WIFICONNECT_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int			echo ;
	}STRUCT_SET_WIFICONNECT_ECHO ;
	
	
	//====================================================
	//��ȡ����״̬
#define CMD_G_WIFI_STATUS		0xa114	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		TYPE_WIFI_LOGIN		wifiAP ;
	}STRUCT_GET_WIFISTATUS_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		TYPE_WIFI_LOGIN		wifiAP ;
	}STRUCT_GET_WIFISTATUS_ECHO ;
	
	
	
	//===========================================================
	//// ��ȡ�豸ϵͳ����
#define CMD_R_DEV_PARA			0x9100	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
	}STRUCT_READ_DEV_PARAM_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		SYSTEM_PARAMETER	param ;			//ϵͳ����
	}STRUCT_READ_DEV_PARAM_ECHO ;
	
	//=========================================================
#define CMD_R_ALARMINFO			0x9600	// ��ѯ������Ϣ
	
	//====================================================
	// ��ѯ��־
#define CMD_R_LOG				0x9700	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		//FindUserLogItem		logFind ;		//
	}STRUCT_READ_LOG_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		//FindLogItems		logItem ;		//
	}STRUCT_READ_LOG_ECHO ;
	
	
	//=================================================
	//// ��״̬��Ϣ
#define CMD_R_DEV_INFO			0x9800	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
	}STRUCT_READ_DEVINFO_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		TYPE_DEVICE_INFO	devInfo ;
		BlockDevInfo_S		storageDev[2]  ;	//2���洢�豸��Ϣ
	}STRUCT_READ_DEVINFO_ECHO ;
	
#define CMD_R_PANEL_INFO		0x9810	// ����������״̬��Ϣ
	
	//===================================================
	// �����ط��б�
#define CMD_R_SEARCH_PLAYLIST	0x9900  
	
	typedef struct
	{
		DEF_CMD_HEADER	;	
		int				mode ;					//��0
		FindFileType	findType ;				
	}STRUCT_READ_PLAYLIST_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		RecordFileName		result ;			//��ѯ���
	}STRUCT_READ_PLAYLIST_ECHO ;
	
	//================================================
	
#define CMD_G_SEARCH_RECORD_DAY	0x9901 	
typedef struct
{
	DEF_CMD_HEADER	;	
	struct tm		day_start ;	
	struct tm		day_end ;	
	int				channel;	
	char   			res[140];
}STRUCT_GET_RECORD_DAY_REQUEST ;	
	//================================================
	// �طſ�ʼ����
#define CMD_PLAYBACK_PLAY		0x9903	
	
	typedef struct
	{
		unsigned int	p_mode; 		// 0-�ط� 1- ����
		unsigned int	p_offset;		// �ط� - ʱ��ƫ����  ���� - ��Сƫ����(K)
		unsigned int 	start_time;		/*�ļ���ʼʱ��*/
		unsigned int 	end_time;		/* �ļ�����ʱ��*/
		unsigned int    filesize;		/*�ļ���С��KΪ��λ*/
		int				m_filetype;		//0-�ֶ���1-��ʱ��2-����
		char			d_name[96]; 	/*�ļ���(��·��)*/
	}PLAYBACK_ITEM;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		PLAYBACK_ITEM	playItem ;
	}STRUCT_PLAYBACK_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int			echo ;
	}STRUCT_PLAYBACK_ECHO ;
	
	
#define CMD_PLAYBACK_STOP		0x9905	// �ط�ֹͣ����
	
	//=================================================
	// ��½��������
#define CMD_REQ_LOGIN			0x9a00	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		char			name[USER_NAME_LEN] ;
		char			pwd[USER_PASS_LEN] ;
	}STRUCT_REQ_LOGIN_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int	permit  ;
		int echo ;
	}STRUCT_REQ_LOGIN_ECHO ;
	
	//================================================
	// ��3G����״̬���ͺ�
#define CMD_R_3G_INFO			0x9b00	
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		unsigned int		l3gtype ;	//0xaaaa5555
	}STRUCT_READ_3GINFO_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int		echo ;
		int		lg3linkstate ;		//0:no 3g connect��1:3g is dialing��2:3g is connecting
		int		lg3type ;			//����modem�ͺ�ѡ��0x00~0xff
		char	lip_buff[16] ;		//
	}STRUCT_READ_3GINFO_ECHO ;
	
	//====================================================
	//added by liql-2012-07-13
	//ͼ����������
#define CMD_S_PIC_NORMAL		0xa115	//����
#define CMD_S_PIC_FLIP			0xa116	//��תON
#define CMD_S_PIC_MIRRORON	0xa117    //����
#define CMD_S_PIC_FLIPMIRROR	0xa118	//����ת
#define CMD_S_PIC_COLORON		0xa119	//��ɫ
#define CMD_S_PIC_COLOROFF		0xa120	//�ڰ�
	
	typedef struct
	{
		DEF_CMD_HEADER	;
	}STRUCT_SET_PIC_REQUEST ;
	
	//=====================================================
	//����sensor������ѹ
#define CMD_S_SENSORFREQ_50		0xa121	//sensor������ѹ50HZ
#define CMD_S_SENSORFREQ_60		0xa122	//sensor������ѹ60HZ
#define CMD_S_SENSORFREQ_OUT 	0x9123	//����ģʽ
#define CMD_S_SENSORFREQ_AUTO	0x9124	//�Զ�ģʽ

	
	typedef struct
	{
		DEF_CMD_HEADER	;
	}STRUCT_SET_SENSORF_REQUEST ;
	

	//====================================================
	//��ȡԤ�õ���Ϣ
#define CMD_S_GET_PRESET_POINT	0xA123
	typedef struct
	{
		DEF_CMD_HEADER	;
	}STRUCT_GET_PRESET_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER	;
		char			presetData[514] ;	//���512��Ԥ�õ���Ϣ+1�ֽ�ͣ��ʱ��+1�ֽڻ����ٶ�
	}STRUCT_GET_PRESET_ECHO ;

	
#define CMD_CMS_REQUEST_VIDEO		0x7000
#define CMD_CMS_STOP_VIDEO		0x7001
#define CMD_CMS_START_PLAYBACK	0x7002
	
#define	CMD_REGSMSPLAT			0xb001
#define	CMD_SMSPLATHEATBEAT		0xb002
#define	CMD_SMSUPDATE			0xb003
	
	//=======================================================
#define  CMD_SET_VIDEO_CODE             0x7788//liql

	typedef struct
	{
		DEF_CMD_HEADER	;
		CAMERA_PARA		param ;
	}STRUCT_SET_VIDEO_CODE_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER	;
		int				echo ;
	}STRUCT_SET_VIDEO_CODE_ECHO ;
	
	//======================================================
	//��ȡ�豸��Ƶ������Ϣ
#define  CMD_R_VIDEO_CODE_PARA			0x7a71//liql
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		
	}STRUCT_READ_VIDEOPARAM_REQUEST ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		CAMERA_PARA		param ;
	}STRUCT_READ_VIDEOPARAM_ECHO ;
	
	//added by panjy ���ͱ�����Ϣ
#define CMD_MOTION_DETECT      0x5030
	
	//=======================================================
	// NVR ֪ͨIPC������������˿�
#define CMD_NVR_PORT 0x5050
	typedef struct
	{
		DEF_CMD_HEADER	;
		unsigned short		alarmListenPort ;
		unsigned short		devType ;
	}STRUCT_NVR_PORT_REQUEST ;
	
	
	//=======================================================
	//��ȡ����
#define CMD_G_SNAPSHOT			0x9040
	//======================================================
	// client -> server
	typedef struct
	{
		DEF_CMD_HEADER	;
	}STRUCT_GET_SNAPSHOT_REQUEST ;
	
	//======================================================
	//server -> client 
	typedef struct
	{
		DEF_CMD_HEADER	;
		int			echo ;
		unsigned int	fileLength ;
		char			imageContent[0] ;
	}STRUCT_GET_SNAPSHOT_ECHO ;
	
	
	//==============================================
	//��������ֵ�趨
#define CMD_S_SENSOR_ALARMINFO			0x9042
	
	//=================================
	// client -> server
	typedef struct
	{
		DEF_CMD_HEADER	;
		float		temperature_min ;			//�¶����ֵ
		float		temperature_max ;			//�¶����ֵ
		float		humidity_min ;			//ʪ�����ֵ
		float		humidity_max ;			//ʪ�����ֵ
		int		sampleRate ;				//��������
	}STRUCT_SET_SENSORALARMINFO_REQUEST ;
	
	//=====================================================
	//server -> client 
	typedef struct
	{
		DEF_CMD_HEADER	;
		int		echo ;
	}STRUCT_SET_SENSORALARMINFO_ECHO ;

	
	//=====================================================
	//������״̬��ȡ
#define CMD_G_SENSOR_ALARMINFO			0x9044
	
	//====================================================
	// client -> server
	typedef struct
	{
		DEF_CMD_HEADER	;
	}STRUCT_GET_SENSORALARMINFO_REQUEST ;
	
	//=====================================================
	//server -> client 
	
	typedef struct
	{
		int 		sensorIndex ;
		float		curTem ;
		float		curHum ;
	}STRUCT_SENSOR_CURINFO ;
	
	typedef struct
	{
		DEF_CMD_HEADER	;
		int		echo ;
		float		minTem ;
		float		maxTem ;
		float		minHum ;
		float		maxHum ;
		int		sampleRate ;
		int		sensorCount ;
		STRUCT_SENSOR_CURINFO sensorInfo[0] ;
	}STRUCT_GET_SENSORALARMINFO_ECHO ;
	
	//====================================================
	//��ȡ��ǰͨ����������Ϣ
#define CMD_GET_CHN_INFO		0x9022
	
	//����ͨ������Ϣ.

	typedef struct
	{
		int				chnid ;			//ͨ����
		unsigned int 	ipcaddr ;		//��ͨ�����󶨵�ipc��ַ
		unsigned short	ipcport ;		//��ͨ�����󶨵�ipc�˿�
		unsigned short	chnstatus ;		//ͨ��״̬,Ŀǰ���� 1-������ �� 0-δ����
		unsigned short	ipcType ;		//IPC����: 0-˽��Э�飬1-onvif
		unsigned short	res ;			//�����ֶ�
	}NVR_CHN_INFO ;

	typedef struct
	{
		DEF_CMD_HEADER	;
	}STRUCT_GET_CHNINFO_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER	;
		int				autoSearchFlag ;
		int				chnCount ;
		NVR_CHN_INFO	chn[0] ;
	}STRUCT_GET_CHNINFO_ECHO ;

	//=================================================
	//����NVRͨ����IPC��Ϣ
#define	CMD_SET_CHN_IPC			0x9024
	typedef struct
	{
		DEF_CMD_HEADER	;		
		char				ipcAddr[16] ;	//ipc��ַ
		unsigned short		ipcPort ;		//ipc�˿�
		unsigned short		ipcType ;		//ipc����
		char				username[16] ;	//��½�û���
		char				password[16] ;	//��½����
	}STRUCT_SET_CHNIPC_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER	;		
		int			echo ;					//0:�ɹ���
	}STRUCT_SET_CHNIPC_ECHO ;

	//=================================================
	//ɾ��NVRͨ�����󶨵�IPC��Ϣ
#define CMD_DEL_CHN_IPC			0x9026
	typedef struct
	{
		DEF_CMD_HEADER	;		
		
	}STRUCT_DEL_CHNIPC_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER	;		
		int				echo ;				//0 :�ɹ�
	}STRUCT_DEL_CHNIPC_ECHO ;

	//=================================================
	//ָ��ͨ�������Զ�����
#define CMD_SET_AUTOADD			0x9028
	typedef struct
	{
		DEF_CMD_HEADER	;		

	}STRUCT_SET_AUTOADD_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER	;		
		int				echo ;
		NVR_CHN_INFO	chnInfo ;	
	}STRUCT_SET_AUTOADD_ECHO ;

	//=================================================
	//�����Զ���������
#define CMD_SET_AUTOSEARCH		0x9030
	typedef struct
	{
		DEF_CMD_HEADER	;		
		int			autoSearchFlag ;
	}STRUCT_SET_AUTOSEARCH_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER	;		
		int			echo ;
	}STRUCT_SET_AUTOSEARCH_ECHO ;

	//===================================================
	//PING��
#define CMD_ID_PING				0x9050	
	
	//==========================================================
	//ping ����addr��CmdHeader.Type = 0 
	typedef struct
	{
		DEF_CMD_HEADER;
	}STRUCT_ID_PING_REQUEST ;
	
	typedef struct
	{
		STRUCT_PING_ECHO	device; 
		char				deviceID[16] ;
	}STRUCT_ID_PING_ECHO ;

	//==========================================================
	//�����豸ID����
#define CMD_S_DEVICEID		0x9054
	typedef struct
	{
		DEF_CMD_HEADER;
		char			mac[6] ;
		char			res[2] ;
		char			deviceID[16] ;
	}STRUCT_SET_DEVICEID_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER;
		int				echo ;		//0�ɹ�,����ʧ�ܴ�����
	}STRUCT_SET_DEVICEID_ECHO ;

	//=========================================================
	//��ȡ�豸ID ����
#define CMD_G_DEVICEID		0x9056

	typedef struct
	{
		DEF_CMD_HEADER;
	}STRUCT_GET_DEVICEID_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER;
		char			mac[6] ;
		char			res[2] ;
		char			deviceID[16] ;
	}STRUCT_GET_DEVICEID_ECHO ;
	
	//add by table : ����udp��������
#define CMD_SET_UDP_REBOOT	0x9036
	typedef struct
	{
		DEF_CMD_HEADER;
		unsigned int	headFlag1	;		//0x55555555
		unsigned int	headFlag2	;		//0xaaaaaaaa
		char			rebootIP[16] ;	
	}STRUCT_UDP_REBOOT_REQUEST ;	
	
//����״̬��ʾ
#define CMD_UPDATE_STATUS		0x9038
	typedef struct
	{
		DEF_CMD_HEADER ;
		unsigned short	percent;		//�ٷֱ�0-100
		unsigned short	status ;		//����״̬��0����״̬,1����״̬
	}STRUCT_UPDATE_STATUS ;

//��ȡ�豸��Ƶ����
#define CMD_G_TALK_SETTING		0x9060

	typedef struct
	{
		DEF_CMD_HEADER ;
	}STRUCT_GET_TALK_SETTING_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER ;

		Audio_Coder_Param		audioParam ;
		//unsigned short		micVol;		//�豸������� 0-31;
		//unsigned short		spkVol ;	//�豸���������� 0-31;
	}STRUCT_GET_TALK_SETTING_ECHO ;

//�豸��Ƶ����
#define CMD_S_TALK_SETTING		0x9062
	typedef struct
	{
		DEF_CMD_HEADER ;
		unsigned short		micVol ;	//�豸�������0-31 ;
		unsigned short		spkVol ;	//�豸������� 0-31;
	}STRUCT_SET_TALK_SETTING_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER ;
		int					echo ;		//0�ɹ�
	}STRUCT_SET_TALK_SETTING_ECHO ;

//========================================================
//�Ƿ�����Ƶ,�޻�Ӧ
//������ֻ��֪ͨ�������������ӷ��͵�ý������
//�Ƿ�ð�����Ƶ
#define CMD_SET_AUDIOSWITCH		0x9066
	typedef struct
	{
		DEF_CMD_HEADER ;
		int		sendAudio ;			//0��������Ƶ , 1������Ƶ
	}STRUCT_SET_AUDIO_SWITCH ;

	typedef struct
	{
		DEF_CMD_HEADER ;
		int		echo ;			//0�ɹ� , ����ʧ�ܣ�������
	}STRUCT_SET_AUDIO_SWITCH_RESP ;

	//��ʼ¼��
#define CMD_START_RECORD		0x9068
	typedef struct
	{
		DEF_CMD_HEADER ;
	}STRUCT_START_RECORD_REQUEST ;

	typedef struct
	{
		DEF_CMD_HEADER ;
		int		echo ;			//0���ɹ���1,�豸����,�޴洢�豸
	}STRUCT_STOP_RECORD_ECHO ;
	//ֹͣ¼��
#define CMD_STOP_RECORD			0x9070
	typedef struct
	{
		DEF_CMD_HEADER ;		
	}STRUCT_STOP_RECORD ;
//��ʼ����
#define CMD_RECORD_BACKUP		0x9072
	typedef struct
	{
		DEF_CMD_HEADER ;
		char	pathname[128];	//��Ҫ���ݵ�Ŀ��¼���ļ���
	}STRUCT_RECORD_BACKUP_REQUEST ;
//�����ļ�״̬
#define CMD_RECORD_BACKUP_STATE	0x9074
	typedef struct
	{
		DEF_CMD_HEADER ;
		unsigned short		state ;		//����״̬ 0,���ڿ���,1 Ŀ���ļ���Ч, 2 �豸���ϣ�3�������
		unsigned short		precent ;	//���Ȱٷֱ�0-100
	}STRUCT_RECORD_BACKUP_STATE ;
//��ȡ�ֶ�¼��״̬
#define CMD_G_RECORD_STATUS 0x9076
typedef struct _STRUCT_GET_RECORD_STATUS {
    DEF_CMD_HEADER;
    int     status;  //�ֶ�¼��״̬��0 �ֶ�¼��δ������1 �ֶ�¼���ѿ���
} STRUCT_GET_RECORD_STATUS_ECHO;

#define CMD_REGIST_G_RECORD_STATUS 0x9077   //clientע���ȡͨ��¼��״̬��Ϣ,nvr�˻ᶨʱ����¼��״̬��client,client��Ҫ����5310�˿�,������
typedef struct 
{	
	DEF_CMD_HEADER ;
	int  echo;   //0 ��ʾ�ɹ�
}STRUCT_REGIST_G_RECORD_ECHO;

#define CMD_RECORD_STATUS 0x9078
typedef struct 
{	
	DEF_CMD_HEADER ;
	int status[32];//��ǰ32��ͨ����¼��״̬
	int  changeFlag;//��־����status[0 -31]�ж�Ӧ��ֵ�Ƿ�ı�, 1Ϊ�ı�
}STRUCT_CHANNEL_RECORD_STATUS;

#define CMD_G_CHANNEL_PARA  0x9080
typedef struct _STRUCT_G_CHANNEL_INFO_RESPONSE {
    DEF_CMD_HEADER;
    int    result;  //0 �ɹ���1 ʧ��
    CHANNEL_PARA    channel_para;
} STRUCT_G_CHANNEL_PARA_RESPONSE;

#define CMD_S_CHANNEL_PARA  0x9081
typedef struct _STRUCT_S_CHANNEL_INFO_RESPONSE {
    DEF_CMD_HEADER;
    int    result;  //0 �ɹ���1 ʧ��
} STRUCT_S_CHANNEL_PARA_RESPONSE;

#define CMD_G_WIFI_AP_CONFIG    0x9084
typedef struct _STRUCT_G_WIFI_AP_CONFIG_RESPONSE {
    DEF_CMD_HEADER;
    int         result;  //0 �ɹ���1 ʧ��
   // ApSettings  apInfo;
} STRUCT_G_WIFI_AP_CONFIG_RESPONSE;

#define CMD_S_WIFI_AP_CONFIG    0x9085  //����wifi����
typedef struct _STRUCT_S_WIFI_AP_CONFIG_RESPONSE {
    DEF_CMD_HEADER;
    int         result;  //0 �ɹ���1 ʧ��
} STRUCT_S_WIFI_AP_CONFIG_RESPONSE;

#define CMD_R_USB_INFO      0x9086      //��ȡUSB��Ϣ
typedef struct _STRUCT_R_USB_INFO_RESPONSE {
    DEF_CMD_HEADER;
    BlockDevInfo_S  usb_info[2];
} STRUCT_R_USB_INFO_RESPONSE;

#define CMD_GET_WIFILED   0x9088  //��ȡWIFIָʾ��
typedef struct _STRUCT_GET_WIFILED_RESPONSE {    
    DEF_CMD_HEADER;
    int result;         //0 �ɹ�, ��0 ʧ��
    int status;         //0 �ر�, 1 ����
} STRUCT_GET_WIFILED_RESPONSE;

#define CMD_SET_WIFILED   0x908a  //����WIFIָʾ��
typedef struct _STRUCT_SET_WIFILED {    
    DEF_CMD_HEADER;
    int status;         //0 �ر�, 1����
} STRUCT_SET_WIFILED;
typedef struct _STRUCT_SET_WIFILED_RESPONSE {   
    DEF_CMD_HEADER;
    int result;         //0 �ɹ�, ��0 ʧ��
} STRUCT_SET_WIFILED_RESPONSE;

#define CMD_GET_AUDIOPARM 0x9090  //��ȡ��Ƶ����
typedef struct _STRUCT_GET_AUDIOPARM_RESPONSE {
    DEF_CMD_HEADER;
    int result;     //0 �ɹ�, ��0 ʧ��
    Audio_Coder_Param param;
} STRUCT_GET_AUDIOPARM_RESPONSE;

typedef struct
{
	int  m_ntp;			 	//NTP��ʱ����
	int  m_timezone;     	//��UTC �����Ĳ��,������,��Χ [-12*3600, 13*3600]
	int  m_tzindex;        	//��ǰʱ����ʱ����(��������ʱ��)������[1-91], 0�����Զ���ȡʱ��
	int  m_daylight;		//��ʱ���Ƿ���������ʱ
	char reserved[12];
} TimeZoneInfo;

/*added by hayson  begin 2013.8.14*/
#define CMD_SET_TIMEZONE	0x9120		/* ����ʱ��*/
#define CMD_GET_TIMEZONE	0x9122		/*��ȡʱ��*/
#define CMD_CODE_I_FRAME	0x9092		/*ǿ��I ֡����*/

/*�����*/
typedef struct      
{
	DEF_CMD_HEADER ;
	int 	stream_type;				/*0:����ͨ��, 1:����ͨ��,2:����ͨ��*/
}STRUCT_FORCE_I_FRAME_REQUEST;

/*��Ӧ��*/
typedef struct      
{
	DEF_CMD_HEADER ;
	int		result;						/*0���ɹ���1��ʧ��*/
}STRUCT_FORCE_I_FRAME_RESPONSE;

/*added by hayson  end 2013.8.24*/


/*added by hayson  begin 2013.11.26*/
#define CMD_S_DEVCONFIG  0x9057
typedef struct
{
	DEF_CMD_HEADER;
	DeviceConfigInfo 	ConfigInfo;
}STRUCT_SET_DEVICE_CONFIG_REQUEST ;

typedef struct
{
	DEF_CMD_HEADER;
	int ret;				/*(0�����óɹ�1��У�����2��ƽ̨�汾��ƥ��)*/
}STRUCT_SET_DEVICE_CONFIG_RESPONSE ;


/*added by hayson  end 2013.11.26*/

/**
 * add by mike,2013-12-19
 * �����Զ�������Ҫ�Ķ�ӦZSP�ӿ�Э��
 */
#define CMD_CHECK_SW_UPDATE_INFO	0x9125		/* �������汾������Ϣ */
/*�����*/
typedef struct      
{
	DEF_CMD_HEADER ;
}STRUCT_CHECK_SW_UPDATE_INFO_REQ;

/*��Ӧ��*/
typedef struct      
{
	DEF_CMD_HEADER ;
	int		CheckFlag;				/* ����Ƿ�ɹ���0:�ɹ���-1:ʧ�� */
	char	NewVersion[32];			/* ����APP�汾 */
	int		UpdateFlag;				/* �Ƿ����°汾���� ��1���ǣ�0: �� */
	char	Description[0];			/* �°汾�����������Ϣ */
	
}STRUCT_CHECK_SW_UPDATE_INFO_RESPONSE;

#define CMD_START_REMOTE_UPDATE		0x9126		/* ��ʼ�Զ����� */
/*�����*/
typedef struct		
{
	DEF_CMD_HEADER ;
}STRUCT_START_REMOTE_UPDATE_REQ;

/*��Ӧ��*/
typedef struct		
{
	DEF_CMD_HEADER ;
	int	Result; /* 0:��ʾ�ɹ���Ӧָ���ʼ�Զ�������-1:����ʧ��*/
	
}STRUCT_START_REMOTE_UPDATE_RESPONSE;

#define CMD_GET_REMOTE_UPDATE_STAT	0x9127	/* ��ȡ�Զ�����״̬ */
/* ����� */
typedef struct		
{
	DEF_CMD_HEADER ;
}STRUCT_GET_REMOTE_UPDATE_STAT_REQ;

/* ��Ӧ�� */
typedef struct		
{
	DEF_CMD_HEADER ;
	int Flag;			/* �Ƿ���ȷ��Ӧ��ʶ,0:�ǣ�-1:�� */
	int UpdateFlag; 	/* 0:��ǰû���ڽ���Զ��������1:���������ļ���2:��������ͣ
						3: ������ɣ�����������4:�������� */
	int process;		/* �����ǰ���ڽ���Զ����������Ϊ������Ϣ */

}STRUCT_GET_REMOTE_UPDATE_STAT_RESPONSE;

#define CMD_CANCEL_REMOTE_UPDATE	0x9128		/* ȡ���Զ����� */
/*�����*/
typedef struct		
{
	DEF_CMD_HEADER ;
}STRUCT_CANCEL_REMOTE_UPDATE_REQ;

/*��Ӧ��*/
typedef struct		
{
	DEF_CMD_HEADER ;
	int	Result; 		/* �Ƿ�ȡ���ɹ�,0:ȡ���ɹ���-1:ȡ��ʧ�� */
} STRUCT_CANCEL_REMOTE_UPDATE_RESPONSE;

#define CMD_PAUSE_REMOTE_UPDATE 0x9129		/* ��ͣ�Զ����� */
/*�����*/
typedef struct		
{
	DEF_CMD_HEADER ;
}STRUCT_PAUSE_REMOTE_UPDATE_REQ;

/*��Ӧ��*/
typedef struct		
{
	DEF_CMD_HEADER ;
	int	Result; 		/* ��ͣ���0:��ͣ�ɹ���-1:��ͣʧ�� */
} STRUCT_PAUSE_REMOTE_UPDATE_RESPONSE;

#define CMD_RESUME_REMOTE_UPDATE	0x9130		/* �ָ��Զ����� */
/*�����*/
typedef struct		
{
	DEF_CMD_HEADER ;
}STRUCT_RESUME_REMOTE_UPDATE_REQ;

/*��Ӧ��*/
typedef struct		
{
	DEF_CMD_HEADER ;	
	int Result; 		/* �ָ����0:�ָ��ɹ���-1:�ָ�ʧ�� */
} STRUCT_RESUME_REMOTE_UPDATE_RESPONSE;

/*add by hayson begin 2014.6.25*/
#define CMD_GET_DATE_FORMAT 	0x9131 	/* ��ȡʱ���ʽ */
#define CMD_SET_DATE_FORMAT 	0x9132	/* ����ʱ���ʽ */

typedef struct      
{
	unsigned char 		DateMode;		/* ������ʾ�ķ�ʽ	0 ��ʾdd-mm-yyyy ,1��ʾyyyy-mm-dd,2��ʾmm-dd-yyyy */
	unsigned char		TimeMode;		/* ʱ��ĸ�ʽ:0 ��ʾ24 hours, 1 ��ʾam/pm */
	unsigned char		DateSeparator;	/* ���ڷָ���: - | / ���� */
	unsigned char		reser;			/* ���� */
}STRUCT_DATE_FORMAT;

/*�����*/
typedef struct		
{
	DEF_CMD_HEADER ;
}STRUCT_COMMON_REQ;

typedef struct		
{
	DEF_CMD_HEADER ;	
	int Result; 		/* �ָ����0:�ָ��ɹ���-1:�ָ�ʧ�� */
} STRUCT_COMMON_RESPONSE;

/*���ڸ�ʽ��*/
typedef struct      
{
	DEF_CMD_HEADER ;
	STRUCT_DATE_FORMAT DateMode_s;
}STRUCT_DATE_FORMAT_REQ;
/* add by hayson end*/

/* ������ȡ¼���豸��Ϣ�ӿ�*/
#define CMD_R_DEV_DISK  0x9801	
typedef struct
{
		DEF_CMD_HEADER	;
}STRUCT_READ_DISK_REQUEST ;

typedef struct
{
		DEF_CMD_HEADER	;
		char diskNum;
		BlockDevInfo_S blockDevInfo;
}STRUCT_READ_DISK_ECHO ;

/*��ȡipc�豸�汾��Ϣ*/
#define CMD_GET_IPC_VERSION 0x9011
typedef struct
{
	DEF_CMD_HEADER;
	char	UbootVersion[16];
	char	KernelVersion[16];
	char	RootfsVersion[16];
	char	AppVersion[16];	
}STRUCT_VERSION_RESPONSE;

/* 
** ssid��¼��������
** @hayson 2014.12.9
*/
#define CMD_S_DEFAULT_CONFIG 0x9058 /* ����ȱʡ��Ϣ*/
#define SSID_LEN	32

/* ȱʡ��Ϣ */
typedef struct
{
	char ap_ssid[SSID_LEN];			/* �ȵ�����*/
	char ap_password[SSID_LEN];		/* �ȵ����� */
	char reserve[448];				/* Ԥ��*/
}DevDefaultConfig;


/*�����*/
typedef struct		
{
	DEF_CMD_HEADER;
	DevDefaultConfig config;
}STRUCT_S_DEFAULT_CONFIG_REQ;

/* ��Ӧ�� */
typedef struct		
{
	DEF_CMD_HEADER;
	int ret;
}STRUCT_S_DEFAULT_CONFIG_RESPONSE;

#define CMD_G_DEFAULT_CONFIG 0x9059 /* ��ȡȱʡ��Ϣ*/
/*�����*/
typedef struct		
{
	DEF_CMD_HEADER;
}STRUCT_G_DEFAULT_CONFIG_REQ;

/* ��Ӧ�� */
typedef struct		
{
	DEF_CMD_HEADER;
	DevDefaultConfig config;
}STRUCT_G_DEFAULT_CONFIG_RESPONSE;


/* 
** miniNVR����ipc����wifi
** @hayson 2015.1.27
*/

#define CMD_SET_IPC_WIFI_BACK 0x9012

/*�����*/
typedef struct		
{
	DEF_CMD_HEADER ;
	TYPE_WIFI_LOGIN	SendData;
}STRUCT_S_BACKUP_IPC_WIFI_REQ;
/* 
** miniNVR֪ͨipc����״̬
** @hayson 2015.2.10
*/
#define CMD_NOTIFY_IPC_NETSTATUS 0x9013

/*�����*/
typedef struct		
{
	DEF_CMD_HEADER ;
	int	 route;
}STRUCT_S_NOTIFY_IPC_REQ;

/*
** �����ŵƿ��Ʋ���
*/
#define CMD_S_DOOR_LAMP_PARAM 0x9600

typedef struct 
{
	unsigned char light_switch;//Light ����1:��2:��3:�Զ�Ĭ��ֵΪ2(��)
	unsigned char white_switch;//�׳�ƿ���1:��2:��Ĭ��:1(��)
	unsigned char breath_switch;//��ɫ�л�(��������) 1:��2:��Ĭ��Ϊ2(��)
	unsigned char color_red;
	unsigned char color_green;
	unsigned char color_blue;
} DoorLampParam;

/* ����� */
typedef struct
{
	DEF_CMD_HEADER;
    DoorLampParam SendData;
} STRUCT_S_DOOR_LAMP_PARAM_REQ;

/* ��Ӧ�� */
typedef struct 
{
	DEF_CMD_HEADER;
	int result; /* 0, �ɹ�. -1, ʧ�� */
} STRUCT_S_DOOR_LAMP_PARAM_RESP;


/* wifiNvr ֪ͨipc ʹ���ض�����*/
#define CMD_S_WIFINVR_NOTIFY 0x9601


/* ����� */
typedef struct 
{
	DEF_CMD_HEADER;
} STRUCT_S_NOTIFY_IPC_WIFINVR_REQ;

/* ��Ӧ�� */
typedef struct 
{
	DEF_CMD_HEADER;
	int echo; /* 0, ֧��; -1, ��֧��; ���߲���Ӧ */
} STRUCT_S_NOTIFY_IPC_WIFINVR_RSP;

/* wifiNVR  ֪ͨIPC �ı�����*/
#define CMD_S_CHANGE_IPC_STREAM_RATE 0x9602

/* ����� */
typedef struct
{
	DEF_CMD_HEADER;
	int level; 
} STRUCT_S_CHANGE_IPC_STREAM_RATE_REQ;

/* ��Ӧ�� */
typedef struct 
{
	DEF_CMD_HEADER;
	int echo;
} STRUCT_S_CHANGE_IPC_STREAM_RATE_RSP;

#define CMD_G_IPC_VENC_ATTR 0x9603 


typedef struct 
{ 
DEF_CMD_HEADER; 
} STRUCT_G_IPC_VENC_ATTR_REQ; 

typedef struct 
{ 
DEF_CMD_HEADER; 
VENC_ATTR_H264_VBR_S attr; 
} STRUCT_G_IPC_VENC_ATTR_RSP; 

#define CMD_S_IPC_VENC_ATTR 0x9604 

typedef struct 
{ 
DEF_CMD_HEADER; 
VENC_ATTR_H264_VBR_S attr; 
} STRUCT_S_IPC_VENC_ATTR_REQ; 

typedef struct 
{ 
DEF_CMD_HEADER; 
int result; 
} STRUCT_S_IPC_VENC_ATTR_RSP; 



/* ��ȡ��Ƶ���ܵ���Կ */
#define CMD_G_VIDEO_KEY 0x9636

/* ����� */
typedef struct
{
	DEF_CMD_HEADER;
} STRUCT_G_VIDEO_KEY_REQ;

/* ��Ӧ�� */
typedef struct
{
	DEF_CMD_HEADER;
	char key[1024];
} STRUCT_G_VIDEO_KEY_RESP;

// ��ȡ��ص���
#define CMD_G_BATTERY_ADC_VALUE 0x9637

/* ����� */
typedef struct
{
	DEF_CMD_HEADER;
} STRUCT_G_BATTERY_ADC_VALUE_REQ;

/* ��Ӧ�� */
typedef struct
{
	DEF_CMD_HEADER;
	int value;
} STRUCT_G_BATTERY_ADC_VALUE_RESP;

#ifdef FACTORY_TEST
#define CMD_S_FACTORY_STATE 0x9640

/* ����� */
typedef struct
{
	DEF_CMD_HEADER;
	char ssid[256]; // ����ssid�����豸״̬
} STRUCT_S_FACTORY_STATE_REQ;

/* ��Ӧ�� */
typedef struct
{
	DEF_CMD_HEADER;
	int ret; // 0,�ɹ�;-1ʧ��
} STRUCT_S_FACTORY_STATE_RESP;
#endif



//�����ƶ��������
#define CMD_S_MD_REGIN 0x9045

/* ����� */
typedef struct
{
	DEF_CMD_HEADER;
	P2P_MD_REGION_CHANNEL region;
} STRUCT_S_MD_REGIN_REQ;

/* ��Ӧ�� */
typedef struct
{
	DEF_CMD_HEADER;
	int echo; // 0,�ɹ�; -1, ʧ��
} STRUCT_S_MD_REGIN_RESP;

//��ȡ�ƶ��������
#define CMD_G_MD_REGIN 0x9046

/* ����� */
typedef struct
{
	DEF_CMD_HEADER;
} STRUCT_G_MD_REGIN_REQ;

/* ��Ӧ�� */
typedef struct
{
	DEF_CMD_HEADER;
	P2P_MD_REGION_CHANNEL region;
} STRUCT_G_MD_REGIN_RESP;

//���Ե��ת��

#define CMD_S_MOTOR_TEST 0x9033

/* ����� */
typedef struct
{  
	DEF_CMD_HEADER;  
	char mode; // 0, ֹͣ;1,ת��
} STRUCT_S_MOTOR_TEST_REQ; 

/* ��Ӧ�� */
/*
typedef struct
{  
	DEF_CMD_HEADER;  
	int echo; // 0, �ɹ�;-1,ʧ��
} STRUCT_S_MOTOR_TEST_RESP; 
*/

#define CMD_S_NIGHT_SWITCH_TEST 0x9034 

/* ����� */
typedef struct
{  
	DEF_CMD_HEADER;  
       char status; // 0, ��;1, ��
} STRUCT_S_NIGHT_TEST_REQ; 

/* ��Ӧ�� */
typedef struct
{
	DEF_CMD_HEADER;
	int echo;//0 �ɹ���-1 ʧ��
} STRUCT_S_NIGHT_TEST_RESP;

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif

