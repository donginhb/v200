#ifndef __WIFICONFIG__H
#define __WIFICONFIG__H

//��ʼ��wifi
int InitWifi();

int StartConnectWifi(TYPE_WIFI_LOGIN* wifilogin);
//��ȡ�ȵ��б�
void GetAP_Lists(char *sendBuf,unsigned int  *apCount);

//IE����wifi����
bool csSetWIFI(SYSTEM_PARAMETER *plocalPara,SYSTEM_PARAMETER *pcsPara);

//��ȡwifi����״̬
int GetWIFI_Status(TYPE_WIFI_LOGIN *pwifidev);

//����wifi MAC��ַ
int SetWifiMac(unsigned char *mac);

//��ȡwifi MAC��ַ
int GetWifiMac(unsigned char *mac);

//status:0--�ر�wifi led;1--����wifi led
bool SetWIFI_LED(bool status);

//����ֵ:0--wifi led�ر�;1--wifi led����
bool GetWIFI_LED();

//����ϵͳ��λ״̬LED��־
void SetSysRestLED();

void wifi_disable(void);

#endif


