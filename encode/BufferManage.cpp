
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <errno.h>      
#include <pthread.h>        
#include <math.h>   
#include <unistd.h>
#include "BufferManage.h"        
#include "DebugPrint.h"
#include "His_Api_define.h"     
#include "ModuleFuncInterface.h"
#define BM				0    
extern int startupdate ;  
int					g_nTestFd[8] = {-1};    
int					g_nATestFd[8] = {-1};   
extern 	bool		mdalarmupload ;
extern 	int 		FrameRate ;
#define PJY_DEBUG
#ifdef PJY_DEBUG
#define DBG_PRINT(args, fmt...) printf(args, ##fmt)
#else 
#define DBG_PRINT(args, fmt...)
#endif

/*****************************************************************************
��������:���캯��
�������:
�������:��
��  ��   ֵ:��
ʹ��˵��:
******************************************************************************/
BufferManage::BufferManage()
{
	memset(&m_FrameBufferPool, 0, sizeof(FrameBufferPool));
	InitBufferLock();
	GetSysTime = NULL;
	m_channel = 0;
	m_u32PCount = 0;
#ifdef TEST_ENCODE_DATA

//	m_nTestFd = -1;              
          
#endif       
    
}    

/*****************************************************************************
��������:���캯��
�������:
�������:��
��  ��   ֵ:��
ʹ��˵��:
******************************************************************************/
BufferManage::BufferManage(DateTimeCallBack GetDatetime, int ch)
{
	memset(&m_FrameBufferPool, 0, sizeof(FrameBufferPool));
	InitBufferLock();
	GetSysTime = GetDatetime;
	m_channel = ch;

#ifdef TEST_ENCODE_DATA
     
//	m_nTestFd = -1;          

#endif 
	   
}

/*****************************************************************************
��������:ע��һ������ص�ʹ���û�
�������:@userid: �û�id(Ŀǰֻ֧��0)
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int BufferManage::RegisterUser(int userid)
{
	return 0;
}

/*****************************************************************************
��������:��ʼ�������ʹ���û���Ϣ
�������:@userid: �û�id(Ŀǰֻ֧��0)
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int BufferManage::InitUserInfo(int userid)
{
	if(userid >= MAX_BUFUSER_ID)
	{
		return -1;
	}

	m_FrameBufferUser[userid].ReadCircleNum = 0;
	m_FrameBufferUser[userid].ReadFrmIndex = 0;
	m_FrameBufferUser[userid].reserve = 0;
	
	return 0;
}

/*****************************************************************************
��������:��λָ���������û��Ķ�ָ����Ϣ
�������:@userid: �û�id(Ŀǰֻ֧��0)
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int BufferManage::ResetUserInfo(int userid)
{ 
	if(userid >= MAX_BUFUSER_ID||(startupdate == 0)) 
	{ 
		return -1; 
	} 
	AddBufferLock(); 

	if(m_FrameBufferPool.ICurIndex <= m_FrameBufferPool.CurFrmIndex) 
	{ 
		m_FrameBufferUser[userid].ReadCircleNum = m_FrameBufferPool.circlenum; 
	} 
	else 
	{ 
		m_FrameBufferUser[userid].ReadCircleNum = m_FrameBufferPool.circlenum-1; 
	} 
	m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.ICurIndex; 
	m_FrameBufferUser[userid].reserve = 0; 
	m_FrameBufferUser[userid].diffpos = 0; 
	m_FrameBufferUser[userid].throwframcount = 0; 
	ReleaseBufferLock(); 

	return 0; 
}

/*****************************************************************************
��������:��ʼ��������еĳ�Ա
�������:��
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int BufferManage::InitBuffer()
{
//	int i = 0;

	m_FrameBufferPool.circlenum = 0;
	m_FrameBufferPool.CurFrmIndex = 0;
	m_FrameBufferPool.TotalFrm = 0;
	m_FrameBufferPool.writepos = 0;
	m_FrameBufferPool.IFrmIndex = 0;     
	m_FrameBufferPool.TotalIFrm = 0;
	memset(m_FrameBufferPool.FrmList, 0, sizeof(FrameInfo)*MAX_FRM_NUM);
	memset(m_FrameBufferPool.IFrmList, 0, sizeof(unsigned short)*MAX_I_F_NUM);

	m_u32IFrameOffset = 0;

//	memset(m_FrameBufferPool.BlockContent, DEFAULT_IFRAMEOFFSET, sizeof());
/*
	for(i = 0; i < BUFF_BLOCK_NUM; i++)
	{
		m_FrameBufferPool.BlockContent.m_iframetime = 0;
		m_FrameBufferPool.BlockContent[i].m_offset = DEFAULT_IFRAMEOFFSET;
	}
*/      

	memset(m_FrameBufferUser, 0, sizeof(FrameBufferUser));

#ifdef TEST_ENCODE_DATA
	char filename[128] = {0};
	int ch = m_channel;

	sprintf(filename, "/mnt/testchannel%02d.264", m_channel);
	unlink(filename);
	g_nTestFd[ch] = open(filename, O_RDWR|O_CREAT, 0777);
	
#endif 
#ifdef TEST_AENCODE_DATA
	char afilename[128] = {0};
	int ch = m_channel;

	sprintf(afilename, "/mnt/testchannel%02d.adpcm", m_channel);
	unlink(afilename);
	g_nATestFd[ch] = open(afilename, O_RDWR|O_CREAT, 0777);
	
#endif 
  
	return 0;
}

/*****************************************************************************
��������:���������
�������:@resolution: �ֱ���
			   @audioflg: �Ƿ�����Ƶ
			   @bufsize: ����buffer ��С��Ϊ0ʱ���ݷֱ���ָ��
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:����ʧ�ܣ�һ����Ҫʹ�ô˻���أ��ϲ�һ��Ҫ����
******************************************************************************/
int BufferManage::CreateBufferPool(int resolution, int audioflg, unsigned long bufsize)
{
	if(audioflg)
	{
		m_FrameBufferPool.TotalFrm = MAX_FRM_NUM;
	}
	else
	{
		m_FrameBufferPool.TotalFrm = MAX_V_FRM_NUM;
	}

	if(bufsize == 0)
	{     
		if(RES_720P == resolution)
		{
			bufsize = BUFFER_SIZE_720P;
		}
		else if(RES_VGA == resolution)
		{
			bufsize = BUFFER_SIZE_720P/4;
		}
		else if(RES_QVGA == resolution)
		{
			bufsize = BUFFER_SIZE_720P/4;
		}
	}

	m_FrameBufferPool.bufferstart = new unsigned char[bufsize];
	if(NULL == m_FrameBufferPool.bufferstart)
	{
		KEY_INFO("CreateBufferPool: new failed!\n");
		return -1;
	}

//	KEY_INFO("m_FrameBufferPool.bufferstart = 0x%x, channel:%d\n", m_FrameBufferPool.bufferstart, m_channel);
	m_FrameBufferPool.buffersize = bufsize;

	memset(m_FrameBufferPool.bufferstart, 0, bufsize);	
	
	InitBuffer();

	return 0;
}

/*****************************************************************************
��������:���ٻ����
�������:��
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int BufferManage::DestroyBufferPool()
{
	if(NULL != m_FrameBufferPool.bufferstart)
	{
		delete []m_FrameBufferPool.bufferstart;
		m_FrameBufferPool.bufferstart = NULL;
		
		memset(&m_FrameBufferPool, 0, sizeof(FrameBufferPool));

		return 0;
	}

	return -1;
}

/*****************************************************************************
��������:��ȡһ����Ƶ֡����Ϣ
�������:@Astream: ��Ƶ���ݽṹָ��
�������:@framelen:֡����
			   @pts:ʱ���
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int BufferManage::GetAudioFrameInfo(void *Astream, int *framelen, unsigned long long *pts)
{
	AUDIO_STREAM_S *stream = (AUDIO_STREAM_S *)Astream;

	*pts = stream->u64TimeStamp;
 	*framelen = stream->u32Len ;	
	return 0;
}

/*****************************************************************************
��������:��ȡһ����Ƶ֡����Ϣ
�������:@Vstream: ��Ƶ���ݽṹָ��
�������:@frametype:֡����(I or P)
			   @framelen:֡����
			   @pts:ʱ���
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int BufferManage::GetVideoFrameInfo(void *Vstream, int *frametype, int *framelen, unsigned long long *pts)
{
	
	
	
	VENC_STREAM_S *stream =(VENC_STREAM_S *)Vstream;
	unsigned int i =0;
	if(stream)
	{	
		if(stream->pstPack[0].DataType.enH264EType == H264E_NALU_PSLICE)
		{
			*frametype = P_FRAME;
		}
		else
		{
			*frametype = I_FRAME;		
		}

		*framelen = 0;	
		
		*pts = stream->pstPack[0].u64PTS;
		for (i = 0; i < stream->u32PackCount; i++)
		{

			
			*framelen+=(stream->pstPack[i].u32Len-stream->pstPack[i].u32Offset);


			
		}		
		//printf("frame type:%d, frame len:%04d, pts: %lld\n", *frametype, *framelen, *pts);
		return 0;
	}
	return -1;
}



long long  framecount = 0;
/*****************************************************************************
��������:���һ����Ƶ֡���������
�������:@Vstream: ��Ƶ���ݽṹָ��
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int BufferManage::PutOneVFrameToBuffer(void *Vstream)
{
	VENC_STREAM_S *stream = (VENC_STREAM_S *)Vstream;
	VENC_STREAM_S *VideoTmp;
	//VENC_PACK_S *packet;
	unsigned int PacketIndex;
	int  frametype = 0;
//	int FrameId = 0;
	int FrameLen = 0;
	unsigned long long pts = 0;
	unsigned int IFrameheader[8] = {0x63643030, 0x63643031, 0x63643032, 0x63643033,0x63643034, 0x63643035, 0x63643036, 0x63643037};
	unsigned int PFrameheader[8] = {0x63643130, 0x63643131, 0x63643132, 0x63643133,0x63643134, 0x63643135, 0x63643136, 0x63643137};
	unsigned int TimeInfo = 0;

	SystemDateTime systime;
	AddBufferLock();

	//������֡����Ϣ
	if(GetVideoFrameInfo(Vstream, &frametype, &FrameLen, &pts) < 0||(startupdate == 0))
	{
		DEBUG_ERROR("GetVideoFrameInfo failed!\n");
		ReleaseBufferLock();
		return -1;
	}
	
	//printf("frame type:%d, frame len:%04d, pts: %lld\n", frametype, FrameLen, pts);
	//DEBUGMSG(BM, ("start: wrtpos:0x%x, circle:%d, curindex:%d\n", m_FrameBufferPool.writepos, m_FrameBufferPool.circlenum, m_FrameBufferPool.CurFrmIndex));
//	DEBUGMSG(0, ("putV start: m_channel = %d, writepos = 0x%x, curFrmIndex = %d\n", m_channel, m_FrameBufferPool.writepos, m_FrameBufferPool.CurFrmIndex));

#if 0
	static	int  FrameNum = 0;

	if(m_channel == 0)
	{
		FrameNum++;
		if(((FrameNum % 15) == 0))
		{
			printf(" ch 0 Cur Num %d  \n", FrameNum);	
		}	
	}
#endif 
	
	//�ж�ʣ��ռ��Ƿ��㹻����һ֡����
	if((m_FrameBufferPool.buffersize - m_FrameBufferPool.writepos) < ((unsigned long)FrameLen + sizeof(VideoFrameHeader)))
	{
		//DEBUGMSG(0, ("****m_channel:%d***wrtpos = 0x%x, len : 0x%x writepos reset to 0***** *********\n", m_channel, m_FrameBufferPool.writepos, FrameLen));

		memset(m_FrameBufferPool.bufferstart+m_FrameBufferPool.writepos,0x0,m_FrameBufferPool.buffersize - m_FrameBufferPool.writepos);
		
		m_FrameBufferPool.TotalFrm = m_FrameBufferPool.CurFrmIndex;
		m_FrameBufferPool.CurFrmIndex = 0;
		m_FrameBufferPool.circlenum += 1;
		m_FrameBufferPool.writepos = 0;
		//DEBUGMSG(0, ("m_channel :%d, write circle = %u\n", m_channel, m_FrameBufferPool.circlenum));
		//DEBUGMSG(0, ("m_channel:%d,  time:%d\n", m_channel, time(NULL)));
		
	}
	framecount++;
	
#if 0
		int i = 0;
		venc_pkt_t *pkt = NULL;
		//VENC_STREAM_S *strmp = (VENC_STREAM_S *)stream->pData;
		
		m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].venc_stream.pkt_cnt = stream->u32PackCount;
		m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].venc_stream.seq = stream->u32Seq;
		for (i = 0; i < (int)stream->u32PackCount; i++) {
			pkt = &m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].venc_stream.pkt[i];
			pkt->len = stream->pstPack[i].u32Len/*[0]*/ + stream->pstPack[i].u32Len/*[1]*/;
			pkt->type = (nalu_type_t)stream->pstPack[i].DataType.enH264EType;
		}
#endif


	//��дһ֡������Ϣ
	m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].FrmStartPos = m_FrameBufferPool.writepos;
	m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].FrmLength = FrameLen + sizeof(VideoFrameHeader);
	m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].Pts = pts;
//	printf("framecount:%lld\n",framecount);
	m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].Flag = frametype;
	
	if(GetSysTime)
	{
		GetSysTime(&systime);
		m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].hour = systime.hour;
		m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].min = systime.minute;
		m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].sec = systime.second;
	}

	if(I_FRAME == frametype)
	{
		*(unsigned int*)(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos) = IFrameheader[0];
		m_u32PCount++;
//		printf("pcount : %d , h:%02d:%02d:%02d	\n", m_u32PCount, systime.hour, systime.minute, systime.second);
		m_u32PCount = 0;
	}
	else 
	{
		*(unsigned int*)(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos) = PFrameheader[0];
		m_u32PCount++;
	}
	
	m_FrameBufferPool.writepos += 4;
	// ֡����
	*(unsigned int*)(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos) = FrameLen;

	m_FrameBufferPool.writepos += 4;
	// ʱ����Ϣ
	TimeInfo = systime.hour|(systime.minute<<8)|(systime.second<<16);

	*(unsigned int*)(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos) = TimeInfo;

	m_FrameBufferPool.writepos += 4;
	// ��һ��I֡��ƫ��
	*(unsigned int*)(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos) = m_u32IFrameOffset;

	m_FrameBufferPool.writepos += 4;
	
// ʱ���
	*(long long *)(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos) = pts;

	m_FrameBufferPool.writepos += 8;
  //������Ϣ
  unsigned int otherinfo =0;
  if(mdalarmupload) 
	  otherinfo|=0x01;
#ifdef ADJUST_STREAM
  otherinfo|=((stream->stH264Info.enRefType)<<1);
#endif	
	otherinfo|=(FrameRate<<6);
#ifdef VIDEO_AES_ENC
	if(I_FRAME == frametype)
  	{
		otherinfo|=(1<<14);//����
	}
 #endif
  *(unsigned int*)(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos) = otherinfo;
 //	printf("mdalarminfo:%d\n",mdalarminfo>>1);

	m_FrameBufferPool.writepos += 4;
// ������Ϣ
	*(unsigned int*)(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos) = frametype;//0x00; //for test

	m_FrameBufferPool.writepos += 4;	
	
	if(I_FRAME == frametype)
	{
		m_u32IFrameOffset = 0;
	}
	
	m_u32IFrameOffset +=((m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].FrmLength +7)&(~7));

	
	//������������

	VideoTmp = stream;		
	if(VideoTmp && VideoTmp->pstPack)
	{
		for(PacketIndex = 0; PacketIndex < VideoTmp->u32PackCount; PacketIndex ++)
		{
			

		memcpy(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos, 
			VideoTmp->pstPack[PacketIndex].pu8Addr+VideoTmp->pstPack[PacketIndex].u32Offset,VideoTmp->pstPack[PacketIndex].u32Len-VideoTmp->pstPack[PacketIndex].u32Offset);
			m_FrameBufferPool.writepos+=(VideoTmp->pstPack[PacketIndex].u32Len-VideoTmp->pstPack[PacketIndex].u32Offset);
			
		}
		
	}
	#ifdef VIDEO_AES_ENC
	if(FrameLen>0&& I_FRAME == frametype)
	{
		unsigned char encinput[256]={0};
		memcpy(encinput,m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos-FrameLen,256);
		AesEncrypt(encinput,m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos-FrameLen);
	}
	
	#endif
	m_FrameBufferPool.writepos = (m_FrameBufferPool.writepos + 7)&(~7);  // ��������λ8�ֽڶ���
	
	//�����I ֡�������i ֡�б�
	if(I_FRAME == frametype)
	{
		//DEBUG_INFO("m_channel:%d, curfrmindex:%d, iFrameindex:%d\n", m_channel, m_FrameBufferPool.CurFrmIndex, m_FrameBufferPool.IFrmIndex);
		m_FrameBufferPool.IFrmList[m_FrameBufferPool.IFrmIndex] = m_FrameBufferPool.CurFrmIndex;
		m_FrameBufferPool.ICurIndex = m_FrameBufferPool.CurFrmIndex;
		m_FrameBufferPool.IFrmIndex++;
		if(m_FrameBufferPool.TotalIFrm < m_FrameBufferPool.IFrmIndex)
		{
			m_FrameBufferPool.TotalIFrm = m_FrameBufferPool.IFrmIndex;
		}
		
		if(m_FrameBufferPool.IFrmIndex >= MAX_I_F_NUM)
		{
			m_FrameBufferPool.IFrmIndex = 0;
		}
	}

	//�޸�֡�����±�
	m_FrameBufferPool.CurFrmIndex++;
	if(m_FrameBufferPool.CurFrmIndex > m_FrameBufferPool.TotalFrm)
	{
		m_FrameBufferPool.TotalFrm = m_FrameBufferPool.CurFrmIndex;
		//m_FrameBufferPool.CurFrmIndex = 0;
		//m_FrameBufferPool.circlenum += 1;
	}
	 
#if 1
	if(m_FrameBufferPool.CurFrmIndex >= MAX_FRM_NUM)
	{
		//DEBUGMSG(1, ("m_channel:%d, CurFrmIndex = %d! more than MAX_FRM_NUM %lx \n", m_channel, m_FrameBufferPool.CurFrmIndex, m_FrameBufferPool.writepos));
		m_FrameBufferPool.TotalFrm = m_FrameBufferPool.CurFrmIndex;
		m_FrameBufferPool.CurFrmIndex = 0;
		m_FrameBufferPool.circlenum += 1;
		m_FrameBufferPool.writepos = 0;
	}
#endif

	ReleaseBufferLock();	 

	//printf("putV end: m_channel = %d, writepos = %ld, curFrmIndex = %d\n", m_channel, m_FrameBufferPool.writepos, m_FrameBufferPool.CurFrmIndex);
	//DEBUGMSG(BM, ("end: wrtpos:0x%x, circle:%d, curindex:%d\n", m_FrameBufferPool.writepos, m_FrameBufferPool.circlenum, m_FrameBufferPool.CurFrmIndex));
	return FrameLen;
	
}




/*****************************************************************************
��������:���һ����Ƶ֡���������
�������:@Asteam: ��Ƶ���ݽṹָ��
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int BufferManage::PutOneAFrameToBuffer(void *Astream,bool talk)
{
	AUDIO_STREAM_S *stream = (AUDIO_STREAM_S *)Astream;
	unsigned long long pts = 0;
	int framelen = 0;
	unsigned int AFrameheader[8] = {0x62773130, 0x62773131, 0x62773132, 0x62773133, 0x62773134, 0x62773135, 0x62773136, 0x62773137};
	
	SystemDateTime systime;
	AddBufferLock();

	//������Ƶ֡��Ϣ
	if(GetAudioFrameInfo(Astream, &framelen, &pts) < 0||(startupdate == 0))
	{
		ReleaseBufferLock();
		return -1;
	}	

	//fprintf(stderr, "A framelen = %d m_channel:%d\n", framelen,m_channel);
	//DEBUGMSG(0, ("putA start: m_channel = %d, writepos = 0x%x, curFrmIndex = %d\n", m_channel, m_FrameBufferPool.writepos, m_FrameBufferPool.CurFrmIndex));
	
	//�ж�ʣ��ռ��Ƿ��㹻����һ֡����
	if((m_FrameBufferPool.buffersize - m_FrameBufferPool.writepos) < ((unsigned long)framelen + sizeof(AudioFrameHeader)))
	{
		//DEBUGMSG(BM, ("****m_channel:%d****wrtpos = 0x%x, writepos reset to 0**************\n", m_channel, m_FrameBufferPool.writepos));
		//DEBUGMSG(0, ("m_channel:%d, time: %d\n", m_channel, time(NULL)));

		
		memset(m_FrameBufferPool.bufferstart+m_FrameBufferPool.writepos,0x0,m_FrameBufferPool.buffersize - m_FrameBufferPool.writepos);
		m_FrameBufferPool.TotalFrm = m_FrameBufferPool.CurFrmIndex;
		m_FrameBufferPool.CurFrmIndex = 0;
		m_FrameBufferPool.circlenum += 1;
		m_FrameBufferPool.writepos = 0;
		
	}

	//��дһ֡������Ϣ
	m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].FrmStartPos = m_FrameBufferPool.writepos;
	m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].FrmLength = framelen + sizeof(AudioFrameHeader);
	m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].Pts = pts;
	m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].Flag = A_FRAME;
    m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].talk = talk;
	if(GetSysTime)       
	{
		GetSysTime(&systime);
		m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].hour = systime.hour;
		m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].min = systime.minute;
		m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].sec = systime.second;
	}
	

	// ֡��
	*(unsigned int*)(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos) = AFrameheader[0];
	
	m_FrameBufferPool.writepos += 4;
	// ֡����
	*(unsigned int*)(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos) = framelen;

	m_FrameBufferPool.writepos += 4;
	// ʱ���
	*(long long *) (m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos) = pts;

	m_FrameBufferPool.writepos += 8;
	
	m_u32IFrameOffset +=((m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].FrmLength +7)&(~7));
	//�����������ݵ�����
	
         
	
	memcpy(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos, stream->pStream, framelen);
	m_FrameBufferPool.writepos += framelen;
	
	
	

	m_FrameBufferPool.writepos = (m_FrameBufferPool.writepos + 7)&(~7);  // ��������λ8�ֽڶ���

	//�޸�֡�����±�
	

	m_FrameBufferPool.CurFrmIndex++;
	if(m_FrameBufferPool.CurFrmIndex > m_FrameBufferPool.TotalFrm)
	{
		m_FrameBufferPool.TotalFrm = m_FrameBufferPool.CurFrmIndex;
		//m_FrameBufferPool.CurFrmIndex = 0;
		//m_FrameBufferPool.circlenum += 1;
	}

#if 1
	if(m_FrameBufferPool.CurFrmIndex >= MAX_FRM_NUM)
	{
		//DEBUGMSG(1, ("m_channel:%d, CurFrmIndex = %d! more than MAX_FRM_NUM %lx \n", m_channel, m_FrameBufferPool.CurFrmIndex, m_FrameBufferPool.writepos));
		m_FrameBufferPool.TotalFrm = m_FrameBufferPool.CurFrmIndex;
		m_FrameBufferPool.CurFrmIndex = 0;
		m_FrameBufferPool.circlenum += 1;
		m_FrameBufferPool.writepos = 0;
	}
#endif
	ReleaseBufferLock();

	//DEBUGMSG(0, ("putA end: m_channel = %d, writepos = 0x%x, curFrmIndex = %d\n", m_channel, m_FrameBufferPool.writepos, m_FrameBufferPool.CurFrmIndex));
	return 0;
}

/*****************************************************************************
��������:���һ֡�����������ݵ��������
�������:@Asteam: ��Ƶ���ݽṹָ��
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int BufferManage::PutOneBinaryDataToBuffer(char *BinaryStream, int len, unsigned long long pts)
{
	
	//�ж�ʣ��ռ��Ƿ��㹻����һ֡����
	if((m_FrameBufferPool.buffersize - m_FrameBufferPool.writepos) < (unsigned long)len)
	{

		AddBufferLock();
		m_FrameBufferPool.TotalFrm = m_FrameBufferPool.CurFrmIndex;
		m_FrameBufferPool.CurFrmIndex = 0;
		m_FrameBufferPool.circlenum += 1;
		m_FrameBufferPool.writepos = 0;
		ReleaseBufferLock();
	}

	//��дһ֡������Ϣ
	m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].FrmStartPos = m_FrameBufferPool.writepos;
	m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].FrmLength = len;
	m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].Pts = pts;
	m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].Flag = B_FRAME;

	if(GetSysTime)
	{
		//SystemDateTime systime;
		//GetSysTime(&systime);
		m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].hour = 0;//systime.hour;
		m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].min = 0;//systime.minute;
		m_FrameBufferPool.FrmList[m_FrameBufferPool.CurFrmIndex].sec = 0;//systime.second;
	}

	//�����������ݵ�����
	memcpy(m_FrameBufferPool.bufferstart + m_FrameBufferPool.writepos, BinaryStream, len);
	m_FrameBufferPool.writepos += len;


	//�޸�֡�����±�
	AddBufferLock();
	m_FrameBufferPool.CurFrmIndex++;
	if(m_FrameBufferPool.CurFrmIndex > m_FrameBufferPool.TotalFrm)
	{
		m_FrameBufferPool.TotalFrm = m_FrameBufferPool.CurFrmIndex;
		//m_FrameBufferPool.CurFrmIndex = 0;
		//m_FrameBufferPool.circlenum += 1;
	}

#if 1
	if(m_FrameBufferPool.CurFrmIndex >= MAX_FRM_NUM)
	{
		//DEBUGMSG(1, ("m_channel:%d, CurFrmIndex = %d! more than MAX_FRM_NUM\n", m_channel, m_FrameBufferPool.CurFrmIndex));
		m_FrameBufferPool.TotalFrm = m_FrameBufferPool.CurFrmIndex;
		m_FrameBufferPool.CurFrmIndex = 0;
		m_FrameBufferPool.circlenum += 1;
		m_FrameBufferPool.writepos = 0;
	}
#endif

	ReleaseBufferLock();

	return 0;
}
/*****************************************************************************
��������:���캯��
�������:@userid:�û�id(0~MAX_BUFUSER_ID, Ŀǰֻ֧��0)
�������:@buffer: ����֡����ʼ��ַ
			   @pFrameInfo:���ش�֡����Ϣ
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:����ʧ�ܣ�����ʹ��buffer ָ��������ݵĻ�ȡ
******************************************************************************/
int BufferManage::GetOneFrameFromBuffer(int userid, unsigned char **buffer, FrameInfo *pFrameInfo)
{
	if((userid > MAX_BUFUSER_ID)||(NULL == buffer)||(NULL == pFrameInfo))
	{
		DEBUG_ERROR("invalid user id = %d\n", userid);

		return -1;
	}

	AddBufferLock();
	
	/*
	�󲿷�����£������̱�д���̿죬���Զ�Ȧ����дȦ��һ������ָ���дָ��һ��
	*/
	if(m_FrameBufferUser[userid].ReadFrmIndex == m_FrameBufferPool.CurFrmIndex
		&&(m_FrameBufferUser[userid].ReadCircleNum == m_FrameBufferPool.circlenum))
	{
		ReleaseBufferLock();
		return -1;
	}

	/*
	���͵Ķ���Ȧ:д��Ȧ�����ȶ���Ȧ���󣻶�ָ��ﵽ�����ĩβ��
	*/
	if((m_FrameBufferUser[userid].ReadFrmIndex >= m_FrameBufferPool.TotalFrm) &&
		(m_FrameBufferUser[userid].ReadCircleNum < m_FrameBufferPool.circlenum))
	{		
		#if 0
		printf("<output> change cycle,ReadFrmIndex = %d,TotalFrm=%d,ReadCircleNum=%lu,circlenum=%lu\n", 
			m_FrameBufferUser[userid].ReadFrmIndex,m_FrameBufferPool.TotalFrm,
			m_FrameBufferUser[userid].ReadCircleNum,m_FrameBufferPool.circlenum);	 
		#endif
		m_FrameBufferUser[userid].ReadFrmIndex = 0;
		m_FrameBufferUser[userid].ReadCircleNum = m_FrameBufferPool.circlenum;

		if(m_FrameBufferUser[userid].ReadFrmIndex == m_FrameBufferPool.CurFrmIndex)
		{
			//*��ʾ�����ݿɶ�
			//debug_Printf("<output> no data\n");
			ReleaseBufferLock();

			return -1;
		}		 
	}

	/*
	���͵����ݸ���:����дСһȦ,��λ�ô���дλ��С
	*/
	if((m_FrameBufferPool.circlenum - m_FrameBufferUser[userid].ReadCircleNum == 1)
	  && m_FrameBufferPool.FrmList[m_FrameBufferUser[userid].ReadFrmIndex].FrmStartPos < m_FrameBufferPool.writepos)
	{
		//*��ʾ����̫��
		#if 1
		printf("--------err: date recover,ReadFrmIndex = %d,TotalFrm=%d,ReadCircleNum=%lu,circlenum=%lu\n", 
			m_FrameBufferUser[userid].ReadFrmIndex,m_FrameBufferPool.TotalFrm,
			m_FrameBufferUser[userid].ReadCircleNum,m_FrameBufferPool.circlenum);
		#endif
		//m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.CurFrmIndex;
		m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.ICurIndex;
		m_FrameBufferUser[userid].ReadCircleNum = m_FrameBufferPool.circlenum;
		ReleaseBufferLock();

		return -1;
	}
	
	/*
	��ԱȽϼ��˵����:���ǳ���,д��Ȧ��,�ȶ���Ȧ������2��
	*/
	if(m_FrameBufferPool.circlenum - m_FrameBufferUser[userid].ReadCircleNum >= 2)
	{
		//*��ʾ����̫��
		#if 0
		debug_Printf("--------err: date recover,circlenum:%lu, ReadCircleNum:%lu\n",
			m_FrameBufferPool.circlenum, m_FrameBufferUser[userid].ReadCircleNum);
		#endif
	//	m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.CurFrmIndex;
		m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.ICurIndex;
		m_FrameBufferUser[userid].ReadCircleNum = m_FrameBufferPool.circlenum;
		ReleaseBufferLock();
		return -1;
	}
	
	*buffer = m_FrameBufferPool.bufferstart + m_FrameBufferPool.FrmList[m_FrameBufferUser[userid].ReadFrmIndex].FrmStartPos;
	memcpy(pFrameInfo, &(m_FrameBufferPool.FrmList[m_FrameBufferUser[userid].ReadFrmIndex]), sizeof(FrameInfo));
	
	m_FrameBufferUser[userid].ReadFrmIndex++;
//	if((m_FrameBufferUser[userid].ReadFrmIndex >= m_FrameBufferPool.TotalFrm) &&
//		(m_FrameBufferUser[userid].ReadFrmIndex > m_FrameBufferPool.CurFrmIndex))
//	{
//		m_FrameBufferUser[userid].ReadFrmIndex = 0;
//		m_FrameBufferUser[userid].ReadCircleNum += 1;
		//*��ʾ����һȦ��
	//	DEBUGMSG(1, (" ||m_channel:%d, read circle = %d\n", m_channel, m_FrameBufferUser[userid].ReadCircleNum));
//	}
//	if(userid>1)
	//printf("%d %d\n",m_FrameBufferUser[userid].ReadFrmIndex,m_FrameBufferPool.CurFrmIndex);
//	if(abs(m_FrameBufferPool.CurFrmIndex - m_FrameBufferUser[userid].ReadFrmIndex) > 90)
//	{
//		if(userid != LOCAL_REC_ID)
//		m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.ICurIndex;
//	}
	if(m_FrameBufferUser[userid].ReadFrmIndex < m_FrameBufferPool.CurFrmIndex &&\
		m_FrameBufferUser[userid].ReadCircleNum == m_FrameBufferPool.circlenum)
		
	{
		m_FrameBufferUser[userid].diffpos = m_FrameBufferPool.CurFrmIndex -m_FrameBufferUser[userid].ReadFrmIndex;
	}
	
	else if(m_FrameBufferUser[userid].ReadFrmIndex >= m_FrameBufferPool.CurFrmIndex &&\
		m_FrameBufferUser[userid].ReadCircleNum < m_FrameBufferPool.circlenum)
		
	{
		m_FrameBufferUser[userid].diffpos = m_FrameBufferPool.TotalFrm+m_FrameBufferPool.CurFrmIndex -m_FrameBufferUser[userid].ReadFrmIndex;
	}
	//printf("diffpos:%d\n",m_FrameBufferUser[userid].diffpos);

	ReleaseBufferLock();
	
	return 0;
	
}


/*****************************************************************************
��������:������ǰ��ʱ��������ʼ��ȡ���ݵ�����λ��
�������:@userid:�û�id(0~2, Ŀǰֻ֧��0)
			   @secnum: ��ǰ������
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:�ڲ�����
******************************************************************************/
int BufferManage::CalculateReadIndexByTime(int userid, int secnum)
{
	unsigned short index = 0;
	unsigned short offset = 0;
	int saveoffset = -1;
	int diff = 0;
	int MaxIFrm = 0;
	char IsFind = 0;
	if(userid > MAX_BUFUSER_ID||(startupdate == 0))
	{
		
		return -1;
	}

	//��ϵͳʱ�ӵ����
	if(GetSysTime)
	{
		SystemDateTime systime;
		int cursec = 0;
		int tmpsec = 0;

		GetSysTime(&systime);
		cursec = systime.hour*3600 + systime.minute*60 + systime.second;
		if(m_FrameBufferPool.IFrmIndex > 0)
		{
			index = m_FrameBufferPool.IFrmIndex -1;
		}
		else
		{
			index = MAX_I_F_NUM -1;
		}
		MaxIFrm = m_FrameBufferPool.TotalIFrm;
		
		if(cursec < secnum)
		{
			KEY_INFO("OVER DAY, CalculateReadIndexByTime: secnum=%d, cursec=%d\n", secnum, cursec);
			secnum -= cursec;
			cursec = 86400 - 1;
			KEY_INFO("@@CalculateReadIndexByTime: secnum=%d, cursec=%d\n", secnum, cursec);
		}


		//���������벻��һȦ
		if(m_FrameBufferPool.circlenum == 0)
		{
			for(int i = 0; i < MaxIFrm; i++)
			{	
				offset = m_FrameBufferPool.IFrmList[index];
				tmpsec = m_FrameBufferPool.FrmList[offset].hour*3600 + 
					m_FrameBufferPool.FrmList[offset].min*60 + m_FrameBufferPool.FrmList[offset].sec;
				DEBUG_INFO("offset = %d, tmpsec = %d\n", offset, tmpsec);
				if((tmpsec < cursec))
				{
					diff = cursec - tmpsec;
					DEBUG_INFO("diff = %d\n", diff);
					if((diff >= secnum) && (diff < 86000))
					{
						IsFind = 1;
						break;
					}

					saveoffset = offset;
				}

				if(index > 0)
				{
					index--;
				}
				else
				{
					index = MAX_I_F_NUM - 1;
				}
			}

			//�����е����ݻ�������Ҫ������
			//��ӵ�һ��I ֡��ʼ����¼��
			if(IsFind == 0)
			{
				if(saveoffset != -1)
				{
					DEBUG_INFO("saveoffset = %d, offset = %d\n", saveoffset, offset);
					offset = saveoffset;
				}
				else
				{
					offset = m_FrameBufferPool.IFrmList[0];
				}
				IsFind = 1;
			}
		}
		else
		{
			for(int i = 0; i < MAX_I_F_NUM; i++)
			{	
				offset = m_FrameBufferPool.IFrmList[index];
				tmpsec = m_FrameBufferPool.FrmList[offset].hour*3600 + 
					m_FrameBufferPool.FrmList[offset].min*60 + m_FrameBufferPool.FrmList[offset].sec;
				DEBUG_INFO("offset = %d, tmpsec = %d\n", offset, tmpsec);
				if((tmpsec < cursec))
				{
					diff = cursec - tmpsec;
					DEBUG_INFO("diff = %d\n", diff);
					if((diff >= secnum) && (diff < 86000))
					{
						IsFind = 1;
						break;
					}

					saveoffset = offset;
				}

				if(index > 0)
				{
					index--;
				}
				else
				{
					index = MAX_I_F_NUM - 1;
				}
			}

			if((IsFind == 0) && (saveoffset != -1))
			{
				DEBUG_INFO("saveoffset = %d, offset = %d\n", saveoffset, offset);
				offset = saveoffset;
				IsFind = 1;
			}
		}

		DEBUG_INFO("IsFind:%d, offset = %d\n", IsFind, offset);

		//�ҵ�I ֡
		if(IsFind)
		{
			m_FrameBufferUser[userid].ReadFrmIndex = offset;
		}
		else
		{
			//û���ҵ���ӵ�ǰI ֡λ����ǰƫ��n ��I ֡
			if(m_FrameBufferPool.IFrmIndex >= 10)
			{
				index = m_FrameBufferPool.IFrmIndex - 10;
			}
			else
			{
				index = MAX_I_F_NUM - (m_FrameBufferPool.IFrmIndex - 10) - 1;
			}

			m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.IFrmList[index];
			DEBUG_INFO("m_FrameBufferPool.IFrmList[%d] = %d\n", index, m_FrameBufferPool.IFrmList[index]);
		}
	}
	//��֧��ʱ������
	else
	{
		//���������벻��һȦ
		if(m_FrameBufferPool.circlenum == 0)
		{
			if(m_FrameBufferPool.IFrmIndex >= secnum)
			{
				index = m_FrameBufferPool.IFrmIndex - secnum;
			}
			else
			{
				index = 0;
			}
		}
		else
		{
			if(m_FrameBufferPool.IFrmIndex >= secnum)
			{
				index = m_FrameBufferPool.IFrmIndex - secnum;
			}
			else
			{
				index = MAX_I_F_NUM - (m_FrameBufferPool.IFrmIndex - secnum) - 1;
			}
		}

		m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.IFrmList[index];
	}

	if(m_FrameBufferUser[userid].ReadFrmIndex <= m_FrameBufferPool.CurFrmIndex)
	{
		diff = m_FrameBufferPool.CurFrmIndex - m_FrameBufferUser[userid].ReadFrmIndex;
	}
	else
	{
		diff = m_FrameBufferPool.TotalFrm - m_FrameBufferUser[userid].ReadFrmIndex + m_FrameBufferPool.CurFrmIndex;
	}

	DEBUG_INFO("read->write diff = %d, write->read diff %d\n", diff, m_FrameBufferPool.TotalFrm - diff);
	
	//�ٽ���һ��ȷ�ϣ�����i ֡λ���Ƿ�ȷʵΪi ֡
	if(m_FrameBufferPool.FrmList[m_FrameBufferUser[userid].ReadFrmIndex].Flag != I_FRAME)
	{
		DEBUG_ERROR("invalid I frame: find index is %d, in fact is %d\n", m_FrameBufferUser[userid].ReadFrmIndex, 
			m_FrameBufferPool.IFrmList[m_FrameBufferPool.IFrmIndex]);
		
		//�������I ֡��ӵ�ǰi ֡λ�ÿ�ʼ��ȡ
		m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.IFrmList[m_FrameBufferPool.IFrmIndex];
	}

	DEBUG_INFO("find start read index is %d\n", m_FrameBufferUser[userid].ReadFrmIndex);

	//ָ��ѭ����Ȧ��
	if(m_FrameBufferUser[userid].ReadFrmIndex < m_FrameBufferPool.CurFrmIndex)
	{
		m_FrameBufferUser[userid].ReadCircleNum = m_FrameBufferPool.circlenum;
	}
	else
	{
		m_FrameBufferUser[userid].ReadCircleNum = (m_FrameBufferPool.circlenum > 0)?(m_FrameBufferPool.circlenum - 1):0;
	}


	return 0;
}

/*****************************************************************************
��������:��ʼ��ȡ����Ƶ��������ָ����ʼ��ȡ���ݵ�����λ��
�������:@userid:�û�id (0~2, Ŀǰֻ֧��0)
			  @flag: ָ����ʼ��ȡ�ķ�ʽ0--��ǰi ֡λ��,1--�ϴν���λ��
			  ֮ǰ�����һ��i ֡λ�ã�2--��ǰʱ��֮ǰ�������λ��
			  @para:flag ==2ʱΪ��ǰ��ʱ��(��λΪ��)
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:�ڸ�����¼��ʱ��Ҫ����һ��
******************************************************************************/
int BufferManage::StartGetFrame(int userid, int flag, void * para)
{
	int diff_circle = 0;
	unsigned short index = 0;
	char	 IsCurIFrameIndex = 0;
	
	if(userid > MAX_BUFUSER_ID||(startupdate == 0))
	{
		//DEBUG_ERROR("invalid user id = %d\n", userid);
		return -1;
	}


	AddBufferLock();

	//�ӵ�ǰʱ����ǰԤ¼ָ��������
	if(flag == 2)
	{
		if(*(int *)para > MAX_PRECORD_SEC)
		{
			*(int *)para = MAX_PRECORD_SEC;
		}

		CalculateReadIndexByTime(userid, *(int *)para);
		ReleaseBufferLock();
		return 0;
	}
	//�ӵ�ǰi ֡λ�ÿ�ʼ��ȡ����
	else if(flag == 0)
	{
		IsCurIFrameIndex = 1;
	}
	//���ϴζ���λ�ÿ�ʼ��ǰ����I ֡
	else if(flag == 1)
	{
		diff_circle = m_FrameBufferPool.circlenum - m_FrameBufferUser[userid].ReadCircleNum;
		if(diff_circle > 1)
		{
			IsCurIFrameIndex = 1;
			DEBUG_INFO("diff_circle = %d\n", diff_circle);
		}
		else if(diff_circle == 1) /*���һȦ*/
		{
			if(m_FrameBufferUser[userid].ReadFrmIndex <= m_FrameBufferPool.CurFrmIndex)
			{
				IsCurIFrameIndex = 1;
				DEBUG_ERROR("the diff of read and write is more than two!\n");
			}
			else
			{
				int diff = 0;
				if(m_FrameBufferPool.TotalFrm > m_FrameBufferUser[userid].ReadFrmIndex)
				{
					diff =  m_FrameBufferPool.TotalFrm - m_FrameBufferUser[userid].ReadFrmIndex + 
						m_FrameBufferPool.CurFrmIndex;
					DEBUG_INFO("diff = %d\n", diff);
					if(diff > 800)
					{
						IsCurIFrameIndex = 1;
					}
				}
				else
				{
					DEBUG_INFO("m_FrameBufferPool.TotalFrm <= m_FrameBufferUser[userid].ReadFrmIndex\n");
					IsCurIFrameIndex = 1;
				}
				
			}
		}
		else if(diff_circle == 0)
		{
			if(m_FrameBufferUser[userid].ReadFrmIndex <= m_FrameBufferPool.CurFrmIndex)
			{
				if(m_FrameBufferPool.CurFrmIndex - m_FrameBufferUser[userid].ReadFrmIndex > 800)
				{
					DEBUG_INFO("m_FrameBufferPool.CurFrmIndex - m_FrameBufferUser[userid].ReadFrmIndex > 400\n");
					IsCurIFrameIndex = 1;
				}
			}
			else
			{
				DEBUG_INFO("diff_circle == 0, m_FrameBufferUser[userid].ReadFrmIndex > m_FrameBufferPool.CurFrmIndex\n");
				IsCurIFrameIndex = 1;
			}
		}
	}

	DEBUGMSG(BM, ("StartGetFrame: IsCurIFrameIndex = %d\n", IsCurIFrameIndex));
	
	//�ӵ�ǰI ֡λ�ÿ�ʼ��ȡ
	if(IsCurIFrameIndex)
	{
		/*m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.IFrmList[m_FrameBufferPool.IFrmIndex];

		//��ͬһȦ
		if(m_FrameBufferUser[userid].ReadFrmIndex < m_FrameBufferPool.CurFrmIndex)
		{
			m_FrameBufferUser[userid].ReadCircleNum = m_FrameBufferPool.circlenum;
		}
		else
		{
			m_FrameBufferUser[userid].ReadCircleNum = (m_FrameBufferPool.circlenum > 0)?(m_FrameBufferPool.circlenum - 1):0;
		}
		
		if(m_FrameBufferPool.FrmList[m_FrameBufferUser[userid].ReadFrmIndex].Flag != I_FRAME)
		{
			m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.CurFrmIndex;
			m_FrameBufferUser[userid].ReadCircleNum = m_FrameBufferPool.circlenum;
			DEBUG_ERROR("StartGetFrame:current is not I frame! flag == 0\n");
		}*/

		m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.CurFrmIndex;
		m_FrameBufferUser[userid].ReadCircleNum = m_FrameBufferPool.circlenum;
		DEBUG_INFO("IsCurIFrameIndex = 1, dst index:%d, src index:%d\n", index, m_FrameBufferUser[userid].ReadFrmIndex);
	}
	else
	{
		index = m_FrameBufferUser[userid].ReadFrmIndex;
		char IsFind = 0;
		int totalnum = m_FrameBufferPool.TotalFrm; 

		DEBUG_INFO("@@@index = %d\n", index);
		
		//�ӵ�ǰλ����ǰ��I ֡
		for(int i = 0; i < totalnum; i++)
		{
			if(m_FrameBufferPool.FrmList[index].Flag == I_FRAME)
			{
				IsFind = 1;
				break;
			}

			if(index > 0)
			{
				index--;
			}
			else
			{
				index = m_FrameBufferPool.TotalFrm - 1;
			}
		}

		if(IsFind)
		{
			DEBUG_INFO("IsFind = 1, dst index:%d, src index:%d\n", index, m_FrameBufferUser[userid].ReadFrmIndex);
			m_FrameBufferUser[userid].ReadFrmIndex = index;
			if(m_FrameBufferUser[userid].ReadFrmIndex < m_FrameBufferPool.CurFrmIndex)
			{
				m_FrameBufferUser[userid].ReadCircleNum = m_FrameBufferPool.circlenum;
			}
			else
			{
				m_FrameBufferUser[userid].ReadCircleNum = (m_FrameBufferPool.circlenum > 0)?(m_FrameBufferPool.circlenum - 1):0;
			}
		}
		else
		{
			DEBUG_INFO("IsFind=0, dst index:%d, src index:%d\n", index, m_FrameBufferUser[userid].ReadFrmIndex);
			m_FrameBufferUser[userid].ReadFrmIndex = m_FrameBufferPool.CurFrmIndex;
			m_FrameBufferUser[userid].ReadCircleNum = m_FrameBufferPool.circlenum;
		}
	}

	DEBUG_INFO("end:m_channel:%d, read index:%d, write index:%d\n",  m_channel, m_FrameBufferUser[userid].ReadFrmIndex, m_FrameBufferPool.CurFrmIndex);
	ReleaseBufferLock();

	return 0;
}

int BufferManage::GetAvDataBlock(int userid, char **buffer, int *len)
{
	int retval = -1;

	

	return retval;
	
}

