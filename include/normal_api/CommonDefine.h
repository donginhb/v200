
/******************************************************************************
  File Name     : CommonDefine.h
  Version       : Initial Draft
  Last Modified :
  Description   : the functions define
  Function List :
  History       :

******************************************************************************/
#ifndef _COMMON_DEFINE_H
#define _COMMON_DEFINE_H

#define SYSTEM_PAL			0
#define SYSTEM_NTSC			1

#define MAX_REC_MODE				4



//�ֱ���
typedef enum{
	RES_D1 = 0,
	RES_HD1,
	RES_CIF,
	RES_QCIF,
	RES_1080P,
	RES_720P,
	RES_VGA,
	RES_QVGA,
}Resolution_E;

typedef struct DATA_GPS 
{ 
	char				cGpsStatus; 				/*gps�Ƿ���Ч��ʶ*/ 
	char				cSpeedUnit; 				/*�ٶȵ�λ*/ 
	unsigned short		usSpeed;				/*�ٶ�ֵ*/ 
	char				cLatitudeDegree;		/*γ��ֵ�Ķ�*/ 
	char				cLatitudeCent; 			/*γ��ֵ�ķ�*/ 
	char				cLongitudeDegree;		/*����ֵ�Ķ�*/ 
	char				cLongitudeCent;			/*����ֵ�ķ�*/ 
	long				lLatitudeSec;			/*γ��ֵ����*/ 
	long				lLongitudeSec;			/*����ֵ����*/ 
	unsigned short		usGpsAngle;				/*gps�н�*/ 
	char				cDirectionLatitude;		/*γ�ȵķ���*/ 
	char				cDirectionLongitude;		// ���ȵķ��� 
	char				reserved[4]; 
}struGPSData;


typedef struct _stru_G_Sensor_
{
	short					x;
	short					y;
	short					z;
	short 					unit;			//> 0 ���ٶ���Ч��ʵ�ʵļ��ٶȼ��㷽���Ǽ��ٶȶ������Դ�ֵ�� 
											//= 0 ��ʾ�����ٶ���Ч����û�а�װ���ٶȴ����������߽��յ�������Ч 
											//< 0 ���ṹ��Ч���Ƿ�ֵ

}struGSensor;

typedef struct DATA_GYRO_SENSOR
{
	short		AccelerateValue;				//ת�Ǽ��ٶ�ֵ
	short		Unit;						//��λ
}struGyroSensor;


typedef struct MCU_DATA_STATUS
{
	unsigned char		AccStatus;			//��Կ���źţ�1-Կ���ź���Ч��0-Կ���ź���Ч
	unsigned char		Brake;				//ɲ���źţ�1-��ɲ���źţ�0-û��ɲ���ź�
 	unsigned char		Winker;				//ת���źţ�1-��ת��2-��ת��3-��������Ч��0-��Ч
	unsigned char		TempValid;			//�¶�ֵ�Ƿ���Ч��1-��Ч��0-��Ч
	unsigned char		TempUnit;			//�¶ȵ�λ��0-���϶ȣ�1-���϶�
	unsigned char		SpeedValid;			//�ٶ�ֵ�Ƿ���Ч��1-��Ч��0-��Ч
	unsigned char  	SpeedUnit;			//�ٶȵ�λ��0-ǧ��/Сʱ��1-Ӣ��/Сʱ
	unsigned char		reserve;			//
	short			Temperature;		//
	short			speed;				//
}struStatus;


typedef struct BINARY_HEAD
{
	unsigned short			DataType;		//��������
	unsigned short			DataLen;		//���������ݵĳ���
	unsigned long long			Pts;				//ʱ�����Ϣ����λΪ����
}struBinHead;

typedef struct DATA_COMMON
{
	//DateType = 0x0f
	struBinHead 		BinaryHead;
	struGSensor		GSensor;
	struGyroSensor	GyroSensor;
	struStatus	 		Status;
	struGPSData		Gps; 
}struMcuData;

typedef struct DATA_IN_TIME
{
	//DateType = 0x03
	struBinHead 		BinaryHead;
	struGSensor 		GSensor;
	struGyroSensor 	GyroSensor;
}struDataRealtime;

//extern "C" unsigned long Mscount(unsigned long Count);


// ���ڻ�ȡH.264 ����NAL��Ԫ��Ϣ
#define MAX_PKT_NUM_IN_STREAM 10
typedef enum nalu_type { 
	NALU_PSLICE = 1, 
	NALU_ISLICE = 5, 
	NALU_SEI = 6, 
	NALU_SPS = 7, 
	NALU_PPS = 8, 
} nalu_type_t;

typedef struct venc_pkt {
	unsigned len;
	nalu_type_t type;
} venc_pkt_t;

typedef struct venc_stream {
	venc_pkt_t pkt[MAX_PKT_NUM_IN_STREAM];
	unsigned pkt_cnt; 
	unsigned seq;
} venc_stream_t;




#endif
	
