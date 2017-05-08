
#ifndef _JIANGHM_TCPLIB_TCPLIB_DEFINE_HEADER_32439287324432
#define _JIANGHM_TCPLIB_TCPLIB_DEFINE_HEADER_32439287324432


#ifdef _WIN32

#pragma warning( disable : 4786 )

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define mysocketclose	closesocket
#define MYEWOULDBLOCK	WSAEWOULDBLOCK
#define MYEINTR			WSAEINTR
#define socklen_t		int
#define RELEASE_CPU( var )		Sleep( (var) )


#else

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>

#define mysocketclose		close
#define MYEWOULDBLOCK		EWOULDBLOCK
#define MYEINTR				EINTR
#define mysock_len			socklen_t
#define RELEASE_CPU(var)	usleep((var)*1000)



#ifdef HAY_DEBUG
	#define TCP_PRINTF(fmt, args...) fprintf(stderr, "\033[1;32m             NETLIB TCP DEBUG(%s:%d):             \033[0m" fmt, __func__, __LINE__, ## args)

#else
	#define TCP_PRINTF(fmt, args...) 
#endif

#define ERR_TCP(fmt, args...) fprintf(stderr, "\033[1;31m             NETLIB TCP RROR(%s:%d):             \033[0m" fmt, __func__, __LINE__, ## args)



#endif//_WIN32


#ifndef uint32
#define	uint8	unsigned char
#define uint16	unsigned short
#define uint32	unsigned int
#endif


//ֱ�Ӵ��ڴ��л�ȡ32λ��16λ��ֵ
//��ҪΪ�˽���޷������δ4�ֽڶ����Э��Ľṹ��
//ֱ��ȡ��ֵ����������������⡣
#define GET_UINT32( var , buf )	((uint8*)(&var))[0] = buf[0] ;\
								((uint8*)(&var))[1] = buf[1] ;\
								((uint8*)(&var))[2] = buf[2] ;\
								((uint8*)(&var))[3] = buf[3] ;


#define GET_UINT16( var , buf ) ((uint8*)(&var))[0] = buf[0] ; \
								((uint8*)(&var))[1] = buf[1] ;


#define SET_UINT32( var , buf ) buf[0] = ((uint8*)(&var))[0] ;\
								buf[1] = ((uint8*)(&var))[1] ;\
								buf[2] = ((uint8*)(&var))[2] ;\
								buf[3] = ((uint8*)(&var))[3] ;


#define SET_UINT16( var , buf ) buf[0] = ((uint8*)(&var))[0] ; \
								buf[1] = ((uint8*)(&var))[1] ;

#ifndef DECLARE_SINGLEOBJ
//���������
//-------------------------------------
//  ��ͷ�ļ�������
//	DECLARE_SINGLEOBJ( CSampleClass ) ;
//	��CPP�ļ��ж��徲̬����
//	IMPLEMENT_SINGLEOBJ( CSampleClass ) ;
//	ע�ⵥ����getInstanceΪ���̰߳�ȫ��
//  ����������̳߳�ʼ����ʱ�����һ��
//-------------------------------------
#define DECLARE_SINGLEOBJ(type)		\
public:	\
	static type* m_instance ;\
	static type* getInstance(){\
		if( NULL == m_instance ){\
			m_instance = new type() ;\
		}\
		return m_instance ;\
	};\
	static void release(){\
		if( m_instance){\
			delete m_instance ;\
			m_instance = NULL ;\
		}\
	};

#define IMPLEMENT_SINGLEOBJ(type) \
	type* type::m_instance = NULL ;

#endif


//------------------------------------------
// �ڴ������Ժ�
// ���������ڴ�й¶
// debugMalloc��debugFree������һ��
// ���������ü��������ص�ǰ�û��ж��ٸ�mallocδ
// ����free����
// �����ϲ����Ҳ����DebugMalloc,��DebugFree��
// �������ڴ�ķ���������
// �ر�DEBUG_MALLOC�꣬��ʹ��ϵͳĬ�ϵ�malloc��free
//------------------------------------------
#define DEBUG_MALLOC
#ifdef  DEBUG_MALLOC
#define DebugMalloc debugMalloc
#define DebugFree	debugFree

//�����Ҫʹ���ڴ���Ժ꣬���庯��
void	*debugMalloc( int size ) ;
void	debugFree( void *pmem )  ;

#else

#define DebugMalloc	malloc
#define DebugFree	free

#endif	//DEBUG_MALLOC

//================================
//�����Ƿ�֧��720P����
#ifdef _WIN32
//#define SUPPORT_720P
#endif


//����ѡ��
//================================
//������ΪIPC����NVR����lib
//���ΪNVR����lib���򿪴˺�
//���ΪIPC����lib���رմ˺�

#define IPC_CLIENT_COUNT		1
#define MAX_STREAM_USER			18

//===============================
//������Ƶͨ������
typedef enum
{
    VGA_CHN_TYPE = 0 ,
    QVGA_CHN_TYPE = 1 ,
    D720P_CHN_TYPE = 2 ,
    ALARM_CHN_TYPE = 3 ,
    RECORD_CHN_TYPE = 4 ,
    D1080P_CHN_TYPE = 5
} ZMD_CHN_TYPE ;

//���建����������
typedef enum
{
    MAIN_CHN_BUF = 1 ,
    SUB_CHN_BUF = 2,
    HIGH_CHN_BUF = 3
} ZMD_BUF_TYPE ;


#define		INVALID_SESSION			-1
#define		INVALID_USERID			-1

//֧�ֵ����ͻ����������
#define MAX_NET_TCP_USER			12
//֧�ֵ������Ƶ��������
#define MAX_MEDIA_PLAYER			6

//tcp�Ự�����������ݵĻ�������С
//����720P���߸���ߴ���Ƶ��I֡�Ƚϴ�
//���Ըû�������ö�������㹻Щ��
#if (defined APP3518) || (defined APP3511) 
#define TCP_RECV_BUF_SIZE			256*1024
#endif

#ifdef DM368
#define TCP_RECV_BUF_SIZE			1024*1024
#endif

#ifdef APP3531
#define TCP_RECV_BUF_SIZE			1024*1024
#endif

//�û�������������
#define CLIENT_NAME_LEN				16
//���뻺��������
#define CLIENT_PASS_LEN				16

//���ô�ipc��ȡ���ݵ�����������
//�������˺궨���ʱ����δ�յ���������Ϊ
//�����쳣���Ͽ�����
#define CLIENT_IDLE_TIME			5

#endif

