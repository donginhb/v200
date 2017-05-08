

#ifndef _JIANGHM_TCPLIB_EXPORT_HEADER_2343987234234
#define _JIANGHM_TCPLIB_EXPORT_HEADER_2343987234234

#include "tcplibdef.h"


//�����¼�֪ͨ�ص�����ԭ��
//


/*
* Function:		��tcp���ӵ���ʱ��֪ͨҵ���Ļص�ԭ��
* Called By:
* Input:

	@session :		Tcp�Ự���
	@client_addr:	�ͻ������ӵ�ַ

* Output:

	@userData :	�û�����.���û�������ҵ���������ύ������㣬����������
				�ص��Ὣ�û����ݷ��ظ�ҵ��㣬ҵ����ʹ�ø�������ȷ�����ĸ�����
				���ӵ�����.����ʹ���û�id

* Return:		ҵ����������ҵ��Ҫ�󷵻��Ƿ���ܸ�����
				true ���ܸ�����
				false �ܾ�������
				����: �ѳ���Ԥ���������ʱ����false��
* Others:
*/
typedef bool (*onAcceptTcpLibCallback)( int &userData , void *session , sockaddr_in *client_addr ) ;


/*
* Function:		֪ͨ���ӷ��������

* Called By:
* Input:

	@userData :			�û�����
	@bConnected :		���ӽ��(always 1),ֻ�����ӳɹ��Ż���ø��¼�

* Output:
* Return:
* Others:
*/
typedef void (*onConnectTcpLibCallback)( int userData , int bConnected ) ;

/*
* Function:		�����֪ͨҵ��㣬�����ѹر�
* Called By:
* Input:

	@userData :		�û�����

* Output:
* Return:
* Others:
*/
typedef void (*onCloseTcpLibCallback)( int userData ) ;

/*
* Function:		�����֪ͨҵ��㣬��Tcp�������ݵ���
* Called By:
* Input:

	@userData :	�û�����
	@data :		�ײ���յ�����������
	@len :		�ײ���յ������ݳ���
* Output:
	@used :		���������Ѿ������˶���.(����Tcp�Ƿ���Ϣ�߽籣���ģ������п��ܻ��յ��жϵ����ݰ�.)
				���һ�ν��յ��������нضϵ����ݰ����ò���������ҵ����������㣬�Ѿ������˶�������.
				�����Ὣ�Ѵ�������ݶ�����
* Return:
* Others:
*/
typedef bool (*onReceiveTcpLibCallback)( int userData , char *data , int len , int &used ) ;

/*
* Function:		ʵ��������OnSend,�����ÿ����һ��recv�󣬻����һ�θûص�
				ҵ�������ڸûص��д���һЩ��ʱ������ѯҵ��
* Called By:
* Input:

	@userData :	�û�����

* Output:
* Return:


* Others:


*/
typedef bool (*onIdleTcpLibCallback)( int userData ) ;


/*
* Function:		�����ӿڶ�����������һ��tcp�ͻ��ˣ������������ͨѶ
				�ṩ��������ַ��3�����뷽ʽ
* Called By:
* Input:

	@userData :		�û�����,�ص�ʱ���Ὣ������ݷ��ظ�ҵ���
	@connect_cb :	onConnect�ص�
	@close_cb :		onClose�ص�
	@receive_cb:	onReceive�ص�
	@idle_cb:		onIdle�ص�

* Output:
* Return:

* Others:

*/
void *CreateClient(struct sockaddr_in *addr , int userData , int isReconnect ,
                   onConnectTcpLibCallback connect_cb , onCloseTcpLibCallback close_cb ,
                   onReceiveTcpLibCallback receive_cb , onIdleTcpLibCallback idle_cb ) ;



/*
* Function:		ֹͣһ���ͻ���,�ú����ṩ�º͵Ĺرգ�����ֹͣ��־���ȴ�
				�߳������սᡣ
ע��:			���øýӿڣ�onCloseClientCallback��������Ӧ��
				�ϲ���Ҫ�ڵ��øýӿ�ʱ�����˳�����

* Called By:
* Input:

	@clientHandle :	�ͻ��˾��

* Output:
* Return:

* Others:

*/
void StopClient( void *clientHandle ) ;

/*
* Function:		ֹͣһ���ͻ���.�ֱ��Ĺرգ�ֱ����ֹһ���߳�
ע��:				�ýӿ����ṩ���ⲿ���ã�������������һ���Ự
					�ϲ���øý�ڣ���Ҫ��������Դ���ͷţ���Ϊ
					��������ӦonCloseCallback()��֪ͨ�û��ر��ˡ�
					�ڻỰ�̱߳�����ã�Ҳ����������ֹ�߳�,���Բ����ͷ���Դ
					�������ܲ�����ã��п��ܻ�������Դ���޷��ͷš�
					�����ⲿ�̵߳���

* Called By:
* Input:

	@clientHandle :	�ͻ��˾��

* Output:
* Return:
* Others:
*/
void TerminateClient( void *clientHandle ) ;

/*
* Function:		��������һ������
ע�� :			���ͽӿ�ֻ�ܿ���clientHandle��
				�����޸�Ҳ��Ҫ����socket������ϲ�,ֻ�����ϲ�ʹ�øú����������ݷ���
				�����ϲ�ʹ��fdֱ�ӷ��ͽ����˷������������tcp���ݰ����ݵĽ���

* Called By:
* Input:

	@sessionHandle :�ͻ��˾��
	@data :			����ָ��
	@datalen :		���ݳ���

* Output:
* Return:
* Others:
*/
int ClientBlockSend( void *clientHandle , char *data , int datalen );

//bool ClientBlockRecv( void* clientHandle , char* buf , int* len );

/*
* Function:		���ÿͻ�������.���ڸ��ϲ�Կͻ��˽�������.
				�ײ��Ͽ���ǰ�ͻ������ӣ���������ַ������
				��������

* Called By:
* Input:

	@clientHandle : �ͻ��˾��
	@addr :			�Ƿ���Ҫ����Ŀ���ַ(Ĭ��=NULL��������)

* Output:
* Return:
* Others:
*/
void ResetClient( void *clientHandle , struct sockaddr_in *addr = 0 ) ;



//==============================================================
//���緽������
/*
* Function:		����һ��tcp������

* Called By:
* Input:

	@acceptcb :		���ӵ���ص�
	@onclosecb :	���ӹرջص�
	@onrecvcb :		���ӽ��յ����ݻص�
	@idlecb :		���ӿ��лص�

* Output:
* Return:	���������������NULLΪ����ʧ��

* Others:

*/
void *CreateTcpServer( onAcceptTcpLibCallback acceptcb , onCloseTcpLibCallback onclosecb ,
                       onReceiveTcpLibCallback onrecvcb , onIdleTcpLibCallback idlecb ) ;

/*
* Function:		����Tcp������

* Called By:
* Input:

	@serverHandle :	���������
	@addr :			������������ַ(0.0.0.0 Ϊ�������б�����ַ)
	@port :			�����˿�
	@isBlock :		�Ƿ��ظ��󶨸õ�ַ��ֱ���ɹ�(Ĭ��true)

* Output:
* Return:	�������
			true: �����ɹ�
			false: ����ʧ��
* Others:
*/
bool StartTcpServer( void *serverHandle , char *addr , unsigned short port , bool isBlock = true ) ;


/*
* Function:		ֹͣһ���Ự.�ýӿ��º͹رջỰ�������Ự��run����Ϊfalse
				�ȴ��߳��º���ֹ

* Called By:
* Input:

	@sessionHandle :	�Ự���

* Output:
* Return:
* Others:
*/
void StopTcpSession( void *sessionHandle ) ;

/*
* Function:		ֹͣһ���Ự.�ֱ��Ĺرգ�ֱ����ֹһ���߳�
ע��:				�ýӿ����ṩ���ⲿ���ã�������������һ���Ự
					�ϲ���øý�ڣ���Ҫ��������Դ���ͷţ���Ϊ
					��������ӦonCloseCallback()��֪ͨ�û��ر��ˡ�
					�ڻỰ�̱߳�����ã�Ҳ����������ֹ�߳�,���Բ����ͷ���Դ
					�������ܲ�����ã��п��ܻ�������Դ���޷��ͷš�
					�����ⲿ�̵߳���

* Called By:
* Input:

	@sessionHandle :	�Ự���

* Output:
* Return:
* Others:
*/

void TerminateTcpSession( void *sessionHandle ) ;


/*
* Function:		��������һ������
ע�� :			���ͽӿ�ֻ�ܿ���sessionHandle��
				�����޸�Ҳ��Ҫ����socket������ϲ�,ֻ�����ϲ�ʹ�øú����������ݷ���
				�����ϲ�ʹ��fdֱ�ӷ��ͽ����˷������������tcp���ݰ����ݵĽ���

* Called By:
* Input:

	@sessionHandle :�Ự���
	@data :			����ָ��
	@datalen :		���ݳ���

* Output:
* Return:
* Others:
*/
int TcpBlockSend( void *sessionHandle , char *data , int datalen , int timeout = 5 );					

int TcpBlockRecv( void *sessionHandle , char *buf , int buflen , int timeout ) ;

int GetSessionSock(void *sessionHandle);


#ifdef DM368
/*
* Function:		����TCP�ķ��ͻ�������С				
* Called By:	
* Input:		

	@fd :SOCK 
	@len :		��Ҫ���õķ��ͻ�������С

* Output:		
* Return:				
* Others:		
*/
static void TcpSetSendBuff( int fd, int len );

#endif
#endif

