#ifndef P2P_ALARM_H_38J7D8AKDFA987ADJAIOQYFJBA
#define P2P_ALARM_H_38J7D8AKDFA987ADJAIOQYFJBA

#include <time.h>
#include <stdint.h>

enum P2pAlarmType
{
	P2P_ALARM_MD = 0,
	P2P_ALARM_IO = 4,
	P2P_ALARM_VIDEOLOST = 5,
	P2P_ALARM_SD_EXCEPTION = 6,
	P2P_ALARM_ANSWER = 7,
	P2P_ALARM_AUDIO_EXCEPTION = 8,
	P2P_ALARM_BABY_CRY = 9,
	P2P_ALARM_OLD_BELL_RING = 11,
	P2P_ALARM_DOORMEGNET_OPEN = 12,
	P2P_ALARM_DOORMEGNET_CLOSE = 13,
	P2P_ALARM_MD_CLOUD_RECORD = 14,
	P2P_ALARM_BELL_RING = 15,
    P2P_ALARM_MICROWAVE = 16,
    P2P_ALARM_SMOG = 17,    /*������*/
    P2P_ALARM_COMBUSTIBLE_GAS = 18, /*��ȼ���屨��*/
    P2P_ALARM_EMERGENCY_BUTTON = 19, /*������ť����*/
    P2P_ALARM_HOME_MODE = 20, /* �ڼ�ģʽ */   
    P2P_ALARM_OUT_MODE = 21,  /* ���ģʽ */
    P2P_ALARM_SLEEP_MODE = 22, /* ˯��ģʽ */
    P2P_ALARM_CUSTOM_MODE = 23, /* �Զ���ģʽ */
    P2P_ALARM_BUZZER_TRIGGER = 24, /*����������*/
    P2P_ALARM_BUZZER_HALT = 25, /*������ֹͣ*/
    P2P_ALARM_PIR = 26, /*PIR����*/
    P2P_ALARM_REMOTE_CONTROL = 27, /*ң��������*/

    P2P_ALARM_NEW_MD = 99,		// malloc alarm_info: check_type/position/delta/need_capture_pic_count
    P2P_ALARM_NEW_MD_CLOUD = 100, // malloc alarm_info: check_type/position/delta
};

class AlarmPoster
{
 public:
	//channel ����Ƶ����IOͨ����
	static AlarmPoster* CreateAlarm(P2pAlarmType type, time_t happen_time = time(NULL), int channel = 0);
	
	//��������豸(�����豸)�����ı������븽����ID
	static void AddSubDeviceID(AlarmPoster *poster, const char* id);
	
	//channel ����Ƶͨ����
	static void AddPicture(AlarmPoster *poster, const char* file_path, int channel = 0, int need_delete = 1);

	static void AddRecord(AlarmPoster *poster, const char* file_path, uint32_t file_size);

	static void AddString(AlarmPoster *poster, const char* name, const char* value);

	//�첽ģʽ�ϴ�����������֮�� �ڲ��Զ�����poster
	static void PostAlarm(AlarmPoster *poster);
	
	//ͬ��ģʽ�ϴ�������������, ��������ʱ�ڲ��Զ�����poster
	static int  SendAlarm(AlarmPoster *poster);
};

#endif
