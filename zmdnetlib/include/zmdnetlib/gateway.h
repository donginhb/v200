#ifndef _GATEWAY_H_
#define _GATEWAY_H_


typedef enum
{
	NET_CARD_LOCAL, 	/*��������*/
	NET_CARD_WIFI,		/*��������*/
}NET_CARD;


/**
 * @brief ɾ����������
 * @netcard  0:��������  1:��������
 * @sznetcard ������������
 * @wifi_name ������������
 * @pGw ��ӵ�����
 */

int delnet_gateway(NET_CARD netcard, char *sznetcard, char* wifi_name, char* pGw);

#endif

