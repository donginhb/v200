#ifndef _FEED_DOG_H_
#define _FEED_DOG_H_

#include "MyList.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "Lock.h"
#include   <sys/time.h> 

struct FDDG_TDINFO
{
	int threadID;		/*�̵߳�ID*/
	int tdfood;		/*�̹߳��Ե�ʳ����*/
	int timenum;		/*�ڹ̶�ʱ����ڵĴ���*/
	int hungernum;	/*ĳ���̼߳����Ĵ�������̬��0*/
	int bDetect;		/*�Լ��û��*/
	int Interval;		/*ʱ������һ���߳�����תһȦ��ʱ��(����)*/
	unsigned long StartTime;	/*�߳�����ʱ��*/
	CLock	lock;
};

class CFeedDog
{
public:
	CFeedDog();
	~CFeedDog();
	static CFeedDog	*Instance();	
	bool	Add2List(int thdId);
	bool SubList(int thdId);
	bool FeedFood();
	bool EatFood(int thdId);	
	bool Start();
	void Stop();
	bool 	Arbitration()	;//�ٲ�
protected:
	//
private:
	//
	unsigned long long GetCurUsecTime();
	CMyList<FDDG_TDINFO> m_tdlist;
	static CFeedDog	*m_pInstance;		
	bool 	m_tdRuning;
	int 		m_thread_ID;	
};



#endif//_FEED_DOG_H_


