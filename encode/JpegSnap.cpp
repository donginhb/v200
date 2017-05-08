
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

#include "CommonDefine.h"
#include "His_Api_define.h"
#include "JpegSnap.h"
#include "ModuleFuncInterface.h"

HI_BOOL bSaveJpg= HI_TRUE; 
HI_BOOL bSaveThm= HI_FALSE;


static const HI_U8 g_SOI[2] = {0xFF, 0xD8};
static const HI_U8 g_EOI[2] = {0xFF, 0xD9};

JpegSnap_S *JpegSnap_S::m_pInstanse = NULL;

JpegSnap_S::JpegSnap_S()
{

	m_VencGrp =0;
	m_VencChn = 2;
	pthread_mutex_init(&m_SnapProcessLock, NULL);
	
}

JpegSnap_S::~JpegSnap_S()
{
	
}


JpegSnap_S *JpegSnap_S::Instanse()
{
	if(NULL == m_pInstanse)
	{
		m_pInstanse = new JpegSnap_S;
	}

	return m_pInstanse;
}

/*****************************************************************************
��������:����ץ��ͨ��
�������:enNorm--��ʽ
			  enPicSize--ץ��ͼƬ��С

�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
HI_S32 JpegSnap_S::CreateSnapChn(VIDEO_NORM_E enNorm, PIC_SIZE_E enPicSize)
{
	HI_S32 s32Ret;
	SIZE_S stSize;
    VENC_GRP VencGrp =0;// m_VencGrp;
    VENC_CHN VencChn =2;// m_VencChn;

	VENC_CHN_ATTR_S stVencChnAttr;
	VENC_ATTR_JPEG_S stJpegAttr;


	Comm_GetPicSize(enNorm, enPicSize, &stSize);

	/******************************************
	 3: ��������ͨ������������
	******************************************/
	stVencChnAttr.stVeAttr.enType = PT_JPEG;
	//stSize.u32Width=1280;
	//stSize.u32Height=960;
	stJpegAttr.u32MaxPicWidth  = stSize.u32Width;
	stJpegAttr.u32MaxPicHeight = stSize.u32Height;
	stJpegAttr.u32PicWidth	= stSize.u32Width;
	stJpegAttr.u32PicHeight = stSize.u32Height;
	stJpegAttr.u32BufSize = (((stSize.u32Width+15)>>4)<<4) * (((stSize.u32Height+15)>>4)<<4);//stSize.u32Width * stSize.u32Height * 2;
	stJpegAttr.bByFrame = HI_TRUE;/*get stream mode is field mode  or frame mode*/
	stJpegAttr.bSupportDCF = HI_FALSE;
//	stJpegAttr.bVIField = HI_FALSE;/*the sign of the VI picture is field or frame?*/
//	stJpegAttr.u32Priority = 0;/*channels precedence level*/
	memcpy(&stVencChnAttr.stVeAttr.stAttrJpeg, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
	
	s32Ret = HI_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
	if (HI_SUCCESS != s32Ret)
	{
		printf("115=================HI_MPI_VENC_CreateChn [%d] faild with %#x!\n",\
				VencChn, s32Ret);
		return s32Ret;
	}
    s32Ret = VideoVencBindVpss(VencChn, VencGrp, VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        printf("881Start Venc failed! 0x%0x\n",s32Ret);
       
    }

	printf("================ %s:%d success \n",__FUNCTION__,__LINE__);


	return HI_SUCCESS;
}

HI_S32 JpegSnap_S:: DestroySnapCh()
{
	HI_S32 s32Ret;
    VENC_GRP VencGrp = m_VencGrp;
    VENC_CHN VencChn = m_VencChn;
	pthread_mutex_lock(&m_SnapProcessLock);
	s32Ret = VideoVIBindUnVenc(VencGrp);
	if (HI_SUCCESS != s32Ret)
	{
	   	printf("VideoVIBindUnVenc failed!\n");      
		//return s32Ret;
	}	

	s32Ret = HI_MPI_VENC_DestroyChn(VencChn);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_VENC_DestroyChn [%d] faild with %#x!\n",\
				VencChn, s32Ret);
		//return s32Ret;
	}
	#if 0
	s32Ret = HI_MPI_VENC_DestroyGroup(VencGrp);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_VENC_DestroyGroup[%d] failed with %#x!\n",\
				 VencGrp, s32Ret);
		//return HI_FAILURE;
	}
	#endif
	pthread_mutex_unlock(&m_SnapProcessLock);
	return HI_SUCCESS;

}

/*****************************************************************************
��������:ץ�Ĵ���
�������:Para---����ͼƬ���ļ����
			

�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/

HI_S32 JpegSnap_S:: JpegSnapProcess(void *Para)

{
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_S32 s32VencFd;
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    HI_S32 SnapFlag = -1;	
  


	VENC_RECV_PIC_PARAM_S stRecvParam;
  	stRecvParam.s32RecvPicNum = 1;
    s32Ret = HI_MPI_VENC_StartRecvPicEx(m_VencChn,&stRecvParam);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_VENC_StartRecvPic faild with%#x!\n", s32Ret);
        return HI_FAILURE;
    }

    /******************************************
     3: ��ȡfd
    ******************************************/
    s32VencFd = HI_MPI_VENC_GetFd(m_VencChn);
    if (s32VencFd < 0)
    {
    	 printf("HI_MPI_VENC_GetFd faild with%#x!\n", s32VencFd);
        return HI_FAILURE;
    }

    FD_ZERO(&read_fds);
    FD_SET(s32VencFd, &read_fds);
    
    TimeoutVal.tv_sec  = 2;
    TimeoutVal.tv_usec = 0;
    s32Ret = select(s32VencFd+1, &read_fds, NULL, NULL, &TimeoutVal);
    if (s32Ret < 0) 
    {
        printf("snap select failed!:%d\n",s32VencFd);
        return HI_FAILURE;
    }
    else if (0 == s32Ret) 
    {
        printf("snap time out!  %d\n",s32VencFd);
       return HI_FAILURE;
    }
    else
    {
        if (FD_ISSET(s32VencFd, &read_fds))
	    {
			/******************************************
			 4: ��ѯ����״̬
			******************************************/

            s32Ret = HI_MPI_VENC_Query(m_VencChn, &stStat);
            if (s32Ret != HI_SUCCESS)
            {
                printf("HI_MPI_VENC_Query failed with %#x!\n", s32Ret);
            	return HI_FAILURE;
            }
            if(0 == stStat.u32CurPacks)
			{
				  printf("NOTE: Current  frame is NULL!\n");
				  return HI_SUCCESS;
			}
            stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * (stStat.u32CurPacks));
            if (NULL == stStream.pstPack)
            {
                printf("malloc memory failed!\n");
            	return HI_FAILURE;
            }
			/******************************************
			 5: ��ȡ����
			******************************************/

            stStream.u32PackCount = stStat.u32CurPacks;
               s32Ret = HI_MPI_VENC_GetStream(m_VencChn, &stStream, -1);// s32Ret = HI_MPI_VENC_GetStream(m_VencChn, &stStream, HI_TRUE);
            if (HI_SUCCESS != s32Ret)
            {
                printf("HI_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
                free(stStream.pstPack);
                stStream.pstPack = NULL;
         //       goto EndSnap;
            }
          	/*����ͼƬ*/
			SaveJpegStream((FILE*)Para,&stStream);
			printf("=======================================>JpegSnapProcess:%d\n",stStream.u32PackCount);
			/******************************************
			6: �ͷ���Դ
			******************************************/

            s32Ret = HI_MPI_VENC_ReleaseStream(m_VencChn, &stStream);
			
            if (HI_SUCCESS != s32Ret)
            {
                printf("HI_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);
                free(stStream.pstPack);
                stStream.pstPack = NULL;
                return HI_FAILURE;// goto EndSnap;
            }

            free(stStream.pstPack);
            stStream.pstPack = NULL;
		}
    }
	SnapFlag = 0;	/*ץ�ĳɹ�*/



	/******************************************
	 7: ֹͣ�����վ�
	******************************************/

    s32Ret = HI_MPI_VENC_StopRecvPic(m_VencChn);
    if (s32Ret != HI_SUCCESS)
    {
        printf("HI_MPI_VENC_StopRecvPic failed with %#x!\n",  s32Ret);
	
        return HI_FAILURE;
    }

    return SnapFlag;
}





/*****************************************************************************
��������:�洢jpeg ͼƬ
�������:fpJpegFile--�ļ�ָ��
			  pstStream--jpeg ������
�������:��
��  ��   ֵ:�ɹ�����0�����򷵻�-1
ʹ��˵��:
******************************************************************************/
HI_S32 JpegSnap_S::SaveJpegStream(FILE* fpJpegFile, VENC_STREAM_S *pstStream)
{
	VENC_PACK_S*  pstData;
	HI_U32 i;

	for (i = 0; i < pstStream->u32PackCount; i++)
	{
		pstData = &pstStream->pstPack[i];
		fwrite(pstData->pu8Addr+pstData->u32Offset, pstData->u32Len-pstData->u32Offset, 1, fpJpegFile);
		fflush(fpJpegFile);
	}

	return HI_SUCCESS;
}

