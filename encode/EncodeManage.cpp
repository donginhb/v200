
#include <time.h>

#include "DebugPrint.h"
#include "EncodeManage.h"
#include "VideoEncode.h"    
#include "FrontOsd.h"
#include "AiAo_Api.h"
#include "ModuleFuncInterface.h"
//#include "AVenc_Api.h"
#include "CommonFunction.h"

#include "JpegSnap.h"	//ץ��ͷ
#include "common.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>
#include "FrontOsd.h"
#include "Video_MD.h"
#include "GpioApi.h"
#include "IR_Cut.h"



#ifdef __cplusplus
extern "C" {
#endif


//����ģʽ1--CBR, 0--VBR
#define ENCODE_MODE				0  

enum{
	ENCODE_STOP = 0,
	ENCODE_STOPING,
	ENCODE_STARTING,
	ENCODE_RUNING,
};



VideoEncode  		*pVideoEncode[MAX_REC_CHANNEL];
BufferManage 		*pBufferManage[MAX_CH_STREAM];
FrontOsd			*pFrontOsdObj;
VideoEncode 		*pVideo;
Audio 		*paudio = NULL;
extern PARAMETER_MANAGE*	g_cParaManage;
extern int startupdate;
extern DeviceConfigInfo 	ConfigInfo;

char			picturemode;
extern int	FrameRate;
extern bool	Night;


/**************************
����ץ��ͨ��
**************************/
bool CreateSnapCh()
{
	JpegSnap_S *pJpep = JpegSnap_S::Instanse();
	if(pJpep != NULL)
	{
	//	pJpep->CreateSnapChn(VIDEO_ENCODING_MODE_NTSC,PIC_QVGA);
		pJpep->CreateSnapChn(VIDEO_ENCODING_MODE_AUTO,PIC_QVGA);
		 
	}
	
	return true;
}

/***************************
����ץ��ͨ��
***************************/
bool DestroySnapCh()
{

	JpegSnap_S *pJpep = JpegSnap_S::Instanse();
	
	pJpep->DestroySnapCh();
	
	return true;
}



int SnapOneChannelImage(int ch, SnapImageArg *Snap)
{
	
	JpegSnap_S *pJpep = JpegSnap_S::Instanse();

	if((NULL == pJpep) || (NULL == Snap) || (ch < 0) || (ch >=MAX_SNAP_CH_NUM)||(strlen(Snap->FileName)==0))
	{
		return -1;
	}		
	if(startupdate == 0)
	{
		printf("handleEmail thread exit file:%s line:%d pid:%d	\n",__FILE__,__LINE__,getpid());
		pthread_exit(0);
	}

	sprintf(Snap->FullFileName, "/tmp/%s", Snap->FileName);
	strcpy(Snap->path, "/tmp");
	
	unlink(Snap->FullFileName); 
	
	FILE *fp = NULL;	
	fp	= fopen(Snap->FullFileName, "w+b"); 	
	if(fp != NULL)
	{
		Snap->Result = pJpep->JpegSnapProcess((void*)fp);	
	}
	else
	{
		printf("open %s  failed\n",Snap->FullFileName);
		perror("open snap pic");		
		return -1;
	}
	
	fflush(fp);

	if(fp  != NULL)
	{
		fclose(fp );
	}
	
	fp	= NULL;
	

	return 0;

}







/*****************************************************************************
��������:
�������:��
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1, 
ʹ��˵��:
******************************************************************************/
int GetSysTime(SystemDateTime *pSysTime)
{
	GlobalGetSystemTime((datetime_setting*)pSysTime);
	return 0;
}

int StartVideo()
{
	//��ȡ�������
	CAMERA_PARA  encode_para;	
	GetEncodePara(&encode_para);
	int framerate = 0;	
	
	/* 720P */
	EncodePara_S EncPara;
	framerate = encode_para.m_ChannelPara[0].m_uFrameRate;
	if((framerate == 0)||(framerate >= 20))
	{
		framerate = 18;
	}	
	if(Night)
		FrameRate  = framerate/2;/*ҹ��ģʽ*/
	else
		FrameRate  = framerate;
	EncPara.norm = 1;
	EncPara.resolution = 0;
	EncPara.framerate = framerate;
	EncPara.VbrOrCbr = encode_para.m_ChannelPara[0].m_uEncType;
	EncPara.quality = encode_para.m_ChannelPara[0].m_uQuality;
	//EncPara.gop = 10;
	EncPara.streamtype = 1;		
	
	EncPara.bitrate = (1024*2)/(EncPara.quality+1);	/*��������*/	
	
	pVideo->StartVideoEncoder(&EncPara);	
#if 0	
	/* VGA */
	
	framerate = encode_para.m_ChannelPara[0].m_uSubFrameRate;
	if((framerate == 0)||(framerate > 25))
	{
		framerate = 25;
	}
	EncPara.norm = 1;
	EncPara.resolution = 2;
	EncPara.framerate = 12;
	EncPara.VbrOrCbr = 0;	
	EncPara.quality = encode_para.m_ChannelPara[0].m_uSubQuality;
	EncPara.bitrate = (320*2)/(EncPara.quality+1); 		/*��������*/		
	pVideo->StartVideoEncoder(&EncPara);
//#endif
#endif	

	/* QVGA */

	framerate = encode_para.m_ChannelPara[0].m_uSubFrameRate;
	if((framerate == 0)||(framerate >= 15))
	{
		framerate = 10;
	}
	EncPara.norm = 1;
	EncPara.resolution = 2;
	EncPara.framerate = framerate;
	EncPara.VbrOrCbr = encode_para.m_ChannelPara[0].m_uSubEncType;	
	EncPara.quality = encode_para.m_ChannelPara[0].m_uSubQuality;
	EncPara.bitrate =(320*2)/(EncPara.quality+1); 		/*��������*/		
	pVideo->StartVideoEncoder(&EncPara);




	
	
	CreateSnapCh();
	SnapImageArg  Snap;
	memset(&Snap,0x0,sizeof(SnapImageArg));
	sprintf(Snap.FileName, "%s", "snaponvif.jpg");
	int snap_ret = -1;
	snap_ret  = SnapOneChannelImage(0, &Snap);
	if(snap_ret == 0)
	{
		printf("path:%s\r\n", Snap.FullFileName);
	}

	return 0;
}

int StartAudio()
{

	SetAudioVolume(0,28);
	SetAudioVolume(1,28);
	
	return StartAudioEncode(0);
	
}

int StartMd()
{

	MD_HANDLE  *VideoMd = NULL;
	VideoMd = MD_HANDLE::Instance();
	if(VideoMd == NULL)
	{
		return -1;
	}
	return VideoMd->StartMotionDetection();
	
	

}



int StartOsd()
{
	if(pFrontOsdObj == NULL)
	{
		return -1;
	}	
	//��ȡ�������
	CAMERA_PARA  encode_para;	
	GetEncodePara(&encode_para);
	
/****************show time *****************************/
	COMMON_PARA  CommPara;
	g_cParaManage->GetSysParameter(SYSCOMMON_SET, &CommPara);
	pFrontOsdObj->m_TimeMode = CommPara.m_uTimeMode;
	if(encode_para.m_ChannelPara[0].m_TimeSwitch == 1)
	{
		pFrontOsdObj->ShowVideoOsd_Time(NULL);
	}

/****************show channel name********************* */
	if(encode_para.m_ChannelPara[0].m_TltleSwitch == 1)
	{
		VideoOsd_S OsdPara;
		memset(&OsdPara,0x0,sizeof(VideoOsd_S));
		//sprintf(OsdPara.chnname,"%s","IPC");
		strncpy(OsdPara.chnname, encode_para.m_ChannelPara[0].m_Title,16);
		OsdPara.x = 0;
		OsdPara.y = 0;
		pFrontOsdObj->ShowVideoOsd_Channel(&OsdPara);
	}

	pFrontOsdObj->StartOsdProcess();



/*********CoverRegion*************************/
	UpdateCoverlayOsd();


	
	return 0;

}

/*****************************************************************************
��������:
�������:��
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1, 
ʹ��˵��:
******************************************************************************/
int InitEncodeSystem()
{

     printf("InitEncodeSystem296\n");
/**********video********/
	pVideo = new VideoEncode();
	if(pVideo == NULL)
	{
		
		exit(1);
	}
	unsigned short 	PowerFreq = 60;
	bool			enable = true;
	CAMERASENSOR_PARA	  sensor;
	g_cParaManage->GetSysParameter(SENSOR_SET,&sensor);
	if(sensor.m_PowerFreq == 1)
	{   printf("=3091\n");
		PowerFreq = 50;
		enable =true;
	}
	else if(sensor.m_PowerFreq == 2)
	{  printf("=3141\n");
		PowerFreq = 60;
		enable =true;
	}
	else if(sensor.m_PowerFreq == 3)
	{ printf("=3191\n");
		enable =true;
		PowerFreq = 0;
	}
	else
	{
		enable = false;
	}
	web_sync_param_t  sync_data;
	PubGetSysParameter(WEB_SET, &sync_data);
	Set_NightSwtich(sync_data.nightvision_switch);
	
	pVideoEncode[0] = pVideo;
	pVideo->InitVideoEncoder();	
		
	pVideo->SetVideoAntiFlickerAttr(enable,PowerFreq);


	pVideo->VideoMirrorFlipSet(sensor.m_picMode);
	picturemode = sensor.m_picMode;
	CAMERA_ANALOG  lchaPara;	
	g_cParaManage->GetSysParameter(SYSANALOG_SET,&lchaPara);
	printf("InitEncodeSystem339\n");   

	SetVideoChnAnaLog(
		lchaPara.m_Channels[0].m_nBrightness,
		lchaPara.m_Channels[0].m_nContrast,
		lchaPara.m_Channels[0].m_nSaturation);	
	
      printf("InitEncodeSystem346\n");

/**********Audio********/

	paudio = new Audio();
	if(paudio == NULL)
	{
		exit(1);
	}
	paudio->InitAudio();
	StartAudio();

/**********osd********/
	pFrontOsdObj = new FrontOsd(&GetSysTime);
	if(pFrontOsdObj == NULL)
	{
		exit(1);
	}
	pFrontOsdObj->VideoOsdInit();
/**********md********/	
	InitSystemMD(); 		
	StartMd();
	
	StartEncodeSystem();

	return 0;
}
  



/*****************************************************************************
��������:
�������:��
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1, 
ʹ��˵��:
******************************************************************************/
int StartEncodeSystem()
{
	StartVideo();
	
	StartOsd();
	
	return 0;
}

/*****************************************************************************
��������: ֹͣ����ϵͳ
�������:IsFinish��ʾ����ػ�������
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1, 
ʹ��˵��:
******************************************************************************/
int StopEncodeSystem(int isSync,int isFinish)
{
	/*ֹͣ����˳�򲻿ɸı�*/
	


	/*stop osd*/
	pFrontOsdObj->StopOsdProcess();

	for(int i=0;i<4;i++)
	{
		pFrontOsdObj->DeleteCoverLayerRegion(i);
	}
	/*stop video encode*/
	pVideo->StopVideocEncode();		

	
	return 0;
}



int CreateFullVideoCoverRegion(int ch, RECT_S rect,int area)
{
	pFrontOsdObj->DeleteCoverLayerRegion(area);
	return pFrontOsdObj->CreateCoverLayerRegion(rect,area);
	

}

int DeleteFullVideoCoverRegion(int ch,int index)
{
	
	return pFrontOsdObj->DeleteCoverLayerRegion(index);
}
int ResetUserData2IFrameBySecond(int channel,int streamtype, int userid, int seconds) 
{ 

	int retval= S_FAILURE; 

	if(pBufferManage[streamtype] != NULL) 
	{ 
		retval = pBufferManage[streamtype]->StartGetFrame(userid, 2, (void *)&seconds); 
	} 
	return retval; 

}


int StartGetFrame4Record(int channel,int streamtype, int userid) 
{ 
	//int pretime = 1;
	//return pBufferManage[streamtype]->StartGetFrame(userid, 2, (void *)&pretime);	
	return pBufferManage[streamtype]->ResetUserInfo(userid);
}

/*****************************************************************************
��������:
�������:@userid:�û�id(1~MAX_BUFUSER_ID )
			  @streamtype Ϊ��������0 :720  1:VGA 2:QVGA
�������:@buffer: ����֡����ʼ��ַ
			   @pFrameInfo:���ش�֡����Ϣ
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:����ʧ�ܣ�����ʹ��buffer ָ��������ݵĻ�ȡ
******************************************************************************/
int GetSendNetFrame(int channel,int streamtype, int userid, unsigned char **buffer, FrameInfo *m_frameinfo)
{
	
	int retval= S_FAILURE;
	

	if((userid <1) ||(startupdate == 0)|| (userid >= MAX_BUFUSER_ID))
	{
		//printf(" invalid user id  %d \n", userid);
		return S_FAILURE;
	}
	if(( streamtype >= MAX_CH_STREAM)||( streamtype <0 ))
	{
		return S_FAILURE;
	}
	if(pBufferManage[streamtype] != NULL)
	{
		 retval = pBufferManage[streamtype]->GetOneFrameFromBuffer(userid, buffer, m_frameinfo);
	}
    if(m_frameinfo->Flag == 3 && m_frameinfo->talk)
    {
      //  printf("drop audio.....\n");
        return -2;
    }
	#ifdef ADJUST_STREAM
	if((pBufferManage[streamtype]->m_FrameBufferUser[userid].diffpos > FrameDiffPos)&&(retval !=-1))/*������ָ���д���ݴﵽһ����ֵ����֡*/
	{
		/*��ʼ��֡�����������ԣ����ӿ�ʼʱ��֡�Ứ����
		����ǰn(100)���²���֡��
		*/
		pBufferManage[streamtype]->m_FrameBufferUser[userid].throwframcount ++;
		if(pBufferManage[streamtype]->m_FrameBufferUser[userid].throwframcount < 100)
		{
			return 0;
		}
		VideoFrameHeader *ptr =(VideoFrameHeader *)(*buffer);

		//printf("type:%d,%d\n",ptr->m_FrameType,ptr->m_nVHeaderFlag);
		if(m_frameinfo->Flag == P_FRAME)
		{

			
			if(ptr->m_FrameType == BASE_PSLICE_REFBYENHANCE||\
				ptr->m_FrameType == ENHANCE_PSLICE_NOTFORREF)
			{
				/*�����ǹؼ�֡*/
				printf("throwfram type:%d,diffpos:%d\n",ptr->m_FrameType,pBufferManage[streamtype]->m_FrameBufferUser[userid].diffpos);
				return -2;/*����ֵΪ-2����ʾ����дָ���ֵ�Ѿ��ܴ����������
							ȡ���ݣ��ײ�����֡����*/
			}
		
		}
	}
	#endif
	return retval;
	
}
/*****************************************************************************
��������:��λָ���������û��Ķ�ָ����Ϣ
�������:streamtype Ϊ��������0 :720  1:VGA 2:QVGA
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int ResetUserData2IFrame(int channel,int streamtype, int userid)
{

	int retval= S_FAILURE;

	if((userid <1)||(startupdate == 0) || (userid >= MAX_BUFUSER_ID))
	{
		//printf(" invalid user id %d \n", userid);
		return S_FAILURE;
	}
	if(( streamtype >= MAX_CH_STREAM)||( streamtype <0 ))
	{
		return S_FAILURE;
	}
	if(streamtype==0)
	{
		RequestIFrame(streamtype,0);
	}
	else if(streamtype==1)
	{		
		RequestIFrame(1,0);
	}
	else if(streamtype==2){
		RequestIFrame(1,0);
	}
	usleep(200000);

	if(pBufferManage[streamtype] != NULL)
	{
		retval = pBufferManage[streamtype]->ResetUserInfo(userid);
	}


	return retval;

}
/*****************************************************************************
��������:ǿ������I֡
�������:streamtype Ϊ��������0 :720  1:VGA 2:QVGA
				time: 0,�������ɣ�
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/

int RequestIFrame (int streamtype,int time)
{	
	 HI_MPI_VENC_RequestIDR (streamtype,HI_TRUE);
	 return 0;
	
}
/*
��������Ƶ�����ӿڣ������Խ�ʱIPC ��ƽ̨������ƵΪ���У�
ƽ̨��IPC������ƵΪ���С�
����ʱ�������ò���:1,StartAudioEncode  2,GetSendNetFrame 3,StopAudioEncode
����ʱ�������ò���:1,StartAudioDecode  2,SendAudioStreamToDecode 3,StopAudioDecode

������Ƶ��������Ĵ�СSetAudioVolume
*/

int GetAudioParam(AudioParm *param)
{
	if((param == NULL)||(paudio == NULL))
	{
		return -1;
	}
	return paudio->GetAudioParam(param);
}

/*****************************************************************************
��������:StartAudioDecode
��������:��ʼ��Ƶ����
�������:Audiotype   0:G711  1:G726 
�������:��
��  ��   ֵ:0:�ɹ�  -1:ʧ��
ʹ��˵��:
******************************************************************************/
int	StartAudioDecode(int Audiotype)
{
	if(paudio != NULL)
	{
		//return paudio->StartAudioDecode();/*������ʱֻ֧��g711*/
		//AudioControl(1);
		printf(".....StartAudioDecode.......\n");
        return paudio->AudioOutPutOnOff(1);		
	}
	return 0;
}
/*****************************************************************************
��������:SendAudioStreamToDecode
��������:������Ƶ���ݽ��룬
�������:
�������:��
��  ��   ֵ:0:�ɹ�  -1:ʧ��
ʹ��˵��:

******************************************************************************/
int	SendAudioStreamToDecode(unsigned char *buffer,int len,int block)
{
	if((paudio != NULL)&&(buffer != NULL))
	{
		return paudio->SendAudioStreamToDecode(buffer,len,block);
	}
	return -1;
}

/*****************************************************************************
��������:StopAudioDecode
��������:������Ƶ����
�������:
�������:��
��  ��   ֵ:0:�ɹ�  -1:ʧ��
ʹ��˵��:

******************************************************************************/
int	StopAudioDecode()
{
	if(paudio != NULL)
	{
		//return paudio->StopAudioDecode();
		printf("......StopAudioDecode.......\n");
        return paudio->AudioOutPutOnOff(0);
	}
	return 0;
}

/*****************************************************************************
��������:StartAudioEncode
��������:��ʼ��Ƶ����
�������:Audiotype   0:G711  1:G726 
�������:��
��  ��   ֵ:0:�ɹ�  -1:ʧ��
ʹ��˵��:

******************************************************************************/
int	StartAudioEncode(int Audiotype)
{
	if(paudio != NULL)
	{
		return paudio->StartAudioEncode(Audiotype);
	}
	return -1;
}
/*****************************************************************************
��������:GetAudioData
��������:����Ƶbuffer��ȡһ֡����
�������:userid �û�id ���10��isfirst �Ƿ��ǵ�һ�λ�ȡ���ݣ�buf ����audiolen ���ݳ���
�������:��
��	��	 ֵ:0:�ɹ�	-1:ʧ��
ʹ��˵��: isfirst ÿ�ο�ʼ�Խ�ʱ��һ��Ϊtrue������Ϊfalse

******************************************************************************/

int	GetAudioData(int userid,bool isfirst,unsigned char *buf,int *audiolen)
{
	if(paudio != NULL)
	{
		//return paudio->GetAudioFromBuffer(userid,isfirst,buf,audiolen);
	}
	return -1;
}

/*****************************************************************************
��������:StopAudioEncode
��������:������Ƶ����
�������:
�������:��
��  ��   ֵ:0:�ɹ�  -1:ʧ��
ʹ��˵��:

******************************************************************************/
int	StopAudioEncode()
{
	if(paudio != NULL)
	{
		return paudio->StopAudioEncode();
	}
	return -1;
}

/*****************************************************************************
��������:SetAudioVolume
��������:
�������::0 :input  1:output  vol (0~31) 	  25ΪĬ������vol Խ������Խ��
�������:��
��  ��   ֵ:0:�ɹ�  -1:ʧ��
ʹ��˵��:

******************************************************************************/
int	SetAudioVolume(int inoutput,unsigned int vol)
{
	if(paudio != NULL)
	{
		
		if(inoutput == 0)
		{
			return paudio->SetInPutVolume(vol);
		}
		else if(inoutput == 1)
		{
			return paudio->SetOutPutVolume(vol);
		}		 
		
	}
	return -1;
}
static int currentVol=26;
int SetAudioVolumeLevel(int level){
	//22-31 default:26
	int vol;
	if(level<0 ||level>100){
		return -1;
	}
	vol=level/10+22;
	if(vol!=currentVol){
		printf("tell kb the volume:%d\n",vol);
		if(paudio != NULL)
		{
			currentVol=vol;			
			paudio->SetOutPutVolume(vol);	
			paudio->StartPlayFile(VOLUME_SOUND,NULL);
			return 0;
		}
	}
	else{
		printf("the voice is the same\n");
		return 0;
	}
	printf("seting failure;paudio is null\n");
	return -1;
}

bool AudioGetSpeeker()
{
	
	if(paudio == NULL)
	{
		return false;
	}
	return paudio->AudioGetSpeeker();

}
bool AudioReleaseSpeeker()
{
	
	if(paudio == NULL)
	{
		return false;
	}
	return paudio->AudioReleaseSpeeker();

}

/*****************************************************************************
��������:����BufferManage���Ѿ����ڵ����µ�I֡����������I֡
�������:streamtype Ϊ��������0 :720  1:VGA 2:QVGA
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int ResetUserData2CurrentIFrame(int channel,int streamtype, int userid)
{	
	int retval= S_FAILURE;

	if((userid <1)||(startupdate == 0) || (userid >= MAX_BUFUSER_ID))
	{
		return S_FAILURE;
	}
	if(( streamtype >= MAX_CH_STREAM)||( streamtype <0 ))
	{
		return S_FAILURE;
	}
	printf("try to find current i frame 0 :720  1:VGA 2:QVGA, streamtype:%d\n",streamtype);
	if(pBufferManage[streamtype] != NULL)
	{
		retval = pBufferManage[streamtype]->ResetUserInfo(userid);
	}
	return retval;
}

int UpdateCoverlayOsd()
{
	int i =0;
	VIDEOOSDINSERT		CoverSet;
	PubGetSysParameter(SYSOSDINSERT_SET, (void*)&CoverSet);
	RECT_S rect;
	int valid = -1;
	for(int k=0;k<4;k++)
	{
		if(k == 0)
		{
			rect.s32X = CoverSet.m_CoverLay[i].m_u16X;
			rect.s32Y = CoverSet.m_CoverLay[i].m_u16Y;
			rect.u32Width = CoverSet.m_CoverLay[i].m_u16Width;
			rect.u32Height = CoverSet.m_CoverLay[i].m_u16Height;
			valid = CoverSet.m_CoverLay[i].m_u8OverValid;
		}
		else if(k == 1)
		{
			rect.s32X = CoverSet.m_CoverLay2[i].m_u16X;
			rect.s32Y = CoverSet.m_CoverLay2[i].m_u16Y;
			rect.u32Width = CoverSet.m_CoverLay2[i].m_u16Width;
			rect.u32Height = CoverSet.m_CoverLay2[i].m_u16Height;
			valid = CoverSet.m_CoverLay2[i].m_u8OverValid;
		}
		else if(k == 2)
		{
			rect.s32X = CoverSet.m_CoverLay3[i].m_u16X;
			rect.s32Y = CoverSet.m_CoverLay3[i].m_u16Y;
			rect.u32Width = CoverSet.m_CoverLay3[i].m_u16Width;
			rect.u32Height = CoverSet.m_CoverLay3[i].m_u16Height;
			valid = CoverSet.m_CoverLay3[i].m_u8OverValid;
		}
		else if(k == 3)
		{
			rect.s32X = CoverSet.m_CoverLay4[i].m_u16X;
			rect.s32Y = CoverSet.m_CoverLay4[i].m_u16Y;
			rect.u32Width = CoverSet.m_CoverLay4[i].m_u16Width;
			rect.u32Height = CoverSet.m_CoverLay4[i].m_u16Height;
			valid = CoverSet.m_CoverLay4[i].m_u8OverValid;
		}

		if(valid)
		{
			printf("x:%d,y:%d,w:%d,h:%d,x+w:%d,y+h:%d\n",rect.s32X,rect.s32Y,rect.u32Width,rect.u32Height,\
				rect.s32X+rect.u32Width,rect.s32Y+rect.u32Height);
		
			int tmp_x = rect.s32X + rect.u32Width;
			int tmp_y = rect.s32Y + rect.u32Height;
#if defined (PT_IPC)
			if(!(CONFIG_FLIP&ConfigInfo.SupportInfo))

#elif defined (V74_NORMAL_IPC)
			if(1)
#else
			if(CONFIG_FLIP&ConfigInfo.SupportInfo)
#endif




			{
				if(picturemode == 1)
				{
					
				}
				else if(picturemode == 2)
				{
					if(tmp_y>480)
					{
						rect.s32Y = 0;
					}
					else
					{
						rect.s32Y = 480-tmp_y;
					}				
				}
				else if(picturemode == 3)
				{
					if(tmp_x>640)
					{
						rect.s32X = 0;
					}
					else
					{
						rect.s32X = 640-tmp_x;
					}

					
				}
				else if(picturemode == 4)
				{
					if(tmp_y>480)
					{
						rect.s32Y = 0;
					}
					else
					{
						rect.s32Y = 480-tmp_y;
					}	
					if(tmp_x>640)
					{
						rect.s32X = 0;
					}
					else
					{
						rect.s32X = 640-tmp_x;
					}

				}	
			}
			else
			{
				if(picturemode == 4)
				{
					
				}
				else if(picturemode == 1)
				{
					if(tmp_y>480)
					{
						rect.s32Y = 0;
					}
					else
					{
						rect.s32Y = 480-tmp_y;
					}				
				}
				else if(picturemode == 2)
				{
					if(tmp_x>640)
					{
						rect.s32X = 0;
					}
					else
					{
						rect.s32X = 640-tmp_x;
					}

					
				}
				else if(picturemode == 3)
				{
					if(tmp_y>480)
					{
						rect.s32Y = 0;
					}
					else
					{
						rect.s32Y = 480-tmp_y;
					}	
					if(tmp_x>640)
					{
						rect.s32X = 0;
					}
					else
					{
						rect.s32X = 640-tmp_x;
					}

				}		
			}
			CreateFullVideoCoverRegion(i, rect,k);
		}
	}	
	return 0;

}








/*****************************************************************************
��������:VideoMirrorFlipSet
��������:����ͼ����ͷ�ת
�������:mode   4:����1 ��ת2 ����3 ����ӷ�ת
�������:��
��  ��   ֵ:0:�ɹ�  -1:ʧ��
ʹ��˵��:

******************************************************************************/

int VideoMirrorFlipSet(int mode)
{
	printf("VideoMirrorFlipSet mode:%d\n",mode);
	if(picturemode != mode)
	{
		pVideo->VideoMirrorFlipSet(mode);
		picturemode = mode;
		UpdateCoverlayOsd();
	}

	return 0;
}
int VideoMirrorSet(bool set)
{
	pVideo->VideoMirrorSet(set);
	return 0;
}
int VideoFlipSet(bool set)
{
	pVideo->VideoFlipSet(set);
	return 0;
}
/*****************************************************************************
��������:VideoSaturationSet
��������:����ͼ�񱥺Ͷ�
�������:value   0~255 
�������:��
��  ��   ֵ:0:�ɹ�  -1:ʧ��
ʹ��˵��:

******************************************************************************/

int VideoSaturationSet(int value)
{

	//return pVideo->m_pISP->SetVideoSaturation((unsigned int)value);
	return 0;
}
/*****************************************************************************
��������:VideoColourSet
��������:����ͼ���ɫ���ߺڰ�
�������:value   1:��ɫģʽ2:�ڰ�ģʽ
�������:��
��  ��   ֵ:0:�ɹ�  -1:ʧ��
ʹ��˵��:

******************************************************************************/

int VideoColourSet(int value)
{	
	if(value == 1)
	{
		
		return pVideo->m_pISP->SetVideoSaturation(139);
	}
	else
	{
		return pVideo->m_pISP->SetVideoSaturation(0);
	}
	return -1;
}

/*****************************************************************************
��������:VideoBrightnessSet
��������:����ͼ������
�������:value   0~255 
�������:��
��  ��   ֵ:0:�ɹ�  -1:ʧ��
ʹ��˵��:

******************************************************************************/
int VideoBrightnessSet(int value)
{

	//return pVideo->m_pISP->SetVideoBrightness((unsigned int)value);
	return 0;
}
/*****************************************************************************
��������:VideoContrastSet
��������:���öԱȶ�
�������:value   0~255 
�������:��
��  ��   ֵ:0:�ɹ�  -1:ʧ��
ʹ��˵��:

******************************************************************************/
int VideoContrastSet(int value)
{

	//return pVideo->m_pISP->SetVideoContrast((unsigned int)value);
	return 0;
}

/*****************************************************************************
��������:VideoPowerSet
��������:���õ�ѹ ��ʱ��Ч
�������:value   1:50HZ 2:60HZ
�������:��
��  ��   ֵ:0:�ɹ�  -1:ʧ��
ʹ��˵��:

******************************************************************************/

int VideoPowerSet(int value)
{
	unsigned short 	PowerFreq = 60;
	bool			enable = true;	
	if(value == 1)
	{
		PowerFreq = 50;
		enable =true;
	}
	else if(value == 2)
	{
		PowerFreq = 60;
		enable =true;
	}
	else if(value== 3)
	{
		enable =true;
		PowerFreq = 0;
	}
	else
	{
		enable = false;
	}
	pVideo->SetVideoAntiFlickerAttr(enable,PowerFreq);

	return  0;
}

int SetVideoChnAnaLog(int brightness,int contrast,int saturation)
{
	
	printf("file=%s,func=%s,line=%d b=%d c=%d s=%d\n",__FILE__,__FUNCTION__,__LINE__,brightness,contrast,saturation);
	return  pVideo->m_pISP->SetVideoChnAnaLog(brightness,contrast,saturation);
}



#ifdef __cplusplus
 };
#endif

