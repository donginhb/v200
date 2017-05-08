
#ifndef __VGA_HI3120_H__

#define __VGA_HI3120_H__



#define HI3120_SET_BRIGHT 					0x20	/* ��������*/
#define HI3120_SET_CONTRAST				0x21  /* ���öԱȶ�	*/
#define HI3120_SET_HUE						0x22   /*����ɫ��*/
#define HI3120_SET_SAT						0x23  /*���ñ��Ͷ�*/
#define HI3120_INIT							0x24  /*����3120 ��ʼ��*/
#define HI3120_SETNORM						0x25  /*������ʾģʽ*/
#define HI3120_HV							0x26   /* ������ʾ��ʾ�ߴ����ʾλ�� */


// ������ʾģʽ
#define	MODE_PAL_TO_800x600P60			0    /*PAL ���� ��� 800 X600 	*/
#define	MODE_NTSC_TO_800x600P60			1   /*NTSC �������800X600	*/
#define 	MODE_PAL_TO_1024x768P60			2   /*pal ���� ���1024 X 768	*/
#define 	MODE_NTSC_TO_1024x768P60		3   /*NTSC �������1024X768	*/
 #define	MODE_PAL_TO_1366x768P60			4  /*pal ���� ���1366 X768	*/
#define 	MODE_NTSC_TO_1366x768P60		5  /*PAL �������1366x 768	*/
#define 	MODE_TEST_COLORBAR				6  /* ����ģʽ	*/


typedef struct 
{
	unsigned int 	m_u32X; 	/*������ʾ��x ������� */
	unsigned int 	m_u32Y; /*������ʾ Y�������*/
	unsigned char m_u32Bri; /*��������1- 63*/
	unsigned char m_u32Con;/* ���öԱȶ�1- 63 */
	unsigned char m_u32Hue; /*����ɫ��1-63*/
	unsigned char m_u32Sat; /* ���ñ��Ͷ� 1- 63*/
	unsigned char m_u32Mode; /* ������ʾģʽ 0--5 */
	
}STRCT_HI3120_SET;



	class  VGA_HI3120 
	{
		

		private:

				int m_n3120Fd;
				
				static VGA_HI3120 *m_pInstance;
		public:

				VGA_HI3120();
				~VGA_HI3120();

				static VGA_HI3120* Instance();

				int SetHi3120WorkMode(int cmd, STRCT_HI3120_SET  ModeSet);

	};

#endif 


