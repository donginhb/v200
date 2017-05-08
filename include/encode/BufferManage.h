
#ifndef _BUFFER_MANAGE_H_
#define _BUFFER_MANAGE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include "CommonDefine.h"
#include "common.h"

//#define TEST_ENCODE_DATA  
//#define TEST_AENCODE_DATA  
#define MAX_FRAME_USER			MAX_BUFUSER_ID //	1
#define MAX_FRM_NUM			(30*150)
#define MAX_V_FRM_NUM			(30*30)
#define MAX_I_F_NUM			30

//���Ԥ¼ʱ��
#define MAX_PRECORD_SEC		20//16
#define MAX_ARECORD_SEC		90

//720P buffer ��С
#define BUFFER_SIZE_720P	0x180000




#define	DEFAULT_IFRAMEOFFSET				0xffff

//#define ADJUST_STREAM
#ifdef ADJUST_STREAM
/*
IframeGop		I֡���			---------��λs
FrameDiffPos		��дָ���� ---------��λ֡

*/
#define		IframeGop 			3
#define		FrameDiffPos		6
#endif
//֡����
typedef enum{
	I_FRAME = 1,
	P_FRAME,
	A_FRAME,
	B_FRAME
}FrameType_E;

//ϵͳ����ʱ��ṹ�嶨��
typedef struct{
	char		 year;
	char		 month;
	char		 mday;
	char		 hour;
	char		 minute;
	char		 second;
	char		 week;		
	char		 reserve;
}SystemDateTime, *pSystemDateTime;

//һ����Ƶ֡����Ƶ֡�ڻ���ص���Ϣ�ṹ��
typedef struct {
	unsigned long			FrmStartPos;/*��֡��buffer�е�ƫ��*/
	unsigned long			FrmLength;  /*��֡����Ч���ݳ���*/
	long long				Pts;			/*�������Ƶ֡����Ϊ��֡��ʱ���*/
	unsigned char			Flag;		/* 1 I ֡, 2 P ֡, 3 ��Ƶ֡*/
	unsigned char			hour;		/*������֡��ʱ��*/
	unsigned char			min;
	unsigned char			sec;
	venc_stream_t			venc_stream; /* ���ܰ������NAL��Ԫ */
    bool                    talk;
}FrameInfo, *pFrameInfo;

typedef struct 
{
	unsigned int		m_nVHeaderFlag; // ֡��ʶ��00dc, 01dc, 01wb
	unsigned int 		m_nVFrameLen;  // ֡�ĳ���
	unsigned char		m_u8Hour;
	unsigned char		m_u8Minute;
	unsigned char		m_u8Sec;
	unsigned char		m_u8Pad0;// ��������Ϣ�����ͣ�����������;�������Ϣ�ṹ0 ����û��1.2.3 ����������Ϣ
	unsigned int		m_nILastOffset;// ��֡�����һ��I FRAME ��ƫ��ֻ��Iframe ����
	long long			m_lVPts;		// ʱ���
	unsigned int		m_bMdAlarm:1;/*bit0 �ƶ���ⱨ��1:������0:û�б���*/
	unsigned int		m_FrameType:4;/*֡����*/
	unsigned int 		m_Lost:1;
	unsigned int 		m_FrameRate:5;
	unsigned int		m_Res:21;	/*bit11-bit31 ��ʱ����*/
	unsigned int		m_nReserved;
}VideoFrameHeader;


typedef struct 
{
	unsigned int		m_nAHeaderFlag; // ֡��ʶ��00dc, 01dc, 01wb
	unsigned int 		m_nAFrameLen;  // ֡�ĳ���
	long long			m_lAPts;		// ʱ���
}AudioFrameHeader;

//֡����صĽṹ����
typedef struct {
	unsigned char  			*bufferstart;				/*ý������buf ��ʼ��ַ*/
	unsigned long               		buffersize;				/*buf �ռ��С*/
	unsigned long	      			writepos;				/*дָ��ƫ��*/
	unsigned long				readpos;				/*��ָ���ƫ��*/

	FrameInfo				FrmList[MAX_FRM_NUM];	/*buf �д洢��֡�б���Ϣ*/
	unsigned short		 	CurFrmIndex;			/*֡�б��е��±�*/			
	unsigned short		 	TotalFrm;				/*buffer �е���֡��*/
	
	unsigned short 			IFrmList[MAX_I_F_NUM];	/*���n ��i ֡��FrmList�е������±���Ϣ*/
	unsigned short			IFrmIndex;				/*��ǰI ֡�б�����*/
	unsigned short			TotalIFrm;				/*�ܵ�I ֡��Ŀ*/
	unsigned short			ICurIndex;				//��ǰI֡����
	
	unsigned long				circlenum;				/*buf���ǵ�Ȧ��*/

	unsigned long				m_u32MaxWpos;			/*���дָ��λ��*/

//	IFRAMELISTCONTENT		BlockContent[BUFF_BLOCK_NUM];			/*������������*/
	
}FrameBufferPool, *pFrameBufferPool;

//һ��FrameBufferPool �û��ṹ�嶨��
typedef struct{
	unsigned short		ReadFrmIndex;			/*���û���֡����ط������õ�֡����*/
	unsigned short		reserve;
	unsigned long		ReadCircleNum;			/*���û���֡����صķ���Ȧ������ʼʱ����
												֡������е�circlenum*/
	unsigned int		diffpos;				/*��ָ���дָ��λ�ò�ֵ����λΪ֡*/
	unsigned int 		throwframcount;			/*�ӿ�ʼ������֡�ĸ���*/
}FrameBufferUser, *pFrameBufferUser;

//����ʱ��ص���������
typedef int (*DateTimeCallBack)(SystemDateTime *pSysTime);

class BufferManage{
public:
	//ý�建���
	FrameBufferPool	m_FrameBufferPool;

	//��ý�建��صķ����û���Ϣ
	FrameBufferUser	m_FrameBufferUser[MAX_FRAME_USER];

	//����ʱ��ص�����
	DateTimeCallBack	GetSysTime;

	//����ֱ���
	short 				m_resolution;

	//�Ƿ�����Ƶ
	char					m_have_audio;

	//ͨ����
	char					m_channel;

	unsigned int 			m_u32PCount;

	unsigned int 			m_u32IFrameOffset;
	
	//buffer ������
	pthread_mutex_t BufManageMutex;
	
	inline void InitBufferLock()
	{
		pthread_mutex_init(&BufManageMutex, NULL);
	}
	
	inline void AddBufferLock()
	{
		pthread_mutex_lock(&BufManageMutex);
	}
	
	inline void ReleaseBufferLock()
	{
		pthread_mutex_unlock(&BufManageMutex);
	}
	
public:
	BufferManage();
	virtual ~BufferManage(){};

//	int					m_nTestFd[8];
 
	
	BufferManage(DateTimeCallBack GetDatetime)
	{
		InitBufferLock();
		GetSysTime = GetDatetime;
	}
	

	//ע��ص�����
	inline void RegisterCallBackFunc(DateTimeCallBack GetDatetime)
	{
		GetSysTime = GetDatetime;
	}

	/*****************************************************************************
	��������:���캯��
	�������:
	�������:��
	��  ��   ֵ:��
	ʹ��˵��:
	******************************************************************************/
	BufferManage(DateTimeCallBack GetDatetime, int ch);

	/*****************************************************************************
	��������:ע��һ������ص�ʹ���û�
	�������:@userid: �û�id(Ŀǰֻ֧��0)
	�������:��
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:
	******************************************************************************/
	virtual int RegisterUser(int userid);

	/*****************************************************************************
	��������:��ʼ�������ʹ���û���Ϣ
	�������:@userid: �û�id(Ŀǰֻ֧��0)
	�������:��
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:
	******************************************************************************/
	virtual int InitUserInfo(int userid);

	/*****************************************************************************
	��������:��λָ���������û��Ķ�ָ����Ϣ
	�������:@userid: �û�id(Ŀǰֻ֧��0)
	�������:��
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:
	******************************************************************************/
	virtual int ResetUserInfo(int userid);

	/*****************************************************************************
	��������:��ʼ��������еĳ�Ա
	�������:��
	�������:��
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:
	******************************************************************************/
	int InitBuffer();

	/*****************************************************************************
	��������:���������
	�������:@resolution: �ֱ���
				   @audioflg: �Ƿ�����Ƶ
				   @bufsize: ����buffer ��С��Ϊ0ʱ���ݷֱ���ָ��
	�������:��
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:����ʧ�ܣ�һ����Ҫʹ�ô˻���أ��ϲ�һ��Ҫ����
	******************************************************************************/
	int CreateBufferPool(int resolution, int audioflg, unsigned long bufsize);

	/*****************************************************************************
	��������:���ٻ����
	�������:��
	�������:��
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:
	******************************************************************************/
	int DestroyBufferPool();


	/*****************************************************************************
	��������:��ȡһ����Ƶ֡����Ϣ
	�������:@Astream: ��Ƶ���ݽṹָ��
	�������:@framelen:֡����
				   @pts:ʱ���
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:
	******************************************************************************/
	virtual int GetAudioFrameInfo(void *Astream, int *framelen, unsigned long long *pts);

	/*****************************************************************************
	��������:��ȡһ����Ƶ֡����Ϣ
	�������:@Vstream: ��Ƶ���ݽṹָ��
	�������:@frametype:֡����(I or P)
				   @framelen:֡����
				   @pts:ʱ���
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:
	******************************************************************************/
	virtual int GetVideoFrameInfo(void *Vstream, int *frametype, int *framelen, unsigned long long *pts);

	/*****************************************************************************
	��������:���һ����Ƶ֡���������
	�������:@Vstream: ��Ƶ���ݽṹָ��
	�������:��
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:
	******************************************************************************/
	virtual int PutOneVFrameToBuffer(void *Vstream);

	/*****************************************************************************
	��������:���һ����Ƶ֡���������
	�������:@Asteam: ��Ƶ���ݽṹָ��
	�������:��
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:
	******************************************************************************/
	virtual int PutOneAFrameToBuffer(void *Astream,bool talk);

	/*****************************************************************************
	��������:���һ֡�����������ݵ��������
	�������:@Asteam: ��Ƶ���ݽṹָ��
	�������:��
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:
	******************************************************************************/
	virtual int PutOneBinaryDataToBuffer(char *BinaryStream, int len, unsigned long long pts);

	/*****************************************************************************
	��������:���캯��
	�������:@userid:�û�id(0~2, Ŀǰֻ֧��0)
	�������:@buffer: ����֡����ʼ��ַ
				   @pFrameInfo:���ش�֡����Ϣ
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:����ʧ�ܣ�����ʹ��buffer ָ��������ݵĻ�ȡ
	******************************************************************************/
	virtual int GetOneFrameFromBuffer(int userid, unsigned char **buffer, FrameInfo *pFrameInfo);

	/*****************************************************************************
	��������:������ǰ��ʱ��������ʼ��ȡ���ݵ�����λ��
	�������:@userid:�û�id(0~2, Ŀǰֻ֧��0)
				   @secnum: ��ǰ������
	�������:��
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:�ڲ�����
	******************************************************************************/
	virtual int CalculateReadIndexByTime(int userid, int secnum);

	/*****************************************************************************
	��������:��ʼ��ȡ����Ƶ��������ָ����ʼ��ȡ���ݵ�����λ��
	�������:@userid:�û�id (0~2, Ŀǰֻ֧��0)
				  @flag: ָ����ʼ��ȡ�ķ�ʽ0--��ǰi ֡λ��,1--�ϴν���λ��
				  ֮ǰ�����һ��i ֡λ�ã�2--��ǰʱ��֮ǰ�������λ��
				  @para:flag ==2ʱΪ��ǰ��ʱ��(��λΪ��)
	�������:��
	��  ��   ֵ:�ɹ�����0�����򷵻�-1
	ʹ��˵��:�ڸ�����¼��ʱ��Ҫ����һ��
	******************************************************************************/
	virtual int StartGetFrame(int userid, int flag, void * para);

	int GetAvDataBlock(int userid, char **buffer, int *len);

	
};
#endif
