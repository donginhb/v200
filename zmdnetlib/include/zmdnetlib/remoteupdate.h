#ifndef __REMOTE_UPDATE_H__
#define __REMOTE_UPDATE_H__

/**
 * @file remoteupdate.h
 *
 * @brief  
 *
 * @auth mike
 *
 * @date 2013-12-19
 *
**/

/**
 * ���õ�ZMODOԶ��������������ַ
 */
#define ZMD_UPDATE_SERVER	"upgrade.meshare.com"
//#define ZMD_UPDATE_SERVER	"172.18.33.6"

/** ����������ʹ�õ�HTTP�˿�*/
#ifndef RU_HTTP_PORT
#define RU_HTTP_PORT (80)
#endif

#ifndef ROK
#define ROK (0)
#endif

#ifndef RFAILED
#define RFAILED (-1)
#endif

/** ��Ϊ�����쳣���жϵ������Ĵ���*/
#ifndef RU_RETRY_DOWNLOAD_COUNT
#define RU_RETRY_DOWNLOAD_COUNT (3)
#endif

#define MAX_HTTP_REQ_LEN (2048)

#ifndef NET_CONNECT_TIME_OUT
#define NET_CONNECT_TIME_OUT (5)
#endif

/**
 * �������Զ��������̵Ŀ���״̬
 */
typedef enum
{
	UPDATE_STAT_IDEL = 0,	/** û���ڽ���Զ������*/
	UPDATE_STAT_IN_DOWNLOAD, /** ���ڽ����ļ�����*/
	UPDATE_STAT_PAUSED,		/** Զ����������ͣ*/
	UPDATE_STAT_IN_UPDATE,	/** �ļ�������ɣ�������*/
	UPDATE_STAT_ERROR,		/** �������󣬱�����������Ӳ���*/
	
}UPDATE_STAT_E;

/**
 * �����˿������ص�ָ��
 */
typedef enum
{
	DOWNLOAD_CMD_IDEL = 0,	/** û������*/
	DOWNLOAD_CMD_START, 	/** ��ʼ����*/
	DOWNLOAD_CMD_PAUSE, 	/** ��ͣ����*/
	DOWNLOAD_CMD_RESUME, 	/** �ָ�����*/
	DOWNLOAD_CMD_CANCEL,	/** ȡ������*/
	
}DOWNLOAD_CMD_E;

/**
 * @brief ��Զ��������������ȡ����汾������Ϣ
 *
 * @param version 	���ص��°汾��
 * @param updateflag 	�Ƿ���Ҫ���£�1:�ǣ�0:��
 * @param description 	�°汾������Ϣ������updateflagΪ1ʱ������
 * @param des_len 	������Ϣ�ĳ���
 * @param ubootversion 	��ǰϵͳUBOOT�汾
 * @param kernelversion 	��ǰϵͳ�ں˰汾
 * @param fsversion 	��ǰϵͳ�ļ�ϵͳ�汾
 * @param appversion 	��ǰϵͳAPP�汾
 *
 * @return 0:��������Ϣ�ɹ���-1:��������Ϣʧ��
 */
int ru_check_update_info
( 
	char* version,
	int* updateflag, 
	char* description, 
	int* des_len,
	const char* ubootversion,
	const char* kernelversion,
	const char* fsversion,
	const char* appversion
);

/**
 * @brief ��ʼϵͳ����
 *
 * @param savefile ���������ļ���д����ļ�
 */
void ru_start_update( const char* savefile);

/**
 * @brief ��ȡ����״̬
 *
 * @param update_stat ����״̬
 * @param process ���ؽ���
 */
void ru_get_update_stat( int* update_stat, int* process );

/**
 * @brief ȡ������
 *
 * @return 0:ȡ���ɹ���1:�޷�ȡ��
 */
int ru_cancel_update();

/**
 * @brief ��ͣ����
 *
 * @return 0:��ͣ�ɹ���1:�޷���ͣ
 */
int ru_pause_update();

/**
 * @brief �ָ�����
 *
 * @return 0:�ָ��ɹ���-1:�޷��ָ�
 */
int ru_resume_update();

#endif /** __REMOTE_UPDATE_H__*/
