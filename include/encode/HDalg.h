#ifndef _HDALG_H_
#define _HDALG_H_
/************************************************************************/
/*                            BG method                                 */
/************************************************************************/
typedef unsigned char uchar;
typedef void* HumanDetHandle;

//UpdateAndHD����ֵ���壨����ֵ��bool��Ϊint��
#define NOT_ENOUGH_CHANGE 0 // ��������ޱ仯������Ҫ¼��
#define ENOUGH_CHANGE     1 // �����б仯����Ҫ¼�񣬵�����Ҫ�ϱ��ƶ����
#define MOTION_DETECTED   3 // ��Ҫ¼������Ҫ�ϱ��ƶ����

// ���²����ò��ϣ�Ϊ��������ʱ������
// �뷵��ֵ��λ���룬ret&MOTION_DETECTED_AREA1 �������ʾ����һ�����ƶ����Դ�����
//#define MOTION_DETECTED_AREA1   2 // ��Ҫ¼������Ҫ�ϱ��ƶ����
//#define MOTION_DETECTED_AREA2   4 // ��Ҫ¼������Ҫ�ϱ��ƶ����
//#define MOTION_DETECTED_AREA3   8 // ��Ҫ¼������Ҫ�ϱ��ƶ����

HumanDetHandle CreateBackModel();

/**
*  @brief SetROI            �����������ģ��
*
*  @param x,y,w,h           �������������Ϳ��
                            �豸�յ�������Ϊfloat���ͣ�ת������(����Ƶͼ����Ϊwidth,height)��
                            x = x_float * width;
                            y = x_float * height;
                            w = w_float * width;
                            h = h_float * height;
*  @param type              0- ɾ��֮ǰ��ROI������µ�ROI
                            1- ����֮ǰ��ROI������µ�ROI
                            ��������Ҫ��1���ᵼ�´˴�������Ч��ROI���䣻
                              ������ʱ����������������Ҳ�ᵼ�´˴�������Ч��
                            2- ɾ�����е�ROI�����ȫͼ
*  @param phdHandle         ʵ��
*
*  @note  
*/
bool SetROI(HumanDetHandle hdHandle, const int x, const int y, const int w, const int h, const int type);

/**
*  @brief UpdateAndHD       �ƶ���⴦��ģ��
*
*  @param ptr_gray          �Ҷ�ͼ��
*  @param width,height,step �Ҷ�ͼ�Ŀ��ߺ�step
*  @param sensitive         �����Ȳ�����ȡֵ0��1��2��3�����������ν���
*  @param phdHandle         ʵ��
*
*  @note  
*/
int UpdateAndHD(HumanDetHandle hdHandle, const uchar* ptr_gray, const int width, const int height, const int step, unsigned int sensitive);
bool ReleaseBackModel( HumanDetHandle* phdHandle );

#endif	//#define _HDALG_H_
