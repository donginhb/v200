
#ifndef __VIDEO_ISP_H__
#define __VIDEO_ISP_H__

#include "common.h"
#include "hi_type.h"
#include "mpi_isp.h"
#include "hi_comm_isp.h"
#include "Video_comm.h"

class CVideoISP{
	public:
		
		CVideoISP();
		~CVideoISP();		
		/*****************************************************************************
		��������:ISP_Run
		��������:
		�������:��
		�������:��
		��	��	 ֵ: 0: �ɹ�-1: ʧ��
		ʹ��˵��:�ⲿ����
		
		******************************************************************************/

		int ISP_Run();
		int StopIsp();
		int SetVideoSaturation(unsigned int value);
		#if 0
		int SetVideoBrightness(unsigned int value);
		#endif
		int SetVideoChnAnaLog(int brightness,int contrast,int saturation);
		int H42_ISPseting();
		int SetAntiFlickerAttr(unsigned char enable,unsigned short Frequency);
	private:

		pthread_t m_IspPid;

};



#endif 

