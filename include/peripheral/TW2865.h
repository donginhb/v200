
#ifndef __TW_2865_H__

#define __TW_2865_H__

#include "common.h"

#define TW2865_NTSC	1
#define TW2865_PAL	2

typedef struct 
{	/*register read and write*/
	unsigned char m_u8Addr;//register address
	unsigned char m_u8Value;//register value
	unsigned char m_u8ChipNo;//chip number, base on zero
	unsigned char m_u8Reserved[5];
}RegisterRW;

typedef struct 
{	/*analog set*/
	unsigned char m_u8Hue;
	unsigned char m_u8Contrast;
	unsigned char m_u8Brightness;
	unsigned char m_u8Saturation;
	unsigned char m_u8ChnNo;/*base 0, [0, 15]*/
	unsigned char m_u8Reserved[7];
	unsigned long SettingBitmap;
}AnalogSetup;


typedef struct 
{
	unsigned char m_u8PbOrLp;/*0:playback; 1:loop*/
	unsigned char m_u8Channel;/*[0, 15]*/
	unsigned char m_u8Reserved[6];
}AudioOuputCtrl;


typedef enum 
{
	ASR_8K,
	ASR_16K,
	ASR_32K,
	ASR_44DOT1K,
	ASR_48K
}AudioSampleEnum;

typedef enum
{
	ANA_HUE_SET,
	ANA_CONTRAST_SET,
	ANA_SATURATE_SET,
	ANA_BRIGHTNESS_SET,
	
}AnalogTypeSetEnum;

#define TW2865CMD_READ_REG			0x00   	/* ��ȡ�Ĵ���ֵ*/
#define TW2865CMD_WRITE_REG			0x01  	/* ���üĴ���ֵ*/
#define TW2865CMD_GET_VL				0x02 	 /* �����Ƶ��ʧ״̬*/
#define TW2865CMD_SET_SAMPLERATE		0x03	/*Audio output sample rate */
#define TW2865CMD_SET_ANALOG			0x04 	/* ����ģ���� */
#define TW2865CMD_GET_ANALOG			0x05 	/* ���ģ����ֵ */
#define TW2865CMD_READ_REGT			0x06 
#define TW2865CMD_AUDIO_OUTPUT		0x07  	/* ������Ƶ���*/
#define TW2865CMD_AUDIO_MUTE			0x08 	/* ���þ��� */
#define TW2865CMD_AUDIO_DEMUTE		0x09  	/* ������� */
#define TW2865CMD_AUDIO_VOLUME		0x0a	/*�������� */
#define TW2865CMD_SET_NORM			0x0b	/*������ʽ*/
#define TW2865CMD_SET_DACPD			0x0c	/*����DAC POWER DOWN*/


class HWTW2865
{

	private:

		int 		m_n32Fd;

		static HWTW2865   *m_pInstance;
		
	public:

		HWTW2865();
		
		~HWTW2865();

		static HWTW2865*  Instance();

		int  GetVideoStatus(int *VideoStatus);

		int SetVideoAnalog(int AType,  int value, int ch);

		int SetAudioVolume(int Volume);

		int SetAudioOutPutMute(int OnOff);

		int SetAudioSampleRate(AudioSampleEnum  SmpRate);

		int SetAudioOutput(int mode, int channel);

		void GetVideoAnalogValue(int AType,  int *value, int ch);

		int   SetTw2865Normal(int mode);

		int 	SetTw2865DAPowerDown(int Off);
		

};


#endif 

