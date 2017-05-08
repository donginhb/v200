
#ifndef __SYSTEM_PARAMETER_STRUCT_H__
#define __SYSTEM_PARAMETER_STRUCT_H__
#include "common.h"

typedef struct 
{

	//*8-15 bits����
	//#define		REL_SPECIAL_COM		0x00000000
	//*16-23 bits �����豸����00:IPC CMOS VGA 01:IPC CMOS 720P 02:IPC CMOS 1080P 03 IPC CCD 04:DVR 05:NVR
	//#define		REL_VERSION_COM		0x00000000
	//*24-31��ʾоƬ����
	//#define		CHIP_TYPE_HI3511	0x50000000	//3507/3511оƬ
	//#define		CHIP_TYPE_HI3515	0x52000000
	//#define		CHIP_TYPE_HI3520	0x54000000
	//#define		DEV_TYPE_INFO		CHIP_TYPE_HI3511+REL_VERSION_COM+REL_SPECIAL_COM+MAX_REC_CHANNEL

	int				DeviceType;				//�豸����DEV_TYPE_INFO
	char			DeviceName[32];			//�豸����
	char			SerialNumber[32];		//MAC��ַ
	char			HardWareVersion[16];		//Ӳ���汾
	char			MCUVersion[16];			//��Ƭ���汾
	char			SoftWareVersion[16];		//����汾
	unsigned int	LocalDeviceCapacity;		//����λ, bit0��ʾ�Ƿ�֧���ƶ�������������½ӿ�(ͨ���������)
	char			reserved[12];
	char			VideoNum;				//��Ƶͨ����
	char			AudioNum;				//��Ƶͨ����
	char			AlarmInNum;			//��������
	char			AlarmOutNum;			//�������
	char			SupportAudioTalk;		//�Ƿ�֧�ֶԽ�1:֧��0:��֧��
	char			SupportStore;			//�Ƿ�֧�ֱ��ش���1:֧��0:��֧��
	char			SupportWifi;				//�Ƿ�֧��WIFI 1:֧��0:��֧��
	char			resver;					//����
	
}TYPE_DEVICE_INFO;


typedef struct 
{
	unsigned char		       	m_uCenterIP[4];   	// ���ķ�����IP
	unsigned char				m_Switch;			// �Ƿ�����ƽ̨
	unsigned char				deviceid[15];		// ƽ̨ע��ID
	char						passwd[16];					// ƽ̨�û�����
	unsigned int				m_heartbeat;				// �������ʱ��(s)
	unsigned short			m_uPhoneListenPt; 	// �ֻ������˿�(1024-65535)
	unsigned short			m_uVideoListenPt; 	// ��Ƶ�����˿�		(1024-65535)
	unsigned short			m_uHttpListenPt;  	// http �����˿�(1024-65535  80)
	unsigned short			m_uEnrolPort;		// ע��������˿�(1024-65535)
	
}TYPE_CENTER_NETWORK;

typedef struct 
{
	unsigned char				m_uLocalIp[4];		// ����ip ��ַ
	unsigned char				m_uMask[4];			// ��������
	unsigned char				m_uGateWay[4];		// ����
	unsigned char				m_uMac[6];			// MAC��ַ
	unsigned char				m_dhcp;				// DHCP����1:��0:��
	unsigned char				m_upnp;				// upnp����1:��0:��
	unsigned short			m_v_port;			// ��Ƶӳ��˿�(1024-65535)
	unsigned short			m_http_port;		// HTTPӳ��˿�(1024-65535)
	unsigned short			m_plat_port;		// ƽ̨ӳ��˿�(1024-65535)
	unsigned short			m_phone_port;		// �ֻ�ӳ��˿�(1024-65535)
}TYPE_NETWORK_DEVICE;


typedef struct
{
	unsigned char				m_umDNSIp[4]; 		// �� DNS
	unsigned char				m_usDNSIp[4];		// ���� DNS
}TYPE_NETWORK_DNS;

typedef struct 
{	
	unsigned char				m_ntp_switch;				// NTP��ʱ����
  	char						m_Reserved[3];				
}TYPE_NETWORK_NTP;

typedef struct
{
	unsigned int         	m_uPppoeIP[4]; 	// ��δʹ��
	char					m_s8UserName[32];
	char					m_s32Passwd[16];
	unsigned char			m_u8PppoeSelected;	//�Ƿ�����PPPOE 1:on 0:off
	unsigned char			m_u8Reserved[3];

}TYPE_PPPOEPARA;

typedef struct
{
	unsigned char 			m_u8NatIpAddr[4];
	unsigned char       	m_u8NatIpValid;
	unsigned char			m_u8Reserved[3];
}TYPE_NATSETPARA;

typedef struct
{
	char					m_s8Name[32];		// ����
	char					m_s8UserName[16];
	char					m_s32Passwd[16];		
	unsigned char			m_u8Selected;		//	�Ƿ�����1:����0:������
	unsigned char			m_server;			// 1:��ʾ3322.org, 2:��ʾdynDDNS.org
	unsigned char			m_u8Reserved[6];
	
}TYPE_DYNAMICDOMAIN;

typedef struct
{
	char					m_server[32];		// ��������ַ
	char					m_account[32];		// �ʻ�
	char					m_password[16];		// ����	
	int					m_port;				//�˿�
}TYPE_FTP;

typedef struct
{
	char					m_title[64];			//����
	char					m_server[32];		// ��������ַ
	char					m_addr[32];			// ���������ַ
	char					m_account[32];		// ���������ַ
	char					m_password[16];		// ����
	int						m_mode;				// ��֤ģʽ
	int						m_u8Sslswitch;		//�Ƿ�����SSL 1:����0: ������
	int						m_u16SslPort;		//�˿�
}TYPE_EMAIAL;

typedef struct
{
	unsigned char			m_u8Selected;		//WIFI�Ƿ�����1:on 0:off
	unsigned char			m_dhcp;			//dhcp�Ƿ�����1:on 0:off
	unsigned char			m_uLocalIp[4];		// ip ��ַ
	unsigned char			m_uMask[4];			// ��������
	unsigned char			m_uGateWay[4];		// ����
	unsigned char			m_uMac[6];			// MAC��ַ
	unsigned char			m_umDNSIp[4]; 		// �� DNS
	unsigned char			m_usDNSIp[4];		// ���� DNS
	
}TYPE_WIFI_ADDR;

typedef struct
{
			 char				RouteDeviceName[32];	//�ȵ�����
	         char				Passwd[32];				//����
	unsigned char				AuthenticationMode;		//��֤ģʽ
	unsigned char				EncryptionProtocol;		//����Э��
	unsigned char				Index;					//ͨ��
	unsigned char				SignalLevel;				//�ȵ�ǿ��(�ͻ�����Ҫ��255�õ��ٷֱ�)1-255
	unsigned char				ConnectStatus;		//����״̬0:δ����1:������ 2:���Ӵ���
	unsigned char				WepKeyMode;     //0:ASCII 1:Hex
	         char               lang:3;		// 0:english 	1:chinese 		2:������		 3:xxx,............. 
			 char				Smartlink:1;//    1 :smartlink 0:normal
			 char				res:4;
			 char               m_Reserved;
	//unsigned int                smart_flag;		 
}TYPE_WIFI_LOGIN;

typedef struct
{
	TYPE_WIFI_ADDR		WifiAddrMode;		//�豸��ȡ��ַ��ʽ
	TYPE_WIFI_LOGIN		LoginWifiDev;		//��½WIFI�ṹ
	
}TYPE_WIFI_DEVICE;


typedef struct 
{
	TYPE_CENTER_NETWORK			m_CenterNet;  	// λ0 ƽ̨�������˿�����
	TYPE_NETWORK_DNS			m_DNS;    		// λ1 DNS ����������
	TYPE_NETWORK_DEVICE			m_Eth0Config;  	// λ2 ������һ���ڼ�ӳ��˿�����
	TYPE_NETWORK_DEVICE			m_Eth1Config;  	// λ3 �����ڶ������ڵ�����
	TYPE_PPPOEPARA				m_PppoeSet;		// λ4 PPPOE
	TYPE_NATSETPARA				m_NatConfig;	// λ5Ԥ���ṹ��
	TYPE_DYNAMICDOMAIN			m_DomainConfig;	// λ6   DDNS����
	TYPE_FTP					m_ftp;			// λ7 FTP�ϴ�
	TYPE_EMAIAL					m_email;		// λ8 �������
	TYPE_NETWORK_NTP			m_NTP;			// NTP��ʱ
	TYPE_WIFI_DEVICE			m_WifiConfig;	//wifi
	unsigned int				m_changeinfo;	// ����ָʾ ��Ӧbit0-bit10 1:�и���0:û�и���

}NETWORK_PARA;

typedef struct 
{
	unsigned int 		m_uAdminPassword;		// ����Ա����
	unsigned int 		m_uOperatePassword;		// ����Ա����
	unsigned int 		m_uCurrentPwd;			// ��ǰ��¼������
	unsigned char		m_uPwdEnable;			// ���빦�ܴ�0: û�����룬1: ������
	unsigned char		m_Reserved[19];			// ���� 23->19
	unsigned int		m_changeinfo;			// ����ָʾ 0�޸��£�1�и���
}PASSWORD_PARA;


typedef struct
{
	unsigned short 		m_u16StartTime;			// ʱ������Ƭ�εĿ�ʼʱ�� 60xh+m
  	unsigned short		m_u16EndTime;			// ʱ������Ƭ�εĽ���ʱ�� 60xh+m
   	unsigned char			m_u8Valid;				// ʱ����Ƿ���Ч
	unsigned char			m_u8Reserved;
	unsigned short		m_validChannel;			// ��Ӧ�Ķ�ʱ��ͨ����Чλ
}TIMETBLSECTION;

typedef struct
{
	unsigned short		m_uTimeTbl[8];			// ʱ���	4��ʱ���,��λ:minute
	unsigned short		m_uMdTbl[8];			// �ƶ����
	unsigned short		m_uAlarmTbl[8];			// �ⲿ����
	unsigned char			m_uWeekDay;			// week date  (0--8):sun--every--*****
	unsigned char			m_uTrigeType;			// ¼�񴥷�����:	0 TIMER , 1 SEN, 2 EVENT
	unsigned char			m_Reserved[6];
}RECTIMERS_PARA;

typedef struct 
{
	RECTIMERS_PARA		m_TimerTask[8];			// ��ʱ¼����������
	unsigned char			m_uTimerSwitch;			// ʱ���¼���Ƿ���
	unsigned char 		m_uPowerRecEnable;		// �Ƿ��п���¼��
	unsigned char			m_u8PreRecordTime;
	unsigned char			m_u8RecordDelay;
	unsigned char			m_Reserved[8];
}REC_TASK_PARA;

typedef struct
{
	TIMETBLSECTION		m_uTimeTbl[4];			// ʱ���	4��ʱ���,��λ:minute
	TIMETBLSECTION		m_uMdTbl[4];			// �ƶ����
	TIMETBLSECTION		m_uAlarmTbl[4];			// �ⲿ����
	TIMETBLSECTION		m_uAorMTbl[4];			// �ⲿ�������ƶ����
	unsigned char		m_uWeekDay;				// ��CSδʹ�� week date  (0--8):sun--every--*****
	unsigned char		m_uTrigeType;			// ��CSδʹ�� ¼�񴥷�����:	0 TIMER , 1 SEN, 2 EVENT
	unsigned char		m_Reserved[6];
}RECSCHEDULE_PARA;

typedef struct 
{
	RECSCHEDULE_PARA	m_TimerTask[8];			// ��ʱ¼����������: ����-����
	unsigned char		m_uTimerSwitch;			// ʱ���¼���Ƿ���
	unsigned char 		m_uPowerRecEnable;		// �Ƿ��п���¼��
	unsigned char		m_u8PreRecordTime;//δ��
	unsigned char		m_u8RecordDelay;//δ��
	unsigned char		m_u8PreRecordTimeValid; //Ԥ¼��־
	unsigned char		m_u8RecordDelayValid;	//��ʱ��־
	unsigned char		m_Reserved[6];
}RECORDTASK; // ÿ��ͨ����¼������

typedef struct
{
	RECORDTASK			m_ChTask[CHANNEL];  		// ÿ��ͨ����¼������
	unsigned int		m_changeinfo;		// ����ָʾ: λ0-31��Ӧ32��ͨ�� 0-�޲�������
	
}GROUPRECORDTASK;

typedef struct
{
	unsigned int 			m_uMachinId;		// ��������
	unsigned char 			m_uTvSystem;		// ϵͳ��ʽ:1 NTSC, 0  PAL
	unsigned char 			m_uHddOverWrite;	// �Զ����ǿ����Ƿ��: 0 no, 1 yes
	unsigned char 			m_uHddorSD;			// Ӳ�̻���SD��¼�����ı�����ϵͳ0:HDD 1:SD
	unsigned char			m_uCursorFb;		// ==0 hifb0, !=0 hifb2
	unsigned char			m_uOutputMode;		// 0:VGA  1:HDMI
	unsigned char           m_SyncIpcTime;  //ͬ��IPCʱ��
	unsigned char           m_HvrMode;
	unsigned char 			m_Reserved[5];		
	unsigned int 			m_serialId;			// �������кţ�ʵ���Ǳ���������������MAC��ַ
	unsigned int			m_ChipType;			// оƬ����:3520 3511 3512
	unsigned int			m_changeinfo;		// ����ָʾ 0�޸��� 1�и���
}MACHINE_PARA;

typedef struct 
{
	unsigned char 			m_uChannelValid; 	// ��������Ƶͨ������1:on  0:off
	unsigned char				m_uAudioSwitch;	// ��������Ƶͨ������1:on  0:off
	unsigned char				m_uSubEncSwitch; // ��������Ƶ����1:on 0:off
	unsigned char				m_uSubAudioEncSwitch; // ��������Ƶ����1:on 0:off
	char 					m_Title[17];	 	 // ͨ������
	unsigned char 			m_uFrameRate;	 // ֡��	PAL(1-25) NTSC(1-30)
	unsigned char 			m_uResolution;	 // ������  0:D1, 1: HD1, 2:CIF 3:QCIF 4:1080P 5:720P 6:VGA 7:QVGA 8:960H
	unsigned char 			m_uQuality;		 // ����quality	values:0--4 (���)(�ܺ�)(��)(һ��)(��)
	unsigned char 			m_uEncType;	// 1:CBR 0:VBR
	unsigned char			m_uSubRes;  	 // ������������0:D1, 1: HD1, 2:CIF 3:QCIF 4:1080P 5:720P 6:VGA 7:QVGA
	unsigned char			m_uSubQuality; 	 // ���������� values:0--4 (���)(�ܺ�)(��)(һ��)(��)	
	unsigned char			m_uSubFrameRate; // ������֡�� PAL(1-25) NTSC(1-30)
	unsigned char 		m_uSubEncType;// 1:CBR 0:VBR
	unsigned char			m_u16RecDelay;	 // ¼����ʱ
	unsigned char			m_u8PreRecTime;  // Ԥ¼ʱ��
	unsigned char			m_TltleSwitch;	 // ͨ�����⿪��0:off 1:on
	unsigned char			m_TimeSwitch;	 // ʱ����⿪��0:off 1:on
	unsigned char			m_u8Reserved[3];
}CHANNEL_PARA;

typedef struct 
{
	CHANNEL_PARA			m_ChannelPara[CHANNEL];  // λ0-31 ͨ���������	
	unsigned int			m_changeinfo;		// ����ָʾ: λ0-31��Ӧ16·CHANNEL_PARA��
}CAMERA_PARA;

typedef struct
{
	unsigned short		m_u16X;/*X ��ʼ����  0 - 720*/
	unsigned short		m_u16Y;/*y ��ʼ���� 0---480*/
	unsigned short		m_u16Width;/*��������Ŀ��*/
	unsigned short		m_u16Height; /*��������ĸ߶�*/
	unsigned char		m_u8OverValid; /*�����Ƿ���Ч1:on 0:0ff*/
	unsigned char		m_u8Reserved[7];
}OVERLAYSET;

typedef struct
{
	unsigned short		m_u16X;  /*X ����  0 - 720*/
	unsigned short		m_u16Y;  /*y ���� 0---480*/
	unsigned char		m_u8OverValid; /*�����Ƿ���*/
	unsigned char		m_u8Reserved[7];		
}CHARINSERTSET;

typedef struct
{
	//ע��:һ��ͨ�����ĸ����򿪹�һ��
	OVERLAYSET			m_CoverLay[CHANNEL];  	/* �ڸǲ�1 */		
	OVERLAYSET			m_CoverLay2[CHANNEL];  	/* �ڸǲ� 2*/	
	OVERLAYSET			m_CoverLay3[CHANNEL];  	/* �ڸǲ�3 */	
	OVERLAYSET			m_CoverLay4[CHANNEL];  	/* �ڸǲ�4 */	
	unsigned int			m_changeinfo1;		// ����ָʾ: ��Ӧ0 
	unsigned int			m_changeinfo2;		// ����ָʾ: ��Ӧ1
	unsigned int			m_changeinfo3;		// ����ָʾ: ��Ӧ2 
	unsigned int			m_changeinfo4;		// ����ָʾ: ��Ӧ3
}VIDEOOSDINSERT;

typedef struct
{
	int 			m_nBrightness; 		// ǰ������ͷ���� value_range:0~255 */
	int 			m_nContrast;  		// ǰ������ͷ �Աȶ�value_range��0~255 */
	int				m_nSaturation;  	// ǰ������ͷ���Ͷ�value_range��0~255 */
	int 			m_nHue;  			// ǰ������ͷɫ��value_range��-128~127 */
}ANALOG_CHANNEL;

typedef struct 
{
	ANALOG_CHANNEL		m_Channels[CHANNEL]; 	// ����ͨ��ǰ��ģ��������
	unsigned int		m_changeinfo;		// ����ָʾ: λ0-31��Ӧ32��ͨ��
}CAMERA_ANALOG;

typedef struct
{
	unsigned short 			m_nAlarmRecDelay; 	// ����¼����ʱ30 -100
//	unsigned short 			m_nAlarmOutDelay;	// ���������ʱ30 -300
//	unsigned short 			m_nBuzzeDelay; 		// ����������ʱ 	
	unsigned short 			m_nPreRecTime; 		//Ԥ¼ʱ��(0--20s)
	unsigned char 			m_Reserved[28];
}ALARM_PARA;

typedef struct 
{
	char					m_cTitle[9];// IO  ����������
	char					m_Reserved[3];
}IO_SENSOR_TITLE;

typedef struct 
{
	IO_SENSOR_TITLE		 	m_cTitle[9];// ����������							
	unsigned short  		m_uSwitch;	// ��������0 �ر� 1����			
	unsigned short  		m_uInMode;	// ����ʱ�ⲿ�����״̬��0 ���գ�1 ����
	unsigned short  		m_uAlarmSwitch;		// �Ƿ񱨾�				
	unsigned short			m_uLogSwitch;		// �Ƿ��¼��־0 ����¼ 1 ��¼				
	unsigned short 			m_uLockSwitch;		// ����¼���Ƿ������0 ���ӣ�1����
	unsigned char			m_Reserved[10];
}SENSOR_IO;	

typedef struct 
{
	SENSOR_IO				m_SensorIO;   // IO ��������
}SENSOR_PARA;


typedef struct
{
	TIMETBLSECTION		m_TBLSection[4]; // ʱ���
	unsigned char		m_u8WeekDay;	 // ���ڼ�
	unsigned char		m_u8Reserved[3];
}ALARMINDATESET;

typedef struct
{
	ALARMINDATESET		m_TimeTblSet[8];    /* ÿ��һ���������ڼ���*/
	unsigned int			m_u32AlarmOutSel;	// ÿһλ����һ�������Ч0  ��Ч  1  ��Ч
	unsigned int			m_u32RecSel;		// ÿһλ����һͨ��¼���Ƿ���Ч��0  ��Ч 1 ��Ч
	unsigned int			m_u32AlarmHandle;	// ÿһλ������ʽ bit0   ��������     bit1     ����           bit2  �ϴ�����
	unsigned char 		m_u8TblValid;		// �Ƿ�����ʱ���
	unsigned char			m_u8AlarmValid;	// �����Ƿ���Ч
	unsigned char			m_u8TrigeType;		/* ����������ƿ��0 ����   1 ����*/
	unsigned char			m_u8Reserved;		// ����5->1
}ALARMINSET;

typedef struct
{
	ALARMINSET			m_AlarmIn[4];
	unsigned int		m_changeinfo;		// ����ָʾ: λ0-7��Ӧ8· ALARMINSET
}GROUPALARMINSET;

typedef struct 
{
	unsigned char		m_uPowerOffSwitch; 	// �Զ��ػ������Ƿ���
	unsigned char 	m_uStartupMode;  	// 0: �ر� 1:��ʱ������2:��Կ�׿�������ʱ�ػ�
	unsigned short	m_uShutDelay; 		// �ػ�����ʱʱ��,����Ϊ��λ��
	unsigned char		m_uStartupHour; 	// ����ʱ��Сʱ
	unsigned char		m_uStartupMin; 		// ����ʱ�䣬���ӣ�
	unsigned char		m_uStartupSec;
	unsigned char		m_uShutDownHour; 	// ��ʱ�ػ�ʱ�䣬Сʱ
	unsigned char		m_uShutDownMin; 	// ��ʱ�ػ�������
	unsigned char		m_uShutDownSec; 	// ��ʱ�ؽ���
	unsigned char 	m_reserved[14];		// ���ݶ��� 30 -> 14
	unsigned int		m_changeinfo;		// ����ָʾ: 0-�޸��� 1-�в�������
}POWER_MANAGE;

typedef	 struct
{
	unsigned char 		m_uDateMode;		// ������ʾ�ķ�ʽ	0 ��ʾdd-mm-yyyy ,1��ʾyyyy-mm-dd,2��ʾmm-dd-yyyy
	unsigned char		m_uTimeMode;		// ʱ��ĸ�ʽ:0 ��ʾ24 hours, 1 ��ʾam/pm
	unsigned char		m_uFilePacketLen;	// ¼���ļ��Ĵ��ʱ��: 0:15, 1:30, 2:45, 3:60(MIN)
	unsigned char		m_uWeekDayStart;	// �޹��� �����տ�ʼ: 0--6�ֱ��ʾ��sunday--saturday 
	unsigned char 		m_uWeekDayEnd;		// �޹��� �����յĽ���: 0--6�ֱ��ʾ��sunday--saturday 	
	unsigned char		m_uIdleTime;		// �޹��� system->date/time->idie time:0--9�ֱ��ʾ0.5, 1, 5, 10	MIN
	unsigned char		m_uTimeInSert;		// �޹��� ʱ�Ӳ���
	unsigned char		m_uTimeSyncMode;	// �޹��� ʱ��ͬ��ģʽ
	unsigned long		m_uManualRecValid;	// �޹��� �ֶ�¼���Ƿ���Чÿһλ����һ��ͨ��0:��Ч 1 ��Ч
	unsigned char		m_date_mode;		// ���ڷָ���: - | / ����
	unsigned char		m_self_start;		// �޹��� ����������
	unsigned char		m_screen_save;		// �޹��� ��Ļ����
	unsigned char		m_language;			// ����ѡ��: 0-���� 1-Ӣ��2-�ݿ���3-����4-����5-����6-��������7-����8-�������9-��������10-��������
	unsigned char		m_uDiaphaneity;		// ͸���� 1-7
	unsigned char 		m_uDecodeMode; 	//0:CIF 1:HD 2:D1
	unsigned char 		m_acceptAlarmMsg;  //0:�����ձ���1:���ձ���
	unsigned char			m_allowNvrAutoAddDevice;	//0:���Զ�����豸��1:ÿ�ο������Զ���������豸/*jackzhou 0703*/
	unsigned char		m_Startup_Wizard;        // ��½������0 - ����ʾ������Ϣ��1 - ÿ�ο�������ʾ������Ϣ   /* ltt 20121022 */
	unsigned char 		m_Reserved[3]; 		// δ�� 8->7->6->5
	unsigned int			m_changeinfo;		// ����ָʾ 0-�޲������� 1-�в�������
}COMMON_PARA;
typedef struct 
{	// �ƶ���������봦��
	ALARMINDATESET		m_TimeTblSet[8];    /* ������ʱ���*/
	unsigned int			m_uBlindSwitch;		// ��Ƶ�ڵ����� -- 1:����,0:�ر�
	unsigned int			m_uBlindSensitive;		// ��Ƶ�ڵ�������/*4���ȼ�:0-�ߣ�1-�ϸߣ�2-�У�3:�ϵ�.4:��*/
	unsigned int  			m_uBlindAlarm;			// �������ÿһλ����һ�����
	unsigned int 			m_uOutputDelay;		//�����ʱ		
	unsigned int			m_uAalarmOutMode;  	// ÿһλ������ʽ bit0   ��������   bit1  ץ��   bit2  FTP�ϴ�bit3 �����ʼ�bit4 ��Ļ��ʾbit5¼��
}CNANNEL_SETUP;
typedef struct
{
	CNANNEL_SETUP	m_Channel[CHANNEL];	// 
	unsigned int		m_changeinfo;		// ����ָʾ:λ0-32��Ӧ32·������
}CAMERA_BLIND;  // ����ͷ�ڸ�

typedef struct 
{	// �ƶ���������봦��
	ALARMINDATESET		m_TimeTblSet[8];    /* ������ʱ���*/
	unsigned int			m_uVideoLossSwitch;		// ��Ƶ��ʧ���� -- 1:����,0:�ر�
	unsigned int  			m_uBlindAlarm;			// �������ÿһλ����һ�������
	unsigned int 			m_uOutputDelay;		//�����ʱ		
	unsigned int			m_uAalarmOutMode;  	// ÿһλ������ʽ bit0   ��������   bit1  ץ��   bit2  FTP�ϴ�bit3 �����ʼ�bit4 ��Ļ��ʾbit5¼��
}CNANNEL_VideoLoss;
typedef struct
{
	CNANNEL_VideoLoss	m_Channel[CHANNEL];	// 
	unsigned int			m_changeinfo;		// ����ָʾ:λ0-32��Ӧ32·
}CAMERA_VideoLoss;  //��Ƶ��ʧ

typedef struct
{
	unsigned int	status;			//1:����0:�ر�
}AlarmPort;

typedef struct
{
	AlarmPort				m_uPort[ALARMOUTPORTNUM];
	unsigned int			m_changeinfo;		// ����ָʾ 0-�޲������� 1-�в�������
}AlarmOutPort;

typedef struct 
{   // �ƶ������������
//	unsigned short	 		m_uMDRaw[12]; 	// �ƶ��������ֳ���16x12���ķ���
	unsigned short			m_x;			// �������ϵ�X����
	unsigned short			m_y;			// ��������Y����
	unsigned short			m_width;		// ����Ŀ��
	unsigned short			m_height;		// ����ĸ߶�
}MD_AREA;

typedef struct
{
	unsigned char			m_u8PresetEn;   	// ����Ԥ�õ� 0 : �ر�  1  ����
	unsigned char			m_u8PresetId;  		// Ԥ�õ���
	unsigned char			m_u8CruiseEn; 		// ����Ѳ��   0   �ر�    1   ����
	unsigned char			m_u8TrackEn;  		// ���ù켣�ر�     1    ����
	unsigned char			m_u8TrackId;  		// �켣���
	unsigned char			m_u8CruiseId;		// Ѳ�����
	unsigned char			m_u8Reserved[2];
	
}PtzLinkAction;

typedef struct 
{	// �ƶ���������봦��
	ALARMINDATESET		m_TimeTblSet[8];    //m_TimeTblSet[0]:����m_TimeTblSet[1]:��һ����
	unsigned char			m_MDMask[20*15];//�����ƶ�����
	unsigned char			m_uAlarmInterval;//����������趨ֵ����1���ӡ�5���ӡ�15���ӡ�30���ӡ�1Сʱ������ʱ������Ĭ��ֵ�趨Ϊ1����,�ֱ��Ӧ:1,5,15,30,60,0 min
	unsigned int			m_uMDSwitch;		// �ƶ�����鿪�� -- 1:����,0:�ر�
	unsigned int			m_uMDSensitive;		// �ƶ����������/*4���ȼ�:0-�ߣ�1-�ϸߣ�2-�У�3:��*/
	unsigned int  			m_uMDAlarm;			// �������ÿһλ����һ�����
	unsigned int 			m_uOutputDelay;		//����������趨ֵ����1���ӡ�5���ӡ�15���ӡ�30���ӡ�1Сʱ������ʱ������Ĭ��ֵ�趨Ϊ1����,�ֱ��Ӧ:1,5,15,30,60,0 min
	unsigned int			m_uAalarmOutMode;  	// ÿһλ������ʽ bit0   ��������   bit1  ץ��   bit2  FTP�ϴ�bit3 �����ʼ�bit4 ��Ļ��ʾbit5¼��bit6 �Ƿ��ϴ�������Ϣ
}MD_SETUP;
typedef struct
{
	MD_SETUP			m_Channel[CHANNEL];
	unsigned int		m_changeinfo;		// ����ָʾ: λ0-31��Ӧ32·ͨ��
}CAMERA_MD;

typedef struct 
{
	char				m_cVoltageDate[16];   	// ����
	char				m_cVoltageTime[12]; 	// ʱ��
	unsigned char		m_uVoltageValues;   	//��ѹֵ		
	char				m_Reserver[7];	
	
}SystemVoltagesInfo;

typedef struct 
{
	unsigned char 		m_uTempeatureValues; 	// �¶�ֵ
	char				m_uTempeatureDate[16];	// ����
	char				m_uTempeatureTime[12];  // ʱ��
	char				m_Reserved[7]; 
	
}SystemTempeatures;


typedef struct 
{
	SystemVoltagesInfo		High_VoltagesInfo;		// ��ѹ��Ϣ		
	SystemVoltagesInfo		Low_VoltagesInfo;		// ��ѹ��Ϣ		
	SystemTempeatures		HighTempeaturesInfo;	// ������Ϣ		
	SystemTempeatures		Low_TempeaturesInfo;	// ������Ϣ		
	unsigned long			TotalRecordTime;		// ¼�����ʱ��
	unsigned short			HighSpeed;			    // ����ٶ�
	unsigned char			m_uFirstRec;
	char					m_Reserved[17];			//18
}SYSTEM_RUNINFO;							

typedef struct 
{
    char  				m_cCommName[16]; /*��������*/
    unsigned char    	m_u8Databit;     /*����λ*/
    unsigned char    	m_u8Stopbit;     /*ֹͣλ*/
    unsigned char    	m_u8Parity;      /*У��λ*/
    unsigned char    	m_u8BaudRate;    /*������*/
    unsigned char		m_Reserved[4];	
}PTZ_ComInfo_S;

typedef struct 
{
   unsigned char   	 	m_s32Chn;        /*��̨��Ӧ��Ƶͨ��*/
   unsigned char    		m_u8Addr;        /*��̨�߼���ַ(0-255)*/
   unsigned char  	 	m_u8Protocol;    /*��̨Э��0---pelco-d  1---pelco-p  */
   unsigned char	 	m_u8BautRate; 	 /* ������ 0 ---1200  1--2400  2--4800  3---9600  4--19200 5--38400  6---57600  7--115200*/
   unsigned char	 	m_u8Speed;		 /*ת���ٶ�1-64*/
   unsigned char    	m_u8Databit;     /*����λĬ��Ϊ8 λ5-8*/
   unsigned char    	m_u8Stopbit;     /*ֹͣλĬ��Ϊ1 λֹͣλ1-2*/
   unsigned char    	m_u8Parity;      /*У��λ0 : û��У�飬 1 ��У�� 2 żУ��*/ 
 }PTZ_Info_S;

typedef struct
{
	PTZ_Info_S 		m_ptz_channel[CHANNEL];
	unsigned int		m_changeinfo;		// ����ָʾ: λ0-31��Ӧ32·ͨ��

}PTZ_PARA;

typedef struct
{
	unsigned char		m_u8Alpha;			// ��Ƶ���͸���� 0��1��2
	unsigned char		m_u8VGAMode;		// VGA��ʾģʽ0 1280*720   1:1024*768     2:1280*1024     3:1920*1080
	unsigned char		m_u8CruiseInterval;	// ������ѯ���
	unsigned char		m_u8AlarmCruiseInterval;  // ������ѯ ���
	unsigned char		m_u8CruiseMode;		// ��ѯģʽbit1 ��ͨ��  bit2 ��ͨ��  bit39 ͨ�� bit4 16 ͨ��
	unsigned char 		m_u8CruiseSwitch; //ȫ�ֿ���
	unsigned char	       m_u8All;                  // view1��view4 �Ƿ�ͬʱѡ�� 0 �� 1 ��                                        ltt 20101022
	unsigned char		m_pad0;			// 
	unsigned int		m_u32SChValid; 		// ÿλ����һ����Ч 0  ��Ч   1    ��Ч
	unsigned int		m_u32FourValid;		// ÿλ����һ����Ч 0  ��Ч   1    ��Ч   bit0-4:first four-second four  bit4 first nine ,bit5 secnine
	unsigned int		m_changeinfo;		// ����ָʾ: 0-�޲�������   1-�в�������  
	
}VIDEODISPLAYSET; 

typedef struct
{
	char				m_pcSnapDir[96];
	char				m_pcRecDir[96];
	char				m_pcdLoadDir[96];
	unsigned char		m_u8Reserved[96];	// ����
	unsigned int		m_changeinfo;		// ����ָʾ:λ0-3��Ӧ

}PCDIR_PARA;

typedef struct 
{
	unsigned char		pelcoP_enterMenu[CHANNEL][4];	// �˵���ʾ
	unsigned char		pelcoP_runpattern[CHANNEL][4];	// ����ɨ��
	unsigned char		pelcoP_cruiseOn[CHANNEL][4];		// �Զ�Ѳ��
	unsigned char		pelcoP_autoScan[CHANNEL][4];		// �Զ�ɨ��
	unsigned char		pelcoP_stopScan[CHANNEL][4];		// �Զ�ɨ��ֹͣ
	unsigned char		pelcoP_pRever1[CHANNEL][16];		// ����4������

	unsigned char		pelcoP_default1[9][4];		// Ĭ��ֵ1
	unsigned char		pelcoP_default2[9][4];		// Ĭ��ֵ2
	unsigned char		pelcoP_default3[9][4];		// �û��Զ���Ĭ��ֵ3

	unsigned char		pelcoD_enterMenu[CHANNEL][4];	// �˵���ʾ
	unsigned char		pelcoD_runpattern[CHANNEL][4];	// ����ɨ��
	unsigned char		pelcoD_cruiseOn[CHANNEL][4];		// �Զ�Ѳ��
	unsigned char		pelcoD_autoScan[CHANNEL][4];		// �Զ�ɨ��
	unsigned char		pelcoD_stopScan[CHANNEL][4];		// �Զ�ɨ��ֹͣ
	unsigned char		pelcoD_pRever1[CHANNEL][16];		// ����4������

	unsigned char		pelcoD_default1[9][4];		// Ĭ��ֵ1
	unsigned char		pelcoD_default2[9][4];		// Ĭ��ֵ2
	unsigned char		pelcoD_default3[9][4];		// �û��Զ���Ĭ��ֵ3

	unsigned int		m_changeinfo[16];			// ����ָʾ: λ0-11 and 16-27

}PELCO_CmdCfg;

typedef struct 
{
	ANALOG_CHANNEL		m_picChn[3];  	  // ���鶨ʱ����Ӧ����ɫֵ
	TIMETBLSECTION		m_picTmr[3];	  // ��ʱ��
	ANALOG_CHANNEL		m_picChnDefault[3][3];  // Ĭ��ֵ
	TIMETBLSECTION		m_picTmrDefault[3][3];  // Ĭ��ֵ

	unsigned int		m_changeinfo;  	  // ����ָʾ: λ0-2��Ӧ3����ɫֵ��λ3-5��Ӧ��ʱ����
										  // λ6-8��Ӧ3��Ĭ����ɫ, λ9-11��Ӧ���鶨ʱ��Ĭ��ֵ
}PICTURE_TIMER;

typedef struct 
{
	ANALOG_CHANNEL		m_SDPic; 		  // SDģ��������
	ANALOG_CHANNEL		m_HDPic;		  // HDģ��������
	unsigned int		m_changeinfo;	  // ����ָʾ: λ0-1��ӦSD,HD
}VODEV_ANALOG;


typedef struct
{
	char				m_cUserName[16];  // �û���
	char				m_s32Passwd[16];  // �û�����
	int					m_s32UserPermit;  // 0:����Ȩ�� 1:��ͨȨ��
	unsigned char		m_u8UserValid;	  // ��δʹ�� ���û��Ƿ���Ч
	unsigned char		m_u8Reserved[3];
}SINGLEUSERSET;

typedef struct
{
	SINGLEUSERSET		m_UserSet[16];
	unsigned int		m_changeinfo;		// ����ָʾ: λ0-15��Ӧ16���û�
}USERGROUPSET;


typedef struct
{
	unsigned int		m_u16AlarmValid;	// 1:����0:�ر�
	unsigned int		m_u16AlarmMode;  	// ÿһλ������ʽ bit0   ��������     bit1     ����      
}EXCEPTIONHANDLE;


typedef struct
{
	
	EXCEPTIONHANDLE     m_ExceptHandle[8]; 	// 0-7 �쳣����  0 ��: ��Ӳ��  1 : Ӳ�̳��� 2 Ӳ����
	unsigned int		m_changeinfo;		// ����ָʾ: 0-�޸��£�1-�в�������
}GROUPEXCEPTHANDLE;

//ϵͳά��
typedef struct   
{
	unsigned char		m_u8Mode;			// 0-������ά�� ��1-����ά��  2-���������ά��
	unsigned char		m_u8WeekDayValid; 	// ����ЧBIT0-BIT7 ��Ӧ������-����һ
	unsigned char		m_u8DayInterval;  	// ����
	unsigned char		m_u8WeekHour; 		// Сʱ
	unsigned char		m_u8WeekMinute;		// ����
	unsigned char		m_u8WeekSec;		// ��
	unsigned char		m_u8DateModeHour; 	// Сʱ
	unsigned char		m_u8DateModeMinute;	// ����
	unsigned int		m_changeinfo;		// ����ָʾ: 0-�޸��£�1-�в�������
}SYSTEMMAINETANCE;

typedef struct
{

	PtzLinkAction			m_Channel[CHANNEL];

}FangZonePtzLinkSet;

typedef struct
{
	FangZonePtzLinkSet  	m_Zone[MAX_ALARM_ZONE];
	unsigned int			m_changeinfo1;		// 0:û�и���1:�и���
}GroupZonePtzLinkSet;

typedef struct
{
	unsigned char 		m_u8StartHour;		
	unsigned char		m_u8StartMin;
	unsigned char 		m_u8EndHour;
	unsigned char		m_u8EndMin;
	unsigned char		m_u8Valid;     	// ʱ��Ƭ���Ƿ���Ч  0 ��Ч  1 ��Ч
	unsigned char		m_u8DefMode; 	// 1  Ϊ����  0  Ϊ����
	unsigned char		m_u8Reserved[2];
	
}DefenceScheduleSliceSet;

typedef struct
{
	unsigned char		m_u8EncryMode;/*0:unencrypted,1:wep,2:shared wep,3:wpa,4:wpa2,5:wpa+wpa2:*/
	unsigned char 	    m_u8WepKeyIndex;/*0:Wepkey1...*/
	unsigned char		m_u8EncryKeyFormat;/*1:ASCII , 2:HEX*/
	unsigned char		m_u8WpaEncryKeyMode;/*1:TKI , 2:AES*/
	char				m_WifiPassword[32];/*max wifi password len:32*/
}WifiSecurity;

typedef struct
{
	char 			m_u8WifiSsidName[32];/*max wifi ssid len:32*/
	unsigned char 	m_u8WifiSwitchStatus;
	unsigned char 	m_u8WifiNetworkMode;
	unsigned char 	m_u8WifiChannelNumber;
	unsigned char		m_u8Reserved;
	WifiSecurity		m_WifiSecuritySettings;
}ApSettings;

typedef struct
{
	unsigned short		m_StartTim;
	unsigned short		m_EndTim;
	unsigned char 		m_u8StartH;		
	unsigned char			m_u8StartM;
	unsigned char			m_u8StartS;
	unsigned char 		m_u8EndH;
	unsigned char			m_u8EndM;
	unsigned char			m_u8EndS;
	unsigned char			m_u8Valid;
	unsigned char			m_u8DefMode;
	
}SchemeMega;

typedef struct
{
	char					m_uAccDomain[32];	// ע�������������IP
	char					m_deviceid[32];		// ƽ̨ע��ID
	unsigned short		m_MsgPort;			// ��Ϣ�˿�
	unsigned char			m_IsSupportNAT;		// �Ƿ�֧�� NAT ��Խ
	unsigned char			m_PtzLockRet;		// �Ƿ�֧�ֿ�����̨ʱ������̨����״̬ 1��֧�֣�0����֧��
	char					m_devLinkType;		// 1��LAN��2ΪADSL��3Ϊ��������
	char					m_DevMaxConnect;	// ָǰ�˺���������ܹ�֧�ֵ������Ƶ·��
	char					m_reserved[10];		// 80 bytes
	
}NetWorkMegaEyes;

typedef struct
{
	char					m_videoId[32];		// ����ͷ��ҵ�� ID -< Camera
	char					m_channelId;		// ChannelId="1"
	char					m_hasPan;			// �Ƿ�����̨
	char					m_isIPCamera;		// 
	char					m_IsLocalSaved;		// �Ƿ񱾵ش洢
	char					m_lDiskFullOption;	// StopRecord/Overlay
	char					m_SchemeCycle;		// day/week/month
	char 					m_StoreCycle;		// ��󱣴�����
	char					m_streamType;		// ��¼�����������
	char					m_ftpUsr[32];		// ���ʴ洢ʹ�õ�ftp�ʺ��û���
	char					m_ftpPwd[16];		// ���ʴ洢ʹ�õ�ftp�ʺ�����
	char					m_remoteIp[4];		// ���Ĵ洢������IP
	unsigned short			m_remotePort;		// �洢�����������˿ں�
	SchemeMega				m_schemeItem[4];	// �ƻ���
	char					m_reserved[4];		// 96+4 +4= 148 bytes
	
}CamerMegaEyes;

typedef struct
{
	char					r_devId[32];		// ��Ƶ������ҵ��ID -< Message
	char					r_Naming[64];		// ��Ƶ������ȫ�ֱ�ʶNaming -< Message
	unsigned short			r_HeartCycle;		// ����������
	unsigned short			r_SysRebootTime;	// ϵͳ�����ʱ��
	unsigned short			r_SysRebootCycle; 	// ϵͳ�����������
	char					r_PlatformTel[14];	// ��Ѷ���غ���
	char					r_ConfigServer[32];	// ���÷����� 
	char					m_cpuUseRat;		// ���ܸ澯��ֵ֮ CPUʹ����
	char					m_memUseRat;		// ���ܸ澯��ֵ֮ �ڴ�ʹ����
	char					m_diskSpaRat;		// ���ܸ澯��ֵ֮ Ӳ��������
	char					m_reserved1;		// ����
	int						m_diskSpaBalance;	// ���ܸ澯��ֵ֮ Ӳ��ʣ����(MB)
	char					r_reserved[76];		// 380-148 = 232 bytes 
	CamerMegaEyes			r_magaCam;			// ���Ż�ͨĿǰ��Ŀֻʹ��һ·��Ƶ

}NetWorkMegaEcho;

typedef struct
{
	NetWorkMegaEyes			m_netWorkMegaEyes;	// ע�����������ʹ��
	NetWorkMegaEcho			m_netWorkMegaEcho;	// ע�᷵����Ϣ
	
	unsigned int			m_changeinfo;		// ʹ��2λ: 0-�޲������� 1-�в������� 464 bytes
}MegaEyes_PARA;

typedef struct
{
	//1:����0:������
	ALARMINDATESET		m_TimeTblSet[8];    /* ÿ��һ���������ڼ��㲼����ʱ���*/
	unsigned char 		m_u8ZoneEnable;  // �Ƿ�����
	unsigned char			m_u8OSDEnable;   // �Ƿ�����Ļ��ʾ
	unsigned char			m_u8RpCenter;    // �Ƿ��ϱ�����
	unsigned char			m_u8EmailEnable; //	�Ƿ���e-mail
	unsigned int			m_u32UionChannel;  //��̨����0-31����0-31ͨ��
	unsigned int			m_u32RecEnable;	   // ����ʹ��ͨ��¼�� ÿһλ��ʾһ��ͨ����ÿһλʹ��һ��ͨ����Ϊ1��λʹ��¼��
	unsigned int			m_u8ShotEnable;   // ����ʹ��ͨ��ץ�ģ� ÿһλ��ʾһ��ͨ����ÿһλʹ��һ��ͨ����Ϊ1��λʹ��¼��
	unsigned int			m_OutPutPort;	//�����ÿλ����һ�������
	unsigned int			m_VoiceAlarm;//����
	unsigned int			m_DetectTime;//�೤ʱ����һ�δ˲���8��IO��Ҫ����һ������󱣴��Ϊ׼��λ:��
	unsigned int			m_OutputTime;//���������ʱ
	unsigned int			m_uFTP;//�Ƿ�FTP�ϴ�
	unsigned int			m_Mode;//0:����1:����
	
}AlarmZoneSet;

typedef struct
{
	AlarmZoneSet			m_AlarmZone[MAX_ALARM_ZONE];	//16����������
	unsigned int			m_changeinfo1;		// 0:û�и���1:�и���

}ALARMZONEGROUPSET;
typedef struct
{
	unsigned char				m_ntp_switch;				// NTP��ʱ����
	unsigned char               m_idx_tzname;               //ʱ�����Ʊ��±꣬[0,90]
	unsigned char				m_daylight_switch;		    //����ʱ����   
	unsigned char   			m_Res1;
	int                         m_diff_timezone;            //��UTC�����Ĳ�࣬����������Χ[-12*3600,13*3600]
  	unsigned char   			m_Res2[8];	                //������������������ʱ		
}NtpSettings;

typedef struct
{
	char					m_addr1[32];			// ���������ַ���ڶ����ʼ����յ�ַ
    char					m_addr2[32];			// ���������ַ���������ʼ����յ�ַ
}MailExt;

typedef struct
{
	char				m_cMasterName[16];   //��������
	char				m_cMasterPhone[16];  //�ֻ�����
}MasterInfo;

/*260 byte*/
typedef struct
{
	ApSettings  			m_apInfo;       /*72Bytes,NVR ����wifiapp������ipc����*/
    NtpSettings             m_ntp;          /*16Bytes*/
    MailExt                 m_MailExt;      /*�ʼ���չ����*/
    MasterInfo              m_Master;       /*������Ϣ*/
	char 				    m_reserved[72];/*72Bytes*/
	unsigned int			m_changeinfo;   /*4Bytes*/		// ����ָʾ: λ0-7��Ӧ8�� DeScheduleGroup
}PARAMETEREXTEND;

typedef struct
{
	unsigned char  			installerCode[7];
	unsigned char  			usrCode[5];
}PassWord_PARA;

typedef struct
{
	unsigned char 			remoteID[11];  // ң����ID��
	unsigned char 			remoteEnable;  // 0-��ֹ 1-ʹ��
	
}RemoteCTL_PARA;

typedef struct
{
	unsigned char 			zoneID[10];		// 1-8Ϊ���ߣ���ID��
	unsigned char			zoneEnable;
	unsigned char 			BuzzAndType;	// ����λ�������ѷ�ʽ:  ����λ�����������:

}PanelZone_PARA;

typedef struct
{
	unsigned char  			entryDelay;  // ������ʱ
	unsigned char  			exitDelay;	 // �˳���ʱ
	unsigned char  			sirenTime;	 // ����ʱ��
	unsigned char  			ringCycle;   // �������
	unsigned char  			commTestTime; // ͨѶ���ʱ��
	unsigned char  			lossDetectTime;	// ��ʧ��� 0-��ʾ�����
	unsigned char  			armDis_tone;  // ��������ʾ��
	unsigned char  			armDis_report; // ����������
	
}SysOption_PARA;

typedef struct
{
	unsigned char  			cmsPhone[4][16]; // ���ĵ绰����
}CmsPhone_PARA;

typedef struct
{
	unsigned char  			voicePhone[4][16]; // �û��绰����
	
}UserPhone_PARA;

typedef struct 
{
	unsigned char			g3modenable;	// 3G ģ��ʹ�� 0--��ʹ��
	unsigned char			g3conmethd;  	// ���ӿ��Ʒ�ʽ 1:һֱ����  0:�ֶ���������״̬
	char  					simcard[16];  	// ��������
	char					ctlpwd[16]; 	// ���ſ�������
	unsigned char			g3card;			// ����modem�ͺ�ѡ��
	char  					phonenum[16];  	// ����/���ű�������
	unsigned char			g3TypeSelCtl;	// ����modem�ͺ�ѡ��ʽ 0:�Զ�ѡ�� 1:�ֶ�ѡ��
	char					m_reserved[4];	// ����
	
	unsigned int			m_changeinfo;	// ����ָʾ: λ0-6
}G3G_CONFIG;

typedef struct 
{
	unsigned char			g3mandialEnable; // 3G �ֶ����ò���ʹ�� 0--��ʹ��
	char					g3UserName[17];	 // 3G �����û���
	char					g3Passwrd[17];	 // 3G ��������
	char					g3CountryCode[17];// 3G ���Ҵ���
	char					m_reserved[16];	 // ����

	unsigned int			m_changeinfo;	 // ����ָʾ: λ0-3
}G3G_DIAL_CONFIG;

typedef struct
{
	unsigned char 			m_u8StartHour;		
	unsigned char			m_u8StartMin;
	unsigned char 			m_u8EndHour;
	unsigned char			m_u8EndMin;
	unsigned char			m_u8Valid;     	// ʱ��Ƭ���Ƿ���Ч  0 ��Ч  1 ��Ч
	unsigned char			m_reserved[3];	// ����
	
}Switch_Timer ;

typedef struct
{
	Switch_Timer			m_switchTimer[4];// ��ʱʱ��
	
}Switch_Week;

typedef struct
{
	unsigned char			m_switchID[10];	 // ��������ID--3�ֽ�
	char 				m_switchName[17];// ���������Զ�������
	unsigned char			m_timerVal;		 // ��ʱ�ܿ���	����: 1   �ر�0
	unsigned char			m_switchMode;	 // ����ģʽ	ѭ��: 1   ����0
	unsigned char			m_switchWork;	 // ����ѡ��	����: 1   �ر�0
	unsigned char			m_switchType;	 // ��������0:Light 1:Power  2:Motor  3:Electrovalve 4:Elderly Care 5:Meteo Station 6:Thermostat 7:Water Meter 8:Electricity Meter
	unsigned char			m_sceneVal;		 // ��������bit0  ����1  ��bit1  ����2��bit2  ����3��bit3  ����4
	Switch_Week				m_switchWeek[7];
	unsigned short			m_keepTime;		 // ����ʱ�� -- ��������Ϊ��λ
	unsigned short			m_cycTime;		 // ѭ��ʱ�� -- ��������Ϊ��λ
	unsigned char			m_nNoAck;		 // ��Ӧʧ��λ��0 �л�Ӧ 1 û�л�Ӧ
	unsigned char			m_reserved[3];	 // ���� 
	
}Switch_PARA ;

typedef struct
{
	char					m_deviceAliases[32];		//�豸����
	char					m_remoteDomain[32];	// Զ����Ƶ������������IP
	char					m_userName[16];
	char					m_passwd[16];
	unsigned int 			m_remotePort;		// Զ����Ƶ�������˿ں�
	unsigned char 		m_remoteCh;			// Զ����Ƶ������ͨ��
	unsigned char 		m_bindCh;			// �󶨵�������ͨ��
	unsigned char			isEnable;			// 1:ת��0:ֹͣת��
	unsigned char			m_isConnectToAlarmServer;	// 1:����0:������
	unsigned char			m_forwardIsSuccess;		// 1:�ɹ�0:ʧ��
	unsigned char			m_devChl;				//Զ���豸ͨ����
	char					m_protocol;				//	0:zmodo  1:onvif
	char					m_streamtype;		//
	unsigned char			m_mac[6];
	char					m_reserv[6];		// ����

}DecoderBind_PARA;

typedef struct
{
	DecoderBind_PARA		m_decBind[CHANNEL];		// Ŀǰ֧��4·��Ƶ����

	unsigned int			m_changeinfo;		// ����ָʾ: λ0-3 ��180�ֽ�

}NETDECODER_PARA;

typedef struct
{
	int	m_colorMode;			// 1:��ɫģʽ2:�ڰ�ģʽ
	int	m_picMode;				// 1:��ͨ2:����+��ת
	int	m_picFlip;				
	int	m_picMirrorn;
	int	m_PowerFreq;			// 0:���⿹��1:50HZ 2:60HZ 3:�Զ�����

}CAMERASENSOR_PARA;

/* p2p sync */
typedef struct 
{
	int 				schedule_id;
	int 				flag; 			// 0: ��schedule Ϊ�����豸���� ; 1: ���Ƹ澯����
	int 				repeat_day; 	//��λ���ֶ�, bit0 ��ʾ�����죬�Դ�����
	int 				off_at;		//�رյ���ʼʱ��
	int			 		on_at;		//�رյĽ���ʱ��˵�������off_at ��on_at ��Ϊ0, ���ʾȫ��ر�
}schedule_time;

typedef struct 
{
	unsigned short		off_at;		//�رյ���ʼʱ��
	unsigned short		on_at;		//�رյĽ���ʱ��˵�������off_at ��on_at ��Ϊ0, ���ʾȫ��ر�
	unsigned char		valid;		/* ��Ч��־*/	
}period_s;

typedef struct 
{
	period_s 			schedule[WEBSEVER_SCHEDULE_MAX_NUM+1];/* ʮ����־ʱ��Σ�һ������ʱ��� */
	unsigned char		day;		 /* ȫ����Ч��־*/	
}config_schedule_time;

typedef struct 
{
	config_schedule_time device_time[7]; /* ����Ϊ0�Դ����� */
	config_schedule_time notify_time[7]; /* ����Ϊ0�Դ����� */
}web_schedule;


/* sync paramter struct */
/*chang_flag bit λ */
/*bit 0: device_schedule*/
/*bit 1: notify_schedule*/
typedef struct _web_sync_param_t 
{
	char 	sync_key[12];
	struct
	{			
		unsigned char light_switch; // Light ���� 1:  �� 2���� 3���Զ� Ĭ��ֵΪ1
		unsigned char white_switch; // �׳㿪�� 1���� 2���� Ĭ�ϣ�1
		unsigned char breath_switch; // ��ɫ�л����������ܣ���1���� 2����
		unsigned char color_red;
		unsigned char color_green;
		unsigned char color_blue;
		unsigned char sunrise_hour;
		unsigned char sunrise_min;
		unsigned char sunset_hour;
		unsigned char sunset_min;
	}lamp;

	char    reserved[4];
    unsigned char    sound_sensitivity;   //����������жȣ�0Ϊ�ͣ�1Ϊ�У�2Ϊ��
    unsigned char    user_mode;           //�û�ģʽ
	unsigned char 	 voice_message_index; // �û�����ʹ���������� 0-4, 0xff ��ʾû��ѡ��
	unsigned char 	 microwave_switch;    // ΢����⿪�أ�0���ر�1������ Ĭ��ֵ��0
	unsigned char    nightvision_switch;  // ҹ�ӿ��� 1���Զ���2���������⣬3���رպ���
	unsigned char    imageflip_switch;	  // ͼ����� 0��������1�����·�ת��2�����Ҿ���3:180����ת����ת�Ӿ���
	
	/* �ʱ�����Ƴ���Ϊ30bytes�� ��Ҫʱ�����ó�32bytes��ʹ�� */
	char	time_zone[64];
	int     time_offset;
	char	reserved2[20];	
	int	 	cvr_timeout_intv;	  // �����ƴ洢�������, ��λ��
	int 	alarm_interval;
	int 	mute;
	int 	device_on;
	/**************���ֿ���*************/
			
	unsigned int use_voice_message:1; // �Ƿ���������
	unsigned int chime:1;			  // �Ƿ�����
	unsigned int light_schedule:1;    // 0�رգ�1����, Ĭ��0
    unsigned int sound_detetion:1;    // ������������, 0�رգ� 1����
	unsigned int switchs_reserved:28;
	
	/***************************/
	int 	device_schedule;
	unsigned char    device_volume;          // �������
	char	reserved4[3];
	short   ircut_admax;
	short   ircut_admin;
	int 	cvr_on; 
	int 	sensitivity;
	schedule_time  time_list[10];
}web_sync_param_t; // 360bytes
typedef struct
{
	float x;      //���������
	float y;      //���������
	float width;  //�����
	float height; //�߱���	
}P2P_MD_REGION_CHANNEL;




typedef struct 
{
	NETWORK_PARA			m_NetWork;			//��������
	MACHINE_PARA			m_Machine;			//��������
	CAMERA_PARA				m_Camera;			//��������
	CAMERA_ANALOG			m_Analog;			//ģ��������
	CAMERASENSOR_PARA		m_Sensor;			//sensor����״̬
	COMMON_PARA				m_CommPara;			//��������
	CAMERA_BLIND			m_CamerBlind;		//��Ƶ�ڵ����
	AlarmOutPort			m_AlarmPort;		//�������������
	CAMERA_VideoLoss		m_CamerVideoLoss;	//��Ƶ��ʧ
	CAMERA_MD				m_CameraMd;			//�ƶ����
	PTZ_PARA				m_PTZ;				//��̨����
	USERGROUPSET			m_Users;			//�û�����
	SYSTEMMAINETANCE		m_SysMainetance;	//ϵͳά��
	VIDEODISPLAYSET			m_DisplaySet;		//��Ƶ���
	GROUPEXCEPTHANDLE		m_SysExcept;		//�쳣����
	PCDIR_PARA				m_PcDir;			//PC�˴�������
	PELCO_CmdCfg			m_pelcoCfg;			//����ͷ����(��ʱδʹ��)
	PICTURE_TIMER			m_picTimer;			//ͼ����ɫ����
	VODEV_ANALOG			m_picVo;			//���
	G3G_DIAL_CONFIG			m_3gDial;			//3G����
	NETDECODER_PARA			m_Netdecoder;		//����������
	VIDEOOSDINSERT			m_OsdInsert;		//��Ƶ�ڵ�
	GROUPRECORDTASK			m_RecordSchedule; 	//¼������
	ALARMZONEGROUPSET		m_ZoneGroup;		//�����������ü�����
	PARAMETEREXTEND			m_ParaExtend;		// ntp��չʹ�ã�
	#if 0
	GroupZonePtzLinkSet		m_PtzLink;			//	2052
	G3G_CONFIG				m_3g;				// 60
	#endif
	web_sync_param_t		m_web;				//	360
	P2P_MD_REGION_CHANNEL	m_mdset;			// 16 
	char					m_res[1736];		//	2052+60-360 =1752-16=1736
	
}SYSTEM_PARAMETER; // 52476�ֽ�

#endif 



