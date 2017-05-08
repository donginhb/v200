/**
 * @file remote.c
 *
 * @brief  ����Զ����������߼�ҵ��
 *
 * @auth mike
 *
 * @date 2013-12-19
 *
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include "remoteupdate.h"
#include "upgrademodule.h"


/** ȫ�ֱ���������*/
extern int errno;
static int g_retry_download_count;		/** ���жϵ������Ĵ���*/
static char g_download_server[128];		/** ���������������ַ*/
static int g_download_port;				/** ��������������˿�*/
static char g_download_page[512];		/** ���������ӦHTTP ҳ��*/
UPDATE_STAT_E g_update_stat;			/** �Զ�����״̬*/
int g_update_file_len;					/** Ҫ���ص������ļ����ܳ�*/
int g_download_file_len;				/** �����ص������ļ��ĳ���*/
UPGRADECHECKSTRUCT g_UpdateFileHeader;  /** �����������ļ���ͷ*/
char g_save_file_name[128];				/** �����ļ��洢���ļ���*/
DOWNLOAD_CMD_E g_download_cmd;			/** ���ؿ���ָ��*/

/** ����������*/
static void* ru_do_start_update( void* arg );

static void ru_set_io_timeout( int socket, int seconds )
{
    struct timeval tv;

    if( seconds > 0 )
    {
        tv.tv_sec  = seconds;
        tv.tv_usec = 0;
        
        (void)setsockopt( socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv) );
        (void)setsockopt( socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv) );
    }
}

static int ru_connect_server( int fd, const struct sockaddr *serv_addr, socklen_t addrlen )
{
    int err = 0, flags = 0;
	
    fd_set rset;
    fd_set wset;
    socklen_t optlen;	
    struct timeval tv;
	
	int timeout = NET_CONNECT_TIME_OUT;
	
    /* make socket non-blocking */
    flags = fcntl( fd, F_GETFL, 0 );
    if( fcntl(fd, F_SETFL, flags|O_NONBLOCK) == RFAILED )
    {
        return RFAILED;
    }

    /* start connect */
    if( connect(fd, serv_addr, addrlen) < 0 )
    {
        if( errno != EINPROGRESS )
        {
            return RFAILED;
        }

        tv.tv_sec = timeout;
        tv.tv_usec = 0;
		
        FD_ZERO( &rset );
        FD_ZERO( &wset );
        FD_SET( fd, &rset );
        FD_SET( fd, &wset );

        /* wait for connect() to finish */
        if( (err = select(fd + 1, &rset, &wset, NULL, &tv)) <= 0 )
        {
            /* errno is already set if err < 0 */
            if (err == 0)
            {
                errno = ETIMEDOUT;
            }
            return RFAILED;
        }

        /* test for success, set errno */
        optlen = sizeof(int);
        if( getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &optlen) < 0 )
        {
            return RFAILED;
        }
        if( err != 0 )
        {
            errno = err;
            return RFAILED;
        }
    }

    /* restore blocking mode */
    if( fcntl(fd, F_SETFL, flags) == RFAILED )
    {
        return RFAILED;
    }	
	
    return ROK;
}

static int ru_open_socket( const char* hostname, int port )
{	
	int fd = -1;
	int error_code = 0;
	struct addrinfo  hints;
	struct addrinfo* res0;
	struct addrinfo* res;
	char port_string[12] = {0};

	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;
	hints.ai_addrlen = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	if( 0 == port )
		port = 80;
	
	sprintf( port_string, "%d", port );

	/** DNS����������������ַ */
	error_code = getaddrinfo( hostname, port_string, &hints, &res0 );
	if( error_code )
	{
		fprintf( stderr, "getaddrinfo failed[%s,%s]: %s\n", hostname,port_string, gai_strerror(error_code) );
		return RFAILED;
	}

	for( res = res0; res; res = res->ai_next )
	{
		fd = socket( res->ai_family, res->ai_socktype, res->ai_protocol );
		if( fd < 0 )
		{	
			perror("create socket failed");
			continue;
		}
		
		if( ru_connect_server(fd, res->ai_addr, res->ai_addrlen) < 0 )
		{
			close(fd);
			fd = -1;
			continue;
		}
		
		break;
	}

	freeaddrinfo(res0);

	if( fd < 0 )
	{
		return RFAILED;
	}

	/** set recv/send timeout */
	ru_set_io_timeout( fd, NET_CONNECT_TIME_OUT );
	
	return fd;

}

static int ru_send_req_to_server( int fd, char* http_req, int len )
{
	ssize_t ret;
	
	if( (NULL == http_req) || (len <= 0) )
	{
		printf("ru_send_req_to_server: Invalid param\n");
		return RFAILED;
	}

	printf("%s\n", http_req);
	
    if( (ret = send(fd, http_req, len, 0)) < 0 )
    {
        if (errno == EINTR)
        {
            fprintf( stderr, "ru_send_req_to_server: operation aborted!\n" );
        }
        else if (errno == EAGAIN)
        {
            fprintf( stderr, "ru_send_req_to_server: network write error,the operation timed out!\n" );
        }
        else
        {
            fprintf( stderr, "ru_send_req_to_server: network write error: %s\n", strerror(errno) );
        }
		
        return RFAILED;
    }
    else if( (size_t)ret == len )
    {
        return ROK;
    }
    else /* 0 <= error_code < len */
    {
        fprintf( stderr, "ru_send_req_to_server write error!" );
        return RFAILED;
    }
}

static int ru_recv_response_from_server( int fd, char* buff, size_t buffsize )
{
    int recv_len = 0, total_len = 0;	
	
	while( total_len < buffsize )
	{
		recv_len = recv( fd, buff + total_len, buffsize - total_len, 0 );
		if( recv_len < 0 )
		{
        	if( errno == EINTR )
            {
                fprintf( stderr, "ru_recv_response_from_server: operation aborted\n" );
            }
            else if( errno == EAGAIN )
            {
                fprintf( stderr, "ru_recv_response_from_server: network read error,the operation timed out\n" );
            }
            else
            {
                fprintf( stderr, "ru_recv_response_from_server: network read error: %s\n", strerror(errno) );
            }
			
            return total_len;
		}
		else if( 0 == recv_len ) /* socket close */
		{
			printf("ru_recv_response_from_server: socket closed!\n");
			return total_len;
		}
		else
		{
			total_len += recv_len;
		}
	}
	
    return total_len;
}

static int ru_create_check_update_info_req
( 
	char* http_req,
	const char* ubootversion,
	const char* kernelversion,
	const char* fsversion,
	const char* appversion
)
{
	const char* format = "GET /AppUpdateInfo?device_version=%s;%s;%s;%s HTTP/1.1\r\n"
						 "Accept: text/html, application/xhtml+xml, */*\r\n"
						 "Host: %s:%d\r\n"
						 "Connection: close\r\n\r\n";
	
	if( NULL == http_req )
	{
		printf("ru_create_check_update_info_req: Invalid param\n");
		return RFAILED;
	}

	snprintf( http_req, MAX_HTTP_REQ_LEN, format, ubootversion, kernelversion, 
				fsversion, appversion, ZMD_UPDATE_SERVER, RU_HTTP_PORT );

	return ROK;
	
}

static int ru_create_start_update_req( char* http_req, int offset )
{
	const char* format = "GET %s HTTP/1.1\r\n"
						 "Accept: text/html, application/xhtml+xml, */*\r\n"
						 "Accept-Language: zh-CN\r\n"
						 "Host: %s:%d\r\n"
						 "Range: bytes=%d-\r\n"
						 "Connection: Keep-Alive\r\n\r\n";
	
	if( NULL == http_req )
	{
		printf("ru_create_start_update_req: Invalid param\n");
		return RFAILED;
	}

	snprintf( http_req, MAX_HTTP_REQ_LEN, format, g_download_page, g_download_server, g_download_port, offset );

	return ROK;
}

static int ru_save_download_file( const char* filename, char* buff, int len )
{
	int fd, ret;

	fd = open( filename, O_RDWR|O_CREAT, 0666 );
	if( fd < 0 )
	{
		printf( "ru_save_download_file: open file %s faild\r\n", filename );
		return RFAILED;
	}
	
	lseek( fd, 0, SEEK_END );
	
	ret = write( fd, buff, len );
	if( ret != len )
	{
		printf( "ru_save_download_file: write file %s faild\r\n", filename );		
		close( fd );
		return RFAILED;
	}
	
	close( fd );
	return ROK;
	
}

static void ru_rm_download_file( const char* filename )
{
	int fd;

	fd = open( filename, O_RDWR );
	if( fd < 0 )
	{
		printf( "ru_rm_download_file: open file %s faild\r\n", filename );
		return;
	}

	ftruncate( fd, 0 );

	close( fd );
	
}

static int ru_check_download_file( const char* file_name, UPGRADECHECKSTRUCT* header )
{
	FILE* fp = NULL;
	
	fp = fopen( file_name, "rb" );
	if( fp != NULL )
	{
		if( CheckUpdateFileMD5(header, fp) < 0 )
		{
			fclose(fp);
			return RFAILED;
		}
		else
		{
			fclose(fp);
			return ROK;
		}
	}

	printf("ru_check_download_file: open file failed!\n");
	
	return RFAILED;
}

static int ru_resume_download_update_file( int sock_fd, const char* file_name )
{
	char* buff, *pos;
	int resp_len = 0, ret = 0;
	
	buff = (char* )malloc(20*1024);
	memset( buff, 0x00, sizeof(20*1024) );
	
	g_download_cmd = DOWNLOAD_CMD_START;
	
	/** 1. �����Ļ�Ҫ����HTTPͷ*/
	resp_len = ru_recv_response_from_server( sock_fd, buff, 2048);
	if( resp_len < 0 )
	{
		printf( "ru_resume_download_update_file: recv failed\n" );
		goto Failed;
	}

	/** ����HTTP��BODY,�������ļ�*/
	pos = strstr( buff, "\r\n\r\n" ); 
	if( NULL == pos )
	{
		printf( "ru_resume_download_update_file: can not find update file\n" );
		goto Failed;
	}
	
	/* ��ʱposָ�������ļ���ͷ*/
	pos += strlen("\r\n\r\n"); 

	/** ����HTTPͷ�󽫺����д���ļ�*/
	ret = ru_save_download_file( file_name, pos, 2048-(pos-buff) );
	if( ret < 0 )
	{
		printf( "ru_resume_download_update_file: save file failed!\n" );
		goto Failed;
	}

	g_download_file_len += 2048-(pos-buff);
	
	/** 3. ���������ļ���ͬʱд�ļ�*/
	while( DOWNLOAD_CMD_START == g_download_cmd )
	{
		memset( buff, 0x00, 20*1024 );
		resp_len = ru_recv_response_from_server( sock_fd, buff, 20*1024 );
		if( 20*1024 != resp_len )
		{
			if( resp_len > 0 )
			{
				g_download_file_len += resp_len;
				ret = ru_save_download_file( file_name, buff, resp_len );
				if( ret < 0 )
				{
					printf( "ru_resume_download_update_file: save file failed!\n" );
					goto Failed;
				}
			}
			
			/** �����ǽ�����ɣ�socket�Ͽ���Ҳ�����ǽ���ʱ��������*/
			printf( "ru_resume_download_update_file: recv %d data\n", g_download_file_len );
			break;
		}
		else
		{
			g_download_file_len += resp_len;
			ret = ru_save_download_file( file_name, buff, resp_len );
			if( ret < 0 )
			{
				printf( "ru_resume_download_update_file: save file failed!\n" );
				goto Failed;
			}
		}
	}


	/** �˳����տ������յ���ͣ����ȡ������*/
	if( DOWNLOAD_CMD_CANCEL == g_download_cmd )
	{
		goto Failed;
	}
	else if( DOWNLOAD_CMD_PAUSE == g_download_cmd )
	{
		goto Failed;
	}
	
	/** �п��������ع������޷������ˣ���Ҫ��������*/
	if( g_download_file_len != g_update_file_len )
	{
		printf( "ru_download_update_file: recv file size %d is not same with http %d!\n", 
					g_download_file_len, g_update_file_len );

		if( g_download_file_len > g_update_file_len )
		{
			g_download_file_len = 0;
			ru_rm_download_file( g_save_file_name );
		}
		
		goto Failed;
	}

	/** 4. MD5У��*/
	ret = ru_check_download_file( file_name, &g_UpdateFileHeader );
	if( ret < 0 )
	{
		printf( "ru_resume_download_update_file: check file failed!\n" );
		g_download_file_len = 0;
		ru_rm_download_file( g_save_file_name );
		goto Failed;
	}
	
	free( buff );
	return ROK;

Failed:
	free( buff );
	return RFAILED;
}

/**
 * @brief ���лָ�ϵͳ�������߳�
 */
static void* ru_do_resume_update( void* arg )
{
	int sock_fd = -1, res;
	char http_req[MAX_HTTP_REQ_LEN] = {0};		
	pthread_t pid;
	
	/** 1. ����״̬�Ի�ӦIE��״̬��ѯ*/
	g_update_stat = UPDATE_STAT_IN_DOWNLOAD;
	
	/** 2. ���ӵ�����������*/
	sock_fd = ru_open_socket( g_download_server, g_download_port );
	if( sock_fd < 0 )
	{
		printf( "ru_do_resume_update: connect to server failed\n" );
		if( DOWNLOAD_CMD_CANCEL == g_download_cmd )
		{
			g_update_stat = UPDATE_STAT_IDEL;
		}
		else
		{
			//g_update_stat = UPDATE_STAT_ERROR;
		}
		
		goto Failed;
	}
	
	/* �û��Ѿ�ȡ������*/
	if( (DOWNLOAD_CMD_CANCEL == g_download_cmd) || (DOWNLOAD_CMD_PAUSE == g_download_cmd) )
	{
		goto Failed;
	}

	/** 3. ����ϵ����������ȡ�����ļ�*/
	res = ru_create_start_update_req( http_req, g_download_file_len );
	if( RFAILED == res )
	{
		printf( "ru_do_resume_update: create start update requst failed\n" );
		goto Failed;
	}

	/** 4. ���Ϳ�ʼ������������������*/
	res = ru_send_req_to_server( sock_fd, http_req, strlen(http_req) );
	if( RFAILED == res )
	{
		printf( "ru_do_resume_update: send request to server failed\n" );
		goto Failed;
	}
	
	/* �û��Ѿ�ȡ������*/
	if( (DOWNLOAD_CMD_CANCEL == g_download_cmd) || (DOWNLOAD_CMD_PAUSE == g_download_cmd) )
	{
		goto Failed;
	}

	/** 5. �������������ļ�*/
	res = ru_resume_download_update_file( sock_fd, g_save_file_name );
	if( RFAILED == res )
	{
		printf( "ru_do_start_update: download update file failed\n" );
		goto Failed;
	}

	printf("ru_do_resume_update: Download File Success!\n");

	/** 6. ��������*/
	g_update_stat = UPDATE_STAT_IN_UPDATE;
	close( sock_fd ); 
	
	sleep(2); 	/** ʹ�ܹ���ʱ��IE��������*/
	UpdateToFlash( g_UpdateFileHeader.m_filetype );
	
	return 0;

Failed:
	/** �˳����տ������յ���ͣ����ȡ������*/
	if( DOWNLOAD_CMD_CANCEL == g_download_cmd )
	{
		printf("ru_do_resume_update: download has been canceld!\n");
		ru_rm_download_file( g_save_file_name );
		g_update_stat = UPDATE_STAT_IDEL;		
	}
	else if( DOWNLOAD_CMD_PAUSE == g_download_cmd )
	{
		printf("ru_do_resume_update: download has been paused!\n");
		g_update_stat = UPDATE_STAT_PAUSED;
	}
	else
	{
		//ru_rm_download_file( g_save_file_name );
		//g_update_stat = UPDATE_STAT_ERROR;
		/** �ϵ�����*/
		sleep(2);
		if( 0 == g_download_file_len )
		{
			/** �����̸߳������½���Զ���Զ�����*/
			if( pthread_create(&pid, NULL, &ru_do_start_update, NULL) < 0 )
			{
				printf("ru_do_start_update: create pthread for update failed!\n");
				g_update_stat = UPDATE_STAT_ERROR;
			}
		}
		else
		{
			/** �����̸߳������½���Զ���Զ�����*/
			if( pthread_create(&pid, NULL, &ru_do_resume_update, NULL) < 0 )
			{
				printf("ru_do_start_update: create pthread for update failed!\n");
				g_update_stat = UPDATE_STAT_ERROR;
			}
		}
	}

	if( -1 != sock_fd)
		close( sock_fd );
	
	return 0;
}


static int ru_download_update_file( int sock_fd, const char* file_name )
{
	char* buff, *pos;
	int resp_len = 0, ret = 0, header_len = 0;
	
	buff = (char* )malloc(20*1024);
	memset( buff, 0x00, sizeof(20*1024) );
		
	/** 1. ����Ҫ��ȡHTTPͷ�Լ������ļ���ͷ*/
	header_len = 2048+sizeof(UPGRADECHECKSTRUCT);
	resp_len = ru_recv_response_from_server( sock_fd, buff, header_len );
	if( header_len != resp_len )
	{
		/** �����ļ���ͷ���ղ����������˴���*/
		printf( "ru_download_update_file: recv failed\n" );
		goto Failed;
	}
	
	/** 2. ͷ�ļ��а�����HTTP��Ϣ�������ļ�ͷ�������������ļ�*/
	pos = strstr( buff, "Content-Length:" );
	if( NULL == pos )
	{
		printf( "ru_download_update_file: can not find Content-Length\n" );
		goto Failed;
	}

	g_update_file_len = atoi( pos+strlen("Content-Length:") );
	printf("ru_download_update_file:total file len = %d\n", g_update_file_len );

	/** ����HTTP��BODY,�������ļ�*/
	pos = strstr( buff, "\r\n\r\n" ); 
	if( NULL == pos )
	{
		printf( "ru_download_update_file: can not find update file\n" );
		goto Failed;
	}
	
	/* ��ʱposָ�������ļ���ͷ*/
	pos += strlen("\r\n\r\n"); 
	memcpy( &g_UpdateFileHeader, pos, sizeof(UPGRADECHECKSTRUCT));
	
	if( CheckUpdateVersion(&g_UpdateFileHeader) < 0 )
	{
		/** �ļ�ͷУ��ʧ��*/
		printf( "ru_download_update_file: file version check failed!\n" );
		goto Failed;
	}

	g_download_file_len = header_len-(pos-buff);

	/** ���ѽ��յ���2K�ļ��еĳ�ȥHTTP���ļ�ͷΪ����д���ļ�*/
	pos += sizeof(UPGRADECHECKSTRUCT);
	ret = ru_save_download_file( file_name, pos, header_len-(pos-buff) );
	if( ret < 0 )
	{
		printf( "ru_download_update_file: save file failed!\n" );
		goto Failed;
	}
	
	/** 3. ���������ļ���ͬʱд�ļ�*/
	while( DOWNLOAD_CMD_START == g_download_cmd )
	{
		memset( buff, 0x00, 20*1024 );
		resp_len = ru_recv_response_from_server( sock_fd, buff, 20*1024 );
		if( 20*1024 != resp_len )
		{
			if( resp_len > 0 )
			{
				g_download_file_len += resp_len;
				ret = ru_save_download_file( file_name, buff, resp_len );
				if( ret < 0 )
				{
					printf( "ru_download_update_file: save file failed!\n" );
					goto Failed;
				}
			}
			
			/** �����ǽ�����ɣ�socket�Ͽ���Ҳ�����ǽ���ʱ��������*/
			printf( "ru_download_update_file: recv %d data\n", g_download_file_len );
			break;
		}
		else
		{
			g_download_file_len += resp_len;
			ret = ru_save_download_file( file_name, buff, resp_len );
			if( ret < 0 )
			{
				printf( "ru_download_update_file: save file failed!\n" );
				goto Failed;
			}
		}
		
	}


	/** �˳����տ������յ���ͣ����ȡ������*/
	if( DOWNLOAD_CMD_CANCEL == g_download_cmd )
	{
		goto Failed;
	}
	else if( DOWNLOAD_CMD_PAUSE == g_download_cmd )
	{
		goto Failed;
	}
	
	/** �п��������ع������޷������ˣ���Ҫ��������*/
	if( g_download_file_len != g_update_file_len )
	{
		printf( "ru_download_update_file: recv file size %d is not same with http %d!\n", 
					g_download_file_len, g_update_file_len );
		
		goto Failed;
	}

	/** 4. MD5У��*/
	ret = ru_check_download_file( file_name, &g_UpdateFileHeader );
	if( ret < 0 )
	{
		printf( "ru_download_update_file: check file failed!\n" );
		g_download_file_len = 0;
		ru_rm_download_file( g_save_file_name );
		goto Failed;
	}
	
	free( buff );
	return ROK;

Failed:
	free( buff );
	return RFAILED;
}

static int ru_parse_check_update_response
(
	int sock_fd,
	char* version,
	int* updateflag, 
	char* description, 
	int* des_len
)
{
	int resp_len = 0, ret = 0;
	char* buff, *pos, *end;
	char download_addr[1024] = {0};
	char download_url[128] = {0};
	
	buff = (char* )malloc(20*1024);
	memset( buff, 0x00, sizeof(20*1024) );

	/** 1. ���ռ��汾���µ���Ӧ*/
	resp_len = ru_recv_response_from_server( sock_fd, buff, 20*1024 - 1 );
	if( resp_len <= 0 )
	{
		printf( "ru_parse_check_update_response: recv response failed!\n" );
		goto Failed;
 	}

	printf( "%s\n", buff );
	
	/** 2. ����UpdateFlag�����ȷ���а汾�����ټ������������ֶ�*/
	if( 200 != atoi(buff+strlen("HTTP/1.1")) )
	{
		printf( "ru_parse_check_update_response: get error response code %d!\n",  atoi(buff+strlen("HTTP/1.1")) );
		goto Failed;
	}
	
	pos = strstr( buff, "<UpdateFlag>" );
	if( NULL == pos)
	{
		printf( "ru_parse_check_update_response: parse UpdateFlag failed!\n" );
		goto Failed;
	}

	*updateflag = atoi( pos + strlen("<UpdateFlag>") );
	if( 0 == *updateflag )
	{
		printf("ru_parse_check_update_response: current version is newest!\n");
		free( buff );
		return ROK;
	}

	/* 3. �����°汾��ϢAppVersion �ֶ�*/
	pos = strstr( buff, "<AppVersion>" );
	if( NULL == pos)
	{
		printf( "ru_parse_check_update_response: parse AppVersion start failed!\n" );
		goto Failed;
	}

	sscanf( pos + strlen("<AppVersion>"), "%[^<]", version );


	/* 3. ����������ϢDescription */
	pos = strstr( buff, "<Description>" );
	if( NULL == pos)
	{
		printf( "ru_parse_check_update_response: parse Description start failed!\n" );
		goto Failed;
	}

	end = strstr( buff, "</Description>" );
	if( NULL == end )
	{
		printf( "ru_parse_check_update_response: parse Description end failed!\n" );
		goto Failed;
	}

	pos += strlen("<Description>");
	*des_len = end - pos;
	strncpy( description, pos, *des_len );

	/* 4. ���������°汾�ĵ�ַ*/
	pos = strstr( buff, "<DownloadAddress>" );
	if( NULL == pos)
	{
		printf( "ru_parse_check_update_response: parse DownloadAddress failed!\n" );
		goto Failed;
	}

	sscanf( pos + strlen("<DownloadAddress>"), "%[^<]", download_addr );

	/** ������URL�н�����������ַ��ҳ�� */
	ret = sscanf( download_addr+strlen("http://"), "%[^/]%s", download_url, g_download_page );
	if( 2 != ret )
	{
		printf( "ru_parse_check_update_response: DownloadAddress url %s format is wrong!\n", download_addr );
		goto Failed;
	}

	/** ���صĵ�ַ����ʹ�õ�ָ���Ķ˿�*/
	if( strchr(download_url, ':') )
	{
		sscanf( download_url, "%[^:]:%d", g_download_server, &g_download_port );
	}
	else
	{
		strcpy( g_download_server, download_url );
		g_download_port = RU_HTTP_PORT;
	}
	
	printf("ru_parse_check_update_response:\n");
	printf("version= %s\n", version);
	printf("updateflag= %d\n", *updateflag);
	printf("description= %s\n", description);
	printf("des_len= %d\n", *des_len);
	printf("DownloadAddress= %s\n", download_addr);
	printf("server = %s\n", g_download_server);
	printf("port = %d\n", g_download_port);
	
	free( buff );
	return ROK;

Failed:
	free( buff );
	return RFAILED;
}

/**
 * @brief ����ϵͳ�������߳�
 */
static void* ru_do_start_update( void* arg )
{
	int sock_fd = -1, res;
	char http_req[MAX_HTTP_REQ_LEN] = {0};		
	pthread_t pid;
	
	/** 1. ����״̬�Ի�ӦIE��״̬��ѯ*/
	g_update_stat = UPDATE_STAT_IN_DOWNLOAD;
	ru_rm_download_file( g_save_file_name );
	
	/** 2. ���ӵ�����������*/
	sock_fd = ru_open_socket( g_download_server, g_download_port );
	if( sock_fd < 0 )
	{
		printf( "ru_do_start_update: connect to server failed\n" );
		
		if( DOWNLOAD_CMD_CANCEL == g_download_cmd )
		{
			g_update_stat = UPDATE_STAT_IDEL;
		}
		else
		{
			//g_update_stat = UPDATE_STAT_ERROR;
		}

		goto Failed;
	}

	/* �û��Ѿ�ȡ������*/
	if( (DOWNLOAD_CMD_CANCEL == g_download_cmd) || (DOWNLOAD_CMD_PAUSE == g_download_cmd) )
	{
		goto Failed;
	}

	/** 3. �����ȡ�°汾��ϢHTTP����*/
	res = ru_create_start_update_req( http_req, 0 );
	if( RFAILED == res )
	{
		printf( "ru_do_start_update: create start update requst failed\n" );
		goto Failed;
	}

	/** 4. ���Ϳ�ʼ������������������*/
	res = ru_send_req_to_server( sock_fd, http_req, strlen(http_req) );
	if( RFAILED == res )
	{
		printf( "ru_do_start_update: send request to server failed\n" );
		goto Failed;
	}
	
	/* �û��Ѿ�ȡ������*/
	if( (DOWNLOAD_CMD_CANCEL == g_download_cmd) || (DOWNLOAD_CMD_PAUSE == g_download_cmd) )
	{
		goto Failed;
	}

	/** 5. ���������ļ�*/
	res = ru_download_update_file( sock_fd, g_save_file_name );
	if( RFAILED == res )
	{
		printf( "ru_do_start_update: download update file failed\n" );
		goto Failed;
	}

	printf("ru_do_start_update: Download File Success!\n");

	/** 6. ��������*/
	g_update_stat = UPDATE_STAT_IN_UPDATE;
	close( sock_fd ); 
	sleep(2); 	/** ʹ�ܹ���ʱ��IE��������*/
	UpdateToFlash( g_UpdateFileHeader.m_filetype );
	
	return 0;

Failed:
	/** �˳����տ������յ���ͣ����ȡ������*/
	if( DOWNLOAD_CMD_CANCEL == g_download_cmd )
	{
		printf("ru_do_start_update: download has been canceld!\n");
		ru_rm_download_file( g_save_file_name );
		g_update_stat = UPDATE_STAT_IDEL;		
	}
	else if( DOWNLOAD_CMD_PAUSE == g_download_cmd )
	{
		printf("ru_do_start_update: download has been paused!\n");
		g_update_stat = UPDATE_STAT_PAUSED;
	}
	else
	{
		//ru_rm_download_file( g_save_file_name );
		//g_update_stat = UPDATE_STAT_ERROR;
		/** �ϵ�����*/
		sleep(2);
		if( 0 == g_download_file_len )
		{
			/** �����̸߳������½���Զ���Զ�����*/
			if( pthread_create(&pid, NULL, &ru_do_start_update, NULL) < 0 )
			{
				printf("ru_do_start_update: create pthread for update failed!\n");
				g_update_stat = UPDATE_STAT_ERROR;
			}
		}
		else
		{
			/** �����̸߳������½���Զ���Զ�����*/
			if( pthread_create(&pid, NULL, &ru_do_resume_update, NULL) < 0 )
			{
				printf("ru_do_start_update: create pthread for update failed!\n");
				g_update_stat = UPDATE_STAT_ERROR;
			}
		}
	}

	if( -1 != sock_fd)
		close( sock_fd );
	
	return 0;
}

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
)
{
	int sock_fd, res;
	char http_req[MAX_HTTP_REQ_LEN] = {0};
	
	if( (NULL == version) || (NULL == updateflag) || 
		(NULL == description) ||(NULL == des_len) )
	{
		printf( "ru_check_update_info: invalied argment\n" );
		return RFAILED;
	}

	/** 1. ���ӵ�����������*/
	sock_fd = ru_open_socket( ZMD_UPDATE_SERVER, RU_HTTP_PORT );
	if( sock_fd < 0 )
	{
		printf( "ru_check_update_info: connect to server failed\n" );
		return RFAILED;
	}

	/** 2. �����ȡ�°汾��ϢHTTP����*/
	res = ru_create_check_update_info_req( http_req, ubootversion, kernelversion, fsversion, appversion );
	if( RFAILED == res )
	{
		printf( "ru_check_update_info: create check update info requst failed\n" );
		close( sock_fd );
		return RFAILED;
	}

	/** 3. ���ͻ�ȡ�°汾��Ϣ��������������*/
	res = ru_send_req_to_server( sock_fd, http_req, strlen(http_req) );
	if( RFAILED == res )
	{
		printf( "ru_check_update_info: send request to server failed\n" );		
		close( sock_fd );
		return RFAILED;
	}

	/** 4. ������������������Ӧ��Ϣ������*/
	res = ru_parse_check_update_response( sock_fd, version, updateflag, description, des_len );
	if( RFAILED == res )
	{
		printf( "ru_check_update_info: parse response failed\n" );		
		close( sock_fd );
		return RFAILED;
	}
	
	close( sock_fd );
	return ROK;
}

/**
 * @brief ��ʼϵͳ����
 */
void ru_start_update( const char* savefile )
{
	pthread_t pid;

	/* ������ܵ�ǰһ������������Ϣ*/
	g_update_file_len = 0;
	g_download_file_len = 0;
	g_retry_download_count = 0;
	g_update_stat = UPDATE_STAT_IDEL;
	g_download_cmd = DOWNLOAD_CMD_START;
	memset( &g_UpdateFileHeader, 0x00, sizeof(UPGRADECHECKSTRUCT) );
	
	strcpy( g_save_file_name, savefile );

	/** �����̸߳���Զ���Զ�����*/
	if( pthread_create(&pid, NULL, &ru_do_start_update, NULL) < 0 )
	{
		printf("ru_start_update: create pthread for update failed!\n");
		g_update_stat = UPDATE_STAT_ERROR;
	}

	return;
}

/**
 * @brief ��ȡ����״̬
 *
 * @param update_stat ����״̬
 * @param process ���ؽ���
 */
void ru_get_update_stat( int* update_stat, int* process )
{
	if( (NULL == update_stat) || (NULL == process) )
		return;
	
	*update_stat = g_update_stat;

	if( g_update_file_len != 0 )
		*process = (g_download_file_len*100)/g_update_file_len;
	else
		*process = 0;
	
	return;
}

/**
 * @brief ȡ������
 *
 * @return 0:ȡ���ɹ���-1:�޷�ȡ��
 */
int ru_cancel_update()
{
	/** ֻ�д�������״̬����ȡ��*/
	if( UPDATE_STAT_IN_UPDATE != g_update_stat )
	{
		g_download_cmd = DOWNLOAD_CMD_CANCEL;
		return ROK;
	}
	else
	{
		return RFAILED;
	}
}

/**
 * @brief ��ͣ����
 *
 * @return 0:��ͣ�ɹ���-1:�޷���ͣ
 */
int ru_pause_update()
{
	/** ֻ�д�������״̬������ͣ*/
	if( UPDATE_STAT_IN_DOWNLOAD == g_update_stat )
	{
		g_download_cmd = DOWNLOAD_CMD_PAUSE;
		return ROK;
	}
	else
	{
		return RFAILED;
	}
}

/**
 * @brief �ָ�����
 *
 * @return 0:�ָ��ɹ���-1:�޷��ָ�
 */
int ru_resume_update()
{
	pthread_t pid;
	
	/** ֻ�д�������״̬������ͣ*/
	if( UPDATE_STAT_PAUSED == g_update_stat )
	{
		printf("ru_resume_update: begin to resume download, has recv file %d\n", g_download_file_len );
		
		g_download_cmd = DOWNLOAD_CMD_START;

		/* һ���ļ���û�յ����൱��������*/
		if( 0 == g_download_file_len )
		{
			/** �����̸߳������½���Զ���Զ�����*/
			if( pthread_create(&pid, NULL, &ru_do_start_update, NULL) < 0 )
			{
				printf("ru_resume_update: create pthread for update failed!\n");
				g_update_stat = UPDATE_STAT_ERROR;
			}
		}
		else
		{
			/** �����̸߳������½���Զ���Զ�����*/
			if( pthread_create(&pid, NULL, &ru_do_resume_update, NULL) < 0 )
			{
				printf("ru_resume_update: create pthread for update failed!\n");
				g_update_stat = UPDATE_STAT_ERROR;
			}
		}
		
		return ROK;
	}
	else
	{
		return RFAILED;
	}
}

