#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <stdio.h>
#include "ping.h"
#define PACKET_SIZE     4096
//struct sockaddr_in m_dest_addr;
//struct sockaddr_in m_from_addr;
//char m_sendpacket[PACKET_SIZE];
//char m_recvpacket[PACKET_SIZE];
//pid_t m_pid;
/*����timeval�ṹ���*/
struct timeval tvSub(struct timeval timeval1,struct timeval timeval2)
{ 	
	struct timeval result;
	result = timeval1;
	if ((result.tv_usec -= timeval2.tv_usec) < 0)
	{    
		//(out->tv_usec-=in->tv_usec)
		--result.tv_sec;
		result.tv_usec += 1000000;
	}
	result.tv_sec -= timeval2.tv_sec;
	return result;
}
/*У����㷨*/
unsigned short getChksum(unsigned short *addr,int len)
{   
	int nleft=len;
	int sum=0;
	unsigned short *w=addr;
	unsigned short answer=0;

	/*��ICMP��ͷ������������2�ֽ�Ϊ��λ�ۼ�����*/
	while(nleft>1)
	{   
		sum+=*w++;
		nleft-=2;
	}
	/*��ICMP��ͷΪ�������ֽڣ���ʣ�����һ�ֽڡ������һ���ֽ���Ϊһ��2�ֽ����ݵĸ��ֽڣ����2�ֽ����ݵĵ��ֽ�Ϊ0�������ۼ�*/
	if( nleft==1)
	{   
		*(unsigned char *)(&answer)=*(unsigned char *)w;
		sum+=answer;
	}
	sum=(sum>>16)+(sum&0xffff);
	sum+=(sum>>16);
	answer=~sum;
	return answer;
}
int packIcmp(int pack_no, struct icmp* icmp)
{   
	int packsize;
	struct icmp *picmp;
	struct timeval *tval;
	
	picmp = icmp;
	picmp->icmp_type=ICMP_ECHO;
	picmp->icmp_code=0;
	picmp->icmp_cksum=0;
	picmp->icmp_seq=pack_no;
	picmp->icmp_id= getpid();
	packsize= 8 + 56;
	tval= (struct timeval *)icmp->icmp_data;
	gettimeofday(tval,NULL);    /*��¼����ʱ��*/
	picmp->icmp_cksum=getChksum((unsigned short *)icmp,packsize); /*У���㷨*/
	return packsize;
}
bool getsockaddr(const char * hostOrIp, struct sockaddr_in* sockaddr) {
	struct hostent *host;
	struct sockaddr_in dest_addr;
	unsigned long inaddr=0l;
	bzero(&dest_addr,sizeof(dest_addr));
	dest_addr.sin_family=AF_INET;
	/*�ж�������������ip��ַ*/
	if( (inaddr=inet_addr(hostOrIp))==INADDR_NONE)
	{    
		if((host=gethostbyname(hostOrIp))==NULL) /*��������*/
		{   
			//printf("gethostbyname error:%s\n", hostOrIp);
			return false;
		}
		memcpy( (char *)&dest_addr.sin_addr,host->h_addr,host->h_length);
	}
	/*��ip��ַ*/
	else if (!inet_aton(hostOrIp, &dest_addr.sin_addr)){  
		//memcpy( (char *)&dest_addr,(char *)&inaddr,host->h_length);
		//fprintf(stderr, "unknow host:%s\n", hostOrIp);
		return false;
	}
	*sockaddr = dest_addr;
	return true;
}
int unpackIcmp(char *buf,int len/*, struct IcmpEchoReply *icmpEchoReply*/)
{   
	int iphdrlen;
	struct ip *ip;
	struct icmp *icmp;
	struct timeval *tvsend, tvrecv, tvresult;
	double rtt;

	ip = (struct ip *)buf;
	iphdrlen = ip->ip_hl << 2;    /*��ip��ͷ����,��ip��ͷ�ĳ��ȱ�־��4*/
	icmp = (struct icmp *)(buf + iphdrlen);  /*Խ��ip��ͷ,ָ��ICMP��ͷ*/
	len -= iphdrlen;            /*ICMP��ͷ��ICMP���ݱ����ܳ���*/
	if(len < 8)                /*С��ICMP��ͷ�����򲻺���*/
	{   
		printf("ICMP packets\'s length is less than 8\n");
		return -1;
	}
	/*ȷ�������յ����������ĵ�ICMP�Ļ�Ӧ*/
	if( (icmp->icmp_type==ICMP_ECHOREPLY) && (icmp->icmp_id == getpid()) )
	{   
		
		tvsend=(struct timeval *)icmp->icmp_data;
		gettimeofday(&tvrecv,NULL);  /*��¼����ʱ��*/
		tvresult = tvSub(tvrecv, *tvsend);  /*���պͷ��͵�ʱ���*/
		rtt=tvresult.tv_sec*1000 + tvresult.tv_usec/1000;  /*�Ժ���Ϊ��λ����rtt*/
		if(rtt==0)
			rtt =1;
		//icmpEchoReply->rtt = rtt;
		//icmpEchoReply->icmpSeq = icmp->icmp_seq;
		//icmpEchoReply->ipTtl = ip->ip_ttl;
		//icmpEchoReply->icmpLen = len;
		
		return rtt;
	}
	else 
	{
		return -1;
	}
	return -1;
	
}
int recvPacket(int m_sockfd ,char *ip)
{       
	int len =0;
	extern int errno;
	char m_recvpacket[PACKET_SIZE]={0x0};
	struct sockaddr_in m_from_addr;
	int maxfds = m_sockfd + 1;
	int nfd  = 0;
	fd_set rset;     
	FD_ZERO(&rset);
	socklen_t fromlen = sizeof(m_from_addr);
	struct timeval timeout;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0; 
		//printf("begin recv\n");
	FD_SET(m_sockfd, &rset);
	if ((nfd = select(maxfds, &rset, NULL, NULL, &timeout))<=0) {             
		perror("select error");	printf("11111111111111\n");
		close(m_sockfd);
		return -2;   
	}
	if (FD_ISSET(m_sockfd, &rset))
	{
		if( (len = recvfrom(m_sockfd, m_recvpacket,sizeof(m_recvpacket),0, (struct sockaddr *)&m_from_addr,&fromlen)) <0)
		{	   
			if(errno==EINTR)
				//continue;
				close(m_sockfd);
			printf("22222222\n");			
			return 0;
		}
		char *fromAddr = inet_ntoa(m_from_addr.sin_addr) ;
		if (fromAddr != ip) {
			//printf("invalid address, discard\n");
			//retry again	
			printf("333333333333\n");
			close(m_sockfd);
			return 0;
		}
	}
	//printf("unpackIcmpunpackIcmpunpackIcmpunpackIcmp\n");
	close(m_sockfd);
	return unpackIcmp(m_recvpacket, len);
	
	
}
int ping(char * host)
{	
	struct protoent *protocol;	
	int m_sockfd =-1;
	int size = 10 * 1024;
	char m_sendpacket[PACKET_SIZE]={0x0};
	struct sockaddr_in m_dest_addr;
	if( (protocol = getprotobyname("icmp") )==NULL)
	{   
		perror("getprotobyname");
		return -1;
	}
	if( (m_sockfd=socket(AF_INET,SOCK_RAW,protocol->p_proto) )<0)
	{       
		perror("socket error");	
		return -1;
	}
	
	/*�����׽��ֽ��ջ�������10K��������ҪΪ�˼�С���ջ����������
	  �Ŀ�����,��������pingһ���㲥��ַ��ಥ��ַ,������������Ӧ��*/
	setsockopt(m_sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size) );
	if (!getsockaddr(host, &m_dest_addr)) {
		close(m_sockfd);
		return -1;
	}
	
	
	int packetsize = packIcmp(1, (struct icmp*)m_sendpacket); /*����ICMP��ͷ*/

	if(sendto(m_sockfd,m_sendpacket, packetsize, 0, (struct sockaddr *) &m_dest_addr, sizeof(m_dest_addr)) < 0  )
	{   
		perror("sendto error");
		close(m_sockfd);
		return -1;
	}
	return recvPacket(m_sockfd,inet_ntoa(m_dest_addr.sin_addr));	
	
	
}
#if 0
int main(int argc,char *argv[])
{
	m_pid = getpid();
	int ret = ping(argv[1]);
	if(ret == 0)
	{
		printf("ping %s result success\n",argv[1]);
	}
	else
	{
		printf("ping %s result failed\n",argv[1]);
	}
	return 0;
}
#endif
