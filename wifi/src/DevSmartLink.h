#ifndef DEVSMARTLINK_H
#define DEVSMARTLINK_H

//#define DEBUG  //���Կ���
#define WIFI_INTERFACE             "wlan0" //wifi��������
#define WIFI_MONITER_INTERFACE     "wlan.m"  //wifi��������������ץȡ��������
#define CHANNEL_CHANGE_STEP        500000U //�ŵ��л�ʱ����
#define IEEE80211_DATA             0x08 //IEEE802.11 ����֡����ֵ,4bits(type+version)
#define IEEE80211_MAC_HEADER       32   //IEEE802.11֡ͷ(MAC Header)�̶�����Ϊ32bytes
#define IEEE80211_PROTOCOL_VERSION 0x00 
#define IP_HEADER                  20   //IPͷ������
#define DEST_PORT                  7773 //UDP���ݰ���Ŀ�ĵص�ַ
#define MAX_UPD_LEN                1472 //UDP���ݰ���Ч�����
#define IEEE80211BG_RADIO_LEN      26   //��80211BG���ٶ��£���Ƶ��Ϣ����Ϊ26bytes
#define IEEE80211N_RADIO_LEN       29   //��80211N���ٶ��£���Ƶ��Ϣ����Ϊ29bytes
#define MAX_CHANNEL_NUMBER         13   //֧�ֵ��ŵ���
#define SOCK_BUFFER_SIZE           256*1024 //256KB,Ĭ��ֻ��108KB

/* AP������Ϣ�ṹ�� */
typedef struct {
	char dev_id[16];
	char ap_ssid[32];
	char ap_pwd[32];
}AP_CONNECT_INFO_T;

typedef struct{
	unsigned short SourcePort;
	unsigned short DestPort;
	unsigned short len;
	unsigned short CheckSum;
}UDP_HEADER_T;

/* Exit from The Distribution System,IEEE802.11֡ͷ*/ 
typedef struct{
	unsigned short	FrameControl;
	unsigned short	duration;
	unsigned char   DestAddr[6];
	unsigned char   BssidAddr[6];
	unsigned char   SourceAddr[6];
	unsigned short	sequence;
	unsigned short	qos;
}IEEE80211_MAC_HEADER_EXIT_T;

/* To The Distribution System,IEEE802.11֡ͷ*/ 
typedef struct{
	unsigned short	FrameControl;
	unsigned short	duration;
	unsigned char   BssidAddr[6];
	unsigned char   SourceAddr[6];
	unsigned char   DestAddr[6];
	unsigned short	sequence;
	unsigned short	qos;
}IEEE80211_MAC_HEADER_TO_T;

#ifdef __cplusplus
extern "C"{
#endif

typedef int(*Dev_SmartLink_Callback_Func)(char *ap_ssid, char *ap_pwd, int lang);

int platform_wifi_smartlink_startup(Dev_SmartLink_Callback_Func pCbFunc, char *iw_path);

int platform_wifi_smartlink_cleanup();

#ifdef __cplusplus
}
#endif

#endif
