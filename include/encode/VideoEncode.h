
#ifndef _VIDEO_ENCODE_H_
#define _VIDEO_ENCODE_H_

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
#include "BufferManage.h"
#include "His_Api_define.h"
#include "CommonFunction.h"
#include "Video_ISP.h"
#include "Video_comm.h"
#include "ModuleFuncInterface.h"
#include "EncodeManage.h"
#define MAX_CH_STREAM	3



//��������ṹ�嶨��
typedef struct {
	unsigned char			norm;		/*0--PAL, 1--NTSC*/	
	unsigned char			resolution;	/*0--720p,1--VGA  2-QVGA*/
	unsigned char			framerate;	/*֡��*/
	unsigned char			VbrOrCbr;	/*1--CBR, 0--VBR*/
	
	unsigned char			quality;		/*����*/
	unsigned char			gop;		/*I ֡���*/
	unsigned char			streamtype;	/*1--��������0--������*/
		
	int						bitrate;		/*����*/


}EncodePara_S;

class VideoEncode;
typedef struct _encoderstream_{/*����*/
	class VideoEncode *pEncoder;	
	class BufferManage	*pBufferManage;
	int 		venc;/*����ͨ��*/
	int			EncodeStat;/*����״̬1:running 0:stop*/
	int			ThreadStat;/*�߳�״̬1:running 0:stop*/
	pthread_t		pid;
}encoderstream_t;

class VideoEncode{
private:

	 
	pthread_t m_IspPid;

	

public:
	VideoEncode();
	~VideoEncode(){};
	CVideoISP 			*m_pISP;
	encoderstream_t		m_EncoderStream[MAX_CH_STREAM];

	int InitVideoEncoder();
	/*****************************************************************************
	��������:InitBuffer
	��������:��ʼ��buffer
	�������:��
	�������:��
	��	��	 ֵ: 0: �ɹ�-1: ʧ��
	ʹ��˵��: �ڲ������ã�
	
	******************************************************************************/
	int InitBuffer();	

	
	/*****************************************************************************
	��������:VideoViConfig
	��������:vi��������
	�������:��
	�������:��
	��	��	 ֵ: 0: �ɹ�-1: ʧ��
	ʹ��˵��: �ڲ������ã�
	
	******************************************************************************/
	int VideoViConfig();
	
	
	/*****************************************************************************
	��������:Hi3518SysInit
	��������:ϵͳ��ʼ
	�������:��
	�������:��
	��	��	 ֵ: 
	ʹ��˵��: �ڲ������ã�
	
	******************************************************************************/
	int Hi3518SysInit();
	/*****************************************************************************
	��������:StartVideoEncoder
	��������:��ʼ����Ƶ����
	�������:pEncPara ��������ṹ��
	�������:��
	��	��	 ֵ: 0: �ɹ�-1: ʧ��
	ʹ��˵��: �ⲿ���ã�
	�����ϵͳ��ʼ����ſ��Ե���
	
	******************************************************************************/
	int StartVideoEncoder(EncodePara_S *pEncPara);
	int StartStreamThread(int venc);
	int VideoStreamThreadBody(int venc);
	/*****************************************************************************
	��������:VideoMirrorFlipSet
	��������:ͼ��ת������Ϥ������
	�������:Para 4:����1 ��ת2 ����3 ����ӷ�ת
	�������:��
	��	��	 ֵ: 0: �ɹ�-1: ʧ��
	ʹ��˵��: �ⲿ���ã�
	�����ϵͳ��ʼ����ſ��Ե���
	
	******************************************************************************/

	int VideoMirrorFlipSet(int Para);

	int VideoMirrorSet(bool set);

	int VideoFlipSet(bool set);

	//ֹͣ����
	int StopVideocEncode();

	int VideoEncodeExit(int VeChn );

	int SensorInit();
	int SetVideoAntiFlickerAttr(bool enable,unsigned short Frequency );

	
};

#endif
