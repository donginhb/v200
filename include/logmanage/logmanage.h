
#ifndef __LOG_MANAGE_H_
#define __LOG_MANAGE_H_

#include <pthread.h>

#include "common.h"

#ifdef __cplusplus
	extern "C" {
#endif

#define STREAMING_LOG    		"SLOG"

#define TS_USELOG  	 			"USER"	// �û���־

#define TF_USELOG				0x01   // �û���־�ļ�

#if 0
// �¼�����
#define  USER_EVENT_ALARM		0x01  // �����¼���ʼ
#define  USER_EVENT_SYSTEM		0x02  // ϵͳ��־
#define  USER_EVENT_FAULT		0x03  // ϵͳ����
#define  USER_EVENT_ALARMEND	0x04  // �����¼�����
#define USER_EVENT_OPERATE	0x05
#endif 


#define CUR_LOG_VERSION			0x0000001

#define DISKMOUNTPIONT			"/hdd00/p01/"

#define FOCUSLOGDIR           	"/log"

#define LOGDISPLAYMAX			12

// ¼���ļ����ҵ�ģʽ
/*  ����㿪ʼ����  */
#define   SEARCH_SET			0  

/*�ӵ�ǰ����ǰ����*/
#define  SEARCH_FORWARD			1
/*�ӵ�ǰ����ǰ����,����ѭ��*/
#define  SEARCH_NOCYCLE			0xff

/*�ӵ�ǰ���������*/
#define  SEARCH_BACKWARD		2

/*�����һҳ��ʼ�鿴*/
#define   SEARCH_END			3

typedef enum
{
	USER_EVENT_ALARM = 0x01,	//������־
	USER_EVENT_SYSTEM = 0x02,  // ϵͳ��־
	USER_EVENT_FAULT	= 0x03,  // ϵͳ����
	USER_EVENT_ALARMEND = 0x04,  // �����¼�����
	USER_EVENT_OPERATE	 = 0x05,  // ������־
	USER_EVENT_MOTION = 0x06,//�ƶ���ⱨ��
}USERLOGTYPE;


typedef enum
{
	ALARM_SUBNULL = 0,
	ALARM_SENSORIN,  // ����������
	ALARM_MD_START, //�ƶ���⿪ʼ
	ALARM_MD_END,  // �ƶ�������
	ALARM_ZONE_IN, // ��������

	ALARM_MAX_COUNT,
	
}ALARMSUBTYPE;


typedef enum
{
	FAULT_SUBNULL = 0,
	FAULT_HDD_FULL, // Ӳ����
	FAULT_IPADDR_CONFLICT, // IP��ͻ
	FAULT_HDD_ERROR,//Ӳ�̴���
	FAULT_ILLGEGAL_ACCESS, //�Ƿ�����
	FAULT_VIDEO_LOSS,		// ��Ƶ��ʧ

	FAULT_MAX_COUNT,
	
}FAULTSUBTYPE;

typedef enum
{
	OPERATE_SUBNULL = 0,
		
	/*
   		 �����оٲ������� ���������磬����û���
	*/


	OPER_MAX_COUNT,
	
}OPERATESUBTYPE;


typedef enum
{

	NULL_TYPE = 0,
	MD_ALARM,      	// �ƶ����
	FANGZONE_ALARM, //��������
	FANGZONE_RP,   	// ��������

}LOGALARMTYPE_E;


typedef struct 
{
	char 		 		m_DevType[12];
	unsigned long  		m_u32DevType;
	unsigned long		m_u32Version;
	char 				m_LogType[4];
	char				m_cDateTime[14];
	char				m_reserved[26];	
}LogFileHeader;


typedef struct 
{

#if 0
	unsigned char 		m_u8TvSystem;  //  ��ʽ
	unsigned char 		m_u8Resolution; // ������
	unsigned char 		m_u8framerate; // ֡��
	unsigned char	 	m_u8SensorNum;// ���������
	unsigned long		m_u32IOffset;  // �ļ�ƫ��
	unsigned char		m_cSensorName[9]; // ����������
	unsigned char		m_u8ChannelNum; // ͨ����
	unsigned char       m_u8AFlag;//��ʼ�¼���m_aflag��00��//�����¼���m_aflag=0xff
	unsigned char 		m_u8Reserved[21];
#else 

	unsigned char		m_u8AlarmType; // 1 : �ƶ���� ��2   ��������   3   �������� 
	unsigned char       m_u8AFlag;	//��ʼ�¼���m_aflag��00��//�����¼���m_aflag=0xff
	unsigned char  		m_u8AlarmNum;  //��������
	unsigned char	 	m_u8FangNum;   // ������
	unsigned char  		m_u8AStatus[8];  //  ����
	unsigned char  		m_u8AZoneNum;  //������ 
	unsigned char		m_u8Reserved[27];
	
#endif

}UserAlarmEvent;

typedef struct 
{
	int			  		m_u32OperCode;	// ������
	unsigned char		m_u8Reseved[36];

}UserOperEvent;


typedef struct 
{
    int			  		m_u32FaultCode; // ������
    unsigned char		m_u8Reseved[36];

}UserFaultEvent;

typedef struct 
{
    int			  		m_u32SystemCode; // ϵͳ��
    unsigned char		m_u8Reseved[36];

}UserSystemEvent;

typedef struct 
{
    int			  		m_u32CH; // �ƶ�����ͨ��
    unsigned char		m_u8Reseved[36];

}UserMotionEvent;

typedef struct 
{
	unsigned char		m_u8EventType; // �¼�����'A'  ALARM ,'O' OPERATION  'F': FAULT . 'S' : system  'M' Motion
	unsigned char		m_u8EDay; // ��
	unsigned char		m_u8EHour;// Сʱ
	unsigned char		m_u8EMinute;// ����
	unsigned char		m_u8ESec;// ��
	unsigned char		m_u8Reserved[3];
	union 
	{
		UserAlarmEvent 		m_AlarmEvent;
		UserFaultEvent		m_FaultEvent;
		UserOperEvent		m_OperEvent;
		UserSystemEvent		m_SystemEvent;
		UserMotionEvent		m_MotionEvent;
	}t_user_event;
	
}EventLogItem;



typedef struct 
{

	unsigned char  		m_u8Year; // ��־��
	unsigned char  		m_u8Month;//��־��
	unsigned char		m_u8Day; // ��־��
	unsigned char		m_u8LogType; //��־����,���������������ϣ�����
	unsigned char		m_u8SearchType; //��Է�ҳ����
	unsigned char		m_u8Reserved;
	
}FindUserLogItem;//finduserlogterm;


typedef struct 
{
	unsigned char 		m_u8ListNum;//�ļ�����
	unsigned char		m_u8StartNum; // ��һ��Ա�������еı��
	EventLogItem 		m_Item[LOGDISPLAYMAX];
}FindLogItems;


class C_LogManage
{
	private:

		datetime_setting     	m_DateTime;			/*ϵͳʱ��*/

		unsigned int			m_u32UserLogOffset;

		int					 	m_u32LastAlarmCount;

		int					 	m_u32LastOperCount;

		int						m_u32LastFaultCount;

		int 					m_HddStatus;

		pthread_mutex_t 		m_LogWriteMutex;
		
		static C_LogManage		*m_pInstance;

		inline void InitLogWriteLock();

		inline void AddLogWriteLock();

		inline void ReleaseLogWriteLock();

		void  GetSystemTime();
		
		void CheckLogFileDateValid();

		int CheckLogDirExist(unsigned char type);

		int  GetLogFileDir(char *dirname, unsigned char type);

		void  GetUserLogFileHeader(LogFileHeader *header);
		
		void RemoveLogFileOrDir(char *filename);


	public :
		C_LogManage();
		
		~C_LogManage();

		static C_LogManage *Instance();
		
		int	  WriteUserLog(unsigned char type, void *EventCode);
		
		int   GetUserLogList(FindUserLogItem *findtype, unsigned char maxcout, FindLogItems *items);

		int	WriteErrLog(int errNum, int errPid);
		
};


#ifdef __cplusplus
}	
#endif 

#endif 
