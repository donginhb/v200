/******************************************************************************
  File Name     : ViVo_Api.cpp
  Version       : Initial Draft
  Last Modified :
  Description   : the functions of vi and vi inplement  
  Function List :
  History       :

******************************************************************************/

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

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

#include "His_Api_define.h"
#include "AiAo_Api.h"
#include "CommonDefine.h"


/*****************************************************************************
��������:��Ƶ��������
�������:@AiMode: AI ͨ������ģʽ
			   @AiDevId: AI �豸��
			   @ChlNum: ����ͨ����
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int AudioInputConfig(AI_PARA_E AiMode, AUDIO_DEV AiDevId, int ChlNum)
{
	AIO_ATTR_S stAttr;

	switch(AiMode)
	{
		case AI_MODE_8K:
			if(1/*ChlNum > 4*/)
			{
				stAttr.enBitwidth = AUDIO_BIT_WIDTH_8;/*��Ƶ�������ȣ���ģʽ�£��˲��������codec �Ĳ�������ƥ��)*/
			}
			else 
			{
				stAttr.enBitwidth = AUDIO_BIT_WIDTH_16;//��Ƶ�������ȣ���ģʽ�£��˲��������codec �Ĳ�������ƥ��)
			}
			
			stAttr.enSamplerate = AUDIO_SAMPLE_RATE_8000;//��Ƶ�����ʣ���ģʽ�£��˲����������ã�
			stAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;//����
			stAttr.enWorkmode = AIO_MODE_I2S_SLAVE;
			stAttr.u32EXFlag = 0;//8bit ��16bit ��չ��־��8bit ����ʱ��Ч��1����չ
			stAttr.u32FrmNum = 40;//����֡��Ŀ
			stAttr.u32PtNumPerFrm = 160;//ÿ֡�Ĳ��������
			break;
		case AI_MODE_16K:

			{
				stAttr.enBitwidth = AUDIO_BIT_WIDTH_16;//��Ƶ�������ȣ���ģʽ�£��˲��������codec �Ĳ�������ƥ��)
			}
			
			stAttr.enSamplerate = AUDIO_SAMPLE_RATE_8000;//��Ƶ�����ʣ���ģʽ�£��˲����������ã�
			stAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;//����
			stAttr.enWorkmode = AIO_MODE_I2S_SLAVE;
			stAttr.u32EXFlag = 0;//8bit ��16bit ��չ��־��8bit ����ʱ��Ч��1����չ
			stAttr.u32FrmNum = 40;//����֡��Ŀ
			stAttr.u32PtNumPerFrm = 160;//ÿ֡�Ĳ��������
			break;
			
		case AI_MODE_32k:
		default:
			return HI_FAILURE;
	}
	
	if(HI_MPI_AI_SetPubAttr(AiDevId, &stAttr) !=0)
	{
        	DEBUG_INFO("set ai %d attr err\n", AiDevId);
		return HI_FAILURE;	
	}
	
	if(HI_MPI_AI_Enable(AiDevId) != 0)/* enable ai device*/
	{
        	DEBUG_INFO("HI_MPI_AI_Enable err\n");
		return HI_FAILURE;
	}
	
	for(int i = 0;i < ChlNum; i ++)//aichn 8
	{
		if(HI_MPI_AI_EnableChn(AiDevId, i) != 0)
		{
	        	DEBUG_INFO("HI_MPI_AI_EnableChn err\n");
			return HI_FAILURE;	
		}		
	}
	
	//DEBUG_INFO(("StartupAudioInputDevice(%d)*********************ok\n",AiDevId));
	return HI_SUCCESS;
}

/*****************************************************************************
��������:��ֹ��Ƶ����
�������:@AiDevId: AI �豸��
			   @ChlNum: ����ͨ����
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int DisableAudioInput(AUDIO_DEV AiDevId, int ChlNum)
{	
	for(int i = 0;i < ChlNum; i ++)//aichn 8
	{
		if(HI_MPI_AI_DisableChn(AiDevId, i) != 0)
		{
	        	DEBUG_INFO("HI_MPI_AI_DisableChn err\n");
			return HI_FAILURE;	
		}		
	}
	
	HI_MPI_AI_Disable(AiDevId);

	return HI_SUCCESS;
}

/*****************************************************************************
��������:��Ƶ�������
�������:@AiMode: AO ͨ������ģʽ
			   @AoDevId: AO �豸��
			   @AoChn: AO ͨ����
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int AudioOutputConfig(AI_PARA_E AiMode, AUDIO_DEV AoDevId, AO_CHN AoChn, int BitMode)
{
	AIO_ATTR_S stAttr;
	HI_S32 s32ret;

	switch(AiMode)
	{
		case AI_MODE_8K:
			if(BitMode > 0)
				stAttr.enBitwidth = AUDIO_BIT_WIDTH_16;//��Ƶ�������ȣ���ģʽ�£��˲��������codec �Ĳ�������ƥ��)
			else 
				stAttr.enBitwidth = AUDIO_BIT_WIDTH_8;
			
			stAttr.enSamplerate = AUDIO_SAMPLE_RATE_8000;//��Ƶ�����ʣ���ģʽ�£��˲����������ã�
			stAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;//����
			stAttr.enWorkmode = AIO_MODE_I2S_MASTER;
			stAttr.u32EXFlag = 0;//8bit ��16bit ��չ��־��8bit ����ʱ��Ч��1����չ
			stAttr.u32FrmNum = 10;//����֡��Ŀ
			stAttr.u32PtNumPerFrm =160;//ÿ֡�Ĳ��������
			break;
		case AI_MODE_16K:
			stAttr.enBitwidth = AUDIO_BIT_WIDTH_16;//��Ƶ�������ȣ���ģʽ�£��˲��������codec �Ĳ�������ƥ��)
			stAttr.enSamplerate = AUDIO_SAMPLE_RATE_16000;//��Ƶ�����ʣ���ģʽ�£��˲����������ã�
			stAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;//����
			stAttr.enWorkmode = AIO_MODE_I2S_SLAVE;
			stAttr.u32EXFlag = 0;//8bit ��16bit ��չ��־��8bit ����ʱ��Ч��1����չ
			stAttr.u32FrmNum = 30;//10;//����֡��Ŀ
			stAttr.u32PtNumPerFrm = 320;//160;//ÿ֡�Ĳ��������
			break;
		case AI_MODE_32k:
		default:
			return HI_FAILURE;
	}
	
	/* set ao public attr*/
	s32ret = HI_MPI_AO_SetPubAttr(AoDevId, &stAttr);
	if(HI_SUCCESS != s32ret)
	{
		DEBUG_INFO("set ao %d attr err:0x%x\n", AoDevId,s32ret);
		return HI_FAILURE;
	}
	
	/* enable ao device*/
	s32ret = HI_MPI_AO_Enable(AoDevId);
	if(HI_SUCCESS != s32ret)
	{
		DEBUG_INFO("enable ao dev %d err:0x%x\n", AoDevId, s32ret);
		return HI_FAILURE;
	}

	/* enable ao channel*/
	s32ret = HI_MPI_AO_EnableChn(AoDevId, AoChn);
	if(HI_SUCCESS != s32ret)
	{
		DEBUG_INFO("enable ao dev %d err:0x%x\n", AoDevId, s32ret);
		return HI_FAILURE;
	}

#ifdef Y_BOARD    
	/* enable ao channel*/
	s32ret = HI_MPI_AO_EnableChn(AoDevId, 2);
	if(HI_SUCCESS != s32ret)
	{
		DEBUG_INFO("enable ao dev %d err:0x%x\n", AoDevId, s32ret);
		return HI_FAILURE;
	}
#endif
	return HI_SUCCESS;
}

/*****************************************************************************
��������:��ֹ��Ƶ���
�������:@AoDevId: AO �豸��
			   @AoChn: AO ͨ����
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int DisableAudioOutput(AUDIO_DEV AoDevId, AO_CHN AoChn)
{
	HI_S32 s32Ret;
	
	s32Ret = HI_MPI_AO_DisableChn(AoDevId, AoChn);
	if(HI_SUCCESS != s32Ret)
	{
		DEBUG_INFO("disable ao channel %d err:0x%x\n", AoDevId, s32Ret);
		return HI_FAILURE;
	}

	s32Ret = HI_MPI_AO_Disable(AoDevId);
	if(HI_SUCCESS != s32Ret)
	{
		DEBUG_INFO("disable ao dev %d err:0x%x\n", AoDevId, s32Ret);
		return HI_FAILURE;
	}

	return HI_SUCCESS;
}


/*****************************************************************************
��������:��Ƶ�����������
�������:@AoDevId: AO �豸��
			   @AoChn: AO ͨ����
			   @Aidev:AI �豸��
			   @Aichn:AI ͨ����
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int Ai2AoHandler(int Aidev, int Aichn, int Aodev, int Aochn)
{
	HI_S32 s32ret;
	AUDIO_FRAME_S stFrame;

	/* get audio frame form ai chn */
	s32ret = HI_MPI_AI_GetFrame(Aidev, Aichn,&stFrame,NULL,HI_IO_BLOCK);
	if(HI_SUCCESS != s32ret)
	{   
		printf("get ai frame err:0x%x ai(%d,%d)\n",s32ret,Aidev, Aichn);       
		return HI_FAILURE;
	}

	printf("AiAo_proc:get ai frame ok %d wth : %d  md : %d  len :%d  \n" , stFrame.u32Seq, stFrame.enBitwidth, stFrame.enSoundmode, stFrame.u32Len);
	
	/* send audio frme to ao */
	s32ret = HI_MPI_AO_SendFrame(Aodev, Aochn, &stFrame, HI_IO_BLOCK);
	if (HI_SUCCESS != s32ret)
	{   
		printf("ao send frame err:0x%x\n",s32ret);      
		return HI_FAILURE;
	}
		
    return HI_SUCCESS;
}

