
#ifndef __VIDEO_AD_CTRL_H__

#define __VIDEO_AD_CTRL_H__



#define ADV7180_SET_CON					0x01		//�Աȶ�

#define ADV7180_SET_BRT					0x02		//����

#define ADV7180_SET_HUE					0x03		// ɫ��

#define ADV7180_SET_SAT					0x04		// ���Ͷ�


#define ADV7180_VIDEOSTATUS				0x10		// ����Ƶ�Ƿ�ʧ

void VideoADInit();

void GetVideoADStatus(int *status);

void VideoADExit();

#endif 

