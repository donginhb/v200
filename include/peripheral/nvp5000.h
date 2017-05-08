#ifndef __VGA_NVP5000_H__

#define __VGA_NVP5000_H__

#define _USE_27Mhz_

#define	NORMSET		0x10	//��ʽ
#define	RESOLUTION		0x11	//�ֱ���
#define   ANALON			0x12

typedef struct 
{
	int 	m_32X; 	/*������ʾ��x ������� */
	int 	m_32Y; /*������ʾ Y�������*/
	int	m_8Bri; /*��������*/
	int 	m_8Con;/* ���öԱȶ� */
	int 	m_8Hue; /*����ɫ��*/
	int 	m_8Sat; /* ���ñ��Ͷ� */
	char m_8Norm; /* ������ʽPAL(0) NTSC(1)*/
	char	m_8Resoult;/*���÷ֱ���800*600(0) 1024*768(1) 1280*1024(2)*/
	
}NVP5000_SET;

	class  VGA_NVP5000 
	{
		

		private:

				int nvp5000Fd;
				
				static VGA_NVP5000 *m_pInstance;
		public:

				VGA_NVP5000();
				~VGA_NVP5000();

				static VGA_NVP5000* Instance();
				int SetNVP5000WorkMode(int cmd, NVP5000_SET  ModeSet);

	};

#endif 

