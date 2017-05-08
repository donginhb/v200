#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#include "RecordManage.h"
#include "BlockDeviceManage.h"
#include "ModuleFuncInterface.h"

/*
��һ���֣������߼�
*/
void *RecordDataThread(void *arg);
void *RecordCtrlThread(void *arg)
{
	plog("%s %d %s tid:[%d] pid:[%d] ppid:[%d]\n", __FILE__,__LINE__,__FUNCTION__,(int)pthread_self(),(int)getpid(),(int)getppid());
//	pthread_detach(pthread_self());
	RecordManage *record = (RecordManage *)arg;

	record->RecordCtrlLoop();	
	return NULL;
}

RecordManage * RecordManage::m_pInstance = NULL;
RecordManage*RecordManage::GetInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new  RecordManage();
	}

	return m_pInstance;
}

RecordManage::RecordManage()
{
	InitRecordManage();
}

RecordManage::~RecordManage()
{
	
}


void RecordManage::InitRecordManage()
{
	plog("InitRecordManage~~~~\r\n");
	s32CtrlThreadQuit = 0;
	s32DataThreadQuit = 0;
	memset(&m_RecSchedule,0,sizeof(m_RecSchedule));
	memset(&m_datetime,0,sizeof(m_datetime));//��¼ϵͳ��ǰʱ��ı���
	memset(&m_preTime,0,sizeof(m_preTime)); //��һ�μ��ʱ������ʱ��ʱ��	
	
	m_u32MdAlarmLink = 0;
	m_u32ExAlarmLink = 0;
	m_u32MwAlarmLink = 0;
	m_u32SD433AlarmLink = 0;
	m_u32AudioAlarmLink = 0;
	
	m_nCurrentHddNum = 0;
	m_s32HddFull = 0;
	m_uHddOverWrite = 0;

	memset(m_uRecordCmd,0,sizeof(m_uRecordCmd));
	memset(m_uRecordStatus,0,sizeof(m_uRecordStatus));
	memset(m_uTimerValid,0,sizeof(m_uRecordStatus));
	memset(m_uMDvalid,0,sizeof(m_uMDvalid));	
	memset(m_uEventValid,0,sizeof(m_uEventValid));
	memset(m_uMwValid,0,sizeof(m_uMwValid));
	memset(m_uSD433valid,0,sizeof(m_uSD433valid));	
	memset(m_uAudioValid,0,sizeof(m_uAudioValid));	
	memset(m_uRecordType,0,sizeof(m_uRecordType));	
	memset(m_AudioStartTime,0,sizeof(m_AudioStartTime));
	memset(m_MDStartTime,0,sizeof(m_MDStartTime));
	m_uRecordLevel = RECORD_LEVEL1;
	
	m_uRecordLevelAlarm = RECORD_LEVEL1;
	m_uRecordLevelMd = RECORD_LEVEL1;
	m_uRecordLevelMw = RECORD_LEVEL1;
	m_uRecordLevelSD433 = RECORD_LEVEL1;
	m_uRecordLevelAudio = RECORD_LEVEL1;
	m_uRecordLevelTimer = RECORD_LEVEL1;
	
	memset(m_RecStartTime,0,sizeof(m_RecStartTime));	
	memset(m_RecEndTime,0,sizeof(m_RecEndTime));
	memset(m_stEndTime,0,sizeof(m_stEndTime));
	memset(m_uAlarmRecDelay,0,sizeof(m_uAlarmRecDelay));
	memset(m_uMDRecDelay,0,sizeof(m_uMDRecDelay));
	memset(m_uMwAlarmRecDelay,0,sizeof(m_uMwAlarmRecDelay));
	memset(m_uSD433AlarmRecDelay,0,sizeof(m_uSD433AlarmRecDelay));	
	memset(m_uAudioAlarmRecDelay,0,sizeof(m_uAudioAlarmRecDelay));		
	memset(m_264FileName,0,sizeof(m_264FileName));
	memset(m_IdxFileName,0,sizeof(m_IdxFileName));
	memset(m_FileInfo,0,sizeof(m_FileInfo));
	memset(m_pRecFp,0,sizeof(m_pRecFp));
	memset(m_index_fds,0,sizeof(m_index_fds));
	memset(m_offset,0,sizeof(m_offset));
	memset(m_uHddEio,0,sizeof(m_uHddEio));
	memset(m_strRecDir,0,sizeof(m_strRecDir));

	//Ϊ�˵��Է��㣬����m_RecSchedule
	//��ʽ�汾����ֵ���ⲿ����
	for(int i = 0; i < MAX_VIDEO_CHANNEL;i ++)
	{
		m_RecSchedule.m_ChTask[i].m_uTimerSwitch = 1;	
		for(int j = 0; j < 8; j++)
		{
			m_RecSchedule.m_ChTask[i].m_TimerTask[j].m_uTimeTbl[0].m_u16StartTime = 0;
			m_RecSchedule.m_ChTask[i].m_TimerTask[j].m_uTimeTbl[0].m_u16EndTime = 60*23+59;
			m_RecSchedule.m_ChTask[i].m_TimerTask[j].m_uTimeTbl[0].m_u8Valid = 1;
		}		
	}
	m_uHddOverWrite = 1;	
}

int RecordManage::CheckTimerTable(unsigned char ch, RECORD_KIND enRecordKind)
{
	unsigned int CurTime = m_datetime.hour*60+m_datetime.minute;
	unsigned int STime = 0, ETime = 0;
	unsigned char Week = m_datetime.week&0x07;
	RECORDTASK	RecTask = m_RecSchedule.m_ChTask[ch];

	for(int i = 0; i < 4; i++)
	{
		if(enRecordKind == TIMER_RECORD)
		{
			STime = RecTask.m_TimerTask[Week].m_uTimeTbl[i].m_u16StartTime;
			ETime = RecTask.m_TimerTask[Week].m_uTimeTbl[i].m_u16EndTime;
			if((CurTime >= STime) && (CurTime <= ETime) && (STime != ETime)
				&&(RecTask.m_TimerTask[Week].m_uTimeTbl[i].m_u8Valid&0x01))
			{
				return 1;
			}
		}	
		else if(enRecordKind == ALARM_RECORD)
		{
			STime = RecTask.m_TimerTask[Week].m_uAlarmTbl[i].m_u16StartTime;
			ETime = RecTask.m_TimerTask[Week].m_uAlarmTbl[i].m_u16EndTime;
			if((CurTime >= STime) && (CurTime <= ETime) && (STime != ETime) 
				&& (RecTask.m_TimerTask[Week].m_uAlarmTbl[i].m_u8Valid&0x01))
			{
				return 1;
			}
		}
		else if(enRecordKind == MD_RECORD)
		{
			STime = RecTask.m_TimerTask[Week].m_uMdTbl[i].m_u16StartTime;
			ETime = RecTask.m_TimerTask[Week].m_uMdTbl[i].m_u16EndTime;
			if((CurTime >= STime) && (CurTime <= ETime) && (STime != ETime)
				&&(RecTask.m_TimerTask[Week].m_uMdTbl[i].m_u8Valid&0x01))
			{
				return 1;
			}
		}		 
	}

	return 0;
}

int RecordManage::getYearMonthDay(int s32Year,int s32Month,int s32Day)
{
	return (s32Year-2000)*365+s32Month*30+s32Day;
}

int RecordManage::getOldRecordDir(char *pstrOldRecDir)
{
	DIR              	  *pDir ;
	struct dirent    *ent  ;  
	char strDir[MAX_RECFILENAMELEN];
	snprintf(strDir,sizeof(strDir),"/hdd%02d/p01/record",m_nCurrentHddNum);
	pDir=opendir(strDir);  
	int s32Year,s32Month,s32Day;
	int s32OldYMD = getYearMonthDay(2049,12,31);
	int s32YMD;
	
	while((ent=readdir(pDir))!=NULL)  
	{
		if(ent->d_type&DT_DIR)
		{
			if(strstr(ent->d_name,"2") != NULL)
			{
				sscanf(ent->d_name,"%04d-%02d-%02d",&s32Year,&s32Month,&s32Day);
				s32YMD = getYearMonthDay(s32Year,s32Month,s32Day);
				if(s32YMD < s32OldYMD)
				{
					s32OldYMD = s32YMD;
					snprintf(pstrOldRecDir,MAX_RECFILENAMELEN,"/hdd%02d/p01/record/%s",m_nCurrentHddNum,ent->d_name);
				}
			}
		}
	}
	closedir(pDir);

	return strlen(pstrOldRecDir) == 0 ? S_FAILURE:S_SUCCESS;
}

int RecordManage::DelDir(char *pstrPathName) 
{ 
	DIR* dp = NULL; 
	char pathname[128] = {0};
	struct dirent* dirp; 
	int ret;
	
	dp = opendir(pstrPathName); 
	if(dp == NULL) 
	{ 
		plog("opendir err,pstrPathName=%s\n",pstrPathName); 
		return S_FAILURE; 
	} 

	//��һ����ɾ���ļ����µ��ļ������ļ���
	struct stat statbuf;
	while((dirp = readdir(dp)) != NULL) 
	{ 
		if(strcmp(dirp->d_name, "..") == 0 || strcmp(dirp->d_name, ".") == 0) 
			continue; 
		
		snprintf(pathname,sizeof(pathname),"%s/%s",pstrPathName,dirp->d_name);
		if(lstat(pathname,&statbuf) < 0)
		{
			closedir(dp);
			return S_FAILURE;	
		}

		if(S_ISDIR(statbuf.st_mode) == 1)
			ret = DelDir(pathname);	
		else
			ret = unlink(pathname);			
		
		if(ret != 0)
		{
			closedir(dp); 
			return ret;
		}
	} 
	closedir(dp);

	//�ڶ�����ɾ���ļ���
	rmdir(pstrPathName); 
	return S_SUCCESS;
} 

int RecordManage::DelOldRecord()
{
	//����ɵ�Ŀ¼
	char strOldRecDir[MAX_RECFILENAMELEN];	
	memset(strOldRecDir,0,sizeof(strOldRecDir));
	if(getOldRecordDir(strOldRecDir) != S_SUCCESS)
	{
		plog("err,getOldRecordDir\n");
		return S_FAILURE;
	}
	
	//���б��ļ�
	char strListFile[MAX_RECFILENAMELEN];
	snprintf(strListFile,sizeof(strListFile),"%s/%s",strOldRecDir,RECORD_LIST_FILENAME);
	int fd = open(strListFile,O_RDWR, 0777);
	if(fd < 0)
	{
		plog("err,open,%s\n",strListFile);
		return S_FAILURE;
	}

	//�����ļ�ͷ
	LISTFILE_FILEHEADER stListFileHeader;
	int s32Ret = read(fd,&stListFileHeader,sizeof(stListFileHeader));
	if(s32Ret != sizeof(stListFileHeader))
	{
		plog("err,read %s\n",strListFile);
		close(fd);
		return S_FAILURE;
	}	

	//������ɵ��ļ������ɾ��
	//ע��:Ϊ���߼������б��ļ��ļ�¼˳�򣬽���ɾ��	
	LISTFILE_NAME stListFileName;
	int s32DelSpace = 0;
	struct stat fileInfo;
	while(1)
	{
		s32Ret = read(fd,&stListFileName,sizeof(stListFileName));
		if(s32Ret == 0)
		{
			plog("read end,%s\n",strListFile);
			close(fd);
			//ɾ���ļ���
			DelDir(strOldRecDir);
			return S_SUCCESS;			
		}
		
		if(s32Ret != sizeof(stListFileName))
		{
			close(fd);
			plog("err,read %s\n",strListFile);
			//ɾ���ļ���
			DelDir(strOldRecDir);			
			return S_FAILURE;
		}	

		//�����Ѿ���ɾ�����ļ�
		if(stListFileName.m_state == ITEM_STATE_DELETED)//�Ѿ���ɾ��
			continue;			

		//��������¼����ļ�
		int bRecordingFile = 0;
		for(int i = 0; i < MAX_VIDEO_CHANNEL; i++)
		{
			if(strcmp(stListFileName.m_name,m_264FileName[i]) == 0)//����¼����ļ�
				bRecordingFile = 1;
		}
		if(bRecordingFile)
			continue;	

		//�ж��ļ��Ƿ���ڣ����������ڵ��ļ�
		if(access(stListFileName.m_name,R_OK|W_OK) != 0)
			continue;
		
		stat(stListFileName.m_name,&fileInfo);
		if(unlink(stListFileName.m_name) != 0)//���ܱ�����ɾ�����ļ�
		{
			plog("err,unlink %s\n",stListFileName.m_name);
			continue;
		}

		//���¼�¼��������¼����Ϊ1���ļ�����Ӧ������¼
		plog("del %s ok\n",stListFileName.m_name);
		char str264Name[MAX_RECFILENAMELEN];
		strcpy(str264Name,stListFileName.m_name);
		while(1)
		{
			lseek(fd,-sizeof(stListFileName),SEEK_CUR);
			stListFileName.m_state = ITEM_STATE_DELETED;
			s32Ret = write(fd,&stListFileName,sizeof(stListFileName));
			if(s32Ret != sizeof(stListFileName))
			{
				plog("err,write,%s\n",strListFile);
				close(fd);
				return S_FAILURE;			
			}		

			s32Ret = read(fd,&stListFileName,sizeof(stListFileName));
			if(s32Ret == 0)
			{
				plog("read end,%s\n",strListFile);
				close(fd);
				//ɾ���ļ���
				DelDir(strOldRecDir);
				return S_SUCCESS;			
			}
			
			if(s32Ret != sizeof(stListFileName))
			{
				close(fd);
				plog("err,read %s\n",strListFile);
				return S_FAILURE;
			}

			if(strcmp(stListFileName.m_name,str264Name) != 0)
				break;
		}
		
		//�ж��Ƿ�ֹͣɾ��
		s32DelSpace += fileInfo.st_size;
		if(s32DelSpace >= ((STOP_DEL_FILE-START_DEL_FILE) << 20))//����20λ��MByteתΪByte
		{
			plog("stop del old record file\n");
			break;
		}
	}

	close(fd);
	return S_FAILURE;		
}
//���������ʣ������
//�����л�����
//�ڴ�����ʱ��������ǣ�����ɾ�����ļ�
//���������������ʾ
//ע��:ʵ���ϣ�ipcֻ��1�����̣��л����̵��߼���������ʵ��
void RecordManage::CheckHdd()
{
	BlockDevInfo_S stBlockDevInfo;
	if(GetBlockDeviceInfo(BLK_DEV_ID_HDD00,&stBlockDevInfo)==-1)
	{
		return ;
	}

	if(stBlockDevInfo.m_u32FreeSpace < START_DEL_FILE
		&& m_uHddOverWrite)
	{
		DelOldRecord();
	}
	else if(stBlockDevInfo.m_u32FreeSpace < START_DEL_FILE
		&& m_uHddOverWrite == 0)
	{
		m_s32HddFull = 1;
	}
}

void RecordManage::CheckTimeValid()
{
	for(int ch = 0; ch < MAX_VIDEO_CHANNEL; ch++)
	{
		m_uTimerValid[ch] = 1;//CheckTimerTable(ch, TIMER_RECORD);
		m_uMDvalid[ch] = 1;//CheckTimerTable(ch, MD_RECORD);
		m_uEventValid[ch] = 1;//CheckTimerTable(ch, ALARM_RECORD);
		m_uMwValid[ch] = 1;//΢��û��ʱ�������
		m_uSD433valid[ch] = 1;//�Ŵ�û��ʱ�������
		m_uAudioValid[ch] = 1;//����û��ʱ�������
	}
}

int RecordManage::IsTimeJump()
{
	time_t cur = (time_t)SysTime2Time(&m_datetime);
	time_t pre = (time_t)SysTime2Time(&m_preTime);

	return (cur - pre > 20) || (pre -cur > 20);
}

void RecordManage::CheckTimeJump()
{	
	if(IsTimeJump())
	{
		for(int ch = 0; ch < MAX_VIDEO_CHANNEL; ch++)
		{
//������Ƭ¼����߼�		
#if 0		
			if(RECORD_ING == m_uRecordStatus[ch])
			{
				m_stEndTime[ch] = m_preTime;
				StopOneChannelRecord(ch);		
				printf("StopOneChannelRecord,CheckTimeJump,ch = %d \n", ch);
			}
#endif
      			/*ͬһСʱ�������ʱ���޸�,���¶�λ���дָ��*/
			if( GetRecordHour(&m_preTime) == GetRecordHour(&m_datetime) )
			{
				for(ch = 0; ch < MAX_VIDEO_CHANNEL; ch++)
				{
					if(RECORD_ING == m_uRecordStatus[ch])
					{
						m_RecEndTime[ch] = m_preTime.hour*3600 + m_preTime.minute*60 + m_preTime.second;
						PauseOneChannelRecord(ch);
						RederectFilePosition(ch,GetCurrentTimeValue());
						//���¶�λ��I֡
						ResetUserData2IFrame(ch, 1, 0); 
						ContinueOneChannelRecord(ch,m_uRecordType[ch]);
					}
					else if(RECORD_STOPED == m_uRecordStatus[ch] )
					{
						if(m_uRecordStatus[ch] == RECORD_STOPED)
						{
							continue;
						}
						
						RederectFilePosition(ch,GetCurrentTimeValue());
					}
				}		
			}
		}		
	}
}

int RecordManage::CheckAlarmRecordValid(int ch)
{
	return (m_u32ExAlarmLink >> ch) & 0x01;
}

int RecordManage::CheckMdRecordValid(int ch)
{
	return (m_u32MdAlarmLink >> ch) & 0x01;
}

int RecordManage::CheckMwRecordValid(int ch)
{
	return (m_u32MwAlarmLink >> ch) & 0x01;
}

int RecordManage::CheckSd433RecordValid(int ch)
{
	return (m_u32SD433AlarmLink >> ch) & 0x01;
}

int RecordManage::CheckAudioRecordValid(int ch)
{
	return (m_u32AudioAlarmLink >> ch) & 0x01;
}

void RecordManage::SetRecInfo(int ch,RECORD_KIND enRecordKind)
{
	if(enRecordKind == ALARM_RECORD){m_uAlarmRecDelay[ch] = RECALARM_DELAY;m_uRecordLevel = m_uRecordLevelAlarm;}
	if(enRecordKind == MD_RECORD) {m_uMDRecDelay[ch] = RECMD_DELAY;m_uRecordLevel = m_uRecordLevelMd;}
	if(enRecordKind == SD433_RECORD) {m_uSD433AlarmRecDelay[ch] = RECMD_DELAY;m_uRecordLevel = m_uRecordLevelSD433;}
	if(enRecordKind == MW_RECORD) {m_uMwAlarmRecDelay[ch] = RECMD_DELAY;m_uRecordLevel = m_uRecordLevelMw;}
	if(enRecordKind == AUDIO_RECORD) {m_uAudioAlarmRecDelay[ch] = RECMD_DELAY;m_uRecordLevel = m_uRecordLevelAudio;}
	if(enRecordKind == TIMER_RECORD) {m_uAudioAlarmRecDelay[ch] = RECMD_DELAY;m_uRecordLevel = m_uRecordLevelTimer;}
	m_uRecordType[ch] = enRecordKind;	
}

int RecordManage::StartOneChannelRecord(int ch,RECORD_KIND enRecordKind)
{	
	//����¼����Ϣ
	SetRecInfo(ch,enRecordKind);

	//��������
	m_uRecordCmd[ch] = RECORD_START;

	//�ȴ����
	plog("StartOneChannelRecord,ch = %d\n",ch);
	while(m_uRecordStatus[ch] != RECORD_ING) usleep(100*100);
	plog("StartOneChannelRecord ok\n");

	return S_SUCCESS;
}

int RecordManage::ContinueOneChannelRecord(int ch,RECORD_KIND enRecordKind)
{	
	//����¼����Ϣ
	SetRecInfo(ch,enRecordKind);

	//��������
	m_uRecordCmd[ch] = RECORD_CONTINUE;

	//�ȴ����
	plog("ContinueOneChannelRecord,ch = %d\n",ch);
	while(m_uRecordStatus[ch] != RECORD_ING) usleep(100*100);
	plog("ContinueOneChannelRecord ok\n");

	return S_SUCCESS;
}

void RecordManage::RestartOneChannelRecord(int ch,RECORD_KIND enRecordKind)
{
	StopOneChannelRecord(ch);
	StartOneChannelRecord(ch,enRecordKind);
}

void RecordManage::ChangeOneChannelRecord(int ch,RECORD_KIND enRecordKind)
{
	PauseOneChannelRecord(ch);
	ContinueOneChannelRecord(ch,enRecordKind);
}

int RecordManage::IsLowRecordType(int ch,RECORD_KIND enRecordKind)
{
	if(enRecordKind == ALARM_RECORD && m_uRecordType[ch] != ALARM_RECORD) return 1;
	if(enRecordKind == SD433_RECORD && (m_uRecordType[ch] == MW_RECORD || m_uRecordType[ch] == MD_RECORD || m_uRecordType[ch] == AUDIO_RECORD ||m_uRecordType[ch] == TIMER_RECORD )) return 1;
	if(enRecordKind == MW_RECORD && (m_uRecordType[ch] == MD_RECORD || m_uRecordType[ch] == AUDIO_RECORD ||m_uRecordType[ch] == TIMER_RECORD )) return 1;
	if(enRecordKind == MD_RECORD && (m_uRecordType[ch] == AUDIO_RECORD ||m_uRecordType[ch] == TIMER_RECORD )) return 1;	
	if(enRecordKind == AUDIO_RECORD && (m_uRecordType[ch] == TIMER_RECORD )) return 1;
	return 0;
}

void  RecordManage::CheckRecordStart()
{
	int currenttime =  GetCurrentTimeValue();

	if(m_s32HddFull)
		return;
	
	for(int ch = 0; ch < MAX_VIDEO_CHANNEL; ch++)
	{
		if(m_uHddEio[ch])
		{
			plog("m_uHddEio[%d] = %d\n",ch,m_uHddEio[ch]);
			continue;
		}

		if((m_uEventValid[ch] > 0)&&(CheckAlarmRecordValid(ch) > 0)) //*����¼��
		{
			if(m_uRecordStatus[ch] == RECORD_ING)//����¼��
			{
				if(m_uRecordType[ch] != ALARM_RECORD)//��ǰ¼���Ǹ澯¼��
				{
					if((currenttime - m_RecStartTime[ch] >= MIN_SYS_ALARM_DELAY))
					{
						plog("stop for prioty < alarm record\n");
						ChangeOneChannelRecord(ch,ALARM_RECORD);
					}
					
					//δ����С¼��ʱ���������л�	
				}
				else//��ǰ¼���Ǹ澯¼�񣬸��¾���¼��ʱ��
				{
					m_uAlarmRecDelay[ch] = RECALARM_DELAY;
				}	   
			}
			else//����¼��
			{
				plog("start alarm ecord, ch = %d\n",ch);
				StartOneChannelRecord(ch,ALARM_RECORD);
			}
		}
		else if(m_uSD433valid[ch] > 0  && CheckSd433RecordValid(ch)) //*�Ŵ����¼��
		{
		  	if(m_uRecordStatus[ch] == RECORD_ING)//����¼��
		  	{
				if(m_uRecordType[ch] != SD433_RECORD)//�����ȼ���¼��
				{
					if((currenttime - m_RecStartTime[ch]) >= MIN_SYS_ALARM_DELAY)
					{
						plog("stop for prioty < sd433 record\n");
						ChangeOneChannelRecord(ch,SD433_RECORD);
					}
					
					//δ����С¼��ʱ���������л�	
				}
				else//��ǰ¼�����Ŵ�¼�񣬸��¾���¼��ʱ��
				{
					m_uSD433AlarmRecDelay[ch] = RECALARM_DELAY;
				}
			}
			else //����¼��
			{
				plog("start sd433 record, ch = %d\n",ch);
				StartOneChannelRecord(ch,SD433_RECORD);
			}
		}	
		else if(m_uMwValid[ch] > 0  && CheckMwRecordValid(ch)) //*΢�����¼��
		{
		  	if(m_uRecordStatus[ch] == RECORD_ING)//����¼��
		  	{
				if(m_uRecordType[ch] != MW_RECORD)//�����ȼ���¼��
				{
					if((currenttime - m_RecStartTime[ch]) >= MIN_SYS_ALARM_DELAY)
					{
						plog("stop for prioty < mw record\n");
						ChangeOneChannelRecord(ch,MW_RECORD);
					}
					
					//δ����С¼��ʱ���������л�	
				}
				else//��ǰ¼����΢��¼�񣬸��¾���¼��ʱ��
				{
					m_uMwAlarmRecDelay[ch] = RECALARM_DELAY;
				}
			}
			else //����¼��
			{
				plog("start mw record, ch = %d\n",ch);
				StartOneChannelRecord(ch,MW_RECORD);
			}
		}				
		else if(m_uMDvalid[ch] > 0  && CheckMdRecordValid(ch)) //*�ƶ����¼��
		{
		  	if(m_uRecordStatus[ch] == RECORD_ING)//����¼��
		  	{
				if(m_uRecordType[ch] != MD_RECORD)//�����ȼ���¼��
				{
					if((currenttime - m_RecStartTime[ch]) >= MIN_SYS_ALARM_DELAY)
					{
						plog("stop for prioty < md record\n");
						ChangeOneChannelRecord(ch,MD_RECORD);
					}
					
					//δ����С¼��ʱ���������л�	
				}
				else//��ǰ¼�����ƶ�¼�񣬸��¾���¼��ʱ��
				{
					m_uMDRecDelay[ch] = RECALARM_DELAY;
				}
			}
			else //����¼��
			{
				plog("start md ecord, ch = %d\n",ch);
				StartOneChannelRecord(ch,MD_RECORD);
			}
		}
		else if(m_uAudioValid[ch] > 0  && CheckAudioRecordValid(ch)) //*�������¼��
		{
		  	if(m_uRecordStatus[ch] == RECORD_ING)//����¼��
		  	{
				if(m_uRecordType[ch] != AUDIO_RECORD)//�����ȼ���¼��
				{
					if((currenttime - m_RecStartTime[ch]) >= MIN_SYS_ALARM_DELAY)
					{
						plog("stop for prioty < audio record\n");
						ChangeOneChannelRecord(ch,AUDIO_RECORD);
					}
					
					//δ����С¼��ʱ���������л�	
				}
				else//��ǰ¼��������¼�񣬸��¾���¼��ʱ��
				{
					m_uAudioAlarmRecDelay[ch] = RECALARM_DELAY;
				}
			}
			else //����¼��
			{
				plog("start audio record, ch = %d\n",ch);
				StartOneChannelRecord(ch,AUDIO_RECORD);
			}
		}		
		else if(m_uTimerValid[ch] > 0) //��ʱ¼��
		{
		  	if(m_uRecordStatus[ch] == RECORD_ING)//����¼��
		  	{			
			  	if(m_uRecordType[ch] != TIMER_RECORD)//����¼��
			  	{
					if((currenttime - m_RecStartTime[ch]) >= MIN_SYS_ALARM_DELAY)
					{
						plog("stop for prioty < timer record\n");
						ChangeOneChannelRecord(ch,TIMER_RECORD);
					}
				}
		  	}
			else //����¼��
			{
				plog("start timer ecord, ch = %d\n",ch);
				StartOneChannelRecord(ch,TIMER_RECORD);
			}
		}
	}
}

int RecordManage::GetCurrentTimeValue()
{
	return m_datetime.hour*3600 + m_datetime.minute*60 + m_datetime.second;
}

void RecordManage::RecordTime2SysTime(int s32RecordTime,SystemDateTime *pstSystemDateTime)
{
	pstSystemDateTime->hour = s32RecordTime/3600;
	pstSystemDateTime->minute = (s32RecordTime%3600)/60;
	pstSystemDateTime->second = (s32RecordTime%3600%60);
}

int RecordManage::GetRecordHour(SystemDateTime  *pstSystemDateTime)
{
	return (((pstSystemDateTime->year)*365+pstSystemDateTime->month*30+pstSystemDateTime->mday)*24+pstSystemDateTime->hour);//m_datetime.year,�Ѿ���ȥ2000
}

int RecordManage::StopOneChannelRecord(int ch)
{
	//��������
	m_uRecordCmd[ch] = RECORD_STOP;

	//�ȴ����
	plog("StopOneChannelRecord,ch = %d\n",ch);
	while(m_uRecordStatus[ch] != RECORD_STOPED) usleep(100*100);	
	plog("StopOneChannelRecord ok\n");

	return S_SUCCESS;
}

int RecordManage::PauseOneChannelRecord(int ch)
{
	//��������
	m_uRecordCmd[ch] = RECORD_PALSE;

	//�ȴ����
	plog("PauseOneChannelRecord,ch = %d\n",ch);
	while(m_uRecordStatus[ch] != RECORD_PALSEED) usleep(100*100);	
	plog("PauseOneChannelRecord ok\n");

	return S_SUCCESS;
}

//* step0 ��ⱨ��¼��
//* step1 ����¼�¼��
//* step2 ��ⶨʱ¼��
//* ����Ƿ�ת����Сʱ
void  RecordManage::CheckRecordEnd()
{
	int currenttime =  GetCurrentTimeValue();

	for(int ch = 0; ch < MAX_VIDEO_CHANNEL; ch++)
	{
		m_stEndTime[ch] = m_datetime;
		
		if(RECORD_STOPED == m_uRecordStatus[ch])
			continue;
		
		if(ALARM_RECORD == m_uRecordType[ch] )
		{
			if(!m_uAlarmRecDelay[ch])//�ⲿ�澯¼����ʱ�ѵ�
			{
				plog(" alarm record %d end delay: %d\n", ch, m_uAlarmRecDelay[ch]);
				StopOneChannelRecord(ch);
			}
		}
		else if(MD_RECORD == m_uRecordType[ch])
		{
			if(!m_uMDRecDelay[ch]) 
			{
				plog(" md  record  %d end delay: %d\n", ch, m_uMDRecDelay[ch]);
				StopOneChannelRecord(ch);
			}
		}
		else if(TIMER_RECORD == m_uRecordType[ch])
		{
			if(!m_uTimerValid[ch])
			{
				plog(" timer record %d end\n", ch);
				StopOneChannelRecord(ch);
			}
		}

		if((currenttime < m_RecStartTime[ch]))
		{
			plog(" timeleft:%d, timevalue:%d, starttime:%d\n", 
				currenttime - m_RecStartTime[ch], currenttime, m_RecStartTime[ch]);	
			StopOneChannelRecord(ch);
		}

		if(m_uHddEio[ch])
		{
			printf(" write hdd error stop rec : %d \n", ch);		
			StopOneChannelRecord(ch);
		}
	}

	/*���Сʱ��ͬͣ��¼��*/
	//printf("%s(%d) \n",__FILE__,__LINE__,GetRecordHour(&m_datetime));
	//printf("%s(%d) \n",__FILE__,__LINE__,GetRecordHour(&m_datetime));
	if(GetRecordHour(&m_datetime) != GetRecordHour(&m_preTime))
	{
		plog("The hour has been changed\n");
		
		//�Ȳ���i֡���ٹر��ļ���ʹ����һ��¼���ļ��������㴦��I֡
		for(int ch = 0; ch < MAX_VIDEO_CHANNEL; ch++)
		{
			ForceIdrInsertion(ch,1);//STREAM_TYPE_HIGHT=0
		}

		for(int ch = 0; ch < MAX_VIDEO_CHANNEL; ch++)
		{
			if(RECORD_STOPED == m_uRecordStatus[ch])
				continue;		

			StopOneChannelRecord(ch);
		}
	}	
}

void RecordManage::CheckAlarmDelay()
{
	for(int ch = 0; ch < MAX_VIDEO_CHANNEL; ch++)
	{
		if(m_uAlarmRecDelay[ch] > 0)
		{
			m_uAlarmRecDelay[ch] = m_uAlarmRecDelay[ch] -1;
		}

		if(m_uMDRecDelay[ch] > 0)
		{
			m_uMDRecDelay[ch] = m_uMDRecDelay[ch] - 1;
		}
	}
}

void RecordManage::CheckRecordTask()
{	
	CheckHdd();
		
	CheckTimeValid();//�鶨ʱ¼��

	CheckRecordStart();//��¼��ʼ

	CheckTimeJump();//��ʱ������
	
	CheckRecordEnd();//��¼�����

	CheckAlarmDelay();//�龯��¼��		
}
void RecordManage::DeleteHistoryVersionRecord()
{
	if(access((char *)("/hdd00/p01/index/"), F_OK)==0)
	{
		printf("\n\n\n\n\n\n\n\n\n\n\n\n +++++++++++++++HistoryVersionRecord exist!!!\n\n\n");
		system("rm /hdd00/p01/* -rf");
		
	}
	else
	{
		printf("\n\n\n\n\n\n\n\n\n\n\n\n  --------------HistoryVersionRecord not exist!!!\n\n\n");
	}

}
/*
�޸�δ�����رյ��ļ���������
ע��:
ֻ�޸���ǰ��¼��
*/
void RecordManage::fixNotCloseFile()
{
	GetSysTime(&m_datetime); //*¼��ʱ��	

	char hdddirname[MAX_RECFILENAMELEN] = {0};
	sprintf(hdddirname, "/hdd%02d/p01/record", m_nCurrentHddNum);
	if(access(hdddirname,R_OK|W_OK) != 0)
	{
		plog("err,%s not exist\n", hdddirname);
		return;
	}
	
	sprintf(hdddirname, "/hdd%02d/p01/record/%04d-%02d-%02d", m_nCurrentHddNum,m_datetime.year+2000,m_datetime.month,m_datetime.mday);
	struct dirent *entry = NULL;
	DIR *dir = NULL;
	dir = opendir(hdddirname);
	if(dir == NULL)
	{
		plog("dir no exist,%s\n",hdddirname);
		return; 
	}	

	int s32Strlen;
	char str264Name[MAX_RECFILENAMELEN] = {0};
	char strIdxName[MAX_RECFILENAMELEN] = {0};
	EVEN_ITEM stEvenItem[MAX_EVEN_NUM];
	int s32EvenNum = 0;	
	int s32RecCh;
	FILE	*pfp;
	VideoFileInfo	stFileInfo;
	while(1)
	{
		if((entry = readdir(dir)) != NULL)
		{
			s32Strlen = strlen(entry->d_name);//recfile_-160121-000000-005959-00001100.264,s32Strlen=42
			if(s32Strlen != 42)
			{
				continue;
			}

			if(strstr(entry->d_name,"264") == NULL)
			{
				continue;
			}			

			snprintf(str264Name,sizeof(str264Name),"%s/%s",hdddirname,entry->d_name);
			strcpy(strIdxName,str264Name);
			//68��"/hdd00/p01/record/2015-12-05/recfile_-151205-161300-162903-41U10300.264"����׺264���±�
			memcpy(strIdxName+68,"idx",strlen("idx"));			


			//����264���ļ�ͷ
			pfp = fopen(str264Name, "rb+");
			if(pfp == NULL)
			{
				plog("err,fopen %s,%s :%d\n", str264Name,strerror(errno), errno);
				continue;	
			}
			int retval = fread(&stFileInfo, sizeof(unsigned char), sizeof(stFileInfo), pfp);		
			if(retval !=  sizeof(stFileInfo))
			{
				fclose(pfp);
				plog("err,fread %s,%s :%d\n", str264Name,strerror(errno), errno);
				continue;		
			}
			if(stFileInfo.m_bNonStop == 0)
			{
				fclose(pfp);
				continue;		
			}
			
			//�������¼�
			s32RecCh = strIdxName[64];
			if(s32RecCh >= 'A')
				s32RecCh = s32RecCh - 'A' + 10;
			else
				s32RecCh = s32RecCh - '0';	

			plog("%s(%d)->%s,%s\n",__FILE__,__LINE__,__FUNCTION__,strIdxName);
			
			s32RecCh--;
			ParserEvenFromIframeIndex(strIdxName, stEvenItem,&s32EvenNum, s32RecCh);
			for(int i = 0; i < s32EvenNum; i++)
			{
				AddOneEven2List(hdddirname,&stEvenItem[i]);
			}		

			//����h264��ͷ��
			stFileInfo.m_bNonStop = 0;
			fseek(pfp, 0, SEEK_SET);
			retval = fwrite(&stFileInfo, sizeof(unsigned char), sizeof(stFileInfo), pfp);		
			if(retval !=  sizeof(stFileInfo))
			{
				fclose(pfp);
				plog("err,fwrite %s,%s :%d\n", str264Name,strerror(errno), errno);
				continue;		
			}			
			fclose(pfp);	
			
		}
		else 
		{
			break;
		}
	}		
}
	
void RecordManage::RecordCtrlLoop()
{
	DeleteHistoryVersionRecord();
	fixNotCloseFile();

	GetSysTime(&m_datetime);
	m_preTime = m_datetime;
		
	while(s32CtrlThreadQuit == 0)
	{
		if(!CheckRecordTime())
		{
			sleep(1);
			continue;
		}
		GetSysTime(&m_datetime); //*¼��ʱ��
		
		CheckRecordTask();

		m_preTime = m_datetime;
		sleep(1);
	}
}

int RecordManage::StartRecordSystem()
{
	if(pthread_create(&ctrlThreadId , NULL, RecordCtrlThread, this) < 0)
	{
		plog("create record data thread failure \n");
		return S_FAILURE;
	}

	if(pthread_create(&dataThreadId , NULL, RecordDataThread, this) < 0)
	{
		plog("create record data thread failure \n");
		return S_FAILURE;
	}	

	return S_SUCCESS;
}

int RecordManage::StopRecordSystem()
{

	s32DataThreadQuit = 1;
	s32CtrlThreadQuit = 1;
	
	int ret = pthread_join(ctrlThreadId, NULL);
	if (ret < 0) {
		perror("ctrlThreadId");
		return S_FAILURE;
	}
	
	ret = pthread_join(dataThreadId, NULL);
	if (ret < 0) {
		perror("ctrlThreadId");
		return S_FAILURE;
	}
	InitRecordManage();

	return S_SUCCESS;
}

void RecordManage::SetRecSchedule(GROUPRECORDTASK *pRecSchedule)
{
	m_RecSchedule = *pRecSchedule;
}

void RecordManage::SetMdAlarmLink(unsigned int u32MdAlarmLink,RECORD_LEVEL enRecordLevel)
{
	if(u32MdAlarmLink == 1)//��1
	{
		//printf("%s(%d)->%s,u32MdAlarmLink=%d\n",__FILE__,__LINE__,__FUNCTION__,u32MdAlarmLink);
		m_u32MdAlarmLink = u32MdAlarmLink;
		m_uRecordLevelMd= enRecordLevel;
		m_MDStartTime[0] = GetCurrentTimeValue();
	}
	if(u32MdAlarmLink == 0 && ABS(GetCurrentTimeValue()-m_MDStartTime[0]) > MIN_SYS_ALARM_DELAY*12)//��0
	{
		m_u32MdAlarmLink = u32MdAlarmLink;
	}	
}

void RecordManage::SetExAlarmLink(unsigned int u32ExAlarmLink,RECORD_LEVEL enRecordLevel)
{
	//printf("%s(%d)->%s,u32ExAlarmLink=%d\n",__FILE__,__LINE__,__FUNCTION__,u32ExAlarmLink);
	m_u32ExAlarmLink = u32ExAlarmLink;
	
	if(u32ExAlarmLink == 1)//��1
	{
		m_uRecordLevelAlarm= enRecordLevel;	
	}
}

void RecordManage::SetMwAlarmLink(unsigned int u32MwAlarmLink,RECORD_LEVEL enRecordLevel)
{
	//printf("%s(%d)->%s,u32MwAlarmLink=%d\n",__FILE__,__LINE__,__FUNCTION__,u32MwAlarmLink);
	m_u32MwAlarmLink = u32MwAlarmLink;
	
	if(u32MwAlarmLink == 1)
	{
		m_uRecordLevelMw= enRecordLevel;	
	}
}

void RecordManage::SetSD433AlarmLink(unsigned int u32SD433AlarmLink,RECORD_LEVEL enRecordLevel)
{
	//printf("%s(%d)->%s,m_u32SD433AlarmLink=%d\n",__FILE__,__LINE__,__FUNCTION__,m_u32SD433AlarmLink);
	m_u32SD433AlarmLink = u32SD433AlarmLink;

	if(u32SD433AlarmLink == 1)
	{
		m_uRecordLevelSD433= enRecordLevel;	
	}
}

void RecordManage::SetAudioAlarmLink(unsigned int u32AudioAlarmLink,RECORD_LEVEL enRecordLevel)
{
	if(u32AudioAlarmLink == 1)//��1
	{
		//printf("%s(%d)->%s,u32AudioAlarmLink=%d\n",__FILE__,__LINE__,__FUNCTION__,u32AudioAlarmLink);
		m_u32AudioAlarmLink = u32AudioAlarmLink;
		m_uRecordLevelAudio= enRecordLevel;
		m_AudioStartTime[0] = GetCurrentTimeValue();
	}
	if(u32AudioAlarmLink == 0 && ABS(GetCurrentTimeValue()-m_AudioStartTime[0]) > MIN_SYS_ALARM_DELAY*12)//��0
	{
		m_u32AudioAlarmLink = u32AudioAlarmLink;
	}
}

void RecordManage::SetHddOverWrite(unsigned char u8HddOverWrite)
{
	m_uHddOverWrite = u8HddOverWrite;
}

void RecordManage::FillRecDirent(rec_dirent *pstRecDirent,RECORD_LEVEL enRecordLevel,int s32StartTime,int s32EndTime,int s32Size,
	                          int channel,RECORD_KIND enRecordKind,char *pstrFileName)
{
	/*Level��bug��û�ҳ�ԭ����ʱ�����ģ�ʵʱ¼��Ķ���level3����ʵʱ¼���Ϊlevel1*/
	
	if(1==enRecordKind)
		pstRecDirent->level = RECORD_LEVEL1;
	else
		pstRecDirent->level = RECORD_LEVEL3;	
//	pstRecDirent->level = (unsigned short)enRecordLevel;
	pstRecDirent->start_time = s32StartTime;
	pstRecDirent->end_time = s32EndTime;
	pstRecDirent->filesize = s32Size;
	pstRecDirent->channel = channel;
	pstRecDirent->m_filetype = (int)enRecordKind;

//	plog("FillRecDirent   pstRecDirent->level   [%d]      enRecordLevel  [%d]    pstRecDirent->m_filetype[%d]    enRecordKind[%d]\r\n",pstRecDirent->level,enRecordLevel,pstRecDirent->m_filetype,enRecordKind);
	strcpy(pstRecDirent->d_name,pstrFileName);
}

int RecordManage::GetEndTimeFrom264(char *pstr264Name)
{
	char strIdxName[MAX_RECFILENAMELEN];
	strcpy(strIdxName,pstr264Name);
	memcpy(strIdxName+68,"idx",strlen("idx"));

	int fd = open(strIdxName,O_RDWR, 0777);
	if(fd < 0)
	{
		plog("err,open,%s\n",strIdxName);
		return S_FAILURE;
	}

	IFRAMELISTCONTENT content;
	lseek(fd,-sizeof(content),SEEK_END);	

	int s32Ret = read(fd,&content,sizeof(content));
	if(s32Ret != sizeof(content))//�ļ�����
	{
		plog("err,read,%s\n",strIdxName);
		close(fd);
		return S_FAILURE;
	}	

	close(fd);
	return content.m_iframetime;
}
int RecordManage::GetRecordOffsetFrom264(const char* pstr264Name, unsigned int start_time, unsigned int *pOffset)
{
/*
	int fd = open(pstr264Name,O_RDWR, 0777);
	if(fd < 0)
	{
		return S_FAILURE;
	}

	lseek(fd, sizeof(IDX_HEADER), SEEK_SET);
	IFRAMELISTCONTENT stIframeListContentCur;	
	while(1)
	{
		int len = read(fd, &stIframeListContentCur, sizeof(stIframeListContentCur));
		if(len <= 0)
		{
			close(fd);
			return S_FAILURE;
		}
		
		if(stIframeListContentCur.m_iframetime >= start_time)
		{
			*pOffset = stIframeListContentCur.m_offset;
			break;	
		}
	}	

	close(fd);
	return S_SUCCESS;
*/
	return S_FAILURE;
}

void RecordManage::UpdateListFile(char *pstrIdxName,int ch)
{
	//�������¼�
	EVEN_ITEM stEvenItem[MAX_EVEN_NUM];
	int s32EvenNum = 0;	
	ParserEvenFromIframeIndex(pstrIdxName, stEvenItem,&s32EvenNum, ch);
	for(int i = 0; i < s32EvenNum; i++)
	{
		char dir[MAX_RECFILENAMELEN];
		memset(dir,0,sizeof(dir));
		memcpy(dir,pstrIdxName,28);// /hdd00/p01/record/2015-01-16
		AddOneEven2List(dir,&stEvenItem[i]);
	}		
}

int  RecordManage::FindRecordFile(FindFileType *findType,RecordFileName *RecordFile, REC_SEARCH enMode,int s32MaxItenNum)
{
	if(enMode == REC_SEARCH_SET)
	{
		//��ͷ��ʼ���������¼����Ҫ��������
		for(int i = 0; i < MAX_VIDEO_CHANNEL; i++)
		{
			//���죬��������¼��
			if(findType->time.tm_year == m_datetime.year 
				&& findType->time.tm_mon == m_datetime.month
				&& findType->time.tm_mday == m_datetime.mday
				&& (strlen(m_IdxFileName[i]) != 0))
			{
				//printf("%s(%d) \n",__FILE__,__LINE__);
				if(s32MaxItenNum == 1)//ֻ��ȡ������û��¼��
				{
					RecordFile->fileNb = 1;	
					return S_SUCCESS;
				}
				else
				{
					UpdateListFile(m_IdxFileName[i],i);					
				}				
			}
		}
		
		m_s32IdxOffset = sizeof(LISTFILE_FILEHEADER);
	}
	
	//���б��ļ�
	char strListFile[MAX_RECFILENAMELEN];
	snprintf(strListFile,sizeof(strListFile),"/hdd%02d/p01/record/%04d-%02d-%02d/%s",m_nCurrentHddNum,
									2000+findType->time.tm_year,findType->time.tm_mon,findType->time.tm_mday,RECORD_LIST_FILENAME);
	int fd = open(strListFile,O_RDWR, 0777);
	if(fd < 0)
	{
		//printf("err,open,%s\n",strListFile);
		return S_FAILURE;
	}

	//
	int s32Ret,s32Size,s32EndTime;
	LISTFILE_NAME stListFileName;
	//struct stat fileInfo;
	RecordFile->fileNb = 0;
	lseek(fd,m_s32IdxOffset,SEEK_SET);
	while(1)
	{
		s32Ret = read(fd,&stListFileName,sizeof(stListFileName));
		if(s32Ret == 0)
		{
			plog("read end,%s\n",strListFile);
			plog("FindRecordFile ok,fileNb = %d\n",RecordFile->fileNb);
			m_s32IdxOffset = lseek(fd,0,SEEK_CUR);
			close(fd);
			return S_SUCCESS;			
		}
		
		if(s32Ret != sizeof(stListFileName))
		{
			close(fd);
			plog("err,read %s\n",strListFile);
			return S_FAILURE;
		}	
		
		if(stListFileName.m_state == ITEM_STATE_DELETED)//�Ѿ���ɾ��
			continue;		

		//printf("%s(%d)->%s\n",__FILE__,__LINE__,stListFileName.m_name);
		//�ж��ļ��Ƿ����
		if(access(stListFileName.m_name,R_OK|W_OK) != 0)
			continue;

		//�ж�¼������
#if 0		
		if((findType->RecordType == 1 && stListFileName.m_record_type != ALARM_RECORD)
			|| (findType->RecordType == 2 && stListFileName.m_record_type != MD_RECORD)
			|| (findType->RecordType == 3 && stListFileName.m_record_type != TIMER_RECORD)
			|| findType->RecordType == 5)
		{
			continue;
		}
#else
		if((findType->RecordType == 1 && stListFileName.stEvenItem.m_RecType != ALARM_RECORD)
			|| (findType->RecordType == 2 && stListFileName.stEvenItem.m_RecType != MD_RECORD)
			|| (findType->RecordType == 3 && stListFileName.stEvenItem.m_RecType != TIMER_RECORD)
			|| findType->RecordType == 5)
		{
			continue;
		}
#endif		
		//�ļ�����Ҫ��
		//��ȡ������Ϣ�����ṹ��
		//stat(stListFileName.m_name,&fileInfo);
		s32Size = (stListFileName.stEvenItem.m_EndOffset-stListFileName.stEvenItem.m_StartOffset)/1000;
		//ZD_DEBUG_Printf("%s(%d)->s32Size=%d\n",__FILE__,__LINE__,s32Size);

		//��ʱ�����һ�������s32Size
		//δ�ҵ����ֹ��ɣ�ԭ�����������ٸ���
		//������ʱ�������ļ�
		if(s32Size > 100*1000*1000)
		{
			ZD_DEBUG_Printf("%s(%d)->m_name=%s\n",__FILE__,__LINE__,stListFileName.m_name);
			ZD_DEBUG_Printf("%s(%d)->s32Size=%d\n",__FILE__,__LINE__,s32Size);
			ZD_DEBUG_Printf("%s(%d)->start_time=%d\n",__FILE__,__LINE__,stListFileName.stEvenItem.start_time);
			ZD_DEBUG_Printf("%s(%d)->end_time=%d\n",__FILE__,__LINE__,stListFileName.stEvenItem.end_time);
			ZD_DEBUG_Printf("%s(%d)->m_RecType=%d\n",__FILE__,__LINE__,stListFileName.stEvenItem.m_RecType);
			continue;
		}
#if 0		
		if(stListFileName.M_filestat == ITEM_FILE_STAT_CREAT && strcmp(stListFileName.m_name,m_264FileName[stListFileName.m_channel-1]) == 0)//����¼����ļ�
			s32EndTime = GetCurrentTimeValue();//��Ϊ�����Զ��޸�����δ�򿪵��ļ�����������¼����ļ���ֻ��������д��Ƶ֡���ļ�
		else if(stListFileName.M_filestat == ITEM_FILE_STAT_CREAT)
			s32EndTime = GetEndTimeFrom264(stListFileName.m_name);
		else
			s32EndTime = stListFileName.m_end_time;
#else
		s32EndTime = stListFileName.stEvenItem.end_time;		
#endif
		FillRecDirent(&RecordFile->namelist[RecordFile->fileNb],(RECORD_LEVEL)stListFileName.stEvenItem.m_RecLevel,stListFileName.stEvenItem.start_time,s32EndTime,s32Size,
					stListFileName.stEvenItem.m_RecCh,(RECORD_KIND)stListFileName.stEvenItem.m_RecType,stListFileName.m_name);		
		
		RecordFile->fileNb++;
		if(RecordFile->fileNb == s32MaxItenNum)
		{
			m_s32IdxOffset = lseek(fd,0,SEEK_CUR);
			break;
		}
	}
	
	close(fd);
	plog("FindRecordFile ok,fileNb = %d\n",RecordFile->fileNb);
	return S_SUCCESS;
}

int  RecordManage::HaveRecordFile(struct tm time)
{
	FindFileType findType;
	RecordFileName RecordFile;
	
	findType.time = time;
	findType.channel = 1;
	findType.RecordType = 0;

	memset(&RecordFile,0,sizeof(RecordFile));
	FindRecordFile(&findType,&RecordFile,REC_SEARCH_SET,1);
	return (RecordFile.fileNb > 0);
}

int RecordManage::GetRecordOffset(const char* path, unsigned int start_time, unsigned int *pOffset)
{
	char strIdxName[MAX_RECFILENAMELEN];
	strcpy(strIdxName,path);
	memcpy(strIdxName+68,"idx",strlen("idx"));

	int fd = open(strIdxName,O_RDWR, 0777);
	if(fd < 0)
	{
		return GetRecordOffsetFrom264(path, start_time, pOffset);
	}

	lseek(fd, sizeof(IDX_HEADER), SEEK_SET);
	IFRAMELISTCONTENT stIframeListContentCur;	
	while(1)
	{
		int len = read(fd, &stIframeListContentCur, sizeof(stIframeListContentCur));
		if(len <= 0)
		{
			close(fd);
			return S_FAILURE;
		}
		
		if(stIframeListContentCur.m_iframetime >= start_time)
		{
			*pOffset = stIframeListContentCur.m_offset;
			//ZD_DEBUG_Printf("%s(%d)->%d\n",__FILE__,__LINE__,*pOffset);
			break;	
		}
	}	

	close(fd);
	return S_SUCCESS;
}


/*
����:pstrDirName��·��������ʽ/hdd00/p01/record/2014-10-10/
		pstEvenItem��ֻʹ��m_RecCh��start_time��end_time��
���:pstEvenItem�����г�Ա������
����ֵ:0����ʾ�ҵ���-1����ʾδ�ҵ���
*/
int  RecordManage::CheckOneEvenFromList(char *pstrDirName,EVEN_ITEM *pstEvenItem)
{
	char listfile[96]={0};
	LISTFILE_NAME  reclistfile;
	FILE *fp = NULL;    
	
	//�ϳ�list�ļ���
	snprintf(listfile,sizeof(listfile),"%s%s",pstrDirName,RECORD_LIST_FILENAME);
	
	//��ȡ�¼�
	fp = fopen(listfile, "rb");
	if(fp == NULL)
	{
		plog("%s(%d)->fopen error,listfile=%s\n",__FILE__,__LINE__,listfile);
		return S_FAILURE;
	} 

	fseek(fp, sizeof(LISTFILE_FILEHEADER), SEEK_SET);
	while(1)
	{
		int readRet = fread(&reclistfile, 1, sizeof(reclistfile), fp);
		if(readRet != sizeof(reclistfile))
		{
			//printf("%s %d %s fread end, readRet=%d!\n",__FILE__,__LINE__,__FUNCTION__,readRet);
			break;
		}

		if(pstEvenItem->m_RecCh == reclistfile.stEvenItem.m_RecCh
			&& pstEvenItem->start_time == reclistfile.stEvenItem.start_time
			&& pstEvenItem->end_time == reclistfile.stEvenItem.end_time
			&& reclistfile.m_state != ITEM_STATE_DELETED)
		{
			*pstEvenItem = reclistfile.stEvenItem;
			fclose(fp);
			plog("%s(%d)->found,m_RecCh=%d,start_time=%d,end_time=%d\n",__FILE__,__LINE__,pstEvenItem->m_RecCh,pstEvenItem->start_time,pstEvenItem->end_time);			
			return S_SUCCESS;
		}
	}

	fclose(fp);
	
	return S_FAILURE;
}


/*
�ڶ����֣�д¼���߼�
*/
void *RecordDataThread(void *arg)
{
	plog("%s %d %s tid:[%d] pid:[%d] ppid:[%d]\n", __FILE__,__LINE__,__FUNCTION__,(int)pthread_self(),(int)getpid(),(int)getppid());
//	pthread_detach(pthread_self());
	RecordManage *record = (RecordManage *)arg;

	record->RecordDataProcess();	
	return NULL;
}

int RecordManage::TestAndCreateDir(char *dirname)
{
	if(access(dirname,R_OK|W_OK) != 0)
	{
		if (mkdir (dirname, 0777) < 0)
		{ 
			perror("Create Record folder failed!\n");
			return S_FAILURE;
		}
	}
	return S_SUCCESS;
}

int RecordManage::CreateRecordDirectory()
{
	char hdddirname[MAX_RECFILENAMELEN] = {0};
	sprintf(hdddirname, "/hdd%02d/p01/record", m_nCurrentHddNum);
	if(TestAndCreateDir(hdddirname) != S_SUCCESS)
	{
		plog("err,TestAndCreateDir,%s\n", hdddirname);
		return S_FAILURE;
	}
	sprintf(hdddirname, "/hdd%02d/p01/record/%04d-%02d-%02d", m_nCurrentHddNum,m_datetime.year+2000,m_datetime.month,m_datetime.mday);
	if(TestAndCreateDir(hdddirname) != S_SUCCESS)
	{
		plog("err,TestAndCreateDir,%s\n", hdddirname);
		return S_FAILURE;
	}
	
	//����
	strcpy(m_strRecDir,hdddirname);
	return S_SUCCESS;
}

int RecordManage::Create264File(int ch)
{	
	int b264FileExist = access(m_264FileName[ch],R_OK|W_OK) == 0;
	if(b264FileExist)//����
	{
		m_pRecFp[ch] = fopen(m_264FileName[ch], "rb+");
	}
	else
	{
		m_pRecFp[ch] = fopen(m_264FileName[ch], "wb+");
	}
	
	if(m_pRecFp[ch] == NULL)
	{
		plog("���ļ�ʧ�� %s\n", strerror(errno));
		return S_FAILURE;
	}

	plog("%s(%d)->open %s ok\n",__FILE__,__LINE__,m_264FileName[ch]);
	
	int retval = fwrite(&m_FileInfo[ch], sizeof(unsigned char), sizeof(m_FileInfo[ch]), m_pRecFp[ch]);		
	if(retval !=  sizeof(m_FileInfo[ch]))
	{
		fclose(m_pRecFp[ch]);
		m_pRecFp[ch] = NULL;
		unlink(m_264FileName[ch]);
		memset((m_264FileName[ch]), 0, MAX_RECFILENAMELEN);
		return S_FAILURE;
	}

	//������ļ����ڣ�����Ҫ��λ�ļ�ĩβ
	if(b264FileExist)
		fseek(m_pRecFp[ch], 0, SEEK_END);	

	//
	m_offset[ch] = ftell(m_pRecFp[ch]);	
	ZD_DEBUG_Printf("%s(%d)->%d\n",__FILE__,__LINE__,m_offset[ch]);
	return S_SUCCESS;
}


int RecordManage::CreateIdxFile(int ch)
{
	IDX_HEADER  header;
	m_index_fds[ch] = open(m_IdxFileName[ch], O_CREAT|O_RDWR, 0777);
	if(m_index_fds[ch] < 0)
		return S_FAILURE;
	
	header.m_header = IDX_FLAG;
	header.m_magic = IDX_MAGIC;

	int retval = write(m_index_fds[ch], &header, sizeof(IDX_HEADER));
	if(retval != sizeof(IDX_HEADER))
	{
		close(m_index_fds[ch]);
		m_index_fds[ch] = -1;
		unlink(m_IdxFileName[ch]);	
		return S_FAILURE;
	}

	//��λ��β��,���ﲻ��ʹ��O_APPEND���������޸�ʱ��ʱ����������
	lseek(m_index_fds[ch],0,SEEK_END);
	
	return S_SUCCESS;
}

void RecordManage::FillListFileHeader(LISTFILE_FILEHEADER *pstListFileHeader,SystemDateTime  *pStart,SystemDateTime  *pEnd,
									unsigned short u16TotalFiles,
									unsigned short u16AlarmFileCnt,unsigned short u16MdFileCnt,
									unsigned short u16TimeFileCnt,unsigned short u16MaunalFileCnt)
{
	pstListFileHeader->m_signature = FILELIST_HEADER_SIGNATURE;
	//pstListFileHeader->m_verDev
	pstListFileHeader->m_verFile = 8;
	if(pStart != NULL)
	{
		//pstListFileHeader->m_begYear = pStart->year;
		//pstListFileHeader->m_begMonth = pStart->month;
		//pstListFileHeader->m_begDay = pStart->mday;
		pstListFileHeader->m_begHour = pStart->hour;
		pstListFileHeader->m_begMinute = pStart->minute;
		pstListFileHeader->m_begSecond = pStart->second;
	}
	if(pEnd != NULL)
	{
		//pstListFileHeader->m_endYear = pEnd->year;
		//pstListFileHeader->m_endMonth = pEnd->month;
		//pstListFileHeader->m_endDay = pEnd->mday;
		pstListFileHeader->m_endHour = pEnd->hour;
		pstListFileHeader->m_endMinute = pEnd->minute;
		pstListFileHeader->m_endSecond = pEnd->second;
	}
	//pstListFileHeader->m_pad1 = ;
	pstListFileHeader->m_total_files = u16TotalFiles;
	//pstListFileHeader->m_cur_file = u16CurFile;
	//pstListFileHeader->m_usdevType = ;
	//pstListFileHeader->m_szdevType = ;
	pstListFileHeader->m_alarmfilecnt = u16AlarmFileCnt;
	pstListFileHeader->m_mdfilecnt = u16MdFileCnt;	
	pstListFileHeader->m_timerfilecnt = u16TimeFileCnt;
	pstListFileHeader->m_maunalfilecnt = u16MaunalFileCnt;
	//pstListFileHeader->m_downloaded = ;
	//pstListFileHeader->m_reserved = ;	
}

int RecordManage::CreateListFile(char *pstrListFile)
{
	LISTFILE_FILEHEADER stListFileHeader;
	
	memset(&stListFileHeader,0,sizeof(stListFileHeader));
	FillListFileHeader(&stListFileHeader, &m_datetime, &m_datetime, 
					0,
					0,0,
					0,0);

	int fd = open(pstrListFile, O_CREAT|O_RDWR, 0777);
	if(fd < 0)
		return S_FAILURE;

	plog("%s(%d)->open %s ok\n",__FILE__,__LINE__,pstrListFile);
	
	int s32Ret = write(fd,&stListFileHeader,sizeof(stListFileHeader));
	if(s32Ret != sizeof(stListFileHeader))
		return S_FAILURE;

	close(fd);
	return S_SUCCESS;
}


void RecordManage::FillListFileName(LISTFILE_NAME *pstListFileName,char *pstrFileName,ITEM_STATE enState,
									ITEM_FILE_STAT enFileStat,int s32StartHms,unsigned char u8RecordType,int channel,int s32EndHms)
{
	pstListFileName->m_signature = FILELIST_ITEM_SIGNATURE;
	strcpy(pstListFileName->m_name,pstrFileName);
	pstListFileName->m_state = (unsigned short)enState;
	//pstListFileName->M_filestat = (unsigned short)enFileStat;
	pstListFileName->stEvenItem.start_time = s32StartHms;
	pstListFileName->stEvenItem.m_RecType = u8RecordType;
	pstListFileName->stEvenItem.m_RecLevel = m_uRecordLevel;
	pstListFileName->stEvenItem.m_RecCh = channel;
	//pstListFileName->m_pad2 = ;
	pstListFileName->stEvenItem.end_time = s32EndHms;
	//pstListFileName->m_drivername = ;
	//pstListFileName->m_vehiclenum = ;
	//pstListFileName->m_download = ;	
	//pstListFileName->m_pad3 = ;	
}

#if 0
int RecordManage::AddRecordFile2List(int ch)
{
	char listfile[MAX_RECFILENAMELEN]={0};
	memcpy(listfile,m_264FileName[ch],strlen(m_strRecDir));
	strcat(listfile, "/");
	strcat(listfile, RECORD_LIST_FILENAME);

	//����б��ļ������ڣ��򴴽����ļ�
	if(access(listfile,R_OK|W_OK) != 0)
	{
		if(CreateListFile(listfile) != S_SUCCESS)
		{
			printf("err,CreateListFile, %s\n",listfile);
			return S_FAILURE;
		}
	}

	LISTFILE_NAME stListFileName;
	memset(&stListFileName,0,sizeof(stListFileName));
	FillListFileName(&stListFileName,m_264FileName[ch],ITEM_STATE_OK,
		                  ITEM_FILE_STAT_CREAT,m_RecStartTime[ch],m_uRecordType[ch],ch+1,m_RecStartTime[ch]);
	
	int fd = open(listfile, O_RDWR, 0777);
	if(fd < 0)
	{
		printf("err,open %s\n",listfile);
		return S_FAILURE;
	}

	//��ȡͷ��
	LISTFILE_FILEHEADER stListFileHeader;
	int s32Ret = read(fd,&stListFileHeader,sizeof(stListFileHeader));
	if(s32Ret != sizeof(stListFileHeader))
	{
		printf("err,read %s\n",listfile);
		close(fd);
		return S_FAILURE;
	}
	
	//����ͷ��	
	stListFileHeader.m_total_files += 1;
	FillListFileHeader(&stListFileHeader, NULL, NULL, 
						stListFileHeader.m_total_files,
						stListFileHeader.m_alarmfilecnt,stListFileHeader.m_mdfilecnt,
						stListFileHeader.m_timerfilecnt,stListFileHeader.m_maunalfilecnt);
	//д��ͷ��
	lseek(fd, 0, SEEK_SET);
	s32Ret = write(fd,&stListFileHeader,sizeof(stListFileHeader));
	if(s32Ret != sizeof(stListFileHeader))
	{
		close(fd);
		printf("err,write %s\n",listfile);
		return S_FAILURE;
	}	
	
	//д���¼
	lseek(fd, 0, SEEK_END);
	s32Ret = write(fd,&stListFileName,sizeof(stListFileName));
	if(s32Ret != sizeof(stListFileName))
	{
		close(fd);
		printf("err,write %s\n",listfile);
		return S_FAILURE;
	}	

	close(fd);
	return S_SUCCESS;
}
#endif

void RecordManage::Fill264Header(int ch,SystemDateTime  *pStart,SystemDateTime  *pEnd,
	unsigned int s32IndexTblOffs,unsigned int s32MovieOffset,int frameRate,int resolution,int bNonStop)
{
	m_FileInfo[ch].m_Hdr.m_magic = RECFILEHEADERMAGIC;
	m_FileInfo[ch].m_Hdr.m_size = 0;
	//m_FileInfo[ch].m_VerDev;//�豸����
	//m_FileInfo[ch].m_VerFile;	// �ļ��汾

	if(pStart != NULL)
	{
		m_FileInfo[ch].m_BegYear = pStart->year; 
		m_FileInfo[ch].m_BegMonth = pStart->month;
		m_FileInfo[ch].m_BegDay = pStart->mday;
		m_FileInfo[ch].m_BegHour = pStart->hour;
		m_FileInfo[ch].m_BegMinute = pStart->minute;
		m_FileInfo[ch].m_BegSecond = pStart->second;  // ��ʼ��������ʱ����
	}
	if(pEnd != NULL)
	{	
		m_FileInfo[ch].m_EndYear = pEnd->year;
		m_FileInfo[ch].m_EndMonth = pEnd->month;
		m_FileInfo[ch].m_EndDay = pEnd->mday;
		m_FileInfo[ch].m_EndHour = pEnd->hour;
		m_FileInfo[ch].m_EndMinute = pEnd->minute;
		m_FileInfo[ch].m_EndSecond = pEnd->second;// ������������ʱ����
	}
	//m_ChlCount;  // tͨ����Ŀ
	//m_FileInfo[ch].m_RecType;	// ¼������
	//m_FileInfo[ch].m_FileProtected;//¼���Ƿ񱣻�
	m_FileInfo[ch].m_RecLevel = m_uRecordLevel;//¼��ȼ�
	
	m_FileInfo[ch].m_VideoType = 0; // ��Ƶ��ʽ
	m_FileInfo[ch].m_bNonStop = bNonStop;
	m_FileInfo[ch].m_bEncrypt = 1;
	//m_FileInfo[ch].m_DevNo; // �豸���
	m_FileInfo[ch].m_Audio.m_bitswidth = 16;  // ��Ƶ�������
	//m_FileInfo[ch].m_Audio.m_bitRate;  // ��Ƶ�������	
	m_FileInfo[ch].m_Audio.m_sampleRate = 8000;  // ��Ƶ�������	

	//ע�⣬���������Ҫ�����ļ�����ʱ���ſ�����д
	m_FileInfo[ch].m_IndexTblOffset = s32IndexTblOffs;// �����������ļ�ƫ��	
	//ע�⣬���������Ҫ�����ļ�����ʱ���ſ�����д
	m_FileInfo[ch].m_MovieOffset = s32MovieOffset; //��ʵ��¼������λ�����Ϊ0XFFFF Ϊ��Чƫ��(����ֵû�м�0x200, ��ʵIframe��λ����m_MovieOffset+0x200)
	
	//m_FileInfo[ch].m_nDevType; //�豸�������ֱ��
	//m_FileInfo[ch].m_Reserved3;
	//m_FileInfo[ch].m_DateShowFmt;// ������ʾ�ĸ�ʽ��ʱ����
	//m_FileInfo[ch].m_cDevType[12];// �豸�����ַ�������
	
	m_FileInfo[ch].m_ChInfo.chlIndex = ch; //ͨ������Ƶ�������
	m_FileInfo[ch].m_ChInfo.frameRate = frameRate; //ͨ������Ƶ�������	
	m_FileInfo[ch].m_ChInfo.resolution = resolution; //ͨ������Ƶ�������
	sprintf(m_FileInfo[ch].m_ChInfo.chlName, "CH%02d",ch+1);//ͨ������Ƶ�������	
}

void RecordManage::PrepareRecordInfor(int ch)
{
	m_RecStartTime[ch] = GetCurrentTimeValue();	//ע�⣬����δ���Ǿ�ǰԤ¼
	memset(&m_stEndTime[ch],0,sizeof(m_stEndTime[ch]));
	
	sprintf(m_264FileName[ch], "%s/recfile_-%02d%02d%02d-%02d%02d%02d-%02d%02d%02d-00000%d00.264", 
						m_strRecDir,
						m_datetime.year,m_datetime.month,m_datetime.mday,
						m_datetime.hour,0,0,/*���㿪ʼ*/
						m_datetime.hour,59,59,/*59��59�����*/
						ch+1);/*������Ч��Ϊ0*/
	//��ӡ¼�񼶱𣬵�����
	plog("%s(%d)->m_uRecordLevel=%d\n",__FILE__,__LINE__,m_uRecordLevel);
	
	strcpy(m_IdxFileName[ch],m_264FileName[ch]);
	//68��"/hdd00/p01/record/2015-12-05/recfile_-151205-161300-162903-41U10300.264"����׺264���±�
	memcpy(m_IdxFileName[ch]+68,"idx",strlen("idx"));

	//264 �ļ�ͷ
	//���264�ļ�����
	memset(&m_FileInfo[ch],0,sizeof(m_FileInfo[ch]));
	if(access(m_264FileName[ch],R_OK|W_OK) == 0)//����
	{
		FILE	*pfp;
		pfp = fopen(m_264FileName[ch], "rb");
		if(pfp == NULL)
		{
			plog("err,fopen %s,%s :%d\n", m_264FileName[ch],strerror(errno), errno);
			return;
		}

		int retval = fread(&m_FileInfo[ch], sizeof(unsigned char), sizeof(m_FileInfo[ch]), pfp);		
		if(retval !=  sizeof(m_FileInfo[ch]))
		{
			fclose(pfp);
			plog("err,fread %s,%s :%d\n", m_264FileName[ch],strerror(errno), errno);
			return;		
		}

		m_FileInfo[ch].m_bNonStop = 1;
		fclose(pfp);
	}
	else//���246�ļ�������
	{
		Fill264Header(ch,&m_datetime,&m_datetime,0,0xFFFF,30,4,1);		
	}
}

int RecordManage::OpenRecordFile(int ch)
{
	int retval = CreateRecordDirectory();
	if(retval == S_FAILURE)
	{
		plog("err,CreateRecordDirectory,%s :%d\n", strerror(errno), errno);
		return retval;
	}

	PrepareRecordInfor(ch);
	
	retval = Create264File(ch);	
	if(retval == S_FAILURE)
	{
		plog("err,Create264File,%s :%d\n", strerror(errno), errno);
		return retval;
	}

	retval = CreateIdxFile(ch);	
	if(retval == S_FAILURE)
	{
		plog("err,CreateIdxFile,%s :%d\n", strerror(errno), errno);
		return retval;
	}		

	//�ϳ�list�ļ���
	char listfile[MAX_RECFILENAMELEN]={0}; 	
	snprintf(listfile,sizeof(listfile),"%s/%s",m_strRecDir,RECORD_LIST_FILENAME);	

	//����б��ļ������ڣ��򴴽����ļ�	
	if(access(listfile,R_OK|W_OK) != 0)
	{
		if(CreateListFile(listfile) != S_SUCCESS)
		{
			plog("err,CreateListFile, %s\n",listfile);
			return S_FAILURE;
		}
	}
#if 0	
	retval = AddRecordFile2List(ch);
	if(retval == S_FAILURE)
	{
		printf("err,AddRecordFile2List,%s :%d\n", strerror(errno), errno);
		return retval;
	}		
#endif

	//���¶�λ��I֡
	//ע�⣬����û�д���ǰ¼��
	//ResetUserData2IFrameBySecond(ch, 0, LOCAL_REC_ID,30); //STREAM_TYPE_HIGHT=0
	ResetUserData2IFrame(ch,1,0);
	return S_SUCCESS;
	
}

/*
����д8֡���������8֡������-1
���򷵻�0
*/
int RecordManage::WriteRecordFile(int ch)
{
	unsigned char *buffer = NULL;
	FrameInfo m_frameinfo;
	memset(&m_frameinfo, 0, sizeof(FrameInfo));
	unsigned int len;
	static RECORD_LEVEL enRecordLevel[MAX_VIDEO_CHANNEL];
	
	for(int i = 0; i < 8; i++)
	{
		//ȡ֡
		#if 1
		if(GetOneFrame4Record(ch,0,LOCAL_REC_ID,&buffer,&m_frameinfo) < 0)//STREAM_TYPE_HIGHT = 0
		{
			return S_FAILURE;
		}
		#endif

		if((enRecordLevel[ch] == RECORD_LEVEL1 || enRecordLevel[ch] == RECORD_LEVEL2)
			&& m_frameinfo.Flag == P_FRAME)
			continue;
		
		//дI֡����
		if(m_frameinfo.Flag == I_FRAME)
		{	
			IFRAMELISTCONTENT  content;
			content.m_iframetime = m_frameinfo.hour*3600+m_frameinfo.min*60+m_frameinfo.sec;
			content.m_offset = m_offset[ch];
			content.m_sampleRate = 8000;//��������д�̶�ֵ����õ������ǣ����ⲿ�ṩ�������
			content.m_bitswidth = 16;
			content.m_RecType = m_uRecordType[ch];
			content.m_VideoType = 1;//
			content.m_FrameRate = 25;//
			content.m_Resolution = RES_VGA;
			content.m_RecLevel = m_uRecordLevel;			
			content.m_res[0] = 0;
			content.m_res[1] = 0;
			content.m_res[2] = 0;
			
			//ZD_DEBUG_Printf("%s(%d)->%d\n",__FILE__,__LINE__,content.m_offset);
			
			if(write(m_index_fds[ch], &content, sizeof(IFRAMELISTCONTENT)) != sizeof(IFRAMELISTCONTENT))
			{
				m_uHddEio[ch] = 1;
				plog("err,WriteIdx    errreason [%s]\n",strerror(errno));
				return S_FAILURE;				
			}

			enRecordLevel[ch] = m_uRecordLevel;
		}
		
		//д֡
		len = (m_frameinfo.FrmLength+7)&(~7);
		if(fwrite(buffer, sizeof(unsigned char), len, m_pRecFp[ch]) != len)
		{
			m_uHddEio[ch] = 1;
			plog("err,WriteFrame    errreason [%s]\n",strerror(errno));
			return S_FAILURE;
		}
		fsync((int)m_pRecFp[ch]);
		//����ƫ��
		m_offset[ch] += len;
	}
	return S_SUCCESS;	
}

#if 0
int RecordManage::ModifyItemInList(int ch,SystemDateTime *pstEnd,char *pstrNewName)
{
	char listfile[MAX_RECFILENAMELEN]={0};
	memcpy(listfile,m_264FileName[ch],strlen(m_strRecDir));
	strcat(listfile, "/");
	strcat(listfile, RECORD_LIST_FILENAME);

	//��
	int fd = open(listfile,O_RDWR, 0777);
	if(fd < 0)
	{
		printf("err,open %s\n",listfile);
		return S_FAILURE;
	}
	
	//��ȡͷ��
	LISTFILE_FILEHEADER stListFileHeader;
	int s32Ret = read(fd,&stListFileHeader,sizeof(stListFileHeader));
	if(s32Ret != sizeof(stListFileHeader))
	{
		close(fd);
		printf("err,read %s\n",listfile);
		return S_FAILURE;
	}
	
	//����ͷ��	
	//д��ͷ��
	if(pstEnd)
	{
		stListFileHeader.m_total_files++;
		stListFileHeader.m_timerfilecnt += (m_uRecordType[ch] == TIMER_RECORD);
		stListFileHeader.m_mdfilecnt    += (m_uRecordType[ch] == MD_RECORD);
		stListFileHeader.m_alarmfilecnt+= (m_uRecordType[ch] == ALARM_RECORD);
		
		FillListFileHeader(&stListFileHeader, NULL, pstEnd,
						stListFileHeader.m_total_files,
						stListFileHeader.m_alarmfilecnt,stListFileHeader.m_mdfilecnt,
						stListFileHeader.m_alarmfilecnt,stListFileHeader.m_maunalfilecnt);					
	}
	else
	{		
		FillListFileHeader(&stListFileHeader, NULL, NULL, 
						stListFileHeader.m_total_files,
						stListFileHeader.m_alarmfilecnt,stListFileHeader.m_mdfilecnt,
						stListFileHeader.m_alarmfilecnt,stListFileHeader.m_maunalfilecnt);			
	}
	lseek(fd,0,SEEK_SET);
	if(write(fd,&stListFileHeader,sizeof(stListFileHeader)) != sizeof(stListFileHeader))
	{
		printf("err,write %s\n",listfile);
		close(fd);
		return S_FAILURE;		
	}

	//��ȡ��¼
	LISTFILE_NAME stListFileName;
	while(1)
	{
		s32Ret = read(fd,&stListFileName,sizeof(stListFileName));
		if(s32Ret == 0)//�ļ�����
		{
			close(fd);
			printf("err,not found %s\n",m_264FileName[ch]);
			return S_FAILURE;			
		}

		if(s32Ret != sizeof(stListFileName))//�ļ�����
		{
			close(fd);
			printf("err,read %s\n",listfile);
			return S_FAILURE;			
		}

		if(strcmp(stListFileName.m_name,m_264FileName[ch]) == 0)
		{
			break;		
		}
	}

	//�޸�
	stListFileName.M_filestat = ITEM_FILE_STAT_CLOSE;
	if(pstEnd)
	{
		stListFileName.m_state = ITEM_STATE_OK;		
		FillListFileName(&stListFileName,pstrNewName,(ITEM_STATE)stListFileName.m_state,
			(ITEM_FILE_STAT)stListFileName.M_filestat,stListFileName.m_start_hms,m_uRecordType[ch],ch+1,m_RecEndTime[ch]);		
	}
	else
	{
		stListFileName.m_state = ITEM_STATE_DELETED;	
		FillListFileName(&stListFileName,stListFileName.m_name,(ITEM_STATE)stListFileName.m_state,
			(ITEM_FILE_STAT)stListFileName.M_filestat,stListFileName.m_start_hms,stListFileName.m_record_type,ch+1,stListFileName.m_end_time);			
	}			

	//����
	lseek(fd,-sizeof(stListFileName),SEEK_CUR);
	s32Ret = write(fd,&stListFileName,sizeof(stListFileName));
	if(s32Ret != sizeof(stListFileName))
	{
		close(fd);		
		printf("err,write %s\n",listfile);
		return S_FAILURE;
	}	

	close(fd);
	return S_SUCCESS;
}
#endif

void RecordManage::ClearRecordInfor(int ch)
{
	m_pRecFp[ch] = NULL;
	m_index_fds[ch] = -1;	
	memset(m_264FileName[ch],0,MAX_RECFILENAMELEN);
	memset(m_IdxFileName[ch],0,MAX_RECFILENAMELEN);
}

int RecordManage::Rewrite264Header(int ch)
{
	//��
	int fd = open(m_264FileName[ch],O_RDWR, 0777);
	if(fd < 0)
	{
		plog("err,open %s\n",m_264FileName[ch]);
		return S_FAILURE;
	}

	//д��
	if(write(fd,&m_FileInfo[ch],sizeof(m_FileInfo[ch])) != sizeof(m_FileInfo[ch]))
	{
		plog("err,write %s\n",m_264FileName[ch]);
		close(fd);
		return S_FAILURE;
	}		

	//�ر�
	fsync(fd);
	close(fd);
	return S_SUCCESS;
}

int RecordManage::AppendIdx(int ch)
{
	int fd264 = open(m_264FileName[ch],O_RDWR, 0777);
	int fdIdx = open(m_IdxFileName[ch],O_RDWR, 0777);
	if(fd264 < 0 || fdIdx < 0)
	{
		plog("err,AppendIdx,open\n");
		return S_FAILURE;
	}

	lseek(fd264,0,SEEK_END);
	lseek(fdIdx,sizeof(VIDEO_STHDR),SEEK_SET);	

	IFRAMELISTCONTENT content;
	int s32Ret;
	while(1)
	{
		s32Ret = read(fdIdx,&content,sizeof(content));
		if(s32Ret == 0)//�ļ�����
		{
			break;
		}

		if(write(fd264,&content,sizeof(content)) != sizeof(content))
		{
			plog("err,AppendIdx,write\n");
			close(fdIdx);
			close(fd264);
			return S_FAILURE;				
		}
	}

	close(fdIdx);
	close(fd264);
	return S_SUCCESS;		
}

#if 0
int RecordManage::Rename264AndFixItem(int ch,SystemDateTime *pstEnd)
{
	char strNewName[MAX_RECFILENAMELEN];
	strcpy(strNewName,m_264FileName[ch]);

	char strEndTime[MAX_RECFILENAMELEN];
	sprintf(strEndTime, "%02d%02d%02d", pstEnd->hour,pstEnd->minute,pstEnd->second);

	//52��"/hdd00/p01/record/2015-12-05/recfile_-151205-161300-162903-41U10300.264",����ʱ����±�
	memcpy(strNewName+52,strEndTime,strlen(strEndTime));

	rename(m_264FileName[ch],strNewName);

	return ModifyItemInList(ch,&m_stEndTime[ch],strNewName);
}
#endif

int RecordManage::RederectFilePosition(int ch,int curtime)
{
	int len = -1;
	IFRAMELISTCONTENT stIframeListContentCur;
	int s32H264Offset = 512; //��¼�������i֡ƫ��
	int s32IdxOffset = sizeof(IDX_HEADER);
	int s32IframeNum = 0;
	int offset; 
	
	if(m_pRecFp[ch] == NULL)
	{
		plog("not rec no stop\n");		
		return S_FAILURE;
	}

	//��λ��ԭʼλ��
	offset = fseek(m_pRecFp[ch], s32H264Offset, SEEK_SET);
	if(offset != s32H264Offset)
	{
		ZD_DEBUG_Printf("%s(%d)->\n",__FILE__,__LINE__);
	}
	ZD_DEBUG_Printf("%s(%d)->offset=%d\n",__FILE__,__LINE__,offset);
	
	offset = lseek(m_index_fds[ch], s32IdxOffset, SEEK_SET);	
	if(offset != s32H264Offset)
	{
		ZD_DEBUG_Printf("%s(%d)->\n",__FILE__,__LINE__);
	}
	ZD_DEBUG_Printf("%s(%d)->offset=%d\n",__FILE__,__LINE__,offset);
	
	plog("curtime=%d:%d:%d\n",curtime/3600,(curtime%3600)/60,curtime%3600%60);

	while(1)
	{
		len = read(m_index_fds[ch], &stIframeListContentCur, sizeof(stIframeListContentCur));
		if(len < 0)
		{
			plog("read idx err,len = %d\n", len);
			perror("read() error");
			break;
		}
		else if(len == 0)
		{
			plog("%s(%d)->read end,s32IframeNum=%d\n",__FILE__,__LINE__,s32IframeNum);	
			break;
		}
		
		s32IframeNum++;		

		//��m_preTime֮ǰ�����ݣ������õģ���Ҫ����
		if((int)stIframeListContentCur.m_iframetime > 0
			&& (int)stIframeListContentCur.m_iframetime - curtime < 0)
		{
			s32H264Offset = stIframeListContentCur.m_offset;
			s32IdxOffset += sizeof(IFRAMELISTCONTENT);
			plog("%s(%d)->m_iframetime=%ld:%ld:%ld\n",__FILE__,__LINE__,stIframeListContentCur.m_iframetime/3600,(stIframeListContentCur.m_iframetime%3600)/60,stIframeListContentCur.m_iframetime%3600%60);
			continue;
		}
		else
		{
			plog("%s(%d)->m_iframetime=%ld:%ld:%ld\n",__FILE__,__LINE__,stIframeListContentCur.m_iframetime/3600,(stIframeListContentCur.m_iframetime%3600)/60,stIframeListContentCur.m_iframetime%3600%60);
			
			lseek(m_index_fds[ch], -sizeof(IFRAMELISTCONTENT), SEEK_CUR);
			
			memset(&stIframeListContentCur,0,sizeof(stIframeListContentCur));
			len = write(m_index_fds[ch], &stIframeListContentCur, sizeof(stIframeListContentCur));
			if(len < 0)
			{
				plog("write index data error,len = %d\n", len);
				break;
			}	
		}
	}

	//��λ264��дָ��
	if(s32H264Offset < 512)
	{
		s32H264Offset = 512;
	}
	fseek(m_pRecFp[ch], s32H264Offset, SEEK_SET);
	plog("%s(%d)->fseek,s32H264Offset=%d\n",__FILE__,__LINE__,s32H264Offset);
	
	//��λidx��дָ��
	if((unsigned int)s32IdxOffset >= sizeof(IDX_HEADER) + sizeof(IFRAMELISTCONTENT))
	{
		s32IdxOffset -= sizeof(IFRAMELISTCONTENT);
	}
	lseek(m_index_fds[ch], s32IdxOffset, SEEK_SET);			
	plog("%s(%d)->fseek,s32IdxOffset=%d\n",__FILE__,__LINE__,s32IdxOffset);

	//��list2�ļ��У�ɾ��curtime֮���¼��
	char listFilePath[128] = {'\0'};
	LISTFILE_NAME  reclistfile;
	memcpy(listFilePath,m_264FileName,28);// /hdd00/p01/record/2015-01-16
	strcat(listFilePath,"/");
	strcat(listFilePath,RECORD_LIST_FILENAME);
	int fd = open(listFilePath,O_RDWR);
	if(fd < 0)
	{
		ZD_DEBUG_Printf("open err, %s",listFilePath);
		return S_FAILURE;
	}
	lseek(fd, sizeof(LISTFILE_FILEHEADER), SEEK_SET);	
	while(1)
	{
		len = read(fd, &reclistfile, sizeof(reclistfile));
		if(len < 0)
		{
			ZD_DEBUG_Printf("read list err,len = %d\n", len);
			perror("read() error");
			break;
		}
		else if(len == 0)
		{
			plog("%s(%d)->read end\n",__FILE__,__LINE__);	
			break;
		}		

		if((int)reclistfile.stEvenItem.m_RecCh != ch+1)//ͨ����һ��
		{
			continue;
		}

		if((int)reclistfile.stEvenItem.end_time/3600 != curtime/3600)//����ͬһ��Сʱ
		{
			plog("%s(%d)->end_time=%d:%d:%d\n",__FILE__,__LINE__,reclistfile.stEvenItem.end_time/3600,(reclistfile.stEvenItem.end_time%3600)/60,reclistfile.stEvenItem.end_time%3600%60);		
			continue;
		}
		
		if(reclistfile.stEvenItem.end_time <= (unsigned int)curtime)//����ʱ�䣬�ڵ�ǰʱ��֮ǰ����Ҫ����
		{
			plog("%s(%d)->end_time=%d:%d:%d\n",__FILE__,__LINE__,reclistfile.stEvenItem.end_time/3600,(reclistfile.stEvenItem.end_time%3600)/60,reclistfile.stEvenItem.end_time%3600%60);
			continue;
		}

		plog("%s(%d)->start_time=%d:%d:%d\n",__FILE__,__LINE__,reclistfile.stEvenItem.start_time/3600,(reclistfile.stEvenItem.start_time%3600)/60,reclistfile.stEvenItem.start_time%3600%60);
		plog("%s(%d)->end_time=%d:%d:%d\n",__FILE__,__LINE__,reclistfile.stEvenItem.end_time/3600,(reclistfile.stEvenItem.end_time%3600)/60,reclistfile.stEvenItem.end_time%3600%60);
		
		reclistfile.m_state = ITEM_STATE_DELETED;
		int pos = lseek( fd, -sizeof(reclistfile), SEEK_CUR );
		if( -1 == pos )
		{
			ZD_DEBUG_Printf("%s %d lseek error !\n",__FILE__,__LINE__);
			continue;
		}

		/*д��*/
		len = write(fd, &reclistfile, sizeof(reclistfile));
		if( len != sizeof(reclistfile) )
		{
			ZD_DEBUG_Printf("%s %d write error !\n",__FILE__,__LINE__);
			continue;
		}		
	}
	close(fd);
	
	return S_SUCCESS;
	
}

int RecordManage::FillEvenItem(int ch,EVEN_ITEM *pstEvenItem,IFRAMELISTCONTENT *pstIframeListContent,int start_time,int end_time,int s32StartOffset,int s32EndOffset,RECORD_LEVEL enRecordLevel)
{
	if(pstIframeListContent != NULL)
	{
		pstEvenItem->m_RecCh = ch + 1;//
		pstEvenItem->m_RecType = pstIframeListContent->m_RecType;
		pstEvenItem->m_VideoType = pstIframeListContent->m_VideoType;
		pstEvenItem->m_FrameRate = pstIframeListContent->m_FrameRate;
		pstEvenItem->m_Resolution = pstIframeListContent->m_Resolution;
		pstEvenItem->m_sampleRate = pstIframeListContent->m_sampleRate;
		pstEvenItem->m_bitswidth = pstIframeListContent->m_bitswidth;
	
	}

	pstEvenItem->start_time = start_time;
	pstEvenItem->end_time = end_time;
	pstEvenItem->m_StartOffset = s32StartOffset;
	pstEvenItem->m_EndOffset = s32EndOffset;

	pstEvenItem->m_RecLevel = enRecordLevel;
		
	return S_SUCCESS;
}

/*
�����¼��Ŀ�ʼʱ��
*/
int RecordManage::FixEvenItenStartTime(EVEN_ITEM *pstEvenItem)
{
	int start_hour,start_minite,start_second;
	
	start_hour = pstEvenItem->start_time / 3600;
	start_minite = (pstEvenItem->start_time % 3600)/60;
	start_second = pstEvenItem->start_time % 3600%60;	

	if(start_minite == 0 && start_second < 8)
	{
		pstEvenItem->start_time = start_hour*3600+start_minite*60+0;
	}
	if(start_minite == 59 && start_second > 52)
	{
		if(start_hour != 23)
		{
			pstEvenItem->start_time = (start_hour+1)*3600+0*60+0;
		}
		else
		{
			pstEvenItem->start_time = (0)*3600+0*60+0;
		}
	}	
	
	return S_SUCCESS;
}

/*
�����¼��Ľ���ʱ��
*/
int RecordManage::FixEvenItenEndTime(EVEN_ITEM *pstEvenItem)
{
	int end_hour,end_minite,end_second;
	
	end_hour = pstEvenItem->end_time / 3600;
	end_minite = (pstEvenItem->end_time % 3600)/60;
	end_second = pstEvenItem->end_time % 3600%60;

	if(end_minite == 59 && end_second > 40)
	{
		pstEvenItem->end_time = end_hour*3600+end_minite*60+59;
	}	
	
	if(end_minite == 0 && end_second < 20)
	{
		if(end_hour != 0)
		{
			pstEvenItem->end_time = (end_hour-1)*3600+59*60+59;
		}
		else
		{
			pstEvenItem->end_time = (23)*3600+59*60+59;
		}		
	}
	
	return S_SUCCESS;
}

int RecordManage::ParserEvenFromIframeIndex(char *pstrIdxFileName, EVEN_ITEM *pstEvenItem, int *ps32EvenNum,int ch)
{
	int len = -1;
	EVEN_ITEM stEvenItem;
	int s32IframeNum = 0;
	int s32EvenNum = 0;
	IFRAMELISTCONTENT stIframeListContentLast;
	IFRAMELISTCONTENT stIframeListContentCur;

	int index_fd = open(pstrIdxFileName,O_RDWR, 0777);
	if(index_fd < 0)
	{
		return S_FAILURE;
	}	

	//����ǰ�漸��i֡ʱ��,��һ��i֡��ʱ�䣬�����ܴ���x(x=һ���������-20)
	int i = 0;
	for(i = 0; i < 5; i++)
	{
		lseek(index_fd, sizeof(IDX_HEADER)+i*sizeof(stIframeListContentCur), SEEK_SET);
		len = read(index_fd, &stIframeListContentCur, sizeof(stIframeListContentCur));
		if(len <= 0)
		{
			close(index_fd);
			return S_FAILURE;
		}//82,800+3,540+40=86,380
		if(stIframeListContentCur.m_iframetime > 23*3600+59*60+40)
		{
			stIframeListContentCur.m_iframetime = 5*i;
			lseek(index_fd, sizeof(IDX_HEADER)+i*sizeof(stIframeListContentCur), SEEK_SET);
			len = write(index_fd, &stIframeListContentCur, sizeof(stIframeListContentCur));
			if(len <= 0)
			{
				close(index_fd);
				return S_FAILURE;
			}		
		}
	}
	
	//���������һ��i֡��ʱ�䣬������С��20
	int s32Offset = lseek(index_fd, 0, SEEK_END);
	s32Offset -= sizeof(stIframeListContentCur);
	if(s32Offset < 0)
	{
		close(index_fd);
		return S_FAILURE;
	}	
	lseek(index_fd, s32Offset, SEEK_SET);
	len = read(index_fd, &stIframeListContentCur, sizeof(stIframeListContentCur));
	if(len <= 0)
	{
		close(index_fd);
		return S_FAILURE;
	}
	if(stIframeListContentCur.m_iframetime < 20)
	{
		stIframeListContentCur.m_iframetime = 23*3600+59*60+59;
		lseek(index_fd, s32Offset, SEEK_SET);
		len = write(index_fd, &stIframeListContentCur, sizeof(stIframeListContentCur));
		if(len <= 0)
		{
			close(index_fd);
			return S_FAILURE;
		}		
	}

	//��ʼ��
	memset(&stEvenItem,0,sizeof(stEvenItem));
	lseek(index_fd, sizeof(IDX_HEADER), SEEK_SET);
	*ps32EvenNum = 0;
	memset(&stIframeListContentLast,0,sizeof(stIframeListContentCur));
	memset(&stIframeListContentCur,0,sizeof(stIframeListContentCur));
	stIframeListContentLast.m_iframetime = 0;
	stIframeListContentLast.m_RecType = -1;	
	int bFrirst = 1;	
	
	while(1)
	{
		len = read(index_fd, &stIframeListContentCur, sizeof(stIframeListContentCur));

		if(len <= 0)
		{
			//printf("move index data to end %d \n", len);
			break;
		}

		//ZD_DEBUG_Printf("stIframeListContentCur.m_RecType=%d\n",stIframeListContentCur.m_RecType);
		s32IframeNum++;

		if(stIframeListContentLast.m_RecType != stIframeListContentCur.m_RecType
			|| stIframeListContentCur.m_iframetime - stIframeListContentLast.m_iframetime >= 20)
		{
			//ZD_DEBUG_Printf("start even,ch = %d,m_RecType=%d,m_iframetime = %ld\n",ch,stIframeListContentCur.m_RecType,stIframeListContentCur.m_iframetime);

			if(bFrirst == 1)//��һ��I֡��ֻ��¼���¼��Ŀ�ʼ(û�н������¼�������)
			{
				FillEvenItem(ch,&stEvenItem,&stIframeListContentCur,stIframeListContentCur.m_iframetime,\
							stIframeListContentCur.m_iframetime,stIframeListContentCur.m_offset,stIframeListContentCur.m_offset,(RECORD_LEVEL)stIframeListContentCur.m_RecLevel);
				
				bFrirst = 0;
				stIframeListContentLast = stIframeListContentCur;//������һ��ѭ��֮ǰ���ȸ���stIframeListContentLast
				continue;
			}

			//ֻ��1��I֡��¼��Σ�����ʱ������Ծ
			if(stEvenItem.start_time == stIframeListContentLast.m_iframetime
				&& stIframeListContentCur.m_iframetime - stIframeListContentLast.m_iframetime >= 20)
			{
				FillEvenItem(ch,&stEvenItem,NULL,stEvenItem.start_time,stIframeListContentLast.m_iframetime+5,\
						stEvenItem.m_StartOffset,stIframeListContentCur.m_offset,(RECORD_LEVEL)stIframeListContentCur.m_RecLevel);							
			}
			//ֻ��1��I֡��¼��Σ�����ʱ������
			else if(stEvenItem.start_time == stIframeListContentLast.m_iframetime
				&& stIframeListContentCur.m_iframetime - stIframeListContentLast.m_iframetime < 20)
			{
				FillEvenItem(ch,&stEvenItem,NULL,stEvenItem.start_time,stIframeListContentCur.m_iframetime,\
						stEvenItem.m_StartOffset,stIframeListContentCur.m_offset,(RECORD_LEVEL)stIframeListContentCur.m_RecLevel);							
			}			
			//ʱ������Ծ������ʱ�䣬Ӧ��Ϊ��һ��I֡��ʱ�䣬����5�룬�����ڻط�ʱ���طź����p֡
			else if(stIframeListContentCur.m_iframetime - stIframeListContentLast.m_iframetime >= 20)
			{
				FillEvenItem(ch,&stEvenItem,NULL,stEvenItem.start_time,stIframeListContentLast.m_iframetime+5,\
						stEvenItem.m_StartOffset,stIframeListContentLast.m_offset,(RECORD_LEVEL)stEvenItem.m_RecLevel);
			}
			//ʱ��û����Ծ������ʱ��Ϊ��ǰ֡��ʱ��
			else
			{
				FillEvenItem(ch,&stEvenItem,NULL,stEvenItem.start_time,stIframeListContentCur.m_iframetime,\
						stEvenItem.m_StartOffset,stIframeListContentCur.m_offset,(RECORD_LEVEL)stIframeListContentCur.m_RecLevel);				
			}

			//������һ���¼�����¼���¼�
			pstEvenItem[s32EvenNum] = stEvenItem;
			if(s32EvenNum == 0)
			{
				FixEvenItenStartTime(&pstEvenItem[s32EvenNum]);
			}
			s32EvenNum++;

			/*��һ���¼��Ŀ�ʼʱ��Ϳ�ʼƫ�ƣ��Ѿ�����ȷ��*/
			FillEvenItem(ch,&stEvenItem,&stIframeListContentCur,stIframeListContentCur.m_iframetime,\
						stIframeListContentCur.m_iframetime,stIframeListContentCur.m_offset,stIframeListContentCur.m_offset,(RECORD_LEVEL)stIframeListContentCur.m_RecLevel);		
		}

		stIframeListContentLast = stIframeListContentCur;//������һ��ѭ��֮ǰ���ȸ���stIframeListContentLast
	}	

	//�����֡���ҿ�ʼʱ�䣬�����ڵ�ǰI֡��ʱ�䣬�������һ���¼��Ľ���ʱ��
	if(s32IframeNum > 0 && stEvenItem.start_time != stIframeListContentCur.m_iframetime)
	{
		FillEvenItem(ch,&stEvenItem,NULL,stEvenItem.start_time,stIframeListContentCur.m_iframetime,\
					stEvenItem.m_StartOffset,stIframeListContentCur.m_offset,(RECORD_LEVEL)stIframeListContentCur.m_RecLevel);
		
		pstEvenItem[s32EvenNum] = stEvenItem;
		FixEvenItenEndTime(&pstEvenItem[s32EvenNum]);
		s32EvenNum++;
			
		ZD_DEBUG_Printf("ch=%d,s32EvenNum = %d\n",ch,(*ps32EvenNum));
	}
	
	*ps32EvenNum = s32EvenNum;

	IDX_HEADER stIdxHeader;
	stIdxHeader.m_magic = IDX_MAGIC;
	stIdxHeader.m_header = IDX_FLAG;
	stIdxHeader.m_block_size = s32IframeNum*sizeof(IFRAMELISTCONTENT);
	lseek(index_fd, 0, SEEK_SET);
	len = write(index_fd,&stIdxHeader,sizeof(stIdxHeader));
	if(len <= 0)
	{
		plog("write iframe stIdxHeader to file fail, len = %d \n", len);
	}		

	ZD_DEBUG_Printf("ch=%d,s32EvenNum = %d\n",ch,(*ps32EvenNum));
	
	close(index_fd);
	return S_SUCCESS;	
}

//pstrDirName����ʽ/hdd00/p01/record/2016-01-23
int  RecordManage::AddOneEven2List(char *pstrDirName,EVEN_ITEM *pstEvenItem)
{
	FILE *fp = NULL;    
	LISTFILE_NAME  reclistfile;
	LISTFILE_NAME  reclistfileDel;
	LISTFILE_FILEHEADER listfileheader;
	char listfile[MAX_RECFILENAMELEN]={0};
 
	//׼��reclistfile
	memset(&reclistfile, 0, sizeof(LISTFILE_NAME));	
	reclistfile.m_signature = FILELIST_ITEM_SIGNATURE;
	reclistfile.m_state = 0; 
	reclistfile.stEvenItem = *pstEvenItem;
	
	//�ϳ�list�ļ���
	snprintf(listfile,sizeof(listfile),"%s/%s",pstrDirName,RECORD_LIST_FILENAME);
	
	//����б��ļ������ڣ��򴴽����ļ�
	if(access(listfile,R_OK|W_OK) != 0)
	{
		if(CreateListFile(listfile) != S_SUCCESS)
		{
			plog("err,CreateListFile, %s\n",listfile);
			return S_FAILURE;
		}
	}
	
	//��ȡ�ļ�ͷ	
	fp = fopen(listfile, "r+");
	if(fp == NULL)
	{
		plog("err,fopen, %s\n",listfile);
		return S_FAILURE;
	}
	
	int length = fread(&listfileheader, 1, sizeof(listfileheader), fp);
	if(length != (sizeof(listfileheader)))
	{
		plog("err,fread, %s\n",listfile);
		fclose(fp);
		return S_FAILURE; 
	}	
		
	switch(pstEvenItem->m_RecType)
	{
		case 0://*�ֶ�¼��
			listfileheader.m_maunalfilecnt = listfileheader.m_maunalfilecnt + 1;
			break;

		case 1://*��ʱ¼��
			listfileheader.m_timerfilecnt = listfileheader.m_timerfilecnt + 1;
			break;

		case 2://*����¼��
			listfileheader.m_alarmfilecnt = listfileheader.m_alarmfilecnt + 1;
			break;

		case 3://*�ƶ����¼��
			listfileheader.m_mdfilecnt = listfileheader.m_mdfilecnt + 1;
			break;
			
		default:
			break;
	}
	listfileheader.m_total_files = listfileheader.m_total_files+1;

	//д���ļ�ͷ
	fseek(fp, 0, SEEK_SET);	
	fwrite(&listfileheader,  1, sizeof(listfileheader), fp);

	//�ϳ��ļ���
	int s32Hdd,s32Part,s32Year,s32Month,s32Day;
	sscanf(pstrDirName,"/hdd%d/p%d/record/%d-%d-%d/",&s32Hdd,&s32Part,&s32Year,&s32Month,&s32Day);/*  /hdd00/p01/record/2015-01-05	*/
	if(s32Year < 2000)
	{
		ZD_DEBUG_Printf("%s(%d)->err,s32Year=%d",__FILE__,__LINE__,s32Year);
		return S_FAILURE;
	}
	s32Year -= 2000;
	
	char m_FrameRate,m_RecCh;
	if(pstEvenItem->m_FrameRate >= 10)
		m_FrameRate = (pstEvenItem->m_FrameRate - 10) + 'A';
	else
		m_FrameRate = (pstEvenItem->m_FrameRate - 0) + '0';
	if(pstEvenItem->m_RecCh >= 10)
		m_RecCh = (pstEvenItem->m_RecCh - 10) + 'A';
	else
		m_RecCh = (pstEvenItem->m_RecCh - 0) + '0';	
	
	snprintf(reclistfile.m_name,sizeof(reclistfile.m_name),
		"%s/recfile_-%02d%02d%02d-%02d%02d%02d-%02d%02d%02d-%1d%1d%1c%1d%1d%1c00.264",pstrDirName,s32Year,s32Month,s32Day,
			pstEvenItem->start_time/3600,0,0,
			pstEvenItem->start_time/3600,59,59,
			0,0,'0',0,0,m_RecCh);		

	//printf("%s(%d)->m_name=%s\n",__FILE__,__LINE__,reclistfile.m_name);
	
	//printf("mannual rec:%d ,timer rec:%d, alarmrec:%d, md rec : %d  \n",
	//	listfileheader.m_maunalfilecnt, listfileheader.m_timerfilecnt,
	//	listfileheader.m_alarmfilecnt, listfileheader.m_mdfilecnt);
	
	//ȥ��ͨ������ʼʱ��һ����������ʱ�䣬�ȵ�ǰʱ��Ľ���ʱ��С���¼�
	while(1)
	{
		int readRet = fread(&reclistfileDel, 1, sizeof(reclistfileDel), fp);
		if(readRet != sizeof(reclistfileDel))
		{
			//printf("%s %d %s fread end, readRet=%d!\n",__FILE__,__LINE__,__FUNCTION__,readRet);
			break;
		}	
				
		if(pstEvenItem->m_RecCh == reclistfileDel.stEvenItem.m_RecCh
			&& pstEvenItem->start_time == reclistfileDel.stEvenItem.start_time
			&& pstEvenItem->end_time >= reclistfileDel.stEvenItem.end_time
			&& reclistfileDel.m_state != ITEM_STATE_DELETED)
		{
			fseek(fp, -sizeof(reclistfileDel), SEEK_CUR);
			reclistfileDel.m_state = ITEM_STATE_DELETED;
			fwrite(&reclistfileDel, 1, sizeof(reclistfileDel), fp);
			//printf("%s(%d)->del,m_RecCh=%d,start_time=%d,end_time=%d\n",__FILE__,__LINE__,reclistfileDel.stEvenItem.m_RecCh,reclistfileDel.stEvenItem.start_time,reclistfileDel.stEvenItem.end_time);			
		}		
	}
	
	//д���¼�
	int writeRet = fwrite(&reclistfile, 1, sizeof(LISTFILE_NAME), fp);
	if(writeRet != sizeof(LISTFILE_NAME))
	{
		plog("%s %d %s fwrite error !\n",__FILE__,__LINE__,__FUNCTION__);
		fflush(fp);
		fclose(fp);
		return S_FAILURE;
	}

	fflush(fp);
	fclose(fp);
	
	return S_SUCCESS;
	
}

int RecordManage::CloseRecordFile(int ch)
{
	//�ر��ļ�
	if(m_pRecFp[ch] == NULL || m_index_fds[ch] == -1)
	{
		plog("err,CloseRecordFile,m_pRecFp or m_index_fds err,%d\n",ch);
		return S_FAILURE;
	}
	fclose(m_pRecFp[ch]);
	close(m_index_fds[ch]);
	plog("%s(%d)->close,%s\n",__FILE__,__LINE__,m_264FileName[ch]);

	//�ļ��Ľ���ʱ��
	if(GetRecordHour(&m_stEndTime[ch]) != 0)
		m_RecEndTime[ch] = GetCurrentTimeValue();
	else
		m_RecEndTime[ch] = m_stEndTime[ch].hour*3600 + m_stEndTime[ch].minute*60 + m_stEndTime[ch].second;

	//���¼��ʱ����̣�ɾ��264,idx�ļ�����ɾ����list�еļ�¼
#if 0	
	if(m_RecEndTime[ch] - m_RecStartTime[ch] < MIN_SYS_ALARM_DELAY)
	{
		unlink(m_264FileName[ch]);
		unlink(m_IdxFileName[ch]);
		ModifyItemInList(ch,NULL,NULL);
		ClearRecordInfor(ch);
		return S_SUCCESS;
	}
#endif

	//����h264ͷ��
	SystemDateTime stStart,stEnd;
	RecordTime2SysTime(m_RecStartTime[ch],&stStart);
	RecordTime2SysTime(m_RecEndTime[ch],&stEnd);	
	Fill264Header(ch, &stStart, &stEnd, m_offset[ch], 0, 30, 4,0);
	if(Rewrite264Header(ch) != S_SUCCESS)
	{
		ClearRecordInfor(ch);
		plog("err,Rewrite264Header,%d\n",ch);
		return S_FAILURE;			
	}	
		
	//׷��idx��264β��
#if 0		
	if(AppendIdx(ch) != S_SUCCESS)
	{
		ClearRecordInfor(ch);
		printf("err,AppendIdx,%d\n",ch);
		return S_FAILURE;			
	}	


	//ɾ��idx�ļ�
	//unlink(m_IdxFileName[ch]);

	//������264�ļ���ͬʱ����list�ļ�¼
	if(Rename264AndFixItem(ch,&stEnd) != S_SUCCESS)
	{
		ClearRecordInfor(ch);
		printf("err,Rename264AndFixItem,%d\n",ch);
		return S_FAILURE;			
	}		
#endif

	//�������¼�
	EVEN_ITEM stEvenItem[MAX_EVEN_NUM];
	int s32EvenNum = 0;	
	ParserEvenFromIframeIndex(m_IdxFileName[ch], stEvenItem,&s32EvenNum, ch);
	for(int i = 0; i < s32EvenNum; i++)
	{
		char dir[MAX_RECFILENAMELEN];
		memset(dir,0,sizeof(dir));
		memcpy(dir,m_264FileName[ch],28);// /hdd00/p01/record/2015-01-16
		AddOneEven2List(dir,&stEvenItem[i]);
	}	
	
	ClearRecordInfor(ch);	
	return S_SUCCESS;

}

void RecordManage::RecordDataProcess()
{
	int idle;
	while(s32DataThreadQuit == 0)
	{
		if(!CheckRecordTime())
		{
			sleep(1);
			continue;
		}
		idle = 1;
		for(int ch = 0; ch < MAX_VIDEO_CHANNEL; ch++)
		{
			if(m_uRecordCmd[ch] == RECORD_START 
				&& m_uRecordStatus[ch] == RECORD_STOPED)//���ļ�
			{
				OpenRecordFile(ch);
				m_uRecordStatus[ch] = RECORD_ING;
			}
			
			if((m_uRecordCmd[ch] == RECORD_START || m_uRecordCmd[ch] == RECORD_CONTINUE)
				&& m_uRecordStatus[ch] == RECORD_ING)//д��Ƶ֡���ļ�
			{
				if(WriteRecordFile(ch) == S_SUCCESS)	idle = 0;
			}
			
			if((m_uRecordCmd[ch] == RECORD_PALSE)
				&& m_uRecordStatus[ch] == RECORD_ING)//��ͣ¼��
			{
				m_uRecordStatus[ch] = RECORD_PALSEED;
				continue;
			}
			
			if((m_uRecordCmd[ch] == RECORD_CONTINUE)
				&& m_uRecordStatus[ch] == RECORD_PALSEED)//д��Ƶ֡���ļ�
			{
				if(WriteRecordFile(ch) == S_SUCCESS)	idle = 0;
				m_uRecordStatus[ch] = RECORD_ING;
			}
			
			if(m_uRecordCmd[ch] == RECORD_STOP
				&& (m_uRecordStatus[ch] == RECORD_ING || m_uRecordStatus[ch] == RECORD_PALSEED))//�ر��ļ�
			{
				CloseRecordFile(ch);
				m_uRecordStatus[ch] = RECORD_STOPED;
			}	
			
			if(m_uRecordCmd[ch] == RECORD_STOP
				&& m_uRecordStatus[ch] == RECORD_STOPED)//���ô���
			{
				continue;
			}			
		}
		
		if(idle)
		{
			usleep(100*1000);
		}
	}
}
