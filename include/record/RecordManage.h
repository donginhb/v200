
#ifndef __RECORD_MANAGE_H_

#define __RECORD_MANAGE_H_

#include <time.h>
#include <pthread.h>

#include "common.h"
#include "systemparameterdefine.h"
#include "BufferManage.h"

#define DISPLAY_MAX_NUM (10)   //ÿ�������ļ��ĸ���

typedef struct TagFindFileType		//��ѯ����ʹ�õĽṹ��
{
	struct tm time;					//��ѯʱ��
	int		m_Res1;					 //δ�ã��豸����0-HDD 1-MHDD 2-SD��
	int 	channel;                                    //ͨ��1~16
	int	RecordType;                             //¼������: 0-ȫ�� 1-���� 2-�ƶ���� 3-��ʱ 5-�ֶ�
	int	m_Res2;                             //δ�ã���ѯ��ʽ 0 ��¼�����Ͳ�ѯ��1 ˾������ѯ 2   ���ƺŲ�ѯ
	char	 m_Res3[17];				//δ��
	char	 m_Res4[17];				//δ��
}FindFileType;

typedef struct tag_rec_diren	   //һ��¼���¼ʹ�õĽṹ��
{
	unsigned short	level; 		    //¼�񼶱�0-�޸澯��1����ȸ澯��2-���ظ澯
	unsigned short	m_res2;		    //δ��
	unsigned int 		start_time;       //�ļ���ʼʱ��
	unsigned int 		end_time;	    // �ļ�����ʱ��
	unsigned int         	filesize;            //�ļ���С��KΪ��λ��ע��:������ʷԭ���ֽڳ���1000�������ǳ���1024
	unsigned int	     	channel; 	    // ͨ����
	int				m_filetype;      //�ļ����ͣ��ο�RECORD_KIND
	int				m_res3;  //�澯���ͣ�δ��
	char				d_name[96];     //�ļ���(��������·��)
}rec_dirent;

typedef struct								//��ѯ���ʹ�õĽṹ�� 
{
	int	fileNb;								//�ļ�����
	rec_dirent    namelist[DISPLAY_MAX_NUM];	//��ѯ���
}RecordFileName;

typedef enum
{
	REC_SEARCH_SET,  		//����㿪ʼ���� 
	REC_SEARCH_NEXT 	      //�ӵ�ǰ����������
}REC_SEARCH;

typedef struct 
{
	unsigned int		m_magic; // ħ��
	unsigned int		m_size;  // �ṹ���С
}VIDEO_STHDR;
#define RECFILEHEADERMAGIC			0x12345678
#define IDX_MAGIC					0x08070620
#define FILELIST_HEADER_SIGNATURE	0x12345678
#define FILELIST_ITEM_SIGNATURE	0x20080610

typedef struct 
{
	unsigned char 		m_bitswidth;  // ��Ƶ������λ��
	unsigned char 		m_reserved1[3];
	unsigned int			m_bitRate;// ����
	unsigned int			m_sampleRate;// ������
}ENCAUDIO_PARA;

typedef struct
{
	unsigned char				chlIndex;   // ͨ����
	unsigned char				frameRate; // ֡��
	unsigned char				resolution; // ������
	unsigned char				reserved1;
	char						chlName[16]; // ͨ������
}ENC_CH_PARA;

typedef struct 						//264�ļ�ͷ��һ��512�ֽڣ�����400�ֽڱ���
{
	VIDEO_STHDR			m_Hdr;
	char						m_VerDev[28];  //�豸����
	char						m_VerFile;	// �ļ��汾
	unsigned char				m_BegYear; 
	unsigned char				m_BegMonth;
	unsigned char				m_BegDay;
	unsigned char				m_BegHour;
	unsigned char				m_BegMinute;
	unsigned char				m_BegSecond;  // ��ʼ��������ʱ����
	unsigned char				m_EndYear;
	unsigned char				m_EndMonth;
	unsigned char				m_EndDay;
	unsigned char				m_EndHour;
	unsigned char				m_EndMinute;
	unsigned char				m_EndSecond;// ������������ʱ����
	unsigned char				m_ChlCount;  // tͨ����Ŀ
	unsigned char				m_RecType;	// ¼������
	unsigned char				m_FileProtected;//¼���Ƿ񱣻�
	unsigned char				m_RecLevel;//¼��ȼ�
	unsigned char				m_VideoType; // ��Ƶ��ʽ,0��ʾNTSC;1��ʾPAL
	unsigned char				m_bNonStop;// ���ڱ���δ�ã���ǰ�汾��0:����������1:��������������ϵ�;
	unsigned char				m_bEncrypt; // ��Ƶ�����Ƿ���ܣ�0:û�м��ܣ�1:�м���
	ENCAUDIO_PARA			m_Audio;  // ��Ƶ�������
	unsigned int				m_IndexTblOffset;// �����������ļ�ƫ�ƣ�ע�⣬Ϊ�˼��߼�������idx�ļ�����׷��������ĩβ����ֵһֱΪ0
	unsigned int				m_MovieOffset; //��ʵ��¼������λ�����Ϊ0XFFFF Ϊ��Чƫ��(����ֵû�м�0x200, ��ʵIframe��λ����m_MovieOffset+0x200)
	unsigned short			m_nDevType; //�豸�������ֱ��
	char						m_Reserved3;
	char						m_DateShowFmt;// ������ʾ�ĸ�ʽ��ʱ����
	char						m_cDevType[12];// �豸�����ַ�������
	ENC_CH_PARA			m_ChInfo; //ͨ������Ƶ�������

	char			recv[400];
}VideoFileInfo;

typedef struct	 //Idx�ļ�ͷ�ṹ��
{
	unsigned int	m_magic;	// ħ��0x08070620
	unsigned 	int	m_header; // �̶�Ϊidx1
	unsigned int	m_block_size; 	// I֡�����������¼��������Ĵ�С��������IDX_HEADER�Ĵ�С��
}IDX_HEADER;
#define		IDX_MAGIC							0x08070620
#define		IDX_FLAG							0x31584449

typedef struct 	//idx�ļ���һ����¼
{
	unsigned long m_iframetime; //*ʱ�������Ϊ��λ3600*hour+60*minute+second 
	unsigned long m_offset; 
	unsigned int m_sampleRate;// ������ 
	unsigned int m_bitswidth; // ��Ƶ������λ�� 
	unsigned char m_RecType; // ¼������,�ο�RECORD_KIND
	unsigned char m_VideoType; // ��Ƶ��ʽ ,�ο�MACHINE_PARA��1 NTSC, 0  PAL
	unsigned char m_FrameRate; // ֡�� 
	unsigned char m_Resolution; // ������ ,�ο�Resolution_E
	unsigned char m_RecLevel; // ¼��ȼ� ,�ο�RECORD_LEVEL
	unsigned char m_res[3]; //�����ֶ� 	
}IFRAMELISTCONTENT;

typedef struct tag_LISTFILE_FILEHEADER //�б��ļ��ļ�ͷ������
{
	unsigned long 			m_signature; 		//�˽ṹ��ı�ʶ
	char					m_verDev[28]; 		// δ��
	char					m_verFile;    		//¼���ļ��İ汾����ͬ�ͺŵ��豸�������Լ��İ汾��Ϣ��ĿǰΪ8
	char					m_begYear;
	char					m_begMonth;
	char					m_begDay;
	char					m_begHour;
	char					m_begMinute;
	char					m_begSecond;
	char					m_endYear;
	char					m_endMonth;
	char					m_endDay;
	char					m_endHour;
	char					m_endMinute;
	char					m_endSecond;
	char 				m_pad1;			//����
	unsigned short 		m_total_files; 		//�ļ��б��е��ܼ�¼����,������ɾ���ļ�¼
	unsigned short 		m_cur_file;   		//δ��
	unsigned short		m_usdevType;		//δ��
	char					m_szdevType[12];	//δ��
	unsigned short		m_alarmfilecnt; 		//����¼�����
	unsigned short		m_mdfilecnt; 		//�ƶ����¼�����
	unsigned short		m_timerfilecnt; 		//��ʱ¼�����
	unsigned short		m_maunalfilecnt; 	//�ֶ�¼�����
	unsigned char			m_downloaded;  		//δ��
	char 				m_reserved[27];
}LISTFILE_FILEHEADER;

#if 0
typedef struct tag_LISTFILE_NAME	//�б��У�ÿ���ļ���¼�ṹ�嶨��
{
	unsigned long 		m_signature; 		//ÿ����¼�ı�־ͷ��
	char 			m_name[96];		//�ļ�������(����·��)
	unsigned short 	m_state; 			//�ļ�ɾ����־ =0x55aa:�ļ���ɾ��,!=0x55aa:�ļ�����,��ʼֵΪ0
	unsigned short 	M_filestat;     		// �ļ�0: creat , 1: close 
	int 			  	m_start_hms;		//���ļ����ɵ�ʱ��.=h*3600+m*60+s
	unsigned char 	m_record_type; 		//0:�ֶ�¼��	1:��ʱ¼��2:����¼��3:�ƶ�֡��¼��
	unsigned char		m_RecLevel;			//¼��ȼ�
	char 			m_channel;			//ͨ����1~16
	char 			m_pad2[1];			//δ��
	int				m_end_time;  		//���ļ�������ʱ��.=h*3600+m*60+s
	char 			m_drivername[17];	//δ��
	char				m_vehiclenum[17];	//δ��
	unsigned char		m_download; 		//�Ƿ������ļ�0x55:��������:��������δ��
	char				m_pad3[20];			//δ��
}LISTFILE_NAME;
#else
typedef struct				//�����¼�
{
	unsigned int m_RecCh; // ¼��ͨ��,ע��:��1��ʼ���
	unsigned int m_RecType; // ¼������ ,�ο�RECORD_KIND
	unsigned int m_VideoType; // ��Ƶ��ʽ��0��ʾpal��1��ʾntsc
	unsigned int	m_FrameRate; // ֡��
	unsigned int	m_Resolution; // �����ȣ��ο�Resolution_E
	unsigned int  m_sampleRate;// ������,8000
	unsigned int m_bitswidth; // ��Ƶ������λ��,16
	unsigned char m_RecLevel;//�ο�RECORD_LEVEL
	unsigned char m_res[7]; //�����ֶ�
	unsigned int 	start_time; /*�¼���ʼʱ��*/
	unsigned int 	end_time;	/*�¼�����ʱ��*/
	int m_StartOffset;//¼��Ƭ�Σ����ļ��еĿ�ʼƫ��
	int m_EndOffset; //¼��Ƭ�Σ����ļ��еĽ���ƫ��
} EVEN_ITEM;

typedef struct tag_LISTFILE_NAME
{
	unsigned long 	m_signature; 	/*ÿ����¼�ı�־ͷ��*/
	char 			m_name[96];		/*�ļ�������(����·��)*/
	unsigned short 	m_state; 		/*�ļ�ɾ����־ =0x55aa:�ļ���ɾ��,!=0x55aa:�ļ�����,��ʼֵΪ0*/
	char 			m_pad2[2];	
	EVEN_ITEM 	stEvenItem;        
}LISTFILE_NAME;
#endif

typedef enum
{
	ITEM_STATE_OK = 0x0000,
	ITEM_STATE_DELETED = 0x55aa
}ITEM_STATE;
typedef enum
{
	ITEM_FILE_STAT_CREAT = 0x0,
	ITEM_FILE_STAT_CLOSE = 0x1
}ITEM_FILE_STAT;

typedef enum				//�ڲ�ʹ�õ�ö����
{
	RECORD_STOP,
	RECORD_PALSE,
	RECORD_CONTINUE,
	RECORD_START
}RECORDCTLCMD;
typedef enum				//�ڲ�ʹ�õ�ö����
{
	RECORD_STOPED,
	RECORD_PALSEED,		
	RECORD_ING	
}RECORDCTLSTATUS;
typedef enum				//¼������ö����
{
	TIMER_RECORD = 1,
	ALARM_RECORD = 2,
	MD_RECORD = 3,
	MW_RECORD = 4,//MICROWALE
	SD433_RECORD = 5,
	AUDIO_RECORD = 6
}RECORD_KIND;
typedef enum				//¼�񼶱�ö����
{
	RECORD_LEVEL1 = 0,//��ʵʱ
	RECORD_LEVEL2 = 1,//¼��δ�õ�����ȼ��������������ȼ���Ϊ��ʵʱ¼��
	RECORD_LEVEL3 = 2//ʵʱ
}RECORD_LEVEL;

/*�ڲ�ʹ�õĺ궨��*/
#define MIN_SYS_ALARM_DELAY			10/*�˺���Ϊ�˱�֤���ֱ���¼����С��ʱһ��*/
#define RECMD_DELAY				       MIN_SYS_ALARM_DELAY
#define RECALARM_DELAY			              MIN_SYS_ALARM_DELAY
#define MAX_RECFILENAMELEN			128
#define START_DEL_FILE					(300)//���̿��ÿռ�С��START_DEL_SPACE����ʼɾ������λMB
#define STOP_DEL_FILE					(600)//���̿��ÿռ����STOP_DEL_FILE��ֹͣɾ������λMB
#define RECORD_LIST_FILENAME			"recfilelist"//�б��ļ��ļ���
#define MAX_EVEN_NUM 					(3600/MIN_SYS_ALARM_DELAY)
//ע��1:
//¼��ģ�飬���������Ƿ���
//�ⲿ��⣬������������Ҳ����ǣ�����ýӿڣ�ֹͣ¼��
//����ɸ��ǣ���������������ýӿڣ�ɾ��¼��

//ע��2:
//�ɺ궨������ⲿ�澯¼��ʱ�����ƶ�¼��ʱ��

//ע��3:
//����ƶ���⿪�أ��ӿ���תΪ�رգ��ⲿӦ�õ���SetMdAlarmLink���ر�¼��
//����ƶ���⿪�أ��ӹر�תΪ��������ô���ڷ����ƶ�ʱ������SetMdAlarmLink����֪¼��ģ�飬���Խ����ƶ����¼��
class RecordManage{
private:
	/*��һ����:�����߼�*/
	static RecordManage * m_pInstance;
	pthread_t ctrlThreadId;	
	pthread_t dataThreadId;		
	int s32CtrlThreadQuit;
	int s32DataThreadQuit;
	SystemDateTime  m_datetime;//��¼ϵͳ��ǰʱ��ı���
	SystemDateTime  m_preTime; //��һ�μ��ʱ������ʱ��ʱ��
	GROUPRECORDTASK m_RecSchedule;//��ʱ¼�񡢸澯¼���ʱ���

	/*�����߳̽���ʹ�õı���*/
	RECORDCTLCMD			m_uRecordCmd[MAX_VIDEO_CHANNEL]; 	//д¼������
	RECORDCTLSTATUS      	m_uRecordStatus[MAX_VIDEO_CHANNEL]; //д¼��״̬

	/*ͨ����صı���*/	
	int 						m_uTimerValid[MAX_VIDEO_CHANNEL];//��ʱ¼��ʱ����Ƿ���Ч
	int 						m_uEventValid[MAX_VIDEO_CHANNEL];	//�ⲿ�澯¼��ʱ����Ƿ���Ч
	int 						m_uMDvalid[MAX_VIDEO_CHANNEL];	//�ƶ�¼��ʱ����Ƿ���Ч
	int 						m_uMwValid[MAX_VIDEO_CHANNEL];//΢���澯¼��ʱ����Ƿ���Ч
	int 						m_uSD433valid[MAX_VIDEO_CHANNEL];	//�Ŵ�¼��ʱ����Ƿ���Ч
	int 						m_uAudioValid[MAX_VIDEO_CHANNEL];//�����澯¼��ʱ����Ƿ���Ч
	
	RECORD_KIND  			m_uRecordType[MAX_VIDEO_CHANNEL];//��ǰ���ڽ��е�¼������
	RECORD_LEVEL  			m_uRecordLevel;//¼��ĵȼ�
	RECORD_LEVEL  			m_uRecordLevelAlarm;//�ⲿ������¼��ȼ�
	RECORD_LEVEL  			m_uRecordLevelMd;//�ƶ�������¼��ȼ�
	RECORD_LEVEL  			m_uRecordLevelMw;//΢��������¼��ȼ�
	RECORD_LEVEL  			m_uRecordLevelSD433;//�Ŵű�����¼��ȼ�
	RECORD_LEVEL  			m_uRecordLevelAudio;//����������¼��ȼ�	
	RECORD_LEVEL  			m_uRecordLevelTimer;//��ʱ������¼��ȼ�	
	
	int						m_RecStartTime[MAX_VIDEO_CHANNEL];//¼��ʼʱ��
	int						m_RecEndTime[MAX_VIDEO_CHANNEL];//¼�����ʱ��	
	SystemDateTime  			m_stEndTime[MAX_VIDEO_CHANNEL];//¼���ļ��Ľ���ʱ��	
	
	int 						m_uAlarmRecDelay[MAX_VIDEO_CHANNEL];//�ⲿ�澯¼����ʱ
	int 						m_uMDRecDelay[MAX_VIDEO_CHANNEL];//�ƶ�¼����ʱ
	int 						m_uMwAlarmRecDelay[MAX_VIDEO_CHANNEL];//�ⲿ�澯¼����ʱ
	int 						m_uSD433AlarmRecDelay[MAX_VIDEO_CHANNEL];//�ƶ�¼����ʱ
	int 						m_uAudioAlarmRecDelay[MAX_VIDEO_CHANNEL];//�ⲿ�澯¼����ʱ
	
	unsigned int				m_u32ExAlarmLink;//�ⲿ�澯¼��1��ͨ����1������	
	unsigned int				m_u32MdAlarmLink;//�ƶ����¼��1��ͨ����1������	
	unsigned int				m_u32MwAlarmLink;//΢�����¼��1��ͨ����1������	
	unsigned int				m_u32SD433AlarmLink;//�Ŵ�¼��1��ͨ����1������	
	unsigned int				m_u32AudioAlarmLink;//�ⲿ�澯¼��1��ͨ����1������
	
	int						m_AudioStartTime[MAX_VIDEO_CHANNEL];//������ʼʱ��
	int						m_MDStartTime[MAX_VIDEO_CHANNEL];//�ƶ���ʼʱ��	
	
	char 					m_264FileName[MAX_VIDEO_CHANNEL][MAX_RECFILENAMELEN];//����·��
	char 					m_IdxFileName[MAX_VIDEO_CHANNEL][MAX_RECFILENAMELEN];//����·��	
	VideoFileInfo				m_FileInfo[MAX_VIDEO_CHANNEL];
	FILE						*m_pRecFp[MAX_VIDEO_CHANNEL];
	int 						m_index_fds[MAX_VIDEO_CHANNEL];
	unsigned long 				m_offset[MAX_VIDEO_CHANNEL];								//I֡��ַƫ��

	
	/*������صı���*/
	int 						m_uHddEio[MAX_VIDEO_CHANNEL];	//д¼������У��Ƿ�����EIO����
	int 						m_nCurrentHddNum;			//��ǰ����¼��Ĵ���
	int                                       m_s32HddFull;				//������
	char 					m_strRecDir[MAX_RECFILENAMELEN];				//��ǰ¼��Ŀ¼
	unsigned char				m_uHddOverWrite;			//���������Ǳ��
		
	//�ļ�������صı���
	int 						m_s32IdxOffset;


	void InitRecordManage();
	//�����̵߳���Ҫ����
	void CheckRecordTask();	

	//������
	void CheckHdd();
	void CheckTimeValid();
	void  CheckRecordStart();
	void CheckTimeJump();
	void  CheckRecordEnd();	
	void CheckAlarmDelay();

	//���ƺ���
	int StopOneChannelRecord(int ch);	
	int StartOneChannelRecord(int ch,RECORD_KIND enRecordKind);	
	int PauseOneChannelRecord(int ch);	
	int ContinueOneChannelRecord(int ch,RECORD_KIND enRecordKind);	
	void RestartOneChannelRecord(int ch,RECORD_KIND enRecordKind);	
	void ChangeOneChannelRecord(int ch,RECORD_KIND enRecordKind);	

	//��������
	int IsLowRecordType(int ch,RECORD_KIND enRecordKind);	
	int CheckAlarmRecordValid(int ch);
	int CheckMdRecordValid(int ch);	
	int CheckMwRecordValid(int ch);
	int CheckSd433RecordValid(int ch);
	int CheckAudioRecordValid(int ch);	
	int GetCurrentTimeValue();
	void RecordTime2SysTime(int s32RecordTime,SystemDateTime *pstSystemDateTime);	
	int GetRecordHour(SystemDateTime  *pstSystemDateTime);		
	int CheckTimerTable(unsigned char ch, RECORD_KIND enRecordKind);	
	int IsTimeJump();	
	int getYearMonthDay(int s32Year,int s32Month,int s32Day);
	int getOldRecordDir(char *pstrOldRecDir);
	int DelDir(char *pstrPathName);
	int DelOldRecord();	
	void DeleteHistoryVersionRecord();
	void fixNotCloseFile();
	int GetEndTimeFrom264(char *pstr264Name);
	int GetRecordOffsetFrom264(const char* pstr264Name, unsigned int start_time, unsigned int *pOffset);
	void UpdateListFile(char *pstrIdxName,int ch);
	void SetRecInfo(int ch,RECORD_KIND enRecordKind);
	
	/*�ڶ�����:д����*/
	//д����������
	int OpenRecordFile(int ch);
	int WriteRecordFile(int ch);
	int CloseRecordFile(int ch);
	
	//��������
	int TestAndCreateDir(char *dirname);
	int CreateRecordDirectory();
	int Create264File(int ch);
	int CreateIdxFile(int ch);	
	int CreateListFile(char *pstrListFile);
	int AddRecordFile2List(int ch);
	void PrepareRecordInfor(int ch);
	void ClearRecordInfor(int ch);
	int ModifyItemInList(int ch,SystemDateTime *pstEnd,char *pNewName);
	int Rewrite264Header(int ch);
	int AppendIdx(int ch);
	int Rename264AndFixItem(int ch,SystemDateTime *pstEnd);
	int FillEvenItem(int ch,EVEN_ITEM *pstEvenItem,IFRAMELISTCONTENT *pstIframeListContent,int start_time,int end_time,int s32StartOffset,int s32EndOffset,RECORD_LEVEL enRecordLevel);
	int FixEvenItenStartTime(EVEN_ITEM *pstEvenItem);
	int FixEvenItenEndTime(EVEN_ITEM *pstEvenItem);
	int ParserEvenFromIframeIndex(char *pstrIdxFileName, EVEN_ITEM *pstEvenItem, int *ps32EvenNum,int ch);
	int  AddOneEven2List(char *pstrDirName,EVEN_ITEM *pstEvenItem);
	int RederectFilePosition(int ch,int curtime);
	void Fill264Header(int ch,SystemDateTime  *pStart,SystemDateTime  *pEnd,
									unsigned int s32IndexTblOffs,unsigned int s32MovieOffset,int frameRate,int resolution,int bNonStop);	
	void FillListFileHeader(LISTFILE_FILEHEADER *pstListFileHeader,SystemDateTime  *pStart,SystemDateTime  *pEnd,
									unsigned short u16TotalFiles,
									unsigned short u16AlarmFileCnt,unsigned short u16MdFileCnt,
									unsigned short u16TimeFileCnt,unsigned short u16MaunalFileCnt);	
	void FillListFileName(LISTFILE_NAME *pstListFileName,char *pstrFileName,ITEM_STATE enState,
									ITEM_FILE_STAT enFileStat,int s32StartHms,unsigned char u8RecordType,int channel,int s32EndHms);
	void FillRecDirent(rec_dirent *pstRecDirent,RECORD_LEVEL enRecordLevel,int s32StartTime,int s32EndTime,int s32Size,
	                          int channel,RECORD_KIND enRecordKind,char *pstrFileName);	
	
public:
	static RecordManage* GetInstance();
	int StartRecordSystem();	
	int StopRecordSystem();	
	void SetRecSchedule(GROUPRECORDTASK *pRecSchedule);
	void SetMdAlarmLink(unsigned int u32MdAlarmLink,RECORD_LEVEL enRecordLevel);
	void SetExAlarmLink(unsigned int u32ExAlarmLink,RECORD_LEVEL enRecordLevel);
	void SetMwAlarmLink(unsigned int u32MwAlarmLink,RECORD_LEVEL enRecordLevel);
	void SetSD433AlarmLink(unsigned int u32SD433AlarmLink,RECORD_LEVEL enRecordLevel);
	void SetAudioAlarmLink(unsigned int u32AudioAlarmLink,RECORD_LEVEL enRecordLevel);
	
	void SetHddOverWrite(unsigned char u8HddOverWrite);
	int  FindRecordFile(FindFileType *findType,RecordFileName *RecordFile, REC_SEARCH enMode,int s32MaxItenNum);
	int  HaveRecordFile(struct tm time);
	int GetRecordOffset(const char* path, unsigned int start_time, unsigned int *pOffset);
	int  CheckOneEvenFromList(char *pstrDirName,EVEN_ITEM *pstEvenItem);
	
	void RecordDataProcess();	
	void RecordCtrlLoop();	
	RecordManage();
	~RecordManage();
};

#endif 
