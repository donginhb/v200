#ifdef HI3518C_MINI_IPC
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  /*Unix standard lib */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include "wifi.h"
#include "serial.h"

unsigned int count_crc16(unsigned int crc, unsigned char *buffer, unsigned int len);


void set_speed(int fd, int speed)
{
	int i = 0;
	int status;
	struct termios Opt;

	int speed_arr[] = {B0, B50,B75, B110, B134,B150 ,B200, B300, B600, B1200, B1800, B2400, 
					   B4800,B9600, B19200,B38400, B57600,B115200,B230400};/*baud rate table*/
	int name_arr[] = { 0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 
					 19200, 38400, 57600, 115200, 230400};	
	int length = sizeof(speed_arr)/sizeof(int);
	tcgetattr(fd, &Opt);

	for(; i < length; i++){
		if(speed == name_arr[i]){
			tcflush(fd, TCIOFLUSH);
			/*���ڲ����ʵ�����ͨ��ʹ��cfsetospeed��cfsetispeed��������ɡ���ȡ��������Ϣ��ͨ��cfgetispeed��cfgetospeed��������ɵ�*/
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			
			/*tcsetattr�������������ն˲���  		TCSANOW���������ݴ�����Ͼ������ı����ԡ�*/
			status = tcsetattr(fd, TCSANOW, &Opt);

			if(status != 0)
				perror("tcsetattr fd1");

			return ;
		}

		tcflush(fd, TCIOFLUSH);  /*tcflush����ˢ�壨�ӵ������뻺�棨�ն����������ѽӹܵ������û�������δ������������棨�û������Ѿ�д������δ���ͣ�.*/
	}
}

int set_Parity(int fd, int databits, int stopbits, int parity)
{
	struct termios options;

	if(tcgetattr(fd, &options) != 0){
		perror("Setup Serial 1");
		return 0;
	}
	options.c_cflag &= ~CSIZE;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw data
	switch(databits){
		case 7:
			options.c_cflag |= CS7;
			break;

		case 8:
			options.c_cflag |= CS8;
			break;

		default:
			fprintf(stderr, "Unsupported data size\n");
			return 0;
	}


	switch(parity){
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;	/* clear parity enable */
			options.c_iflag &= ~INPCK;	/* enable parity checking */
			break;

		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB);	/* set parity */
			options.c_iflag |= INPCK;	/* disable parity checking */
			break;	

		case 'e':
		case 'E':
			options.c_cflag |= PARENB;	/* Enalbe parity */
			options.c_cflag &= ~PARODD;	/* ODD check */
			options.c_iflag |= INPCK;	/* Disable parity checking */
			break;

		case 'S':
		case 's':
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break;

		default:
			fprintf(stderr,"Unsupported parity\n");
			return (0);
	}

	switch(stopbits){
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;

		case 2:
			options.c_cflag |= CSTOPB;
			break;

		default:
			fprintf(stderr,"Unsupported stop bits\n");
			return (0);
	}

	if(parity != 'n')	/*set input parity options*/
		options.c_iflag |= INPCK; 

    options.c_oflag &=~(INLCR | IGNCR | ICRNL);
	options.c_oflag &=~(ONLCR|OCRNL);
	tcflush(fd,TCIFLUSH);
	options.c_cc[VTIME] = 200; /* ���ó�ʱ200����*/   
	options.c_cc[VMIN] = 100; /*��100���ֽں��ȥ��ȡ*/

	//options.c_cc[VTIME] = 150; /* ���ó�ʱ150����*/   
	//options.c_cc[VMIN] = 0; /*��100���ֽں��ȥ��ȡ*/

	if(tcsetattr(fd, TCSANOW, &options) != 0){
		perror("SetupSerial 3");   
		return (0); 
	}

	return 1;
}

int OpenSerailDev()
{
	//int fd = open(SERIAL_DEV, O_RDWR);
	int fd = open(SERIAL_DEV, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(-1 ==fd){
		perror("Can't open serial port");
		return -1;
	}
	else
	{
		printf("open %s successful \n",SERIAL_DEV);
		//fcntl(fd,F_SETFL,0);//����ģʽ
		set_speed(fd, BAUDRATE);
		if(set_Parity(fd,8,1,'N')== 0){
		   printf("Set Parity Error\n");
    	 return 0 ;
	    }
	}
	
	return fd;
}

int SerialRead(int fd,void *buff, size_t n)
{

	int nread = 0;
	fd_set serial;
	struct timeval rdtimeout;
	int retval = -1;

	
read:		
	FD_ZERO(&serial);
	FD_SET(fd, &serial);
	rdtimeout.tv_sec = 5;
	rdtimeout.tv_usec = 0;


	if((retval = select(fd+1, &serial, NULL, NULL, &rdtimeout)) <= 0)
	{   
		if(retval == -1)
			printf("\nReadCom select error:%d\n",errno);
		if(retval == 0)//time out
			goto read;	
		
	}

	//������дBUFFER
	if(FD_ISSET(fd, &serial) != 0)
	{
		usleep(1000);
		nread = read(fd, buff, n);
		if(nread<=0 )
		{
			if (nread == 0) 
			{
				return 0;	/* End of descriptor. */
			} else {
				if (errno == EINTR || errno == EAGAIN) {
					perror("readn");
					goto read;	/* Call read() again. */
				} else {
					return -1;
				}
			}
		}
	}  
	
	printf("read in length=%d\n",nread);	
	tcflush(fd, TCIFLUSH);//��ȡ���ݺ���ջ���
#ifdef WIFI_DEBUG
	int i = 0 ;
	unsigned char *tmp = (unsigned char *) buff;
	for( ; i < nread ;i++)
		printf("%02x ", tmp[i]);
	printf("\n\n");
#endif
	return nread;
}

//�������豸д����
int SerialWrite(int fd,const void *buf, size_t n)
{
	size_t nleft;
	ssize_t nw;
	unsigned char *ptr;
	unsigned char send_buf[4096] ={0};
	unsigned int crc16 =0xffffffff;
	unsigned int crc16_ =0xffffffff;
	printf("write out length=%d\n",n);

	tcflush(fd, TCOFLUSH);//����ǰ��ջ���������ʹ����������û����ȡ
    //����CRC16
    memcpy(send_buf,buf,n);
	crc16 = count_crc16(crc16,(unsigned char *)buf,n);
	printf("%s:crc16=%d\n",__FUNCTION__,crc16);
	memcpy(send_buf+n,&crc16,sizeof(crc16));
	
#ifdef WIFI_DEBUG
	size_t i = 0 ;
	unsigned char *tmp=NULL;

	tmp = (unsigned char *)send_buf;
	for( ; i < n + sizeof(crc16);i++)
		printf("%02x ", tmp[i]);
	printf("\n\n");
#endif
	ptr = (unsigned char *)send_buf;
	nleft = n+sizeof(crc16);
	while (nleft > 0) {
		if ((nw = write(fd, ptr, nleft)) <= 0) {
			if (nw < 0 && (errno == EINTR || errno == EAGAIN)) {
				continue;	/* Call write() again. */
			} else {
				return -1;
			}
		}
		printf("write length=%d\n",nw);
		nleft -= nw;
		ptr += nw;
		usleep(1000);
	}
	
	printf("write out sucessfull length=%d\n",n + sizeof(crc16) - nleft);
	return (n - nleft);
}

#endif
