
#ifndef __VIDEO_MD_H__
#define __VIDEO_MD_H__

#include "common.h"
#include "hi_type.h"
#include "hi_comm_vda.h"
#include "mpi_vda.h"
#include "Audio.h"
#include "HDalg.h"


#include "ModuleFuncInterface.h"
#include "parametermanage.h"

#define MAXOBJNUM		32

#define VIDEOINPUT_MAX_CHANNEL_NUM		CHANNEL_MAX
#define WIDTH_NUM	40   //80
#define HEIGHT_NUM  30   //45
#define MAX_MACROCELL_NUM   WIDTH_NUM*HEIGHT_NUM	/* 80*45 or 40*30 */
#define USER_WIDTH_NUM  20  //10
#define USER_HEIGHT_NUM  15  //10
#define MAX_USERSET_AREA	USER_WIDTH_NUM*USER_HEIGHT_NUM

#define MDTION_ALARM_DEALY_TIME		65
#define MD_VALID_NAME_LEN   	(HI_U8)16
#define MD_MACROCELL_WIDTH      (HI_U8)16
#define MD_MACROCELL_HEIGHT     (HI_U8)16

typedef int 	(*Connect_Wifi)(char *ssid ,char *password,int lang);/*����wifi �ص�*/
typedef int 	(*Get_Wifi_Status)(void);/*��ȡwifi����״̬���ɹ�true,ʧ��false*/
int ZMD_StartQRScan(bool isfirst,Connect_Wifi ConnectWifiCB,Get_Wifi_Status GetWifiStatusCB );/*����QRɨ��*/
int Get_Md_Status();


typedef struct hiMD_AREA_CFG_S
{
	HI_S32 chno; /* ͨ����*/
	HI_BOOL enable_flag; /* �û�ָ���Ƿ�ʹ�ܸ���������ƶ����*/
	HI_BOOL run_flag; /*���������ڽ����ƶ�����־*/
	HI_S32 Macro_threshold; /* �����ֵ */
	HI_S32 Macro_ratio; /* �����б��������� */
	HI_S32 FrameInterval; /* ֡�����ÿ����֡��һ���ƶ���⴦�� */
	RECT_S m_Area;
	HI_S8 userMask[MAX_USERSET_AREA];
	HI_S8 mask[MAX_MACROCELL_NUM]; /* �����ƶ���������Ӧ�ĺ��� */
}MD_AREA_CFG_S;

//���У��ṹ��MD_STATUS_S�ṹ�������¡�
typedef struct hiMD_STATUS_S
{
HI_U8 u8Mask[MAX_MACROCELL_NUM]; /* �ƶ����������ʶ */
HI_U8 u8MDValue[MAX_MACROCELL_NUM]; /* SAD��ȡֵ0��100��ʾ */
} MD_STATUS_S;
typedef struct _MdInfo_
{
	MD_AREA_CFG_S		MDPara;
	int 				MdAlarm;	
	bool				MdThreadRun;	
	bool 				mdThreadexit;
}Md;
typedef struct _MdObjRegion_
{
	unsigned short		Left;
	unsigned short		Top;
	unsigned short		Right;
	unsigned short		Bottom;
	unsigned int 		area;
}MdobjRegion;




class MD_HANDLE
{

private :

	
	
	
	static  MD_HANDLE		*m_pInstance;	
	


	Md					m_Mdpara;
	MdobjRegion			m_Objregion[MAXOBJNUM];
	unsigned char 		m_RelMdData[MAX_MACROCELL_NUM];
	unsigned int		m_objnum;	
	int					m_Real_threshold;
	int 				m_sentive;
	int 				m_web_upload;
	pthread_t			m_pid;
	pthread_mutex_t 	m_mdlock;









	/*****************************************************************************
	��������:���������Ƿ�����ƶ����
	�������:pstVdaData ��������Դ���ڲ�ʹ��
	�������:��
	��  ��   ֵ:0:�ɹ�  -1:ʧ��
	ʹ��˵��: 
	******************************************************************************/
	int AnalyseMDObj(VDA_DATA_S *pstVdaData);
	/*****************************************************************************
	��������:��������ж��Ƿ�����ƶ����
	�������:pstVdaData ��������Դ���ڲ�ʹ��
	�������:��
	��  ��   ֵ:0:�ɹ�  -1:ʧ��
	ʹ��˵��: 
	******************************************************************************/
	int AnalyseMDSad (VDA_DATA_S *pstVdaData);
	int MDAlarmSet(bool alarm);
	int MDRecordSet();

public:

	MD_HANDLE();
	~MD_HANDLE();
	static  MD_HANDLE*  Instance();
	/*****************************************************************************
	��������:��ʼ��md����
	�������:
	
	�������:��
	��	��	 ֵ:��
	ʹ��˵��:
	******************************************************************************/
	void InitMDConfig();
	/*****************************************************************************
	��������:����md����
	�������:uMask --- Ҫ������������
				  sentive----������0~4
				  ch---ͨ���� ��ʱû��ʹ�ã�
	
	�������:��
	��	��	 ֵ:��
	ʹ��˵��: 
	******************************************************************************/
	void MotionDetectionUserCfg(int ch, unsigned char *uMask, int sentive);
	/*****************************************************************************
	��������:��ȡmd״̬
	�������:				 
				  ch---ͨ���� ��ʱû��ʹ�ã�
	
	�������:0 :û�б���  1:����
	��	��	 ֵ:��
	ʹ��˵��: 
	******************************************************************************/

	int GetVideoMdStatus(int ch);
	int GetRealMdStatus();
	int GetVideoObjStatus(MdobjRegion **pMdregion);
	/*****************************************************************************
	��������:�����ƶ����
	�������:	
	�������:��
	��	��	 ֵ:0:�ɹ�	-1:ʧ��
	ʹ��˵��: 
	******************************************************************************/

	int StartMotionDetection();
	/*****************************************************************************
	��������:
	�������:	
	�������:��
	��	��	 ֵ:��
	ʹ��˵��: 
	******************************************************************************/

	void ResetMotionArea(int ch);
	/*****************************************************************************
	��������:ֹͣ�ƶ����
	�������:	
	�������:��
	��	��	 ֵ:0:�ɹ�	-1:ʧ��
	ʹ��˵��: 
	******************************************************************************/
	int StopMdProcess();

	
	/*****************************************************************************
	��������:�ƶ���⴦���߳�
	�������:	
	�������:��
	��	��	 ֵ:0:�ɹ�	-1:ʧ��
	ʹ��˵��: 
	******************************************************************************/
	int MDThreadBody();
	


	int MOTION_CHECK( HumanDetHandle  phdHandle );
	int ALARM_PRO();
	volatile int md_snap_time;
	int		MD_STATUS;
	long 	MD_interval;
	void Set_Md_Flag(int flag);
	int Get_Md_Flag();
	
};

#endif 

