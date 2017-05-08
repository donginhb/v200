#ifndef __MD5_INCLUDED__
#define __MD5_INCLUDED__
#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <dirent.h>
#include <arpa/inet.h> 

//MD5ժҪֵ�ṹ��
typedef struct MD5VAL_STRUCT
{
	unsigned int a;
	unsigned int b;
	unsigned int c;
	unsigned int d;
} MD5VAL;

//�����ַ�����MD5ֵ(����ָ���������ɺ�������)
MD5VAL md5(char *str, unsigned int size );
void md5String(char *str, char* out );

//MD5�ļ�ժҪ
MD5VAL md5File(FILE *fpin);

#endif

