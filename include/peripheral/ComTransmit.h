#ifndef _COM_TRANSMIT_H_
#define _COM_TRANSMIT_H_

#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct {
	unsigned long 		baudrate;	/*������*/
	unsigned char 		databit;		/*����λ��ȡֵ5��6��7��8*/
	unsigned char 		parity;		/*��żУ��λ��ȡֵ'n':��У��'o':��У��'e'żУ��*/
	unsigned char 		stopbit;		/*ֹͣλ��ȡֵ1,2*/
	unsigned char 		reserve;		/*�����ֽ�*/
}Terminal;

typedef struct
{	
	int  sysSpeed;
	unsigned long  userSpeed;
}speedPara;

typedef enum
{
	READ_TIMEOUT_F = 0,
	WRITE_TIMEOUT_F
}SELECT_TIMEOUT_F;


typedef enum
{
   BAUTRATE_1200 = 0,
   BAUTRATE_2400,
   BAUTRATE_4800,
   BAUTRATE_9600,
   BAUTRATE_19200,
   BAUTRATE_38400,
   BAUTRATE_57600,
   BAUTRATE_115200,

}SERIALBAUTRATE_E;


class CComTransmit
{
private:
	//	
	FILE 	*m_fp;	
	int 		m_fd;	
	struct timeval Rtimeout;
	struct timeval Wtimeout;
public:
	//
	CComTransmit();
	~CComTransmit();
	int OpenCom(const char *deviceName);
	void CloseCom();	
	int SetComSpeed(unsigned int speed);
	int SetComParity(int databits,int stopbits,int parity,int flowctrl);
	int SetComRawMode();
	int SetComSelectTimeOut(int sec,int usec,SELECT_TIMEOUT_F RWflag);//0--read,1--wirte
	int IOFlush();
	int ReadCom(char *ReadBuffer,int size,int *retSize);
	int WriteCom(char *WriteBuffer,int size);	
	void ConvertCR2LF(int  Switch);
	int GetPortFD();
	FILE* GetPortFP();
protected:
	//
};

#endif//_COM_TRANSMIT_H_

