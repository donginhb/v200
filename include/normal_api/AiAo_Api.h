/******************************************************************************
  File Name     : ViVo_Api.h
  Version       : Initial Draft 1.0
  Last Modified :
  Description   : Function declare of the vi and vo
  Function List :
  History       :

******************************************************************************/
#ifndef _AIAO_API_H_
#define _AIAO_API_H_

#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_sys.h"
#include "hi_comm_vo.h"
#include "hi_comm_vi.h"
#include "hi_comm_region.h"
#include "hi_comm_venc.h"
#include "mpi_vb.h"
#include "mpi_sys.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
//#include "mpi_vpp.h"
#include "mpi_venc.h"

 enum AI_PARA_E{
	AI_MODE_8K = 0,
	AI_MODE_16K,
	AI_MODE_32k
};

/*****************************************************************************
��������:��Ƶ��������
�������:@AiMode: AI ͨ������ģʽ
			   @AiDevId: AI �豸��
			   @ChlNum: ����ͨ����
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int AudioInputConfig(AI_PARA_E AiMode, AUDIO_DEV AiDevId, int ChlNum);

/*****************************************************************************
��������:��ֹ��Ƶ����
�������:@AiDevId: AI �豸��
			   @ChlNum: ����ͨ����
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int DisableAudioInput(AUDIO_DEV AiDevId, int ChlNum);

/*****************************************************************************
��������:��Ƶ�������
�������:@AiMode: AO ͨ������ģʽ
			   @AoDevId: AO �豸��
			   @AoChn: AO ͨ����
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int AudioOutputConfig(AI_PARA_E AiMode, AUDIO_DEV AoDevId, AO_CHN AoChn, int BitMode);

/*****************************************************************************
��������:��ֹ��Ƶ���
�������:@AoDevId: AO �豸��
			   @AoChn: AO ͨ����
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
int DisableAudioOutput(AUDIO_DEV AoDevId, AO_CHN AoChn);


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
int Ai2AoHandler(int Aidev, int Aichn, int Aodev, int Aochn);

#endif
