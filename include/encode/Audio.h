/******************************************************************************
  File Name     : Audio.h
  Version       : 
  Last Modified :2013.01.08
  Description   : ��Ƶ�����
  Function List :
  History       :
   Modification: Created file
  by:harvey
******************************************************************************/

#ifndef _AUDIO_H_
#define _AUDIO_H_
#include "CommonDefine.h"
#include "hi_comm_aio.h"
#include "acodec.h"
#include "Video_comm.h"
#include "hi_common.h"
#include "hi_comm_aenc.h"
#include "mpi_aenc.h"
#include "mpi_ai.h"
#include "mpi_adec.h"
#include "mpi_ao.h"
#include "common.h"
#include "BufferManage.h"

#include "AbnormalVoice.h"


#define MAXUSERNUM     8
#define BUFFERLEN     1024*50
#define NumPerFrm		160

#define ZBAR_SCAN_FILE	  "/app/voice/zbarscan"

#define ZBAR_SCAN_ST_C    "/app/voice/chinese_connecting.wav"
#define ZBAR_SCAN_ST_E    "/app/voice/english_connecting.wav"
#define ZBAR_SCAN_ST_S    "/app/voice/spanish_connecting.wav"



#define ZBAR_SCAN_W_C     "/app/voice/chinese_wait.wav"
#define ZBAR_SCAN_W_E     "/app/voice/english_wait.wav"
#define ZBAR_SCAN_W_S     "/app/voice/spanish_wait.wav"



#define ZBAR_SCAN_SU_C    "/app/voice/chinese_success.wav"
#define ZBAR_SCAN_SU_E    "/app/voice/english_success.wav"
#define ZBAR_SCAN_SU_S    "/app/voice/spanish_success.wav"
#define VOLUMEMUSIC 	   "/app/voice/sound"

#define ACODEC_FILE     "/dev/acodec"


#define GPIO_DEV        "/dev/gpio_dev"
#define GPIO_ADUIO_AMPLIFIER _IO('p',0x9)

typedef enum AUDIO_TYPE
{
    ZBAR_SCAN,
  	WIIF_CONNECT,
  	ZBAR_WIFI_START_CH,
  	ZBAR_WIFI_START_EN,
  	ZBAR_WIFI_START_SP,
  	ZBAR_WIFI_WAIT_CH,
  	ZBAR_WIFI_WAIT_EN,
  	ZBAR_WIFI_WAIT_SP,
  	ZBAR_WIFI_SUCCESS_CH,
  	ZBAR_WIFI_SUCCESS_EN,
  	ZBAR_WIFI_SUCCESS_SP,
  	BUTT,
  	VOLUME_SOUND,
    
} PLAY_TPYE;

typedef struct
{
    bool	bStart;
	bool 	bthreadrun;
    pthread_t stAencPid;
    HI_S32  AeChn;
    HI_S32  AdChn;
    HI_BOOL bSendAdChn;
	class Audio *pAudio;

} AudioEnc_S;
typedef struct
{
	unsigned char Type;/*0 g711 , 1 g726*/
	unsigned int  datalen;
	unsigned long long timestamp;
} AudioHead;



typedef struct
{
	
	unsigned char 			samplerate; 	/*������ 0:8K ,1:12K,  2: 11.025K, 3:16K ,4:22.050K ,5:24K ,6:32K ,7:48K*/;	
	unsigned char 			audiotype;		/*��������0 :g711 ,1:G726*/
	unsigned char 			enBitwidth;		/*λ��0 :8 ,1:16 2:32*/
	unsigned char			inputvolume;		/*��������0 --31 */
	unsigned char 			outputvolume;		/*�������0 --31*/
	unsigned short			framelen ;		//��Ƶ֡��С(80/160/240/320/480/1024/2048)
	char 					reserved[9];		/*����9��һ��16���ֽ�*/
} AudioParm;




class Audio
{
	private:
			AIO_ATTR_S m_stAioAttr;
			AudioEnc_S m_AudioEnc;
			bool	   m_DecodeState;/*0:����δ��ʼ 1:�������ڽ���*/
			pthread_mutex_t m_AudioLock;

			unsigned char m_inputvolume;
			unsigned char m_outputvolume;

			
	public:
		Audio();
		~Audio();

		int GetAudioParam(AudioParm *param);
		/*****************************************************************************
		��������:InitAudio
		��������:��ʼ����Ƶ����Ҫ���������������������
		�������:��
		�������:��
		��  ��   ֵ:0:�ɹ�  -1:ʧ��
		ʹ��˵��: �ⲿ���ã�����Ƶ�����ǰ��Ҫ����
		by :harvey
		******************************************************************************/
		int 	InitAudio();

		bool	InitAbnormalVoice();
		void 	AbnormalVoiceProcess(char *buf,int len);
		/*****************************************************************************
		��������:StartAudioEncode
		��������:������Ƶ���룬������ g711����
		�������:��
		�������:��
		��	��	 ֵ:0:�ɹ�	-1:ʧ��
		ʹ��˵��: �ⲿ����?		by :harvey
		******************************************************************************/
		int StartAudioEncode(int Audiotype);
		
		/*****************************************************************************
		��������:StopAudioEncode
		��������:�ر���Ƶ���룬
		�������:��
		�������:��
		��	��	 ֵ:0:�ɹ�	-1:ʧ��
		ʹ��˵��: �ⲿ���ã�
		by :harvey
		******************************************************************************/
		int StopAudioEncode();
		
		/*****************************************************************************
		��������:StartAudioDecode
		��������:������Ƶ���룬
		�������:��
		�������:��
		��	��	 ֵ:0:�ɹ�	-1:ʧ��
		ʹ��˵��: �ⲿ���ã�����SendAudioStreamToDecode ǰ��Ҫ�ȿ�������
		by :harvey
		******************************************************************************/
		int StartAudioDecode();
		
		/*****************************************************************************
		��������:StopAudioDecode
		��������:ֹͣ��Ƶ���룬
		�������:��
		�������:��
		��	��	 ֵ:0:�ɹ�	-1:ʧ��
		ʹ��˵��: �ⲿ���ã�StopAudioDecode��Ҫ�ٿ��������ſ��Ե���
		by :harvey
		******************************************************************************/
		int StopAudioDecode();
		int StartPlayFile(PLAY_TPYE  Type,void *other);
		/*****************************************************************************
		��������:SetInPutVolume
		��������:������������
		�������:vol (0~31)       15ΪĬ������vol Խ������Խ��
		�������:��
		��  ��   ֵ:0:�ɹ�  -1:ʧ��
		ʹ��˵��: �ⲿ���ã�
		by :harvey
		******************************************************************************/
		int SetInPutVolume(unsigned int vol);
		
		/*****************************************************************************
		��������:SetOutPutVolume
		��������:�����������
		�������:vol (0~31)       25ΪĬ������vol Խ������Խ��
		�������:��
		��  ��   ֵ:0:�ɹ�  -1:ʧ��
		ʹ��˵��: �ⲿ���ã�
		by :harvey
		******************************************************************************/
		int SetOutPutVolume(unsigned int vol);
		
		/*****************************************************************************
		��������:SendAudioStreamToDecode
		��������:�������������Ƶ������
		�������:��
		�������:��
		��	��	 ֵ:0:�ɹ�	-1:ʧ��
		ʹ��˵��: �ⲿ�����ٴ˺�������ǰ��Ҫ������Ƶ����
		by :harvey
		******************************************************************************/
		int SendAudioStreamToDecode(unsigned char *buffer,int len,int block=0);
		
		/*****************************************************************************
		��������:AudioEncodeThreadProcess
		��������:���봦���߳�
		�������:parg
		�������:��
		��	��	 ֵ:��
		ʹ��˵��: 
		by :harvey
		******************************************************************************/
		void AudioEncodeThreadProcess(void *parg);
		void PalyFileThreadProcess(void *parg);


        int AudioOutPutOnOff(unsigned int flag);

		bool AudioGetSpeeker();
		bool AudioReleaseSpeeker();
		void AudioInput(bool cmd);
		int GetAudioAlarm();
		void SetAudioAlarmValue(int value);
		int AudioAlarmSet(int alarm);
	private:
		
		/*****************************************************************************
		��������:StartAudioAI
		��������:�������룬��ͨ��chid  --1
		�������:��
		�������:��
		��	��	 ֵ:��
		ʹ��˵��: �ڲ�����
		by :harvey
		******************************************************************************/
		int StartAudioAI();
		
		/*****************************************************************************
		��������:StartAudioAO
		��������:���������˫����
		�������:��
		�������:��
		��	��	 ֵ:��
		ʹ��˵��: �ڲ�����
		by :harvey
		******************************************************************************/
		int StartAudioAO();


	
		
		int AudioExit();
		bool	m_audioinput;
		bool	m_speekfree;
		bool	m_PlayFile;
		bool	m_initAbnormalVoice;
        unsigned int    m_TalkDelay;
		PLAY_TPYE		m_PlayType;
		pthread_mutex_t m_speeker;
		int		m_Audioalarm;

};


#endif

